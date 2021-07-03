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

use<../splitflap.scad>;
use<../shapes.scad>;

include<../m4_dimensions.scad>;

// Constants from splitflap.scad, using functions to extract
thickness = thickness();
front_forward_offset = front_forward_offset();
enclosure_length_right = enclosure_length_right();
enclosure_height_lower = enclosure_height_lower();
enclosure_wall_to_wall_width = enclosure_wall_to_wall_width();
enclosure_vertical_inset = enclosure_vertical_inset();
captive_nut_inset = captive_nut_inset();
mounting_hole_inset = mounting_hole_inset();
side_tab_width = side_tab_width();

eps = 0.01;

module position_mounting_bracket() {
    translate([thickness, -enclosure_length_right + front_forward_offset, -enclosure_height_lower]) {
        children();
    }
}

function mounting_bracket_width(side_clearance) = enclosure_wall_to_wall_width - 2 * thickness - 2 * side_clearance;
function mounting_bracket_length(front_clearance) = enclosure_length_right - front_clearance; 
function mounting_bracket_height(height_clearance) = enclosure_vertical_inset - height_clearance;

module mounting_bracket_2d(hole_diameter, clearance) {
    function unpack(val, pos) = (val[pos] == undef) ? 0 : val[pos];  // use vector if possible, 0 otherwise

    side_clearance  = unpack(clearance, 0);  // X clearance between the mount and the sides of the enclosure (each side)
    front_clearance = unpack(clearance, 1);  // Y clearance between the mount and the front of the enclosure

    difference() {
        // base geometry
        translate([side_clearance, 0, 0])
            square([mounting_bracket_width(side_clearance), mounting_bracket_length(front_clearance)]);

        // mounting hole
        translate([(enclosure_wall_to_wall_width - 2 * thickness)/2, mounting_hole_inset]) {
            circle(r=hole_diameter/2, $fn=30);
        }
    }
}

module mounting_bracket_base(hole_diameter=m4_hole_diameter, clearance=[0.1, 0.1, 0.2]) {
    module bolt_negative() {
        cutout_negative_scale = [1.2, 1.2, 1];  // scale factors for the increased sizes
        bolt_negative_length = 13;  // length of the bolt cutout
        nut_negative_excess_thickness = 1;  // extra thickness around the nut negative

        union() {
            scale(cutout_negative_scale) {
                // bolt negative
                standard_m4_bolt(bolt_length=bolt_negative_length);
            
                // nut negative
                translate([0, 0, captive_nut_inset - nut_negative_excess_thickness/2])
                    // note that we're using projection and linear extrusion here so
                    // that we can center the extra thickness on both sides of the nut,
                    // rather than putting it all just on one side as with 'scale()'
                    linear_extrude(height=m4_nut_length + nut_negative_excess_thickness)
                        projection()
                            standard_m4_nut(hole=false);
            }
        }
    }

    function unpack(val, pos) = (val[pos] == undef) ? 0 : val[pos];  // use vector if possible, 0 otherwise
    height_clearance = unpack(clearance, 2);  // Z clearance between the mount and the 'bottom' piece of the enclosure

    difference() {
        // base geometry, 3D
        linear_extrude(height=enclosure_vertical_inset - height_clearance, convexity=10)
            mounting_bracket_2d(hole_diameter, clearance);

        bolt_y_offset = enclosure_length_right - (thickness + 1.5 * side_tab_width);
        bolt_z_offset = enclosure_vertical_inset + thickness/2;

        // left side bolt
        translate([-thickness + enclosure_wall_to_wall_width, bolt_y_offset, bolt_z_offset])
            rotate([180, 90, 0])
                bolt_negative();

        // right side bolt
        translate([-thickness, bolt_y_offset, bolt_z_offset])
            rotate([0, 90, 0])
                bolt_negative();

        // front bolt
        translate([enclosure_wall_to_wall_width/2 - thickness, enclosure_length_right + thickness, bolt_z_offset]) {
            rotate([0, 90, -90])
                bolt_negative();
        }
    }
}

module mounting_bracket_t_slot(hole_diameter=m4_hole_diameter, slot_width=6, slot_depth=5, nut_length=12.5, clearance=[0.1, 0.1, 0.2]) {
    function unpack(val, pos) = (val[pos] == undef) ? 0 : val[pos];  // use vector if possible, 0 otherwise

