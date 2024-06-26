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
#include <iostream>
#include <memory>

#include "sudoku.h"
#include "eliminators.h"

using namespace sudoku;

Solver::Solver(
    const std::string & str) {
    if (str.size() != SUDOKU_GRID_LENGTH) {
        throw std::invalid_argument("Invalid sudoku size");
    }

    init_solved(str);
    init_candidates();
    remove_solved(solved);
}

void
Solver::pretty_print() const {
    std::cout << "+-------------------+" << std::endl;

    int i = 0;

    for (auto cell: this->solved) {
        if ((i + 1) % 9 == 1) {
            std::cout << "| ";
        }

        if (cell.value == 0) {
            std::cout << ".";
        } else {
            std::cout << static_cast<int>(cell.value);
        }

        if ((i + 1) % 9 == 0) {
            std::cout << " |" << std::endl;
        } else {
            std::cout << " ";
        }

        i++;
    }

    std::cout << "+-------------------+" << std::endl;
}

void
Solver::solve()
{
    add_eliminator(new eliminator::SimpleSingles());
    add_eliminator(new eliminator::Singles());

    auto solvedgetters = CellGetter(
        [&]() { return this->solved; },
        [&](const Cell & c, index_t i) { return c.pos.box == i; },
        [&](const Cell & c, index_t i) { return c.pos.column == i; },
        [&](const Cell & c, index_t i) { return c.pos.row == i; });

    auto candgetters = CellGetter(
        [&]() { return this->candidates; },
        [&](const Cell & c, index_t i) { return c.pos.box == i; },
        [&](const Cell & c, index_t i) { return c.pos.column == i; },
        [&](const Cell & c, index_t i) { return c.pos.row == i; });

    while (true) {
        std::cout << "candidates left: " << candidates.size() << std::endl;

        bool progress = false;

        for (auto elim: eliminators) {
            auto result = elim->eliminate(solvedgetters, candgetters);

            if (result.solved.size() > 0) {
                update_solved(result.solved);
                progress = true;
                break;
            }

            if (result.eliminated.size() > 0) {
                // update_candidates(result.eliminated);
                progress = true;
                break;
            }
        }

        if (!progress) {
            break;
        }
    }

    // elim.eliminate(this->solved, this->candidates);
}

void
Solver::init_solved(
    const std::string & str)
{
    int row = 1;
    int col = 1;

    std::transform(
        str.cbegin(),
        str.cend(),
        back_inserter(solved),
        [&row, &col](const char & c) {
            index_t val = c - '0';
            auto ncell = Cell(Position(row, col), val);

            col++;
            if (col > SUDOKU_NUMBERS) {
                col = 1;
                row++;
            }
            return ncell;
        });
}

void
Solver::init_candidates()
{
    for (auto c: solved) {
        if (c.value == 0) {
            for (index_t n = 1; n <= SUDOKU_NUMBERS; n++) {
                candidates.push_back(Cell(c.pos, n));
            }
        }
    }
}

void
Solver::remove_solved(
    const cells_t & cells)
{
    auto nend = candidates.end();

    for (auto c1: cells) {
        if (c1.value != 0) {
            nend = std::remove_if(candidates.begin(), nend,
                                  [&c1](const Cell & c2) {
                                      return c1.value == c2.value && c1.pos.sees(c2.pos);
                                  });
        }
    }
    candidates.erase(nend, candidates.end());
}

void
Solver::update_solved(
    const cells_t & cells)
{
    auto nend = candidates.end();

    for (auto c1: cells) {
        auto idx = (c1.pos.row - 1) * SUDOKU_NUMBERS + (c1.pos.column - 1);
        Cell & c = this->solved[idx];
        // c.dump(); c1.dump();

        if (c.value == 0) {
            c.value = c1.value;

            nend = std::remove_if(candidates.begin(), nend,
                                 [&c](const Cell & c2) {
                                     return (c.value == c2.value && c.pos.sees(c2.pos))
                                     || (c.value != c2.value && c.pos == c2.pos);
                                 });
        }
    }
    candidates.erase(nend, candidates.end());
}

void
Solver::add_eliminator(
    std::shared_ptr<eliminator::Eliminator> e)
{
    this->eliminators.push_back(e);
}

void
Solver::add_eliminator(
    eliminator::Eliminator * e)
{
    this->add_eliminator(std::shared_ptr<eliminator::Eliminator>(e));
}
