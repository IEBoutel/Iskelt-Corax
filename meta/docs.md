# Documentation

## CLI Documentation

| Command | Description |
| - | - |
| `quit` | Exit the CLI |
| `new` | Reset the engine |
| `opt <option> [value]` | Set or retrieve an option. See [CLI Options](#cli-options) |
| `move <move>...` | Apply UCI-format moves |
| `back [number]` | Undo a specified number of moves. Defaults to 1 |
| `gen [apply]` | Determine the optimal move in the current position. The "apply" parameter automatically applies the generated move if present |
| `sgen [apply]` | Equivalent to `gen` but only outputs the determined move |
| `perft <depth>` | Determine `perft(depth)` from the current position |
| `eval` | Returns a static evaluation of the current position from the perspective of the side to move |
| `piece <piece>` | Retrieve a piece bitboard. Piece codes take the form `WP`, `BN`, etc. |
| `hash` | Calculate the Zobrist hash of the current position |
| `plist` | Lists pseudo-legal moves in UCI format |
| `list` | Lists legal moves in UCI format |
| `state` | Retrieve a board state value. See [Board State](#board-state) |
| `help` | Returns the command list |

## CLI Options

| Option | Description |
| - | - |
| time | Move generation time limit in milliseconds |
| min_depth | Minimum search depth (plies) in move generation. Overrides the time limit |
| max_depth | Maximum search depth (plies) in move generation |

## Board State

| Value | Description |
| - | - |
| `(w\|b)km` | Has the (white\|black) king moved? |
| `(w\|b)ksrm` | Has the (white\|black) king-side rook moved? |
| `(w\|b)qsrm` | Has the (white\|black) queen-side rook moved? |
| `(w\|b)c` | (white\|black) castle state. 0 for no castle, 1 for king-side castle, and 2 for queen-side castle |
| `t` | Player to move. 0 for white and 1 for black |
| `es` | Square of pawn that can be taken via en-passant. 0 if en-passant is unavailable |