/*
   Copyright 2020 Scott Bezek and the splitflap contributors

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

use<roboto/RobotoCondensed-Regular.ttf>;
include<flap_dimensions.scad>;

/*
letter_font = "RobotoCondensed";
letter_height = 0.75;  // aspect, 0-1
letter_width  =  0.8;  // aspect, 0-1
letter_gap_comp = true;  // shifts letter positions to compensate for gap between flaps

letter_offset_x = -0.78;  // offset from center (mm)
letter_offset_y = 0.5;
*/

letter_font = "Bangers";
letter_height = 0.85;  // aspect, 0-1
letter_width  =  0.7;  // aspect, 0-1
letter_gap_comp = true;  // shifts letter positions to compensate for gap between flaps

letter_offset_x = -5.5;  // offset from center (mm)
letter_offset_y = -1;

// Per-letter position/size overrides. Each entry is a set of overrides for a single letter specified in
// an array with entries:
//
// - Letter to override (e.g. "M"). Case must match for the override to apply.
// - Additional X position offset, in mm (e.g. -5). Can be undef or 0 to omit.
// - Additional Y position offset, in mm (e.g. 2.5). Can be undef or 0 to omit.
// - Height override, as a value relative to flap height (e.g. 0.7). Replaces letter_height for this letter. Can be undef to omit.
// - Width override, as a value relative to default font width (e.g. 0.7). Replaces letter_width for this letter. Can be undef to omit.

_letter_overrides = [
    ["M", 2.5, 0],
    ["Q", 0, 1, 0.82],
    ["W", -2, 0],
    [",", 0, -4, 0.6, .8],
    ["'", 0, 6, 0.65, .8],
];

function get_letter_overrides(letter) =
    _letter_overrides[search([letter], _letter_overrides)[0]];
