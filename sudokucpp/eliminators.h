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
#ifndef ELIMINATORS_H
#define ELIMINATORS_H

#include "sudoku.h"

namespace sudoku {
    namespace eliminator {
        struct Result
        {
            cells_t solved;
            cells_t eliminated;
        };

        class Eliminator
        {
        public:
            virtual Result eliminate(const CellGetter & solved,
                                     const CellGetter & candidates) = 0;
        };

        class SimpleSingles : public Eliminator {
        public:
            virtual Result eliminate(const CellGetter & solved,
                                     const CellGetter & candidates);
        };

        class Singles : public Eliminator {
        public:
            virtual Result eliminate(const CellGetter & solved,
                                     const CellGetter & candidates);
        };
    }
}

#endif
