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

include<flap.scad>;
include<spool.scad>;

// Enclosure tabs: front/back
enclosure_tab_clearance = 0.10;

enclosure_left_zip_side_inset = 5.0;  // inset from left for the bottom zip tie holes, edge to outside edge
enclosure_left_zip_bottom_inset = 22.5;  // inset from bottom for the bottom zip tie holes, edge to group center

enclosure_left_zip_top_inset = 22.5;  // inset from top for the top zip tie holes, edge to group center

// Radius where flaps are expected to flap in their *most collapsed* (90 degree) state
exclusion_radius = sqrt(flap_height*flap_height + flap_pitch_radius*flap_pitch_radius);
// Radius where flaps are expected to flap in their *most extended* state
outer_exclusion_radius = flap_pitch_radius + flap_height + 2;
front_forward_offset = flap_pitch_radius + flap_thickness/2;

// Width measured from the outside of the walls
enclosure_wall_to_wall_width = thickness + spool_width_slop/2 + spool_width_clearance + spool_width_slop/2 + max(28byj48_mount_bracket_height + m4_button_head_length, 4 + 28byj48_mount_bracket_height - spool_width_slop/2) + thickness;

// Width of the front panel
enclosure_width = enclosure_wall_to_wall_width + 28byj48_chassis_height + 28byj48_chassis_height_clearance - thickness - 28byj48_mount_bracket_height;
enclosure_horizontal_inset = (enclosure_width - enclosure_wall_to_wall_width)/2;
front_window_upper_base = (flap_height - flap_pin_width/2);
front_window_overhang = 3;
front_window_upper = front_window_upper_base - front_window_overhang;
front_window_lower = sqrt(outer_exclusion_radius*outer_exclusion_radius - front_forward_offset*front_forward_offset);
front_window_slop = 0;
front_window_width = spool_width_slop + spool_width_clearance + front_window_slop;
front_window_right_inset = thickness - front_window_slop/2;


enclosure_vertical_margin = 10; // gap between top/bottom of flaps and top/bottom of enclosure
enclosure_vertical_inset = max(thickness*1.5, m4_nut_width_corners_padded/2); // distance from top of sides to top of the top piece
enclosure_height_upper = exclusion_radius + enclosure_vertical_margin + thickness + enclosure_vertical_inset;
enclosure_height_lower = flap_pitch_radius + flap_height + enclosure_vertical_margin + thickness + enclosure_vertical_inset;
enclosure_height = enclosure_height_upper + enclosure_height_lower;

enclosure_horizontal_rear_margin = thickness; // minumum distance between the farthest feature and the rear
enclosure_length = front_forward_offset + 28byj48_mount_center_offset + m4_hole_diameter/2 + enclosure_horizontal_rear_margin;
enclosure_length_right = front_forward_offset + m4_hole_diameter/2 + 2;

enclosure_indicator_inset = 3.0;  // inset on both X and Y
enclosure_indicator_size = 1.75;  // symbol size
enclosure_indicator_arrow_width = 2.25;
enclosure_indicator_arrow_height = enclosure_indicator_arrow_width * 2;
enclosure_indicator_position_y = (enclosure_height - enclosure_vertical_inset - thickness) - enclosure_indicator_inset;

backstop_bolt_vertical_offset = - (exclusion_radius + outer_exclusion_radius)/2;
backstop_bolt_forward_range = 14;

mounting_hole_inset = m4_button_head_diameter/2 + 2;

side_tab_width = enclosure_length_right / 4;
side_tab_width_fraction = 0.5;
captive_nut_inset=6;
num_front_tabs = 2;
front_tab_width = (enclosure_wall_to_wall_width - 2*thickness) / (num_front_tabs*2 - 1);


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

module enclosure_right() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length_right]);
            translate([enclosure_height_upper, enclosure_length_right - front_forward_offset, 0])
                circle(r=m4_hole_diameter/2, $fn=30);

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

module enclosure_right_etch() {
    // alignment indicator (circle)
    enclosure_etch_style()
        translate([enclosure_height - enclosure_indicator_position_y, enclosure_length_right - enclosure_indicator_inset])
            circle(r=enclosure_indicator_size/2, $fn=60);
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
    enclosure_etch_style()
        translate([captive_nut_inset + m4_nut_length + 1, 1, thickness]) {
            text_label([str("rev. ", render_revision), render_date, "github.com/scottbez1/splitflap"]);
        }
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

