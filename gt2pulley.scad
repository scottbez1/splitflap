use <util.scad>;


module gt2pulley(teeth, inner_radius, height, center=false) {
    // Pulley is made of a circle that extends to the tooth-center-radius (shorter
    // than the pitch radius), with holes (for the belt's teeth) subtracted. The
    // pulley's "teeth" are extended by adding two smaller tangent circles on either
    // side of the tooth hole. The space between is filled by a wedge tangent to both
    // circles.
    //
    // This is not quite proper GT2 profile (there's a third tangent circle between the
    // primary tooth indent and the smaller fillet circles, but that seems less
    // critical so has been excluded for simplicity)

    pitch = 2;
    tooth_r = 0.555;
    pld=.254;
    tooth_to_belt_gap = 0.75;
    belt_thickness = 1.38;


    pitch_r = teeth * pitch / (2*PI);
    belt_outer_r = pitch_r + (belt_thickness - pld - tooth_to_belt_gap);
    belt_surface_r = pitch_r - pld;
    echo("Belt surface radius:", belt_surface_r);
    belt_inner_r = belt_surface_r - tooth_to_belt_gap;
    tooth_center_r = belt_inner_r + tooth_r;


    tooth_fillet_r=.15;
    tangent_circle_delta_theta = 2*asin(0.5*(tooth_r + tooth_fillet_r)/tooth_center_r);

    wedge_angle = 360/teeth-2*tangent_circle_delta_theta;

    module teeth_holes() {
        for (i = [0 : teeth - 1]) {
            angle=360/teeth*i;
            translate([cos(angle)*tooth_center_r, sin(angle)*tooth_center_r])
                rotate([0,0,angle])
                    circle(r = tooth_r, $fn=2*teeth);
        }
    }

    // Radius of tangent point between tooth circle and fillet circles
    tangency_radius = sqrt(pow(tooth_fillet_r, 2) + pow(tooth_center_r, 2) - 2 * tooth_fillet_r*tooth_center_r*cos((180-tangent_circle_delta_theta)/2));

    module gt2pulley_2d() {
        difference() {
            union() {
                circle(r = tangency_radius, $fn=4*teeth);
                for (i = [0 : teeth - 1]) {
                    angle=360/teeth*i;
                    next_angle=360/teeth*(i+1);
                    translate([cos(angle+tangent_circle_delta_theta)*tooth_center_r, sin(angle+tangent_circle_delta_theta)*tooth_center_r])
                        rotate([0,0, angle+tangent_circle_delta_theta])
                            circle(r = tooth_fillet_r, $fn=2*teeth);
                    translate([cos(next_angle-tangent_circle_delta_theta)*tooth_center_r, sin(next_angle-tangent_circle_delta_theta)*tooth_center_r])
                        rotate([0,0, next_angle-tangent_circle_delta_theta])
                            circle(r = tooth_fillet_r, $fn=2*teeth);
                    rotate([0,0, angle+tangent_circle_delta_theta]) wedge(tooth_center_r + tooth_fillet_r, wedge_angle, 4);
                }
            }
            teeth_holes();
            circle(r=inner_radius, $fn=30);
        }
    }



    if (height > 0) {
        translate([0,0, center ? -height/2 : 0]) {
            linear_extrude(height) {
                gt2pulley_2d();
            }

            /*
            color("red")
            linear_extrude(height*2/3)
            circle(r=pitch_r, $fn=60);

            color("yellow")
            linear_extrude(height/3)
            circle(r=belt_outer_r, $fn=60);
            */
        }
    } else {
        gt2pulley_2d();
    }
}

projection() {
gt2pulley(15, 2.5, 3);
translate([22,0,0]) gt2pulley(45, 2.5, 3);
translate([-25,0,0]) gt2pulley(60, 1.5, 3);
}


