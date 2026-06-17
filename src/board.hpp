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

#ifndef __BOARD_HPP__
#define __BOARD_HPP__

#include <stdint.h>
#include <string>
#include <random>

#define WP 0
#define WN 1
#define WB 2
#define WR 3
#define WQ 4
#define WK 5

#define BP 6
#define BN 7
#define BB 8
#define BR 9
#define BQ 10
#define BK 11

#define WO 0
#define BO 1
#define AO 2

#define WHITE 0
#define BLACK 1

#define NP 0
#define PN 1
#define PB 2
#define PR 3
#define PQ 4

#define NC 0
#define KSC 1
#define QSC 2

#define setBit(v, i) ((v) |= 1ull << (i))
#define getBit(v, i) ((v >> (i)) & 1ull)
#define clrBit(v, i) ((v) &= ~(1ull << (i)))

#define getFileMask(f) (0x101010101010101ull << (f))

typedef struct {
    uint8_t wkm : 1;     // King moved
    uint8_t wksrm : 1;   // King-side rook moved
    uint8_t wqsrm : 1;   // Queen-side rook moved
    uint8_t wc : 2;      // White castled

    uint8_t bkm : 1;
    uint8_t bksrm : 1;
    uint8_t bqsrm : 1;
    uint8_t bc : 2;

    uint8_t t : 1;     // Turn
    uint8_t es : 6;    // En passant square
    uint8_t hmc : 6;   // Half-move clock
} State;

typedef struct {
    uint8_t piece : 4;
    uint8_t from : 6;
    uint8_t to : 6;
    uint8_t promotion : 3;
    uint8_t ep : 1;
    uint8_t castle : 2;
} Move;

typedef struct {
    uint64_t pieces[12];
    uint64_t occupied[3];
    State state;
    Move move;
} History;

class Board {
    public:
        uint64_t pieces[12];
        uint64_t occupied[3];
        State state = {0};

        History history[1024];
        uint16_t history_n = 0;
        uint16_t bmc = 0;   // Base move count (for FEN setup)

        uint64_t pawn_attacks[2][64] = {0};
        uint64_t knight_attacks[64] = {0};
        uint64_t king_attacks[64] = {0};

        uint64_t diagonal_masks[64] = {0};
        uint64_t diagonal_magics[64] = {0};
        uint64_t (*diagonal_magic_attacks)[512];
        uint64_t straight_masks[64] = {0};
        uint64_t straight_magics[64] = {0};
        uint64_t (*straight_magic_attacks)[4096];

        std::mt19937_64 rng;
        uint64_t zobrist[781];

        Board (void);
        ~Board (void);

        void resetPosition (void);

        uint64_t random64 (void);
        uint64_t sparseRandom64 (void);

        uint64_t getHash (void);

        void fillPawnAttacks (void);
        void fillKnightAttacks (void);
        void fillKingAttacks (void);

        void fillDiagonalMasks (void);
        void fillDiagonalAttacks (uint64_t occupancies[64][512], uint64_t attacks[64][512]);
        void findDiagonalMagics (uint64_t occupancies[64][512], uint64_t attacks[64][512]);
        void fillStraightMasks (void);
        void fillStraightAttacks (uint64_t occupancies[64][4096], uint64_t attacks[64][4096]);
        void findStraightMagics (uint64_t occupancies[64][4096], uint64_t attacks[64][4096]);

        uint8_t diagonalSlider (Move *moves, uint8_t piece, uint8_t turn);
        uint8_t straightSlider (Move *moves, uint8_t piece, uint8_t turn);

        bool isWhiteSquareAttacked (uint8_t square);
        bool isBlackSquareAttacked (uint8_t square);
        bool isPositionLegal (void);

        uint8_t generateWhitePawnMoves (Move *moves);
        uint8_t generateWhiteKnightMoves (Move *moves);
        uint8_t generateWhiteBishopMoves (Move *moves);
        uint8_t generateWhiteRookMoves (Move *moves);
        uint8_t generateWhiteQueenMoves (Move *moves);
        uint8_t generateWhiteKingMoves (Move *moves);

        uint8_t generateBlackPawnMoves (Move *moves);
        uint8_t generateBlackKnightMoves (Move *moves);
        uint8_t generateBlackBishopMoves (Move *moves);
        uint8_t generateBlackRookMoves (Move *moves);
        uint8_t generateBlackQueenMoves (Move *moves);
        uint8_t generateBlackKingMoves (Move *moves);

        uint8_t generatePseudoLegalMoves (Move *moves);

        bool applyMove (Move &move);             // Returns false if illegal
        bool applyMove (std::string &move);      // UCI
        void undoMove (void);
        std::string moveToString (Move &move);   // UCI

        uint64_t perft (uint8_t n);

        inline bool isCapture (Move m) {return getBit(occupied[AO], m.to) || m.ep;}
        inline bool isInCheck (void) {return state.t ? isBlackSquareAttacked(__builtin_ctzll(pieces[BK])) : isWhiteSquareAttacked(__builtin_ctzll(pieces[WK]));}
        inline bool simpleMovesEq (Move m1, Move m2) {return m1.to == m2.to && m1.from == m2.from;}
        uint8_t getPieceAt (uint8_t square);
};

#endif