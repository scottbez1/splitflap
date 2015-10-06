use<gt2pulley.scad>;
use<spool.scad>;
use<publicDomainGearV1.1.scad>;
use<28byj-48.scad>;


// ##### RENDERING OPTIONS #####
render_enclosure = 2; // 2=opaque color; 1=translucent; 0=invisible
render_flaps = true;
render_units = 2;
render_unit_separation = 50;

/*
$vpt = [enclosure_width/2, 0, 0];
$vpr = [60, 0, 135 + animation_angle];
$vpd = 600;
*/



eps=.1;

// M3 bolts
m3_bolt_diameter=3+.1;
m3_bolt_length=12+1;
m3_bolt_cap_head_diameter=5.5+.2;
m3_bolt_cap_head_length=3+1;
m3_nut_width_flats=5.5 + .1;
m3_nut_width_corners=6.01;
m3_nut_length=2.4+.1;

captive_nut_inset=6;


thickness = 3.2;

rod_radius = 2.5;
rod_radius_slop = 0.25;
assembly_inner_radius = rod_radius + rod_radius_slop;
bearing_outer_radius = 6;


assembly_color = [.76, .60, .42];
assembly_color1 = [.882, .694, .486]; //"e1b17c";
assembly_color2 = [.682, .537, .376]; //"ae8960";
assembly_color3 = [.416, .325, .227]; //"6A533A";
assembly_color4 = [.204, .161, .114]; //"34291D";

bearing_color = [1,1,1,1];

joining_rod_radius = 1;

flap_width = 54;
flap_height = 43;
flap_thickness = 30 / 1000 * 25.4; // 30 mil
flap_corner_radius = 3.1; // 2.88-3.48mm

// Amount of slop of the flap side to side between the 2 spools
flap_width_slop = 2;

// Amount of slop on either side of the flap pin within the spool hole
flap_pin_slop = 1;

// Amount of slop for the spool assembly side-to-side inside the enclosure
spool_width_slop = 1;


num_flaps = 40;
flap_hole_radius = 1.5;
flap_gap = 1;

flap_pitch_radius = num_flaps * (flap_hole_radius*2 + flap_gap) / (2*PI);


// Gears
drive_pitch = 3;
motor_teeth = 48;
idler_teeth = 70;
spool_teeth = 48;

idler_shaft_radius = m3_bolt_diameter/2;

gear_separation = 0.5;

// Radius where flaps are expected to flap in their *most collapsed* (90 degree) state
exclusion_radius = sqrt(flap_height*flap_height + flap_pitch_radius*flap_pitch_radius);

spool_outer_radius = flap_pitch_radius + 2*flap_hole_radius;
flap_notch = sqrt(spool_outer_radius*spool_outer_radius - flap_pitch_radius*flap_pitch_radius);
echo(flap_notch=flap_notch);


flap_pin_width = flap_hole_radius*2 - 1;

idler_angle = 20;
motor_angle = -95;

idler_offset = - pitch_radius(drive_pitch, spool_teeth) - pitch_radius(drive_pitch, idler_teeth) - gear_separation;
idler_center_y_offset = cos(idler_angle) * idler_offset;
idler_center_z_offset = -sin(idler_angle) * idler_offset;
motor_offset = - pitch_radius(drive_pitch, motor_teeth) - pitch_radius(drive_pitch, idler_teeth) - gear_separation;
motor_center_y_offset = idler_center_y_offset + cos(motor_angle) * motor_offset; 
motor_center_z_offset = idler_center_z_offset - sin(motor_angle) * motor_offset;


enclosure_width = spool_width_slop + thickness*4 + flap_width + flap_width_slop;
front_window_upper = (flap_height - flap_pin_width/2);
front_window_lower = front_window_upper + (flap_pitch_radius*0.75); // some margin for falling flaps
front_window_width = spool_width_slop + flap_width + flap_width_slop;
enclosure_vertical_margin = 10; // gap between top/bottom of flaps and top/bottom of enclosure
enclosure_height_upper = exclusion_radius + enclosure_vertical_margin + 2*thickness;
enclosure_height_lower = flap_pitch_radius + flap_height + enclosure_vertical_margin + 2*thickness;
enclosure_height = enclosure_height_upper + enclosure_height_lower;

