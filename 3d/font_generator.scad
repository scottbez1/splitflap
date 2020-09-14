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
spacing_x = 10;
spacing_y = 2;
kerf_width = 0;

num_characters = len(character_list) - 1;

module flap_x_pos(i) {
    extent = (flap_width + spacing_x) * i;
    translate([extent, 0, 0])
        children();
}

module top_flap(i) {
    flap_x_pos(i)
    translate([0, flap_pin_width/2, 0])
        children();
}

module bottom_flap(i) {
    flap_x_pos(i)
    translate([flap_width, -flap_pin_width/2 - spacing_y, 0])
        rotate([0, 0, 180])
            children();
}

render_index = -1;

projection_renderer(render_index = render_index, render_etch=false, kerf_width=kerf_width, panel_height = 0, panel_horizontal = 0, panel_vertical = 0) {
    for(i = [0 : num_characters]) {
        top_flap(i)
            flap();
        bottom_flap(i)
            flap();
    }

    for(i = [0 : num_characters]) {
        top_flap(i)
            flap_letter(character_list[i], 1);
        bottom_flap(i)
            flap_letter(character_list[i], 2);
    }
}


