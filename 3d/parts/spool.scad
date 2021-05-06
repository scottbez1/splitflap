/*
   Copyright 2015-2016 Scott Bezek and the splitflap contributors

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

include<../common.scad>;
include<flap.scad>;
include<motor.scad>;
include<hardware.scad>;

thickness = 3.0;

spool_strut_num_joints = 3;
spool_strut_tab_width=8;
spool_strut_tab_width_narrow=6;
spool_strut_tab_outset=8;

spool_width_slop = 1.4;  // amount of slop for the spool assembly side-to-side inside the enclosure
spool_width = flap_width - flap_notch_depth*2 + flap_width_slop + thickness*2;  // spool width, outside face (spool to spool)
spool_width_clearance = max(spool_width, flap_width + flap_width_slop);  // width clearance for the spool, either for the spool itself or the flaps

spool_strut_width = (spool_strut_tab_outset + thickness/2) * 2;
spool_strut_length = spool_width;
spool_strut_inner_length = spool_width - 3 * thickness;


spool_tab_clearance = -0.06;  // for the tabs connecting the struts to the spool ends (interference fit)
spool_retaining_clearance = 0.10;  // for the notches in the spool retaining wall
spool_joint_clearance = 0.10;  // for the notched joints on the spool struts

spool_strut_exclusion_radius = sqrt((spool_strut_tab_outset+thickness/2)*(spool_strut_tab_outset+thickness/2) + (spool_strut_tab_width/2)*(spool_strut_tab_width/2));
flap_pitch_radius = flap_spool_pitch_radius(num_flaps, flap_hole_radius, flap_hole_separation); //num_flaps * (flap_hole_radius*2 + flap_hole_separation) / (2*PI);
echo(spool_strut_exclusion_radius );
echo(flap_pitch_radius );
echo((spool_strut_exclusion_radius + flap_pitch_radius)/2);
magnet_diameter = 4;
magnet_hole_clearance = -0.07;  // interference fit
magnet_hole_radius = (magnet_diameter + magnet_hole_clearance)/2;

magnet_hole_offset = (spool_strut_exclusion_radius + flap_pitch_radius)/2;



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
flap_spool_outset = flap_hole_radius;


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
        color(i % 2 == 0 ? assembly_color2 : assembly_color3)
        translate([0, sin(angle)*(spool_strut_tab_outset + spool_strut_explosion), cos(angle)*(spool_strut_tab_outset + spool_strut_explosion)])
            rotate([-angle, 0, 0])
                spool_strut();
    }
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
                // Hole for press fit magnet
                translate([magnet_hole_offset, 0]) {
                    circle(r=magnet_hole_radius, $fn=15);
                }
            }
        }
    }
}

module flap_spool_etch() {
    enclosure_etch_style()
        flap_spool_home_indicator(num_flaps, flap_hole_radius, flap_hole_separation, flap_spool_outset);
}



module flap_spool(flaps, flap_hole_radius, flap_hole_separation, outset, height) {
    pitch_radius = flap_spool_pitch_radius(flaps, flap_hole_radius, flap_hole_separation);
    outer_radius = flap_spool_outer_radius(flaps, flap_hole_radius, flap_hole_separation, outset);


    module flap_spool_2d() {
        difference() {
            circle(r=outer_radius, $fn=60);
            for (i = [0 : flaps - 1]) {
                translate([cos(360/flaps*i)*pitch_radius, sin(360/flaps*i)*pitch_radius]) circle(r=flap_hole_radius, $fn=15);
            }
        }
    }

    if (height > 0) {
        linear_extrude(height) {
            flap_spool_2d();
        }
    } else {
        flap_spool_2d();
    }
}

module flap_spool_home_indicator(flaps, flap_hole_radius, flap_hole_separation, outset, height=0) {
    pitch_radius = flap_spool_pitch_radius(flaps, flap_hole_radius, flap_hole_separation);
    outer_radius = flap_spool_outer_radius(flaps, flap_hole_radius, flap_hole_separation, outset);

    module flap_spool_home_indicator_2d() {
        angle = (360 / flaps) * round(0.25 * flaps);  // always point directly to a flap hole

        translate([cos(angle) * pitch_radius, sin(angle) * pitch_radius])
        rotate([0, 0, angle])
        translate([-flap_hole_radius * 2, 0])
        hull() {
            circle(r=flap_hole_radius/2, $fn=30);
            translate([-flap_hole_radius * 1.25, 0])
            circle(r=flap_hole_radius/2, $fn=30);
        }
    }

    if (height > 0) {
        // convexity parameter fixes 'difference()' face polarity in preview mode... somehow
        linear_extrude(height, convexity=2)
            flap_spool_home_indicator_2d();
    } else {
        flap_spool_home_indicator_2d();
    }
}

function flap_spool_pitch_radius(flaps, flap_hole_radius, separation) = 
    flaps * (flap_hole_radius*2 + separation) / (2*PI);

function flap_spool_outer_radius(flaps, flap_hole_radius, separation, outset) = 
    flap_spool_pitch_radius(flaps, flap_hole_radius, separation) + flap_hole_radius + outset;

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
