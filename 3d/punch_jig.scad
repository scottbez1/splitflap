print_tolerance = 0.2;
jig_thickness = 2;

clamp_inset = 0.4;

punch_width = 26;
punch_lower_height = 7.7;
punch_gap_height = 2.5;
slot_inset_depth = 9.3;
slot_depth = 3;
slot_width = 15;

flap_width = 54;
flap_height = 43;
flap_pin_width = 1.4;

eps = 0.1;

jig_length = flap_width + slot_inset_depth;

union() {
    linear_extrude(height=punch_gap_height - print_tolerance*2) {
        difference() {
            square([jig_length, jig_thickness*2 + print_tolerance*2 + flap_height]);
            translate([slot_inset_depth - print_tolerance*2, jig_thickness + print_tolerance + (punch_width - slot_width)/2 - flap_pin_width - print_tolerance]) {
                square([flap_width + slot_depth + print_tolerance*2 + eps, jig_thickness + print_tolerance + flap_height - (punch_width - slot_width)/2 + flap_pin_width + print_tolerance + eps]);
            }
        }
    }

    translate([0, 0, punch_gap_height - print_tolerance*2]) {
        linear_extrude(height=punch_lower_height, scale=[1, (jig_thickness + clamp_inset)/jig_thickness]) {
            square([jig_length, jig_thickness]);
        }
    }

    translate([0, jig_thickness*2 + print_tolerance*2 + punch_width, punch_gap_height - print_tolerance*2]) {
        linear_extrude(height=punch_lower_height, scale=[1, (jig_thickness + clamp_inset)/jig_thickness]) {
            translate([0, -jig_thickness]) {
                square([slot_inset_depth - print_tolerance*2, jig_thickness]);
            }
        }
    }
}


%translate([slot_inset_depth, jig_thickness + print_tolerance + (punch_width - slot_width)/2, -8]) {
    cube([slot_depth, slot_width, 16]);
}
