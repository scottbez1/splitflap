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

flap_rendered_angle = 90;

module split_flap_3d(letter, include_connector) {
    module position_front() {
        translate([0, front_forward_offset + thickness, -enclosure_height_lower])
            rotate([90, 0, 0])
                children();
    }

    module positioned_front() {
        position_front()
            enclosure_front();
    }

    module positioned_front_etch() {
        position_front()
            enclosure_front_etch();
    }

    module position_left() {
        translate([enclosure_wall_to_wall_width, -enclosure_length + front_forward_offset, -enclosure_height_lower])
            rotate([0, -90, 0])
                children();
    }

    module positioned_left() {
        position_left()
            enclosure_left();
    }

    module positioned_left_etch() {
        position_left()
            enclosure_left_etch();
    }

    module position_right() {
        translate([0, -enclosure_length_right + front_forward_offset, enclosure_height_upper])
            rotate([0, 90, 0])
                children();
    }

    module positioned_right() {
        position_right()
            enclosure_right();
    }

    module positioned_right_etch() {
        position_right()
            enclosure_right_etch();
    }

    module positioned_top() {
        translate([0, front_forward_offset, enclosure_height_upper - enclosure_vertical_inset])
            rotate([180, 0, 0])
                enclosure_top();
    }

    module position_bottom() {
        translate([0, front_forward_offset - enclosure_length_right, -enclosure_height_lower + enclosure_vertical_inset])
            children();
    }

    module positioned_bottom() {
        position_bottom()
            enclosure_bottom();
    }

    module positioned_bottom_etch() {
        position_bottom()
            enclosure_bottom_etch();
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
        translate([enclosure_wall_to_wall_width, front_forward_offset - (thickness + 1.5 * side_tab_width), enclosure_height_upper - enclosure_vertical_inset - thickness/2]) {
            rotate([0, -90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }

        // Bottom
        translate([enclosure_wall_to_wall_width, front_forward_offset - (thickness + 1.5 * side_tab_width), -enclosure_height_lower + enclosure_vertical_inset + thickness/2]) {
            rotate([0, -90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }
    }

    module positioned_right_bolts() {
        // Top
        translate([0, front_forward_offset - (thickness + 1.5 * side_tab_width), enclosure_height_upper - enclosure_vertical_inset - thickness/2]) {
            rotate([0, 90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }

        // Bottom
        translate([0, front_forward_offset - (thickness + 1.5 * side_tab_width), -enclosure_height_lower + enclosure_vertical_inset + thickness/2]) {
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
            positioned_front_etch();
            positioned_left_etch();
            positioned_right_etch();
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
            %positioned_front_etch();
            %positioned_left_etch();
            %positioned_right_etch();
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
                    pcb(pcb_to_spool, render_sensor_jig);
                    translate([0, 0, -thickness - 2 * eps]) {
                        standard_m4_bolt(nut_distance=thickness + pcb_thickness + 4*eps);
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

            flap_offset = thickness > flap_notch_depth ? -flap_notch_depth + thickness + flap_width_slop/2 : flap_width_slop/2;
            translate([flap_offset, 0, 0]) {
                // Collapsed flaps on the top
                for (i=[0:num_flaps/2 - 1]) {
                    if (i == 0 || render_flaps == 2) {
                        rotate([360/num_flaps * i, 0, 0]) {
                            translate([flap_width, flap_pitch_radius, 0]) {
                                rotate([flap_rendered_angle, 0, 180]) {
                                    flap();
                                    if (i == 0) { 
                                        flap_letter(letter, 1);  // 1 = top
                                    }
                                }
                            }
                        }
                    }
                }

                // Hanging flaps on the bottom
                for (i=[1:num_flaps/2]) {
                    angle = -360/num_flaps*i;
                    translate([0, flap_pitch_radius*cos(angle), flap_pitch_radius * sin(angle)]) {
                        if (i == 1 || render_flaps == 2) {
                            rotate([-90, 0, 0]) {
                                flap();
                                if (i == 1) {
                                    flap_letter(letter, 2);  // 2 = bottom
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(render_spool) {
        translate([(spool_width_clearance - spool_width + spool_width_slop) / 2 + thickness, 0, 0]) {
            spool_struts();

            // motor spool
            translate([spool_width - thickness + 5*spool_horizontal_explosion, 0, 0]) {
                rotate([0, 90, 0]) {
                    color(assembly_color)
                        flap_spool_complete(motor_shaft=true, magnet_hole=true);
                    translate([0, 0, -eps - thickness])
                        flap_spool_etch();
                }
            }
            color(assembly_color1) {
                translate([thickness - 3*spool_horizontal_explosion, 0, 0]) {
                    rotate([0, 90, 0]) {
                        spool_retaining_wall(m4_bolt_hole=true);
                    }
                }
            }
            translate([-5*spool_horizontal_explosion, 0, 0]) {
                rotate([0, 90, 0]) {
                    color(assembly_color)
                        flap_spool_complete(captive_nut=true);
                    flap_spool_etch();
                }
            }
            translate([thickness * 2, 0, 0]) {
                rotate([0, -90, 0]) {
                    standard_m4_bolt(nut_distance=thickness + 7*spool_horizontal_explosion);
                }
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
