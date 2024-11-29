# Wolf Game

A basic simulator for Wolf Game.

## Running

### Manual

To run manually, the following dependencies are needed: `make`, `g++`. Run with:

```bash
make && ./out <gamefile>
```

### OnlineGDB

If you do not have access to a Linux environment, it is possible to use
OnlineGDB to run this program.

1. Go to https://www.onlinegdb.com/#
2. Upload the following files (cloud icon in top left):
  * `run.cpp`
  * `game.cpp`
  * `game.h`
  * `coord.h`
  * `gamefile.txt`
3. Replace the contents of `main.cpp` with the contents of `run.cpp`, and delete
   `run.cpp`.
4. In "Command line arguments" at the bottom, enter "./gamefile.txt"
5. Click Run at the top.

## Usage

This program parses a gamefile to create a simulation of a specific game. A
gamefile consists of lines of commands. The following commands are available:

### set

| Subcommand | Parameters | Explanation | Example |
| :--------: | :--------: | :---------- | :-----: |
| `dimensions` | rows, cols | Takes two integers as the height and width of the board. | `set dimensions 13 13` |
| `deer_turns_to_win` | turns remaining | Takes one integers as the duration of the game -- How long the deer must survive for before winning. | `set deer_turns_to_win 14` |
| `deer_moves`, `wolf_moves` | x, y, array | Takes two integers origin of the movement pattern, and an array to specify valid positions. The example shows that a wolf is allowed to move to any of the 8 squares of the same color either diagonally or orthogonally adjacent to it, or stay in the same place on its turn. | set wolf_moves 2 2 [<br>..X..<br>.X.X.<br>X.X.X<br>.X.X.<br>..X..<br>] |

### create

| Subcommand | Parameters | Explanation | Example |
| :--------: | :--------: | :---------- | :-----: |
| `deer` | name | Takes a string as the name of the deer. | `create deer Alfa` |
| `wolf` | name | Takes a string as the name of the next wolf. | `create wolf Bravo` |

### game

| Subcommand | Parameters | Explanation | Example |
| :--------: | :--------: | :---------- | :-----: |
| `start` | none | Creates an instance of the game. This invalidates all commands listed above the game section, and activates all commands listed below the game section. All `set` subcommands must be called before calling `game start`, but there is currently no check in place for this and the program will instead manifest odd errors. | `game start` |

### move

Take a name that was previously specified in a `create` command, and an alphanumeric coordinate to move to (e.g. `AB12`), where the letters specify a column and the numbers specify a row. 

