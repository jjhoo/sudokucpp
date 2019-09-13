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
#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdio>
#include <cstdint>

#include <ace/OS.h>
#include <ace/Trace.h>

namespace sudoku
{
    typedef uint8_t index_t;

    const index_t SUDOKU_BOXES = 3;
    const index_t SUDOKU_NUMBERS = 9;

    const index_t SUDOKU_GRID_LENGTH = SUDOKU_NUMBERS * SUDOKU_NUMBERS;
    const index_t SUDOKU_COLUMNS = SUDOKU_NUMBERS;
    const index_t SUDOKU_ROWS = SUDOKU_NUMBERS;

    struct position
    {
        position(index_t r, index_t c) {
            this->row = r;
            this->column = c;

            this->box = (((r - 1) / 3) * 3 + ((c - 1) / 3)) + 1;
        }

        bool operator<(const position & other) const {
            if (this->row == other.row) {
                return this->column < other.column;
            }

            return this->row < other.row;
        }

        bool operator==(const position & other) const {
            return this->row == other.row && this->column == other.column;
        }

        bool eq_row(const position & other) const {
            return this->row == other.row;
        }

        bool eq_column(const position & other) const {
            return this->column == other.column;
        }

        bool eq_box(const position & other) const {
            return this->box == other.box;
        }

        bool sees(const position & other) {
            return (eq_row(other) || eq_column(other) || eq_box(other));
        }

        index_t row;
        index_t column;
        index_t box;
    };

    struct cell
    {
        cell(sudoku::position p, index_t v) : pos(p.row, p.column)  {
            this->value = v;
        }

        void dump(void) const {
            printf("Cell %d %d %d\n", pos.row, pos.column, value);
        }

        position    pos;
        index_t     value;
    };

    typedef std::vector<cell> cells_t;

    class cellgetter {
    public:
        cellgetter(std::function<cells_t()> all,
                   std::function<bool(const cell &, index_t)> boxfilter,
                   std::function<bool(const cell &, index_t)> colfilter,
                   std::function<bool(const cell &, index_t)> rowfilter)
            : get_box_filter(boxfilter),
              get_col_filter(colfilter),
              get_row_filter(rowfilter) {
            get_all_fun = all;
        }

        const cells_t get_all() const {
            return this->get_all_fun();
        }

        const cells_t get_box(index_t i) const {
            ACE_TRACE(ACE_TEXT("cellgetter::get_box"));

            auto all = this->get_all();
            cells_t res;
            std::copy_if(all.cbegin(), all.cend(), back_inserter(res),
                         [this,i](const cell & c) {
                             return this->get_box_filter(c, i);
                         });

            return res;
        }

        const cells_t get_column(index_t i) const {
            ACE_TRACE(ACE_TEXT("cellgetter::get_column"));

            auto all = this->get_all();
            cells_t res;
            std::copy_if(all.cbegin(), all.cend(), back_inserter(res),
                         [this,i](const cell & c) {
                             return this->get_col_filter(c, i);
                         });
            return res;
        }

        const cells_t get_row(index_t i) const {
            ACE_TRACE(ACE_TEXT("cellgetter::get_row"));

            auto all = this->get_all();
            cells_t res;
            std::copy_if(all.cbegin(), all.cend(), back_inserter(res),
                         [this,i](const cell & c) {
                             return this->get_row_filter(c, i);
                         });

            return res;
        }

    protected:
        std::function<cells_t()> get_all_fun;
        std::function<bool(const cell &, index_t)> get_box_filter;
        std::function<bool(const cell &, index_t)> get_col_filter;
        std::function<bool(const cell &, index_t)> get_row_filter;
    };

    namespace eliminator {
        struct Result
        {
            cells_t solved;
            cells_t eliminated;
        };

        class Eliminator
        {
        public:
            virtual Result eliminate(const cellgetter & solved,
                                     const cellgetter & candidates) = 0;
        };

        class SimpleSingles : public Eliminator {
        public:
            virtual Result eliminate(const cellgetter & solved,
                                     const cellgetter & candidates) {
                ACE_TRACE(ACE_TEXT("SimpleSingles::eliminate"));

                std::map<position, cells_t> pos_cell_map;
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
        };

