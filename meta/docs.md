# Documentation

## Corax CLI Documentation

| Command | Description |
| - | - |
| `uci` | Switch to [UCI](#uci-cli-documentation) |
| `quit` | Exit the CLI |
| `new` | Reset the engine |
| `opt <option> [value]` | Set or retrieve an option. See [CLI Options](#corax-cli-options) |
| `move <move>...` | Apply UCI-format moves |
| `back [number]` | Undo a specified number of moves. Defaults to 1 |
| `gen [apply]` | Determine the optimal move in the current position. The "apply" parameter automatically applies the generated move if present |
| `sgen [apply]` | Equivalent to `gen` but only outputs the determined move |
| `agen [apply] <wtime> <btime> <winc> <binc>` | Time control adapting move generation. Overrides other move generation limits |
| `perft <depth>` | Determine `perft(depth)` from the current position |
| `eval` | Returns a static evaluation of the current position from the perspective of the side to move |
| `piece <piece>` | Retrieve a piece bitboard. Piece codes take the form `WP`, `BN`, etc. |
| `hash` | Calculate the Zobrist hash of the current position |
| `plist` | Lists pseudo-legal moves in UCI format |
| `list` | Lists legal moves in UCI format |
| `state` | Retrieve a board state value. See [Board State](#board-state) |
| `pos [FEN]` | Set the current position if FEN is provided, otherwise retrieve the current position in FEN |
| `help` | Returns the command list |
| `time <command>` | Time a command |
| `disp` | Display board |

## Corax CLI Options

| Option | Description |
| - | - |
| `time` | Move generation time limit in milliseconds |
| `min_depth` | Minimum search depth (plies) in move generation. Overrides the time limit |
| `max_depth` | Maximum search depth (plies) in move generation |
| `tt` | Transposition table size (exponent of two) |

## Board State

| Value | Description |
| - | - |
| `(w\|b)km` | Has the (white\|black) king moved? |
| `(w\|b)ksrm` | Has the (white\|black) king-side rook moved? |
| `(w\|b)qsrm` | Has the (white\|black) queen-side rook moved? |
| `(w\|b)c` | (white\|black) castle state. 0 for no castle, 1 for king-side castle, and 2 for queen-side castle |
| `t` | Player to move. 0 for white and 1 for black |
| `es` | Square of pawn that can be taken via en-passant. 0 if en-passant is unavailable |

## UCI CLI Documentation

| Command | Description |
|-|-|
| `corax` | Switch to Corax CLI |
| `quit` | Exit the CLI |
| `isready` | Await ready state |
| `position <fen [FEN] \| startpos> [moves <move>...]` | Set position |
| `go [wtime <wtime>] [btime <btime>] [winc <winc>] [binc <binc>] [depth <depth>] [movetime <movetime>] [perft <depth>]` | Generate a move |
| `time <command>` | Time a command |
| `disp` | Display board |