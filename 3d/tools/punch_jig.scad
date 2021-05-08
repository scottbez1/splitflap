/*
   Copyright 2018 Scott Bezek and the splitflap contributors

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

include<../flap_dimensions.scad>;

print_tolerance = 0.1;
jig_thickness = 2;

clamp_inset = 0.4;

punch_width = 26;
punch_lower_height = 7.7;
punch_gap_height = 2.5;
slot_inset_depth = 9.3;
slot_depth = 3;
slot_width = 15;

eps = 0.1;

jig_length = flap_width + slot_inset_depth;
flap_void_width = flap_width + slot_depth + print_tolerance*2 + eps;
flap_void_height = jig_thickness + print_tolerance + flap_height - (punch_width - slot_width)/2 + flap_pin_width + print_tolerance + eps;
flap_corner_relief = 0.05; // as multiplier of flap void

union() {
    linear_extrude(height=punch_gap_height - print_tolerance*2) {
        difference() {
            square([jig_length, jig_thickness*2 + print_tolerance*2 + flap_height]);
            translate([slot_inset_depth - print_tolerance*2, jig_thickness + print_tolerance + (punch_width - slot_width)/2 - flap_pin_width - print_tolerance]) {
                union() {
                    square([flap_void_width, flap_void_height]);
                    // create relief for flap corner
                    translate([-1,-1]) {
                        square([flap_void_width*flap_corner_relief, flap_void_width*flap_corner_relief]);
                    }
                }

            }
        }
    }

    // create support for right side of punch
    translate([0, 0, punch_gap_height - print_tolerance*2]) {
        linear_extrude(height=punch_lower_height, scale=[1, (jig_thickness + clamp_inset)/jig_thickness]) {
            square([jig_length, jig_thickness]);
        }
    }

    // create support for left side of punch
    translate([0, jig_thickness*2 + print_tolerance*2 + punch_width, punch_gap_height - print_tolerance*2]) {
        linear_extrude(height=punch_lower_height, scale=[1, (jig_thickness + clamp_inset)/jig_thickness]) {
            translate([0, -jig_thickness]) {
                square([slot_inset_depth - print_tolerance*2, jig_thickness]);
            }
        }
    }
}


%translate([slot_inset_depth, jig_thickness + print_tolerance + (punch_width - slot_width)/2, -8]) {
    cube([slot_depth, slot_width, 16]);
}
