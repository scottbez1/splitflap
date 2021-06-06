/*
   Copyright 2015-2021 Scott Bezek and the splitflap contributors

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
include<flap_dimensions.scad>;
include<flap_fonts.scad>;
include<global_constants.scad>;

character_list = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,'";

function get_character_list() = character_list;
function get_flap_index_for_letter(letter) = search(letter, character_list)[0];
function get_letter_for_front(flap_index) = 
    flap_index >= len(character_list) ? get_letter_for_front(flap_index - len(character_list)) :
    flap_index < 0 ? get_letter_for_front(flap_index + len(character_list)) :
    character_list[flap_index];

// The letter (bottom half) on the back of a flap is the same as the letter (top half) on the front of the next flap
function get_letter_for_back(flap_index) = get_letter_for_front(flap_index + 1);

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


module _flap(flap_color) {
    color(flap_color) {
        translate([0, 0, -flap_thickness/2]) {
            linear_extrude(height=flap_thickness) {
                flap_2d();
            }
        }
    }
}

module _draw_letter(letter) {
    overrides = get_letter_overrides(letter);
    height = is_undef(overrides[3]) ? letter_height : overrides[3];
    width = is_undef(overrides[4]) ? letter_width : overrides[4];
    translate([0, -flap_height * height, 0]) {  // valign compensation
        scale([width, 1, 1]) {
            offset_x = is_undef(overrides[1]) ? letter_offset_x : letter_offset_x + overrides[1];
            offset_y = is_undef(overrides[2]) ? letter_offset_y : letter_offset_y + overrides[2];
            translate([offset_x, offset_y]) {
                text(text=letter, size=flap_height * height * 2, font=letter_font, halign="center");
            }
        }
    }
}

module _flap_letter(letter, letter_color, flap_gap, front=true, bleed = 0, render_keepout_violations = 0) {
    color(letter_color) {
        translate([0, 0, front ? flap_thickness/2 + eps : -flap_thickness/2 - eps]) {
            linear_extrude(height=0.1, center=true) {
                intersection() {
                    offset(r=bleed) {
                        flap_2d();
                    }
                    translate([flap_width/2, -flap_pin_width/2, 0]) {
                        rotation = front ? 0 : -180;  // flip upside-down for back
                        gap_comp = (letter_gap_comp == true) ? -flap_gap/2 : 0;
                        translate([0, gap_comp, 0]) {
                            rotate([rotation, 0, 0]) {
                                _draw_letter(letter);
                            }
                        }
                    }
                }
            }
        }
    }
    if (render_keepout_violations > 0) {
        color([1,0,0]) {
            translate([0, 0, front ? flap_thickness/2 + eps : -flap_thickness/2 - eps]) {
                linear_extrude(height=1, center=true) {
                    intersection() {
                        offset(r=bleed) {
                            flap_2d();
                        }
                        translate([0, flap_height - flap_pin_width/2 -render_keepout_violations, 0]) {
                            square([flap_width, render_keepout_violations]);
                        }
                        translate([flap_width/2, -flap_pin_width/2, 0]) {
                            rotation = front ? 0 : -180;  // flip upside-down for back
                            gap_comp = (letter_gap_comp == true) ? -flap_gap/2 : 0;
                            translate([0, gap_comp, 0]) {
                                rotate([rotation, 0, 0]) {
                                    _draw_letter(letter);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

module flap_with_letters(flap_color, letter_color, flap_index, flap_gap, flap=true, front_letter=true, back_letter=true, bleed=0, render_keepout_violations=0) {
    if (flap) {
        _flap(flap_color);
    }
    if (front_letter) {
        _flap_letter(get_letter_for_front(flap_index), letter_color, flap_gap, front=true, bleed=bleed, render_keepout_violations=render_keepout_violations);
    }
    if (back_letter) {
        _flap_letter(get_letter_for_back(flap_index), letter_color, flap_gap, front=false, bleed=bleed, render_keepout_violations=render_keepout_violations); 
    }
}

// Example:
i = 1;
gap = 5;

flap_with_letters([1,0,0], [1,1,0], flap_index=i, flap_gap=gap, bleed=2);
translate([0, -flap_pin_width-gap, 0])
rotate([180, 0, 0]) {
    flap_with_letters([1,0,0], [1,1,0], flap_index=i - 1, flap_gap=gap, bleed=2);
}