front_forward_offset = flap_pitch_radius + flap_thickness/2;
enclosure_horizontal_rear_margin = 20; // gap between back of gears and back of enclosure
enclosure_length = front_forward_offset - motor_center_y_offset + pitch_radius(drive_pitch, motor_teeth) + enclosure_horizontal_rear_margin;

motor_shaft_radius = 2.5;
motor_slop_radius = 3;


spool_strut_tab_width=10;
spool_strut_tab_outset=10;
spool_strut_width = (spool_strut_tab_outset + thickness/2) * 2;
spool_strut_length_inset = thickness*0.25;
spool_strut_length = flap_width + flap_width_slop + (2 * thickness) - (2 * spool_strut_length_inset);

// Enclosure connector tabs: front/back
num_front_tabs = 2;
front_tab_width = (enclosure_width - 2*thickness) / (num_front_tabs*2 - 1);

num_side_tabs = 3;
side_tab_width = (enclosure_length - 2*thickness) / (num_side_tabs*2 - 1);



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
module m3_captive_nut() {
    captive_nut(m3_bolt_diameter, m3_bolt_length, m3_nut_width_flats, m3_nut_length, captive_nut_inset);
}



// ##### Struts for bracing spool #####
module spool_strut_tab_hole() {
    square([thickness, spool_strut_tab_width], center=true);
}
module spool_strut_tab_holes() {
    for (i=[0:3]) {
        angle = 90*i;
        translate([cos(angle)*spool_strut_tab_outset, sin(angle)*spool_strut_tab_outset])
            rotate([0,0,angle])
                spool_strut_tab_hole();
    }
}
module spool_strut() {
    joint_tabs = 10;
    inner_length = flap_width + flap_width_slop - 2 * thickness;
    joint_tab_width = inner_length / joint_tabs;
    linear_extrude(thickness, center=true) {
        union() {
            translate([spool_strut_length_inset, -spool_strut_tab_width / 2]) {
                square([spool_strut_length, spool_strut_tab_width]);
            }
            translate([thickness, -spool_strut_width / 2]) {
                difference() {
                    square([inner_length, spool_strut_width]);

                    // subtract out tabs
                    union() {
                        for (i = [0:2:joint_tabs-1]) {
                            translate([i*joint_tab_width, -eps])
                                square([joint_tab_width, thickness+eps]);
                        }
                        for (i = [1:2:joint_tabs-1]) {
                            translate([i*joint_tab_width, spool_strut_width - thickness])
                                square([joint_tab_width, thickness+eps]);
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
        translate([0, sin(angle)*spool_strut_tab_outset, cos(angle)*spool_strut_tab_outset])
            rotate([-angle, 0, 0])
                spool_strut();
    }
}


module flap_spool_complete() {
    linear_extrude(thickness) {
        difference() {
            flap_spool(num_flaps, flap_hole_radius, flap_gap, assembly_inner_radius,
                    height=0);

            spool_strut_tab_holes();
        }
    }
}

module spool_with_pulleys_assembly() {
    layer_separation = thickness;

    module gear_with_strut_tab_holes() {
        linear_extrude(height=thickness, center=true) {
            difference() {
                gear(drive_pitch, spool_teeth, 0, 5);
                spool_strut_tab_holes();
            }
        }
    }

    union() {
        flap_spool_complete();

        // Gears on spool
        translate([0,0,thickness/2 + layer_separation])
            gear_with_strut_tab_holes();
        translate([0,0,thickness/2 + layer_separation*2])
            gear_with_strut_tab_holes();
    }
}

module flap() {
    color("white")
    translate([0, -flap_pin_width/2, -flap_thickness/2])
    linear_extrude(height=flap_thickness) {
        difference() {
            union() {
                square([flap_width, flap_height - flap_corner_radius + eps]);

                // rounded corners
                hull() {
                    translate([flap_corner_radius, flap_height - flap_corner_radius])
                        circle(r=flap_corner_radius, $fn=40);
                    translate([flap_width - flap_corner_radius, flap_height - flap_corner_radius])
                        circle(r=flap_corner_radius, $fn=40);
                }
            }
            translate([-eps, flap_pin_width])
                square([eps + thickness, flap_notch]);
            translate([flap_width - thickness, flap_pin_width])
                square([eps + thickness, flap_notch]);
        }
    }
}

module translated_flap() {
    translate([0, flap_pitch_radius, 0])
    rotate([90, 0, 0])
    flap();
}



// double-flatted motor shaft of 28byj-48 motor (2D)
module motor_shaft() {
    intersection() {
        circle(r=motor_shaft_radius, $fn=50);
        square([motor_shaft_radius*2, 3], center=true);
    }
}

// 28byj-48 stepper motor centered on its shaft
module stepper_shaft_centered() {
    translate([18.8/2+0.7, -8, 0])
        rotate([0, 90, 0])
            rotate([0, 0, 90])
                StepMotor28BYJ();
}

module front_tabs_negative() {
    for (i = [0 : num_front_tabs-1]) {
        translate([thickness + (i*2+0.5) * front_tab_width, 0, 0])
            square([front_tab_width, thickness], center=true);
    }
    for (i = [0 : num_front_tabs-2]) {
        translate([thickness + (i*2+1.5) * front_tab_width, 0, 0])
            circle(r=m3_bolt_diameter/2, $fn=30);
    }
}

module enclosure_front() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_width, enclosure_height]);

            // Viewing window cutout
            translate([thickness, enclosure_height_lower - front_window_lower])
                square([front_window_width, front_window_lower + front_window_upper]);

            // Front lower tabs
            translate([0, thickness * 1.5, 0])
                front_tabs_negative();

            // Front upper tabs
            translate([0, enclosure_height - thickness * 1.5, 0])
                front_tabs_negative();
        }
    }
}

