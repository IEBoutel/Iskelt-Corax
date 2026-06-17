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

#include "board.hpp"
#include <cstring>
#include <random>

Board::Board (void) {
    pieces[WP] = 0b1111111100000000;
    pieces[WN] = 0b01000010;
    pieces[WB] = 0b00100100;
    pieces[WR] = 0b10000001;
    pieces[WQ] = 0b00001000;
    pieces[WK] = 0b00010000;

    pieces[BP] = 0b11111111ull << 48;
    pieces[BN] = 0b01000010ull << 56;
    pieces[BB] = 0b00100100ull << 56;
    pieces[BR] = 0b10000001ull << 56;
    pieces[BQ] = 0b00001000ull << 56;
    pieces[BK] = 0b00010000ull << 56;

    occupied[WO] = 0xFFFF;
    occupied[BO] = 0xFFFFull << 48;
    occupied[AO] = 0xFFFF | (0xFFFFull << 48);

    state.wkm = 0;
    state.wksrm = 0;
    state.wqsrm = 0;

    state.bkm = 0;
    state.bksrm = 0;
    state.bqsrm = 0;

    state.t = WHITE;
    state.es = 0;

    fillPawnAttacks();
    fillKnightAttacks();
    fillKingAttacks();

    diagonal_magic_attacks = new uint64_t[64][512]();
    straight_magic_attacks = new uint64_t[64][4096]();

    auto diagonal_occupancies = new uint64_t[64][512]();
    auto diagonal_attacks = new uint64_t[64][512]();
    auto straight_occupancies = new uint64_t[64][4096]();
    auto straight_attacks = new uint64_t[64][4096]();

    fillDiagonalMasks();
    fillDiagonalAttacks(diagonal_occupancies, diagonal_attacks);
    findDiagonalMagics(diagonal_occupancies, diagonal_attacks);
    fillStraightMasks();
    fillStraightAttacks(straight_occupancies, straight_attacks);
    findStraightMagics(straight_occupancies, straight_attacks);

    delete[] diagonal_occupancies;
    delete[] diagonal_attacks;
    delete[] straight_occupancies;
    delete[] straight_attacks;

    rng = std::mt19937_64(std::random_device{}());

    for (uint64_t i = 0; i < 781; i++) {
        zobrist[i] = random64();
    }

    state.hash = getHash();
}

uint64_t Board::random64 (void) {
    return rng();
}

uint64_t Board::sparseRandom64 (void) {
    return random64() & random64() & random64();
}

void Board::fillPawnAttacks (void) {
    for (uint8_t i = 0; i < 56; i++) {
        if (i % 8 != 7) {
            pawn_attacks[WHITE][i] |= 1ull << (i + 9);
            pawn_attacks[BLACK][i + 8] |= 1ull << (i + 1);
        }
        
        if (i % 8) {
            pawn_attacks[WHITE][i] |= 1ull << (i + 7);
            pawn_attacks[BLACK][i + 8] |= 1ull << (i - 1);
        }
    }
}

void Board::fillKnightAttacks (void) {
    for (uint8_t i = 0; i < 64; i++) {
        if (i % 8 != 7 && i < 48) {
            knight_attacks[i] |= 1ull << (i + 17);
        }

        if (i % 8 && i < 48) {
            knight_attacks[i] |= 1ull << (i + 15);
        }

        if (i % 8 > 1 && i < 56) {
            knight_attacks[i] |= 1ull << (i + 6);
        }

        if (i % 8 < 6 && i < 56) {
            knight_attacks[i] |= 1ull << (i + 10);
        }

        if (i % 8 && i > 15) {
            knight_attacks[i] |= 1ull << (i - 17);
        }

        if (i % 8 != 7 && i > 15) {
            knight_attacks[i] |= 1ull << (i - 15);
        }

        if (i % 8 < 6 && i > 7) {
            knight_attacks[i] |= 1ull << (i - 6);
        }

        if (i % 8 > 1 && i > 7) {
            knight_attacks[i] |= 1ull << (i - 10);
        }
    }
}

