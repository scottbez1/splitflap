/*
   Copyright 2015-2018 Scott Bezek and the splitflap contributors

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
include <m4_dimensions.scad>;
include <sensor_pcb_dimensions.scad>;


module pcb_outline_2d(hole=true) {
    difference() {
        translate([-pcb_edge_to_hole_x, -pcb_height + pcb_edge_to_hole_y]) {
            square([pcb_length, pcb_height]);
        }
        if(hole) {
            circle(r=m4_hole_diameter/2, $fn=30);
        }
    }
}

// 3D PCB module, origin at the center of the mounting hole on the bottom surface of the PCB
module pcb(pcb_to_spool, render_jig=false, jig_thickness=0) {
    color([0, 0.5, 0]) {
        linear_extrude(height=pcb_thickness) {
            pcb_outline_2d();
        }
    }

    // Connector
    color([0, 0, 0]) {
        translate([pcb_hole_to_connector_pin_1_x - connector_pin_pitch - pcb_connector_width/2, -pcb_hole_to_connector_pin_1_y - pcb_connector_length, pcb_thickness]) {
            cube([pcb_connector_width, pcb_connector_length, pcb_connector_height]);
        }
    }

    // Connector pins
    color([0.5, 0.5, 0.5]) {
        translate([pcb_hole_to_connector_pin_1_x - pcb_connector_pin_width/2, -pcb_hole_to_connector_pin_1_y - pcb_connector_pin_width/2, -pcb_connector_pin_tail_length + pcb_thickness + 2.5/2]) {
            cube([pcb_connector_pin_width, pcb_connector_pin_width, pcb_connector_pin_tail_length]);
            translate([-connector_pin_pitch, 0, 0]) {
                cube([pcb_connector_pin_width, pcb_connector_pin_width, pcb_connector_pin_tail_length]);
            }
            translate([-connector_pin_pitch * 2, 0, 0]) {
                cube([pcb_connector_pin_width, pcb_connector_pin_width, pcb_connector_pin_tail_length]);
            }
        }
    }

    // Sensor pins
    color([0.5, 0.5, 0.5]) {
        pin_extra_length = 0.1;  // pins excess sticking out from the back of the PCB
        sensor_z_offset = pcb_to_spool - sensor_spool_distance - hall_effect_thickness/2 - 0.1;
        sensor_pin_length = sensor_z_offset + pcb_thickness + pin_extra_length;
        assert(sensor_pin_length < hall_effect_pin_length_max, "Warning: design is too thick to fit sensor");

        translate([pcb_hole_to_sensor_pin_1_x - pcb_sensor_pin_width/2, pcb_hole_to_sensor_pin_1_y - pcb_sensor_pin_width/2, -sensor_z_offset]) {
            cube([pcb_sensor_pin_width, pcb_sensor_pin_width, sensor_pin_length]);
            translate([-sensor_pin_pitch, 0, 0]) {
                cube([pcb_sensor_pin_width, pcb_sensor_pin_width, sensor_pin_length]);
            }
            translate([-sensor_pin_pitch * 2, 0, 0]) {
                cube([pcb_sensor_pin_width, pcb_sensor_pin_width, sensor_pin_length]);
            }
        }
    }

    // Sensor body
    color([0, 0, 0]) {
        translate([pcb_hole_to_sensor_pin_1_x - sensor_pin_pitch - hall_effect_width/2, pcb_hole_to_sensor_pin_1_y, -pcb_to_spool + sensor_spool_distance]) {
            cube([hall_effect_width, hall_effect_height, hall_effect_thickness]);
        }
    }

    // Jig
    if(render_jig) {
        color([1, 1, 0])
        translate([-pcb_edge_to_hole_x - pcb_jig_align_thickness - pcb_jig_align_clearance, pcb_hole_to_sensor_pin_1_y + pcb_sensor_pin_width/2 + thickness, -pcb_to_sensor(pcb_to_spool) + pcb_jig_depth_clearance])
        rotate([90, 0, 0])
            sensor_jig(pcb_to_spool, jig_thickness);
    }
}

// 2D cutouts needed to mount the PCB module, origin at the center of the mounting hole
module pcb_cutouts(adjustment_range=pcb_adjustment_range) {
    hull_slide(delta=adjustment_range) {
        // Bolt slot
        hull() {
            circle(r=m4_hole_diameter/2, $fn=30);
            translate([pcb_hole_to_sensor_pin_1_x + sensor_pin_pitch - m4_hole_diameter/2, 0, 0])
                circle(r=m4_hole_diameter/2, $fn=30);
        }
        // Pin header slot
        translate([pcb_hole_to_connector_pin_1_x - connector_pin_pitch, -pcb_hole_to_connector_pin_1_y]) {
            hull() {
                pin_slot_height = pcb_connector_pin_width + pcb_connector_pin_slop;
                pin_slot_width = connector_pin_pitch * 4 - pin_slot_height;
                translate([pin_slot_width/2, 0, 0])
                    circle(pin_slot_height/2, $fn=15);
                translate([-pin_slot_width/2, 0, 0])
                    circle(pin_slot_height/2, $fn=15);
            }
        }
    }
}

module hull_slide(delta) {
    for (i = [0:$children - 1]) {
        hull() {
            translate([-delta, 0]) {
                children(i);
            }
            translate([delta, 0]) {
                children(i);
            }
        }
    }
}

function pcb_to_sensor(pcb_to_spool) = pcb_to_spool - sensor_spool_distance - hall_effect_thickness;  // using sensor rear face
function pcb_hole_to_sensor_x() = pcb_hole_to_sensor_x;
function pcb_hole_to_sensor_y() = pcb_hole_to_sensor_y;
function pcb_thickness() = pcb_thickness;
function sensor_jig_height(pcb_to_spool) = pcb_to_sensor(pcb_to_spool) - pcb_jig_depth_clearance + pcb_jig_align_length + pcb_thickness;
function sensor_jig_width(pcb_to_spool) = pcb_length + (pcb_jig_align_thickness + pcb_jig_align_clearance) * 2;

module sensor_jig(pcb_to_spool, thickness) {
    module fillet() {
        eps = 0.01;
        difference() {
            translate([-eps, -eps, 0])
                square(pcb_jig_corner_fillet + eps);
            translate([pcb_jig_corner_fillet, pcb_jig_corner_fillet, 0])
                circle(r=pcb_jig_corner_fillet, $fn=20);
        }
    }

    linear_extrude(thickness) {
        difference() {
        union() {
            square([sensor_jig_width(pcb_to_spool), pcb_to_sensor(pcb_to_spool) - pcb_jig_depth_clearance]);  // main body
            square([pcb_jig_align_thickness, sensor_jig_height(pcb_to_spool)]);  // alignment edge, left
            translate([sensor_jig_width(pcb_to_spool) - pcb_jig_align_thickness, 0])
            square([pcb_jig_align_thickness, sensor_jig_height(pcb_to_spool)]);  // alignment edge, right
        }
        fillet();
        mirror([1, 0, 0])
            translate([-sensor_jig_width(pcb_to_spool), 0, 0])
                fillet();
        }
    }
}

// Example usage:
pcb(10);
