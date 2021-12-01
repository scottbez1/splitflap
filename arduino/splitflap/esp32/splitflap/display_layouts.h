/*
   Copyright 2021 Scott Bezek and the splitflap contributors

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
#pragma once

#include <stdint.h>

// Customize these settings and select a layout algorithm at the bottom if you have a different arrangement of modules:
#define DISPLAY_COLUMNS 6


// EXAMPLE LAYOUT ALGORITHMS:
static void getLayoutPositionSingleRowZigZag(const uint8_t module_index, uint8_t* out_row, uint8_t* out_col) {
    *out_row = module_index / DISPLAY_COLUMNS;

    // Each row alternates left-to-right, then right-to-left so data can be easily chained,
    // winding back and forth down the rows.
    *out_col = (*out_row % 2) ?
        (DISPLAY_COLUMNS - 1 - (module_index % DISPLAY_COLUMNS))
        : module_index % DISPLAY_COLUMNS;

}


/**
 * Each Driver connects to 3 columns across 2 rows, and then zig zags back on the next set of 2 rows
 *
 * (viewed from the module front side)
 *
 *  [1]  [3]  [5]      [7]  [9]  [11]
 *   [Driver #0]  --->  [Driver #1]   -->--v
 *  [0]  [2]  [4]      [6]  [8]  [10]      |
 *                                         |
 *                                         v
 *                                         |
 * [22]  [20]  [18]     [16]  [14]  [12]   |
 *   [Driver #3]    <--   [Driver #2]  <---<
 *  (upside down)        (upside down)
 * [23]  [21]  [19]     [17]  [15]  [13]
 *
 * Set flip_first_rows if the first row chain goes the other direction (driver 0 is on the right when
 * viewed from the front side)
 */
static void getLayoutPositionDualRowZigZag(const bool flip_first_rows, const uint8_t module_index, uint8_t* out_row, uint8_t* out_col) {
    uint8_t row_pair = module_index / 2 / DISPLAY_COLUMNS;
    bool upside_down_row_pair = (row_pair + flip_first_rows) % 2;

    uint8_t row_base = row_pair * 2;
    uint8_t row_offset = upside_down_row_pair ? (module_index % 2) : 1 - (module_index % 2);
    *out_row = row_base + row_offset;

    // Every set of 2 rows alternates left-to-right, then right-to-left so data can be easily chained,
    // winding back and forth down the groups of rows.
    *out_col = upside_down_row_pair ?
        (DISPLAY_COLUMNS - 1 - ((module_index / 2) % DISPLAY_COLUMNS))
        : ((module_index / 2) % DISPLAY_COLUMNS);
}



static void getLayoutPosition(const uint8_t module_index, uint8_t* out_row, uint8_t* out_col) {
    // Select a layout algorithm by uncommenting, or implement your own here:

    // getLayoutPositionDualRowZigZag(true, module_index, out_row, out_col);
    getLayoutPositionSingleRowZigZag(module_index, out_row, out_col);
}