void Board::fillKingAttacks (void) {
    for (uint8_t i = 0; i < 64; i++) {
        if (i < 56) {
            king_attacks[i] |= 1ull << (i + 8);
        }

        if (i < 56 && i % 8 != 7) {
            king_attacks[i] |= 1ull << (i + 9);
        }

        if (i < 56 && i % 8) {
            king_attacks[i] |= 1ull << (i + 7);
        }

        if (i % 8) {
            king_attacks[i] |= 1ull << (i - 1);
        }

        if (i % 8 != 7) {
            king_attacks[i] |= 1ull << (i + 1);
        }

        if (i > 7) {
            king_attacks[i] |= 1ull << (i - 8);
        }

        if (i > 7 && i % 8) {
            king_attacks[i] |= 1ull << (i - 9);
        }

        if (i > 7 && i % 8 != 7) {
            king_attacks[i] |= 1ull << (i - 7);
        }
    }
}

void Board::fillDiagonalMasks (void) {
    for (uint8_t i = 0; i < 64; i++) {
        for (uint8_t j = i; j < 56 && j % 8 != 7; j += 9) {
            setBit(diagonal_masks[i], j);
        }

        for (uint8_t j = i; j < 56 && j % 8; j += 7) {
            setBit(diagonal_masks[i], j);
        }

        for (uint8_t j = i; j > 7 && j % 8; j -= 9) {
            setBit(diagonal_masks[i], j);
        }

        for (uint8_t j = i; j > 7 && j % 8 != 7; j -= 7) {
            setBit(diagonal_masks[i], j);
        }

        clrBit(diagonal_masks[i], i);
    }
}

void Board::fillStraightMasks (void) {
    for (uint8_t i = 0; i < 64; i++) {
        for (uint8_t j = i; j < 56; j += 8) {
            setBit(straight_masks[i], j);
        }

        for (uint8_t j = i; j % 8 != 7; j++) {
            setBit(straight_masks[i], j);
        }

        for (uint8_t j = i; j > 7; j -= 8) {
            setBit(straight_masks[i], j);
        }

        for (uint8_t j = i; j % 8; j--) {
            setBit(straight_masks[i], j);
        }

        clrBit(straight_masks[i], i);
    }
}

void Board::fillDiagonalAttacks (uint64_t occupancies[64][512], uint64_t attacks[64][512]) {
    for (uint8_t i = 0; i < 64; i++) {
        uint16_t j;
        uint8_t n1 = __builtin_popcountll(diagonal_masks[i]);
        uint16_t n2 = 1 << n1;

        for (j = 0; j < n2; j++) {
            uint64_t mask = diagonal_masks[i];
            uint8_t k = 0;
            uint8_t l = 0;

            while (mask) {
                k = __builtin_ctzll(mask);
                clrBit(mask, k);

                if (getBit(j, l)) {
                    setBit(occupancies[i][j], k);
                }

                l++;
            }
        }

        for (j = 0; j < n2; j++) {
            uint8_t k;

            for (k = i + 9; k < 64 && k % 8; k += 9) {
                setBit(attacks[i][j], k);

                if (getBit(occupancies[i][j], k)) {
                    break;
                }
            }

            for (k = i + 7; k < 64 && k % 8 != 7; k += 7) {
                setBit(attacks[i][j], k);

                if (getBit(occupancies[i][j], k)) {
                    break;
                }
            }

            for (k = i - 9; k < i && k % 8 != 7; k -= 9) {
                setBit(attacks[i][j], k);

                if (getBit(occupancies[i][j], k)) {
                    break;
                }
            }

            for (k = i - 7; k < i && k % 8; k -= 7) {
                setBit(attacks[i][j], k);

                if (getBit(occupancies[i][j], k)) {
                    break;
                }
            }
        }
    }
}

