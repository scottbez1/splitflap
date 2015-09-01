use<gt2pulley.scad>;
use<spool.scad>;
use<publicDomainGearV1.1.scad>;
use<28byj-48.scad>;

eps=.1;

thickness = 3.2;

teeth = 45;

rod_radius = 2.5;
assembly_inner_radius = 3.5;
bearing_outer_radius = 6;


assembly_color = [.76, .60, .42, 1];
bearing_color = [1,1,1,1];

joining_rod_radius = 1;

flap_width = 54;
flap_height = 43;
flap_thickness = 30 / 1000 * 25.4; // 30 mil
flap_width_slop = 2;
flap_pin_slop = 1;


num_flaps = 40;
flap_hole_radius = 1.5;
flap_gap = 1;


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


drive_pitch = 3;
motor_teeth = 40;
idler_teeth = 40;
spool_teeth = 40;

idler_shaft_radius = 1.5;

gear_separation = 0.5;

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

// Flap area
flap_pitch_radius = num_flaps * (flap_hole_radius*2 + flap_gap) / (2*PI);
exclusion_radius = sqrt(flap_height*flap_height + flap_pitch_radius*flap_pitch_radius);
echo(flap_exclusion_radius=exclusion_radius);
%translate([thickness + flap_width_slop/2, 0, 0])
    rotate([0, 90, 0])
        cylinder(r=exclusion_radius, h=flap_width - 2*thickness);


spool_outer_radius = flap_pitch_radius + 2*flap_hole_radius;
flap_notch = sqrt(spool_outer_radius*spool_outer_radius - flap_pitch_radius*flap_pitch_radius);
echo(flap_notch=flap_notch);

module flap() {
    flap_pin_width = flap_hole_radius*2 - 1;

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
    color("white")
    translate([flap_width_slop/2, flap_pitch_radius, 0])
    rotate([90, 0, 0])
    flap();
}

for (i=[0:7]) {
    rotate([360/num_flaps * i, 0, 0]) translated_flap();
}


// double-flatted motor shaft of 28byj-48 motor (2D)
module motor_shaft() {
    intersection() {
        circle(d=5, $fn=50);
        square([5, 3], center=true);
    }
}

idler_center_y_offset = -pitch_radius(drive_pitch, spool_teeth) - pitch_radius(drive_pitch, idler_teeth) - gear_separation;
motor_center_y_offset = idler_center_y_offset - pitch_radius(drive_pitch, motor_teeth) - pitch_radius(drive_pitch, idler_teeth) - gear_separation;

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

module stepper_shaft_centered() {
    translate([9.5, -8, 0])
        rotate([0, 90, 0])
            rotate([0, 0, 90])
                StepMotor28BYJ();
}

translate([flap_width + flap_width_slop + 3*thickness, motor_center_y_offset, 0])
    stepper_shaft_centered();