module rod_mount_negative() {
    circle(r=rod_radius, center=true, $fn=30);
}


// holes for 28byj-48 motor, centered around motor shaft
module motor_mount() {
    motor_mount_separation = 35;
    motor_mount_hole_radius = 4.2/2;
    circle(r=motor_shaft_radius+motor_slop_radius, center=true, $fn=30);
    translate([-motor_mount_separation/2, -8])
        circle(r=motor_mount_hole_radius, center=true, $fn=30);
    translate([motor_mount_separation/2, -8])
        circle(r=motor_mount_hole_radius, center=true, $fn=30);
}

module side_tabs_negative(reverse=false) {
    for (i = [0 : num_side_tabs-1]) {
        translate([0, thickness + (i*2 + 0.5) * side_tab_width, 0])
            square([thickness, side_tab_width], center=true);
    }
    for (i = [0 : num_side_tabs-1]) {
        bolt_head_hole = (i % 2 == (reverse ? 1 : 0));
        translate([0, thickness + (i*2 + 1.5) * side_tab_width, 0])
            circle(r=(bolt_head_hole ? m3_bolt_cap_head_diameter : m3_bolt_diameter)/2, $fn=30);
    }
}

module enclosure_left() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length]);
            translate([enclosure_height_lower, enclosure_length - front_forward_offset, 0])
                rod_mount_negative();

            // idler bolt hole
            translate([enclosure_height_lower + idler_center_z_offset, enclosure_length - front_forward_offset + idler_center_y_offset])
                circle(r=idler_shaft_radius, center=true, $fn=30);

            translate([enclosure_height_lower + motor_center_z_offset, enclosure_length - front_forward_offset + motor_center_y_offset])
                motor_mount();

            // bottom side tabs
            translate([thickness * 1.5, 0, 0])
                side_tabs_negative(reverse=true);

            // top side tabs
            translate([enclosure_height - thickness * 1.5, 0, 0])
                side_tabs_negative(reverse=false);
        }
    }
}

module shaft_centered_motor_hole() {
    margin = 5;
    width = 35 + 3.5*2 + margin*2;
    length = 18 + 14 + margin*2;

    translate([-width/2, -(margin + 18 + 8)])
        square([width, length]);
}

