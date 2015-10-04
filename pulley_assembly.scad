use<gt2pulley.scad>;
use<spool.scad>;
use<publicDomainGearV1.1.scad>;
use<28byj-48.scad>;

eps=.1;

thickness = 3.2;

teeth = 45;

rod_radius = 2.5;
rod_radius_slop = 0.25;
assembly_inner_radius = rod_radius + rod_radius_slop;
bearing_outer_radius = 6;


assembly_color = [.76, .60, .42, 1];
bearing_color = [1,1,1,1];

joining_rod_radius = 1;

flap_width = 54;
flap_height = 43;
flap_thickness = 30 / 1000 * 25.4; // 30 mil

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
motor_teeth = 40;
idler_teeth = 40;
spool_teeth = 40;

idler_shaft_radius = 1.5;

gear_separation = 0.5;

// Radius where flaps are expected to flap in their *most collapsed* (90 degree) state
exclusion_radius = sqrt(flap_height*flap_height + flap_pitch_radius*flap_pitch_radius);

spool_outer_radius = flap_pitch_radius + 2*flap_hole_radius;
flap_notch = sqrt(spool_outer_radius*spool_outer_radius - flap_pitch_radius*flap_pitch_radius);
echo(flap_notch=flap_notch);


flap_pin_width = flap_hole_radius*2 - 1;


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
enclosure_length = front_forward_offset + pitch_radius(drive_pitch, spool_teeth) + 2*gear_separation + 2*pitch_radius(drive_pitch, idler_teeth) + 2*gear_separation + 2*pitch_radius(drive_pitch, motor_teeth) + enclosure_horizontal_rear_margin;

motor_shaft_radius = 2.5;
motor_slop_radius = 3;



module joining_rod_holes() {
        translate([bearing_outer_radius + joining_rod_radius*3, 0, 0]) circle(r=joining_rod_radius, $fn=30);
        translate([-(bearing_outer_radius + joining_rod_radius*3), 0, 0]) circle(r=joining_rod_radius, $fn=30);
}

module add_joining_rods_and_extrude(height) {
    linear_extrude(height) {
        add_joining_rods() {
            children();
        }
    }
}
module add_joining_rods() {
    difference() {
        children(0);
        joining_rod_holes();
    }
}



module flap_spool_complete() {
    linear_extrude(thickness) {
        difference() {
            flap_spool(num_flaps, flap_hole_radius, flap_gap, assembly_inner_radius,
                    height=0, 
                    spool_strut_tab_dimens=[thickness, 10],
                    spool_strut_tab_outset=12);
            joining_rod_holes();
        }
    }
}

module spool_with_pulleys_assembly() {
    layer_separation = thickness;
    color(assembly_color)
    union() {
        flap_spool_complete();

        // Gears on spool
        translate([0,0,thickness/2 + layer_separation])
            gear(drive_pitch, spool_teeth, thickness, 5);
        translate([0,0,thickness/2 + layer_separation*2])
            gear(drive_pitch, spool_teeth, thickness, 5);
    }
}

module flap() {
    color("white")
    translate([0, -flap_pin_width/2, -flap_thickness/2])
    difference() {
        cube([flap_width, flap_height, flap_thickness]);
        translate([-eps, flap_pin_width, -eps])
            cube([eps + thickness, flap_notch, flap_thickness + 2*eps]);
        translate([flap_width - thickness, flap_pin_width, -eps])
            cube([eps + thickness, flap_notch, flap_thickness + 2*eps]);
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

idler_center_y_offset = -pitch_radius(drive_pitch, spool_teeth) - pitch_radius(drive_pitch, idler_teeth) - gear_separation;
motor_center_y_offset = idler_center_y_offset - pitch_radius(drive_pitch, motor_teeth) - pitch_radius(drive_pitch, idler_teeth) - gear_separation;

// 28byj-48 stepper motor centered on its shaft
module stepper_shaft_centered() {
    translate([18.8/2+0.7, -8, 0])
        rotate([0, 90, 0])
            rotate([0, 0, 90])
                StepMotor28BYJ();
}

module enclosure_front() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_width, enclosure_height]);
            translate([thickness, enclosure_height_lower - front_window_lower])
                square([front_window_width, front_window_lower + front_window_upper]);
        }
    }
}

module rod_mount_negative() {
    circle(r=rod_radius, center=true, $fn=30);
}


// holes for 28byj-48 motor
module motor_mount() {
    circle(r=motor_shaft_radius+motor_slop_radius, center=true, $fn=30);
    translate([-35/2, -8])
        circle(r=2, center=true, $fn=30);
    translate([35/2, -8])
        circle(r=2, center=true, $fn=30);
}

module enclosure_left() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length]);
            translate([enclosure_height_lower, enclosure_length - front_forward_offset, 0])
                rod_mount_negative();

            // idler bolt hole
            translate([enclosure_height_lower, enclosure_length - front_forward_offset - pitch_radius(drive_pitch, spool_teeth) - gear_separation - pitch_radius(drive_pitch, idler_teeth)])
                circle(r=idler_shaft_radius, center=true, $fn=30);

            translate([enclosure_height_lower, enclosure_length - front_forward_offset - pitch_radius(drive_pitch, spool_teeth) - gear_separation - 2*pitch_radius(drive_pitch, idler_teeth) - gear_separation - pitch_radius(drive_pitch, motor_teeth)])
                motor_mount();
        }
    }
}

module enclosure_right() {
    linear_extrude(height=thickness) {
        difference() {
            square([enclosure_height, enclosure_length]);
            translate([enclosure_height_upper, enclosure_length - front_forward_offset, 0])
                rod_mount_negative();
        }
    }
}

translate([0, front_forward_offset + thickness, -enclosure_height_lower])
    rotate([90, 0, 0])
        enclosure_front();

color("green")
translate([enclosure_width, -enclosure_length + front_forward_offset, -enclosure_height_lower])
    rotate([0, -90, 0])
        enclosure_left();

color("red")
translate([0, -enclosure_length + front_forward_offset, enclosure_height_upper])
    rotate([0, 90, 0])
        enclosure_right();

translate([spool_width_slop/2 + thickness, 0, 0]) {
    // Flap area
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

    // spool with gears
    translate([flap_width + flap_width_slop - thickness, 0, 0]) rotate([0, 90, 0]) spool_with_pulleys_assembly();
    color(assembly_color)
        rotate([0, 90, 0]) flap_spool_complete();

    // idler gear
    translate([flap_width + flap_width_slop + thickness, idler_center_y_offset, 0])
        rotate([0, 90, 0])
            rotate([0, 0, 360/idler_teeth/2])
            gear(drive_pitch, idler_teeth, thickness, idler_shaft_radius*2);

    // motor gear
    translate([flap_width + flap_width_slop + thickness, motor_center_y_offset, 0])
        rotate([0, 90, 0])
            linear_extrude(height = thickness, center = true)
                difference() {
                    gear(drive_pitch, motor_teeth, 0, 0);
                    motor_shaft();
                }

    echo(motor_pitch_radius=pitch_radius(drive_pitch, motor_teeth));

    translate([flap_width + flap_width_slop + 3*thickness, motor_center_y_offset, 0])
        stepper_shaft_centered();
}

