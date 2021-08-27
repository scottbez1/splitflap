/*
   Copyright 2018 Scott Bezek and the splitflap contributors

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
sensor_spool_distance = 0.70;  // distance from the sensor to the face of the spool

// From datasheet:
hall_effect_height = (2.8 + 3.2) / 2;
hall_effect_width = (3.9 + 4.3) / 2;
hall_effect_thickness = (1.40 + 1.60) / 2;
hall_effect_sensor_offset_y = hall_effect_height - 1.25;
hall_effect_pin_length_max = 14.5;

// From sensor.kicad_pcb:
pcb_height = 16.256;
pcb_length = 16.256;
pcb_hole_to_sensor_pin_1_x = 8.636;
pcb_hole_to_sensor_pin_1_y = 1.27;
sensor_pin_pitch = 1.27;
pcb_hole_to_connector_pin_1_x = 8.636;
pcb_hole_to_connector_pin_1_y = 8.636;
connector_pin_pitch = 2.54;
pcb_edge_to_hole_x = 4.572;
pcb_edge_to_hole_y = 4.572;

pcb_adjustment_range = 4;
pcb_hole_radius = m4_hole_diameter/2;

// Jig dimensions
pcb_jig_corner_fillet = 2;
pcb_jig_align_thickness = 2;
pcb_jig_align_length = 0;  // past the PCB thickness
pcb_jig_align_clearance = 0.25;  // on x, around the PCB
pcb_jig_depth_clearance = 0.1;  // on y, from sensor to jig


// Computed dimensions
pcb_hole_to_sensor_x = pcb_hole_to_sensor_pin_1_x - sensor_pin_pitch;
pcb_hole_to_sensor_y = pcb_hole_to_sensor_pin_1_y + hall_effect_sensor_offset_y;


// Rough numbers for 3d rendering only (non-critical dimensions)
pcb_connector_height = 3.2;
pcb_connector_width = 8.2;
pcb_connector_length = 18;
pcb_connector_pin_width = 0.64;
pcb_connector_pin_slop = 0.4;
pcb_connector_pin_tail_length = 3.05 + 2.5/2;

pcb_sensor_pin_width = 0.43;