module enclosure_right() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length]);
            translate([enclosure_height_upper, enclosure_length - front_forward_offset, 0])
                rod_mount_negative();

            // hole for adjacent unit's idler bolt
            translate([enclosure_height_upper - idler_center_z_offset, enclosure_length - front_forward_offset + idler_center_y_offset])
                square([m3_nut_width_corners + 2, m3_nut_width_corners + 2], center=true);

            // hole for adjacent unit's motor
            translate([enclosure_height_upper - motor_center_z_offset, enclosure_length - front_forward_offset + motor_center_y_offset])
                shaft_centered_motor_hole();

            // top side tabs
            translate([1.5*thickness, 0, 0])
                side_tabs_negative(reverse=true);

            // bottom side tabs
            translate([enclosure_height - 1.5*thickness, 0, 0])
                side_tabs_negative(reverse=false);
        }
    }
}

module front_back_tabs() {
    for (i = [0 : 2 : num_front_tabs*2-2]) {
        translate([i * front_tab_width, -eps, 0])
            square([front_tab_width, thickness + eps]);
    }
}

module side_tabs() {
    for (i = [0 : 2 : num_side_tabs*2-2]) {
        translate([-eps, i * side_tab_width, 0])
            square([thickness + eps, side_tab_width]);
    }
}

module front_back_captive_nuts() {
    for (i = [0 : 2 : num_front_tabs-1]) {
        translate([(i*2 + 1.5) * front_tab_width, -thickness, 0])
            m3_captive_nut();
    }
}

module side_captive_nuts(reverse = false) {
    for (i = [0 : num_side_tabs-1]) {
        translate([-thickness, (i*2 + 1.5) * side_tab_width, 0]) {
            rotate([0, 0, -90]) {
                bolt_head_hole = (i % 2 == (reverse ? 1 : 0));
                if (bolt_head_hole) {
                    translate([-m3_bolt_cap_head_diameter/2, 0])
                        square([m3_bolt_cap_head_diameter, m3_bolt_cap_head_length]);
                } else {
                    m3_captive_nut();
                }
            }
        }
    }
}


module enclosure_top() {
    // note, this is flipped upside down (around the x axis) when assembled so the clean side faces out
    linear_extrude(height = thickness) {
        difference() {
            union() {
                square([enclosure_width - 2 * thickness, enclosure_length]);

                // back tabs
                translate([0, enclosure_length, 0])
                    front_back_tabs();

                // front tabs
                mirror([0, 1, 0])
                    front_back_tabs();

                // left tabs
                translate([enclosure_width - 2 * thickness, thickness, 0])
                    side_tabs();

                // right tabs
                mirror([1, 0, 0])
                    translate([0, thickness, 0])
                        side_tabs();
            }

            // front captive nuts
            front_back_captive_nuts();

            // back captive nuts
            translate([0, enclosure_length, 0])
                mirror([0,1,0])
                    front_back_captive_nuts();

            // right captive nuts
            translate([0, thickness, 0])
                side_captive_nuts(reverse = false);

            // left captive nuts
            translate([enclosure_width - 2 * thickness, thickness, 0])
                mirror([1, 0, 0])
                    side_captive_nuts(reverse = true);
        }
    }
}

module enclosure_bottom() {
    linear_extrude(height = thickness) {
        difference() {
            union() {
                square([enclosure_width - 2 * thickness, enclosure_length]);

                // front tabs
                translate([0, enclosure_length, 0])
                    front_back_tabs();

                // back tabs
                mirror([0, 1, 0])
                    front_back_tabs();

                // right tabs
                translate([enclosure_width - 2 * thickness, thickness, 0])
                    side_tabs();

                // left tabs
                mirror([1, 0, 0])
                    translate([0, thickness, 0])
                        side_tabs();
            }

            // back captive nuts
            front_back_captive_nuts();

            // front captive nuts
            translate([0, enclosure_length, 0])
                mirror([0,1,0])
                    front_back_captive_nuts();

            // left captive nuts
            translate([0, thickness, 0])
                side_captive_nuts(reverse = false);

            // right captive nuts
            translate([enclosure_width - 2 * thickness, thickness, 0])
                mirror([1, 0, 0])
                    side_captive_nuts(reverse = true);

        }
    }
}

