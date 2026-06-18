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

#include <random>
#include <chrono>

#include "util.hpp"

uint64_t random64 (void) {
    static auto rng = std::mt19937_64(std::random_device{}());

    return rng();
}

uint64_t sparseRandom64 (void) {
    return random64() & random64() & random64();
}

uint64_t getTime (void) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}