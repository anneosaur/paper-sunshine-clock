#include <DS3231.h>
#include <DS18B20.h>
#include <avr/wdt.h>

#define VA 5
#define VB 9
#define VC 10
#define VD 6

#define GB1 A6
#define GT1 24

#define GB2 8
#define GT2 20

#define GB3 7
#define GT3 21

#define LED A2

#define TIME_ADJ_BTN 3
#define MODE_ADJ_BTN 2

#define DELAY 1000

#define SPARKLE_RECENT_COUNT 20

#define MODE_CLOCK 0
#define MODE_SPARKLE 1
#define MODE_TIME_SET 2
#define MODE_TIME_RESET 3

DS3231 myRTC;
DS18B20 ds(A1);

int voltageSupply[4] = { VA, VB, VC, VD };

int ground[3][2] = {
  { GB1, GT1 },
  { GB2, GT2 },
  { GB3, GT3 },
};

bool h12Flag;
bool pmFlag;

int lastSegment;

volatile int mode = MODE_CLOCK;
volatile bool modeSwitch = false;

volatile bool incrementTimeTriggered = false;
volatile bool decrementTimeTriggered = false;
unsigned long lastTimeSetTouch = 0;

unsigned long lastTimeResetTouch = 0;
volatile bool timeResetTriggered = false;
int timeResetBtnCount = 0;

unsigned long lastTempUpdate = 0;
float lastTemp;


int sparkleRecent[SPARKLE_RECENT_COUNT] = {};

void setup() {

  wdt_enable(WDTO_8S);
  Wire.begin();

  pinMode(LED, OUTPUT);

  pinMode(VA, OUTPUT);
  pinMode(VB, OUTPUT);
  pinMode(VC, OUTPUT);
  pinMode(VD, OUTPUT);

  pinMode(GB1, OUTPUT);
  pinMode(GT1, OUTPUT);

  pinMode(GB2, OUTPUT);
  pinMode(GT2, OUTPUT);

  pinMode(GB3, OUTPUT);
  pinMode(GT3, OUTPUT);

  pinMode(TIME_ADJ_BTN, INPUT);
  pinMode(MODE_ADJ_BTN, INPUT);

  reset();

  myRTC.setClockMode(true);

  attachInterrupt(digitalPinToInterrupt(TIME_ADJ_BTN), interruptTimeSetMode, RISING);
  attachInterrupt(digitalPinToInterrupt(MODE_ADJ_BTN), interruptSparkleMode, RISING);

  ds.selectNext();

  lastSegment = getSegment();

  digitalWrite(LED, HIGH);
  clockStartup();
  digitalWrite(LED, LOW);
}

void interruptSparkleMode() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 500) {
    if (mode == MODE_CLOCK) {
      mode = MODE_SPARKLE;
      digitalWrite(LED, HIGH);
    } else if (mode == MODE_SPARKLE) {
      mode = MODE_CLOCK;
      modeSwitch = true;
      digitalWrite(LED, LOW);
    } else if (mode == MODE_TIME_SET) {
      incrementTimeTriggered = true;
      lastTimeSetTouch = millis();
    }
  }
  last_interrupt_time = interrupt_time;
}

void interruptTimeSetMode() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 500) {
    if (mode == MODE_CLOCK) {
      digitalWrite(LED, HIGH);
      mode = MODE_TIME_SET;
      modeSwitch = true;
    } else if (mode == MODE_TIME_SET) {
      decrementTimeTriggered = true;
    } else if (mode == MODE_SPARKLE) {
      mode = MODE_TIME_RESET;
      timeResetTriggered = true;
      modeSwitch = true;
    } else if (mode == MODE_TIME_RESET) {
      timeResetTriggered = true;
    }
  }
  last_interrupt_time = interrupt_time;
}

void clockSequence(int holdDelay, int segment, int pwm, bool returnEarly = false) {
  for (int i = 0; i < 4; i++) {
    for (int s = 0; s <= segment; s++) {
      if (int(floor((s % 8) / 2)) == i) {
        turnOn(s, pwm);
        if (returnEarly && segment == s) {
          _delay(holdDelay);
          reset();
          return;
        };
      }
    }
    _delay(holdDelay);
    reset();
  }
}

