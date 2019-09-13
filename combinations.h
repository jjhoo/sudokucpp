// -*- C++ -*-
// Copyright (c) 2019 Jani J. Hakala <jjhakala@gmail.com> Jyväskylä, Finland
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
#ifndef COMBINATIONS_H
#define COMBINATIONS_H

#include <cstdint>
#include <experimental/optional>
#include <vector>

namespace sudoku {
    namespace combination {
        struct Combination {
            std::vector<ssize_t> cjs;
            ssize_t length;
            ssize_t koo;
            ssize_t j;
            ssize_t k;

            bool visit_flag;

            // newCombination initialize a combination generator of length elements.
            Combination(ssize_t length, ssize_t koo) : length(length), koo(koo), j(koo), k(koo) {
                for (ssize_t i = 0; i <= koo; i++) {
                    this->cjs.push_back(i - 1);
                }

                this->cjs.push_back(length);
                this->cjs.push_back(0);

                for (auto n: this->cjs) {
                    printf("%ld\n", n);
                }
            }

            std::vector<ssize_t> visit() {
                auto n = this->k + 1;
                this->visit_flag = false;

                std::vector<ssize_t> res;

                std::copy(this->cjs.cbegin() + 1, this->cjs.cbegin() + n, back_inserter(res));

                return res;
            }

            std::experimental::optional<std::vector<ssize_t>> next() {
                if (this->visit_flag) {
                    return this->visit();
                }

                if (this->j > 0) {
                    //  T6
                    auto x = this->j;
                    this->cjs[this->j] = x;
                    this->j--;

                    this->visit_flag = true;
                    return this->visit();

                }

                // T3
                if ((this->cjs[1] + 1) < this->cjs[2]) {
                    this->cjs[1]++;

                    this->visit_flag = true;
                    return this->visit();
                }


                // T4
                this->j = 2;
                bool cont = true;
                auto x = -1;

                while (cont) {
                    this->cjs[this->j-1] = this->j - 2;
                    x = this->cjs[this->j] + 1;

                    if (x == this->cjs[this->j + 1]) {
                        this->j++;
                    } else {
                        cont = false;
                    }
                }

                // T5
                if (this->j > this->k) {
                    return std::experimental::nullopt;
                }

                // T6
                this->cjs[this->j] = x;
                this->j--;

                this->visit_flag = true;
                return this->visit();
            }
        };
    }
}

#endif
