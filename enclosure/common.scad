use <dotSCAD/util/radians.scad>
use <dotSCAD/util/degrees.scad>

angle = 24;
paper_w = 207.5;
paper_h = 106.5;
margin = 4;
bottom = 3;
thickness = 10;
paper_thickness = 1;
height = paper_h + bottom + margin * 2;
paper_setback = 1;

base_diam = 100;

paper_margin = 2;
paper_visable_w = paper_w - paper_margin * 2 ;
paper_visable_h = paper_h - paper_margin * 2;

main_radius = (paper_visable_w + margin * 2) / radians(angle);
paper_angle = degrees(paper_w / (main_radius + paper_setback));
paper_radius = main_radius + paper_setback;
front_cutout_angle = degrees(paper_visable_w / main_radius);

total_height = bottom + paper_h + margin/2;

resistor_thickness = 2;

// back cover
wall_thickness = 2.15;
outer_height = 12.5;
inner_height = outer_height - wall_thickness;

screw_outer_diam = 2.7;
screw_head_diam = 5.25;
screw_head_height = 1.5;

standoff_hole_diam = 3.2;
standoff_diam = 6.75;
screw_insert_height = 4;

pcb_thickness = 0.8;

distance_from_bottom = 3.5;
bottom_standoff_height = distance_from_bottom;
top_standoff_height = inner_height - bottom_standoff_height - pcb_thickness - 0.75;

inner_radius = base_diam / 2 - wall_thickness;
pcb_reduction = 0.25;
pcb_radius = inner_radius - pcb_reduction;

pcb_button_height = 4.3;

module ScrewHoles(d) {
    screw_angle = 60;
    screw_dist = pcb_radius - 10;
    screw_diam = d;

    translate([0, -2, 0])
        for (i = [[-55, 0], [0, 1], [55, 0]]) {
            rotate([0, 0, i[0]])
                translate([0, screw_dist - i[1], 0])
                    circle(d = screw_diam);
        }
}

function opposite(angle, hyp) = sin(angle) * hyp;
function adjacent(angle, hyp) = cos(angle) * hyp;
function pythag_b(hyp, a) = sqrt(pow(hyp, 2) - pow(a, 2));