        class Singles : public Eliminator {
        public:
            virtual Result eliminate(const cellgetter & solved,
                                     const cellgetter & candidates) {
                ACE_TRACE(ACE_TEXT("Singles::eliminate"));

                Result result;

                auto getters = {
                    &cellgetter::get_row,
                    &cellgetter::get_column,
                    &cellgetter::get_box
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
        };
    };

    class solver
    {
    public:
        solver(const std::string & str) {
            if (str.size() != SUDOKU_GRID_LENGTH) {
                throw std::invalid_argument("Invalid sudoku size");
            }

            init_solved(str);
            init_candidates();
            remove_solved(solved);
        }

        virtual const cells_t & get_candidates() const {
            return this->candidates;
        }

        virtual void pretty_print() const {
            printf("+-------------------+\n");

            int i = 0;

            for (auto cell: this->solved) {
                if ((i + 1) % 9 == 1) {
                    printf("| ");
                }

                if (cell.value == 0) {
                    printf(".");
                } else {
                    printf("%d", cell.value);
                }

                if ((i + 1) % 9 == 0) {
                    printf(" |\n");
                } else {
                    printf(" ");
                }

                i++;
            }

            printf("+-------------------+\n");
        }

        virtual void solve() {
            std::vector<eliminator::Eliminator *> eliminators = {
                new eliminator::SimpleSingles(),
                new eliminator::Singles(),
            };

            auto solvedgetters = cellgetter(
                [&]() { return this->solved; },
                [&](const cell & c, index_t i) { return c.pos.box == i; },
                [&](const cell & c, index_t i) { return c.pos.column == i; },
                [&](const cell & c, index_t i) { return c.pos.row == i; });

            auto candgetters = cellgetter(
                [&]() { return this->candidates; },
                [&](const cell & c, index_t i) { return c.pos.box == i; },
                [&](const cell & c, index_t i) { return c.pos.column == i; },
                [&](const cell & c, index_t i) { return c.pos.row == i; });

            while (true) {
                printf("candidates left: %zu\n", candidates.size());

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

    protected:
        virtual void init_solved(const std::string & str) {
            int row = 1;
            int col = 1;

            std::transform(
                str.cbegin(),
                str.cend(),
                back_inserter(solved),
                [&row, &col](const char & c) {
                    index_t val = c - '0';
                    auto ncell = cell(position(row, col), val);

                    col++;
                    if (col > SUDOKU_NUMBERS) {
                        col = 1;
                        row++;
                    }
                    return ncell;
                });
        }

        virtual void init_candidates() {
            for (auto c: solved) {
                if (c.value == 0) {
                    for (index_t n = 1; n <= SUDOKU_NUMBERS; n++) {
                        candidates.push_back(cell(c.pos, n));
                    }
                }
            }
        }

        virtual void remove_solved(const cells_t & cells) {
            for (auto c1: cells) {
                if (c1.value != 0) {
                    candidates.erase(
                        std::remove_if(candidates.begin(), candidates.end(),
                                       [&c1](const cell & c2) {
                                           return c1.value == c2.value && c1.pos.sees(c2.pos);
                                       }),
                        candidates.end());
                }
            }
        }

        virtual void update_solved(const cells_t & cells) {
            for (auto c1: cells) {
                auto idx = (c1.pos.row - 1) * SUDOKU_NUMBERS + (c1.pos.column - 1);
                cell & c = this->solved[idx];
                // c.dump(); c1.dump();

                if (c.value == 0) {
                    c.value = c1.value;

                    candidates.erase(
                        std::remove_if(candidates.begin(), candidates.end(),
                                       [&c](const cell & c2) {
                                           return (c.value == c2.value && c.pos.sees(c2.pos))
                                               || (c.value != c2.value && c.pos == c2.pos);
                                       }),
                        candidates.end());
                }
            }
        }

        cells_t candidates;
        cells_t solved;
    };
}


int main(void)
{
    auto puzzle = sudoku::solver(
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
