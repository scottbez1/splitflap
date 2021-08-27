/*
   Copyright 2021 Scott Bezek and the splitflap contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

use <../shapes.scad>

// Customization Options
wall_thickness = 4;            // thickness of the perimeter wall, along XY
bottom_thickness = 2.5;        // thickness of the bottom face, along Z
pcb_xy_clearance = 0.5;        // gap between PCB edges and the perimeter wall, XY
pcb_z_clearance = 0.4;         // gap between the top of the PCB PCB and the top of the enclosure
solder_point_clearance = 4.5;  // distance between the bottom of the PCB and the floor of the enclosure, Z
wall_corner_radius = 5;        // radius of the perimeter wall corners
wall_top_radius = 1.5;         // radius for the 3D fillet on the top edge of mount

mounting_hole_diameter = 6;       // diameter of the holes in the mount, large enough for heat-set inserts
mounting_hole_clearance = 5;      // body clearance around the holes in the mount, XY
mounting_hole_boss_radius = 5;    // radius for the 2D inside corners of the mounting hole bosses
mounting_boss_top_fillet = 2;     // radius of the 3D fillet on top of the boss
mounting_boss_bottom_fillet = 2;  // radius of the 3D fillet on the bottom of the boss (additive)

support_boss_width = 15;           // total width of the support boss, along X
support_boss_length = 6;           // total length of the support boss, along Y
support_boss_y_offset = 10;        // offset of the boss from the PCB edge, along Y
support_boss_fillet_radius = 2;    // radius of the support boss corners
support_boss_top_fillet = 1;       // radius of the 3D fillet on top of the boss
support_boss_bottom_fillet = 1.5;  // radius of the 3D fillet on the bottom of the boss (additive)

screw_hole_diameter = 4;         // diameter of the holes on either side for screwing into a surface
screw_hole_head_diameter = 8.5;  // diameter of the bolt head used in the screw hole (for counter-bore)
screw_hole_clearance = 4;        // body clearance around the screw holes, XY
screw_hole_body_thickness = 4;   // thickness of the area under the screw hole
screw_hole_body_fillet = 100;    // radius for the body fillet on the screw hole body (large = max possible)

// PCB dimensions taken from KiCAD ("Classic" Driver)
pcb_width = 96;
pcb_length = 48;
pcb_thickness = 1.6;

pcb_hole_left_offset = 8.128;  // offset from left wall to left holes center
pcb_hole_distance_x = 83.312;  // hole distances center-to-center
pcb_hole_distance_y = 39.878;

pcb_hole_coordinates = [
    [pcb_hole_left_offset, pcb_length/2 - pcb_hole_distance_y/2],  // bottom left
    [pcb_hole_left_offset, pcb_length/2 + pcb_hole_distance_y/2],  // top left
    [pcb_hole_left_offset + pcb_hole_distance_x, pcb_length/2 - pcb_hole_distance_y/2],  // bottom right
];

// Pre-Calculated Values
wall_xy_offset = pcb_xy_clearance + wall_thickness;  // offset from outside wall to PCB edge (XY)
height_to_pcb = bottom_thickness + solder_point_clearance;  // distance from Z origin to the bottom face of the PCB

total_width = pcb_width + 2*wall_xy_offset;
total_length = pcb_length + 2*wall_xy_offset;
total_height = height_to_pcb + pcb_thickness + pcb_z_clearance;

mounting_hole_boss_offset = mounting_hole_diameter/2 + mounting_hole_clearance;  // distance from hole center to boss edge

screw_hole_outside_radius = screw_hole_diameter/2 + screw_hole_clearance;  // distance from hole center to outside edge
screw_hole_3D_fillet = total_height - screw_hole_body_thickness - wall_top_radius;  // radius for the 3D fillet on the screw hole body

eps = 0.01;

module dummy_pcb() {
    color([0, 0.5, 0])
    translate([wall_xy_offset, wall_xy_offset, bottom_thickness + solder_point_clearance]) {
        linear_extrude(height=pcb_thickness)
            difference() {
                square([pcb_width, pcb_length]);
                mounting_holes();
            }
    }
}

module pcb_mount_base_2d() {
    rounded_square([total_width, total_length], r=wall_corner_radius, $fn=60);
}

module pcb_mount_cutout() {
    translate([0, 0, bottom_thickness])
        linear_extrude(height=total_height + eps, convexity=10)
            translate([wall_thickness, wall_thickness])
                square([pcb_width + 2*pcb_xy_clearance, pcb_length + 2*pcb_xy_clearance]);
}

module mounting_holes() {
    for(i = [0 : 2]) {
        translate(pcb_hole_coordinates[i])
            circle(r=mounting_hole_diameter/2, $fn=30);
    }
}

module pcb_support_boss() {
    translate([total_width - wall_thickness - support_boss_width, total_length - wall_xy_offset - support_boss_length - support_boss_y_offset]) {
        difference() {
            linear_extrude(height=height_to_pcb)
                rounded_square([support_boss_width + eps, support_boss_length], r=support_boss_fillet_radius, corners=[0, 1], $fn=60);
            translate([0, 0, height_to_pcb])
                square_fillet_3d([support_boss_width, support_boss_length], r=support_boss_top_fillet, r_corner=support_boss_fillet_radius, overlap=0.1, corners=[0,1], edges=[0,1,3], $fn=30);
        }
        translate([0, 0, bottom_thickness])
            square_fillet_3d([support_boss_width, support_boss_length], r=support_boss_bottom_fillet, r_corner=support_boss_fillet_radius, overlap=0.1, additive=true, corners=[0,1], edges=[0,1,3], $fn=60);
    }
}

module screw_mount_2d(fillet_radius) {
    translate([-screw_hole_outside_radius, 0]) {
        difference() {
            // Main Body
            union() {
                circle(r=screw_hole_outside_radius, $fn=60);
                translate([0, -screw_hole_outside_radius])
                    square([screw_hole_outside_radius, 2*screw_hole_outside_radius]);
            }
            // Through-Hole
            circle(r=screw_hole_diameter/2, $fn=60);
        }

        if(fillet_radius > 0) {
            // X/Y Fillet, Top
            translate([screw_hole_outside_radius, screw_hole_outside_radius])
                mirror([1, 0, 0])
                    fillet_tool(r=fillet_radius, $fn=60);

            // X/Y Fillet, Bottom
            translate([screw_hole_outside_radius, -screw_hole_outside_radius])
                mirror([1, 1, 0])
                    fillet_tool(r=fillet_radius, $fn=60);
        }
    }
}

module screw_mount() {
    fillet_radius = min(screw_hole_body_fillet, screw_hole_outside_radius);  // fillet can't pass 90°
    thickness = min(screw_hole_body_thickness, total_height);  // don't make extension taller than main body

    render(convexity=10)  // fixes render issues in preview mode (due to number of 3D intersections)
    translate([0, total_length/2])
        difference() {
            union() {
                // Main Body
                linear_extrude(height=thickness)
                    screw_mount_2d(fillet_radius);


                // 3D Fillet
                translate([0, 0, thickness]) {
                    length = 2*(screw_hole_outside_radius + fillet_radius + eps);
                    intersection() {
                        translate([0, length/2])
                            rotate([90, -90, 0])
                                linear_extrude(height=length)
                                    fillet_tool(r=screw_hole_3D_fillet, $fn=120);
                        
                        linear_extrude(height=total_height - thickness)
                            screw_mount_2d(fillet_radius);
                    }
                }
            }
            
            // Bolt Counter-Bore
            translate([-screw_hole_outside_radius, 0, thickness - eps])
                linear_extrude(height=total_height - thickness + 2*eps)
                    circle(r=screw_hole_head_diameter/2, $fn=30);
        }
}

module pcb_mount() {
    difference() {
        union() {
            // Base "Bathtub"
            difference() {
                linear_extrude(height=total_height, convexity=10)
                    pcb_mount_base_2d();
                pcb_mount_cutout();
            }

            // Mounting Holes Bosses
            translate([wall_thickness, wall_thickness]) {
                // Bottom Left
                translate([-eps, -eps]) {
                    x = pcb_hole_coordinates[0][0] + pcb_xy_clearance + mounting_hole_boss_offset + eps;
                    y = pcb_hole_coordinates[0][1] + pcb_xy_clearance + mounting_hole_boss_offset + eps;

                    difference() {
                        linear_extrude(height=height_to_pcb)
                            rounded_square([x, y], r=mounting_hole_boss_radius, corners=[2], $fn=60);
                        translate([0, 0, height_to_pcb])
                            square_fillet_3d([x, y], r=mounting_boss_top_fillet, r_corner=mounting_hole_boss_radius, overlap=0.1, corners=[2], edges=[1,2], $fn=60);
                    }

                    translate([0, 0, bottom_thickness])
                        square_fillet_3d([x, y], r=mounting_boss_bottom_fillet, r_corner=mounting_hole_boss_radius, overlap=0.1, additive=true, corners=[2], edges=[1,2], $fn=60);
                }

                // Top Left
                translate([-eps, pcb_hole_coordinates[1][1] + pcb_xy_clearance - mounting_hole_boss_offset]) {
                    x = pcb_hole_coordinates[1][0] + pcb_xy_clearance + mounting_hole_boss_offset + eps;
                    y = 2*pcb_xy_clearance + pcb_length - pcb_hole_coordinates[1][1] + mounting_hole_boss_offset + eps;

                    difference() {
                        linear_extrude(height=height_to_pcb)
                            rounded_square([x, y], r=mounting_hole_boss_radius, corners=[3], $fn=60);
                        translate([0, 0, height_to_pcb])
                            square_fillet_3d([x, y], r=mounting_boss_top_fillet, r_corner=mounting_hole_boss_radius, overlap=0.1, corners=[3], edges=[2,3], $fn=60);
                    }

                    translate([0, 0, bottom_thickness])
                        square_fillet_3d([x, y], r=mounting_boss_bottom_fillet, r_corner=mounting_hole_boss_radius, overlap=0.1, additive=true, corners=[3], edges=[2,3], $fn=60);
                }

                // Bottom Right
                translate([pcb_hole_coordinates[2][0] + pcb_xy_clearance - mounting_hole_boss_offset, -eps]) {
                    x = pcb_width - pcb_hole_coordinates[2][0] + pcb_xy_clearance + mounting_hole_boss_offset + eps;
                    y = pcb_hole_coordinates[2][1] + pcb_xy_clearance + mounting_hole_boss_offset + eps;
                    
                    difference() {
                        linear_extrude(height=height_to_pcb)
                            rounded_square([x, y], r=mounting_hole_boss_radius, corners=[1], $fn=60);
                        translate([0, 0, height_to_pcb])
                            square_fillet_3d([x, y], r=mounting_boss_top_fillet, r_corner=mounting_hole_boss_radius, overlap=0.1, corners=[1], edges=[0,1], $fn=60);
                    }

                    translate([0, 0, bottom_thickness])
                        square_fillet_3d([x, y], r=mounting_boss_bottom_fillet, r_corner=mounting_hole_boss_radius, overlap=0.1, additive=true, corners=[1], edges=[0,1], $fn=60);
                }
            }

            // Board Support Boss
            pcb_support_boss();

            // Left screw mount
            screw_mount();

            // Right screw mount
            translate([total_width, 0])
                mirror([1, 0, 0])
                    screw_mount();
        }

        // Mounting Hole... Holes
        translate([wall_xy_offset, wall_xy_offset, -eps])
            linear_extrude(height=total_height + 2*eps)
                mounting_holes();

        // Top Edge 3D Fillet
        translate([0, 0, total_height])
            square_fillet_3d(size=[total_width, total_length], r=wall_top_radius, r_corner=wall_corner_radius, $fn=60);
    }
}

pcb_mount();
