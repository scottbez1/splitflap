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

module rounded_square(size, center=false, r=0.0, $fn=$fn) {
    width  = size[0] == undef ? size : size[0];  // unpack vector if present
    height = size[1] == undef ? size : size[1];

    if(r <= 0.0) {
        square([width, height], center=center);
    } else {
        radius = min(min(r, width/2), height/2);  // radius cannot be larger than rectangle
        center_x = center ? 0 : width/2;
        center_y = center ? 0 : height/2;

        translate([center_x, center_y])
            hull() {
                x =  width/2 - radius;
                y = height/2 - radius;

                translate([ x,  y]) circle(r=radius, $fn=$fn);
                translate([ x, -y]) circle(r=radius, $fn=$fn);
                translate([-x, -y]) circle(r=radius, $fn=$fn);
                translate([-x,  y]) circle(r=radius, $fn=$fn);
            }
    }
}

module triangle(size, center=false) {
    function tri_height(a, c) = sqrt(c*c - a*a);  // pythagoras

    width  = size[0] == undef ? size : size[0];  // unpack vector if present
    height = size[1] == undef ? tri_height(width/2, width) : size[1];  // calculate height with trig if only width provided
    angle = atan(height/(width/2));  // angle in left corner, degrees

    pts = [
        [      0,      0],
        [width/2, height],
        [  width,      0],
    ];

    x_offset = center ?  -width/2 : 0;
    y_offset = center ? -tan(angle/2) * width/2 : 0;  // opposite edge of right triangle using bisected angle from left corner
    translate([x_offset, y_offset])
        polygon(points=pts, convexity=1);
}
