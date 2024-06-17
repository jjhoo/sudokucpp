// -*- C++ -*-
// Copyright (c) 2019 Jani J. Hakala <jjhakala@gmail.com> Finland
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, version 3 of the
//  License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "config.h"
#include "sudoku.h"
#include "eliminators.h"
#include "combinations.h"
#include "permutations.h"

using namespace sudoku::eliminator;

Result
SimpleSingles::eliminate(
    const CellGetter & solved,
    const CellGetter & candidates) {
    ACE_TRACE(ACE_TEXT("SimpleSingles::eliminate"));

    std::map<Position, cells_t> pos_cell_map;
    Result result;

    for (auto c: candidates.get_all()) {
        pos_cell_map[c.pos].push_back(c);
    }

    for (auto pc: pos_cell_map) {
        if (pc.second.size() == 1) {
            result.solved.push_back(pc.second[0]);
        }
    }

    return result;
}

Result
Singles::eliminate(
    const CellGetter & solved,
    const CellGetter & candidates) {
    ACE_TRACE(ACE_TEXT("Singles::eliminate"));

    Result result;

    auto getters = {
        &CellGetter::get_row,
        &CellGetter::get_column,
        &CellGetter::get_box
    };

    for (index_t i = 1; i <= SUDOKU_NUMBERS; i++) {
        for (auto getter: getters) {
            auto set = (candidates.*getter)(i);
            std::map<index_t, cells_t> value_cell_map;

            for (auto c: set) {
                value_cell_map[c.value].push_back(c);
            }

            for (auto pc: value_cell_map) {
                if (pc.second.size() == 1) {
                    result.solved.push_back(pc.second[0]);
                }
            }
        }
    }

    // printf("solved\n");
    // for (auto c: result.solved) c.dump();

    return result;
}
