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
include<../common.scad>;


zip_tie_height = 3.0;  // height of the zip-tie hole
zip_tie_width = 2.0;  // width of the zip-tie holes
zip_tie_spacing = 6.5;  // spacing between each zip-tie hole, inside edges
zip_tie_fillet = 0.5;  // radius of the ounded zip-tie hole corners


// M4 bolts
m4_hole_diameter = 4.5;
m4_bolt_length = 12;
m4_button_head_diameter = 7.6 + .2;
m4_button_head_length = 2.2 + .2;
m4_nut_width_flats = 7 + .2;
m4_nut_width_corners = 7/cos(180/6);
m4_nut_width_corners_padded = m4_nut_width_corners + .2;
m4_nut_length = 3.2;
m4_nut_length_padded = m4_nut_length + .2;

hardware_color = [0.75, 0.75, 0.8];  // steel, "bfbfcc"
bolt_color = hardware_color;
nut_color = color_multiply(hardware_color, [0.933, 0.933, 0.9, 1.0]);  // "b2b2b7" with steel

module standard_m4_bolt(nut_distance=-1) {
    if (render_bolts) {
        color(bolt_color)
            roughM4_7380(10);
        if (nut_distance >= 0) {
            color(nut_color) {
                translate([0, 0, nut_distance]) {
                    linear_extrude(m4_nut_length) {
                        difference() {
                            circle(r=m4_nut_width_corners/2, $fn=6);
                            circle(r=m4_hole_diameter/2, $fn=20);
                        }
                    }
                }
            }
        }
    }
}

// ##### CAPTIVE NUT NEGATIVE #####

// Centered in the x dimension
module captive_nut(bolt_diameter, bolt_length, nut_width, nut_length, nut_inset) {
    union() {
        translate([-bolt_diameter/2, 0, 0])
            square([bolt_diameter, bolt_length]);
        translate([-nut_width/2, nut_inset, 0])
            square([nut_width, nut_length]);
    }
}
module m4_captive_nut(bolt_length=m4_bolt_length) {
    captive_nut(m4_hole_diameter, bolt_length + 1, m4_nut_width_flats, m4_nut_length_padded, captive_nut_inset);
}


module zip_tie_holes() {
    spacing = (zip_tie_spacing + zip_tie_width)/2;

    translate([-spacing, 0, 0])
        rounded_square([zip_tie_width, zip_tie_height], center=true, r=zip_tie_fillet, $fn=30);
    translate([spacing, 0, 0])
        rounded_square([zip_tie_width, zip_tie_height], center=true, r=zip_tie_fillet, $fn=30);
}

module front_back_captive_nuts() {
    for (i = [0 : 2 : num_front_tabs-1]) {
        translate([(i*2 + 1.5) * front_tab_width, -thickness, 0])
            m4_captive_nut();
    }
}

module side_captive_nuts(hole_types=[]) {
    for (i = [0 : len(hole_types)-1]) {
        hole_type = hole_types[i];
        translate([-thickness, (i*2 + 1.5) * side_tab_width, 0]) {
            rotate([0, 0, -90]) {
                if (hole_type == 2) {
                } else if (hole_type == 1) {
                    m4_captive_nut();
                }
            }
        }
    }
}
