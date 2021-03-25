// MDF, .120in nominal
// https://www.ponoko.com/materials/mdf-fiberboard
thickness = 3.0;

etch_depth = 0.1;  // for render
eps=.01;
num_flaps = 40;

// Rendering Colors
assembly_color = [0.76, 0.60, 0.42];  // MDF, "c1996b"
etch_color = [0, 0, 0];  // black, "000000"

hardware_color = [0.75, 0.75, 0.8];  // steel, "bfbfcc"

assembly_color1 = color_multiply(assembly_color, [1.161, 1.157, 1.157, 1.0]);  // "e1b17c" with MDF
assembly_color2 = color_multiply(assembly_color, [0.897, 0.895, 0.895, 1.0]);  // "ae8960" with MDF
assembly_color3 = color_multiply(assembly_color, [0.547, 0.542, 0.540, 1.0]);  // "6a533a" with MDF
assembly_color4 = color_multiply(assembly_color, [0.268, 0.268, 0.271, 1.0]);  // "34291d" with MDF

// multiply two equal matricies by each element, limiting to a max of 1.0
function color_multiply(x, y) =
    [ for(j=[0:len(x) - 1]) min(x[j] * y[j], 1.0) ];

// inverts a color matrix by subtracting the input channel values from 1.0
function color_invert(x) =
    [ for(j=[0:len(x) - 1]) (1.0 - x[j]) ];

module enclosure_etch_style() {
    color(etch_color)
        translate([0, 0, thickness])
            linear_extrude(height=etch_depth)
                children();
}
