include<common.scad>;
include<parts/spool.scad>;


flap_spool_complete(motor_shaft=true, magnet_hole=true);

translate([0,0,spool_strut_length]) {
    rotate([0,90,0])
        spool_struts();
}


translate([60, 0, 0]) {    
    flap_spool_complete(captive_nut=true);

    translate([0,0, thickness]) {
        spool_retaining_wall(m4_bolt_hole=true);
    }
}