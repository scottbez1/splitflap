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

include <flap_dimensions.scad>
use <splitflap.scad>

num_flaps = 40;

flap_thickness_allowance = 1 - flap_thickness;  // fudged, so all flaps are considered 1 mm thick
cavity_top_allowance = 2;  // extra case height above the flaps

flap_clearance = 0.5;  // distance between the flap edges and the case walls
wall_thickness = 3.5;  // thickness of the case walls around the flaps
bottom_thickness = 5.0;  // thickness of the bottom of the case, below the flaps

thumb_hole_diameter = 20.0;  // diameter for the thumb hole at the bottom of the case
pinch_cutout_width = 25.0;  // width of the 'pinch' opening for grabbing flaps
pinch_cutout_offset = 12.5;  // offset from the bottom of the pinch hole to the bottom of the cavity


// Calculated Values
eps = 0.01;  // extra distance for through geometry

cavity_height = num_flaps * (flap_thickness + flap_thickness_allowance) + cavity_top_allowance;

case_height = bottom_thickness + cavity_height;
case_width = flap_width + flap_clearance * 2 + wall_thickness * 2;
case_length = flap_height + flap_clearance * 2 + wall_thickness * 2;

pinch_cutout_height = cavity_height - pinch_cutout_offset;  // total height of the cutout
pinch_cutout_top_offset = case_height - cavity_height + pinch_cutout_offset + pinch_cutout_width/2;  // top of the case to the center of the pinch cutout circle

module flap_position() {
    translate([-flap_width/2, -(flap_height - flap_pin_width)/2, 0])
    children();
}

module case_body() {
    linear_extrude(height = case_height)
    flap_position()
    offset(delta = wall_thickness + flap_clearance)
    flap_2d(cut_tabs = false);
}

module flap_cavity() {
    translate([0, 0, case_height - cavity_height])
    linear_extrude(height = cavity_height + eps)
    flap_position()
    offset(delta = flap_clearance)
    flap_2d();
}

module thumb_hole() {
    translate([0, 0, -eps])
    linear_extrude(bottom_thickness + eps*2)
    circle(r=thumb_hole_diameter/2, $fn=100);
}

module pinch_hole() {
    rotate([90, 0, 0])
    translate([0, pinch_cutout_top_offset , -case_length/2 - eps])
    linear_extrude(height=case_length + eps*2)
    union() {
        translate([-pinch_cutout_width/2, 0, 0])
        square([pinch_cutout_width, pinch_cutout_height - pinch_cutout_width/2 + eps]);
        circle(r=pinch_cutout_width/2, $fn=100);
    }
}


difference() {
    case_body();
    flap_cavity();
    thumb_hole();
    pinch_hole();
}
