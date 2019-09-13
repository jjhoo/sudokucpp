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
#ifndef PERMUTATIONS_H
#define PERMUTATIONS_H

#include <cstdint>
#include <experimental/optional>
#include <vector>

namespace sudoku {
    namespace permutation {
        struct Permutation {
            std::vector<size_t> ajs;
            size_t length;
            bool visit_flag;

            Permutation(size_t length) : length(length), visit_flag(true) {
                ajs.push_back(0);

                for (size_t i = 1; i <= length + 1; i++) {
                    ajs.push_back(i);
                }
            }

            std::vector<size_t> visit() {
                auto n = this->length + 1;

                std::vector<size_t> res;
                std::copy(this->ajs.cbegin() + 1, this->ajs.cbegin() + n, back_inserter(res));

                this->visit_flag = false;

                return res;
            }

            std::experimental::optional<std::vector<size_t>> next() {
                if (this->visit_flag) {
                    return this->visit();
                }

                // L2
                auto j = this->length - 1;
                bool cont = true;

                while (cont) {
                    if (this->ajs[j] >= this->ajs[j + 1]) {
                        j--;
                    } else if (this->ajs[j] < this->ajs[j + 1]) {
                        cont = false;
                    } else if (j == 0) {
                        cont = false;
                    }
                }

                if (j == 0) {
                    this->visit_flag = false;
                    return std::experimental::nullopt;
                }

                // L3
                auto l = this->length;
                if (this->ajs[j] >= this->ajs[l]) {
                    cont = true;

                    while (cont) {
                        l--;
                        if (this->ajs[j] < this->ajs[l]) {
                            cont = false;
                        }
                    }

                }

                std::swap(this->ajs[j], this->ajs[l]);

                // L4
                auto k = j + 1;
                l = this->length;

                while (k < l) {
                    std::swap(this->ajs[k], this->ajs[l]);
                    k++;
                    l--;
                }

                this->visit_flag = true;
                return this->visit();
            }
        };
    }
}


#endif
