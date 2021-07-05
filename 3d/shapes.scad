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

module rounded_square(size, center=false, r=0.0, corners=[0,1,2,3], $fn=$fn) {
    translation_matrix = [
        [-1, -1],
        [-1,  1],
        [ 1,  1],
        [ 1, -1],
    ];

    module add_corner(index, center, r) {
        // this is tiny because the geometry is 'hull'ed and we need to get out
        // of the way of the rounds at larger radii
        square_size = 0.001;

        function circle_offset(axis) =
            center[axis] * translation_matrix[index][axis];

        function square_offset(axis) = 
            center[axis] * translation_matrix[index][axis] + r * translation_matrix[index][axis] + (translation_matrix[index][axis] >= 1 ? -square_size : 0);

        if(len(search(index, corners)) > 0)
            translate([circle_offset(0), circle_offset(1)])
                circle(r=r, $fn=$fn);
        else {
            translate([square_offset(0), square_offset(1)])
                square(square_size);
        }
    }

    width  = size[0] == undef ? size : size[0];  // unpack vector if present
    height = size[1] == undef ? size : size[1];

    if(r <= 0.0) {
        square([width, height], center=center);
    } else {
        radius = min(min(r, width/2), height/2);  // radius cannot be larger than rectangle
        center_x = center ? 0 : width/2;
        center_y = center ? 0 : height/2;

        translate([center_x, center_y]) {
            hull() {
                x =  width/2 - radius;
                y = height/2 - radius;
                for(i = [0:3])
                    add_corner(i, [x, y], radius);
            }
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

// positioned where origin is at the point to be rounded (first quadrant)
module fillet_tool(r, overlap=0.01, $fn=$fn) {
    tool_size = r + overlap;
    translate([r, r, 0])
        mirror([1, 1, 0])
            difference() {
                square([tool_size, tool_size]);
                circle(r=r, $fn=$fn);
            }
}

module fillet_tool_3d(r, r_corner=undef, overlap=0.01, additive=false, $fn=$fn) {
    corner_radius = (r_corner == undef) ? r : r_corner;  // use fillet radius if corner radius not provided
    translate([corner_radius, corner_radius, 0])
        rotate([0, 180, 90])
            rotate_extrude(angle=90, convexity=10, $fn=$fn)
                translate([corner_radius, 0, 0])
                    rotate([0, 0, additive ? -90 : 90])
                        fillet_tool(r, overlap=overlap, $fn=$fn);
}

module square_fillet_3d(size, r, r_corner=0.0, overlap=0.01, center=false, additive=false, corners=[0,1,2,3], edges=[0,1,2,3], $fn=$fn) {
    function in_list(x, list) = len(search(x, list)) > 0 ? true : false;

    width  = size[0] == undef ? size : size[0];  // unpack vector if present
    length = size[1] == undef ? size : size[1];

    max_radius = min(width, length)/2;  // largest radius possible along XY

    // Both radius and corner radius can be any value between 0 and the shortest
    // edge of the rectangle / 2 (maximum radius for full round).
    
    // If radius is zero, no geometry is generated (no radius = no fillet)
    // If corner radius is zero, corner rounding geometry is not generated
    radius = (r >= max_radius) ? max_radius : r;

    // if present, corner radius cannot be less than fillet radius (invalid geometry)
    corner_radius_inter = (r_corner != 0 && r_corner < radius) ? radius : r_corner;

    // corner radius cannot be greater than the max radius
    corner_radius = min(corner_radius_inter, max_radius);

    function corner_offset_single(x) = in_list(x % 4, corners) ? corner_radius : -overlap;  // calculate the corner offset for a given corner index
    function corner_offset(x) = corner_offset_single(x) + corner_offset_single(x + 1);  // calculate the corner offset for a given edge

    if(radius > 0) {
        center_x = center ? -width/2 : 0;
        center_y = center ? -length/2 : 0;

        translate([center_x, center_y]) {
            union() {
                // Y Straight, Origin
                if(in_list(0, edges)) {
                    translate([0, length - corner_offset_single(1), 0])
                        rotate([0, additive ? 180 : 0, 0])
                            rotate([0, 90, 270])
                                linear_extrude(height=length - corner_offset(0))
                                    fillet_tool(radius, overlap=overlap, $fn=$fn);
                }

                // X Straight, At Length
                if(in_list(1, edges)) {
                    translate([width - corner_offset_single(2), length, 0])
                        rotate([additive ? 180 : 0, 0, 0])
                            rotate([0, 90, 180])
                                linear_extrude(height=width - corner_offset(1))
                                    fillet_tool(radius, overlap=overlap, $fn=$fn);
                }

                // Y Straight, At Width
                if(in_list(2, edges)) {
                    translate([width, corner_offset_single(3), 0])
                        rotate([0, additive ? 180 : 0, 0])
                            rotate([0, 90, 90])
                                linear_extrude(height=length - corner_offset(2))
                                    fillet_tool(radius, overlap=overlap, $fn=$fn);
                }

                // X Straight, Origin
                if(in_list(3, edges)) {
                    translate([corner_offset_single(0), 0, 0])
                        rotate([additive ? 180 : 0, 0, 0])
                            rotate([0, 90, 0])
                                linear_extrude(height=width - corner_offset(3))
                                    fillet_tool(radius, overlap=overlap, $fn=$fn);
                }

                if(corner_radius > 0) {
                    // Corner: Bottom Left
                    if(in_list(0, corners)) {
                        fillet_tool_3d(radius, corner_radius, overlap=overlap, additive=additive, $fn=$fn);
                    }

                    // Corner: Top Left
                    if(in_list(1, corners)) {
                        translate([0, length, 0])
                            rotate([0, 0, 270])
                                fillet_tool_3d(radius, corner_radius, overlap=overlap, additive=additive, $fn=$fn);
                    }

                    // Corner: Top Right
                    if(in_list(2, corners)) {
                    translate([width, length, 0])
                        rotate([0, 0, 180])
                            fillet_tool_3d(radius, corner_radius, overlap=overlap, additive=additive, $fn=$fn);
                    }

                    // Corner: Bottom Right
                    if(in_list(3, corners)) {
                        translate([width, 0, 0])
                            rotate([0, 0, 90])
                                fillet_tool_3d(radius, corner_radius, overlap=overlap, additive=additive, $fn=$fn);
                    }
                }
            }
        }
    }
}

module cone(angle, base, height=undef, center=false, $fn=$fn) {
    // where:
    // * angle is the angle at the top of the cone, looking towards the base
    // * base is the diameter at the bottom of the cone
    // * height is the total height from base to top, if given

    base_radius = base/2;  // convert base diameter to radius
    full_height = base_radius / tan(angle/2);  // height to a point at the given angle

    // if no height is provided or height is out of scale, extrude up to a point
    if(height == undef || abs(height) >= full_height) {
        cylinder(h=full_height, r1=base_radius, r2=0, center=center);
    }
    // if height is provided, calculate top radius for given angle
    else {
        top_radius = tan(angle/2) * (full_height - height);
        cylinder(h=height, r1=base_radius, r2=top_radius, center=center);
    }
}
