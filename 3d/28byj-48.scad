/*
   Copyright 2015-2018 Scott Bezek and the splitflap contributors

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

28byj48_chassis_radius = 28/2;
28byj48_chassis_height = 19;
28byj48_shaft_offset = 8;
28byj48_mount_outer_radius = 3.5;
28byj48_mount_hole_radius = 4.2/2;
28byj48_mount_center_offset = 35/2;
28byj48_mount_bracket_height = 0.8;

28byj48_shaft_collar_radius = 9/2;
28byj48_shaft_collar_height = 1.5;

28byj48_shaft_radius = 5/2;
28byj48_shaft_height = 10;
28byj48_shaft_slotted_width = 3;
28byj48_shaft_slotted_height = 6;

28byj48_backpack_width = 14.6;
28byj48_backpack_extent = 18; // seen values from 17-18
28byj48_backpack_height = 16;

// Basic 28BYJ-48 model based on various dimensioned drawings.
// Origin is centered on the shaft, on the front face of the motor, with the shaft in the +z direction
module Stepper28BYJ48() {
    $fn = 60;

    eps = 0.01;

    gray = [0.7, 0.7, 0.7];
    gold = [1, 0.843, 0];
    blue = [0, 0, 1];

    module mounting_holes() {
        linear_extrude(height=28byj48_mount_bracket_height) {
        difference() {
            hull() {
                translate([28byj48_mount_center_offset, 0]) {
                    circle(r=28byj48_mount_outer_radius);
                }
                translate([-28byj48_mount_center_offset, 0]) {
                    circle(r=28byj48_mount_outer_radius);
                }
            }

            translate([28byj48_mount_center_offset, 0]) {
                circle(r=28byj48_mount_hole_radius);
            }
            translate([-28byj48_mount_center_offset, 0]) {
                circle(r=28byj48_mount_hole_radius);
            }
        }
        }
    }

    module shaft_collar() {
        cylinder(r=28byj48_shaft_collar_radius, h=28byj48_shaft_collar_height + eps);
    }

    module shaft() {
        color(gold) {
            difference() {
                cylinder(r=28byj48_shaft_radius, h=28byj48_shaft_height);
                translate([-(28byj48_shaft_radius + eps), 28byj48_shaft_slotted_width / 2, 28byj48_shaft_height - 28byj48_shaft_slotted_height]) {
                    cube([(28byj48_shaft_radius+eps)*2, 28byj48_shaft_radius, 28byj48_shaft_slotted_height+eps]);
                }
                translate([-(28byj48_shaft_radius + eps), - 28byj48_shaft_radius - (28byj48_shaft_slotted_width / 2), 28byj48_shaft_height - 28byj48_shaft_slotted_height]) {
                    cube([(28byj48_shaft_radius+eps)*2, 28byj48_shaft_radius, 28byj48_shaft_slotted_height+eps]);
                }
            }
        }
    }

    module chassis() {
        color(gray) {
            translate([0, 0, -28byj48_chassis_height]) {
                cylinder(r=28byj48_chassis_radius, h=28byj48_chassis_height);
            }
            translate([0, 0, -28byj48_mount_bracket_height]) {
                mounting_holes();
            }
            translate([0, 28byj48_shaft_offset, -eps]) {
                shaft_collar();
            }
        }
    }

    module backpack() {
        color(blue) {
            translate([-28byj48_backpack_width/2, 0, -28byj48_backpack_height - eps]) {
                cube([28byj48_backpack_width, 28byj48_backpack_extent, 28byj48_backpack_height]);
            }
        }
    }

    translate([0, -28byj48_shaft_offset, 0]) {
        chassis();
        translate([0, 28byj48_shaft_offset, 0]) {
            shaft();
        }
        translate([0, -28byj48_backpack_extent, 0]) {
            backpack();
        }
    }
}
