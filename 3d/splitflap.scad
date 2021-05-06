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

use<assert.scad>;
use<label.scad>;
use<projection_renderer.scad>;
use<roboto/RobotoCondensed-Regular.ttf>;
use<rough7380.scad>;
use<spool.scad>;
use<shapes.scad>;

include<28byj-48.scad>;
include<flap_fonts.scad>;
include<pcb.scad>;
include<common.scad>;
include<splitflap_2d.scad>;
include<splitflap_3d.scad>;
include<parts/flap.scad>;
include<parts/spool.scad>;
include<parts/enclosure.scad>;
include<parts/hardware.scad>;
include<parts/connector.scad>;

// ##### RENDERING OPTIONS #####

render_3d = true;

// 3d parameters:
render_enclosure = 2; // 0=invisible; 1=translucent; 2=opaque color;
render_flaps = 2; // 0=invisible; 1=front flap only; 2=all flaps
render_flap_area = 0; // 0=invisible; 1=collapsed flap exclusion; 2=collapsed+extended flap exclusion
render_letters = "44";
render_units = len(render_letters);
render_unit_separation = 0;
render_spool = true;
render_pcb = true;
render_sensor_jig = false;
render_bolts = true;
render_motor = true;

// 2d parameters:
render_index = -1;
render_etch = false;
render_2d_mirror = false;

// Panelization:
panel_vertical = 0;
panel_horizontal = 0;

// Version label:
render_revision = "deadbeef";
render_date = "YYYY-MM-DD";

// Ponoko kerf values are 0.2 mm for MDF and acrylic (all thicknesses)
// Remember: it's better to underestimate (looser fit) than overestimate (no fit)
kerf_width = 0.2 - 0.02;

num_flaps = 40;

function get_flap_gap() = flap_gap;  // for exposing this value when this file is 'used' and not 'included' in other files

spool_outer_radius = flap_spool_outer_radius(num_flaps, flap_hole_radius, flap_hole_separation, flap_spool_outset); //flap_pitch_radius + 2*flap_hole_radius;

//legacyAssert(spool_width >= flap_width, "Flap is wider than spool!");

// distance from the outside spool face to the inside of the left enclosure
pcb_to_spool = enclosure_wall_to_wall_width - front_window_width - thickness + spool_width_slop/2;

// 'get' functions to extract these values for when this file is 'used' and not 'included'
function connector_bracket_length() = connector_bracket_length_outer;
function connector_bracket_width() = connector_bracket_width;

mounting_hole_inset = m4_button_head_diameter/2 + 2;

enclosure_indicator_inset = 3.0;  // inset on both X and Y
enclosure_indicator_size = 1.75;  // symbol size
enclosure_indicator_arrow_width = 2.25;
enclosure_indicator_arrow_height = enclosure_indicator_arrow_width * 2;
enclosure_indicator_position_y = (enclosure_height - enclosure_vertical_inset - thickness) - enclosure_indicator_inset;

zip_tie_height = 3.0;  // height of the zip-tie hole
zip_tie_width = 2.0;  // width of the zip-tie holes
zip_tie_spacing = 6.5;  // spacing between each zip-tie hole, inside edges
zip_tie_fillet = 0.5;  // radius of the rounded zip-tie hole corners

enclosure_left_zip_side_inset = 5.0;  // inset from left for the bottom zip tie holes, edge to outside edge
enclosure_left_zip_bottom_inset = 22.5;  // inset from bottom for the bottom zip tie holes, edge to group center

enclosure_left_zip_top_inset = 22.5;  // inset from top for the top zip tie holes, edge to group center

alignment_bar_diameter = 6.35;  // 1/4"
alignment_bar_clearance = 0.25;
alignment_bar_fillet_radius = 1.25;

alignment_bar_cutout_width = alignment_bar_diameter + (2 * alignment_bar_clearance);
alignment_bar_center = (enclosure_length - enclosure_length_right) - alignment_bar_cutout_width/2;


echo(kerf_width=kerf_width);
echo(enclosure_height=enclosure_height);
echo(enclosure_height_upper=enclosure_height_upper);
echo(enclosure_height_lower=enclosure_height_lower);
echo(enclosure_width=enclosure_width);
echo(enclosure_wall_to_wall_width=enclosure_wall_to_wall_width);
echo(enclosure_length=enclosure_length);
echo(enclosure_length_right=enclosure_length_right);
echo(enclosure_length_real=enclosure_length+thickness);
echo(spool_strut_inner_length=spool_strut_inner_length);
echo(front_window_width=front_window_width);
echo(front_window_upper=front_window_upper);
echo(front_window_lower=front_window_lower);
echo(front_window_height=front_window_lower+front_window_upper);
echo(front_forward_offset=front_forward_offset);
echo(flap_exclusion_radius=exclusion_radius);
echo(flap_hole_radius=flap_hole_radius);
echo(flap_notch_height=flap_notch_height);
echo(pcb_to_sensor=pcb_to_sensor(pcb_to_spool));

// Dead code?
assembly_inner_radius = m4_hole_diameter/2;
module shaft_centered_motor_hole() {
    margin = 5;
    width = 28byj48_mount_center_offset*2 + 3.5*2 + margin*2;
    length = 18 + 14 + margin*2;

    translate([-width/2, -(margin + 18 + 8)])
        square([width, length]);
}

if (render_3d) {
    for (i = [0 : render_units - 1]) {
        translate([-enclosure_width/2 + (-(render_units-1) / 2 + i)*(enclosure_width + render_unit_separation), 0, 0])
            split_flap_3d(render_letters[render_units - 1 - i], include_connector=(i != render_units - 1));
    }
} else {
    split_flap_2d();
}
