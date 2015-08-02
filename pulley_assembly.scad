use<gt2pulley.scad>;
use<spool.scad>;
use<publicDomainGearV1.1.scad>;

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
motor_teeth = 60;
idler_teeth = 30;
spool_teeth = 40;

idler_shaft_radius = 1.5;

gear_separation = 0.2;

module spool_with_pulleys_assembly() {
    layer_separation = thickness;
    union() {
        color(assembly_color)
            flap_spool_complete();

/*
        color(assembly_color)
            translate([0,0,layer_separation])
                add_joining_rods_and_extrude(thickness)
                    gt2pulley(teeth, assembly_inner_radius, 0);

        color(assembly_color)
            translate([0,0,layer_separation*2])
                add_joining_rods_and_extrude(thickness)
                    gt2pulley(teeth, bearing_outer_radius, 0);
        color(bearing_color)
            translate([0,0,layer_separation*2]) 
                add_joining_rods_and_extrude(thickness)
                    difference() {
                        circle(r=bearing_outer_radius, $fn=60);
                        circle(r=rod_radius, $fn=60);
                    }
*/

        color(assembly_color)
            translate([0,0,thickness/2 + layer_separation])
                gear(drive_pitch, spool_teeth, thickness, 5);

        color(assembly_color)
            translate([0,0,thickness/2 + layer_separation*2])
                gear(drive_pitch, spool_teeth, thickness, 5);
    }
}

translate([flap_width - thickness, 0, 0]) rotate([0, 90, 0]) spool_with_pulleys_assembly();
color(assembly_color)
    rotate([0, 90, 0]) flap_spool_complete();

// Flap area
flap_pitch_radius = num_flaps * (flap_hole_radius*2 + flap_gap) / (2*PI);
exclusion_radius = sqrt(flap_height*flap_height + flap_pitch_radius*flap_pitch_radius);
echo(flap_exclusion_radius=exclusion_radius);
%translate([thickness, 0, 0])
    rotate([0, 90, 0])
        cylinder(r=exclusion_radius, h=flap_width - 2*thickness);


// double-flatted motor shaft of 28byj-48 motor (2D)
module motor_shaft() {
    intersection() {
        circle(d=5, $fn=50);
        square([5, 3], center=true);
    }
}

idler_center_y_offset = -pitch_radius(drive_pitch, spool_teeth) - pitch_radius(drive_pitch, idler_teeth) - gear_separation;

// motor gear
translate([flap_width + thickness, idler_center_y_offset - pitch_radius(drive_pitch, motor_teeth) - pitch_radius(drive_pitch, idler_teeth) - gear_separation, 0])
    rotate([0, 90, 0])
        linear_extrude(height = thickness, center = true)
            difference() {
                gear(drive_pitch, motor_teeth, 0, 0);
                motor_shaft();
            }

// idler gear
translate([flap_width + thickness, idler_center_y_offset, 0])
    rotate([0, 90, 0])
        rotate([0, 0, 360/idler_teeth/2])
        gear(drive_pitch, idler_teeth, thickness, idler_shaft_radius*2);

echo(motor_pitch_radius=pitch_radius(drive_pitch, motor_teeth));


