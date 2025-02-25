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

flap_number = 0;
flap_gap = get_flap_gap();
flap_with_letters([0,0,0], [1,1,1], flap_index=flap_number, flap_gap=flap_gap, bleed=0, print_3d=true);