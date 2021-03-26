/*
   Copyright 2015-2020 Scott Bezek and the splitflap contributors

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

module laser_mirror() {
    if (render_2d_mirror) {
        mirror([1, 0, 0])
            children();
    }
    else {
        children();
    }
}

module laser_etch() {
    if (!render_2d_mirror && (render_etch || render_index == -1)) {
        children();
    }
}

module split_flap_2d() {
    laser_mirror() {
        panel_height = enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width + kerf_width + spool_strut_width + kerf_width;
        projection_renderer(render_index=render_index, render_etch=render_etch, kerf_width=kerf_width, panel_height=panel_height, panel_horizontal=panel_horizontal, panel_vertical=panel_vertical) {
            // Main enclosure (left, right, front)
            translate([0, 0])
                enclosure_left();

            laser_etch()
                translate([0, 0])
                    enclosure_left_etch();

            translate([0, enclosure_length + kerf_width])
                enclosure_right();

            laser_etch()
                translate([0, enclosure_length + kerf_width])
                    enclosure_right_etch();

            translate([0, enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width - enclosure_horizontal_inset])
                rotate([0, 0, -90])
                    enclosure_front();

            laser_etch()
                translate([0, enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width - enclosure_horizontal_inset])
                    rotate([0, 0, -90])
                        enclosure_front_etch();

            // Top and bottom
            translate([enclosure_height + kerf_width + enclosure_length_right, enclosure_wall_to_wall_width + kerf_width])
                rotate([0, 0, 90])
                    enclosure_top();

            translate([enclosure_height + kerf_width, enclosure_wall_to_wall_width])
                rotate([0, 0, -90])
                    enclosure_bottom();

            // Bottom laser etching
            laser_etch()
                translate([enclosure_height + kerf_width, enclosure_wall_to_wall_width])
                    rotate([0, 0, -90])
                        enclosure_bottom_etch();

            // Spool struts cut out of right side
            translate([thickness*2 + 5, enclosure_length + kerf_width + enclosure_length_right - spool_strut_width/2 - 3, thickness])
                spool_strut();

            // Spool struts at the top
            spool_strut_y_offset = enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width + kerf_width + spool_strut_width/2;
            translate([spool_strut_length, spool_strut_y_offset, thickness/2])
                rotate([0, 0, 180])
                    spool_strut();
            translate([spool_strut_length*2 + kerf_width, spool_strut_y_offset, thickness/2])
                rotate([0, 0, 180])
                    spool_strut();
            translate([spool_strut_length*3 + kerf_width*2, spool_strut_y_offset, thickness/2])
                rotate([0, 0, 180])
                    spool_strut();

            // Connector brackets on the top right
            translate([enclosure_height + kerf_width, 2 * enclosure_wall_to_wall_width + 2 * kerf_width - thickness, 0])
                connector_bracket();

            translate([enclosure_height + kerf_width + connector_bracket_width - connector_bracket_length_outer, 2 * enclosure_wall_to_wall_width + 3 * kerf_width - thickness + connector_bracket_width + connector_bracket_length_outer, 0])
                rotate([0,0,-90])
                    connector_bracket();

            // Flap spools in flap window
            flap_spool_y_off = enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width - front_window_right_inset - enclosure_horizontal_inset - front_window_width/2;
            flap_spool_x_off = spool_outer_radius + enclosure_height_lower - front_window_lower + kerf_width + 2;
            translate([flap_spool_x_off, flap_spool_y_off])
                flap_spool_complete(motor_shaft=true, magnet_hole=true);
            translate([flap_spool_x_off + spool_outer_radius*2 + 2, flap_spool_y_off])
                flap_spool_complete(captive_nut=true);

            // Flap spool etching
            laser_etch() {
                translate([flap_spool_x_off, flap_spool_y_off])
                    mirror([0, 1, 0])
                    flap_spool_etch();
                translate([flap_spool_x_off + spool_outer_radius*2 + 2, flap_spool_y_off])
                    flap_spool_etch();
            }

            // Spool retaining wall in motor window
            translate([enclosure_height_lower + 28byj48_shaft_offset - 28byj48_chassis_radius + (28byj48_chassis_radius + motor_backpack_extent)/2, enclosure_length - front_forward_offset - 28byj48_chassis_radius - motor_hole_slop/2 + spool_strut_width/2 + kerf_width])
                spool_retaining_wall(m4_bolt_hole=true);

            // Sensor soldering jig
            translate([enclosure_height_lower + 28byj48_shaft_offset - 28byj48_chassis_radius + (28byj48_chassis_radius + motor_backpack_extent)/2 + sensor_jig_width(pcb_to_spool)/2, enclosure_length - front_forward_offset + 28byj48_chassis_radius + motor_hole_slop/2 - kerf_width])
                rotate([0, 0, 180])
                    sensor_jig(pcb_to_spool);
        }
    }
}