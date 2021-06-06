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
use<projection_renderer.scad>;
use<splitflap.scad>;

keepout_size = get_flap_arc_separation() * 1.1;

character_list = get_character_list();
num_columns = 4;  // 0 for infinite
start_row = 0;
row_count = 1000;

only_side = 1;
show_top_and_bottom = true;

spacing_x = 0;
spacing_y = 5;

bleed = 0.5;

flap_color = [1,1,1];
letter_color = [0,0,0];


flip_over = false;

echo(flap_height=flap_height);

cols = (num_columns == 0) ? len(character_list) : num_columns;
total_rows = floor((len(character_list)-1+cols) / cols);

kerf_width = 0;
render_fill = false;

flap_gap = get_flap_gap();

module flap_transform(row, col) {
    x_pos = (flap_width + spacing_x) * col;
    y_pos = show_top_and_bottom ?
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
    flap_with_letters(flap_color, letter_color, index, flap_gap, flap=flap, front_letter=front_letter, back_letter=back_letter, bleed=bleed, render_keepout_violations=keepout_size);

    if (show_top_and_bottom) {
        translate([0, -flap_pin_width -flap_gap, 0]) {
            rotate([180,0,0]) {
                flap_with_letters(flap_color, letter_color, index - 1, flap_gap, flap=flap, front_letter=back_letter, back_letter=front_letter, bleed=bleed, render_keepout_violations=keepout_size);
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

module alignment_holes() {
    alignment_offset = 5;
    led_offset = 15;
    alignment_diameter = 5;

    visible_rows = total_rows - start_row;

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

    // LED holes
    color([0,0,1]) {
        linear_extrude(height=5) {
        translate([led_offset, flap_height + alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        translate([led_offset, -(flap_height + spacing_y)*(visible_rows - 1) - alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        translate([cols * flap_width + (cols - 1) * spacing_x - led_offset, flap_height + alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        translate([cols * flap_width + (cols - 1) * spacing_x - led_offset, -(flap_height + spacing_y)*(visible_rows - 1) - alignment_offset - flap_pin_width/2]) {
            circle(d=alignment_diameter, $fn=30);
        }
        }
    }
}

module flip() {
    translate([flip_over ? cols*flap_width + (cols-1)*spacing_x : 0, 0, 0]) {
        rotate([0, flip_over ? 180 : 0, 0]) {
            children();
        }
    }
}

render_index = -1;
render_etch = false;

projection_renderer(render_index = render_index, render_etch = render_etch, kerf_width = kerf_width, panel_height = 0, panel_horizontal = 0, panel_vertical = 0) {
    flip() {
        for(i = [0 : len(character_list) - 1]) {
            flap_pos(i) {
                configured_flap(i, flap=true, front_letter=false, back_letter=false);
            }
        }
    }

    fill_text() {
        flip() {
            for(i = [0 : len(character_list) - 1]) {
                show_front = only_side == 0 || only_side == 1;
                show_back = only_side == 0 || only_side == 2;
                flap_pos(i) {
                    configured_flap(i, flap=false, front_letter=show_front, back_letter=show_back);
                }
            }
            alignment_holes();
        }
    }
}
