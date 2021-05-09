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
use<projection_renderer.scad>;
use<splitflap.scad>;

character_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'.? ";
num_columns = 10;  // 0 for infinite

spacing_x = 10;
spacing_y = 10;

kerf_width = 0;
render_fill = false;

flap_gap = get_flap_gap();

// 0: do not do double-sided render
// 1: render front
// 2: render back
side = 0;
bleed = false;

start_row = 0;
row_count = 1000;

module flap_transform(row, col, flip) {
    x_pos = (flap_width + spacing_x) * col;
    y_pos = (flap_height * 2 + spacing_y + flap_gap/2) * row;
    translate([x_pos, -y_pos, 0])
        translate([flip * flap_width, (0.5 - flip) * (flap_pin_width + flap_gap), 0])
            rotate([0, 0, flip * 180])
                children();
}

module flap_pos(i, j) {
    // i: character id, j = 0: bottom-of-letter, j = 1: top-of-letter
    cols = (num_columns == 0) ? len(character_list) : num_columns;
    if (side == 0) {
        // regular render
        col = i % cols;
        row = floor(i / cols);
        offsetted_row = row - start_row;
        flip = 1 - j;
        if (offsetted_row >= 0 && offsetted_row < row_count) {
            flap_transform(offsetted_row, col, flip)
                children();
        }
    } else {
        // double sided render
        char_side = i % 2; // 0: front, 1: back
        if (char_side + 1 == side) {
            k = (floor(i / 2) * 2 + j + char_side) % len(character_list);
            flip = (char_side == 0) ? (1 - j) : j;
            row = floor(k / 2 / cols);
            col_front = floor(k / 2) % cols;
            col = (char_side == 0) ? col_front : (cols - col_front - 1);
            offsetted_row = row - start_row;
            if (offsetted_row >= 0 && offsetted_row < row_count) {
                flap_transform(offsetted_row, col, flip)
                    children();
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

render_index = -1;
render_etch = false;

projection_renderer(render_index = render_index, render_etch = render_etch, kerf_width = kerf_width, panel_height = 0, panel_horizontal = 0, panel_vertical = 0, bleed = bleed) {
    for(i = [0 : len(character_list) - 1])
        for(j = [0 : 1]) {
            flap_pos(i, j)
                flap();
        }

    fill_text()
        for(i = [0 : len(character_list) - 1])
            for(j = [0 : 1])
                flap_pos(i, j)
                    flap_letter(character_list[i], 2-j, bleed);
}
