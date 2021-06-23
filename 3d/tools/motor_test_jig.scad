use<../splitflap.scad>;
use<../pcb.scad>;
use<../28byj-48.scad>;

include<../m4_dimensions.scad>;

$fn = 30;

eps = 0.01;


export = true;


module dim(enabled=true) {
    if (enabled) {   
        %color([0,0,0, .2]) {
            children();
        }
    } else {
        children();
    }
}

// holes for 28byj-48 motor, centered around motor shaft
module motor_mount_holes() {
    translate([-28byj48_mount_center_offset(), -28byj48_shaft_offset()]) {
        circle(r=motor_mount_hole_radius, $fn=30);
    }
    translate([28byj48_mount_center_offset(), -28byj48_shaft_offset()]) {
        circle(r=motor_mount_hole_radius, $fn=30);
    }
}

module sector(radius, angles, fn = 24) {
    r = radius / cos(180 / fn);
    step = -360 / fn;

    points = concat([[0, 0]],
        [for(a = [angles[0] : step : angles[1] - 360]) 
            [r * cos(a), r * sin(a)]
        ],
        [[r * cos(angles[1]), r * sin(angles[1])]]
    );

    difference() {
        circle(radius, $fn = fn);
        polygon(points);
    }
}


module pcb_xy_placement() {
    translate([0, get_magnet_hole_offset()]) {
        // Translate so the sensor is at the origin
        translate([-pcb_hole_to_sensor_x(), pcb_hole_to_sensor_y()]) {
            rotate([180,0,0]) {
                children();
            }
        }
    }

}


inner_radius = 28byj48_chassis_radius() + 1;
outer_radius = 28byj48_mount_center_offset() + 5;

fillet = 5;

module motor_mount_positive() {
    translate([0, -28byj48_shaft_offset()]) {
                hull() {
                    sector(outer_radius, [0, 180]);
                    translate([-outer_radius+fillet, 0]) {
                        circle(r=fillet);
                    }
                    translate([outer_radius-fillet, 0]) {
                        circle(r=fillet);
                    }
                }
    }
}
module motor_mount_negative() {
    translate([0, -28byj48_shaft_offset()]) {
        hull() {
            translate([28byj48_mount_center_offset(), 0]) {
                circle(d=m4_hole_diameter);
            }
            translate([-28byj48_mount_center_offset(), 0]) {
                circle(d=m4_hole_diameter);
            }
        }
        sector(inner_radius, [0, 180]);
        translate([-inner_radius, -fillet - eps])
        square([2*inner_radius, fillet + eps]);
    }
}

module motor_test_jig() {
    linear_extrude(height=get_thickness()) {
        difference() {
            union() {
                motor_mount_positive();
                pcb_xy_placement() {
                    offset(r=2) {
                        hull() {
                            pcb_outline_2d(false);
                            translate([0, 5]) {
                                pcb_outline_2d(false);
                            }
                        }
                    }
                }
            }

            motor_mount_negative();
            pcb_xy_placement() {
                // Expand small cutouts for 3d printing
                offset(r=0.3) {
                    pcb_cutouts(adjustment_range=0);
                }
            }
        }
    }
}

module magnet_arm() {
    linear_extrude(height=get_thickness()) {
        difference() {
            hull() {
                circle(r=5);
                translate([0, get_magnet_hole_offset()]) {
                    circle(r=3);
                }
            }
            offset(r=0.2) {
                motor_shaft();
            }
            translate([0, get_magnet_hole_offset()]) {
                circle(d=get_magnet_diameter() + 0.2);
            }
            translate([-.4, get_magnet_hole_offset()]) {
                square([0.8, 10]);
            }
        }
    }
}

if (export) {
    motor_test_jig();
    translate([30, 0]) {
        magnet_arm();
    }
} else {
    dim() {
        translate([0, 0, 28byj48_mount_bracket_height()]) {
            Stepper28BYJ48();
        }
        translate([0, 0, -get_thickness()]) {
            pcb_xy_placement() {
                        pcb(get_pcb_to_spool());
            }
        }
    }
    translate([0, 0, -get_thickness()]) {
        motor_test_jig();
    }
    translate([0, 0, 6]) {
        magnet_arm();
    }
}
