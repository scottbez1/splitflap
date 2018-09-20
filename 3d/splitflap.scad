/*
   Copyright 2015-2018 Scott Bezek and the splitflap contributors

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

include<28byj-48.scad>;
include<flap_dimensions.scad>;
include<m4_dimensions.scad>;
include<pcb.scad>;

// ##### RENDERING OPTIONS #####

render_3d = true;

// 3d parameters:
render_enclosure = 2; // 0=invisible; 1=translucent; 2=opaque color;
render_flaps = 2; // 0=invisible; 1=front flap only; 2=all flaps
render_flap_area = 0; // 0=invisible; 1=collapsed flap exclusion; 2=collapsed+extended flap exclusion
render_letters = "44";
render_units = len(render_letters);
render_unit_separation = 0;
render_pcb = true;
render_bolts = true;
render_motor = true;

// 2d parameters:
render_index = -1;
render_etch = false;

// Panelization:
panel_vertical = 0;
panel_horizontal = 0;

// Version label:
render_revision = "deadbeef";
render_date = "YYYY-MM-DD";

spool_explosion = 0; // 0-1
spool_strut_explosion = lookup(spool_explosion, [
    [0, 0],
    [0.2, 0],
    [1, 30],
]);
spool_horizontal_explosion = lookup(spool_explosion, [
    [0, 0],
    [1, 8],
]);


// Kerf based off http://blog.ponoko.com/2011/07/12/figuring-out-kerf-for-precision-parts/
// It's better to underestimate (looser fit) than overestimate (no fit)
kerf_width = 0.19 - 0.05;

// MDF, .125in nominal
// http://www.ponoko.com/make-and-sell/show-material/64-mdf-natural
thickness = 3.2;

eps=.01;

captive_nut_inset=6;

assembly_inner_radius = m4_hole_diameter/2;

assembly_color = [.76, .60, .42];
assembly_color1 = [.882, .694, .486]; //"e1b17c";
assembly_color2 = [.682, .537, .376]; //"ae8960";
assembly_color3 = [.416, .325, .227]; //"6A533A";
assembly_color4 = [.204, .161, .114]; //"34291D";

flap_rendered_angle = 90;

letter_height = flap_height * 0.75 * 2;

// Amount of slop of the flap side to side between the 2 spools
flap_width_slop = 0.1;

// Amount of slop for the spool assembly side-to-side inside the enclosure
spool_width_slop = 1;


num_flaps = 40;

flap_hole_radius = (flap_pin_width + 1) / 2;
flap_gap = 1;

flap_spool_outset = flap_hole_radius;
flap_pitch_radius = flap_spool_pitch_radius(num_flaps, flap_hole_radius, flap_gap); //num_flaps * (flap_hole_radius*2 + flap_gap) / (2*PI);
spool_outer_radius = flap_spool_outer_radius(num_flaps, flap_hole_radius, flap_gap, flap_spool_outset); //flap_pitch_radius + 2*flap_hole_radius;

// Radius where flaps are expected to flap in their *most collapsed* (90 degree) state
exclusion_radius = sqrt(flap_height*flap_height + flap_pitch_radius*flap_pitch_radius);
// Radius where flaps are expected to flap in their *most extended* state
outer_exclusion_radius = flap_pitch_radius + flap_height + 2;

front_forward_offset = flap_pitch_radius + flap_thickness/2;

flap_notch = sqrt(spool_outer_radius*spool_outer_radius - flap_pitch_radius*flap_pitch_radius);
flap_notch_depth = 3;


spool_width = flap_width - flap_notch_depth*2 + flap_width_slop + thickness*2;
assert(spool_width >= flap_width, "Flap is wider than spool!");
spool_strut_tabs = 3;
spool_strut_tab_width=8;
spool_strut_tab_outset=8;
spool_strut_width = (spool_strut_tab_outset + thickness/2) * 2;
spool_strut_length_inset = 0;
spool_strut_length = spool_width - (2 * spool_strut_length_inset);
spool_strut_inner_length = spool_width - 3 * thickness;

spool_strut_exclusion_radius = sqrt((spool_strut_tab_outset+thickness/2)*(spool_strut_tab_outset+thickness/2) + (spool_strut_tab_width/2)*(spool_strut_tab_width/2));


magnet_hole_radius = (4 - 0.1)/2;
magnet_hole_offset = (spool_strut_exclusion_radius + flap_pitch_radius)/2;

28byj48_chassis_height_slop = 1;

motor_shaft_under_radius = 0.1;
motor_slop_radius = 3;


// Width measured from the outside of the walls
enclosure_wall_to_wall_width = thickness + spool_width_slop/2 + spool_width + spool_width_slop/2 + max(28byj48_mount_bracket_height + m4_button_head_length, 4 + 28byj48_mount_bracket_height - spool_width_slop/2) + thickness;

// Width of the front panel
enclosure_width = enclosure_wall_to_wall_width + 28byj48_chassis_height + 28byj48_chassis_height_slop - thickness - 28byj48_mount_bracket_height;
enclosure_horizontal_inset = (enclosure_width - enclosure_wall_to_wall_width)/2;
front_window_upper_base = (flap_height - flap_pin_width/2);
front_window_overhang = 3;
front_window_upper = front_window_upper_base - front_window_overhang;
front_window_lower = sqrt(outer_exclusion_radius*outer_exclusion_radius - front_forward_offset*front_forward_offset);
front_window_slop = 0;
front_window_width = spool_width_slop + spool_width + front_window_slop;
front_window_right_inset = thickness - front_window_slop/2;
enclosure_vertical_margin = 10; // gap between top/bottom of flaps and top/bottom of enclosure
enclosure_vertical_inset = thickness*1.5; // distance from top of sides to top of the top piece
enclosure_height_upper = exclusion_radius + enclosure_vertical_margin + thickness + enclosure_vertical_inset;
enclosure_height_lower = flap_pitch_radius + flap_height + enclosure_vertical_margin + thickness + enclosure_vertical_inset;
enclosure_height = enclosure_height_upper + enclosure_height_lower;

enclosure_horizontal_rear_margin = thickness; // minumum distance between the farthest feature and the rear

enclosure_length = front_forward_offset + 28byj48_mount_center_offset + m4_hole_diameter/2 + enclosure_horizontal_rear_margin;


// Enclosure tabs: front/back
num_front_tabs = 2;
front_tab_width = (enclosure_wall_to_wall_width - 2*thickness) / (num_front_tabs*2 - 1);

enclosure_length_right = front_forward_offset + m4_hole_diameter/2 + 2;

side_tab_width = enclosure_length_right / 4;
side_tab_width_fraction = 0.5;


backstop_bolt_vertical_offset = - (exclusion_radius + outer_exclusion_radius)/2;
backstop_bolt_forward_range = 14;

motor_mount_hole_radius = m4_hole_diameter/2;
motor_backpack_extent = 28byj48_backpack_extent + 2; // Add 2mm to make sure there's room for the wires
motor_hole_slop = 1;

connector_bracket_length_outer = 14;
connector_bracket_length_inner = side_tab_width * 2 - m4_button_head_diameter/2;
connector_bracket_thickness = captive_nut_inset - thickness - 0.2;
connector_bracket_width = enclosure_width - enclosure_wall_to_wall_width + thickness*2 + connector_bracket_thickness*2;
connector_bracket_overlap = 4;


mounting_hole_inset = m4_button_head_diameter/2 + 2;

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
echo(flap_notch=flap_notch);


module standard_m4_bolt(nut_distance=-1) {
    if (render_bolts) {
        roughM4_7380(10);
        if (nut_distance >= 0) {
            color([0.70, 0.70, 0.72]) {
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


// ##### Struts for bracing spool #####
module spool_strut_tab_hole() {
    square([thickness, spool_strut_tab_width], center=true);
}
module spool_strut_tab_holes() {
    for (i=[0:3]) {
        angle = 90*i;
        translate([cos(angle)*spool_strut_tab_outset, sin(angle)*spool_strut_tab_outset])
            rotate([0,0,angle])
                spool_strut_tab_hole();
    }
}
module spool_strut() {
    joint_tab_width = spool_strut_inner_length / spool_strut_tabs;
    linear_extrude(thickness, center=true) {
        union() {
            translate([spool_strut_length_inset, -spool_strut_tab_width / 2]) {
                square([spool_strut_length, spool_strut_tab_width]);
            }
            translate([thickness*2, -spool_strut_width / 2]) {
                difference() {
                    square([spool_strut_inner_length, spool_strut_width]);

                    // subtract out tabs
                    union() {
                        for (i = [0:2:spool_strut_tabs-1]) {
                            translate([i*joint_tab_width, -eps])
                                square([joint_tab_width, thickness+eps]);
                        }
                        for (i = [1:2:spool_strut_tabs-1]) {
                            translate([i*joint_tab_width, spool_strut_width - thickness])
                                square([joint_tab_width, thickness+eps]);
                        }
                    }
                }
            }
        }
    }
}

module spool_struts() {
    for (i=[0:3]) {
        angle = 90*i;
        //color([i < 2 ? 0 : 1, i == 0 || i == 2 ? 0 : 1, 0])
        color(i % 2 == 0 ? assembly_color2 : assembly_color3)
        translate([0, sin(angle)*(spool_strut_tab_outset + spool_strut_explosion), cos(angle)*(spool_strut_tab_outset + spool_strut_explosion)])
            rotate([-angle, 0, 0])
                spool_strut();
    }
}


module flap_spool_complete(captive_nut=false, motor_shaft=false, magnet_hole=false) {
    linear_extrude(thickness) {
        difference() {
            flap_spool(num_flaps, flap_hole_radius, flap_gap, flap_spool_outset,
                    height=0);

            spool_strut_tab_holes();
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

module spool_retaining_wall(m4_bolt_hole=false) {
    linear_extrude(thickness) {
        difference() {
            square([spool_strut_width, spool_strut_width], center=true);
            spool_strut_tab_holes();
            if (m4_bolt_hole) {
                circle(r=m4_hole_diameter/2, $fn=30);
            }
        }
    }
}


module flap() {
    color([1, 1, 1])
    translate([0, -flap_pin_width/2, -flap_thickness/2])
    linear_extrude(height=flap_thickness) {
        difference() {
            union() {
                square([flap_width, flap_height - flap_corner_radius + eps]);

                // rounded corners
                hull() {
                    translate([flap_corner_radius, flap_height - flap_corner_radius])
                        circle(r=flap_corner_radius, $fn=40);
                    translate([flap_width - flap_corner_radius, flap_height - flap_corner_radius])
                        circle(r=flap_corner_radius, $fn=40);
                }
            }
            translate([-eps, flap_pin_width])
                square([eps + flap_notch_depth, flap_notch]);
            translate([flap_width - flap_notch_depth, flap_pin_width])
                square([eps + flap_notch_depth, flap_notch]);
        }
    }
}

module translated_flap() {
    translate([0, flap_pitch_radius, 0]) {
        rotate([flap_rendered_angle, 0, 0]) {
            flap();
        }
    }
}



// double-flatted motor shaft of 28byj-48 motor (2D)
module motor_shaft() {
    union() {
        intersection() {
            circle(r=28byj48_shaft_radius-motor_shaft_under_radius, $fn=50);
            square([28byj48_shaft_radius*2, 3], center=true);
        }
        square([28byj48_shaft_radius/3, 28byj48_shaft_radius*4], center=true);
    }
}

module front_tabs_negative() {
    for (i = [0 : num_front_tabs-1]) {
        translate([thickness + (i*2+0.5) * front_tab_width, 0, 0])
            square([front_tab_width, thickness], center=true);
    }
    for (i = [0 : num_front_tabs-2]) {
        translate([thickness + (i*2+1.5) * front_tab_width, 0, 0])
            circle(r=m4_hole_diameter/2, $fn=30);
    }
}

module connector_bracket() {
    linear_extrude(height=thickness) {
        difference() {
            square([connector_bracket_width, connector_bracket_length_outer]);
            translate([connector_bracket_thickness, -eps]) {
                square([connector_bracket_width - connector_bracket_thickness*2, connector_bracket_length_outer - connector_bracket_length_inner + eps]);
            }
            translate([connector_bracket_thickness, -eps]) {
                square([thickness, connector_bracket_length_outer - connector_bracket_overlap + eps]);
            }
            translate([connector_bracket_width - connector_bracket_thickness - thickness, -eps]) {
                square([thickness, connector_bracket_length_outer - connector_bracket_overlap + eps]);
            }
        }
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

// holes for 28byj-48 motor, centered around motor shaft
module motor_mount() {
    circle(r=28byj48_shaft_radius+motor_slop_radius, center=true, $fn=30);
    translate([-28byj48_mount_center_offset, -8]) {
        circle(r=motor_mount_hole_radius, center=true, $fn=30);
    }
    translate([28byj48_mount_center_offset, -8]) {
        circle(r=motor_mount_hole_radius, center=true, $fn=30);
    }
    translate([-28byj48_chassis_radius - motor_hole_slop/2, -28byj48_shaft_offset - motor_backpack_extent - motor_hole_slop/2, 0]) {
        square([28byj48_chassis_radius*2 + motor_hole_slop, 28byj48_chassis_radius + motor_backpack_extent + motor_hole_slop]);
    }
}

module side_tabs_negative(hole_sizes=[], extend_last_tab=false) {
    for (i = [0 : len(hole_sizes)]) {
        length = (extend_last_tab && i == len(hole_sizes)) ? side_tab_width * side_tab_width_fraction + eps : side_tab_width * side_tab_width_fraction;
        translate([-thickness / 2, thickness + (i*2) * side_tab_width + side_tab_width * (1 - side_tab_width_fraction)/2, 0])
            square([thickness, length]);
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
    translate([enclosure_vertical_inset - thickness, -connector_bracket_overlap]) {
        square([thickness, connector_bracket_overlap + eps]);
    }
}

module enclosure_left() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length]);
            translate([enclosure_height_lower, enclosure_length - front_forward_offset, 0])
                circle(r=m4_hole_diameter/2, center=true, $fn=30);

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
        }
    }
}

module shaft_centered_motor_hole() {
    margin = 5;
    width = 28byj48_mount_center_offset*2 + 3.5*2 + margin*2;
    length = 18 + 14 + margin*2;

    translate([-width/2, -(margin + 18 + 8)])
        square([width, length]);
}

module enclosure_right() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length_right]);
            translate([enclosure_height_upper, enclosure_length_right - front_forward_offset, 0])
                circle(r=m4_hole_diameter/2, center=true, $fn=30);

            // backstop bolt slot
            translate([enclosure_height_upper - backstop_bolt_vertical_offset, enclosure_length_right - front_forward_offset, 0]) {
                backstop_bolt_slot(radius = m4_hole_diameter/2);
            }

            // top side tabs
            translate([0.5*thickness + enclosure_vertical_inset, enclosure_length_right, 0])
                mirror([0, 1, 0])
                    side_tabs_negative(hole_sizes=[1]);

            // bottom side tabs
            translate([enclosure_height - 0.5*thickness - enclosure_vertical_inset, enclosure_length_right, 0])
                mirror([0, 1, 0])
                    side_tabs_negative(hole_sizes=[1]);

            // Connector bracket cuts
            translate([enclosure_height, enclosure_length_right]) {
                mirror([1, 0, 0]) {
                    connector_bracket_side_holes();
                }
            }
            translate([0, enclosure_length_right]) {
                connector_bracket_side_holes();
            }
        }
    }
}

module front_back_tabs() {
    for (i = [0 : 2 : num_front_tabs*2-2]) {
        translate([i * front_tab_width, -eps, 0])
            square([front_tab_width, thickness + eps]);
    }
}

module side_tabs(tabs) {
    for (i = [0 : 2 : tabs*2-2]) {
        translate([-eps, i * side_tab_width + side_tab_width * (1 - side_tab_width_fraction)/2, 0])
            square([thickness + eps, side_tab_width * side_tab_width_fraction]);
    }
}

module front_back_captive_nuts() {
    for (i = [0 : 2 : num_front_tabs-1]) {
        translate([(i*2 + 1.5) * front_tab_width, -thickness, 0])
            m4_captive_nut();
    }
}

module side_captive_nuts(hole_types=[]) {
    for (i = [0 : len(hole_types)-1]) {
        hole_type = hole_types[i];
        translate([-thickness, (i*2 + 1.5) * side_tab_width, 0]) {
            rotate([0, 0, -90]) {
                if (hole_type == 2) {
                } else if (hole_type == 1) {
                    m4_captive_nut();
                }
            }
        }
    }
}


module enclosure_top() {
    // note, this is flipped upside down (around the x axis) when assembled so the clean side faces out
    linear_extrude(height = thickness) {
        translate([thickness, 0, 0]) {
            difference() {
                union() {
                    square([enclosure_wall_to_wall_width - 2 * thickness, enclosure_length_right]);

                    // front tabs
                    mirror([0, 1, 0])
                        front_back_tabs();

                    // left tabs
                    translate([enclosure_wall_to_wall_width - 2 * thickness, thickness, 0])
                        side_tabs(2);

                    // right tabs
                    mirror([1, 0, 0])
                        translate([0, thickness, 0])
                            side_tabs(2);
                }

                // front captive nuts
                front_back_captive_nuts();

                // right captive nuts
                translate([0, thickness, 0])
                    side_captive_nuts(hole_types = [1]);

                // left captive nuts
                translate([enclosure_wall_to_wall_width - 2 * thickness, thickness, 0])
                    mirror([1, 0, 0])
                        side_captive_nuts(hole_types = [1]);

                // mounting hole
                translate([(enclosure_wall_to_wall_width - 2 * thickness)/2, enclosure_length_right - mounting_hole_inset]) {
                    circle(r=m4_hole_diameter/2, $fn=30);
                }
            }
        }
    }
}

module enclosure_bottom() {
    linear_extrude(height = thickness) {
        translate([thickness, 0, 0]) {
            difference() {
                union() {
                    square([enclosure_wall_to_wall_width - 2 * thickness, enclosure_length_right]);

                    // front tabs
                    translate([0, enclosure_length_right, 0])
                        front_back_tabs();

                    // left tabs
                    translate([enclosure_wall_to_wall_width - 2 * thickness, enclosure_length_right - thickness, 0])
                        mirror([0, 1, 0])
                            side_tabs(2);

                    // right tabs
                    translate([0, enclosure_length_right - thickness, 0])
                        mirror([0, 1, 0])
                            mirror([1, 0, 0])
                                side_tabs(2);
                }

                // front captive nuts
                translate([0, enclosure_length_right, 0])
                    mirror([0,1,0])
                        front_back_captive_nuts();

                // right captive nuts
                translate([0, enclosure_length_right - thickness, 0])
                    mirror([0, 1, 0])
                        side_captive_nuts(hole_types = [1]);

                // left captive nuts
                translate([enclosure_wall_to_wall_width - 2 * thickness, enclosure_length_right - thickness, 0])
                    mirror([0, 1, 0])
                        mirror([1, 0, 0])
                            side_captive_nuts(hole_types = [1]);

                // mounting hole
                translate([(enclosure_wall_to_wall_width - 2 * thickness)/2, mounting_hole_inset]) {
                    circle(r=m4_hole_diameter/2, $fn=30);
                }
            }
        }
    }
}

module enclosure_bottom_etch() {
    color([0, 0, 0])
    linear_extrude(height=2, center=true) {
        translate([captive_nut_inset + m4_nut_length + 1, 1, thickness]) {
            text_label([str("rev. ", render_revision), render_date, "github.com/scottbez1/splitflap"]);
        }
    }
}

module split_flap_3d(letter, include_connector) {
    module positioned_front() {
        translate([0, front_forward_offset + thickness, -enclosure_height_lower])
            rotate([90, 0, 0])
                enclosure_front();
    }

    module positioned_left() {
        translate([enclosure_wall_to_wall_width, -enclosure_length + front_forward_offset, -enclosure_height_lower])
            rotate([0, -90, 0])
                enclosure_left();
    }

    module positioned_right() {
        translate([0, -enclosure_length_right + front_forward_offset, enclosure_height_upper])
            rotate([0, 90, 0])
                enclosure_right();
    }

    module positioned_top() {
        translate([0, front_forward_offset, enclosure_height_upper - enclosure_vertical_inset])
            rotate([180, 0, 0])
                enclosure_top();
    }

    module positioned_bottom() {
        translate([0, front_forward_offset - enclosure_length_right, -enclosure_height_lower + enclosure_vertical_inset]) {
            enclosure_bottom();
        }
    }

    module positioned_bottom_etch() {
        translate([0, front_forward_offset - enclosure_length_right, -enclosure_height_lower + enclosure_vertical_inset]) {
            translate([0, 0, thickness]) {
                enclosure_bottom_etch();
            }
        }
    }

    module positioned_top_connector() {
        translate([enclosure_wall_to_wall_width - thickness - connector_bracket_thickness, front_forward_offset - connector_bracket_length_outer, enclosure_height_upper - enclosure_vertical_inset]) {
            connector_bracket();
        }
    }

    module positioned_bottom_connector() {
        translate([enclosure_wall_to_wall_width - thickness - connector_bracket_thickness, front_forward_offset - connector_bracket_length_outer, - enclosure_height_lower + enclosure_vertical_inset - thickness]) {
            connector_bracket();
        }
    }

    module positioned_left_bolts() {
        // Top
        translate([enclosure_wall_to_wall_width, front_forward_offset - side_tab_width * 2, enclosure_height_upper - enclosure_vertical_inset - thickness/2]) {
            rotate([0, -90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }

        // Bottom
        translate([enclosure_wall_to_wall_width, front_forward_offset - side_tab_width * 2, -enclosure_height_lower + enclosure_vertical_inset + thickness/2]) {
            rotate([0, -90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }
    }

    module positioned_right_bolts() {
        // Top
        translate([0, front_forward_offset - side_tab_width * 2, enclosure_height_upper - enclosure_vertical_inset - thickness/2]) {
            rotate([0, 90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }

        // Bottom
        translate([0, front_forward_offset - side_tab_width * 2, -enclosure_height_lower + enclosure_vertical_inset + thickness/2]) {
            rotate([0, 90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }
    }

    module positioned_front_bolts() {
        // Top
        translate([enclosure_wall_to_wall_width/2, front_forward_offset + thickness, enclosure_height_upper - enclosure_vertical_inset - thickness/2]) {
            rotate([0, 90, -90]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }

        // Bottom
        translate([enclosure_wall_to_wall_width/2, front_forward_offset + thickness, -enclosure_height_lower + enclosure_vertical_inset + thickness/2]) {
            rotate([0, 90, -90]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }
    }

    module positioned_enclosure() {
        if (render_enclosure == 2) {
            color(assembly_color1)
                positioned_front();
            color(assembly_color2)
                positioned_left();
            color(assembly_color2)
                positioned_right();
            color(assembly_color3)
                positioned_top();
            color(assembly_color3)
                positioned_bottom();
            positioned_bottom_etch();
            if (include_connector) {
                color(assembly_color4)
                    positioned_top_connector();
                color(assembly_color4)
                    positioned_bottom_connector();
            }
            if (render_bolts) {
                positioned_left_bolts();
                positioned_right_bolts();
                positioned_front_bolts();
            }
        } else if (render_enclosure == 1) {
            %positioned_front();
            %positioned_left();
            %positioned_right();
            %positioned_top();
            %positioned_bottom();
            %positioned_bottom_etch();
            if (include_connector) {
                %positioned_top_connector();
                %positioned_bottom_connector();
            }
            if (render_bolts) {
                positioned_left_bolts();
                positioned_right_bolts();
                positioned_front_bolts();
            }
        }
    }

    positioned_enclosure();
    if (render_pcb) {
        translate([enclosure_wall_to_wall_width + eps, -pcb_hole_to_sensor_x, -magnet_hole_offset - pcb_hole_to_sensor_y]) {
            rotate([0, 90, 0]) {
                rotate([0, 0, 90]) {
                    pcb();
                    translate([0, 0, -thickness - 2 * eps]) {
                        standard_m4_bolt(nut_distance=thickness + pcb_thickness + 4*eps);
                    }
                }
            }
        }
    }

    module letter_top_half() {
        rotate([-90, 0, 0]) {
            rotate([0, 0, 180]) {
                linear_extrude(height=0.1, center=true) {
                    translate([-flap_width / 2, -flap_pin_width/2]) {
                        difference() {
                            text(text=letter, size=letter_height, font="RobotoCondensed", halign="center", valign="center");
                            translate([-flap_width, -flap_height - eps]) {
                                square([2 * flap_width, flap_height]);
                            }
                        }
                    }
                }
            }
        }
    }

    module letter_bottom_half() {
        rotate([-90, 0, 0]) {
            rotate([0, 0, 180]) {
                linear_extrude(height=0.1, center=true) {
                    translate([-flap_width / 2, flap_pin_width/2]) {
                        difference() {
                            text(text=letter, size=letter_height, font="RobotoCondensed", halign="center", valign="center");
                            translate([-flap_width, eps]) {
                                square([2 * flap_width, flap_height]);
                            }
                        }
                    }
                }
            }
        }
    }

    translate([spool_width_slop/2 + thickness, 0, 0]) {
        // Flap area
        if (render_flaps > 0) {
            rotate([0, 90, 0]) {
                if (render_flap_area >= 1) {
                    translate([0, 0, thickness]) {
                        cylinder(r=exclusion_radius, h=flap_width - 2*thickness);
                    }
                }
                if (render_flap_area >= 2) {
                    translate([0, 0, thickness + (flap_width - 2*thickness)/4]) {
                        cylinder(r=outer_exclusion_radius, h=(flap_width - 2*thickness)/2);
                    }
                }
            }

            translate([-flap_notch_depth + thickness + flap_width_slop/2, 0, 0]) {
                // Collapsed flaps on the top
                for (i=[0:num_flaps/2 - 1]) {
                    if (i == 0 || render_flaps == 2) {
                        rotate([360/num_flaps * i, 0, 0]) translated_flap();
                    }
                    if (i == 0) {
                        color([0,0,0]) {
                            translate([0, flap_pitch_radius + flap_thickness/2, 0]) {
                                letter_top_half();
                            }
                        }
                    }
                }

                for (i=[1:num_flaps/2]) {
                    angle = -360/num_flaps*i;
                    translate([0, flap_pitch_radius*cos(angle), flap_pitch_radius * sin(angle)]) {
                        if (i == 1 || render_flaps == 2) {
                            rotate([-90, 0, 0]) {
                                flap();
                            }
                        }
                        if (i == 1) {
                            color([0,0,0]) {
                                translate([0, flap_thickness/2, 0]) {
                                    letter_bottom_half();
                                }
                            }
                        }
                    }
                }
            }
        }

        spool_struts();

        // motor spool
        color(assembly_color) {
            translate([spool_width - thickness + 5*spool_horizontal_explosion, 0, 0]) {
                rotate([0, 90, 0]) {
                    flap_spool_complete(motor_shaft=true, magnet_hole=true);
                }
            }
        }
        color(assembly_color1) {
            translate([thickness - 3*spool_horizontal_explosion, 0, 0]) {
                rotate([0, 90, 0]) {
                    spool_retaining_wall(m4_bolt_hole=true);
                }
            }
        }
        color(assembly_color) {
            translate([-5*spool_horizontal_explosion, 0, 0]) {
                rotate([0, 90, 0]) {
                    flap_spool_complete(captive_nut=true);
                }
            }
        }
        translate([thickness * 2, 0, 0]) {
            rotate([0, -90, 0]) {
                standard_m4_bolt(nut_distance=thickness + 7*spool_horizontal_explosion);
            }
        }
    }

    if (render_motor) {
        translate([enclosure_wall_to_wall_width - thickness - 28byj48_mount_bracket_height, 0, 0]) {

            rotate([-90, 0, 0]) {

                rotate([0, -90, 0]) {
                    Stepper28BYJ48();
                }
                translate([0, -28byj48_shaft_offset, 0]) {
                    translate([0, 0, -28byj48_mount_center_offset]) {
                        rotate([0, 90, 0]) {
                            rotate([0, 0, 90]) {
                                standard_m4_bolt(nut_distance=thickness+28byj48_mount_bracket_height);
                            }
                        }
                    }
                    translate([0, 0, 28byj48_mount_center_offset]) {
                        rotate([0, 90, 0]) {
                            rotate([0, 0, 90]) {
                                standard_m4_bolt(nut_distance=thickness+28byj48_mount_bracket_height);
                            }
                        }
                    }
                }
            }
        }
    }
}

module laser_etch() {
    if (render_etch || render_index == -1) {
        children();
    }
}

if (render_3d) {
    for (i = [0 : render_units - 1]) {
        translate([-enclosure_width/2 + (-(render_units-1) / 2 + i)*(enclosure_width + render_unit_separation), 0, 0])
            split_flap_3d(render_letters[render_units - 1 - i], include_connector=(i != render_units - 1));
    }
} else {
    sp = 5;
    panel_height = enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width + kerf_width + spool_strut_width + kerf_width;
    projection_renderer(render_index=render_index, render_etch=render_etch, kerf_width=kerf_width, panel_height=panel_height, panel_horizontal=panel_horizontal, panel_vertical=panel_vertical) {
        translate([0, 0])
            enclosure_left();
        translate([0, enclosure_length + kerf_width])
            enclosure_right();
        translate([0, enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width - enclosure_horizontal_inset])
            rotate([0, 0, -90])
                enclosure_front();

        translate([enclosure_height + kerf_width + enclosure_length_right, enclosure_length + kerf_width + enclosure_length_right + kerf_width + 2])
            rotate([0, 0, 90])
            enclosure_top();

        translate([enclosure_height + kerf_width, enclosure_length + kerf_width + enclosure_length_right + 2])
            rotate([0, 0, -90])
                enclosure_bottom();

        laser_etch()
            translate([enclosure_height + kerf_width, enclosure_length + kerf_width + enclosure_length_right + 2, thickness])
                rotate([0, 0, -90])
                    enclosure_bottom_etch();

        // Spool struts cut out of right side
        translate([thickness*2 + 5, enclosure_length + kerf_width + enclosure_length_right - spool_strut_width/2 - 3, thickness])
            spool_strut();

        // Spool struts at the top
        spool_strut_y_offset = enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width + kerf_width + spool_strut_width/2;
        translate([spool_strut_length, spool_strut_y_offset, thickness/2])
            rotate([0, 0, 180])
                spool_strut();
        translate([spool_strut_length*2 + kerf_width, spool_strut_y_offset, thickness/2])
            rotate([0, 0, 180])
                spool_strut();
        translate([spool_strut_length*3 + kerf_width*2, spool_strut_y_offset, thickness/2])
            rotate([0, 0, 180])
                spool_strut();

        // Connector brackets cut out of left side
        translate([12, (enclosure_length - connector_bracket_length_outer*2)/2, thickness])
            connector_bracket();
        translate([12 + connector_bracket_width, (enclosure_length - connector_bracket_length_outer*2)/2 + 2 * connector_bracket_length_outer + kerf_width, thickness])
            rotate([0, 0, 180])
                connector_bracket();

        // Flap spools in window
        flap_spool_y_off = enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width - front_window_right_inset - enclosure_horizontal_inset - front_window_width/2;
        flap_spool_x_off = spool_outer_radius + enclosure_height_lower - front_window_lower + kerf_width + 2;
        translate([flap_spool_x_off, flap_spool_y_off])
            flap_spool_complete(motor_shaft=true, magnet_hole=true);
        translate([flap_spool_x_off + spool_outer_radius*2 + 2, flap_spool_y_off])
            flap_spool_complete(captive_nut=true);

        translate([enclosure_height_lower + 28byj48_shaft_offset - 28byj48_chassis_radius - motor_hole_slop/2 + spool_strut_width/2 + kerf_width, enclosure_length - front_forward_offset - 28byj48_chassis_radius - motor_hole_slop/2 + spool_strut_width/2 + kerf_width])
            spool_retaining_wall(m4_bolt_hole=true);
    }
}
