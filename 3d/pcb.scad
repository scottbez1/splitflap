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
AH1815_height = (2.8 + 3.2) / 2;
AH1815_width = (3.9 + 4.3) / 2;
AH1815_thickness = (1.40 + 1.60) / 2;
AH1815_sensor_offset_y = AH1815_height - 1.25;

// From sensor.kicad_pcb:
pcb_height = 16.256;
pcb_length = 16.256;
pcb_hole_to_sensor_pin_1_x = 8.636;
pcb_hole_to_sensor_pin_1_y = 1.27;
sensor_pin_pitch = 1.27;
sensor_pin_length = 8; // XXX FIXME
pcb_hole_to_connector_pin_1_x = 8.636;
pcb_hole_to_connector_pin_1_y = 8.636;
connector_pin_pitch = 2.54;
pcb_edge_to_hole_x = 4.572;
pcb_edge_to_hole_y = 4.572;

pcb_adjustment_range = 4;
pcb_hole_radius = m4_hole_diameter/2;


// Computed dimensions
pcb_hole_to_sensor_x = pcb_hole_to_sensor_pin_1_x - sensor_pin_pitch;
pcb_hole_to_sensor_y = pcb_hole_to_sensor_pin_1_y + AH1815_sensor_offset_y;


// Rough numbers for 3d rendering only (non-critical dimensions)
pcb_connector_height = 3.2;
pcb_connector_width = 8.2;
pcb_connector_length = 18;


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

    // Sensor pins
    color([0.5, 0.5, 0.5]) {
        translate([pcb_hole_to_sensor_pin_1_x - sensor_pin_pitch/4, pcb_hole_to_sensor_pin_1_y - sensor_pin_pitch/4, -sensor_pin_length + pcb_thickness + 0.1]) {
            cube([sensor_pin_pitch / 2, sensor_pin_pitch / 2, sensor_pin_length]);
            translate([-sensor_pin_pitch, 0, 0]) {
                cube([sensor_pin_pitch / 2, sensor_pin_pitch / 2, sensor_pin_length]);
            }
            translate([-sensor_pin_pitch * 2, 0, 0]) {
                cube([sensor_pin_pitch / 2, sensor_pin_pitch / 2, sensor_pin_length]);
            }
        }
    }

    // Sensor body
    color([0, 0, 0]) {
        translate([pcb_hole_to_sensor_pin_1_x - sensor_pin_pitch - AH1815_width/2, pcb_hole_to_sensor_pin_1_y, -sensor_pin_length - AH1815_thickness/2 + pcb_thickness]) {
            cube([AH1815_width, AH1815_height, AH1815_thickness]);
        }
    }
}

// 2D cutouts needed to mount the PCB module, origin at the center of the mounting hole
module pcb_cutouts() {
    hull_slide() {
        translate([-m4_hole_diameter/2, -m4_hole_diameter/2]) {
            square([m4_hole_diameter/2 + pcb_hole_to_sensor_pin_1_x + sensor_pin_pitch, m4_hole_diameter]);
        }
        translate([pcb_hole_to_connector_pin_1_x - connector_pin_pitch, -pcb_hole_to_connector_pin_1_x]) {
            square([connector_pin_pitch * 4, connector_pin_pitch], center=true);
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

