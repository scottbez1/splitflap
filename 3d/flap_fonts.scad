/*
   Copyright 2020-2021 Scott Bezek and the splitflap contributors

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

include<flap_dimensions.scad>;

use<fonts/roboto/RobotoCondensed-Regular.ttf>;
use<fonts/Epilogue/Epilogue-VariableFont_wght.ttf>;

// To try other experimental fonts, download fonts from Google Fonts to these locations and add `use` statements
// fonts/PoiretOne/PoiretOne-Regular.ttf
// fonts/Voltaire/Voltaire-Regular.otf
// fonts/PTSansNarrow/PTSansNarrow-Regular.ttf

// -----------------------
// Configurable parameters
// -----------------------

font_preset = "Epilogue";       // See available presets below
letter_gap_comp = true;         // Shifts letter positions to compensate for gap between flaps

// ---------------------------
// End configurable parameters
// ---------------------------



// Configure font presets below. Each font has the following settings:
//      'font'
//          Font name - see https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/Text#Using_Fonts_&_Styles
//      'height'
//          Font size (height) relative to flaps. i.e. a value of 1 sets the font size equal to the height of the top and bottom flaps.
//      'width'
//          Width scale factor. 1 = use default font width.
//      'offset_x'
//          Horizontal offset, in mm, of letters within flaps. A value of 0 uses default font centering.
//      'offset_y'
//          Vertical offset, in mm, of letters within flaps. A value of 0 uses default font centering.
//      'overrides'
//          Array of position/size overrides for specific letters. Each entry is a set of overrides for a single letter,
//          specified as an array with the following entries:
//              - Letter to override (e.g. "M"). Case must match for the override to apply.
//              - Additional X position offset, in mm (e.g. -5). Can be undef or 0 to omit.
//              - Additional Y position offset, in mm (e.g. 2.5). Can be undef or 0 to omit.
//              - Height override, as a value relative to flap height (e.g. 0.7). Replaces letter_height for this letter. Can be undef to omit.
//              - Width override, as a value relative to default font width (e.g. 0.7). Replaces letter_width for this letter. Can be undef to omit.
//              - Thickness offset override.
_font_settings = [
    "Roboto", [
        "font", "RobotoCondensed",
        "height", 0.6,
        "width", 1,
        "offset_x", -0.78,
        "offset_y", 0.7,
        "overrides", [
            ["@", 0, 1],
        ],
    ],

    // https://fonts.google.com/specimen/Bangers
    "Bangers", [
        "font", "Bangers",
        "height", 0.85,
        "width", 0.7,
        "offset_x", -5.5,
        "offset_y", -1,
        "overrides", [
            ["M", 2.5, 0],
            ["Q", 0, 1, 0.82],
            ["W", -2, 0],
            [",", 0, -4, 0.6, .8],
            ["'", 0, 6, 0.65, .8],
        ],
    ],

    "OCR-A", [
        "font", "OCRAStd",
        "height", 0.7,
        "width", 1,
        "offset_x", -0.78,
        "offset_y", 0,
        "overrides", [],
    ],

    "Epilogue", [
        "font", "Epilogue:style=Medium",
        "height",             0.7,
        "width",                1,
        "offset_x",         -0.65,
        "offset_y",          -0.8,
        "color_offset_y",     0.4,
        "thickness_offset",     0,
        "overrides", [
            //        x,     y, height, width, thickness
            ["@",   1.2,     0,    .65,   0.7,       0.4],
            ["&",   1.2,     0,  undef,   0.9,         0],
            ["C",     0,     0,  undef,  0.95,         0],
            ["G",   0.9,     0,  undef,  0.98,         0],
            ["W", -0.15, undef,  undef,  0.72,         1],
            ["M", -0.16, undef,  undef, 0.795,       0.6],
            ["O",     0,     0,  undef,  0.92,         0],
            ["Q",     0,     3,   0.62, undef,       0.4],
            [",",     0,  -1.6,    0.6, undef,         0],
        ],
    ],

    "Poiret", [
        "font", "PoiretOne",
        "height", 0.65,
        "width", 0.75,
        "offset_x", -0.6,
        "offset_y", 0.65,
        "overrides", [
            ["W", 0, 0, 0.65, 0.7]
        ],
        "thickness_offset", 2,
        "color_height", 0.455,
        "color_offset_y", 1.4,
    ],

    "Voltaire", [
        "font", "Voltaire:style=Regular",
        "height", 0.75,
        "width", 1,
        "offset_x", -0.6,
        "offset_y", -0.65,
        "overrides", [],
        // "color_height", 0.455,
        // "color_offset_y", 1.4,
    ],

    "PTSansNarrow", [
        "font", "PTSansNarrow",
        "height", 0.7,
        "width", 1,
        "offset_x", -0.6,
        "offset_y", 2,
        "overrides", [
            ["@", 0, 4, 0.65, 0.8],
            ["Q", 0, 4, 0.6],
        ],
        // "color_height", 0.455,
        // "color_offset_y", 1.4,
    ],

    "Righteous", [
        "font", "Righteous",
        "height", 0.6,
        "width", 1,
        "offset_x", 0,
        "offset_y", 0,
        "overrides", [
            // ["@", 0, 4, 0.65, 0.8],
            // ["Q", 0, 4, 0.6],
        ],
        // "color_height", 0.455,
        // "color_offset_y", 1.4,
    ],
];

// Private functions
function _get_entry_in_dict_array(arr, key) = search([key], arr) != [[]] ? arr[search([key], arr)[0] + 1] : undef;
function _get_font_settings() = _get_entry_in_dict_array(_font_settings, font_preset);

// Public functions
function use_letter_gap_compensation() = letter_gap_comp;
function get_font_setting(key) = _get_entry_in_dict_array(_get_font_settings(), key);
function get_letter_overrides(letter) =
    get_font_setting("overrides")[search([letter], get_font_setting("overrides"))[0]];