void Board::fillStraightAttacks (uint64_t occupancies[64][4096], uint64_t attacks[64][4096]) {
    for (uint8_t i = 0; i < 64; i++) {
        uint16_t j;
        uint8_t n1 = __builtin_popcountll(straight_masks[i]);
        uint16_t n2 = 1 << n1;

        for (j = 0; j < n2; j++) {
            uint64_t mask = straight_masks[i];
            uint8_t k = 0;
            uint8_t l = 0;

            while (mask) {
                k = __builtin_ctzll(mask);
                clrBit(mask, k);

                if (getBit(j, l)) {
                    setBit(occupancies[i][j], k);
                }

                l++;
            }
        }

        for (j = 0; j < n2; j++) {
            uint8_t k;

            for (k = i + 8; k < 64; k += 8) {
                setBit(attacks[i][j], k);

                if (getBit(occupancies[i][j], k)) {
                    break;
                }
            }

            for (k = i + 1; k % 8; k++) {
                setBit(attacks[i][j], k);

                if (getBit(occupancies[i][j], k)) {
                    break;
                }
            }

            for (k = i - 1; k % 8 != 7; k--) {
                setBit(attacks[i][j], k);

                if (getBit(occupancies[i][j], k)) {
                    break;
                }
            }

            for (k = i - 8; k < i; k -= 8) {
                setBit(attacks[i][j], k);

                if (getBit(occupancies[i][j], k)) {
                    break;
                }
            }
        }
    }
}

void Board::findDiagonalMagics (uint64_t occupancies[64][512], uint64_t attacks[64][512]) {
    for (uint8_t i = 0; i < 64; i++) {
        uint8_t n1 = __builtin_popcountll(diagonal_masks[i]);
        uint16_t n2 = 1 << n1;

        while (1) {
            uint64_t magic = sparseRandom64();
            uint64_t used[512] = {0};
            bool filled[512] = {false};
            uint16_t index;
            bool fail = false;
            uint16_t j;

            for (j = 0; j < n2; j++) {
                index = (occupancies[i][j] * magic) >> (64 - n1);

                if (!filled[index]) {
                    filled[index] = true;
                    used[index] = attacks[i][j];
                    diagonal_magic_attacks[i][index] = attacks[i][j];
                } else if (used[index] != attacks[i][j]) {
                    fail = true;
                    break;
                }
            }

            if (!fail) {
                diagonal_magics[i] = magic;
                break;
            }
        }
    }
}

void Board::findStraightMagics (uint64_t occupancies[64][4096], uint64_t attacks[64][4096]) {
    for (uint8_t i = 0; i < 64; i++) {
        uint8_t n1 = __builtin_popcountll(straight_masks[i]);
        uint16_t n2 = 1 << n1;

        while (1) {
            uint64_t magic = sparseRandom64();
            uint64_t used[4096] = {0};
            bool filled[4096] = {false};
            uint16_t index;
            bool fail = false;

            for (uint16_t j = 0; j < n2; j++) {
                index = (occupancies[i][j] * magic) >> (64 - n1);

                if (!filled[index]) {
                    filled[index] = true;
                    used[index] = attacks[i][j];
                    straight_magic_attacks[i][index] = attacks[i][j];
                } else if (used[index] != attacks[i][j]) {
                    fail = true;
                    break;
                }
            }

            if (!fail) {
                straight_magics[i] = magic;
                break;
            }
        }
    }
}

