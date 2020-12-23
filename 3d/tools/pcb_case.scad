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

include<../pcb.scad>;

render_pcb = false;  // draw PCB in position in case
cases_per_row = 1;  // number of cases per each row
dual_rows = false;  // whether to put mirrored cases on the opposite side

case_wall_thickness = 3;  // thickness of the case walls around the PCB, XY
case_bottom_thickness = 3;  // thickness of the case below the PCB, Z
case_fillet_radius = 3;  // radius for the corner fillets on the outside of the case

pcb_edge_clearance = 0.5;  // distance between the PCB and the case, XY
pcb_depth_clearance = 2;  // distance above the PCB in the case (including the connector)
pcb_hole_clearance = 0;  // clearance between the hole boss and the PCB hole edge, XY

hall_effect_x_clearance = 1;  // distance between the hall effect sensor and the pocket walls, along X (less variability)
hall_effect_y_clearance = 2;  // distance between the hall effect sensor and the pocket walls, along Y (more variability)
hall_effect_pocket_depth = 8;  // depth of the hall effect pocket from the cutting plane
hall_effect_pocket_radius = 1;  // radius for the rounded corners of the hall effect pocket

connector_body_clearance = 0.5;  // distance between the connector and the case, XY
connector_body_z_offset = pcb_thickness - 0.5;  // distance between the connector body and the case, Z
connector_body_fillet = 3;  // rounded fillet on the connector body opening
connector_pin_clearance = 1;  // distance between the connector pins and the cutout walls, XY
connector_pin_depth = 2.5;  // depth of the connector pin cutout

mounting_boss_height = pcb_thickness + 0.4;  // distance above the PCB for the mounting hole boss


// calculations
eps = 0.01;
pcb_to_spool = 7.8;  // dummy for PCB render, as we don't have the whole assembly for this value

combined_clearance = pcb_edge_clearance + case_wall_thickness;  // total edge clearance, from PCB edge to outside edge
pcb_cutout_depth = pcb_thickness + pcb_connector_height/2 + pcb_depth_clearance;  // depth of the PCB cutout, from top

case_width = pcb_length + combined_clearance * 2;  // total case width, along X
case_length = pcb_height + combined_clearance * 2;  // total case length, along Y
case_height = case_bottom_thickness + hall_effect_pocket_depth + pcb_cutout_depth;  // total case height, along Z

case_x_offset = pcb_edge_to_hole_x + combined_clearance;  // x offset from the origin to the left corner
case_y_offset = case_width - pcb_edge_to_hole_y - combined_clearance;  // y offset from the origin to the left corner

pcb_cutout_plane = case_height - pcb_cutout_depth;  // PCB cutout Z position, from bottom (origin)
mounting_boss_radius = m4_hole_diameter/2 - pcb_hole_clearance;  // radius for the extruded feature through the mounting hole
mounting_sphere_z = pcb_cutout_plane + pcb_thickness + mounting_boss_height - mounting_boss_radius;  // Z position of the mounting sphere boss
connector_cutout_depth = pcb_cutout_depth - connector_body_z_offset;  // cutout depth of the connector, from top


module fillet_tool(radius) {
    difference() {
        translate([-eps, -eps])
            square(radius + eps);
        translate([radius, radius, 0])
            circle(r=radius, $fn=100);
    }
}

module fillet_case(w, h) {
    translate([0, 0, -eps])
    linear_extrude(case_height + eps*2) {
        // bottom left
        fillet_tool(case_fillet_radius);

        // top left
        translate([0, h])
            mirror([0, 1, 0])
                fillet_tool(case_fillet_radius);

        translate([w, 0]) {
            mirror([1, 0, 0]) {
                // bottom right
                fillet_tool(case_fillet_radius);
                
                // top right
                translate([0, h])
                    mirror([0, 1, 0])
                        fillet_tool(case_fillet_radius);
            }
        }
    }
}

module mounting_boss() {
    union() {
        translate([0, 0, mounting_sphere_z])
            sphere(r=mounting_boss_radius, $fn=30);

        // if we need to make a column to extend it out
        if(mounting_sphere_z > pcb_cutout_plane) {
            translate([0, 0, pcb_cutout_plane])
                linear_extrude(height=mounting_sphere_z - pcb_cutout_plane)
                    circle(r=mounting_boss_radius, $fn=30);
        }
    }
}

module connector_pin_pocket() {
    pocket_radius = (pcb_connector_pin_width * sqrt(2)) / 2 + connector_pin_clearance;  // square pin inscribed inside circle, plus clearance

    translate([0, 0, pcb_cutout_plane - connector_pin_depth])
    linear_extrude(height=connector_pin_depth + eps)
        translate([pcb_hole_to_connector_pin_1_x, -pcb_hole_to_connector_pin_1_y, 0]) {
            hull() { 
                for(i = [0 : 2]) {
                    translate([-connector_pin_pitch * i, 0, 0])
                        circle(r=pocket_radius, $fn=30);
                }
            }
        }
}

