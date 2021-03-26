/*
   Copyright 2015-2020 Scott Bezek and the splitflap contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

use<assert.scad>;
use<label.scad>;
use<projection_renderer.scad>;
use<roboto/RobotoCondensed-Regular.ttf>;
use<rough7380.scad>;
use<spool.scad>;
use<shapes.scad>;

include<28byj-48.scad>;
include<flap_fonts.scad>;
include<pcb.scad>;
include<common.scad>;
include<splitflap_2d.scad>;
include<splitflap_3d.scad>;
include<parts/flap.scad>;
include<parts/spool.scad>;
include<parts/enclosure.scad>;
include<parts/hardware.scad>;
include<parts/connector.scad>;

// ##### RENDERING OPTIONS #####

render_3d = true;

// 3d parameters:
render_enclosure = 2; // 0=invisible; 1=translucent; 2=opaque color;
render_flaps = 2; // 0=invisible; 1=front flap only; 2=all flaps
render_flap_area = 0; // 0=invisible; 1=collapsed flap exclusion; 2=collapsed+extended flap exclusion
render_letters = "44";
render_units = len(render_letters);
render_unit_separation = 0;
render_spool = true;
render_pcb = true;
render_sensor_jig = false;
render_bolts = true;
render_motor = true;

// 2d parameters:
render_index = -1;
render_etch = false;
render_2d_mirror = false;

// Panelization:
panel_vertical = 0;
panel_horizontal = 0;

// Version label:
render_revision = "deadbeef";
render_date = "YYYY-MM-DD";

// Ponoko kerf values are 0.2 mm for MDF and acrylic (all thicknesses)
// Remember: it's better to underestimate (looser fit) than overestimate (no fit)
kerf_width = 0.2 - 0.02;

num_flaps = 40;

function get_flap_gap() = flap_gap;  // for exposing this value when this file is 'used' and not 'included' in other files

spool_outer_radius = flap_spool_outer_radius(num_flaps, flap_hole_radius, flap_hole_separation, flap_spool_outset); //flap_pitch_radius + 2*flap_hole_radius;

//legacyAssert(spool_width >= flap_width, "Flap is wider than spool!");

// distance from the outside spool face to the inside of the left enclosure
pcb_to_spool = enclosure_wall_to_wall_width - front_window_width - thickness + spool_width_slop/2;

// 'get' functions to extract these values for when this file is 'used' and not 'included'
function connector_bracket_length() = connector_bracket_length_outer;
function connector_bracket_width() = connector_bracket_width;

mounting_hole_inset = m4_button_head_diameter/2 + 2;

enclosure_indicator_inset = 3.0;  // inset on both X and Y
enclosure_indicator_size = 1.75;  // symbol size
enclosure_indicator_arrow_width = 2.25;
enclosure_indicator_arrow_height = enclosure_indicator_arrow_width * 2;
enclosure_indicator_position_y = (enclosure_height - enclosure_vertical_inset - thickness) - enclosure_indicator_inset;

zip_tie_height = 3.0;  // height of the zip-tie hole
zip_tie_width = 2.0;  // width of the zip-tie holes
zip_tie_spacing = 6.5;  // spacing between each zip-tie hole, inside edges
zip_tie_fillet = 0.5;  // radius of the rounded zip-tie hole corners

enclosure_left_zip_side_inset = 5.0;  // inset from left for the bottom zip tie holes, edge to outside edge
enclosure_left_zip_bottom_inset = 22.5;  // inset from bottom for the bottom zip tie holes, edge to group center

enclosure_left_zip_top_inset = 22.5;  // inset from top for the top zip tie holes, edge to group center

alignment_bar_diameter = 6.35;  // 1/4"
alignment_bar_clearance = 0.25;
alignment_bar_fillet_radius = 1.25;

alignment_bar_cutout_width = alignment_bar_diameter + (2 * alignment_bar_clearance);
alignment_bar_center = (enclosure_length - enclosure_length_right) - alignment_bar_cutout_width/2;


echo(kerf_width=kerf_width);
echo(enclosure_height=enclosure_height);
echo(enclosure_height_upper=enclosure_height_upper);
echo(enclosure_height_lower=enclosure_height_lower);
echo(enclosure_width=enclosure_width);
echo(enclosure_wall_to_wall_width=enclosure_wall_to_wall_width);
echo(enclosure_length=enclosure_length);
echo(enclosure_length_right=enclosure_length_right);
echo(enclosure_length_real=enclosure_length+thickness);
echo(spool_strut_inner_length=spool_strut_inner_length);
echo(front_window_width=front_window_width);
echo(front_window_upper=front_window_upper);
echo(front_window_lower=front_window_lower);
echo(front_window_height=front_window_lower+front_window_upper);
echo(front_forward_offset=front_forward_offset);
echo(flap_exclusion_radius=exclusion_radius);
echo(flap_hole_radius=flap_hole_radius);
echo(flap_notch_height=flap_notch_height);
echo(pcb_to_sensor=pcb_to_sensor(pcb_to_spool));


module standard_m4_bolt(nut_distance=-1) {
    if (render_bolts) {
        color(bolt_color)
            roughM4_7380(10);
        if (nut_distance >= 0) {
            color(nut_color) {
                translate([0, 0, nut_distance]) {
                    linear_extrude(m4_nut_length) {
                        difference() {
                            circle(r=m4_nut_width_corners/2, $fn=6);
                            circle(r=m4_hole_diameter/2, $fn=20);
                        }
                    }
                }
            }
        }
    }
}


// ##### CAPTIVE NUT NEGATIVE #####

// Centered in the x dimension
module captive_nut(bolt_diameter, bolt_length, nut_width, nut_length, nut_inset) {
    union() {
        translate([-bolt_diameter/2, 0, 0])
            square([bolt_diameter, bolt_length]);
        translate([-nut_width/2, nut_inset, 0])
            square([nut_width, nut_length]);
    }
}
module m4_captive_nut(bolt_length=m4_bolt_length) {
    captive_nut(m4_hole_diameter, bolt_length + 1, m4_nut_width_flats, m4_nut_length_padded, captive_nut_inset);
}


module zip_tie_holes() {
    spacing = (zip_tie_spacing + zip_tie_width)/2;

    translate([-spacing, 0, 0])
        rounded_square([zip_tie_width, zip_tie_height], center=true, r=zip_tie_fillet, $fn=30);
    translate([spacing, 0, 0])
        rounded_square([zip_tie_width, zip_tie_height], center=true, r=zip_tie_fillet, $fn=30);
}


module flap_spool_complete(captive_nut=false, motor_shaft=false, magnet_hole=false) {
    linear_extrude(thickness, convexity=10) {  // 'convexity' to fix rendering errors with etch 'difference()' result
        difference() {
            flap_spool(num_flaps, flap_hole_radius, flap_hole_separation, flap_spool_outset,
                    height=0);

            spool_strut_tab_holes(narrow=captive_nut);
            if (captive_nut) {
                circle(r=m4_nut_width_corners_padded/2, $fn=6);
            }
            if (motor_shaft) {
                rotate([0, 0, 90]) {
                    motor_shaft();
                }
            }
            if (magnet_hole) {
                // Hole for press fit magnet
                translate([magnet_hole_offset, 0]) {
                    circle(r=magnet_hole_radius, $fn=15);
                }
            }
        }
    }
}

module flap_spool_etch() {
    enclosure_etch_style()
        flap_spool_home_indicator(num_flaps, flap_hole_radius, flap_hole_separation, flap_spool_outset);
}




module front_tabs_negative() {
    for (i = [0 : num_front_tabs-1]) {
        translate([thickness + (i*2+0.5) * front_tab_width, 0, 0])
            square([front_tab_width + enclosure_tab_clearance, thickness + enclosure_tab_clearance], center=true);
    }
    for (i = [0 : num_front_tabs-2]) {
        translate([thickness + (i*2+1.5) * front_tab_width, 0, 0])
            circle(r=m4_hole_diameter/2, $fn=30);
    }
}

module connector_bracket_2d() {
    difference() {
        square([connector_bracket_width, connector_bracket_length_outer]);
        translate([connector_bracket_thickness, -eps]) {
            square([connector_bracket_width - connector_bracket_thickness*2, connector_bracket_length_outer - connector_bracket_length_inner + eps]);
        }
        translate([connector_bracket_thickness - connector_bracket_clearance/2, -eps]) {
            square([thickness + connector_bracket_clearance, connector_bracket_length_outer - connector_bracket_overlap + connector_bracket_depth_clearance + eps]);
        }
        translate([connector_bracket_width - connector_bracket_thickness - thickness - connector_bracket_clearance/2, -eps]) {
            square([thickness + connector_bracket_clearance, connector_bracket_length_outer - connector_bracket_overlap + connector_bracket_depth_clearance + eps]);
        }
    }
}

module connector_bracket() {
    linear_extrude(height=thickness) {
        connector_bracket_2d();
    }
}


module enclosure_front() {
    linear_extrude(height=thickness) {
        difference() {
            translate([-enclosure_horizontal_inset, 0, 0]) {
                square([enclosure_width, enclosure_height]);
            }

            // Viewing window cutout
            translate([front_window_right_inset, enclosure_height_lower - front_window_lower])
                square([front_window_width, front_window_lower + front_window_upper]);

            // Front lower tabs
            translate([0, thickness * 0.5 + enclosure_vertical_inset, 0])
                front_tabs_negative();

            // Front upper tabs
            translate([0, enclosure_height - thickness * 0.5 - enclosure_vertical_inset, 0])
                front_tabs_negative();
        }
    }
}

module enclosure_front_etch() {
    // alignment indicator, left side (triangle)
    enclosure_etch_style()
        translate([enclosure_wall_to_wall_width - thickness - enclosure_indicator_inset, enclosure_indicator_position_y])
            triangle(enclosure_indicator_size, center=true);

    // alignment indicator, right side (circle)
    enclosure_etch_style()
        translate([thickness + enclosure_indicator_inset, enclosure_indicator_position_y])
            circle(r=enclosure_indicator_size/2, $fn=60);

    // position indicator, 'up' arrow
    enclosure_etch_style()
        translate([enclosure_width - enclosure_horizontal_inset * 1.5, enclosure_height - enclosure_indicator_arrow_height/2 - enclosure_indicator_inset])
            arrow([enclosure_indicator_arrow_width, enclosure_indicator_arrow_height], center=true);
}



module side_tabs_negative(hole_sizes=[], extend_last_tab=false) {
    for (i = [0 : len(hole_sizes)]) {
        length = (extend_last_tab && i == len(hole_sizes)) ? side_tab_width * side_tab_width_fraction + eps : side_tab_width * side_tab_width_fraction;
        translate([0,  thickness + (i*2) * side_tab_width + side_tab_width * (1 - side_tab_width_fraction)/2 + length/2, 0])
            square([thickness + enclosure_tab_clearance, length + enclosure_tab_clearance], center=true);
    }
    for (i = [0 : len(hole_sizes) - 1]) {
        hole_size = hole_sizes[i];
        if (hole_size > 0) {
            bolt_head_hole = hole_size == 2;
            translate([0, thickness + (i*2 + 1.5) * side_tab_width, 0])
                circle(r=(bolt_head_hole ? m4_button_head_diameter : m4_hole_diameter)/2, $fn=30);
        }
    }
}

module backstop_bolt_slot(radius) {
    hull() {
        circle(r=radius, $fn=15);
        translate([0, backstop_bolt_forward_range]) {
            circle(r=radius, $fn=15);
        }
    }
}

module connector_bracket_side_holes() {
    // overlap slot
    translate([enclosure_vertical_inset - thickness - connector_bracket_clearance/2, -connector_bracket_overlap]) {
        square([thickness + connector_bracket_clearance, connector_bracket_overlap + eps]);
    }
}

module alignment_bar() {
    color(assembly_color1)
        translate([enclosure_width - enclosure_horizontal_inset, -enclosure_length_right + front_forward_offset - alignment_bar_diameter/2, -enclosure_height_lower + alignment_bar_diameter/2])
            rotate([0, -90, 0])
                linear_extrude(height=enclosure_width * len(render_letters))
                    circle(r=alignment_bar_diameter/2, $fn=60);
}

module enclosure_left() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length]);
            translate([enclosure_height_lower, enclosure_length - front_forward_offset, 0])
                circle(r=m4_hole_diameter/2, $fn=30);

            translate([enclosure_height_lower, enclosure_length - front_forward_offset]) {
                rotate([0, 0, 90]) {
                    motor_mount();
                }
            }

            // bottom side tabs
            translate([thickness * 0.5 + enclosure_vertical_inset, enclosure_length, 0])
                mirror([0, 1, 0])
                side_tabs_negative(hole_sizes=[1]);

            // top side tabs
            translate([enclosure_height - thickness * 0.5 - enclosure_vertical_inset, enclosure_length, 0])
                mirror([0, 1, 0])
                    side_tabs_negative(hole_sizes=[1]);

            // Connector bracket cuts
            translate([enclosure_height, enclosure_length]) {
                mirror([1, 0, 0]) {
                    connector_bracket_side_holes();
                }
            }
            translate([0, enclosure_length]) {
                connector_bracket_side_holes();
            }


            // PCB mounting holes
            translate([enclosure_height_lower - magnet_hole_offset - pcb_hole_to_sensor_y, enclosure_length - front_forward_offset - pcb_hole_to_sensor_x]) {
                rotate([180, 0, 0]) {
                    rotate([0, 0, -90]) {
                        pcb_cutouts();
                    }
                }
            }

            // Zip tie holes, sensor (leading to bottom)
            translate([enclosure_left_zip_bottom_inset, zip_tie_height/2 + enclosure_left_zip_side_inset, 0])
                zip_tie_holes();

            // Zip tie holes, motor (leading to top)
            translate([enclosure_height - enclosure_left_zip_top_inset, enclosure_length - front_forward_offset])
                rotate([0, 0, 90])  // cable channel facing 'up'
                    zip_tie_holes();

            // Alignment bar cutout
            translate([0, alignment_bar_center]) {
                union() {
                    // Cutout
                    translate([alignment_bar_cutout_width/2, 0])
                        circle(r=alignment_bar_cutout_width/2, $fn=40);
                    square([alignment_bar_cutout_width, alignment_bar_cutout_width], center=true);

                    // Front-side fillet
                    // translate([0, alignment_bar_cutout_width/2, 0])
                    //     fillet_tool(r=alignment_bar_fillet_radius, overlap=1, $fn=40);

                    // Back-side fillet
                    translate([0, -alignment_bar_cutout_width/2, 0])
                        mirror([0, 1, 0])
                            fillet_tool(r=alignment_bar_fillet_radius, $fn=40);
                }
            }
        }
    }
}

module enclosure_left_etch() {
    // alignment indicator (triangle)
    enclosure_etch_style()
        translate([enclosure_indicator_position_y, enclosure_length - enclosure_indicator_inset])
            rotate([0, 0, -90])
                triangle(enclosure_indicator_size, center=true);
}

module shaft_centered_motor_hole() {
    margin = 5;
    width = 28byj48_mount_center_offset*2 + 3.5*2 + margin*2;
    length = 18 + 14 + margin*2;

    translate([-width/2, -(margin + 18 + 8)])
        square([width, length]);
}

if (render_3d) {
    for (i = [0 : render_units - 1]) {
        translate([-enclosure_width/2 + (-(render_units-1) / 2 + i)*(enclosure_width + render_unit_separation), 0, 0])
            split_flap_3d(render_letters[render_units - 1 - i], include_connector=(i != render_units - 1));
    }
} else {
    split_flap_2d();
}