uint8_t Board::generateWhitePawnMoves (Move *moves) {
    uint8_t n = 0;
    uint64_t pawns = pieces[WP];
    uint8_t i;
    uint8_t j;
    uint64_t m;

    while (pawns) {
        i = __builtin_ctzll(pawns);
        clrBit(pawns, i);
        uint64_t pc = (~occupied[AO] & (1ull << (i + 8)));

        m = (pawn_attacks[WHITE][i] & occupied[BO])                        // Standard attacks
            | pc                                                           // Standard push
            | (i < 16 && pc ? (~occupied[AO] & (1ull << (i + 16))) : 0);   // Double push

        while (m) {
            j = __builtin_ctzll(m);
            clrBit(m, j);

            if (i < 48) {   // Standard
                moves[n].piece = WP;
                moves[n].from = i;
                moves[n].to = j;
                moves[n].promotion = NP;
                moves[n].ep = 0;
                n++;
            } else {   // Promote
                for (uint8_t k = PN; k <= PQ; k++) {
                    moves[n].piece = WP;
                    moves[n].from = i;
                    moves[n].to = j;
                    moves[n].promotion = k;
                    moves[n].ep = 0;
                    n++;
                }
            }
        }

        // En passant
        if ((i - state.es == 1 || state.es - i == 1) && i < 40 && i > 31) {
            moves[n].piece = WP;
            moves[n].from = i;
            moves[n].to = state.es + 8;
            moves[n].promotion = NP;
            moves[n].ep = 1;
            n++;
        }
    }

    return n;
}

uint8_t Board::generateBlackPawnMoves (Move *moves) {
    uint8_t n = 0;
    uint64_t pawns = pieces[BP];
    uint8_t i;
    uint8_t j;
    uint64_t m;

    while (pawns) {
        i = __builtin_ctzll(pawns);
        clrBit(pawns, i);
        uint64_t pc = (~occupied[AO] & (1ull << (i - 8)));

        m = (pawn_attacks[BLACK][i] & occupied[WO])                        // Standard attacks
            | pc                                                           // Standard push
            | (i > 47 && pc ? (~occupied[AO] & (1ull << (i - 16))) : 0);   // Double push

        while (m) {
            j = __builtin_ctzll(m);
            clrBit(m, j);

            if (i > 15) {   // Standard
                moves[n].piece = BP;
                moves[n].from = i;
                moves[n].to = j;
                moves[n].promotion = NP;
                moves[n].ep = 0;
                n++;
            } else {   // Promote
                for (uint8_t k = PN; k <= PQ; k++) {
                    moves[n].piece = BP;
                    moves[n].from = i;
                    moves[n].to = j;
                    moves[n].promotion = k;
                    moves[n].ep = 0;
                    n++;
                }
            }
        }

        // En passant
        if ((i - state.es == 1 || state.es - i == 1) && i < 32 && i > 23) {
            moves[n].piece = BP;
            moves[n].from = i;
            moves[n].to = state.es - 8;
            moves[n].promotion = NP;
            moves[n].ep = 1;
            n++;
        }
    }

    return n;
}

uint8_t Board::generateWhiteKnightMoves (Move *moves) {
    uint8_t n = 0;
    uint64_t knights = pieces[WN];
    uint8_t i;
    uint8_t j;
    uint64_t m;

    while (knights) {
        i = __builtin_ctzll(knights);
        clrBit(knights, i);

        m = (knight_attacks[i] & ~occupied[WO]);

        while (m) {
            j = __builtin_ctzll(m);
            clrBit(m, j);

            moves[n].piece = WN;
            moves[n].from = i;
            moves[n].to = j;
            n++;
        }
    }

    return n; 
}

uint8_t Board::generateBlackKnightMoves (Move *moves) {
    uint8_t n = 0;
    uint64_t knights = pieces[BN];
    uint8_t i;
    uint8_t j;
    uint64_t m;

    while (knights) {
        i = __builtin_ctzll(knights);
        clrBit(knights, i);

        m = (knight_attacks[i] & ~occupied[BO]);

        while (m) {
            j = __builtin_ctzll(m);
            clrBit(m, j);

            moves[n].piece = BN;
            moves[n].from = i;
            moves[n].to = j;
            n++;
        }
    }

    return n; 
}

