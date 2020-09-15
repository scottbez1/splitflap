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

include<flap_dimensions.scad>;
use<projection_renderer.scad>;
use<splitflap.scad>;

character_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 '.?";
num_columns = 10;  // 0 for infinite

spacing_x = 10;
spacing_y = 5;

kerf_width = 0;
render_fill = false;

num_characters = len(character_list) - 1;
flap_gap = get_flap_gap();

module flap_pos(i) {
    x_pos = (flap_width + spacing_x) * ((num_columns == 0) ? i : (i % num_columns));
    y_pos = (flap_height * 2 + spacing_y + flap_gap) * ((num_columns == 0) ? 0 : floor((i / num_columns)));
    translate([x_pos, -y_pos, 0])
        children();
}

module top_flap(i) {
    flap_pos(i)
    translate([0, flap_pin_width/2 + flap_gap, 0])
        children();
}

module bottom_flap(i) {
    flap_pos(i)
    translate([flap_width, -flap_pin_width/2 - flap_gap, 0])
        rotate([0, 0, 180])
            children();
}

module fill_text() {
    // If 'render_fill' is 'true', only continue if 'render_etch' is also set by the script
    if (!render_fill || render_fill && render_etch) {
        children();
    }
}

render_index = -1;
render_etch = false;

projection_renderer(render_index = render_index, render_etch = render_etch, kerf_width = kerf_width, panel_height = 0, panel_horizontal = 0, panel_vertical = 0) {
    for(i = [0 : num_characters]) {
        top_flap(i)
            flap();
        bottom_flap(i)
            flap();
    }

    fill_text()
    for(i = [0 : num_characters]) {
        top_flap(i)
            flap_letter(character_list[i], 1);
        bottom_flap(i)
            flap_letter(character_list[i], 2);
    }
}
