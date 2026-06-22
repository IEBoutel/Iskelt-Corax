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

#include <iostream>
#include <sstream>

#include "cli.hpp"

std::string LICENSE_STRING = "Iskelt Corax  Copyright (C) 2026  Iskander Edward Boutel\n";
std::string USAGE_STRING = "\nCorax:\n  uci\n  quit\n  new\n  opt <option> [value]\n  move <move>...\n  back [number]\n  gen [apply]\n  sgen [apply]\n  agen [apply] <wtime> <btime> <winc> <binc>\n  perft <depth>\n  eval\n  piece <piece>\n  hash\n  plist\n  list\n  state\n  pos [FEN]\n  help\n\nUCI:\n  corax\n  quit\n  isready\n  position <fen [FEN] | startpos> [moves <move>...]\n  go wtime <wtime> btime <btime> winc <winc> binc <binc>\n";

CLI::CLI (void) {
    engine = new Engine;
}

void CLI::commandQuit (void) {
    exit(0);
}

void CLI::commandNew (void) {
    delete engine;
    engine = new Engine;
}

std::string CLI::commandOpt (std::string option, std::string value) {
    if (option == "time") {
        time = std::stoi(value);

        return "OK";
    }
    
    if (option == "min_depth") {
        min_depth = (uint8_t) std::stoul(value);

        return "OK";
    }
    
    if (option == "max_depth") {
        max_depth = (uint8_t) std::stoul(value);

        return "OK";
    }

    if (option == "tt") {
        tt = std::stoi(value);
        engine->setTTSize(tt);

        return "OK";
    }

    return "BAD OPT";
}

std::string CLI::commandMove (std::vector<std::string> moves) {
    for (auto &move : moves) {
        if (!engine->board.applyMove(move)) {
            engine->board.undoMove();

            return std::string("BAD MOVE: ") + move;
        }
    }

    return "OK";
}

std::string CLI::commandGen (std::string apply) {
    Move move;
    int depth;
    int score = engine->generateMove(time, min_depth, max_depth, &move, &depth);

    if (apply == "apply") {
        engine->board.applyMove(move);
    }

    return std::string("Move: ") + engine->board.moveToString(move) + ", Score: " + std::to_string(score) + ", Final Depth: " + std::to_string(depth) + ", Leaves: " + std::to_string(engine->n);
}

std::string CLI::commandSGen (std::string apply) {
    Move move;
    int depth;
    int score = engine->generateMove(time, min_depth, max_depth, &move, &depth);

    if (apply == "apply") {
        engine->board.applyMove(move);
    }

    return engine->board.moveToString(move);
}

std::string CLI::commandAGen (std::string apply, std::string wtime, std::string btime, std::string winc, std::string binc) {
    Move move;
    int depth;
    int score = engine->generateMove(std::stoi(wtime), std::stoi(btime), std::stoi(winc), std::stoi(binc), &move, &depth);

    if (apply == "apply") {
        engine->board.applyMove(move);
    }

    return std::string("Move: ") + engine->board.moveToString(move) + ", Score: " + std::to_string(score) + ", Final Depth: " + std::to_string(depth) + ", Leaves: " + std::to_string(engine->n);
}

std::string CLI::commandBack (std::string n) {
    for (int i = 0; i < std::stoi(n); i++) {
        if (!engine->board.history_n) {
            return std::string("BAD N: ") + std::to_string(i + 1);
        }

        engine->board.undoMove();
    }

    return "OK";
}

std::string CLI::commandPerft (std::string d) {
    return std::to_string(engine->board.perft(std::stoi(d)));
}

std::string CLI::commandEvaluate (void) {
    return std::to_string(engine->evaluatePosition()) + (engine->board.state.t == WHITE ? "W" : "B");
}

std::string CLI::commandHash (void) {
    return std::to_string(engine->board.getHash());
}

std::string CLI::commandPiece (std::string piece) {
    if (piece == "WP") {
        return std::to_string(engine->board.pieces[WP]);
    } else if (piece == "WN") {
        return std::to_string(engine->board.pieces[WN]);
    } else if (piece == "WB") {
        return std::to_string(engine->board.pieces[WB]);
    } else if (piece == "WR") {
        return std::to_string(engine->board.pieces[WR]);
    } else if (piece == "WQ") {
        return std::to_string(engine->board.pieces[WQ]);
    } else if (piece == "WK") {
        return std::to_string(engine->board.pieces[WK]);
    } else if (piece == "BP") {
        return std::to_string(engine->board.pieces[BP]);
    } else if (piece == "BN") {
        return std::to_string(engine->board.pieces[BN]);
    } else if (piece == "BB") {
        return std::to_string(engine->board.pieces[BB]);
    } else if (piece == "BR") {
        return std::to_string(engine->board.pieces[BR]);
    } else if (piece == "BQ") {
        return std::to_string(engine->board.pieces[BQ]);
    } else if (piece == "BK") {
        return std::to_string(engine->board.pieces[BK]);
    } else {
        return "BAD PIECE";
    }
}

