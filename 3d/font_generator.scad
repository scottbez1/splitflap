/*
   Copyright 2020 Scott Bezek and the splitflap contributors

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
use<flap.scad>;
use<flap_characters.scad>;
use<projection_renderer.scad>;
use<splitflap.scad>;


// -----------------------
// Configurable parameters
// -----------------------

num_columns = 5;                // Number of columns for layout; 0 for infinite
start_row = 0;                  // First row to render
row_count = 1000;               // Number of rows to render

only_side = 0;                  // 0=both, 1=top only, 2=bottom only

// "Standard" layout, of all flaps front side or back side, which can be rendered to 2 separate files
// for double-sided (duplex) printing on a single substrate. Note that the flap order goes
// right-to-left on the back side, corresponding to the flipped order when printing on the back side
// of the substrate. See MODE_FRONT_BACK for a better alternative when applying lettering to flaps
// one-by-one.
MODE_DOUBLE_SIDED = 0;

// Similar to MODE_DOUBLE_SIDED, this generates separate front and back layouts, but both the front
// and back sides are ordered left-to-right. This cannot be used for duplex printing on a single sheet
// substrate, but is ideal for things like adhesive vinyl where each letter will be applied individually.
MODE_FRONT_BACK = 1;

// If you want to view the full font with each letter as it appears when at the front of the display,
// it requires rendering twice as many flaps and a special layout with the bottom flaps flipped over.
// You also probably want to set only_side to 1 when using this layout.
MODE_FULL_FONT = 2;

// Each flap's front side is laid out side-by-side with its back side.
// You probably want to set only_side to 1 when using this layout.
MODE_SIDE_BY_SIDE = 3;


layout_mode = MODE_DOUBLE_SIDED;

flip_individual_flaps = false;
flip_entire_layout = false;              // Flip the entire layout of flaps over (e.g. when exporting the bottom sides)

// Gap between flaps
spacing_x = 5;
spacing_y = 5;

bleed = 0;                      // Amount of bleed (in mm) for text to expand beyond the flap boundary

flap_color = [1,1,1];
letter_color = [0,0,0];

render_alignment_marks = false; // Whether to render markings to help with alignment/registration (e.g. for screen printing)

print_3d = false; // embeds and substracts a 0.3mm think font into the flap. Used together with colored_stl_exporter.py

// ---------------------------
// End configurable parameters
// ---------------------------


character_list = get_character_list();

cols = (num_columns == 0) ? len(character_list) : num_columns;
total_rows = floor((len(character_list)-1+cols) / cols);
visible_rows = min(row_count, total_rows - start_row);

kerf_width = 0;
render_fill = false;

flap_gap = get_flap_gap();

assert(!(layout_mode != MODE_DOUBLE_SIDED && render_alignment_marks), "Alignment marks are only supported with duplex double-sided layout mode");

if (!is_projection_rendering()) {
    echo("Info: this model is intended for use via generate_fonts.py in order to export flap font files.");
}

module flap_transform(row, col) {
    x_pos = layout_mode == MODE_SIDE_BY_SIDE ?
        (flap_width*2 + spacing_x*3) * col :
        (flap_width + spacing_x) * col;
    y_pos = layout_mode == MODE_FULL_FONT ?
        (flap_height*2 + flap_gap + spacing_y) * row:
        (flap_height + spacing_y) * row;

    translate([x_pos, -y_pos, 0]) {
        children();
    }
}

module flap_pos(i) {
    col = i % cols;
    row = floor(i / cols);
    offsetted_row = row - start_row;
    if (offsetted_row >= 0 && offsetted_row < row_count) {
        flap_transform(offsetted_row, col) {
            children();
        }
    }
}

module configured_flap(index, flap, front_letter, back_letter) {
    flap_with_letters(flap_color, letter_color, index, flap_gap, flap=flap, front_letter=front_letter, back_letter=back_letter, bleed=bleed, print_3d=print_3d);

    if (layout_mode == MODE_FULL_FONT) {
        translate([0, -flap_pin_width -flap_gap, 0]) {
            rotate([180,0,0]) {
                flap_with_letters(flap_color, letter_color, index - 1, flap_gap, flap=flap, front_letter=back_letter, back_letter=front_letter, bleed=bleed, print_3d=print_3d);
            }
        }
    }

    if (layout_mode == MODE_SIDE_BY_SIDE) {
        translate([2*flap_width + spacing_x, 0]) {
            rotate([0, 180,0]) {
                flap_with_letters(flap_color, letter_color, index, flap_gap, flap=flap, front_letter=back_letter, back_letter=front_letter, bleed=bleed, print_3d=print_3d);
            }
        }
    }
}

module fill_text() {
    // If 'render_fill' is 'true', only continue if 'render_etch' is also set by the script
    if (!render_fill || render_fill && render_etch) {
        children();
    }
}

module alignment_marks() {
    alignment_offset = 5;
    secondary_alignment_offset = 15;
    alignment_diameter = 5;

    // Holes in each corner, intended for alignment pins when building a flap tray, which register the flap tray to the printing platen for repeatability
    color([1,0,0]) {
        linear_extrude(height=5) {
        translate([0, flap_height + alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        translate([0, -(flap_height + spacing_y)*(visible_rows - 1) - alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        translate([cols * flap_width + (cols - 1) * spacing_x, flap_height + alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        translate([cols * flap_width + (cols - 1) * spacing_x, -(flap_height + spacing_y)*(visible_rows - 1) - alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        }
    }

    // Additional alignment holes in each corner, intended as registration marks for aligning the screen/image to the flap tray during initial setup
    color([0,0,1]) {
        linear_extrude(height=5) {
        translate([secondary_alignment_offset, flap_height + alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        translate([secondary_alignment_offset, -(flap_height + spacing_y)*(visible_rows - 1) - alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        translate([cols * flap_width + (cols - 1) * spacing_x - secondary_alignment_offset, flap_height + alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        translate([cols * flap_width + (cols - 1) * spacing_x - secondary_alignment_offset, -(flap_height + spacing_y)*(visible_rows - 1) - alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        }
    }
}

module _flip() {
    translate([flip_entire_layout ? cols*flap_width + (cols-1)*spacing_x : 0, 0, 0]) {
        rotate([0, flip_entire_layout ? 180 : 0, 0]) {
            children();
        }
    }
}

module _flip_flap() {
    translate([flip_individual_flaps ? flap_width : 0, 0, 0]) {
        rotate([0, flip_individual_flaps ? 180 : 0, 0]) {
            children();
        }
    }
}

render_index = -1;
render_etch = false;

projection_renderer(render_index = render_index, render_etch = render_etch, kerf_width = kerf_width, panel_height = 0, panel_horizontal = 0, panel_vertical = 0) {
    _flip() {
        for(i = [0 : len(character_list) - 1]) {
            flap_pos(i) {
                _flip_flap() {
                    configured_flap(i, flap=true, front_letter=false, back_letter=false);
                }
            }
        }
    }

    fill_text() {
        _flip() {
            for(i = [0 : len(character_list) - 1]) {
                show_front = only_side == 0 || only_side == 1;
                show_back = only_side == 0 || only_side == 2;
                flap_pos(i) {
                    _flip_flap() {
                        configured_flap(i, flap=false, front_letter=show_front, back_letter=show_back);
                    }
                }
            }
        }
    }

    if (render_alignment_marks) {
        fill_text() {
            _flip() {
                alignment_marks();
            }
        }
    }
}
