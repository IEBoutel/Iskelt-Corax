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

#include "engine.hpp"
#include <cstring>
#include <chrono>

int PS[5] = {100, 290, 310, 500, 900};   // Material scores
int PW[4] = {1, 1, 2, 4};                // Piece phase weights

int P_DP = -25;                                      // Double pawn
int P_IP = -25;                                      // Isolated pawn
int P_PP[8] = {0, 5, 10, 20, 40, 60, 80, 100};       // Passed pawn bonus (rank based)
int B_DB = 25;                                       // Double bishop
int R_OF = 15;                                       // Semi-open rook file (doubles for full open)
int K_DP[8] = {0, 0, -5, -20, -40, -40, -40, -40};   // King defender pawn (castles) (rank based)
int K_MP = -50;                                      // Missing pawn (castle)
int K_CB = 50;                                       // Castle bonus

int TB = 10;   // Turn bonus (tempo)

int MG_BP_PST[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    45, 50, 50, 55, 55, 50, 50, 45,
    15, 20, 20, 10, 10, 20, 20, 15,
    5, 10, 10, 20, 20, 10, 10, 5,
    0, 5, 5, 10, 10, 5, 5, 0,
    0, -5, -5, 0, 0, -5, -5, 0,
    5, 0, 0, -5, -5, 0, 0, 5,
    0, 0, 0, 0, 0, 0, 0, 0
};

int MG_WP_PST[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 0, 0, -5, -5, 0, 0, 5,
    0, -5, -5, 0, 0, -5, -5, 0,
    0, 5, 5, 10, 10, 5, 5, 0,
    5, 10, 10, 20, 20, 10, 10, 5,
    15, 20, 20, 10, 10, 20, 20, 15,
    45, 50, 50, 55, 55, 50, 50, 45,
    0, 0, 0, 0, 0, 0, 0, 0
};

int MG_N_PST[64] = {
    -20, -10, -5, -5, -5, -5, -10, -20,
    -10, -5, 0, 0, 0, 0, -5, -10,
    -5, 0, 10, 15, 15, 10, 0, -5,
    -5, 5, 15, 20, 20, 15, 5, -5,
    -5, 5, 15, 20, 20, 15, 5, -5,
    -5, 0, 10, 15, 15, 10, 0, -5,
    -10, -5, 0, 0, 0, 0, -5, -10,
    -20, -10, -5, -5, -5, -5, -10, -20
};

int MG_B_PST[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};

int MG_R_PST[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0
};

int MG_Q_PST[64] = {
    -20, -5, -5, -5, -5, -5, -5, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -20, -5, -5, -5, -5, -5, -5, -20
};

int MG_WK_PST[64] = {
    20, 30, 10, 0, 0, 10, 30, 20,
    0, -5, -10, -10, -10, -10, -5, 0,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30
};

int MG_BK_PST[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    0, -5, -10, -10, -10, -10, -5, 0,
    20, 30, 10, 0, 0, 10, 30, 20
};