std::string CLI::commandPList (void) {
    std::string output;
    Move moves[256];
    uint8_t n = engine->board.generatePseudoLegalMoves(moves);

    for (uint8_t i = 0; i < n; i++) {
        output += engine->board.moveToString(moves[i]) + " ";
    }

    return output;
}

std::string CLI::commandList (void) {
    std::string output;
    Move moves[256];
    uint8_t n = engine->board.generatePseudoLegalMoves(moves);

    for (uint8_t i = 0; i < n; i++) {
        if (engine->board.applyMove(moves[i])) {
            output += engine->board.moveToString(moves[i]) + " ";
        }

        engine->board.undoMove();
    }

    return output;
}

std::string CLI::commandState (std::string v) {
    if (v == "wkm") {
        return std::to_string(engine->board.state.wkm);
    } else if (v == "wksrm") {
        return std::to_string(engine->board.state.wksrm);
    } else if (v == "wqsrm") {
        return std::to_string(engine->board.state.wqsrm);
    } else if (v == "wc") {
        return std::to_string(engine->board.state.wc);
    } else if (v == "bkm") {
        return std::to_string(engine->board.state.bkm);
    } else if (v == "bksrm") {
        return std::to_string(engine->board.state.bksrm);
    } else if (v == "bqsrm") {
        return std::to_string(engine->board.state.bqsrm);
    } else if (v == "bc") {
        return std::to_string(engine->board.state.bc);
    } else if (v == "t") {
        return std::to_string(engine->board.state.t);
    } else if (v == "es") {
        return std::to_string(engine->board.state.es);
    } else {
        return "BAD VALUE";
    }
}

std::string CLI::commandOpt (std::string option) {
    if (option == "time") {
        return std::to_string(time);
    }
    
    if (option == "min_depth") {
        return std::to_string(min_depth);
    }
    
    if (option == "max_depth") {
        return std::to_string(max_depth);
    }

    if (option == "tt") {
        return std::to_string(tt);
    }

    return "BAD OPT";
}

std::string CLI::commandHelp (void) {
    return USAGE_STRING;
}

std::string CLI::commandPos (void) {
    std::string output;
    uint8_t n = 0;

    for (uint8_t r = 7; r < 255; r--) {
        for (uint8_t f = 0; f < 8; f++) {
            uint8_t piece = engine->board.getPieceAt(r * 8 + f);

            if (piece != 255 && n) {
                output += std::to_string(n);
                n = 0;
            }

            switch (piece) {
                case WP:
                    output += "P";
                    break;

                case WN:
                    output += "N";
                    break;

                case WB:
                    output += "B";
                    break;

                case WR:
                    output += "R";
                    break;

                case WQ:
                    output += "Q";
                    break;

                case WK:
                    output += "K";
                    break;

                case BP:
                    output += "p";
                    break;

                case BN:
                    output += "n";
                    break;

                case BB:
                    output += "b";
                    break;

                case BR:
                    output += "r";
                    break;

                case BQ:
                    output += "q";
                    break;

                case BK:
                    output += "k";
                    break;
                
                case 255:
                    n++;
                    break;
            }
        }

        if (n) {
            output += std::to_string(n);
            n = 0;
        }

        if (r) {
            output += "/";
        }
    }

    output += std::string(" ") + (engine->board.state.t ? "b" : "w") + std::string(" ");
    
    bool wks = !engine->board.state.wc && !engine->board.state.wkm && !engine->board.state.wksrm;
    bool wqs = !engine->board.state.wc && !engine->board.state.wkm && !engine->board.state.wqsrm;
    bool bks = !engine->board.state.bc && !engine->board.state.bkm && !engine->board.state.bksrm;
    bool bqs = !engine->board.state.bc && !engine->board.state.bkm && !engine->board.state.bqsrm;

    if (wks) {
        output += "K";
    }

    if (wqs) {
        output += "Q";
    }

    if (bks) {
        output += "k";
    }

    if (bqs) {
        output += "q";
    }

    if (!wks && !wqs && !bks && !bqs) {
        output += "-";
    }

    output += " ";

    if (engine->board.state.es) {
        output += 'a' + ((engine->board.state.es + (engine->board.state.t ? -8 : 8)) % 8);
        output += '1' + ((engine->board.state.es + (engine->board.state.t ? -8 : 8)) / 8);
    } else {
        output += "-";
    }

    output += " " + std::to_string(engine->board.state.hmc) + " " + std::to_string(1 + (engine->board.history_n + engine->board.bmc) / 2);

    return output;
}

