/*
   Copyright 2015-2021 Scott Bezek and the splitflap contributors

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

use<28byj-48.scad>;
use<assert.scad>;
use<color_util.scad>;
use<flap.scad>;
use<flap_characters.scad>;
use<label.scad>;
use<pcb.scad>;
use<projection_renderer.scad>;
use<rough7380.scad>;
use<spool.scad>;
use<shapes.scad>;

include<flap_dimensions.scad>;
include<global_constants.scad>;
include<m4_dimensions.scad>;

// ##### RENDERING OPTIONS #####

render_3d = true;

// 3d parameters:
render_enclosure = 2; // 0=invisible; 1=translucent; 2=opaque color;
render_flaps = 2; // 0=invisible; 1=front flap only; 2=all flaps
render_flap_area = 0; // 0=invisible; 1=collapsed flap exclusion; 2=collapsed+extended flap exclusion
render_letters = 2;  // 0=invisible; 1=front flap only; 2=all flaps

render_message = "Ag";

render_unit_separation = 0;
render_spool = true;
render_pcb = true;
render_bolts = true;
render_motor = true;

// 2d parameters:
render_index = -1;
render_etch = false;
render_2d_mirror = false;
render_home_indicator_as_cut = false;
render_front_panel = true;

enable_connectors = true;
enable_alignment_bar = true;
enable_mounting_holes = true;
enable_source_info = true;

// Panelization:
panel_vertical = 0;
panel_horizontal = 0;

// Version label:
render_revision = "deadbeef";
render_date = "YYYY-MM-DD";

spool_explosion = 0; // 0-1
spool_strut_explosion = lookup(spool_explosion, [
    [0, 0],
    [0.2, 0],
    [1, 30],
]);
spool_horizontal_explosion = lookup(spool_explosion, [
    [0, 0],
    [1, 8],
]);

num_flaps = 52;
echo(num_flaps=num_flaps);

// Ponoko kerf values are 0.2 mm for MDF and acrylic (all thicknesses)
// Remember: it's better to underestimate (looser fit) than overestimate (no fit)
kerf_width = 0.185;

// MDF, .120in nominal
// https://www.ponoko.com/materials/mdf-fiberboard
thickness = 3.0;

etch_depth = 0.1;  // for render

captive_nut_inset=6;

assembly_inner_radius = m4_hole_diameter/2;


// Rendering Colors
assembly_color = [0.15, 0.15, 0.15];
etch_color = [0.3, 0.3, 0.3];

hardware_color = [0.75, 0.75, 0.8];  // steel, "bfbfcc"
flap_color = [0.05, 0.05, 0.05];
letter_color = color_invert(flap_color);  // inverse of the flap color, for contrast
assembly_colors = [
    color_multiply(assembly_color, [1.161, 1.157, 1.157, 1.0]),
    color_multiply(assembly_color, [0.897, 0.895, 0.895, 1.0]),
    color_multiply(assembly_color, [0.547, 0.542, 0.540, 1.0]),
    color_multiply(assembly_color, [0.268, 0.268, 0.271, 1.0]),
];

bolt_color = hardware_color;
nut_color = color_multiply(hardware_color, [0.933, 0.933, 0.9, 1.0]);  // "b2b2b7" with steel


flap_width_slop = 0.5;  // amount of slop of the flap side to side between the 2 spools

spool_width_slop = 1.4;  // amount of slop for the spool assembly side-to-side inside the enclosure

spool_tab_clearance = 0;  // for the tabs connecting the struts to the spool ends (interference fit)
spool_retaining_clearance = 0.10;  // for the notches in the spool retaining wall
spool_joint_clearance = 0.10;  // for the notched joints on the spool struts


character_list = get_character_list();
echo(character_list=len(character_list));
assert(len(character_list) == num_flaps || _is_projection_rendering, "WARNING: num_flaps and character_list mismatch!");
render_flap_index = undef;
render_string = is_undef(render_flap_index) ? render_message : character_list[render_flap_index];
render_units = len(render_string);

flap_hole_radius = (flap_pin_width + 0.8) / 2;
flap_hole_separation = 1.2;  // additional spacing between hole edges
flap_gap = (flap_hole_radius * 2 - flap_pin_width) + flap_hole_separation;

flap_spool_outset = 0.8;
flap_pitch_radius = flap_spool_pitch_radius(num_flaps, flap_hole_radius, flap_hole_separation); //num_flaps * (flap_hole_radius*2 + flap_hole_separation) / (2*PI);
spool_outer_radius = flap_spool_outer_radius(num_flaps, flap_hole_radius, flap_hole_separation, flap_spool_outset); //flap_pitch_radius + 2*flap_hole_radius;

// Radius where flaps are expected to flap in their *most collapsed* (90 degree) state
exclusion_radius = sqrt(flap_height*flap_height + flap_pitch_radius*flap_pitch_radius);
// Radius where flaps are expected to flap in their *most extended* state
outer_exclusion_radius = flap_pitch_radius + flap_height + 2;

front_forward_offset = flap_pitch_radius + flap_thickness/2;

spool_width = flap_width - flap_notch_depth*2 + flap_width_slop + thickness*2;  // spool width, outside face (spool to spool)
spool_width_clearance = max(spool_width, flap_width + flap_width_slop);  // width clearance for the spool, either for the spool itself or the flaps

//legacyAssert(spool_width >= flap_width, "Flap is wider than spool!");
spool_strut_num_joints = 3;
spool_strut_tab_width=8;
spool_strut_tab_width_narrow=6;
spool_strut_tab_outset=8;
spool_strut_width = (spool_strut_tab_outset + thickness/2) * 2;
spool_strut_length = spool_width;
spool_strut_inner_length = spool_width - 3 * thickness;

spool_strut_exclusion_radius = sqrt((spool_strut_tab_outset+thickness/2)*(spool_strut_tab_outset+thickness/2) + (spool_strut_tab_width/2)*(spool_strut_tab_width/2));

m4_axle_hole_diameter = 4.3;    // Slightly closer fit than the standard m4_hole_diameter, since a loose fit here will cause the spool to sit at a slight angle

magnet_diameter = 4;
magnet_hole_clearance = -0.05;  // interference fit
magnet_hole_radius = (magnet_diameter + magnet_hole_clearance)/2;
magnet_hole_offset = pcb_hole_to_sensor_x();

// Clearance between the motor chassis and the outside right wall of the previous module
28byj48_chassis_height_clearance = 1.4;

motor_shaft_under_radius = 0.08;  // interference fit
motor_slop_radius = 3;


// Width measured from the outside of the walls
enclosure_wall_to_wall_width = thickness + spool_width_slop/2 + spool_width_clearance + spool_width_slop/2 + max(28byj48_mount_bracket_height() + m4_button_head_length, 4 + 28byj48_mount_bracket_height() - spool_width_slop/2) + thickness;

// Width of the front panel
enclosure_width = enclosure_wall_to_wall_width + 28byj48_chassis_height() + 28byj48_chassis_height_clearance - thickness - 28byj48_mount_bracket_height();
front_window_upper_base = (flap_height - flap_pin_width/2);
front_window_overhang = 1;
front_window_upper = front_window_upper_base - front_window_overhang;
front_window_lower = sqrt(outer_exclusion_radius*outer_exclusion_radius - front_forward_offset*front_forward_offset);
front_window_height = front_window_lower+front_window_upper;
front_window_width = spool_width_slop + spool_width_clearance;
front_window_right_inset = thickness;
enclosure_horizontal_inset = (enclosure_width - front_window_width)/2 - front_window_right_inset; // center the window in the front face (the inset is measured with respect to the *outside* of the wall, hence the "front_window_right_inset" correction)
enclosure_vertical_clearance_top = 5; // gap between top of flaps and top of enclosure
enclosure_vertical_clearance_bottom = 1; // gap between bottom of flaps and bottom of enclosure
enclosure_vertical_inset = max(thickness*1.5, m4_nut_width_corners_padded/2); // distance from top of sides to top of the top piece
enclosure_height_upper = exclusion_radius + enclosure_vertical_clearance_top + thickness + enclosure_vertical_inset;
enclosure_height_lower = flap_pitch_radius + flap_height + enclosure_vertical_clearance_bottom + thickness + enclosure_vertical_inset;
enclosure_height = enclosure_height_upper + enclosure_height_lower;

enclosure_horizontal_rear_margin = 2; // minumum distance between the farthest feature and the rear

enclosure_length = front_forward_offset + 28byj48_mount_center_offset() + m4_hole_diameter/2 + enclosure_horizontal_rear_margin;

// distance from the outside spool face to the inside of the left enclosure
pcb_to_spool = enclosure_wall_to_wall_width - front_window_width - thickness + spool_width_slop/2;

// Enclosure tabs: front/back
enclosure_tab_clearance = 0.10;

num_front_tabs = 2;
front_tab_width = (front_window_width) / (num_front_tabs*2 - 1);

enclosure_length_right = front_forward_offset + m4_hole_diameter/2 + enclosure_horizontal_rear_margin;

side_tab_width = 2.5;
side_tab_bolt_head_clearance = m4_button_head_diameter / 2 + 0.5;
side_tab_length = thickness * 0.8;
side_tab_left_inset = 1;

backstop_bolt_vertical_offset = - (exclusion_radius + outer_exclusion_radius)/2;
backstop_bolt_forward_range = 14;

motor_mount_hole_radius = m4_hole_diameter/2;
motor_backpack_extent = 28byj48_backpack_extent() + 2; // Add 2mm to make sure there's room for the wires
motor_hole_slop = 1;
motor_window_radius = 5;

connector_bracket_length_outer = 14;
connector_bracket_length_inner = enclosure_length_right / 2 - m4_button_head_diameter/2 - 1;
connector_bracket_thickness = captive_nut_inset - thickness - 0.2;
connector_bracket_width = enclosure_width - enclosure_wall_to_wall_width + thickness*2 + connector_bracket_thickness*2;
connector_bracket_overlap = 4;
connector_bracket_clearance = 0.40;
connector_bracket_depth_clearance = 0.20;

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

captive_nut_bolt_length = m4_bolt_length + 3; // 1mm tolerance + 2mm flexibility in bolt length

alignment_bar_diameter = 6.35;  // 1/4"
alignment_bar_clearance = 0.25;
alignment_bar_fillet_radius = 1.25;

alignment_bar_cutout_width = alignment_bar_diameter + (2 * alignment_bar_clearance);
alignment_bar_center = (enclosure_length - enclosure_length_right) - alignment_bar_cutout_width/2;


// Exported values
// (Functions allow other files to reference these values when this file is 'used' and not 'included')
function get_assembly_colors() = assembly_colors;
function get_captive_nut_inset() = captive_nut_inset;
function get_connector_bracket_length() = connector_bracket_length_outer;
function get_connector_bracket_width() = connector_bracket_width;
function get_enclosure_height() = enclosure_height;
function get_enclosure_height_lower() = enclosure_height_lower;
function get_enclosure_height_upper() = enclosure_height_upper;
function get_enclosure_length_right() = enclosure_length_right;
function get_enclosure_vertical_inset() = enclosure_vertical_inset;
function get_enclosure_wall_to_wall_width() = enclosure_wall_to_wall_width;
function get_enclosure_width() = enclosure_width;
function get_flap_arc_separation() = (flap_hole_radius*2 + flap_hole_separation);
function get_flap_color() = flap_color;
function get_flap_gap() = flap_gap;
function get_flap_pin_width() = flap_pin_width;
function get_front_forward_offset() = front_forward_offset;
function get_front_window_lower() = front_window_lower;
function get_front_window_right_inset() = front_window_right_inset;
function get_front_window_upper() = front_window_upper;
function get_front_window_width() = front_window_width;
function get_letter_color() = letter_color;
function get_magnet_diameter() = magnet_diameter;
function get_magnet_hole_offset() = magnet_hole_offset;
function get_mounting_hole_inset() = mounting_hole_inset;
function get_num_flaps() = num_flaps;
function get_pcb_to_spool() = pcb_to_spool;
function get_thickness() = thickness;

function get_captive_nut_bolt_length() = captive_nut_bolt_length;



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
echo(front_window_height=front_window_height);
echo(front_forward_offset=front_forward_offset);
echo(flap_exclusion_radius=exclusion_radius);
echo(flap_hole_radius=flap_hole_radius);
echo(flap_notch_height=flap_notch_height);


module standard_m4_bolt(nut_distance=-1, bolt_length=10) {
    if (render_bolts) {
        color(bolt_color)
            roughM4_7380(bolt_length);
        if (nut_distance >= 0) {
            translate([0, 0, nut_distance])
                standard_m4_nut();
        }
    }
}

module standard_m4_nut(hole=true) {
    color(nut_color) {
        linear_extrude(m4_nut_length) {
            difference() {
                circle(r=m4_nut_width_corners/2, $fn=6);
                if(hole == true)
                    circle(r=m4_hole_diameter/2, $fn=20);
            }
        }
    }
}


// ##### CAPTIVE NUT NEGATIVE #####

// Centered in the x dimension
module captive_nut(bolt_diameter, bolt_length, nut_width, nut_length, nut_inset) {
    union() {
        translate([-bolt_diameter/2, 0, 0])
            square([bolt_diameter, bolt_length]);
        translate([-nut_width/2, nut_inset, 0])
            square([nut_width, nut_length]);
    }
}
module m4_captive_nut() {
    captive_nut(m4_hole_diameter, captive_nut_bolt_length, m4_nut_width_flats, m4_nut_length_padded, captive_nut_inset);
}


module zip_tie_holes() {
    spacing = (zip_tie_spacing + zip_tie_width)/2;

    translate([-spacing, 0, 0])
        rounded_square([zip_tie_width, zip_tie_height], center=true, r=zip_tie_fillet, $fn=30);
    translate([spacing, 0, 0])
        rounded_square([zip_tie_width, zip_tie_height], center=true, r=zip_tie_fillet, $fn=30);
}


// ##### Struts for bracing spool #####
module spool_strut_tab_hole(narrow, clearance) {
    square([thickness + clearance, narrow ? spool_strut_tab_width_narrow + clearance : spool_strut_tab_width + clearance], center=true);
}
module spool_strut_tab_holes(narrow=false, clearance=spool_tab_clearance) {
    for (i=[0:3]) {
        angle = 90*i;
        translate([cos(angle)*spool_strut_tab_outset, sin(angle)*spool_strut_tab_outset])
            rotate([0,0,angle])
                spool_strut_tab_hole(narrow, clearance);
    }
}
module spool_strut() {
    joint_tab_width = spool_strut_inner_length / spool_strut_num_joints;
    linear_extrude(thickness, center=true) {
        union() {
            translate([0, -spool_strut_tab_width_narrow / 2]) {
                square([thickness + eps, spool_strut_tab_width_narrow]);
            }
            translate([thickness, -spool_strut_tab_width / 2]) {
                square([spool_strut_length - thickness, spool_strut_tab_width]);
            }
            translate([thickness*2, -spool_strut_width / 2]) {
                difference() {
                    square([spool_strut_inner_length, spool_strut_width]);

                    // subtract out joints
                    union() {
                        for (i = [0:2:spool_strut_num_joints-1]) {
                            translate([i*joint_tab_width - spool_joint_clearance/2, -eps])
                                square([joint_tab_width + spool_joint_clearance, thickness + spool_joint_clearance/2 + eps]);
                        }
                        for (i = [1:2:spool_strut_num_joints-1]) {
                            translate([i*joint_tab_width, spool_strut_width - thickness - spool_joint_clearance/2])
                                square([joint_tab_width, thickness + spool_joint_clearance + eps]);
                        }
                    }
                }
            }
        }
    }
}

module spool_struts() {
    for (i=[0:3]) {
        angle = 90*i;
        //color([i < 2 ? 0 : 1, i == 0 || i == 2 ? 0 : 1, 0])
        color(i % 2 == 0 ? assembly_colors[1] : assembly_colors[2])
        translate([0, sin(angle)*(spool_strut_tab_outset + spool_strut_explosion), cos(angle)*(spool_strut_tab_outset + spool_strut_explosion)])
            rotate([-angle, 0, 0])
                spool_strut();
    }
}


module flap_spool_complete(captive_nut=false, motor_shaft=false, magnet_hole=false) {
    linear_extrude(thickness, convexity=10) {  // 'convexity' to fix rendering errors with etch 'difference()' result
        difference() {
            flap_spool(num_flaps, flap_hole_radius, flap_hole_separation, flap_spool_outset,
                    height=0);

            spool_strut_tab_holes(narrow=captive_nut);
            if (captive_nut) {
                circle(r=m4_nut_width_corners_padded/2, $fn=6);
            }
            if (motor_shaft) {
                rotate([0, 0, 90]) {
                    motor_shaft();
                }
            }
            if (magnet_hole) {
                // Hole for press fit magnet, 90 degrees from home flap position
                translate([0, magnet_hole_offset]) {
                    circle(r=magnet_hole_radius, $fn=15);
                }
            }
            if (render_home_indicator_as_cut) {
                flap_spool_home_indicator(num_flaps, flap_hole_radius, flap_hole_separation, flap_spool_outset, height=0);
            }
        }
    }
}

module flap_spool_etch() {
    enclosure_etch_style()
        flap_spool_home_indicator(num_flaps, flap_hole_radius, flap_hole_separation, flap_spool_outset);
}

module spool_retaining_wall(m4_bolt_hole=false) {
    linear_extrude(thickness) {
        difference() {
            square([spool_strut_width, spool_strut_width], center=true);
            spool_strut_tab_holes(clearance=spool_retaining_clearance);
            if (m4_bolt_hole) {
                circle(r=m4_hole_diameter/2, $fn=30);
            }
        }
    }
}

// double-flatted motor shaft of 28byj-48 motor (2D)
module motor_shaft() {
    union() {
        intersection() {
            circle(r=28byj48_shaft_radius()-motor_shaft_under_radius, $fn=50);
            square([28byj48_shaft_radius()*2, 3], center=true);
        }
        square([28byj48_shaft_radius()/3, 28byj48_shaft_radius()*4], center=true);
    }
}

module front_tabs_negative(upper, tool_diameter=0) {
    // tool_diameter is an optional parameter to adjust these cutouts to compensate for a rotary cutting tool, which
    // requires "dog-bones" for corners and adjustment of the cutout if the tool is larger than thickness. This will
    // generally not look good if cut all the way through the material, but with a CNC router these can be cut as
    // pockets which are not visible from the front.
    assert(tool_diameter <= m4_hole_diameter, "Tool diameter is too large to cut M4 holes");

    cutout_height = max(thickness, tool_diameter);

    // Offset is inverted on upper vs lower so that larger cutouts from tool diameter don't allow vertical movement freedom
    cutout_offset = (upper ? 1 : -1) * (cutout_height - thickness)/2;

    for (i = [0 : num_front_tabs-1]) {
        translate([thickness + (i*2+0.5) * front_tab_width, cutout_offset, 0]) {
            square([front_tab_width + enclosure_tab_clearance, cutout_height + enclosure_tab_clearance], center=true);

            // Dog-bones
            if (tool_diameter > 0) {
                // Dog-bones are rendered as squares to simplify the number of line segments in the final SVG output
                translate([(front_tab_width + enclosure_tab_clearance)/2 - tool_diameter/2, (cutout_height + enclosure_tab_clearance)/2]) {
                    square([tool_diameter, tool_diameter], center=true);
                }
                translate([(front_tab_width + enclosure_tab_clearance)/2 - tool_diameter/2, -(cutout_height + enclosure_tab_clearance)/2]) {
                    square([tool_diameter, tool_diameter], center=true);
                }
                translate([-(front_tab_width + enclosure_tab_clearance)/2 + tool_diameter/2, (cutout_height + enclosure_tab_clearance)/2]) {
                    square([tool_diameter, tool_diameter], center=true);
                }
                translate([-(front_tab_width + enclosure_tab_clearance)/2 + tool_diameter/2, -(cutout_height + enclosure_tab_clearance)/2]) {
                    square([tool_diameter, tool_diameter], center=true);
                }
            }
        }
    }
    for (i = [0 : num_front_tabs-2]) {
        translate([thickness + (i*2+1.5) * front_tab_width, 0, 0]) {
            if (tool_diameter == 0) {
                circle(r=m4_hole_diameter/2, $fn=30);
            } else {
                square([m4_hole_diameter, m4_hole_diameter], center=true);
            }
        }
    }
}

module connector_bracket_2d() {
    difference() {
        square([connector_bracket_width, connector_bracket_length_outer]);
        translate([connector_bracket_thickness, -eps]) {
            square([connector_bracket_width - connector_bracket_thickness*2, connector_bracket_length_outer - connector_bracket_length_inner + eps]);
        }
        translate([connector_bracket_thickness - connector_bracket_clearance/2, -eps]) {
            square([thickness + connector_bracket_clearance, connector_bracket_length_outer - connector_bracket_overlap + connector_bracket_depth_clearance + eps]);
        }
        translate([connector_bracket_width - connector_bracket_thickness - thickness - connector_bracket_clearance/2, -eps]) {
            square([thickness + connector_bracket_clearance, connector_bracket_length_outer - connector_bracket_overlap + connector_bracket_depth_clearance + eps]);
        }
    }
}

module connector_bracket() {
    linear_extrude(height=thickness) {
        connector_bracket_2d();
    }
}

module enclosure_etch_style() {
    color(etch_color)
        translate([0, 0, thickness])
            linear_extrude(height=etch_depth)
                children();
}

module enclosure_front_base_2d() {
    translate([-enclosure_horizontal_inset, 0, 0]) {
        square([enclosure_width, enclosure_height]);
    }
}

module enclosure_front_cutouts_2d(tool_diameter=0) {
    // Viewing window cutout
    translate([front_window_right_inset, enclosure_height_lower - front_window_lower])
        square([front_window_width, front_window_lower + front_window_upper]);

    // Front lower tabs
    translate([0, thickness * 0.5 + enclosure_vertical_inset, 0])
        front_tabs_negative(upper=false, tool_diameter=tool_diameter);

    // Front upper tabs
    translate([0, enclosure_height - thickness * 0.5 - enclosure_vertical_inset, 0])
        front_tabs_negative(upper=true, tool_diameter=tool_diameter);
}

module enclosure_front() {
    linear_extrude(height=thickness) {
        difference() {
            enclosure_front_base_2d();
            enclosure_front_cutouts_2d();
        }
    }
}

module enclosure_front_etch() {
    // alignment indicator, left side (triangle)
    enclosure_etch_style()
        translate([enclosure_wall_to_wall_width - thickness - enclosure_indicator_inset, enclosure_indicator_position_y])
            triangle(enclosure_indicator_size, center=true);

    // alignment indicator, right side (circle)
    enclosure_etch_style()
        translate([thickness + enclosure_indicator_inset, enclosure_indicator_position_y])
            circle(r=enclosure_indicator_size/2, $fn=60);

    // position indicator, 'up' arrow
    enclosure_etch_style()
        translate([enclosure_wall_to_wall_width + ((enclosure_width - enclosure_horizontal_inset) - enclosure_wall_to_wall_width)/2, enclosure_height - enclosure_indicator_arrow_height/2 - enclosure_indicator_inset])
            arrow([enclosure_indicator_arrow_width, enclosure_indicator_arrow_height], center=true);
}

// holes for 28byj-48 motor, centered around motor shaft
module motor_mount() {
    translate([-28byj48_mount_center_offset(), 28byj48_shaft_offset()]) {
        circle(r=motor_mount_hole_radius, $fn=30);
    }
    translate([28byj48_mount_center_offset(), 28byj48_shaft_offset()]) {
        circle(r=motor_mount_hole_radius, $fn=30);
    }

    hull() {
        x = -28byj48_chassis_radius() - motor_hole_slop/2 + motor_window_radius;
        y = [28byj48_shaft_offset() + motor_backpack_extent + motor_hole_slop/2 - motor_window_radius,
            28byj48_shaft_offset() - 28byj48_chassis_radius() - motor_hole_slop/2 + motor_window_radius];

        translate([ x, y[0], 0]) circle(r=motor_window_radius, $fn=40);
        translate([-x, y[1], 0]) circle(r=motor_window_radius, $fn=40);
        translate([-x, y[0], 0]) circle(r=motor_window_radius, $fn=40);
        translate([ x, y[1], 0]) circle(r=motor_window_radius, $fn=40);
    }
}

module side_tabs_negative(left_side) {
    translate([0, enclosure_length_right/2, 0]) {
        circle(r=m4_hole_diameter/2, $fn=30);
    }

    translate([0, enclosure_length_right / 2 - side_tab_bolt_head_clearance - side_tab_width/2, 0]) {
        square([thickness + enclosure_tab_clearance, side_tab_width + enclosure_tab_clearance], center=true);
    }

    position = left_side ?
        enclosure_length_right - side_tab_width - side_tab_left_inset:
        enclosure_length_right / 2 + side_tab_bolt_head_clearance + side_tab_width;
    translate([0, position, 0]) {
        square([thickness + enclosure_tab_clearance, side_tab_width*2 + enclosure_tab_clearance], center=true);
    }
}

module backstop_bolt_slot(radius) {
    hull() {
        circle(r=radius, $fn=15);
        translate([0, backstop_bolt_forward_range]) {
            circle(r=radius, $fn=15);
        }
    }
}

module connector_bracket_side_holes() {
    // overlap slot
    translate([enclosure_vertical_inset - thickness - connector_bracket_clearance/2, -connector_bracket_overlap]) {
        square([thickness + connector_bracket_clearance, connector_bracket_overlap + eps]);
    }
}

module alignment_bar() {
    color(assembly_colors[0])
        translate([enclosure_width - enclosure_horizontal_inset, -enclosure_length_right + front_forward_offset - alignment_bar_diameter/2, -enclosure_height_lower + alignment_bar_diameter/2])
            rotate([0, -90, 0])
                linear_extrude(height=enclosure_width * render_units)
                    circle(r=alignment_bar_diameter/2, $fn=60);
}

module enclosure_left() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length]);
            translate([enclosure_height_lower, enclosure_length - front_forward_offset, 0])
                circle(r=m4_hole_diameter/2, $fn=30);

            translate([enclosure_height_lower, enclosure_length - front_forward_offset]) {
                rotate([0, 0, 90]) {
                    motor_mount();
                }
            }

            // bottom side tabs
            translate([thickness * 0.5 + enclosure_vertical_inset, enclosure_length, 0])
                mirror([0, 1, 0])
                side_tabs_negative(true);

            // top side tabs
            translate([enclosure_height - thickness * 0.5 - enclosure_vertical_inset, enclosure_length, 0])
                mirror([0, 1, 0])
                    side_tabs_negative(true);

            if (enable_connectors) {
                // Connector bracket cuts
                translate([enclosure_height, enclosure_length]) {
                    mirror([1, 0, 0]) {
                        connector_bracket_side_holes();
                    }
                }
                translate([0, enclosure_length]) {
                    connector_bracket_side_holes();
                }
            }


            // Zip tie holes, bottom
            translate([20, zip_tie_spacing/2 + 5, 0])
                rotate([0, 0, 90])  // cable channel facing 'up/down'
                    zip_tie_holes();

            if (enable_alignment_bar) {
                // Alignment bar cutout
                translate([0, alignment_bar_center]) {
                    union() {
                        // Cutout
                        translate([alignment_bar_cutout_width/2, 0])
                            circle(r=alignment_bar_cutout_width/2, $fn=40);
                        square([alignment_bar_cutout_width, alignment_bar_cutout_width], center=true);

                        // Front-side fillet
                        // translate([0, alignment_bar_cutout_width/2, 0])
                        //     fillet_tool(r=alignment_bar_fillet_radius, overlap=1, $fn=40);

                        // Back-side fillet
                        translate([0, -alignment_bar_cutout_width/2, 0])
                            mirror([0, 1, 0])
                                fillet_tool(r=alignment_bar_fillet_radius, $fn=40);
                    }
                }
            }
        }
    }
}

module enclosure_left_etch() {
    // alignment indicator (triangle)
    enclosure_etch_style()
        translate([enclosure_indicator_position_y, enclosure_length - enclosure_indicator_inset])
            rotate([0, 0, -90])
                triangle(enclosure_indicator_size, center=true);
}

module shaft_centered_motor_hole() {
    margin = 5;
    width = 28byj48_mount_center_offset()*2 + 3.5*2 + margin*2;
    length = 18 + 14 + margin*2;

    translate([-width/2, -(margin + 18 + 8)])
        square([width, length]);
}

module enclosure_right() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length_right]);
            translate([enclosure_height_upper, enclosure_length_right - front_forward_offset, 0])
                circle(r=m4_axle_hole_diameter/2, $fn=30);

            // backstop bolt slot
            translate([enclosure_height_upper - backstop_bolt_vertical_offset, enclosure_length_right - front_forward_offset, 0]) {
                backstop_bolt_slot(radius = m4_hole_diameter/2);
            }

            // top side tabs
            translate([0.5*thickness + enclosure_vertical_inset, enclosure_length_right, 0])
                mirror([0, 1, 0])
                    side_tabs_negative();

            // bottom side tabs
            translate([enclosure_height - 0.5*thickness - enclosure_vertical_inset, enclosure_length_right, 0])
                mirror([0, 1, 0])
                    side_tabs_negative();

            if (enable_connectors) {
                // Connector bracket cuts
                translate([enclosure_height, enclosure_length_right]) {
                    mirror([1, 0, 0]) {
                        connector_bracket_side_holes();
                    }
                }
                translate([0, enclosure_length_right]) {
                    connector_bracket_side_holes();
                }
            }
        }
    }
}

module enclosure_right_etch() {
    // alignment indicator (circle)
    enclosure_etch_style()
        translate([enclosure_height - enclosure_indicator_position_y, enclosure_length_right - enclosure_indicator_inset])
            circle(r=enclosure_indicator_size/2, $fn=60);
}

module front_back_tabs() {
    for (i = [0 : 2 : num_front_tabs*2-2]) {
        translate([i * front_tab_width, -eps, 0])
            square([front_tab_width, thickness + eps]);
    }
}

module side_tabs(left_side) {
    translate([-eps, enclosure_length_right / 2 - side_tab_bolt_head_clearance - side_tab_width, 0]) {
        square([side_tab_length + eps, side_tab_width]);
    }

    position = left_side ?
        enclosure_length_right - side_tab_width*2 - side_tab_left_inset :
        enclosure_length_right / 2 + side_tab_bolt_head_clearance;

    translate([-eps, position, 0]) {
        square([side_tab_length + eps, side_tab_width*2]);
    }
}

module front_back_captive_nuts() {
    for (i = [0 : 2 : num_front_tabs-1]) {
        translate([(i*2 + 1.5) * front_tab_width, -thickness, 0])
            m4_captive_nut();
    }
}

module side_captive_nuts(hole_types=[]) {
    translate([-thickness, enclosure_length_right/2, 0]) {
        rotate([0, 0, -90]) {
            m4_captive_nut();
        }
    }
}


module enclosure_top() {
    // note, this is flipped upside down (around the x axis) when assembled so the clean side faces out
    linear_extrude(height = thickness) {
        translate([thickness, 0, 0]) {
            difference() {
                union() {
                    square([enclosure_wall_to_wall_width - 2 * thickness, enclosure_length_right]);

                    // front tabs
                    mirror([0, 1, 0])
                        front_back_tabs();

                    // left tabs
                    translate([enclosure_wall_to_wall_width - 2 * thickness, 0, 0])
                        side_tabs(true);

                    // right tabs
                    mirror([1, 0, 0])
                        translate([0, 0, 0])
                            side_tabs(false);
                }

                // front captive nuts
                front_back_captive_nuts();

                // right captive nuts
                side_captive_nuts(hole_types = [1]);

                // left captive nuts
                translate([enclosure_wall_to_wall_width - 2 * thickness, 0, 0])
                    mirror([1, 0, 0])
                        side_captive_nuts(hole_types = [1]);

                if (enable_mounting_holes) {
                    // mounting hole
                    translate([(enclosure_wall_to_wall_width - 2 * thickness)/2, enclosure_length_right - mounting_hole_inset]) {
                        circle(r=m4_hole_diameter/2, $fn=30);
                    }
                }
            }
        }
    }
}

module enclosure_bottom() {
    linear_extrude(height = thickness) {
        translate([thickness, 0, 0]) {
            difference() {
                union() {
                    square([enclosure_wall_to_wall_width - 2 * thickness, enclosure_length_right]);

                    // front tabs
                    translate([0, enclosure_length_right, 0])
                        front_back_tabs();

                    // left tabs
                    translate([enclosure_wall_to_wall_width - 2 * thickness, enclosure_length_right, 0])
                        mirror([0, 1, 0])
                            side_tabs(true);

                    // right tabs
                    translate([0, enclosure_length_right, 0])
                        mirror([0, 1, 0])
                            mirror([1, 0, 0])
                                side_tabs(false);
                }

                // front captive nuts
                translate([0, enclosure_length_right, 0])
                    mirror([0,1,0])
                        front_back_captive_nuts();

                // right captive nuts
                translate([0, enclosure_length_right, 0])
                    mirror([0, 1, 0])
                        side_captive_nuts(hole_types = [1]);

                // left captive nuts
                translate([enclosure_wall_to_wall_width - 2 * thickness, enclosure_length_right, 0])
                    mirror([0, 1, 0])
                        mirror([1, 0, 0])
                            side_captive_nuts(hole_types = [1]);

                if (enable_mounting_holes) {
                    // mounting hole
                    translate([(enclosure_wall_to_wall_width - 2 * thickness)/2, mounting_hole_inset]) {
                        circle(r=m4_hole_diameter/2, $fn=30);
                    }
                }
            }
        }
    }
}

module enclosure_bottom_etch() {
    if (enable_source_info) {
        enclosure_etch_style()
            translate([captive_nut_inset + m4_nut_length + 1, 1, thickness]) {
                text_label([str("rev. ", render_revision), render_date, "github.com/scottbez1/splitflap"]);
            }
    }
}

module split_flap_3d(front_flap_index, include_connector, include_front_panel=true) {
    module position_front() {
        if (include_front_panel) {
            translate([0, front_forward_offset + thickness, -enclosure_height_lower])
                rotate([90, 0, 0])
                    children();
        }
    }

    module positioned_front() {
        position_front()
            enclosure_front();
    }

    module positioned_front_etch() {
        position_front()
            enclosure_front_etch();
    }

    module position_left() {
        translate([enclosure_wall_to_wall_width, -enclosure_length + front_forward_offset, -enclosure_height_lower])
            rotate([0, -90, 0])
                children();
    }

    module positioned_left() {
        position_left()
            enclosure_left();
    }

    module positioned_left_etch() {
        position_left()
            enclosure_left_etch();
    }

    module position_right() {
        translate([0, -enclosure_length_right + front_forward_offset, enclosure_height_upper])
            rotate([0, 90, 0])
                children();
    }

    module positioned_right() {
        position_right()
            enclosure_right();
    }

    module positioned_right_etch() {
        position_right()
            enclosure_right_etch();
    }

    module positioned_top() {
        translate([0, front_forward_offset, enclosure_height_upper - enclosure_vertical_inset])
            rotate([180, 0, 0])
                enclosure_top();
    }

    module position_bottom() {
        translate([0, front_forward_offset - enclosure_length_right, -enclosure_height_lower + enclosure_vertical_inset])
            children();
    }

    module positioned_bottom() {
        position_bottom()
            enclosure_bottom();
    }

    module positioned_bottom_etch() {
        position_bottom()
            enclosure_bottom_etch();
    }

    module positioned_top_connector() {
        translate([enclosure_wall_to_wall_width - thickness - connector_bracket_thickness, front_forward_offset - connector_bracket_length_outer, enclosure_height_upper - enclosure_vertical_inset]) {
            connector_bracket();
        }
    }

    module positioned_bottom_connector() {
        translate([enclosure_wall_to_wall_width - thickness - connector_bracket_thickness, front_forward_offset - connector_bracket_length_outer, - enclosure_height_lower + enclosure_vertical_inset - thickness]) {
            connector_bracket();
        }
    }

    module positioned_left_bolts() {
        // Top
        translate([enclosure_wall_to_wall_width, front_forward_offset - enclosure_length_right/2, enclosure_height_upper - enclosure_vertical_inset - thickness/2]) {
            rotate([0, -90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }

        // Bottom
        translate([enclosure_wall_to_wall_width, front_forward_offset - enclosure_length_right/2, -enclosure_height_lower + enclosure_vertical_inset + thickness/2]) {
            rotate([0, -90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }
    }

    module positioned_right_bolts() {
        // Top
        translate([0, front_forward_offset - enclosure_length_right/2, enclosure_height_upper - enclosure_vertical_inset - thickness/2]) {
            rotate([0, 90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }

        // Bottom
        translate([0, front_forward_offset - enclosure_length_right/2, -enclosure_height_lower + enclosure_vertical_inset + thickness/2]) {
            rotate([0, 90, 0]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }
    }

    module positioned_front_bolts() {
        // Top
        translate([front_window_right_inset + 1.5*front_tab_width, front_forward_offset + thickness, enclosure_height_upper - enclosure_vertical_inset - thickness/2]) {
            rotate([0, 90, -90]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }

        // Bottom
        translate([front_window_right_inset + 1.5*front_tab_width, front_forward_offset + thickness, -enclosure_height_lower + enclosure_vertical_inset + thickness/2]) {
            rotate([0, 90, -90]) {
                standard_m4_bolt(nut_distance=captive_nut_inset);
            }
        }
    }

    module positioned_enclosure() {
        if (render_enclosure == 2) {
            color(assembly_colors[0])
                positioned_front();
            color(assembly_colors[1])
                positioned_left();
            color(assembly_colors[1])
                positioned_right();
            color(assembly_colors[2])
                positioned_top();
            color(assembly_colors[2])
                positioned_bottom();
            positioned_front_etch();
            positioned_left_etch();
            positioned_right_etch();
            positioned_bottom_etch();
            if (include_connector) {
                color(assembly_colors[3])
                    positioned_top_connector();
                color(assembly_colors[3])
                    positioned_bottom_connector();
            }
            if (render_bolts) {
                positioned_left_bolts();
                positioned_right_bolts();
                positioned_front_bolts();
            }
        } else if (render_enclosure == 1) {
            %positioned_front();
            %positioned_left();
            %positioned_right();
            %positioned_top();
            %positioned_bottom();
            %positioned_front_etch();
            %positioned_left_etch();
            %positioned_right_etch();
            %positioned_bottom_etch();
            if (include_connector) {
                %positioned_top_connector();
                %positioned_bottom_connector();
            }
            if (render_bolts) {
                positioned_left_bolts();
                positioned_right_bolts();
                positioned_front_bolts();
            }
        }
    }

    positioned_enclosure();
    if (render_pcb) {
        translate([enclosure_wall_to_wall_width - thickness - 28byj48_mount_bracket_height() - eps, 0, 0]) {
            rotate([0, -90, 0]) {
                rotate([0, 0, -90]) {
                    pcb(pcb_to_spool);
                }
            }
        }
    }

    translate([spool_width_slop/2 + thickness, 0, 0]) {
        // Flap area
        if (render_flaps > 0) {
            rotate([0, 90, 0]) {
                if (render_flap_area >= 1) {
                    translate([0, 0, thickness]) {
                        cylinder(r=exclusion_radius, h=flap_width - 2*thickness);
                    }
                }
                if (render_flap_area >= 2) {
                    translate([0, 0, thickness + (flap_width - 2*thickness)/4]) {
                        cylinder(r=outer_exclusion_radius, h=(flap_width - 2*thickness)/2);
                    }
                }
            }

            flap_offset = thickness > flap_notch_depth ? -flap_notch_depth + thickness + flap_width_slop/2 : flap_width_slop/2;
            translate([flap_offset, 0, 0]) {
                // Collapsed flaps on the top
                for (i=[0:num_flaps/2 - 1]) {
                    if (i == 0 || render_flaps == 2) {
                        show_letters = render_letters == 2 || (render_letters == 1 && i == 0);
                        rotate([360/num_flaps * i, 0, 0]) {
                            translate([flap_width, flap_pitch_radius, 0]) {
                                rotate([90, 0, 180]) {
                                    flap_with_letters(flap_color, letter_color, front_flap_index + i, flap_gap, front_letter=show_letters, back_letter=show_letters);
                                }
                            }
                        }
                    }
                }
                // Hanging flaps on the bottom
                for (i=[1:num_flaps/2]) {
                    flap_index = num_flaps - i;
                    angle = -360/num_flaps*i;
                    translate([flap_width, flap_pitch_radius*cos(angle), flap_pitch_radius * sin(angle)]) {
                        if (i == 1 || render_flaps == 2) {
                            show_letters = render_letters == 2 || (render_letters == 1 && i == 1);
                            rotate([-90, 0, 180]) {
                                flap_with_letters(flap_color, letter_color, front_flap_index + flap_index, flap_gap, front_letter=show_letters, back_letter=show_letters);
                            }
                        }
                    }
                }
            }
        }
    }

    if(render_spool) {
        translate([(spool_width_clearance - spool_width + spool_width_slop) / 2 + thickness, 0, 0]) {
            spool_struts();

            // motor spool
            translate([spool_width - thickness + 5*spool_horizontal_explosion, 0, 0]) {
                rotate([0, 90, 0]) {
                    color(assembly_colors[0])
                        flap_spool_complete(motor_shaft=true, magnet_hole=true);
                    translate([0, 0, -eps - thickness])
                        flap_spool_etch();
                }
            }
            color(assembly_colors[0]) {
                translate([thickness - 3*spool_horizontal_explosion, 0, 0]) {
                    rotate([0, 90, 0]) {
                        spool_retaining_wall(m4_bolt_hole=true);
                    }
                }
            }
            translate([-5*spool_horizontal_explosion, 0, 0]) {
                rotate([0, 90, 0]) {
                    color(assembly_colors[0])
                        flap_spool_complete(captive_nut=true);
                    flap_spool_etch();
                }
            }
            translate([thickness * 2, 0, 0]) {
                rotate([0, -90, 0]) {
                    standard_m4_bolt(nut_distance=thickness + 7*spool_horizontal_explosion);
                }
            }
        }
    }

    if (render_motor) {
        translate([enclosure_wall_to_wall_width - thickness - 28byj48_mount_bracket_height(), 0, 0]) {

            rotate([90, 0, 0]) {
                rotate([0, -90, 0]) {
                    Stepper28BYJ48();
                }
                translate([0, -28byj48_shaft_offset(), 0]) {
                    translate([0, 0, -28byj48_mount_center_offset()]) {
                        rotate([0, 90, 0]) {
                            rotate([0, 0, 90]) {
                                standard_m4_bolt(nut_distance=thickness+28byj48_mount_bracket_height());
                            }
                        }
                    }
                    translate([0, 0, 28byj48_mount_center_offset()]) {
                        rotate([0, 90, 0]) {
                            rotate([0, 0, 90]) {
                                translate([0, 0, -pcb_thickness()]) { // One motor bolt is also used to clamp the sensor PCB
                                    standard_m4_bolt(nut_distance=thickness+28byj48_mount_bracket_height() + pcb_thickness());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

module laser_etch() {
    if (!render_2d_mirror && (render_etch || render_index == -1)) {
        children();
    }
}

module laser_mirror() {
    if (render_2d_mirror) {
        mirror([1, 0, 0])
            children();
    }
    else {
        children();
    }
}

if (render_3d) {
    for (i = [0 : render_units - 1]) {
        translate([-enclosure_width/2 + (-(render_units-1) / 2 + i)*(enclosure_width + render_unit_separation), 0, 0])
            split_flap_3d(get_flap_index_for_letter(render_string[render_units - 1 - i]), include_connector=(i != render_units - 1));
    }
} else {
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

            if (render_front_panel) {
                translate([0, enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width - enclosure_horizontal_inset])
                    rotate([0, 0, -90])
                        enclosure_front();
            }

            if (render_front_panel) {
                laser_etch()
                    translate([0, enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width - enclosure_horizontal_inset])
                        rotate([0, 0, -90])
                            enclosure_front_etch();
            }

            // Top and bottom
            translate([enclosure_height + kerf_width + enclosure_length_right, enclosure_wall_to_wall_width + kerf_width - thickness * 3 + side_tab_length * 3])
                rotate([0, 0, 90])
                    enclosure_top();

            translate([enclosure_height + kerf_width, enclosure_wall_to_wall_width - thickness + side_tab_length])
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

            if (enable_connectors) {
                // Connector brackets on the top right
                translate([enclosure_height + kerf_width, 2 * enclosure_wall_to_wall_width + 2 * kerf_width - 4 * thickness + 4 * side_tab_length, 0])
                    connector_bracket();
            }

            if (enable_connectors) {
                // Connector brackets on the top right
                translate([enclosure_height + kerf_width, 2 * enclosure_wall_to_wall_width + 3 * kerf_width - 4 * thickness + 4 * side_tab_length + connector_bracket_length_outer, 0])
                    connector_bracket();
            }

            // Flap spools in flap window
            spools_too_large = spool_outer_radius*2 + kerf_width*3 + 1 > front_window_width || spool_outer_radius*4 + kerf_width*3 + 1 > front_window_height;
            if (spools_too_large) {
                echo("Note: spools are too large to place inside front window cutout; placing outside instead.");
            }
            flap_spool_y_off = spools_too_large ?
                spool_outer_radius + spool_strut_y_offset + spool_strut_width / 2 + kerf_width :
                enclosure_length + kerf_width + enclosure_length_right + kerf_width + enclosure_width - front_window_right_inset - enclosure_horizontal_inset - front_window_width/2;
            flap_spool_x_off = spools_too_large ?
                spool_outer_radius :
                spool_outer_radius + enclosure_height_lower - front_window_lower + kerf_width + 0.5;
            translate([flap_spool_x_off, flap_spool_y_off])
                flap_spool_complete(motor_shaft=true, magnet_hole=true);
            translate([flap_spool_x_off + spool_outer_radius*2 + kerf_width, flap_spool_y_off])
                mirror([0, 1, 0])
                    flap_spool_complete(captive_nut=true);

            // Flap spool etching
            if (!render_home_indicator_as_cut) {
                laser_etch() {
                    translate([flap_spool_x_off, flap_spool_y_off])
                        flap_spool_etch();
                    translate([flap_spool_x_off + spool_outer_radius*2 + kerf_width, flap_spool_y_off])
                        mirror([0, 1, 0])
                            flap_spool_etch();
                }
            }

            // Spool retaining wall in motor window
            translate([enclosure_height_lower - 28byj48_shaft_offset() + 28byj48_chassis_radius() - (28byj48_chassis_radius() + motor_backpack_extent)/2, enclosure_length - front_forward_offset - 28byj48_chassis_radius() - motor_hole_slop/2 + spool_strut_width/2 + kerf_width])
                spool_retaining_wall(m4_bolt_hole=true);
        }
    }
}
