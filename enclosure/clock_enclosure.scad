use <dotSCAD/arc.scad>
use <dotSCAD/util/radians.scad>
use <dotSCAD/util/degrees.scad>
include <common.scad>

use <dotSCAD/bend.scad>

$fn = 128;


// Enclosure
enclosure_display = "F"; // [F:Full, T:Top Only, W:Without top]

module Base() {
  difference() {
    linear_extrude(bottom)
      circle(d = base_diam);
  }
}

module BaseStandoffs() {
  difference() {
    translate([0, 0, bottom])
      linear_extrude(bottom_standoff_height)
        ScrewHoles(standoff_diam);

    BottomScrewHoles();
  }
}

module BottomScrewHoles() {
  linear_extrude(10)
    ScrewHoles(screw_outer_diam);

  linear_extrude(screw_head_height)
    ScrewHoles(screw_head_diam);
}

module Body() {
  linear_extrude(total_height)
    arc(
      radius = main_radius,
      angle = [90 - angle / 2, 90 + angle / 2],
      width = thickness,
      width_mode = "LINE_OUTWARD"
    );
}

module CableCutOut() {
  coutout_height = 5;
  coutout_width = 12;

  translate([-coutout_width / 2,  2, bottom])
    cube([coutout_width, thickness, coutout_height]);
}

module FrontCutout() {
  translate([0, 0, bottom + margin/2])
    linear_extrude(paper_visable_h)
      arc(
        radius = main_radius,
        angle = [90 - front_cutout_angle / 2, 90 + front_cutout_angle / 2],
        width = paper_setback,
        width_mode = "LINE_OUTWARD"
      );
}

module Paper() {
  translate([0, 0, bottom])
    linear_extrude(paper_h)
      arc(
        radius = paper_radius,
        angle = [90 - paper_angle / 2, 90 + paper_angle / 2],
        width = paper_thickness,
        width_mode = "LINE_OUTWARD"
      );
}

module ResistorCutout() {
    translate([0, -206/radians(angle) +resistor_thickness+paper_setback + paper_thickness, 3])
    rotate([0, 0, 90 - angle/2])
    bend(size = [206, 106, resistor_thickness], angle = angle)
    linear_extrude(2)
    import("clock_face.svg"); 
}

module ScrewInserts(diam, height, z) {
  translate([0, thickness / 2 + paper_setback, z])
    rotate([0, 0, 10.5])
      translate([0, main_radius, 0])
        linear_extrude(height)
          circle(d = diam);

  translate([0, thickness / 2 + paper_setback, z])
    rotate([0, 0, -10.5])
      translate([0, main_radius, 0])
        linear_extrude(height)
          circle(d = diam);
}

module TopRetainingClip(diam){
    translate([0, -main_radius+ thickness / 2 + paper_setback, total_height-margin+1.2])
    rotate([0, 0, (90 - 20/2)])
    rotate_extrude(angle=20) translate([main_radius,0,0]) circle(d=diam);

    translate([0, -main_radius+ thickness / 2 + paper_setback, total_height-margin+1.2])
    rotate([0, 0, 78]) // 78
    rotate_extrude(angle=1) translate([main_radius,0,0]) circle(d=diam); // 1

    translate([0, -main_radius+ thickness / 2 + paper_setback, total_height-margin+1.2])
    rotate([0, 0, 102]) // 102
    rotate_extrude(angle=-1) translate([main_radius,0,0]) circle(d=diam); // -1
}

module Enclosure() {
    difference() {
      union() {
        translate([0, -main_radius, 0])
          Body();
        Base();
      }
      CableCutOut();

      translate([0, -main_radius, 0])
      union() {
        ScrewInserts(standoff_hole_diam, margin + 4, total_height -margin - 4);
        ScrewInserts(screw_head_diam, screw_head_height, total_height - 1);
      }

      translate([0, -main_radius, 0])      
      union() {
        Paper();
        FrontCutout();
      }

      ResistorCutout();
      BottomScrewHoles();
    }
    
  BaseStandoffs();
}

module EnclosureWithoutTop() {
  cube_w = paper_w + margin * 2;

  difference() {
    Enclosure();
    translate([-cube_w/2, -base_diam / 2, total_height - margin])
      cube([cube_w, base_diam, margin]);
  }
  TopRetainingClip(3);  
}

module EnclosureTop() {
  cube_w = paper_w + margin * 2;
  cube_d = base_diam;
  cube_z = total_height - margin;
  difference() {
    Enclosure();
    TopRetainingClip(3.6);

    translate([-cube_w/2, -cube_d/2, 0])
    cube([cube_w, cube_d, cube_z]);  
  }
}

if (enclosure_display == "F") Enclosure();
if (enclosure_display == "T") EnclosureTop();
if (enclosure_display == "W") EnclosureWithoutTop();