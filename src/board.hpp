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

#include "util.hpp"

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

        uint64_t zobrist[781];

        Board (void);
        ~Board (void);

        void resetPosition (void);

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

        uint64_t generatePawnMoves (uint8_t i, uint8_t colour);
        inline uint64_t generateKnightMoves (uint8_t i, uint8_t colour) {return knight_attacks[i] & ~occupied[colour];}
        inline uint64_t generateDiagonalMoves (uint8_t i, uint8_t colour) {return (~occupied[colour]) & diagonal_magic_attacks[i][((occupied[AO] & diagonal_masks[i]) * diagonal_magics[i]) >> (64 - __builtin_popcountll(diagonal_masks[i]))];}
        inline uint64_t generateStraightMoves (uint8_t i, uint8_t colour) {return (~occupied[colour]) & straight_magic_attacks[i][((occupied[AO] & straight_masks[i]) * straight_magics[i]) >> (64 - __builtin_popcountll(straight_masks[i]))];}
        inline uint64_t generateKingMoves (uint8_t i, uint8_t colour) {return (king_attacks[i] & ~occupied[colour]);}

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
        uint8_t applyMove (std::string &move);   // UCI (returns 0 if legal, 1 if illegal in second stage, 2 if illegal in first stage)
        void undoMove (void);
        std::string moveToString (Move &move);   // UCI

        uint64_t perft (uint8_t n);

        inline bool isCapture (Move m);
        inline bool isInCheck (void);
        inline bool simpleMovesEq (Move m1, Move m2);
        inline bool fullMovesEq (Move m1, Move m2);
        uint8_t getPieceAt (uint8_t square);
};

inline bool Board::isCapture (Move m) {
    return getBit(occupied[AO], m.to) || m.ep;
}

inline bool Board::isInCheck (void) {
    return state.t ? isBlackSquareAttacked(__builtin_ctzll(pieces[BK])) : isWhiteSquareAttacked(__builtin_ctzll(pieces[WK]));
}

inline bool Board::simpleMovesEq (Move m1, Move m2) {
    return m1.to == m2.to && m1.from == m2.from;
}

inline bool Board::fullMovesEq (Move m1, Move m2) {
    return m1.to == m2.to
            && m1.from == m2.from
            && m1.piece == m2.piece
            && ((m1.piece == WP || m1.piece == BP) ? (m1.promotion == m2.promotion && m1.ep == m2.ep) : true)
            && ((m1.piece == WK || m1.piece == BK) ? (m1.castle == m2.castle) : true);
}

#endif