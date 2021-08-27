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

module flap_spool(flaps, flap_hole_radius, flap_hole_separation, outset, height) {
    pitch_radius = flap_spool_pitch_radius(flaps, flap_hole_radius, flap_hole_separation);
    outer_radius = flap_spool_outer_radius(flaps, flap_hole_radius, flap_hole_separation, outset);


    module flap_spool_2d() {
        difference() {
            circle(r=outer_radius, $fn=60);
            for (i = [0 : flaps - 1]) {
                translate([cos(360/flaps*i)*pitch_radius, sin(360/flaps*i)*pitch_radius]) circle(r=flap_hole_radius, $fn=15);
            }
        }
    }

    if (height > 0) {
        linear_extrude(height) {
            flap_spool_2d();
        }
    } else {
        flap_spool_2d();
    }
}

module flap_spool_home_indicator(flaps, flap_hole_radius, flap_hole_separation, outset, height=0) {
    pitch_radius = flap_spool_pitch_radius(flaps, flap_hole_radius, flap_hole_separation);
    outer_radius = flap_spool_outer_radius(flaps, flap_hole_radius, flap_hole_separation, outset);

    module flap_spool_home_indicator_2d() {
        translate([pitch_radius -flap_hole_radius * 3, 0]) {
            hull() {
                circle(r=flap_hole_radius/2, $fn=30);
                translate([-flap_hole_radius * 1.25, 0]) {
                    circle(r=flap_hole_radius/2, $fn=30);
                }
            }
        }
    }

    if (height > 0) {
        // convexity parameter fixes 'difference()' face polarity in preview mode... somehow
        linear_extrude(height, convexity=2)
            flap_spool_home_indicator_2d();
    } else {
        flap_spool_home_indicator_2d();
    }
}

function flap_spool_pitch_radius(flaps, flap_hole_radius, separation) = 
    flaps * (flap_hole_radius*2 + separation) / (2*PI);

function flap_spool_outer_radius(flaps, flap_hole_radius, separation, outset) = 
    flap_spool_pitch_radius(flaps, flap_hole_radius, separation) + flap_hole_radius + outset;


flap_spool(40, 1.5, 5, 3, 3.2);
