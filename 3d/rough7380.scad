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

module roughM4_7380(length) {
    eps = 0.1;
    thread_diameter = 4;
    head_diameter = 7.6;
    head_length = 2.2;
    driving_depth = 1.3;
    driving_apothem = 2.5/2;
    driving_radius = driving_apothem / cos(180/6);

    sphere_offset = 1.2;  // Just a guess
    sphere_radius = sqrt(sphere_offset*sphere_offset + (head_diameter/2)*(head_diameter/2));


    difference() {
        union() {
            translate([0, 0, -eps]) {
                cylinder(h=length+eps, d=thread_diameter, $fn=30);
            }
            difference() {
                translate([0, 0, sphere_offset]) {
                    sphere(sphere_radius, $fn=30);
                }
                translate([0, 0, sphere_radius]) {
                    cube([sphere_radius*2 + eps, sphere_radius*2 + eps, sphere_radius*2 + eps], center=true);
                }
                translate([0, 0, -sphere_radius - head_length]) {
                    cube([sphere_radius*2 + eps, sphere_radius*2 + eps, sphere_radius*2 + eps], center=true);
                }
            }
        }
        translate([0, 0, -head_length-eps]) {
            cylinder(h=driving_depth+eps, r=driving_radius, $fn=6);
        }
    }
}

roughM4_7380(12);
