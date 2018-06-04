/*
   Copyright 2015-2016 Scott Bezek and the splitflap contributors

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

use<roboto/Roboto-Bold.ttf>;

module roboto(text, size) {
    text(text=text, size=size, font="Roboto:style=Bold");
}
module text_label(lines) {
    text_height=2;
    module text_lines(lines, text_height, spacing=1.5) {
        for (i = [0 : len(lines)-1]) {
            translate([0, text_height * spacing * (len(lines)-1-i), 0]) {
                roboto(lines[i], text_height);
            }
        }
    }
    text_lines(lines, text_height);
}


