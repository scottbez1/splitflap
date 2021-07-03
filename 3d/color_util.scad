/*
   Copyright 2015-2021 Scott Bezek and the splitflap contributors

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

// multiply two equal matricies by each element, limiting to a max of 1.0
function color_multiply(x, y) =
    [ for(j=[0:len(x) - 1]) min(x[j] * y[j], 1.0) ];

// inverts a color matrix by subtracting the input channel values from 1.0
function color_invert(x) =
    [ for(j=[0:len(x) - 1]) (1.0 - x[j]) ];