void clockStartup() {
  int segment = getSegment();
  int startupDelay = getStartupDelay();
  int stopSegment;
  bool returnEarly = false;

  stopSegment = (segment < 7) ? segment : 7;
  if (stopSegment <= 7) returnEarly = true;
  clockSequence(startupDelay, stopSegment, 0, returnEarly);

  if (segment > 7) {
    stopSegment = (segment < 15) ? segment : 15;
    if (stopSegment <= 15) returnEarly = true;
    clockSequence(startupDelay, stopSegment, 0, returnEarly);
  }

  if (segment > 15) {
    stopSegment = (segment < 23) ? segment : 23;
    if (stopSegment <= 23) returnEarly = true;
    clockSequence(startupDelay, stopSegment, 0, returnEarly);
  }
}

void sparkleAddToRecent(int s) {
  for (int i = 0; i < SPARKLE_RECENT_COUNT - 1; i++) {
    sparkleRecent[i] = sparkleRecent[i + 1];
  }
  sparkleRecent[SPARKLE_RECENT_COUNT - 1] = s;
}

int getPwm() {
  float temp = getAdjustedTemp();
  int pwm = (8 * temp) - 445;
  return pwm;
}

int getStartupDelay() {
  float temp = getTemp();
  int startupDelay = (((-0.42 * temp) + 37) * 1000) + 2000;
  startupDelay = startupDelay > 10000 ? 10000 : startupDelay;
  return startupDelay;
}

int getSegment() {
  float hour = float(myRTC.getHour(h12Flag, pmFlag));
  int minute = myRTC.getMinute();

  if (hour == 12) hour = 0;
  if (minute >= 30) hour += 0.5;
  int segment = int(hour * 2);
  return segment;
}

float getAdjustedTemp() {
  float boardTemperatureF = getTemp();
  float offset = (0.3 * lastSegment / 2) + 2;
  float adjustedTemp = boardTemperatureF - offset;

  return adjustedTemp;
}

float getTemp() {
  if (lastTempUpdate == 0 || millis() - lastTempUpdate > 60000) {
    float boardTemperatureF = ds.getTempF();
    if (boardTemperatureF < 60) boardTemperatureF = 60;

    lastTempUpdate = millis();
    lastTemp = boardTemperatureF;
    return boardTemperatureF;
  }
  return lastTemp;
}

void incrementTime() {
  int currentHour = myRTC.getHour(h12Flag, pmFlag);
  if (currentHour == 12) currentHour = 0;
  int newHour = currentHour;
  int currentMinute = myRTC.getMinute();

  int newMinute = currentMinute + 30;
  if (newMinute >= 60) {
    newMinute -= 60;
    newHour += 1;
    if (newHour == 12) newHour = 0;
  }

  myRTC.setHour(newHour);
  myRTC.setMinute(newMinute);

  incrementTimeTriggered = false;
}

void decrementTime() {
  int currentHour = myRTC.getHour(h12Flag, pmFlag);
  if (currentHour == 12) currentHour = 0;
  int newHour = currentHour;
  int currentMinute = myRTC.getMinute();

  int newMinute = currentMinute - 30;
  if (newMinute <= 0) {
    newMinute += 60;
    newHour -= 1;
    if (newHour < 0) newHour = 11;
  }

  myRTC.setHour(newHour);
  myRTC.setMinute(newMinute);

  decrementTimeTriggered = false;
}

bool sparkleIsRecent(int s) {
  for (int i = 0; i < SPARKLE_RECENT_COUNT; i++) {
    if (sparkleRecent[i] == s) return true;
  };

  return false;
}

void reset() {
  digitalWrite(GB1, LOW);
  digitalWrite(GT1, LOW);

  digitalWrite(GB2, LOW);
  digitalWrite(GT2, LOW);

  digitalWrite(GB3, LOW);
  digitalWrite(GT3, LOW);

  digitalWrite(VA, HIGH);
  digitalWrite(VB, HIGH);
  digitalWrite(VC, HIGH);
  digitalWrite(VD, HIGH);
}

