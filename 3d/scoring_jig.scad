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

include<flap_dimensions.scad>

print_tolerance = 0.1;
eps = 0.1;
jig_thickness = 2.0; // Vertical thickness of the jig base.
jig_border = 8.0; // horizontal extent of the jig outside the flap size.
flap_slot_depth = 2 * flap_thickness; // depth of the recess the flap sits in.
cutting_slot_width = 2.0; // Relief in the border for the blade to start and stop in.

jig_width = jig_border*2 + print_tolerance * 2 + flap_width;

ruler_width = 24.6;
ruler_depth = 1.6;
ruler_backstop_height = ruler_depth * 2;

difference() {
    union() {
        // Main body of the jig
        linear_extrude(height=jig_thickness + flap_slot_depth) {
            square([jig_width,jig_border*2 + print_tolerance + ruler_width + flap_height]);
        }
        // Backstop for ruler/cutting guide
        linear_extrude(height=jig_thickness + flap_slot_depth + ruler_backstop_height) {
            square(size=[jig_border*2 + print_tolerance * 2 + flap_width, jig_border]);
        }
    }
    union() {
        translate([jig_border - print_tolerance,-eps,jig_thickness]) {
            // Main area for card to sit into
            linear_extrude(height=flap_slot_depth + ruler_backstop_height + eps) {
                square([flap_width + 2 * print_tolerance, flap_height + print_tolerance + ruler_width + jig_border + eps]);
            }
        }
        translate([-eps ,jig_border + ruler_width, jig_thickness]) {
            // remove material from the left side so only the top and right sides are braced
            linear_extrude(height=flap_slot_depth + eps) {
                square([jig_border + print_tolerance + eps, flap_height + print_tolerance + jig_border + eps]);
            }
        }
        translate([-eps, jig_border + ruler_width - cutting_slot_width / 2, jig_thickness]) {
            // Relieve a slot for the blade to start and finish in
            linear_extrude(height=flap_slot_depth + eps) {
                square([jig_width + 2*eps,cutting_slot_width]);
            }
        }
    }
}

