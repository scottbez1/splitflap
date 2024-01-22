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


module pcb_outline_2d() {
    difference() {
        translate([-pcb_edge_to_hole_x, -pcb_height + pcb_edge_to_hole_y]) {
            square([pcb_length, pcb_height]);
        }
        circle(r=pcb_hole_radius, $fn=30);
        translate([pcb_hole_to_bolt_hole_x, -pcb_hole_to_bolt_hole_y]) {
            circle(r=pcb_bolt_hole_radius, $fn=30);
        }
    }
}

// 3D PCB module, origin at the center of the mounting hole on the bottom surface of the PCB
module pcb(pcb_to_spool) {
    color([0, 0.5, 0]) {
        linear_extrude(height=pcb_thickness) {
            pcb_outline_2d();
        }
    }

    mirror([0, 0, 1]) {
    // Connector
    color([0, 0, 0]) {
        translate([pcb_hole_to_connector_pin_2_x - pcb_connector_width/2, -pcb_hole_to_connector_pin_2_y - pcb_connector_length, 0]) {
            cube([pcb_connector_width, pcb_connector_length, pcb_connector_height]);
        }
    }

    // Connector pins
    color([0.5, 0.5, 0.5]) {
        translate([pcb_hole_to_connector_pin_2_x - pcb_connector_pin_width/2, -pcb_hole_to_connector_pin_2_y - pcb_connector_pin_width/2, -pcb_connector_pin_tail_length + 2.5/2]) {
            cube([pcb_connector_pin_width, pcb_connector_pin_width, pcb_connector_pin_tail_length]);
            translate([-connector_pin_pitch, 0, 0]) {
                cube([pcb_connector_pin_width, pcb_connector_pin_width, pcb_connector_pin_tail_length]);
            }
            translate([connector_pin_pitch, 0, 0]) {
                cube([pcb_connector_pin_width, pcb_connector_pin_width, pcb_connector_pin_tail_length]);
            }
        }
    }
    }

    // Sensor body
    color([0, 0, 0]) {
        translate([pcb_hole_to_sensor_x - hall_effect_width/2, pcb_hole_to_sensor_y - hall_effect_height/2, pcb_thickness]) {
            cube([hall_effect_width, hall_effect_height, hall_effect_thickness]);
        }
    }
}

function pcb_hole_to_sensor_x() = pcb_hole_to_sensor_x;
function pcb_hole_to_sensor_y() = pcb_hole_to_sensor_y;
function pcb_thickness() = pcb_thickness;

// Example usage:
pcb(10);
