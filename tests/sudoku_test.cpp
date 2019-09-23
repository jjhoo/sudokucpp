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

#include <iostream>
#include <gtest/gtest.h>

#include "sudokucpp/sudoku.h"

TEST(SudokuTest, SolvePuzzle1)
{
    auto puzzle = sudoku::Solver(
        "000040700500780020070002006810007900460000051009600078900800010080064009002050000");

    puzzle.pretty_print();

    auto cands = puzzle.get_candidates();
    std::cout << "candidates left: " <<  cands.size() << std::endl;
    EXPECT_EQ(cands.size(), 163) << "Expected 163 candidates left, got " << cands.size();

    puzzle.solve();
    cands = puzzle.get_candidates();
    EXPECT_EQ(cands.size(), 77) << "Expected 77 candidates left, got " << cands.size();
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
