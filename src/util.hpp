/*
Iskelt Corax - a chess engine
Copyright (C) 2026  Iskander Edward Boutel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <stdint.h>

#define setBit(v, i) ((v) |= 1ull << (i))
#define getBit(v, i) ((v >> (i)) & 1ull)
#define clrBit(v, i) ((v) &= ~(1ull << (i)))

#define getFileMask(f) (0x101010101010101ull << (f))

uint64_t random64 (void);
uint64_t sparseRandom64 (void);

uint64_t getTime (void);

#endif