uint8_t Board::diagonalSlider (Move *moves, uint8_t piece, uint8_t turn) {
    uint64_t p = pieces[piece];
    uint8_t n = 0;
    uint8_t i;
    uint8_t j;

    while (p) {
        i = __builtin_ctzll(p);
        clrBit(p, i);

        uint64_t mask = diagonal_masks[i];
        uint64_t m = (~occupied[turn]) & diagonal_magic_attacks[i][((occupied[AO] & mask) * diagonal_magics[i]) >> (64 - __builtin_popcountll(mask))];

        while (m) {
            j = __builtin_ctzll(m);
            clrBit(m, j);

            moves[n].piece = piece;
            moves[n].from = i;
            moves[n].to = j;
            n++;
        }
    }

    return n;
}

uint8_t Board::straightSlider (Move *moves, uint8_t piece, uint8_t turn) {
    uint64_t p = pieces[piece];
    uint8_t n = 0;
    uint8_t i;
    uint8_t j;

    while (p) {
        i = __builtin_ctzll(p);
        clrBit(p, i);

        uint64_t mask = straight_masks[i];
        uint64_t m = (~occupied[turn]) & straight_magic_attacks[i][((occupied[AO] & mask) * straight_magics[i]) >> (64 - __builtin_popcountll(mask))];

        while (m) {
            j = __builtin_ctzll(m);
            clrBit(m, j);

            moves[n].piece = piece;
            moves[n].from = i;
            moves[n].to = j;
            n++;
        }
    }

    return n;
}

uint8_t Board::generateWhiteBishopMoves (Move *moves) {
    return diagonalSlider(moves, WB, WHITE);
}

uint8_t Board::generateBlackBishopMoves (Move *moves) {
    return diagonalSlider(moves, BB, BLACK);
}

uint8_t Board::generateWhiteRookMoves (Move *moves) {
    return straightSlider(moves, WR, WHITE);
}

uint8_t Board::generateBlackRookMoves (Move *moves) {
    return straightSlider(moves, BR, BLACK);
}

uint8_t Board::generateWhiteQueenMoves (Move *moves) {
    uint8_t n = straightSlider(moves, WQ, WHITE);

    return n + diagonalSlider(moves + n, WQ, WHITE);
}

uint8_t Board::generateBlackQueenMoves (Move *moves) {
    uint8_t n = straightSlider(moves, BQ, BLACK);

    return n + diagonalSlider(moves + n, BQ, BLACK);
}

uint8_t Board::generateWhiteKingMoves (Move *moves) {
    uint8_t n = 0;
    uint64_t king = pieces[WK];
    uint8_t i;
    uint8_t j;
    uint64_t m;

    i = __builtin_ctzll(king);
    m = (king_attacks[i] & ~occupied[WO]);

    while (m) {
        j = __builtin_ctzll(m);
        clrBit(m, j);

        moves[n].piece = WK;
        moves[n].from = i;
        moves[n].to = j;
        moves[n].castle = NC;
        n++;
    }

    if (!state.wkm) {
        if (!state.wksrm && !(occupied[AO] & (3ull << (i + 1)))) {
            moves[n].piece = WK;
            moves[n].from = i;
            moves[n].to = i + 2;
            moves[n].castle = KSC;
            n++;
        }

        if (!state.wqsrm && !(occupied[AO] & (7ull << (i - 3)))) {
            moves[n].piece = WK;
            moves[n].from = i;
            moves[n].to = i - 2;
            moves[n].castle = QSC;
            n++;
        }
    }

    return n; 
}

