use<gt2pulley.scad>;
use<spool.scad>;

eps=.1;

thickness = 3.2;

teeth = 45;

rod_radius = 2.5;
assembly_inner_radius = 3.5;
bearing_outer_radius = 6;


assembly_color = [.76, .60, .42, 1];
bearing_color = [1,1,1,1];

joining_rod_radius = 1;

flap_width=54;


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
            flap_spool(40, 1.5, assembly_inner_radius,
                    height=0, 
                    spool_strut_tab_dimens=[thickness, 10],
                    spool_strut_tab_outset=12);
            joining_rod_holes();
        }
    }
}



module spool_with_pulleys_assembly() {
    layer_separation = thickness + 5;
    union() {
        color(assembly_color)
            flap_spool_complete();

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

        color(assembly_color)
            translate([0,0,layer_separation*3])
                add_joining_rods_and_extrude(thickness)
                    gt2pulley(teeth, assembly_inner_radius, 0);    
    }
}

translate([flap_width - thickness, 0, 0]) rotate([0, 90, 0]) spool_with_pulleys_assembly();
color(assembly_color)
    rotate([0, 90, 0]) flap_spool_complete();
