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

pcb_thickness = 1.6;

// From datasheet:
hall_effect_height = (2.8 + 3.2) / 2;
hall_effect_width = (3.9 + 4.3) / 2;
hall_effect_thickness = (1.40 + 1.60) / 2;
hall_effect_sensor_offset_y = hall_effect_height - 1.25;

// From sensor.kicad_pcb:
pcb_height = 16.256;
pcb_length = 16.256;
pcb_hole_to_sensor_pin_1_x = 8.636;
pcb_hole_to_sensor_pin_1_y = 1.27;
sensor_pin_pitch = 1.27;
sensor_pin_length = 8;
pcb_hole_to_connector_pin_1_x = 8.636;
pcb_hole_to_connector_pin_1_y = 8.636;
connector_pin_pitch = 2.54;
pcb_edge_to_hole_x = 4.572;
pcb_edge_to_hole_y = 4.572;

pcb_adjustment_range = 4;
pcb_hole_radius = m4_hole_diameter/2;


// Computed dimensions
pcb_hole_to_sensor_x = pcb_hole_to_sensor_pin_1_x - sensor_pin_pitch;
pcb_hole_to_sensor_y = pcb_hole_to_sensor_pin_1_y + hall_effect_sensor_offset_y;


// Rough numbers for 3d rendering only (non-critical dimensions)
pcb_connector_height = 3.2;
pcb_connector_width = 8.2;
pcb_connector_length = 18;
pcb_connector_pin_width = 0.64;
pcb_connector_pin_slop = 0.1;
pcb_connector_pin_tail_length = 3.05 + 2.5/2;

pcb_sensor_pin_width = 0.43;


// 3D PCB module, origin at the center of the mounting hole on the bottom surface of the PCB
module pcb() {
    color([0, 0.5, 0]) {
        linear_extrude(height=pcb_thickness) {
            difference() {
                translate([-pcb_edge_to_hole_x, -pcb_height + pcb_edge_to_hole_y]) {
                    square([pcb_length, pcb_height]);
                }
                circle(r=m4_hole_diameter/2, $fn=30);
            }
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
        translate([pcb_hole_to_sensor_pin_1_x - pcb_sensor_pin_width/2, pcb_hole_to_sensor_pin_1_y - pcb_sensor_pin_width/2, -sensor_pin_length + pcb_thickness + 0.1]) {
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
        translate([pcb_hole_to_sensor_pin_1_x - sensor_pin_pitch - hall_effect_width/2, pcb_hole_to_sensor_pin_1_y, -sensor_pin_length - hall_effect_thickness/2 + pcb_thickness]) {
            cube([hall_effect_width, hall_effect_height, hall_effect_thickness]);
        }
    }
}

// 2D cutouts needed to mount the PCB module, origin at the center of the mounting hole
module pcb_cutouts() {
    hull_slide() {
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

module hull_slide() {
    for (i = [0:$children - 1]) {
        hull() {
            translate([-pcb_adjustment_range, 0]) {
                children(i);
            }
            translate([pcb_adjustment_range, 0]) {
                children(i);
            }
        }
    }
}

