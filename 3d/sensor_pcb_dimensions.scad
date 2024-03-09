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

pcb_thickness = 0.8;

// From datasheet:
hall_effect_height = 1.7;
hall_effect_width = 3;
hall_effect_thickness = 1.22;

// From sensor.kicad_pcb:
pcb_height = 20;
pcb_length = 40;
pcb_hole_to_sensor_x = 17.5;
pcb_hole_to_sensor_y = 0;
pcb_hole_to_connector_pin_2_x = 28;
pcb_hole_to_connector_pin_2_y = 10;
pcb_hole_to_bolt_hole_x = 17.5;     // 28byj48_mount_center_offset
pcb_hole_to_bolt_hole_y = 8;        // 28byj48_shaft_offset
connector_pin_pitch = 2.54;
pcb_edge_to_hole_x = 7;
pcb_edge_to_hole_y = 7;

pcb_hole_radius = 9.4/2;            // 28byj48_shaft_collar_radius
pcb_bolt_hole_radius = 4.3/2;       // M4

// Rough numbers for 3d rendering only (non-critical dimensions)
pcb_connector_height = 3.2;
pcb_connector_width = 8.2;
pcb_connector_length = 18;
pcb_connector_pin_width = 0.64;
pcb_connector_pin_slop = 0.4;
pcb_connector_pin_tail_length = 3.05 + 2.5/2;
