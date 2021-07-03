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
use<projection_renderer.scad>;
use<splitflap.scad>;

rows = 6;
cols = 18;

frame_width = 96*25.4;
frame_height = 48*25.4;

// Set either the center_center_* or gap_* values to distribute modules based on center-to-center distance or a gap between modules
center_center_x = 4*25.4;
center_center_y = 7.25*25.4;

gap_x = undef;
gap_y = undef;


render_index = -1;
render_etch = false;

kerf_width = 0.18;



assert(is_undef(center_center_x) || center_center_x >= get_enclosure_width(), "Horizontal center-to-center value must be at least the enclosure width");
assert(is_undef(center_center_y) || center_center_y >= get_enclosure_height(), "Vertical center-to-center value must be at least the enclosure height");
assert(is_undef(gap_x) || gap_x >= 0);
assert(is_undef(gap_y) || gap_y >= 0);

layout_center_center_x = is_undef(center_center_x) ? get_enclosure_width() + gap_x : center_center_x;
layout_center_center_y = is_undef(center_center_y) ? get_enclosure_height() + gap_y : center_center_y;

module centered_front() {
    translate([-get_front_window_right_inset() - get_front_window_width()/2, -get_enclosure_height_lower()]) {
        children();
    }
}

//projection_renderer(render_index = render_index, render_etch = render_etch, kerf_width = kerf_width, panel_height = 0, panel_horizontal = 0, panel_vertical = 0) {

//}

color("red")
translate([(cols-1)/2 * layout_center_center_x - center_center_x*6*1.5, -(rows-1)/2 * layout_center_center_y, -1.1]) {
    square([center_center_x*6, frame_height], center=true);
}
color("green")
translate([(cols-1)/2 * layout_center_center_x - center_center_x*6*0.5, -(rows-1)/2 * layout_center_center_y, -1.1]) {
    square([center_center_x*6, frame_height], center=true);
}
color("pink")
translate([(cols-1)/2 * layout_center_center_x + center_center_x*6*0.5, -(rows-1)/2 * layout_center_center_y, -1.1]) {
    square([center_center_x*6, frame_height], center=true);
}
color("orange")
translate([(cols-1)/2 * layout_center_center_x + center_center_x*6*1.5, -(rows-1)/2 * layout_center_center_y, -1.1]) {
    square([center_center_x*6, frame_height], center=true);
}

color([0.5, 0.5, 0.5, 0.2])
render(convexity = 2) {
    difference() {
        translate([(cols-1)/2 * layout_center_center_x, -(rows-1)/2 * layout_center_center_y]) {
            square([frame_width, frame_height], center=true);
        }

        for (i=[0:cols-1]) {
            for (j=[0:rows-1]) {
                translate([i * layout_center_center_x, -j * layout_center_center_y]) {
                    centered_front() {
                        enclosure_front_cutouts_2d();
                    }
                }
            }
        }
    }
}

for (i=[0:cols-1]) {
    for (j=[0:rows-1]) {
        translate([i * layout_center_center_x, -j * layout_center_center_y]) {
            translate([-flap_width/2, get_flap_gap()/2]) {
                flap();
            }
            translate([-flap_width/2, -get_flap_gap()/2]) {
                rotate([-180, 0, 0])
                    flap();
            }
        }
    }
}