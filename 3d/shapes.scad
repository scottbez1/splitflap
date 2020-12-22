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
    width  = size[0] == undef ? size : size[0];  // unpack vector if present
    height = size[1] == undef ? size : size[1];

    pts = [
        [      0,      0],
        [width/2, height],
        [  width,      0],
    ];

    x_offset = center ?  -width/2 : 0;
    y_offset = center ? -height/2 : 0;
    translate([x_offset, y_offset])
        polygon(points=pts, convexity=1);
}

module arrow(size, aspect=[0.5, 0.3], center=false) {
    function unpack(val, pos) = val[pos] == undef ? val : val[pos];  // use vector if possible, value otherwise
    function recenter(vect, pos) = center ? -vect[pos]/2 : 0;  // if 'center', return negative vector position/2

    size  = [   unpack(size, 0),   unpack(size, 1) ];  // overall bounding box
    ratio = [ unpack(aspect, 0), unpack(aspect, 1) ];  // ratio between head and body, 1.0 max

    head = [                  size[0], size[1] * ratio[1] ];  // head bounding box
    base = [ size[0] * (1 - ratio[0]), size[1] - head[1]  ];  // base bounding box

    translate([recenter(size, 0), recenter(size, 1)]) {
        translate([(size[0] - base[0]) / 2, 0])
            square(base);
        translate([0, base[1]])
            triangle(head);
    }
}
