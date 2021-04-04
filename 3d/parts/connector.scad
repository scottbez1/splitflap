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

include<enclosure.scad>;
include<hardware.scad>;
include<../common.scad>;

connector_bracket_length_outer = 14;
connector_bracket_length_inner = side_tab_width * 2 - m4_button_head_diameter/2;
connector_bracket_thickness = captive_nut_inset - thickness - 0.2;
connector_bracket_width = enclosure_width - enclosure_wall_to_wall_width + thickness*2 + connector_bracket_thickness*2;
connector_bracket_overlap = 4;
connector_bracket_clearance = 0.40;
connector_bracket_depth_clearance = 0.20;


module connector_bracket_2d() {
    difference() {
        square([connector_bracket_width, connector_bracket_length_outer]);
        translate([connector_bracket_thickness, -eps]) {
            square([connector_bracket_width - connector_bracket_thickness*2, connector_bracket_length_outer - connector_bracket_length_inner + eps]);
        }
        translate([connector_bracket_thickness - connector_bracket_clearance/2, -eps]) {
            square([thickness + connector_bracket_clearance, connector_bracket_length_outer - connector_bracket_overlap + connector_bracket_depth_clearance + eps]);
        }
        translate([connector_bracket_width - connector_bracket_thickness - thickness - connector_bracket_clearance/2, -eps]) {
            square([thickness + connector_bracket_clearance, connector_bracket_length_outer - connector_bracket_overlap + connector_bracket_depth_clearance + eps]);
        }
    }
}

module connector_bracket() {
    linear_extrude(height=thickness) {
        connector_bracket_2d();
    }
}

module connector_bracket_side_holes() {
    // overlap slot
    translate([enclosure_vertical_inset - thickness - connector_bracket_clearance/2, -connector_bracket_overlap]) {
        square([thickness + connector_bracket_clearance, connector_bracket_overlap + eps]);
    }
}