uint8_t Board::generateBlackKingMoves (Move *moves) {
    uint8_t n = 0;
    uint64_t king = pieces[BK];
    uint8_t i;
    uint8_t j;
    uint64_t m;

    i = __builtin_ctzll(king);
    m = (king_attacks[i] & ~occupied[BO]);

    while (m) {
        j = __builtin_ctzll(m);
        clrBit(m, j);

        moves[n].piece = BK;
        moves[n].from = i;
        moves[n].to = j;
        moves[n].castle = NC;
        n++;
    }

    if (!state.bkm) {
        if (!state.bksrm && !(occupied[AO] & (3ull << (i + 1)))) {
            moves[n].piece = BK;
            moves[n].from = i;
            moves[n].to = i + 2;
            moves[n].castle = KSC;
            n++;
        }

        if (!state.bqsrm && !(occupied[AO] & (7ull << (i - 3)))) {
            moves[n].piece = BK;
            moves[n].from = i;
            moves[n].to = i - 2;
            moves[n].castle = QSC;
            n++;
        }
    }

    return n; 
}

uint8_t Board::generatePseudoLegalMoves (Move *moves) {
    uint8_t n;

    if (state.t == WHITE) {
        n = generateWhitePawnMoves(moves);
        n += generateWhiteKnightMoves(moves + n);
        n += generateWhiteBishopMoves(moves + n);
        n += generateWhiteRookMoves(moves + n);
        n += generateWhiteQueenMoves(moves + n);
        n += generateWhiteKingMoves(moves + n);
    } else {
        n = generateBlackPawnMoves(moves);
        n += generateBlackKnightMoves(moves + n);
        n += generateBlackBishopMoves(moves + n);
        n += generateBlackRookMoves(moves + n);
        n += generateBlackQueenMoves(moves + n);
        n += generateBlackKingMoves(moves + n);
    }

    return n;
}

bool Board::isWhiteSquareAttacked (uint8_t square) {
    uint64_t mask = diagonal_masks[square];
    uint64_t occupancy = occupied[AO] & mask;
    uint64_t shift = 64 - __builtin_popcountll(mask);
    uint64_t index = (occupancy * diagonal_magics[square]) >> shift;
    uint64_t bishop_attacks = diagonal_magic_attacks[square][index];

    mask = straight_masks[square];
    occupancy = occupied[AO] & mask;
    shift = 64 - __builtin_popcountll(mask);
    index = (occupancy * straight_magics[square]) >> shift;
    uint64_t rook_attacks = straight_magic_attacks[square][index];

    uint64_t queen_attacks = bishop_attacks | rook_attacks;

    if ((pieces[BP] & pawn_attacks[WHITE][square]) | (pieces[BN] & knight_attacks[square]) | (pieces[BB] & bishop_attacks) | (pieces[BR] & rook_attacks) | (pieces[BQ] & queen_attacks) | (pieces[BK] & king_attacks[square])) {
        return true;
    }

    return false;
}

bool Board::isBlackSquareAttacked (uint8_t square) {
    uint64_t mask = diagonal_masks[square];
    uint64_t occupancy = occupied[AO] & mask;
    uint64_t shift = 64 - __builtin_popcountll(mask);
    uint64_t index = (occupancy * diagonal_magics[square]) >> shift;
    uint64_t bishop_attacks = diagonal_magic_attacks[square][index];

    mask = straight_masks[square];
    occupancy = occupied[AO] & mask;
    shift = 64 - __builtin_popcountll(mask);
    index = (occupancy * straight_magics[square]) >> shift;
    uint64_t rook_attacks = straight_magic_attacks[square][index];

    uint64_t queen_attacks = bishop_attacks | rook_attacks;

    if ((pieces[WP] & pawn_attacks[BLACK][square]) | (pieces[WN] & knight_attacks[square]) | (pieces[WB] & bishop_attacks) | (pieces[WR] & rook_attacks) | (pieces[WQ] & queen_attacks) | (pieces[WK] & king_attacks[square])) {
        return true;
    }

    return false;
}

