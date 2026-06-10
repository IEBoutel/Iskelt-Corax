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

#ifndef __CLI_HPP__
#define __CLI_HPP__

#include "engine.hpp"
#include <vector>

class CLI {
    public:
        Engine engine;

        int time = 5000;
        uint8_t min_depth = 5;
        uint8_t max_depth = 255;

        void commandQuit (void);
        void commandNew (void);
        std::string commandOpt (std::string option, std::string value);
        std::string commandOpt (std::string option);
        std::string commandMove (std::vector<std::string> moves);
        std::string commandBack (std::string n);
        std::string commandGen (std::string apply);
        std::string commandSGen (std::string apply);
        std::string commandPerft (std::string d);
        std::string commandEvaluate (void);
        std::string commandPiece (std::string piece);
        std::string commandHash (void);
        std::string commandPList (void);
        std::string commandList (void);
        std::string commandState (std::string v);
        std::string commandHelp (void);

        void launch (void);
};

#endif