int EG_K_PST[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -30, -10, -10, -10, -10, -30, -40,
    -30, -10, 0, 10, 10, 0, -10, -30,
    -30, -10, 5, 15, 15, 5, -10, -30,
    -30, -10, 5, 15, 15, 5, -10, -30,
    -30, -10, 0, 10, 10, 0, -10, -30,
    -40, -30, -10, -10, -10, -10, -30, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

int Engine::evaluateWhitePosition (void) {
    int score = 0;
    uint8_t i = 0;
    uint64_t pawns = board.pieces[WP];
    uint64_t knights = board.pieces[WN];
    uint64_t bishops = board.pieces[WB];
    uint64_t rooks = board.pieces[WR];
    uint64_t queens = board.pieces[WQ];
    uint8_t phase = (PW[0] * __builtin_popcountll(board.pieces[WN] | board.pieces[BN])) + (PW[1] * __builtin_popcountll(board.pieces[WB] | board.pieces[BB])) + (PW[2] * __builtin_popcountll(board.pieces[WR] | board.pieces[BR])) + (PW[3] * __builtin_popcountll(board.pieces[WQ] | board.pieces[BQ]));
    
    while (pawns) {
        i = __builtin_ctzll(pawns);
        clrBit(pawns, i);

        score += PS[0] + MG_WP_PST[i];

        if (!(board.pieces[BP] & pp_mask[WHITE][i])) {
            score += P_PP[i / 8];
        }

        if (__builtin_popcountll(board.pieces[WP] & dp_mask[i]) > 1) {
            score += P_DP;
        }

        if (!(board.pieces[WP] & ip_mask[i])) {
            score += P_IP;
        }
    }

    while (knights) {
        i = __builtin_ctzll(knights);
        clrBit(knights, i);

        score += PS[1] + MG_N_PST[i];
    }

    if (__builtin_popcountll(bishops) > 1) {
        score += B_DB;
    }

    while (bishops) {
        i = __builtin_ctzll(bishops);
        clrBit(bishops, i);

        score += PS[2] + MG_B_PST[i];
    }

    while (rooks) {
        i = __builtin_ctzll(rooks);
        clrBit(rooks, i);

        score += PS[3] + MG_R_PST[i];
        uint64_t file_mask = getFileMask(i % 8);

        if (!(file_mask & board.pieces[WP])) {
            score += R_OF;
        }

        if (!(file_mask & board.pieces[BP])) {
            score += R_OF;
        }
    }

    while (queens) {
        i = __builtin_ctzll(queens);
        clrBit(queens, i);

        score += PS[4] + MG_Q_PST[i];
    }

    uint8_t king = __builtin_ctzll(board.pieces[WK]);

    if ((board.state.wc == KSC && (king == 6 || king == 7)) || (board.state.wc == QSC && (king == 0 || king == 1 || king == 2))) {
        uint64_t rp = board.pieces[WP] & kp_mask[king];
        score += (phase * (K_CB + K_MP * (3 - __builtin_popcountll(rp)))) / MAX_PHASE;

        while (rp) {
            i = __builtin_ctzll(rp);
            clrBit(rp, i);

            score += (phase * K_DP[i / 8]) / MAX_PHASE;
        }
    }

    return score + (phase * MG_WK_PST[king] + (MAX_PHASE - phase) * EG_K_PST[king]) / MAX_PHASE + (TB * !board.state.t);
}

int Engine::evaluateBlackPosition (void) {
    int score = 0;
    uint8_t i = 0;
    uint64_t pawns = board.pieces[BP];
    uint64_t knights = board.pieces[BN];
    uint64_t bishops = board.pieces[BB];
    uint64_t rooks = board.pieces[BR];
    uint64_t queens = board.pieces[BQ];
    uint8_t phase = (PW[0] * __builtin_popcountll(board.pieces[WN] | board.pieces[BN])) + (PW[1] * __builtin_popcountll(board.pieces[WB] | board.pieces[BB])) + (PW[2] * __builtin_popcountll(board.pieces[WR] | board.pieces[BR])) + (PW[3] * __builtin_popcountll(board.pieces[WQ] | board.pieces[BQ]));
    
    while (pawns) {
        i = __builtin_ctzll(pawns);
        clrBit(pawns, i);

        score += PS[0] + MG_BP_PST[i];

        if (!(board.pieces[WP] & pp_mask[BLACK][i])) {
            score += P_PP[7 - (i / 8)];
        }

        if (__builtin_popcountll(board.pieces[BP] & dp_mask[i]) > 1) {
            score += P_DP;
        }

        if (!(board.pieces[BP] & ip_mask[i])) {
            score += P_IP;
        }
    }

    while (knights) {
        i = __builtin_ctzll(knights);
        clrBit(knights, i);

        score += PS[1] + MG_N_PST[i];
    }

    if (__builtin_popcountll(bishops) > 1) {
        score += B_DB;
    }

    while (bishops) {
        i = __builtin_ctzll(bishops);
        clrBit(bishops, i);

        score += PS[2] + MG_B_PST[i];
    }

    while (rooks) {
        i = __builtin_ctzll(rooks);
        clrBit(rooks, i);

        score += PS[3] + MG_R_PST[i];
        uint64_t file_mask = getFileMask(i % 8);

        if (!(file_mask & board.pieces[WP])) {
            score += R_OF;
        }

        if (!(file_mask & board.pieces[BP])) {
            score += R_OF;
        }
    }

    while (queens) {
        i = __builtin_ctzll(queens);
        clrBit(queens, i);

        score += PS[4] + MG_Q_PST[i];
    }

    uint8_t king = __builtin_ctzll(board.pieces[BK]);

    if ((board.state.bc == KSC && (king == 63 || king == 62)) || (board.state.bc == QSC && (king == 56 || king == 57 || king == 58))) {
        uint64_t rp = board.pieces[BP] & kp_mask[king];
        score += (phase * (K_CB + K_MP * (3 - __builtin_popcountll(rp)))) / MAX_PHASE;

        while (rp) {
            i = __builtin_ctzll(rp);
            clrBit(rp, i);

            score += (phase * K_DP[7 - (i / 8)]) / MAX_PHASE;
        }
    }

    return score + (phase * MG_BK_PST[king] + (MAX_PHASE - phase) * EG_K_PST[king]) / MAX_PHASE + (TB * board.state.t);
}

int Engine::evaluatePosition (void) {
    return board.state.t == WHITE ? evaluateWhitePosition() - evaluateBlackPosition() : evaluateBlackPosition() - evaluateWhitePosition();
}

int Engine::determineBestMove (uint8_t d, Move *move, int alpha, int beta, int ply) {
    if (md && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - ts > tl) {
        return TIME_BREAK;
    }

    if (board.history_n > 7) {
        if (
            board.history[board.history_n - 1].move.to == board.history[board.history_n - 5].move.to && board.history[board.history_n - 1].move.from == board.history[board.history_n - 5].move.from &&
            board.history[board.history_n - 2].move.to == board.history[board.history_n - 6].move.to && board.history[board.history_n - 2].move.from == board.history[board.history_n - 6].move.from &&
            board.history[board.history_n - 3].move.to == board.history[board.history_n - 7].move.to && board.history[board.history_n - 3].move.from == board.history[board.history_n - 7].move.from &&
            board.history[board.history_n - 4].move.to == board.history[board.history_n - 8].move.to && board.history[board.history_n - 4].move.from == board.history[board.history_n - 8].move.from
        ) {
            return -STALEMATE;
        }
    }

    int oalpha = alpha;
    uint64_t hash = board.getHash();
    TTEntry entry = tt[hash & TT_MAX];
    bool match = entry.hash == hash;

    if (match && entry.depth > d) {
        switch (entry.type) {
            case LOWER:
                alpha = std::max(alpha, entry.score);
                break;
            
            case EXACT:
                if (move) {
                    memcpy(move, &(entry.move), sizeof(Move));
                }

                return entry.score;
            
            case UPPER:
                beta = std::min(beta, entry.score);
                break;
            
            default:;
        }

        if (alpha >= beta) {
            if (move) {
                memcpy(move, &(entry.move), sizeof(Move));
            }

            return entry.score;
        }
    }

    if (!d) {
        return quiesce(alpha, beta, ply + 1);
    }

    bool check = board.state.t ? board.isBlackSquareAttacked(__builtin_ctzll(board.pieces[BK])) : board.isWhiteSquareAttacked(__builtin_ctzll(board.pieces[WK]));

    if (d > 2 && !check) {
        board.state.t = !board.state.t;
        int score = -determineBestMove(d - 3, NULL, -beta, 1 - beta, ply + 1);
        board.state.t = !board.state.t;

        if (score >= beta) {
            return score;
        }
    }

    int max = -1000000;
    Move moves[256];
    uint8_t n = board.generatePseudoLegalMoves(moves);
    uint8_t c = 0;
    uint8_t legal = 0;
    int scores[256];

    for (uint8_t i = 0; i < n; i++) {
        scores[i] = scoreMove(moves[i], ply);

        if (moves[i].to == entry.move.to && moves[i].from == entry.move.from) {
            scores[i] += 100000;
        }
    }

    for (uint8_t i = 0; i < n; i++) {
        int best = i;
        int best_score = scores[i];

        for (int j = i + 1; j < n; j++) {
            int score = scores[j];
            
            if (score > best_score) {
                best = j;
                best_score = score;
            }
        }

        std::swap(moves[i], moves[best]);
        std::swap(scores[i], scores[best]);

        bool capture = getBit(board.occupied[AO], moves[i].to) || moves[i].ep;

        if (board.applyMove(moves[i])) {
            legal++;
            uint8_t r = 1;
            int lb = -beta;
            bool ncheck = board.state.t ? board.isBlackSquareAttacked(__builtin_ctzll(board.pieces[BK])) : board.isWhiteSquareAttacked(__builtin_ctzll(board.pieces[WK]));

            if (i > 10 && d > 2 && !check && !capture && !ncheck) {
                r++;
                lb = -1 - alpha;
            }

            int score = -determineBestMove(d - r, NULL, lb, -alpha, ply + 1);

            if (r > 1 && score > alpha) {
                score = -determineBestMove(d - 1, NULL, -beta, -alpha, ply + 1);
            }

            if (score == STALEMATE) {
                score = -STALEMATE;
            }

            if (score == -TIME_BREAK) {
                board.undoMove();

                return TIME_BREAK;
            }

            if (score > max) {
                max = score;
                c = i;

                if (score > alpha) {
                    alpha = score;
                }
            }

            if (score >= beta) {
                board.undoMove();
                max = score;
                c = i;

                if (!(getBit(board.occupied[AO], moves[i].to) || moves[i].ep)) {
                    if (killers[ply][0].to != moves[i].to || killers[ply][0].from != moves[i].from) {
                        killers[ply][1] = killers[ply][0];
                        killers[ply][0] = moves[i];
                    }

                    int hist = std::min(d * d, 2048);
                    history[board.state.t][moves[i].to][moves[i].from] = hist - (history[board.state.t][moves[i].to][moves[i].from] * hist / 2048);
                }

                break;
            }
        }

        board.undoMove();
    }

    if (!legal) {
        if (board.state.t == WHITE) {
            if (board.isWhiteSquareAttacked(__builtin_ctzll(board.pieces[WK]))) {
                max = -CHECKMATE + ply;
            } else {
                max = -STALEMATE;
            }
        } else {
            if (board.isBlackSquareAttacked(__builtin_ctzll(board.pieces[BK]))) {
                max = -CHECKMATE + ply;
            } else {
                max = -STALEMATE;
            }
        }
    } else if (d >= entry.depth || !entry.hash) {
        entry.hash = hash;
        entry.move = moves[c];
        entry.depth = d;
        entry.score = max;
        
        if (max <= oalpha) {
            entry.type = UPPER;
        } else if (max >= beta) {
            entry.type = LOWER;
        } else {
            entry.type = EXACT;
        }

        tt[hash & TT_MAX] = entry;
    }

    if (move) {
        memcpy(move, moves + c, sizeof(Move));
    }

    return max;
}

int Engine::quiesce (int alpha, int beta, int ply) {
    bool check = board.state.t ? board.isBlackSquareAttacked(__builtin_ctzll(board.pieces[BK])) : board.isWhiteSquareAttacked(__builtin_ctzll(board.pieces[WK]));
    int cs = evaluatePosition();

    if (!check) {
        if (cs >= beta) {
            n++;

            return beta;
        }
    }

    if (cs > alpha) {
        alpha = cs;
    }

    Move moves[256];
    uint8_t n = board.generatePseudoLegalMoves(moves);
    uint8_t legal = 0;
    int scores[256];

    for (uint8_t i = 0; i < n; i++) {
        scores[i] = scoreMove(moves[i], ply);
    }

    for (uint8_t i = 0; i < n; i++) {
        int best = i;
        int best_score = scores[i];

        for (int j = i + 1; j < n; j++) {
            int score = scores[j];
            
            if (score > best_score) {
                best = j;
                best_score = score;
            }
        }

        std::swap(moves[i], moves[best]);
        std::swap(scores[i], scores[best]);

        
        if (board.applyMove(moves[i])) {
            legal++;

            if ((board.state.t ? board.isBlackSquareAttacked(__builtin_ctzll(board.pieces[BK])) : board.isWhiteSquareAttacked(__builtin_ctzll(board.pieces[WK]))) || check || getBit(board.occupied[AO], moves[i].to) || moves[i].ep) {
                int score = -quiesce(-beta, -alpha, ply + 1);

                if (score >= beta) {
                    board.undoMove();

                    return beta;
                }

                if (score > alpha) {
                    alpha = score;
                }
            }
        }

        board.undoMove();
    }

    if (!legal) {
        if (check) {
            return -CHECKMATE + ply;
        } else {
            return -STALEMATE;
        }
    }

    return alpha;
}

int Engine::generateMove (int time, uint8_t min_depth, uint8_t max_depth, Move *move, int *depth) {
    ts = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    tl = time;
    md = false;
    n = 0;
    Move m;
    int best_score;
    Move best_move;

    for (int d = min_depth; d <= max_depth; d++) {
        uint64_t cts = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        int output = determineBestMove(d, &m, -1000000, 1000000, 0);

        if (output == -CHECKMATE || output == CHECKMATE || output == STALEMATE || output == -STALEMATE) {
            return output;
        }

        if (output == TIME_BREAK) {
            d--;
            memcpy(move, &best_move, sizeof(Move));
            memcpy(depth, &d, sizeof(int));

            return best_score;
        } else {
            best_score = output;
            best_move = m;
        }

        if (d == max_depth || (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - cts) > (time / 2)) {
            memcpy(move, &best_move, sizeof(Move));
            memcpy(depth, &d, sizeof(int));

            return best_score;
        }

        if (d == min_depth) {
            md = true;
        }
    }

    return 0;   // This should never be reached
}

int Engine::scoreMove (Move &move, int ply) {
    int score = 0;

    if (move.to == killers[ply][0].to && move.from == killers[ply][0].from) {
        score += 200;
    }

    if (move.to == killers[ply][1].to && move.from == killers[ply][1].from) {
        score += 100;
    }

    score += history[board.state.t][move.to][move.from];

    if (getBit(board.occupied[AO], move.to)) {
        uint8_t taker = move.piece > WK ? move.piece - 6 : move.piece;

        switch (taker) {
            case WP:
                score -= 100;
                break;

            case WN:
                score -= 290;
                break;
            
            case WB:
                score -= 310;
                break;
            
            case WR:
                score -= 500;
                break;

            case WQ:
                score -= 900;
                break;
            
            case WK:
                score -= 2000;
            
            default:;
        }

        uint8_t taken;

        for (taken = WP; taken < BK; taken++) {
            if (getBit(board.pieces[taken], move.to)) {
                break;
            }
        }

        taken = taken > WK ? taken - 6 : taken;

        switch (taken) {
            case WP:
                score += 1000;
                break;

            case WN:
                score += 2900;
                break;
            
            case WB:
                score += 3100;
                break;
            
            case WR:
                score += 5000;
                break;

            case WQ:
                score += 9000;
                break;
        }
    }

    return score;
}

Engine::Engine (void) {
    for (uint8_t i = 0; i < 64; i++) {
        pp_mask[WHITE][i] = getFileMask(i % 8) & (UINT64_MAX << i);
        pp_mask[BLACK][i] = getFileMask(i % 8) & (~(UINT64_MAX << i));
        dp_mask[i] = getFileMask(i % 8);
        kp_mask[i] = getFileMask(i % 8);

        if (i % 8) {
            pp_mask[WHITE][i] |= getFileMask((i - 1) % 8) & (UINT64_MAX << (i - 1));
            pp_mask[BLACK][i] |= getFileMask((i - 1) % 8) & (~(UINT64_MAX << (i - 1)));
            ip_mask[i] = getFileMask((i - 1) % 8);
            kp_mask[i] |= getFileMask((i - 1) % 8);
        }

        if (i % 8 != 7) {
            pp_mask[WHITE][i] |= getFileMask((i + 1) % 8) & (UINT64_MAX << (i + 1));
            pp_mask[BLACK][i] |= getFileMask((i + 1) % 8) & (~(UINT64_MAX << (i + 1)));
            ip_mask[i] |= getFileMask((i + 1) % 8);
            kp_mask[i] |= getFileMask((i + 1) % 8);
        }
    }

    tt = new TTEntry[TT_MAX + 1]();
}