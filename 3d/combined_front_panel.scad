/*
   Copyright 2021 Scott Bezek and the splitflap contributors

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

use<color_util.scad>;
use<flap.scad>;
use<projection_renderer.scad>;
use<splitflap.scad>;

// -----------------------
// Configurable parameters
// -----------------------

rows = 6;
cols = 18;

// Set either frame_width/height, or frame_margin_x/y to set the overall frame dimensions. margin_x/y will calculate the necessary frame width
// based on the number of modules and spacing and add the margin onto both sides.
frame_width = 96*25.4;
frame_height = 48*25.4;
frame_margin_x = undef;
frame_margin_y = undef;

// Set either the center_center_* or gap_* values to distribute modules based on center-to-center distance or a gap between modules.
// You can set gap_x and gap_y to 0 to get the closest possible spacing of modules.
center_center_x = 4*25.4;
center_center_y = 7.25*25.4;
gap_x = undef;
gap_y = undef;

// Vertical centering mode
//   0 = center based on letter/flap center
//   1 = center based on window cutout center
//   2 = center based on module overall center
center_mode = 0;

// Use tool_diameter for rotary/milling cutters, or kerf_width for laser.
// kerf_width applies a naive offset that is only valid for very small kerf from a laser cutter (e.g. 0.2mm)
// tool_diameter will modify a few parts of the design for milling bits, but will not handle all sizes well; use caution and review the design carefully.
tool_diameter = 0; // e.g. set to "1/8*25.4 * 1.1" for a 1/8 inch bit, plus 10%
kerf_width = 0;


// Preview-only paramters:

display_text = [
    "THIS IS SOME TEXT ",
    "",
    "    SPLIT     FLAP",
    "           HI     ",
    "WOW               ",
    "                 .",
];

// Number of full modules to render in 3d preview
render_full_modules_count = 4;

// ---------------------------
// End configurable parameters
// ---------------------------


render_index = -1;
render_etch = false;

assert(is_undef(center_center_x) || center_center_x >= get_enclosure_width(), "Horizontal center-to-center value must be at least the enclosure width");
assert(is_undef(center_center_y) || center_center_y >= get_enclosure_height(), "Vertical center-to-center value must be at least the enclosure height");
assert(is_undef(gap_x) || gap_x >= 0);
assert(is_undef(gap_y) || gap_y >= 0);

layout_center_center_x = is_undef(gap_x) ? center_center_x : get_enclosure_width() + gap_x;
layout_center_center_y = is_undef(gap_y) ? center_center_y : get_enclosure_height() + gap_y;

echo(debug_gap_x = layout_center_center_x - get_enclosure_width());
echo(debug_gap_y = layout_center_center_y - get_enclosure_height());

layout_frame_width = is_undef(frame_margin_x) ? frame_width : layout_center_center_x * (cols - 1) + get_enclosure_width() + frame_margin_x*2;
layout_frame_height = is_undef(frame_margin_y) ? frame_height : layout_center_center_y * (rows - 1) + get_enclosure_height() + frame_margin_y*2;

y_offset = center_mode == 0 ? 0 :
            center_mode == 1 ? get_front_window_lower() - (get_front_window_upper() + get_front_window_lower())/2 :
            center_mode == 2 ? get_enclosure_height_lower() - get_enclosure_height()/2:
            0;

echo(debug_frame_margin_top = (layout_frame_height - layout_center_center_y * (rows - 1))/2 - get_enclosure_height_upper() - y_offset);
echo(debug_frame_margin_bottom = (layout_frame_height - layout_center_center_y * (rows - 1))/2 - get_enclosure_height_lower() + y_offset);

module centered_front() {
    translate([-get_front_window_right_inset() - get_front_window_width()/2, -get_enclosure_height_lower() + y_offset]) {
        children();
    }
}

flap_color = get_flap_color();
letter_color = get_letter_color();
assembly_colors = get_assembly_colors();
frame_color = assembly_colors[0];

projection_renderer(render_index = render_index, render_etch = render_etch, kerf_width = kerf_width, panel_height = 0, panel_horizontal = 0, panel_vertical = 0) {
    color(frame_color) {
        linear_extrude(height=get_thickness()) {
            difference() {
                translate([(cols-1)/2 * layout_center_center_x, -(rows-1)/2 * layout_center_center_y]) {
                    square([layout_frame_width, layout_frame_height], center=true);
                }

                for (i=[0:cols-1]) {
                    for (j=[0:rows-1]) {
                        translate([i * layout_center_center_x, -j * layout_center_center_y]) {
                            centered_front() {
                                enclosure_front_cutouts_2d(tool_diameter=tool_diameter);
                            }
                        }
                    }
                }
            }
        }
    }
}

if (render_index == -1 && !is_projection_rendering()) {
    for (i=[0:cols-1]) {
        for (j=[0:rows-1]) {
            index = i + j*cols;
            translate([i * layout_center_center_x, -j * layout_center_center_y]) {
                if (index < render_full_modules_count) {
                    translate([get_front_window_right_inset() + get_front_window_width()/2, y_offset, -get_front_forward_offset()]) {
                        rotate([90, 0, 180]) {
                            split_flap_3d(get_flap_index_for_letter(display_text[j][i]), false, false);
                        }
                    }
                } else {
                    // Render just a flap
                    translate([-flap_width/2, 0]) {
                        flap_with_letters(flap_color, letter_color, get_flap_index_for_letter(display_text[j][i]), get_flap_gap());
                    }
                    translate([-flap_width/2, -(get_flap_gap() + get_flap_pin_width())]) {
                        rotate([-180, 0, 0]) {
                            flap_with_letters(flap_color, letter_color, get_flap_index_for_letter(display_text[j][i])-1, get_flap_gap());
                        }
                    }
                }
            }
        }
    }
}
