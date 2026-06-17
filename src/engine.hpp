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

#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include "board.hpp"

#define TIME_BREAK -INT32_MAX

#define CHECKMATE 100000
#define STALEMATE 50000

#define MAX_PHASE 24

#define LOWER 0
#define EXACT 1
#define UPPER 2

typedef struct {
    uint64_t hash;
    Move move;
    uint8_t depth;
    int score;
    uint8_t type;
} TTEntry;

class Engine {
    public:
        Board board;

        uint64_t ts;
        uint64_t tl;
        uint8_t md;
        uint64_t n;

        Move killers[256][2] = {0};
        int history[2][64][64] = {0};   // (turn, to, from)
        TTEntry *tt;
        int tt_max;

        uint64_t pp_mask[2][64] = {0};
        uint64_t ip_mask[64] = {0};
        uint64_t dp_mask[64] = {0};
        uint64_t kp_mask[64] = {0};

        Engine (void);
        ~Engine (void);

        int evaluateWhitePosition (void);
        int evaluateBlackPosition (void);
        int evaluatePosition (void);


        bool isRepetition (void);
        int scoreMove (Move &move, int ply, uint8_t phase);

        int quiesce (int alpha, int beta, int ply);
        int determineBestMove (uint8_t d, Move *move, int alpha, int beta, int ply);
        int generateMove (int time, uint8_t min_depth, uint8_t max_depth, Move *move, int *depth);
        int generateMove (int wtime, int btime, int winc, int binc, Move *move, int *depth);

        void setTTSize (int e);
};

#endif