    side_clearance  = unpack(clearance, 0);  // X clearance between the mount and the sides of the enclosure (each side)

    boss_width = mounting_bracket_width(side_clearance) - 2;  // arbitrary, giving it some breathing room
    fillet_radius = slot_width - 1;

    union() {
        // 3D base geometry
        mounting_bracket_base(hole_diameter, clearance);

        difference() {
            // t-slot boss
            mirror([0, 0, 1])
                difference() {
                    linear_extrude(height=slot_depth)
                        difference() {
                            // base geometry
                            translate([(enclosure_wall_to_wall_width - 2 * thickness)/2, mounting_hole_inset])
                                rounded_square([boss_width, slot_width], center=true, r=fillet_radius, $fn=60);

                            // t-nut subtraction
                            translate([(enclosure_wall_to_wall_width - 2 * thickness)/2, mounting_hole_inset])
                                square([nut_length, slot_width + eps], center=true);
                        }
                }

            // rounded fillet
            translate([(enclosure_wall_to_wall_width - 2 * thickness)/2, mounting_hole_inset])
                translate([0, 0, -slot_depth])
                    mirror([0, 0, 1])
                        square_fillet_3d([boss_width, slot_width], r=fillet_radius/2, r_corner=fillet_radius, center=true, $fn=60);
        }
    }
}


module mounting_bracket_screw_holes(hole_diameter=m4_hole_diameter, screw_diameter=m4_hole_diameter, num_screws=2, screw_clearance=4, screw_inset=8, csk=9, csk_angle=82, r=2.5, clearance=[0.1, 0.1, 0.2]) {
    function unpack(val, pos) = (val[pos] == undef) ? 0 : val[pos];  // use vector if possible, 0 otherwise

    side_clearance  = unpack(clearance, 0);  // X clearance between the mount and the sides of the enclosure (each side)
    height_clearance = unpack(clearance, 2);  // Z clearance between the mount and the 'bottom' piece of the enclosure

    width = mounting_bracket_width(side_clearance);  // width of the mount
    height = mounting_bracket_height(height_clearance) + thickness + height_clearance;  // height of the screw area on the mount

    screw_area_radius = r;  // radius for the fillet on the screw area extension
    screw_area_depth = screw_diameter + 2*screw_clearance + screw_area_radius;  // Y distance of the screw area extension

    screw_left_inset = max(screw_inset, screw_clearance) + screw_diameter/2;  // inset from edge of the mount to the edge of the hole
    screw_area_width = (width - 2*screw_left_inset);  // area for screws to be placed in (accounting for diameter and inset)
    screw_spacing = (num_screws > 1) ? screw_area_width / (num_screws - 1) : screw_area_width/2;  // spacing between screws, on center

    module screw_hole() {
        union() {
            // holes
            linear_extrude(height=height + 2*eps)
                circle(r=screw_diameter/2, $fn=60);

            // countersinks
            if(csk > 0) {
                translate([0, 0, height + 2*eps])
                    rotate([0, 180, 0])
                        cone(angle=csk_angle, base=csk, $fn=60);
            }
        }
    }

    union() {
        // 3D base geometry
        mounting_bracket_base(hole_diameter, clearance);

        // screw area boss
        translate([0, -screw_area_depth, 0]) {
            difference() {
                // boss geometry
                linear_extrude(height=height)
                    square([width, screw_area_depth]);

                // rounded edge
                if(screw_area_radius > 0) {
                    translate([-eps, 0, height])
                        rotate([0, 90, 0])
                            linear_extrude(height=width + 2*eps)
                                fillet_tool(r=screw_area_radius, $fn=60);
                }

                // screw holes
                translate([screw_left_inset, screw_area_depth/2 + screw_area_radius/2, -eps]) {
                    if(num_screws == 1) {
                        translate([screw_spacing, 0, 0])
                            screw_hole();
                    }
                    else if(num_screws > 1) {
                        for ( i = [0 : num_screws - 1] ) {
                            translate([screw_spacing * i, 0, 0])
                                screw_hole();
                        }
                    }
                }
            }
        }
    }
}
