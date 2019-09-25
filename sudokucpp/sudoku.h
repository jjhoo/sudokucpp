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
#ifndef SUDOKU_H
#define SUDOKU_H

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdint>

#if HAVE_LIBACE
#include <ace/OS.h>
#include <ace/Trace.h>
#else
#define ACE_TRACE(x)
#define ACE_TEXT(x)
#endif

namespace sudoku
{
    typedef uint8_t index_t;

    const index_t SUDOKU_BOXES = 3;
    const index_t SUDOKU_NUMBERS = 9;

    const index_t SUDOKU_GRID_LENGTH = SUDOKU_NUMBERS * SUDOKU_NUMBERS;
    const index_t SUDOKU_COLUMNS = SUDOKU_NUMBERS;
    const index_t SUDOKU_ROWS = SUDOKU_NUMBERS;

    struct Position
    {
        Position(index_t r, index_t c) {
            this->row = r;
            this->column = c;

            this->box = (((r - 1) / 3) * 3 + ((c - 1) / 3)) + 1;
        }

        bool operator<(const Position & other) const {
            if (this->row == other.row) {
                return this->column < other.column;
            }

            return this->row < other.row;
        }

        bool operator==(const Position & other) const {
            return this->row == other.row && this->column == other.column;
        }

        bool eq_row(const Position & other) const {
            return this->row == other.row;
        }

        bool eq_column(const Position & other) const {
            return this->column == other.column;
        }

        bool eq_box(const Position & other) const {
            return this->box == other.box;
        }

        bool sees(const Position & other) {
            return (eq_row(other) || eq_column(other) || eq_box(other));
        }

        index_t row;
        index_t column;
        index_t box;
    };

    struct Cell
    {
        Cell(sudoku::Position p, index_t v) : pos(p.row, p.column)  {
            this->value = v;
        }

        void dump(void) const;

        Position    pos;
        index_t     value;
    };

    typedef std::vector<Cell> cells_t;

    class CellGetter {
    public:
        CellGetter(std::function<cells_t()> all,
                   std::function<bool(const Cell &, index_t)> boxfilter,
                   std::function<bool(const Cell &, index_t)> colfilter,
                   std::function<bool(const Cell &, index_t)> rowfilter)
            : get_box_filter(boxfilter),
              get_col_filter(colfilter),
              get_row_filter(rowfilter) {
            get_all_fun = all;
        }

        const cells_t get_all() const {
            return this->get_all_fun();
        }

        const cells_t get_box(index_t i) const {
            ACE_TRACE(ACE_TEXT("CellGetter::get_box"));

            auto all = this->get_all();
            cells_t res;
            std::copy_if(all.cbegin(), all.cend(), back_inserter(res),
                         [this,i](const Cell & c) {
                             return this->get_box_filter(c, i);
                         });

            return res;
        }

        const cells_t get_column(index_t i) const {
            ACE_TRACE(ACE_TEXT("CellGetter::get_column"));

            auto all = this->get_all();
            cells_t res;
            std::copy_if(all.cbegin(), all.cend(), back_inserter(res),
                         [this,i](const Cell & c) {
                             return this->get_col_filter(c, i);
                         });
            return res;
        }

        const cells_t get_row(index_t i) const {
            ACE_TRACE(ACE_TEXT("CellGetter::get_row"));

            auto all = this->get_all();
            cells_t res;
            std::copy_if(all.cbegin(), all.cend(), back_inserter(res),
                         [this,i](const Cell & c) {
                             return this->get_row_filter(c, i);
                         });

            return res;
        }

    protected:
        std::function<cells_t()> get_all_fun;
        std::function<bool(const Cell &, index_t)> get_box_filter;
        std::function<bool(const Cell &, index_t)> get_col_filter;
        std::function<bool(const Cell &, index_t)> get_row_filter;
    };

    namespace eliminator {
        class Eliminator;
    }

    class Solver
    {
    public:
        Solver(const std::string & str);

        virtual const cells_t & get_candidates() const {
            return this->candidates;
        }

        virtual void pretty_print() const;
        virtual void solve();
    protected:
        virtual void init_solved(const std::string & str);
        virtual void init_candidates();
        virtual void remove_solved(const cells_t & cells);
        virtual void update_solved(const cells_t & cells);

        virtual void add_eliminator(std::shared_ptr<eliminator::Eliminator>);
        virtual void add_eliminator(eliminator::Eliminator *);

        cells_t candidates;
        cells_t solved;
        std::vector<std::shared_ptr<eliminator::Eliminator>> eliminators;
    };
}

#endif