bool Board::applyMove (Move &move) {
    memcpy(history[history_n].pieces, pieces, sizeof(pieces));
    memcpy(history[history_n].occupied, occupied, sizeof(occupied));
    history[history_n].state = state;
    history[history_n].move = move;
    history_n++;
    bool inc_hmc = true;

    bool ep_set = false;

    if (move.piece == WP || move.piece == BP) {
        uint8_t np = move.promotion ? move.piece + move.promotion : move.piece;
        inc_hmc = false;
        
        clrBit(pieces[move.piece], move.from);
        setBit(pieces[np], move.to);

        if (move.ep) {
            clrBit(pieces[move.piece == WP ? BP : WP], state.es);
        }

        if (move.to - move.from == 16 || move.from - move.to == 16) {
            state.es = move.to;
            ep_set = true;
        }
    } else if ((move.piece == WK || move.piece == BK) && move.castle) {
        if (move.castle == KSC) {
            if (state.t == WHITE) {
                if (isWhiteSquareAttacked(move.from) || isWhiteSquareAttacked(move.from + 1)) {
                    return false;
                }
            } else {
                if (isBlackSquareAttacked(move.from) || isBlackSquareAttacked(move.from + 1)) {
                    return false;
                }
            }

            clrBit(pieces[move.piece], move.from);
            setBit(pieces[move.piece], move.to);
            clrBit(pieces[move.piece - 2], move.from + 3);
            setBit(pieces[move.piece - 2], move.from + 1);
        } else {
            if (state.t == WHITE) {
                if (isWhiteSquareAttacked(move.from) || isWhiteSquareAttacked(move.from - 1) || isWhiteSquareAttacked(move.from - 2)) {
                    return false;
                }
            } else {
                if (isBlackSquareAttacked(move.from) || isBlackSquareAttacked(move.from - 1) || isBlackSquareAttacked(move.from - 2)) {
                    return false;
                }
            }

            clrBit(pieces[move.piece], move.from);
            setBit(pieces[move.piece], move.to);
            clrBit(pieces[move.piece - 2], move.from - 4);
            setBit(pieces[move.piece - 2], move.from - 1);
        }

        if (state.t == WHITE) {
            state.wc = move.castle;
        } else {
            state.bc = move.castle;
        }
    } else {
        clrBit(pieces[move.piece], move.from);
        setBit(pieces[move.piece], move.to);
    }

    if (getBit(occupied[AO], move.to)) {
        for (uint8_t i = WP + 6 * !state.t; i <= WK + 6 * !state.t; i++) {
            if (getBit(pieces[i], move.to)) {
                inc_hmc = false;
                clrBit(pieces[i], move.to);
                break;
            }
        }
    }

    occupied[WO] = pieces[WP] | pieces[WN] | pieces[WB] | pieces[WR] | pieces[WQ] | pieces[WK];
    occupied[BO] = pieces[BP] | pieces[BN] | pieces[BB] | pieces[BR] | pieces[BQ] | pieces[BK];
    occupied[AO] = occupied[WO] | occupied[BO];

    state.wkm = state.wkm ? 1 : !(pieces[WK] & (1ull << 4));
    state.wksrm = state.wksrm ? 1 : !(pieces[WR] & (1ull << 7));
    state.wqsrm = state.wqsrm ? 1 : !(pieces[WR] & 1ull);

    state.bkm = state.bkm ? 1 : !(pieces[BK] & (1ull << 60));
    state.bksrm = state.bksrm ? 1 : !(pieces[BR] & (1ull << 63));
    state.bqsrm = state.bqsrm ? 1 : !(pieces[BR] & (1ull << 56));

    state.t = !state.t;
    state.hash = getHash();

    if (!ep_set) {
        state.es = 0;
    }

    if (inc_hmc) {
        state.hmc++;
    }

    return isPositionLegal();
}

bool Board::isPositionLegal (void) {
    if (state.t == WHITE) {
        return !isBlackSquareAttacked(__builtin_ctzll(pieces[BK]));
    }

    return !isWhiteSquareAttacked(__builtin_ctzll(pieces[WK]));
}

