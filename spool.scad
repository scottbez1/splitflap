

module flap_spool(flaps, flap_hole_radius, flap_gap, inner_radius, height,
        spool_strut_tab_dimens, spool_strut_tab_outset) {
    inset = flap_hole_radius;
    separation = flap_gap;

    pitch_radius = flaps * (flap_hole_radius*2 + separation) / (2*PI);
    outer_radius = pitch_radius + flap_hole_radius + inset;


    // TODO: probably want to convert at least 2 of these to captive nut T joints
    module spool_strut_tab_hole() {
        translate([spool_strut_tab_dimens[0]/2, 0]) square(spool_strut_tab_dimens, true);
    }
    module spool_strut_tab_holes() {
        for (i=[0:3]) {
            angle = 90*i;
            translate([cos(angle)*spool_strut_tab_outset, sin(angle)*spool_strut_tab_outset])
                rotate([0,0,angle])
                    spool_strut_tab_hole();
        }
    }


    module flap_spool_2d() {
        difference() {
            circle(r=outer_radius, $fn=60);
            for (i = [0 : flaps - 1]) {
                translate([cos(360/flaps*i)*pitch_radius, sin(360/flaps*i)*pitch_radius]) circle(r=flap_hole_radius, $fn=15);
            }
            circle(r=inner_radius, $fn=30);
            spool_strut_tab_holes();
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


flap_spool(40, 1.5, 5, 3);
