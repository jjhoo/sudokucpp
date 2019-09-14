// -*- C++ -*-
// Copyright (c) 2019 Jani J. Hakala <jjhakala@gmail.com> Tampere, Finland
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

#include <cstdio>

#include "sudoku.h"

int main(void)
{
    auto puzzle = sudoku::Solver(
        "000040700500780020070002006810007900460000051009600078900800010080064009002050000");

    puzzle.pretty_print();

    auto cands = puzzle.get_candidates();
    printf("candidates left: %lu\n", cands.size());

    // for (auto c: cands) c.dump();

    {
        puzzle.solve();
        auto cands = puzzle.get_candidates();
        printf("candidates left: %lu\n", cands.size());
        puzzle.pretty_print();
    }

    return 0;
}
