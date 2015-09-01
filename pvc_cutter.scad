

eps = 0.001;

module alum_extrusion(height, width, wall_thickness, length) {
    color([0.8, 0.8, 0.8, 0.5])
    linear_extrude(length, center=true) {
        difference() {
            square([width, height], center=true);
            square([width - wall_thickness, height - wall_thickness], center=true);
        }
    }
}

module alum_half_by_half(height) {
    alum_extrusion(0.5*25.4, 0.5*25.4, 1/16*25.4, height);
}
module alum_one_by_one_and_half(height) {
    alum_extrusion(1*25.4, 1.5*25.4, 1/16*25.4, height);
}


translate([0, 0, 3*25.4])
    alum_half_by_half(4*25.4);

translate([0.5*25.4 + eps, 0, 2*25.4])
    alum_half_by_half(4*25.4);
translate([-0.5*25.4 - eps, 0, 2*25.4])
    alum_half_by_half(4*25.4);

translate([0,-3*25.4 -0.75*25.4 - eps, 3*25.4])
    alum_one_by_one_and_half(6*25.4);
translate([0, 0.75*25.4 + eps, 3*25.4])
    alum_one_by_one_and_half(6*25.4);


translate([-.5*25.4, 0, 5*25.4])
rotate([0, 45, 0])
translate([0, 0, 1.75*25.4])
alum_half_by_half(4*25.4);


