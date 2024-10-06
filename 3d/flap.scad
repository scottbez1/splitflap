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
include<global_constants.scad>;
use<flap_fonts.scad>;
use<flap_characters.scad>;

// TODO: extract core flap spool dimensions used for vertical_keepout_size instead of using the full splitflap file
use<splitflap.scad>;

character_list = get_character_list();
color_list = [
    ["p", [122,  40, 203] / 255], // #7a28cb
    ["r", [230,  57,  70] / 255], // "#e63946
    ["y", [255, 214,  57] / 255], // "#ffd639
    ["g", [102, 215, 209] / 255], // "#66d7d1
    ["w", [255, 255, 255] / 255], // "#ffffff
];


// Facet number variable refers to a special variable "$fn" that is used to control the number of facets to generate an arc.
// Higher than 100 is not recommended according to the docs as it demands a lot resources
letter_facet_number = 100;

// Vertical keepout refers to the portion of bottom flaps that are visible stacked behind the frontmost flap.
vertical_keepout_mode = 1;              // 0=ignore; 1=highlight; 2=cut
vertical_keepout_size_factor = 1.1;     // Expand calculated keepout region by this factor. 1=no expansion, 1.5=50% expansion, etc


vertical_keepout_size = get_flap_arc_separation() * vertical_keepout_size_factor;

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

module _apply_special_color(letter, standard_letter_color) {
    color_index = search([letter], color_list);
    if (len(color_index) > 0 && is_num(color_index[0])) {
        color(color_list[color_index[0]][1]) {
            children();
        }
    } else {
        color(standard_letter_color) {
            children();
        }
    }
}

module _draw_letter(letter, flap_gap) {
    overrides = get_letter_overrides(letter);
    height = is_undef(overrides[3]) ? get_font_setting("height") : overrides[3];
    width = is_undef(overrides[4]) ? get_font_setting("width") : overrides[4];
    offset_x = is_undef(overrides[1]) ? get_font_setting("offset_x") : get_font_setting("offset_x") + overrides[1];
    offset_y = is_undef(overrides[2]) ? get_font_setting("offset_y") : get_font_setting("offset_y") + overrides[2];
    thickness_offset = is_undef(overrides[5]) ? (is_undef(get_font_setting("thickness_offset")) ? 0 : get_font_setting("thickness_offset")) : overrides[5];

    color_index = search([letter], color_list);
    if (len(color_index) > 0 && is_num(color_index[0])) {
        color_height = get_font_setting("color_height");
        color_height_with_default = is_undef(color_height) ? height : color_height;
        color_offset_y = get_font_setting("color_offset_y");
        color_offset_y_with_default = is_undef(color_offset_y) ? offset_y : color_offset_y;
        translate([0, color_offset_y_with_default]) {
            square([flap_width - flap_notch_depth * 2 - 4, (flap_height * 2 + flap_gap) * color_height_with_default], center=true);
        }
    } else {
        translate([0, -flap_height * height, 0]) {  // valign compensation
            scale([width, 1, 1]) {
                translate([offset_x, offset_y]) {
                    offset(delta=thickness_offset) {
                        text(text=letter, size=flap_height * height * 2, font=get_font_setting("font"), halign="center", $fn=letter_facet_number);
                    }
                }
            }
        }
    }
}

module _flap_letter(letter, letter_color, flap_gap, front=true, bleed = 0) {
    _apply_special_color(letter, letter_color) {
        translate([0, 0, front ? flap_thickness/2 + eps : -flap_thickness/2 - eps]) {
            linear_extrude(height=0.1, center=true) {
                intersection() {
                    difference() {
                        offset(r=bleed) {
                            flap_2d();
                        }

                        // Note that the verticle keepout (if applicable) is subtracted *after* the bleed is applied; otherwise
                        // text would bleed into the keepout zone.
                        if (vertical_keepout_mode > 0) {
                            vertical_keepout_width = flap_width + bleed*2; // keepout must expand by "bleed" on the left and right
                            translate([-bleed, flap_height - flap_pin_width/2 - vertical_keepout_size, 0]) {
                                square([vertical_keepout_width, vertical_keepout_size]);
                            }
                        }

                        // TODO: consider adding horizontal keepout; it may be undesirable for text to meet the sides of the flaps
                        // (or possibly extend beyond the sides if bleed > 0).
                    }
                    translate([flap_width/2, -flap_pin_width/2, 0]) {
                        rotation = front ? 0 : -180;  // flip upside-down for back
                        gap_comp = (use_letter_gap_compensation() == true) ? -flap_gap/2 : 0;
                        translate([0, gap_comp, 0]) {
                            rotate([rotation, 0, 0]) {
                                _draw_letter(letter, flap_gap);
                            }
                        }
                    }
                }
            }
        }
    }

    // Highlight any portion of the letter within the keepout region in red, if desired.
    if (vertical_keepout_mode == 1) {
        color([1,0,0]) {
            translate([0, 0, front ? flap_thickness/2 + eps : -flap_thickness/2 - eps]) {
                linear_extrude(height=1, center=true) {
                    intersection() {
                        offset(r=bleed) {
                            flap_2d();
                        }
                        translate([0, flap_height - flap_pin_width/2 - vertical_keepout_size, 0]) {
                            square([flap_width, vertical_keepout_size]);
                        }
                        translate([flap_width/2, -flap_pin_width/2, 0]) {
                            rotation = front ? 0 : -180;  // flip upside-down for back
                            gap_comp = (use_letter_gap_compensation() == true) ? -flap_gap/2 : 0;
                            translate([0, gap_comp, 0]) {
                                rotate([rotation, 0, 0]) {
                                    _draw_letter(letter, flap_gap);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

module flap_with_letters(flap_color, letter_color, flap_index, flap_gap, flap=true, front_letter=true, back_letter=true, bleed=0) {
    if (len(character_list) != get_num_flaps()) {
        echo("Warning: character_list and num_flaps mismatch!");
    }
    if (flap) {
        _flap(flap_color);
    }
    if (front_letter) {
        _flap_letter(get_letter_for_front(flap_index), letter_color, flap_gap, front=true, bleed=bleed);
    }
    if (back_letter) {
        _flap_letter(get_letter_for_back(flap_index), letter_color, flap_gap, front=false, bleed=bleed); 
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