module enclosure_back() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_width, enclosure_height]);

            // Back lower tabs
            translate([0, enclosure_height - thickness * 1.5, 0])
                front_tabs_negative();

            // Back upper tabs
            translate([0, thickness * 1.5, 0])
                front_tabs_negative();
        }
    }
}



module split_flap_3d() {
    module positioned_front() {
        translate([0, front_forward_offset + thickness, -enclosure_height_lower])
            rotate([90, 0, 0])
                enclosure_front();
    }

    module positioned_left() {
        translate([enclosure_width, -enclosure_length + front_forward_offset, -enclosure_height_lower])
            rotate([0, -90, 0])
                enclosure_left();
    }

    module positioned_right() {
        translate([0, -enclosure_length + front_forward_offset, enclosure_height_upper])
            rotate([0, 90, 0])
                enclosure_right();
    }

    module positioned_top() {
        translate([thickness, front_forward_offset, enclosure_height_upper - thickness])
            rotate([180, 0, 0])
                enclosure_top();
    }

    module positioned_bottom() {
        translate([thickness, front_forward_offset - enclosure_length, -enclosure_height_lower + thickness])
            enclosure_bottom();
    }

    module positioned_back() {
        translate([0, -enclosure_length + front_forward_offset - thickness, enclosure_height_upper])
            rotate([-90, 0, 0])
                enclosure_back();
    }

    module positioned_enclosure() {
        if (render_enclosure == 2) {
            color(assembly_color1)
                positioned_front();
            color(assembly_color2)
                positioned_left();
            color(assembly_color2)
                positioned_right();
            color(assembly_color3)
                positioned_top();
            color(assembly_color3)
                positioned_bottom();
            color(assembly_color1)
                positioned_back();
        } else if (render_enclosure == 1) {
            %positioned_front();
            %positioned_left();
            %positioned_right();
            %positioned_top();
            %positioned_bottom();
            %positioned_back();
        }
    }

    positioned_enclosure();

    translate([spool_width_slop/2 + thickness, 0, 0]) {
        // Flap area
        if (render_flaps) {
            echo(flap_exclusion_radius=exclusion_radius);
            *translate([thickness, 0, 0])
                rotate([0, 90, 0])
                    cylinder(r=exclusion_radius, h=flap_width - 2*thickness);

            translate([flap_width_slop/2, 0, 0]) {
                // Collapsed flaps on the top
                for (i=[0:19]) {
                    rotate([360/num_flaps * i, 0, 0]) translated_flap();
                }

                for (i=[1:20]) {
                    angle = -360/num_flaps*i;
                    translate([0, flap_pitch_radius*cos(angle), flap_pitch_radius * sin(angle)])
                        rotate([-90, 0, 0])
                            flap();
                }
            }
        }

        spool_struts();

        // spool with gears
        color(assembly_color)
            translate([flap_width + flap_width_slop - thickness, 0, 0]) rotate([0, 90, 0]) spool_with_pulleys_assembly();
        color(assembly_color)
            rotate([0, 90, 0]) flap_spool_complete();

        // idler gear
        color(assembly_color2)
        translate([flap_width + flap_width_slop + thickness, idler_center_y_offset, idler_center_z_offset])
            rotate([0, 90, 0])
                rotate([0, 0, 360/idler_teeth/2])
                gear(drive_pitch, idler_teeth, thickness, idler_shaft_radius*2);

        // motor gear
        color(assembly_color1)
        translate([flap_width + flap_width_slop + thickness, motor_center_y_offset, motor_center_z_offset])
            rotate([0, 90, 0])
                linear_extrude(height = thickness, center = true)
                    difference() {
                        gear(drive_pitch, motor_teeth, 0, 0);
                        motor_shaft();
                    }

        echo(motor_pitch_radius=pitch_radius(drive_pitch, motor_teeth));

        translate([flap_width + flap_width_slop + 3*thickness, motor_center_y_offset, motor_center_z_offset])
            stepper_shaft_centered();
    }
}

for (i = [0 : render_units - 1]) {
    translate([-enclosure_width/2 + (-(render_units-1) / 2 + i)*(enclosure_width + render_unit_separation), 0, 0])
        split_flap_3d();
}

