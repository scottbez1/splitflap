/*
   Copyright 2015-2020 Scott Bezek and the splitflap contributors

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

module legacyAssert(truth, msg="(no messsage provided)") {
    if (!truth) {
        // Intentionally don't close the <div> tag to make the entire console red
        echo(str(
            "<div style='background-color:#FF0000'>",
            "<br><br>",
            "####################",
            "<br><br>",
            "<span style='font-size: x-large'>ASSERTION ERROR!</span>",
            "<br><br>",
            msg,
            "<br><br>",
            "####################",
            "<br><br>"));

        color("red") {
            cube([1000, 1000, 1000], center=true);
        }
    }
}

required_widgets = 24;
widgets = 12 + 3;

legacyAssert(widgets >= required_widgets, str("Not enough widgets! Required ", required_widgets, " but found ", widgets));


