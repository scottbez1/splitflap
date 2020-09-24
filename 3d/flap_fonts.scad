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

letter_font = "RobotoCondensed";
letter_height = 0.75;  // aspect, 0-1
letter_width  =  0.8;  // aspect, 0-1
letter_gap_comp = true;  // shifts letter positions to compensate for gap between flaps

letter_offset_x = -0.78;  // offset from center (mm)
letter_offset_y = 0.5;
