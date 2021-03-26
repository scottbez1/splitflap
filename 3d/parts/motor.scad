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

include<../28byj-48.scad>;

motor_shaft_under_radius = 0.08;  // interference fit
motor_slop_radius = 3;
motor_mount_hole_radius = m4_hole_diameter/2;
motor_backpack_extent = 28byj48_backpack_extent + 2; // Add 2mm to make sure there's room for the wires
motor_hole_slop = 1;
motor_window_radius = 5;

// holes for 28byj-48 motor, centered around motor shaft
module motor_mount() {
    circle(r=28byj48_shaft_radius+motor_slop_radius, $fn=30);
    translate([-28byj48_mount_center_offset, -8]) {
        circle(r=motor_mount_hole_radius, $fn=30);
    }
    translate([28byj48_mount_center_offset, -8]) {
        circle(r=motor_mount_hole_radius, $fn=30);
    }

    hull() {
        x = -28byj48_chassis_radius - motor_hole_slop/2 + motor_window_radius;
        y = [-28byj48_shaft_offset - motor_backpack_extent - motor_hole_slop/2 + motor_window_radius,
            -28byj48_shaft_offset + 28byj48_chassis_radius + motor_hole_slop/2 - motor_window_radius];

        translate([ x, y[0], 0]) circle(r=motor_window_radius, $fn=40);
        translate([-x, y[1], 0]) circle(r=motor_window_radius, $fn=40);
        translate([-x, y[0], 0]) circle(r=motor_window_radius, $fn=40);
        translate([ x, y[1], 0]) circle(r=motor_window_radius, $fn=40);
    }
}

// double-flatted motor shaft of 28byj-48 motor (2D)
module motor_shaft() {
    union() {
        intersection() {
            circle(r=28byj48_shaft_radius-motor_shaft_under_radius, $fn=50);
            square([28byj48_shaft_radius*2, 3], center=true);
        }
        square([28byj48_shaft_radius/3, 28byj48_shaft_radius*4], center=true);
    }
}