std::string CLI::commandPos (std::string layout, std::string turn, std::string castling, std::string enpassant, std::string half_moves, std::string full_moves) {
    memset(engine->board.pieces, 0, sizeof(engine->board.pieces));
    
    uint8_t r = 7;
    uint8_t f = 0;

    for (char &c : layout) {
        if (c >= '1' && c <= '8') {
            f += c - '0';
            continue;
        }

        switch (c) {
            case 'P':
                setBit(engine->board.pieces[WP], r * 8 + f);
                break;
            
            case 'N':
                setBit(engine->board.pieces[WN], r * 8 + f);
                break;

            case 'B':
                setBit(engine->board.pieces[WB], r * 8 + f);
                break;

            case 'R':
                setBit(engine->board.pieces[WR], r * 8 + f);
                break;

            case 'Q':
                setBit(engine->board.pieces[WQ], r * 8 + f);
                break;

            case 'K':
                setBit(engine->board.pieces[WK], r * 8 + f);
                break;
            
            case 'p':
                setBit(engine->board.pieces[BP], r * 8 + f);
                break;
            
            case 'n':
                setBit(engine->board.pieces[BN], r * 8 + f);
                break;

            case 'b':
                setBit(engine->board.pieces[BB], r * 8 + f);
                break;

            case 'r':
                setBit(engine->board.pieces[BR], r * 8 + f);
                break;

            case 'q':
                setBit(engine->board.pieces[BQ], r * 8 + f);
                break;

            case 'k':
                setBit(engine->board.pieces[BK], r * 8 + f);
                break;
            
            case '/':
                r--;
                f = 255;
                break;
        }

        f++;
    }

    engine->board.occupied[WO] = engine->board.pieces[WP] | engine->board.pieces[WN] | engine->board.pieces[WB] | engine->board.pieces[WR] | engine->board.pieces[WQ] | engine->board.pieces[WK];
    engine->board.occupied[BO] = engine->board.pieces[BP] | engine->board.pieces[BN] | engine->board.pieces[BB] | engine->board.pieces[BR] | engine->board.pieces[BQ] | engine->board.pieces[BK];
    engine->board.occupied[AO] = engine->board.occupied[WO] | engine->board.occupied[BO];
    engine->board.state.t = turn == "b";

    if (castling != "-") {
        for (char &c : castling) {
            switch (c) {
                case 'K':
                    engine->board.state.wc = 0;
                    engine->board.state.wkm = 0;
                    engine->board.state.wksrm = 0;
                    break;
                
                case 'Q':
                    engine->board.state.wc = 0;
                    engine->board.state.wkm = 0;
                    engine->board.state.wqsrm = 0;
                    break;

                case 'k':
                    engine->board.state.bc = 0;
                    engine->board.state.bkm = 0;
                    engine->board.state.bksrm = 0;
                    break;

                case 'q':
                    engine->board.state.bc = 0;
                    engine->board.state.bkm = 0;
                    engine->board.state.bqsrm = 0;
                    break;
            }
        }
    }

    if (enpassant != "-") {
        engine->board.state.es = ((enpassant[1] - '1') * 8) + (enpassant[0] - 'a') + (engine->board.state.t ? 8 : -8);
    } else {
        engine->board.state.es = 0;
    }

    engine->board.state.hmc = std::stoi(half_moves);
    engine->board.bmc = (std::stoi(full_moves) - 1) * 2 + engine->board.state.t;
    engine->board.history_n = 0;

    return "OK";
}

std::string CLI::uciGo (std::string wtime, std::string btime, std::string winc, std::string binc) {
    Move move;
    int depth;
    int score = engine->generateMove(std::stoi(wtime), std::stoi(btime), std::stoi(winc), std::stoi(binc), &move, &depth);

    return "bestmove " + engine->board.moveToString(move);
}

