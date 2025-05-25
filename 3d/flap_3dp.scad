/*
   Copyright 2025 Scott Bezek and the splitflap contributors

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
use<flap.scad>;

// TODO: extract core flap spool dimensions used for flap gap instead of using the full splitflap file
use<splitflap.scad>;

generateFullFlap = false; // If true, generates the full flap STL
generateText = false; // If generateFullFlap is false, this controls what part of the STL to generate
generateFrontText = true; // If generateText is true, this controls generation of the front text
generateBackText = true; // If generateText is true, this controls generation of the back text

flap_number = 0;

flap_gap = get_flap_gap();
if (generateFullFlap) {
	// Generate the full flap using the standard function
    flap_with_letters([0,0,0], [1,1,1], flap_index=flap_number, flap_gap=flap_gap, bleed=0, print_3d=true);
} else {
    if (generateText == false) {
		// Generate just the flap STL with the letters removed
        difference() {
            flap_with_letters([0,0,0], [1,1,1], front_letter=false, back_letter=false, flap_index=flap_number, flap_gap=flap_gap, bleed=0, print_3d=true);
            flap_with_letters([0,0,0], [1,1,1], flap=false, flap_index=flap_number, flap_gap=flap_gap, bleed=0, print_3d=true);
        }
    } else {
		// Generate just the letters STL
        flap_with_letters([0,0,0], [1,1,1], front_letter=generateFrontText, back_letter=generateBackText, flap=false, flap_index=flap_number, flap_gap=flap_gap, bleed=0, print_3d=true);
    }
}