module connector_body_cutout() {
    // translation offsets from origin to front left corner of connector body
    x_offset = pcb_hole_to_connector_pin_1_x - connector_pin_pitch - pcb_connector_width/2;
    y_offset = -pcb_hole_to_connector_pin_1_y - pcb_connector_length;
    z_offset = case_height - connector_cutout_depth;

    // calculation for right fillet
    right_edge = x_offset + pcb_connector_width + connector_body_clearance;  // X coordinate from left edge to right edge of cutout
    right_gap = (case_width - case_x_offset) - case_wall_thickness - right_edge;  // X distance between right edge of connector and start of right case wall
    right_radius = min(connector_body_fillet, right_gap - eps);  // largest fillet that fits in the space

    module connector_fillet(radius) {
        translate([0, 0, z_offset + connector_cutout_depth])
            translate([x_offset - connector_body_clearance, y_offset - connector_body_clearance, 0])
                mirror([1, 0, 0])  // fillet facing case edge
                rotate([-90, 0, 0])  // extruding towards case center
                linear_extrude(height=pcb_connector_length + connector_body_clearance*2)
                    fillet_tool(radius);
    }

    union() {
        // main body
        translate([0, 0, z_offset])
            linear_extrude(height=connector_cutout_depth + eps)
            offset(delta=connector_body_clearance)
                translate([x_offset, y_offset, pcb_thickness])
                square([pcb_connector_width, pcb_connector_length]);
        
        // left fillet
        connector_fillet(connector_body_fillet);

        // right fillet
        translate([(x_offset - connector_body_clearance)*2 + pcb_connector_width + connector_body_clearance*2, 0])
            mirror([1, 0, 0])
                connector_fillet(right_radius);
    }
}

module hall_effect_pocket() {
    pocket_size_x = hall_effect_width + hall_effect_x_clearance * 2;
    pocket_size_y = hall_effect_height + pcb_sensor_pin_width/2 + hall_effect_y_clearance * 2;

    pocket_offset_x = (pocket_size_x / 2) - hall_effect_pocket_radius;
    pocket_offset_y = (pocket_size_y / 2) - hall_effect_pocket_radius;

    pocket_offset_z = case_height - pcb_cutout_depth - hall_effect_pocket_depth;
    pocket_depth = pcb_cutout_depth + hall_effect_pocket_depth;

    translate([0, 0, pocket_offset_z])
        linear_extrude(height=pocket_depth + eps)
            translate([pcb_hole_to_sensor_pin_1_x - sensor_pin_pitch, pcb_hole_to_sensor_pin_1_y + hall_effect_height/2 - pcb_sensor_pin_width/4, 0]) {
                // filleted version, using 'hull' and X/Y offsets based on pocket size
                if(hall_effect_pocket_radius > eps) {
                    hull() {
                        translate([-pocket_offset_x, pocket_offset_y])
                            circle(r=hall_effect_pocket_radius, $fn=30);
                        translate([pocket_offset_x, pocket_offset_y])
                            circle(r=hall_effect_pocket_radius, $fn=30);
                        translate([-pocket_offset_x, -pocket_offset_y])
                            circle(r=hall_effect_pocket_radius, $fn=30);
                        translate([pocket_offset_x, -pocket_offset_y])
                            circle(r=hall_effect_pocket_radius, $fn=30);
                    }
                } else {
                    // non-filleted version, using a basic 'square' on center
                    square([pocket_size_x, pocket_size_y], center=true);
                }
            }
}

module pcb_case(with_pcb=false) {
    translate([case_x_offset, case_y_offset]) {
        union() {
            difference() {
                // case body
                linear_extrude(height=case_height)
                    offset(delta=combined_clearance)
                        pcb_outline_2d(hole=false);

                // PCB cutout
                translate([0, 0, pcb_cutout_plane])
                    linear_extrude(height=pcb_cutout_depth + eps)
                        offset(delta=pcb_edge_clearance)
                            pcb_outline_2d(hole=false);

                hall_effect_pocket();
                connector_pin_pocket();
                connector_body_cutout();
            }
            mounting_boss();
        }

        if(with_pcb) {
            translate([0, 0, pcb_cutout_plane])
                pcb(pcb_to_spool);
        }
    }
}

module pcb_case_multi(num=1) {
    union() {
        for(x = [0 : num - 1]) {
            translate([x * (case_width - case_wall_thickness), 0])
            pcb_case(render_pcb);
        }
    }
}

module pcb_case_complete(columns=1, dual=false) {
    multi_case_width = case_width*columns - case_wall_thickness * (columns - 1);
    multi_case_length = dual ? case_length * 2 : case_length;

    difference() {
        union() {
            pcb_case_multi(columns);
            if(dual) {
                translate([multi_case_width, multi_case_length, 0])
                rotate([0, 0, 180])
                pcb_case_multi(columns);
            }
        }
        fillet_case(multi_case_width, multi_case_length);
    }
}

pcb_case_complete(cases_per_row, dual_rows);
