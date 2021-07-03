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

// Almost nothing should be a global constant. Think hard before adding to this file.

// "Epsilon" - a small error tolerance value, used when designing 3d parts to avoid exact complanar faces that cause OpenSCAD rendering artifacts
// See https://3dprinting.stackexchange.com/a/9795
eps=0.01;
