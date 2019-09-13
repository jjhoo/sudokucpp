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

    class cellops {
    public:
        cellops(std::function<cells_t()> allop,
                std::function<cells_t(index_t)> boxop,
                std::function<cells_t(index_t)> colop,
                std::function<cells_t(index_t)> rowop) {
            get_all_op = allop;
            get_box_op = boxop;
            get_col_op = colop;
            get_row_op = rowop;
        }

        const cells_t get_all() const {
            return this->get_all_op();
        }

        const cells_t get_box(index_t i) const {
            // printf("%s, %d\n", __FILE__, __LINE__);
            return this->get_box_op(i);
        }

        const cells_t get_column(index_t i) const {
            // printf("%s, %d\n", __FILE__, __LINE__);
            return this->get_col_op(i);
        }

        const cells_t get_row(index_t i) const {
            // printf("%s, %d\n", __FILE__, __LINE__);
            return this->get_row_op(i);
        }

    protected:
        std::function<cells_t()> get_all_op;
        std::function<cells_t(index_t)> get_box_op;
        std::function<cells_t(index_t)> get_col_op;
        std::function<cells_t(index_t)> get_row_op;
    };

    typedef const cells_t (cellops::*CellUnitFun)(index_t i);

    struct EliminatorResult
    {
        cells_t solved;
        cells_t eliminated;
    };

    class Eliminator
    {
    public:
        virtual EliminatorResult eliminate(const cellops & solved,
                                           const cellops & candidates) = 0;
    };

    class SimpleSinglesEliminator : public Eliminator {
    public:
        virtual EliminatorResult eliminate(const cellops & solved,
                                           const cellops & candidates) {
            printf("%s, %d\n", __FILE__, __LINE__);

            std::map<position, cells_t> pos_cell_map;
            EliminatorResult result;

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

    class SinglesEliminator : public Eliminator {
    public:
        virtual EliminatorResult eliminate(const cellops & solved,
                                           const cellops & candidates) {
            printf("%s, %d\n", __FILE__, __LINE__);
            EliminatorResult result;

            auto ops = {
                &cellops::get_row,
                &cellops::get_column,
                &cellops::get_box
            };

            for (index_t i = 1; i <= SUDOKU_NUMBERS; i++) {
                for (auto op: ops) {
                    auto set = (candidates.*op)(i);
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
            std::vector<Eliminator *> eliminators = {
                new SimpleSinglesEliminator(),
                new SinglesEliminator(),
            };

            auto solvedops = cellops(
                [&]() {
                    return this->solved;
                },
                [&](index_t i) {
                    cells_t res;
                    std::copy_if(this->solved.cbegin(), this->solved.cend(), back_inserter(res),
                                 [i](const cell & c) {
                                     return c.pos.box == i;
                                 }
                        );
                    return res;
                },
                [&](index_t i) {
                    cells_t res;
                    std::copy_if(this->solved.cbegin(), this->solved.cend(), back_inserter(res),
                                 [i](const cell & c) {
                                     return c.pos.column == i;
                                 }
                        );
                    return res;
                },
                [&](index_t i) {
                    cells_t res;
                    std::copy_if(this->solved.cbegin(), this->solved.cend(), back_inserter(res),
                                 [i](const cell & c) {
                                     return c.pos.row == i;
                                 }
                        );
                    return res;
                }
                );

            auto candops = cellops(
                [&]() {
                    return this->candidates;
                },
                [&](index_t i) {
                    cells_t res;
                    std::copy_if(this->candidates.cbegin(),
                                 this->candidates.cend(),
                                 back_inserter(res),
                                 [i](const cell & c) {
                                     return c.pos.box == i;
                                 }
                        );
                    return res;
                },
                [&](index_t i) {
                    cells_t res;
                    std::copy_if(this->candidates.cbegin(),
                                 this->candidates.cend(),
                                 back_inserter(res),
                                 [i](const cell & c) {
                                     return c.pos.column == i;
                                 }
                        );
                    return res;
                },
                [&](index_t i) {
                    cells_t res;
                    std::copy_if(this->candidates.cbegin(),
                                 this->candidates.cend(),
                                 back_inserter(res),
                                 [i](const cell & c) {
                                     return c.pos.row == i;
                                 }
                        );
                    return res;
                }
                );

            bool progress = false;

            while (true) {
                printf("candidates left: %lu\n", candidates.size());

                progress = false;

                for (auto elim: eliminators) {
                    auto result = elim->eliminate(solvedops, candops);

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
            printf("%lu\n", cells.size());

            for (auto c1: cells) {
                auto idx = (c1.pos.row - 1) * SUDOKU_NUMBERS + (c1.pos.column - 1);
                printf("%d %d %d %d\n", idx, c1.pos.row, c1.pos.column, c1.value);

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
