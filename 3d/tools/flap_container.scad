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

include <../flap_dimensions.scad>
use <../splitflap.scad>

num_flaps = 40;
containers_x = 1;
containers_y = 1;

flap_thickness_allowance = 1.1 - flap_thickness;  // fudged, so all flaps are considered 1.1 mm thick
cavity_top_allowance = 2;  // extra case height above the flaps

flap_clearance = 0.5;  // distance between the flap edges and the case walls
wall_thickness = 3.5;  // thickness of the case walls around the flaps
bottom_thickness = 5.0;  // thickness of the bottom of the case, below the flaps

thumb_hole_diameter = 20.0;  // diameter for the thumb hole at the bottom of the case
pinch_cutout_width = 25.0;  // width of the 'pinch' opening for grabbing flaps
pinch_cutout_offset = 12.5;  // offset from the bottom of the pinch hole to the bottom of the cavity

band_slot_diameter = 10.0;  // diameter of the slot for the retaining band
band_slot_depth = 2.5;  // depth of the retaining band slot, measured from the base to the peak

fillet_case_corners = 3.0;  // bottom outside corner fillet
fillet_flap_notch = 1.0;  // inside of flap notches, in cavity
fillet_pinch_top = 5.0;  // at the top of the case, where the pinch cutout starts
fillet_thumb_hole = 1.0;  // 3D fillet on the top inside of the thumb hole


// Calculated Values
eps = 0.01;  // extra distance for through geometry

cavity_height = num_flaps * (flap_thickness + flap_thickness_allowance) + cavity_top_allowance;

case_height = bottom_thickness + cavity_height;
case_width = flap_width + flap_clearance * 2 + wall_thickness * 2;
case_length = flap_height + flap_clearance * 2 + wall_thickness * 2;

pinch_cutout_height = cavity_height - pinch_cutout_offset;  // total height of the cutout
pinch_cutout_top_offset = case_height - cavity_height + pinch_cutout_offset + pinch_cutout_width/2;  // top of the case to the center of the pinch cutout circle

module fillet_tool(radius) {
    difference() {
        translate([-eps, -eps])
            square(radius + eps);
        translate([radius, radius, 0])
            circle(r=radius, $fn=100);
    }
}

module flap_position() {
    translate([-flap_width/2, -(flap_height - flap_pin_width)/2, 0])
    children();
}

module case_body() {
    linear_extrude(height = case_height)
    flap_position() {
        difference() {
            diff = wall_thickness + flap_clearance;
            offset(delta = diff)
                flap_2d(cut_tabs = false);

            translate([0, -flap_pin_width/2]) {  // re-centering on bottom edge
                 // bottom left corner fillet
                translate([-diff, -diff])
                    fillet_tool(fillet_case_corners);

                // bottom right corner fillet
                translate([-diff + case_width, -diff])
                    mirror([1, 0, 0])
                    fillet_tool(fillet_case_corners);
            }
        }
    }
}

module flap_cavity() {
    module notch_fillet() {
        mirror([1,0,0])
        translate([flap_clearance - flap_notch_depth, flap_clearance + flap_pin_width/2])
        intersection() {
            square([flap_notch_depth, flap_notch_height_default]);  // limit to size of notch
            fillet_tool(fillet_flap_notch);
        }
    }

    module notch_top() {
        mirror([0, 1, 0])
        translate([0, -flap_pin_width - flap_notch_height_default])
        children();
    }

    module notch_right() {
        translate([flap_width, 0, 0])
        mirror([1, 0, 0])
        children();
    }
 
    translate([0, 0, case_height - cavity_height])
    linear_extrude(height = cavity_height + eps)
    flap_position() {
        offset(delta = flap_clearance)
            flap_2d();

        // left, notch bottom
        notch_fillet();

        // left, notch top
        notch_top()
            notch_fillet();

        // right, notch bottom
        notch_right()
            notch_fillet();

        notch_right()
            notch_top()
                notch_fillet();
    }
}

module thumb_hole() {
    translate([0, 0, -eps])
    linear_extrude(bottom_thickness + eps*2)
        circle(r=thumb_hole_diameter/2, $fn=100);
}

module thumb_hole_fillet() {
    translate([0, 0, bottom_thickness]) {
        rotate_extrude($fn = 100)
            translate([thumb_hole_diameter/2, 0, 0])
                rotate([180, 0, 0])
                fillet_tool(fillet_thumb_hole);
    }
}

module pinch_hole() {
    rotate([90, 0, 0])
    translate([0, pinch_cutout_top_offset , -case_length/2 - eps])
    linear_extrude(height=case_length + eps*2)
    union() {
        translate([-pinch_cutout_width/2, 0, 0])
            square([pinch_cutout_width, pinch_cutout_height - pinch_cutout_width/2 + eps]);
        circle(r=pinch_cutout_width/2, $fn=100);
        
        // left top fillet
        translate([-pinch_cutout_width/2, pinch_cutout_height - pinch_cutout_width/2,0])
            mirror([1, 1, 0])
                fillet_tool(fillet_pinch_top);

        // right top fillet
        translate([pinch_cutout_width/2, pinch_cutout_height - pinch_cutout_width/2,0])
            mirror([0, 1, 0])
                fillet_tool(fillet_pinch_top);
    }
}

module band_slot() {
    rotate([90, 0, 0])
    translate([0, band_slot_depth - band_slot_diameter/2, -case_length/2 - eps])
    linear_extrude(height=case_length + eps*2)
    circle(r=band_slot_diameter/2, $fn=60);
}

module flap_container() {
    difference() {
        case_body();
        flap_cavity();
        thumb_hole();
            thumb_hole_fillet();
        pinch_hole();
        band_slot();
    }
}

union() {
    for(x = [0 : containers_x - 1]) {
        translate([x * (case_width - wall_thickness), 0])
        for(y = [0 : containers_y - 1]) {
            translate([0, y * (case_length - wall_thickness), 0])
            flap_container();
        }
    }
}
