/*
   Copyright 2018 Scott Bezek and the splitflap contributors

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

flap_width = 54;
flap_height = 43;
flap_thickness = 30 / 1000 * 25.4; // 30 mil
flap_corner_radius = 3.1; // 2.88-3.48mm (used just for display)
flap_width_slop = 0.5;  // amount of slop of the flap side to side between the 2 spools

flap_notch_height_auto = false;
flap_notch_height_default = 15;
flap_notch_depth = 3.2;

flap_notch_height = (flap_notch_height_auto == true) ? sqrt(spool_outer_radius*spool_outer_radius - flap_pitch_radius*flap_pitch_radius) : flap_notch_height_default;

flap_pin_width = 1.4;
flap_color = [1, 1, 1];  // white, "ffffff"

eps=.01;

flap_hole_radius = (flap_pin_width + 1) / 2;
flap_hole_separation = 1;  // additional spacing between hole edges
flap_gap = (flap_hole_radius * 2 - flap_pin_width) + flap_hole_separation;
letter_color = color_invert(flap_color);  // inverse of the flap color, for contrast


// flap();

module flap_2d(cut_tabs = true) {
    translate([0, -flap_pin_width/2, 0])
    difference() {
        union() {
            square([flap_width, flap_height - flap_corner_radius]);

            // rounded corners
            hull() {
                translate([flap_corner_radius, flap_height - flap_corner_radius])
                    circle(r=flap_corner_radius, $fn=40);
                translate([flap_width - flap_corner_radius, flap_height - flap_corner_radius])
                    circle(r=flap_corner_radius, $fn=40);
            }
        }
        // spool tabs
        if(cut_tabs) {
            translate([-eps, flap_pin_width])
                square([eps + flap_notch_depth, flap_notch_height]);
            translate([flap_width - flap_notch_depth, flap_pin_width])
                square([eps + flap_notch_depth, flap_notch_height]);
        }
    }
}

module flap() {
    color(flap_color)
    translate([0, 0, -flap_thickness/2])
    linear_extrude(height=flap_thickness) {
        flap_2d();
    }
}

module draw_letter(letter) {
    translate([0, -flap_height * letter_height, 0])  // valign compensation
        scale([letter_width, 1, 1])
            translate([letter_offset_x, letter_offset_y])
                text(text=letter, size=flap_height * letter_height * 2, font=letter_font, halign="center");
}

module flap_letter(letter, half = 0) {
    color(letter_color)
    translate([0, 0, flap_thickness/2 + eps])
    linear_extrude(height=0.1, center=true) {
        if (half != 0) {  // trimming to top (1) or bottom (2)
            intersection() {
                flap_2d();  // limit to bounds of flap
                translate([flap_width/2, -flap_pin_width/2, 0]) {
                    rotation = (half == 2) ? -180 : 0;  // flip upside-down for bottom
                    gap_comp = (letter_gap_comp == true) ? -flap_gap/2 : 0;
                    translate([0, gap_comp, 0])
                        rotate([0,0,rotation])
                            draw_letter(letter);
                }
            }
        } else {
            translate([flap_width/2, -flap_pin_width/2 - flap_gap/2, 0])
                draw_letter(letter);
        }
    }
}

