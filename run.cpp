#include <cstdlib>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
using namespace std;

#include "game.h"
#include "utils.h"

#define umap unordered_map
#define uset unordered_set

// Map from name to player id
umap<string, int> names;

// Helper function to parse the moves arrays. The valid moves of a player type
// are passed in as an array, and thus must be handled specially since the main
// parsing function breaks up each line.
// NOTE: Should consider refactoring this into a `get_array()` function and
// keeping most of the logic in the main parsing function.
void parse_moves(istream &in, istream &toks, uset<coord> &moves, int &line_num) {
	// Read origin row and col
	int o_r, o_c;
	string t;
	toks >> o_r >> o_c >> t;
	if (t != "[") {
		// If the player is at the X in the middle, then '.' is not a valid move, 'X' is a valid move.
		error("line %d: Set moves should be a coordinate pair followed by an array. For example:\n"
				"set xxxx_moves 2 2 [\n"
				"..X..\n"
				".X.X.\n"
				"X.X.X\n"
				".X.X.\n"
				"..X..\n"
				"]",
		line_num);
	}

	// Read into memory
	int r = 0, c = 0;
	for (char i; in >> noskipws >> i;) {
		switch (i) {
			case '.': c++; break; // Not a valid move
			case 'X': // Valid move
				moves.insert({r - o_r, c - o_c});
				c++;
				break;
			case '\n': // Next row
				c = 0;
				r++;
				line_num++;
				break;
			case ']':
				getline(in, t);
				line_num++;
				in >> skipws;
				return;
			default:
				error("line %d: Only '.' and 'X' are allowed in moves array", line_num);
		}
	}
	in >> skipws;
}

// Parse the given file, and create and play a game.
Game *parse_gamefile(istream &in) {
	int deer_turns_to_win;
	coord size;
	coord deer;
	vector<coord> wolves;
	vector<string> player_names(1, ""); // Placeholder for deer name
	uset<coord> deer_moves;
	uset<coord> wolf_moves;

	Game *g;

	// Parse the file
	string cmd;
	int line;
	for (line = 1; getline(in, cmd); line++) {
		stringstream toks(cmd);
		string t;
		toks >> t;

		// Parse valid commands
		if (t == "#" || t == "//" || t == "") {
			// Ignore comments or blank lines
		} else if (t == "set") {
			toks >> t;
			if (t == "dimensions") {
				toks >> size.r >> size.c;
			} else if (t == "deer_turns_to_win") {
				toks >> deer_turns_to_win;
			} else if (t == "deer_moves") {
				parse_moves(in, toks, deer_moves, line);
			} else if (t == "wolf_moves") {
				parse_moves(in, toks, wolf_moves, line);
			} else {
				error("line %d: No setting named %s", line, t.c_str());
			}
		} else if (t == "create") {
			toks >> t;
			if (t == "deer") {
				static bool deer_created = false;
				if (deer_created) {
					error("line %d: Only one deer is allowed", line);
				}

				// Add the name
				toks >> t;
				names[t] = 0;
				player_names[0] = t;
				toks >> t;
				deer = atoc(t);
			} else if (t == "wolf") {
				static int wolf_count = 0;
				toks >> t;
				wolf_count++;
				names[t] = wolf_count;
				player_names.push_back(t);
				toks >> t;
				wolves.push_back(atoc(t));
			} else {
				error("line %d: Found '%s'. Valid entities to create are ['deer', 'wolf']", line, t.c_str());
			}
		} else if (t == "game") {
			toks >> t;
			if (t == "start") {
				// Create the game
				g = new Game(deer_turns_to_win, size, deer, wolves, player_names, deer_moves, wolf_moves);
				break;
			} else {
				error("line %d: Invalid game command %s", line, t.c_str());
			}
		} else {
			error("line %d: No command named %s", line, t.c_str());
			exit(1);
		}
	}
	line++;

	for (; getline(in, cmd); line++) {
		stringstream toks(cmd);
		string t;
		toks >> t;

		if (t == "#" || t == "//" || t == "") {
			// Ignore comments or blank lines
		} else if (t == "game") {
			toks >> t;
			if (t == "show") {
				toks >> t;
				if (t == "board" || t == "") {
					g->print_board();
				} else if (t == "deer_cover") {
					g->print_deer_cover();
				} else if (t == "wolf_cover") {
					g->print_wolf_cover();
				} else if (t == "combined_cover") {
					g->print_combined_cover();
				} else {
					error("line %d: Invalid show subcommand %s", line, t.c_str());
				}
			} else {
				error("line %d: Invalid game command %s", line, t.c_str());
			}
		} else if (t == "move") {
			string name;
			toks >> name >> t;
			// Failed to find a player with that name
			if (names.find(name) == names.end()) {
				error("line %d: No player named %s", line, name.c_str());
			}
			int id = names[name];
			coord c = atoc(t);
			if (!g->move(id, c)) {
				coord o = g->get_player(id);
				// Cannot call .c_str() on function return
				string o_col = itoc(o.c);
				string col = itoc(c.c);
				error("line %d: Invalid move %s from %s%d to %s%d", line, name.c_str(), o_col.c_str(), o.r+1, col.c_str(), c.r+1);
			}
		} else {
			error("line %d: No command named %s", line, t.c_str());
			exit(1);
		}
	}

	return g;
}

int main(int argc, char **argv) {
	Game *g;
	if (argc > 1) {
		ifstream f;
		f.open(argv[argc - 1]);
		g = parse_gamefile(f);
	} else {
		g = parse_gamefile(cin);
	}
	delete g;
}