void turnOn(int num, int pwm) {
  if (num < 0 || num > 23) reset();

  int h = voltageSupply[int(floor((num % 8) / 2))];

  int group = int(floor(num / 8));
  int g = ground[group][num % 2];

  analogWrite(h, pwm);
  digitalWrite(g, HIGH);
}

void _delay(int delayCount) {
  for (int c = 0; c < delayCount; c += 100) {
    if (incrementTimeTriggered || decrementTimeTriggered || modeSwitch) return;
    delay(100);
    wdt_reset();
  }
}

// Modes
void modeClock() {
  int segment = getSegment();

  if ((segment == 0 && lastSegment >= 1) || segment > lastSegment) {
    reset();
    turnOn(segment, 0);
    lastSegment = segment;

    int additionalDelay = 0;
    if (segment % 2 == 1) additionalDelay = 3000;
    _delay(getStartupDelay() + additionalDelay);
    reset();

    if (mode == MODE_CLOCK) {
      digitalWrite(LED, HIGH);
      clockSequence(2000, segment - 1, 0);
      digitalWrite(LED, LOW);
    }

  } else if (segment < lastSegment) {
    lastSegment = segment;
  }

  int pwm = getPwm();

  clockSequence(DELAY, segment, pwm);
}

void modeSparkle() {
  int segment = rand() % (23 + 1);
  if (sparkleIsRecent(segment)) return;

  turnOn(segment, 0);
  sparkleAddToRecent(segment);

  if (segment % 2 == 1) {
    turnOn(segment - 1, 0);
    sparkleAddToRecent(segment - 1);
  };

  int sparkleDelay = getStartupDelay() - 3000;
  _delay(sparkleDelay);
  reset();

  _delay(10000 - sparkleDelay);
}

void timeSetBlink() {
  digitalWrite(LED, LOW);
  delay(250);
  digitalWrite(LED, HIGH);
  delay(250);
  digitalWrite(LED, LOW);
  delay(250);
  digitalWrite(LED, HIGH);
}

void modeTimeSet() {
  if (lastTimeSetTouch == 0) {
    lastTimeSetTouch = millis();
  }
  if (millis() - lastTimeSetTouch > 5000) {
    digitalWrite(LED, LOW);
    lastTimeSetTouch = 0;
    mode = MODE_CLOCK;
  }

  if (incrementTimeTriggered) {
    incrementTime();
    lastTimeSetTouch = millis();
    timeSetBlink();
  } else if (decrementTimeTriggered) {
    decrementTime();
    lastTimeSetTouch = millis();
    timeSetBlink();
  }

  modeClock();
}

void modeTimeReset() {
  if (lastTimeResetTouch == 0) {
    lastTimeResetTouch = millis();
  }

  if (millis() - lastTimeResetTouch > 5000) {
    digitalWrite(LED, LOW);
    lastTimeResetTouch = 0;
    timeResetTriggered = false;
    timeResetBtnCount = 0;
    mode = MODE_CLOCK;
  }

  if (timeResetTriggered) {
    if (timeResetBtnCount == 9) {
      timeResetBtnCount = 0;
      lastTimeResetTouch = 0;
      myRTC.setHour(0);
      myRTC.setMinute(0);
      myRTC.setSecond(0);
      mode = MODE_CLOCK;
      digitalWrite(LED, LOW);
    } else {
      timeResetBtnCount += 1;
      lastTimeResetTouch = millis();
      timeResetTriggered = false;
      timeSetBlink();
    }
  }
  wdt_reset();
  delay(25);
}

void loop() {
  if (mode == MODE_CLOCK) {
    if (modeSwitch) {
      modeSwitch = false;
      clockStartup();
    }
    modeClock();
  } else if (mode == MODE_SPARKLE) {
    modeSwitch = false;
    modeSparkle();
  } else if (mode == MODE_TIME_SET) {
    modeSwitch = false;
    modeTimeSet();
  } else if (mode == MODE_TIME_RESET) {
    modeSwitch = false;
    modeTimeReset();
  }
}