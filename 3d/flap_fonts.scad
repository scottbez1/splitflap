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

use<roboto/RobotoCondensed-Regular.ttf>;

// -----------------------
// Configurable parameters
// -----------------------

font_preset = "Roboto-Example";   // See available presets below
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
_font_settings = [
    "Roboto-Example", [
        "font", "RobotoCondensed",
        "height", 0.75,
        "width", 0.8,
        "offset_x", -0.78,
        "offset_y", 0.5,
        "overrides", [],
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
];

// Private functions
function _get_entry_in_dict_array(arr, key) = arr[search([key], arr)[0] + 1];
function _get_font_settings() = _get_entry_in_dict_array(_font_settings, font_preset);

// Public functions
function use_letter_gap_compensation() = letter_gap_comp;
function get_font_setting(key) = _get_entry_in_dict_array(_get_font_settings(), key);
function get_letter_overrides(letter) =
    get_font_setting("overrides")[search([letter], get_font_setting("overrides"))[0]];