void CLI::launch (void) {
    std::cout << LICENSE_STRING << std::endl;

    std::string cmd;

    while (1) {
        std::cout << "> ";
        std::getline(std::cin, cmd);

        if (cmd.empty()) {
            continue;
        }

        std::istringstream iss(cmd);
        std::vector<std::string> words;
        std::string word;

        while (iss >> word) {
            words.push_back(word);
        }

        if (uci) {
            if (words[0] == "corax") {
                uci = false;
                std::cout << "OK" << std::endl;
            } else if (words[0] == "isready") {
                std::cout << "readyok" << std::endl;
            } else if (words[0] == "position") {
                if (words[1] == "startpos") {
                    commandPos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", "w", "KQkq", "-", "0", "1");

                    if (words[2] == "moves") {
                        std::cout << commandMove(std::vector(words.begin() + 3, words.end())) << std::endl;
                    }
                } else if (words[1] == "fen") {
                    commandPos(words[2], words[3], words[4], words[5], words[6], words[7]);

                    if (words[8] == "moves") {
                        std::cout << commandMove(std::vector(words.begin() + 9, words.end())) << std::endl;
                    }
                }
            } else if (words[0] == "go") {
                std::cout << uciGo(words[2], words[4], words[6], words[8]) << std::endl;
            } else if (words[0] == "quit") {
                commandQuit();
            } else if (words[0] == "ucinewgame") {
                commandNew();
            } else if (words[0] == "setoption") {
                if (words.size() != 5) {
                    std::cout << "BAD OPT" << std::endl;
                    continue;
                }

                commandOpt(words[2], words[4]);
            } else {
                std::cout << "BAD CMD" << std::endl;
            }
        } else {
            if (words[0] == "quit") {
                commandQuit();
            } else if (words[0] == "new") {
                commandNew();
                std::cout << "OK" << std::endl;
            } else if (words[0] == "opt") {
                if (words.size() > 2) {
                    std::cout << commandOpt(words[1], words[2]) << std::endl;
                } else if (words.size() == 2) {
                    std::cout << commandOpt(words[1]) << std::endl;
                } else {
                    std::cout << "BAD OPT" << std::endl;
                }
            } else if (words[0] == "move") {
                if (words.size() == 1) {
                    std::cout << "BAD OPT" << std::endl;
                    continue;
                }

                std::cout << commandMove(std::vector(words.begin() + 1, words.end())) << std::endl;
            } else if (words[0] == "gen") {
                if (words.size() == 2) {
                    std::cout << commandGen(words[1]) << std::endl;
                } else {
                    std::cout << commandGen("") << std::endl;
                }
            } else if (words[0] == "sgen") {
                if (words.size() == 2) {
                    std::cout << commandSGen(words[1]) << std::endl;
                } else {
                    std::cout << commandSGen("") << std::endl;
                }
            } else if (words[0] == "agen") {
                if (words.size() == 6) {
                    std::cout << commandAGen(words[1], words[2], words[3], words[4], words[5]) << std::endl;
                } else if (words.size() == 5) {
                    std::cout << commandAGen("", words[1], words[2], words[3], words[4]) << std::endl;
                } else {
                    std::cout << "BAD OPT" << std::endl;
                }
            } else if (words[0] == "perft") {
                if (words.size() == 1) {
                    std::cout << "BAD OPT" << std::endl;
                    continue;
                }

                std::cout << commandPerft(words[1]) << std::endl;
            } else if (words[0] == "eval") {
                std::cout << commandEvaluate() << std::endl;
            } else if (words[0] == "back") {
                if (words.size() == 2) {
                    std::cout << commandBack(words[1]) << std::endl;
                } else {
                    std::cout << commandBack("1") << std::endl;
                }
            } else if (words[0] == "piece") {
                if (words.size() == 1) {
                    std::cout << "BAD OPT" << std::endl;
                    continue;
                }

                std::cout << commandPiece(words[1]) << std::endl;
            } else if (words[0] == "plist") {
                std::cout << commandPList() << std::endl;
            } else if (words[0] == "list") {
                std::cout << commandList() << std::endl;
            } else if (words[0] == "state") {
                if (words.size() == 1) {
                    std::cout << "BAD OPT" << std::endl;
                    continue;
                }

                std::cout << commandState(words[1]) << std::endl;
            } else if (words[0] == "hash") {
                std::cout << commandHash() << std::endl;
            } else if (words[0] == "help") {
                std::cout << commandHelp() << std::endl;
            } else if (words[0] == "pos") {
                if (words.size() == 1) {
                    std::cout << commandPos() << std::endl;
                } else if (words.size() == 7) {
                    std::cout << commandPos(words[1], words[2], words[3], words[4], words[5], words[6]) << std::endl;
                } else {
                    std::cout << "BAD OPT" << std::endl;
                }
            } else if (words[0] == "uci") {
                uci = true;
                std::cout << "id name Iskelt Corax\nid author Iskander Edward Boutel\nuciok" << std::endl;
            } else {
                std::cout << "BAD CMD" << std::endl;
            }
        }
    }
}