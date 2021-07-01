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
        bolt_negative_scale = [1.2, 1.2, 1];
        bolt_negative_length = 13;

        scale(bolt_negative_scale)
            standard_m4_bolt(captive_nut_inset, bolt_negative_length);
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

    boss_width = mounting_bracket_width(side_clearance) - 2;
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


mounting_bracket_t_slot();
