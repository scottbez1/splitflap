

module flap_spool(flaps, flap_hole_radius, flap_gap, inner_radius, height) {
    inset = flap_hole_radius;
    separation = flap_gap;

    pitch_radius = flaps * (flap_hole_radius*2 + separation) / (2*PI);
    outer_radius = pitch_radius + flap_hole_radius + inset;


    module flap_spool_2d() {
        difference() {
            circle(r=outer_radius, $fn=60);
            for (i = [0 : flaps - 1]) {
                translate([cos(360/flaps*i)*pitch_radius, sin(360/flaps*i)*pitch_radius]) circle(r=flap_hole_radius, $fn=15);
            }
            circle(r=inner_radius, $fn=30);
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


flap_spool(40, 1.5, 5, 3, 3.2);
