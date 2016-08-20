/*
   Copyright 2015-2016 Scott Bezek and the splitflap contributors

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

// Basic 28BYJ-48 model based on various dimensioned drawings.
// Origin is centered on the shaft, on the front face of the motor, with the shaft in the +z direction
module Stepper28BYJ48() {
    $fn = 60;

    eps = 0.01;
    chassis_radius = 28/2;
    chassis_height = 19;
    shaft_offset = 8;
    mount_outer_radius = 3.5;
    mount_hole_radius = 4.2/2;
    mount_center_offset = 35/2;
    mount_bracket_height = 0.8;

    shaft_collar_radius = 9/2;
    shaft_collar_height = 1.5;

    shaft_radius = 5/2;
    shaft_height = 10;
    shaft_slotted_width = 3;
    shaft_slotted_height = 6;

    backpack_width = 14.6;
    backpack_extent = 18; // seen values from 17-18
    backpack_height = 16;

    gray = [0.7, 0.7, 0.7];
    gold = [1, 0.843, 0];
    blue = [0, 0, 1];

    module mounting_holes() {
        linear_extrude(height=mount_bracket_height) {
        difference() {
            hull() {
                translate([mount_center_offset, 0]) {
                    circle(r=mount_outer_radius);
                }
                translate([-mount_center_offset, 0]) {
                    circle(r=mount_outer_radius);
                }
            }

            translate([mount_center_offset, 0]) {
                circle(r=mount_hole_radius);
            }
            translate([-mount_center_offset, 0]) {
                circle(r=mount_hole_radius);
            }
        }
        }
    }

    module shaft_collar() {
        cylinder(r=shaft_collar_radius, h=shaft_collar_height + eps);
    }

    module shaft() {
        color(gold) {
            difference() {
                cylinder(r=shaft_radius, h=shaft_height);
                translate([-(shaft_radius + eps), shaft_slotted_width / 2, shaft_height - shaft_slotted_height]) {
                    cube([(shaft_radius+eps)*2, shaft_radius, shaft_slotted_height+eps]);
                }
                translate([-(shaft_radius + eps), - shaft_radius - (shaft_slotted_width / 2), shaft_height - shaft_slotted_height]) {
                    cube([(shaft_radius+eps)*2, shaft_radius, shaft_slotted_height+eps]);
                }
            }
        }
    }

    module chassis() {
        color(gray) {
            translate([0, 0, -chassis_height]) {
                cylinder(r=chassis_radius, h=chassis_height);
            }
            translate([0, 0, -mount_bracket_height]) {
                mounting_holes();
            }
            translate([0, shaft_offset, -eps]) {
                shaft_collar();
            }
        }
    }

    module backpack() {
        color(blue) {
            translate([-backpack_width/2, 0, -backpack_height - eps]) {
                cube([backpack_width, backpack_extent, backpack_height]);
            }
        }
    }

    translate([0, -shaft_offset, 0]) {
        chassis();
        translate([0, shaft_offset, 0]) {
            shaft();
        }
        translate([0, -backpack_extent, 0]) {
            backpack();
        }
    }
}

Stepper28BYJ48();

