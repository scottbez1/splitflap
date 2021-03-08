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

include <../shapes.scad>
use <../splitflap.scad>

num_connectors = 4;  // number of connectors (vertically) per section
thickness = 3.0;  // thickness of each connector (should be close to splitflap.scad panel thickness)
cases_per_row = 2;  // number of cases per each row
dual_rows = true;  // whether to put mirrored cases on the opposite side

connector_clearance = 0.5;  // distance between the connector edges and the case walls
wall_thickness = 3.5;  // thickness of the case walls around the connectors
bottom_thickness = 5.0;  // thickness of the bottom of the case, below the connectors
top_clearance = 0.4;  // clearance above the top of the connectors

// auto-calculated constants
combined_offset = connector_clearance + wall_thickness;  // total offset from base to outer edge
side_wall_height = (thickness * num_connectors) + top_clearance;  // total height of the walls above the base
case_width = connector_bracket_width() + (combined_offset * 2);  // width of the case (along X)
case_width_per = case_width - wall_thickness;  // width of each case when in a group (combined wall thickness)
case_length = connector_bracket_length() + (combined_offset * 2);  // length of the case (along Y)
case_length_dual_offset = case_length + connector_bracket_length() - wall_thickness;  // Y offset for the 'dual' case row
case_length_dual = case_length + case_length_dual_offset;  // total case length alokng Y with 'dual' option enabled


module base_outline() {
    offset(r=combined_offset, $fn=60) {
        connector_bracket_2d();
    }
}

module connector_case_cutout(num) {
    width = 14 + connector_clearance;  // arbitrary
    radius = 2.0;  // nicely rounded corners
    height = (side_wall_height) + radius;  // total height of the tool (Z)
    depth = connector_bracket_length();  // total depth of the tool (Y)

    translate([0, depth, 0])
    rotate([90, 0, 0])
    linear_extrude(depth, convexity=2)
    translate([connector_bracket_width()/2 - width/2, bottom_thickness, 0])
    rounded_square([width, height], r=radius, $fn=30);
}

module connector_case_single(num) {
    difference() {
        // case body
        union() {
            // pocket
            translate([0, 0, bottom_thickness]) {
                linear_extrude(side_wall_height, convexity=10) {
                    difference() {
                        base_outline();
                        offset(delta=connector_clearance) {
                            connector_bracket_2d();
                        }
                    }
                }
            }

            // bottom
            linear_extrude(bottom_thickness, convexity=2) {
                base_outline();
            }
        }
        connector_case_cutout(num);
    }
}

module connector_case_row(num_conn, num_cases) {
    union() {
        for ( i = [0 : num_cases - 1] ) {
            translate([case_width_per * i, 0, 0])
            connector_case_single(num_conn);
        }
    }
    
}

module connector_case(num_conn, num_cases, dual=false) {
    union() {
        connector_case_row(num_conn, num_cases);
        if(dual) {
            translate([0, case_length_dual_offset, 0])
            mirror([0, 1, 0])
            connector_case_row(num_conn, num_cases);
        }
    }
}


connector_case(num_connectors, cases_per_row, dual_rows);