void Board::undoMove (void) {
    history_n--;
    memcpy(pieces, history[history_n].pieces, sizeof(pieces));
    memcpy(occupied, history[history_n].occupied, sizeof(occupied));
    state = history[history_n].state;
}

uint64_t Board::perft(uint8_t n) {
    Move moves[256];

    if (!n) {
        return 1;
    }

    uint8_t n1 = generatePseudoLegalMoves(moves);
    uint64_t n2 = 0;

    for (uint8_t i = 0; i < n1; i++) {
        if (applyMove(moves[i])) {
            n2 += perft(n - 1);
        }

        undoMove();
    }

    return n2;
}

bool Board::applyMove (std::string &move) {
    if (move.length() < 4) {
        return false;
    }

    Move m;

    m.from = (move[0] - 'a') + ((move[1] - '1') * 8);
    m.to = (move[2] - 'a') + ((move[3] - '1') * 8);
    m.piece = getPieceAt(m.from);

    if (move.length() == 5) {
        switch (move[4]) {
            case 'n':
                m.promotion = PN;
                break;

            case 'b':
                m.promotion = PB;
                break;

            case 'r':
                m.promotion = PR;
                break;

            case 'q':
                m.promotion = PQ;
                break;
            
            default:
                return false;
        }
    } else {
        m.promotion = NP;
    }

    if (m.piece == WP) {
        if ((m.from - state.es == 1 || state.es - m.from == 1) && m.to - state.es == 8) {
            m.ep = 1;
        }
    } else if (m.piece == BP) {
        if ((m.from - state.es == 1 || state.es - m.from == 1) && state.es - m.to == 8) {
            m.ep = 1;
        }
    }

    if (m.piece == WK) {
        if (m.to - m.from == 2) {
            if (state.wkm || state.wksrm) {
                return false;
            }

            m.castle = KSC;
        } else if (m.from - m.to == 2) {
            if (state.wkm || state.wqsrm) {
                return false;
            }

            m.castle = QSC;
        }
    }

    if (m.piece == BK) {
        if (m.to - m.from == 2) {
            if (state.bkm || state.bksrm) {
                return false;
            }

            m.castle = KSC;
        } else if (m.from - m.to == 2) {
            if (state.bkm || state.bqsrm) {
                return false;
            }

            m.castle = QSC;
        }
    }

    return applyMove(m);
}

std::string Board::moveToString (Move &move) {
    std::string output = std::string() + (char) ((move.from % 8) + 'a') + (char) ((move.from / 8) + '1') + (char) ((move.to % 8) + 'a') + (char) ((move.to / 8) + '1');

    if (move.promotion && (move.piece == WP || move.piece == BP)) {
        switch (move.promotion) {
            case PN:
                output += 'n';
                break;

            case PB:
                output += 'b';
                break;

            case PR:
                output += 'r';
                break;

            case PQ:
                output += 'q';
                break;
            
            default:;
        }
    }

    return output;
}

uint64_t Board::getHash (void) {
    uint64_t hash = 0;

    for (uint8_t i = WP; i <= BK; i++) {
        uint64_t bb = pieces[i];
        uint8_t j;

        while (bb) {
            j = __builtin_ctzll(bb);
            clrBit(bb, j);

            hash ^= zobrist[(i * 64) + j];
        }
    }

    if (!state.wkm) {
        hash ^= zobrist[768] * state.wksrm;
        hash ^= zobrist[769] * state.wqsrm;
    }

    if (!state.bkm) {
        hash ^= zobrist[770] * state.bksrm;
        hash ^= zobrist[771] * state.bqsrm;
    }

    if (state.es) {
        hash ^= zobrist[772 + (state.es % 8)];
    }

    return hash ^ (zobrist[780] * state.t);
}

uint8_t Board::getPieceAt (uint8_t square) {
    for (uint8_t p = WP; p <= BK; p++) {
        if (getBit(pieces[p], square)) {
            return p;
        }
    }

    return 255;
}