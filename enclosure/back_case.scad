use <dotSCAD/rounded_square.scad>
include <common.scad>

$fn = 128;

button_diam = 4.2;
button_x = 20.5 / 2;
button_y = pcb_radius - 6.5;
button_thickness = 1;

light_pipe_outer_diam = 4;
light_pipe_inner_diam = 2.1;
light_pipe_coord_x = 19.8;
light_pipe_coord_y = 27.88 + 10.4;
led_height = 1;

module Pcb() {
  translate([0, 0, inner_height - top_standoff_height - pcb_thickness])
    linear_extrude(pcb_thickness)
      difference() {
        circle(pcb_radius);
        translate([0, -main_radius, 0])
        circle(main_radius + thickness + pcb_reduction);
        ScrewHoles(screw_outer_diam);
      }
}

module Inner() {
  difference() {
    linear_extrude(inner_height)
    circle(inner_radius);

    translate([0, 0, inner_height -top_standoff_height ])
    linear_extrude(top_standoff_height)
    translate([0, -main_radius, 0])
    circle(main_radius + thickness + 1);
  }
}

module Outer() {
  linear_extrude(outer_height)
    difference() {
      circle(d = base_diam);
      translate([0, -main_radius, 0])
        circle(main_radius + thickness);
    }
}

module UpperSandoffs() {
  difference() {
  translate([0, 0, inner_height - top_standoff_height])
    linear_extrude(top_standoff_height)
      ScrewHoles(standoff_diam);

  translate([0, 0, inner_height - top_standoff_height])
    linear_extrude(screw_insert_height)
      ScrewHoles(standoff_hole_diam);
  }
}

module LightPipe() {
  difference() {
    translate([light_pipe_coord_x, light_pipe_coord_y, inner_height - top_standoff_height + led_height])
      linear_extrude(top_standoff_height - led_height)
        circle(d=light_pipe_outer_diam);

    translate([light_pipe_coord_x, light_pipe_coord_y, inner_height - top_standoff_height + led_height])
      linear_extrude(top_standoff_height - led_height)
        circle(d=light_pipe_inner_diam);
  }
}

module LightPipeCutout() {
  translate([light_pipe_coord_x, light_pipe_coord_y, 0])
    linear_extrude(outer_height)
      circle(d=light_pipe_inner_diam);
}

module BackCover() {
  difference() {
    Outer();
    Inner();
    CableCutout();
    ButtonCutout();
    UsbCutout();
    LightPipeCutout();
    TempSensorCutout();
  }
  UpperSandoffs();
  LightPipe();
}

module CableCutout() {
  length = 12;
  height = 2;
  translate([-length / 2, 5, inner_height - top_standoff_height])
    cube([length, 10, height]);
}

module ButtonCutout() {
  translate([button_x, button_y, 0])
    linear_extrude(outer_height + 1)
      circle(d = button_diam);

  translate([-button_x, button_y, 0])
    linear_extrude(outer_height + 1)
      circle(d = button_diam);
}

module ButtonStandoff() {
  translate([button_x, button_y, 0])
    linear_extrude(outer_height + 2)
      circle(d = button_diam);

  translate([-button_x, button_y, 0])
    linear_extrude(outer_height + 2)
      circle(d = button_diam);
}

module UsbCutout() {
  usb_w = 3.78;
  usb_h = 9.44;
  translate([0, 52, -usb_w / 2 + inner_height - top_standoff_height + usb_w])
    rotate([90, 0, 0])
      linear_extrude(5)
        rounded_square([usb_h, usb_w], corner_r = 1.5, center = true);
}

module TempSensorCutout() {
  translate([adjacent(49, 45), opposite(49, 45), inner_height - top_standoff_height + pcb_thickness])
  rotate([-49, 90, 0])
  linear_extrude(5)
  rounded_square([2, 5], corner_r=0.5, center=true);
}

//Pcb();
BackCover();
