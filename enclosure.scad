exploded=false;

// MDF, .125in nominal
// http://www.ponoko.com/make-and-sell/show-material/64-mdf-natural
thickness=3.2;

// M4 bolts
bolt_diameter=4;
bolt_length=20;
bolt_head_diameter=7;
bolt_head_length=4;
nut_width=7;
nut_length=3.2;
nut_inset=8;




height=80;
width=80;
length=80;

top_bottom_inset = thickness * 2;

part_separation=5;
eps=0.1;

// Centered in the x dimension
module captive_nut(thickness, bolt_diameter, bolt_length, nut_width, nut_length, nut_inset) {
    union() {
        translate([-bolt_diameter/2, 0, 0]) cube([bolt_diameter, bolt_length, thickness]);
        translate([-nut_width/2, nut_inset, 0]) cube([nut_width, nut_length, thickness]);
    }
}

module joint_tab_with_captive_nut(tab_width, thickness, bolt_diameter, bolt_length, nut_width, nut_length, nut_inset) {
    union() {
        translate([tab_width/2, 0, 0]) captive_nut(thickness, bolt_diameter, bolt_length, nut_width, nut_length, nut_inset);
        cube([tab_width, thickness, thickness]);
    }
}

front_corner_inset = 2*thickness;
front_width = width - 2*front_corner_inset;
front_tab_width = front_width/5;
side_length = length - 2*thickness;
side_tab_length = side_length/5;

module top() {

    module front_tab() {
        translate([0, -2*eps, -eps])
            joint_tab_with_captive_nut(front_tab_width, thickness+2*eps, bolt_diameter, bolt_length, nut_width, nut_length, nut_inset);
    }

    module side_tab() {
        translate([0, -2*eps, -eps])
            joint_tab_with_captive_nut(side_tab_length, thickness+2*eps, bolt_diameter, bolt_length, nut_width, nut_length, nut_inset);
    }

    module side_bolt_head_tab() {
        translate([0, -eps, -eps])
            union() {
                cube([side_tab_length, thickness+eps, thickness+2*eps]);
                translate([(side_tab_length - bolt_head_diameter)/2, 0, 0])
                    cube([bolt_head_diameter, bolt_head_length+eps, thickness+2*eps]);
            }
    }

    module front_corner_tab() {
        translate([-eps, -eps, -eps])
            cube([front_corner_inset+eps, thickness+eps, thickness+2*eps]);
    }

    difference() {
        cube([width, length, thickness]);

        // front captive nuts
        translate([front_corner_inset + front_tab_width, 0, 0]) front_tab();
        translate([front_corner_inset + 3*front_tab_width, 0, 0]) front_tab();

        // back captive nuts
        translate([front_corner_inset + front_tab_width, length, 0]) mirror([0,1,0]) front_tab();
        translate([front_corner_inset + 3*front_tab_width, length, 0]) mirror([0,1,0]) front_tab();

        // right side captive nut
        translate([width, thickness + side_tab_length, 0])
            rotate([0, 0, 90])
                side_tab();

        // right side tab + hole for opposite bolt head
        translate([width, thickness + 3*side_tab_length, 0])
            rotate([0, 0, 90])
                side_bolt_head_tab();

        // left side captive nut
        translate([0, thickness + 4*side_tab_length, 0])
            rotate([0, 0, -90])
                side_tab();

        // left side tab + hole for opposite bolt head
        translate([0, thickness + 2*side_tab_length, 0])
            rotate([0, 0, -90])
                side_bolt_head_tab();

        // corners
        front_corner_tab();
        translate([width, 0, 0]) mirror([1, 0, 0]) front_corner_tab();
        translate([width, length, 0]) mirror([1, 0, 0]) mirror([0, 1, 0]) front_corner_tab();
        translate([0, length, 0]) mirror([0, 1, 0]) front_corner_tab();
    }
}

module front() {
    module top_bottom_holes() {
        // Slots
        translate([front_corner_inset, 0, -eps]) cube([front_tab_width, thickness, thickness+2*eps]);
        translate([front_corner_inset + 2*front_tab_width, 0, -eps]) cube([front_tab_width, thickness, thickness+2*eps]);
        translate([front_corner_inset + 4*front_tab_width, 0, -eps]) cube([front_tab_width, thickness, thickness+2*eps]);

        // Bolt holes
        translate([front_corner_inset + 1.5*front_tab_width, thickness/2, -eps]) cylinder(h=thickness + 2*eps, r=bolt_diameter/2, $fn=60);
        translate([front_corner_inset + 3.5*front_tab_width, thickness/2, -eps]) cylinder(h=thickness + 2*eps, r=bolt_diameter/2, $fn=60);
    }
    color("red") {
        difference() {
            cube([width, height, thickness]);
            translate([0, top_bottom_inset, 0]) top_bottom_holes();
            translate([0, height-top_bottom_inset, 0]) mirror([0, 1, 0]) top_bottom_holes();
        }
    }
}

module bottom() {
    color("green")
    translate([0, length, 0]) mirror([0, 1, 0]) top();
}

module side() {
    module side_top_tab() {
        cube([top_bottom_inset + thickness + eps, side_tab_length, thickness]);
    }

    // For the bolt that holds the side and top together
    module side_top_bolt_hole() {
        translate([top_bottom_inset + thickness/2, side_tab_length/2, -eps])
            cylinder(h=thickness+2*eps, r=bolt_diameter/2, $fn=60);
    }

    // For the neighboring enclosure's top bolt head to fit into
    module side_top_bolt_head_hole() {
        translate([top_bottom_inset + thickness/2, side_tab_length/2, -eps])
            cylinder(h=thickness+2*eps, r=bolt_head_diameter/2, $fn=60);
    }

    color("pink")
    translate([0, thickness, 0])
        difference() {
            union() {
                translate([top_bottom_inset + thickness, 0, 0]) cube([height - 2*top_bottom_inset - 2*thickness, length - 2*thickness, thickness]);

                // Tabs to join to top
                translate([0, side_tab_length, 0]) side_top_tab();
                translate([0, 3*side_tab_length, 0]) side_top_tab();
                translate([height, side_tab_length, 0]) mirror([1, 0, 0]) side_top_tab();
                translate([height, 3*side_tab_length, 0]) mirror([1, 0, 0]) side_top_tab();
            }
            translate([0, side_tab_length, 0]) side_top_bolt_head_hole();
            translate([0, 3*side_tab_length, 0]) side_top_bolt_hole();
            translate([height, side_tab_length, 0]) mirror([1, 0, 0]) side_top_bolt_hole();
            translate([height, 3*side_tab_length, 0]) mirror([1, 0, 0]) side_top_bolt_head_hole();
        }
}

if (exploded) {
    top();
    translate([width + part_separation, 0, 0]) bottom();
    translate([0, length + part_separation, 0]) front();
    translate([width + part_separation, length + part_separation, 0]) front();
    translate([(width + part_separation)*2, 0, 0]) side();
} else {
    translate([0, 0, top_bottom_inset]) bottom();
    translate([0, 0, height - top_bottom_inset]) mirror([0,0,1]) top();
    translate([0, -eps, 0]) rotate([90, 0, 0]) translate([0, 0, -thickness]) front();
    translate([0, length+eps, 0]) mirror([0,1,0]) rotate([90, 0, 0]) translate([0, 0, -thickness]) front();
    translate([width + eps, 0, 0]) rotate([0, -90, 0]) side();
}
