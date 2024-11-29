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

#define umap unordered_map
#define uset unordered_set
#define error(...) printf("Error: "); printf(__VA_ARGS__); cout << endl; exit(1);

// Map from name to player id
umap<string, int> names;

int Atoi(string alph) {
	// Read into a vector of numbers
	vector<int> v;
	for (int i = 0; i < alph.size(); i++) {
		char c = alph[i];
		int x;
		if ('A' <= c && c <= 'Z') {
			x = c - 'A';
		} else if ('a' <= c && c <= 'z') {
			x = c - 'a';
		} else {
			error("atoi: failed to convert '%s'", alph.c_str());
		}
		v.push_back(x);
	}

	// Convert into a value
	int n = 0;
	for (int i = v.size() - 1; i >= 0; i--) {
		n = n * 26 + v[i];
	}
	return n;
}

coord atoc(string s) {
	coord c;
	string alph = "";
	int i;
	for (i = 0; i < s.size(); i++) {
		// Switch over to the number portion
		if ('0' <= s[i] && s[i] <= '9') { break; }
		alph.push_back(s[i]);
	}
	// Alphanumeric coordinates are 1-indexed, but we use 0-indexed arrays
	c.r = stoi(s.substr(i)) - 1;
	c.c = Atoi(alph);
	return c;
}

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

Game *parse_gamefile(istream &in) {
	int deer_turns_to_win;
	coord size;
	coord deer;
	vector<coord> wolves;
	vector<string> player_names(1, ""); // Placeholder for deer name
	uset<coord> deer_moves;
	uset<coord> wolf_moves;

	// ((id, coord), line num)
	// Line num passed for error handling
	vector<pair<pair<int, coord>, int>> move_list;

	// Parse the file
	string cmd;
	for (int i = 1; getline(in, cmd); i++) {
		stringstream toks(cmd);
		string t;
		toks >> t;

		// Parse valid commands
		if (t == "#" || t == "//" || t == "") {
			// Ignore comments or blank lines
		} else if (t == "board") {
			toks >> size.r >> size.c;
		} else if (t == "set") {
			toks >> t;
			if (t == "deer_turns_to_win") {
				toks >> deer_turns_to_win;
			} else if (t == "deer_moves") {
				parse_moves(in, toks, deer_moves, i);
			} else if (t == "wolf_moves") {
				parse_moves(in, toks, wolf_moves, i);
			} else {
				error("line %d: No setting named %s", i, t.c_str());
			}
		} else if (t == "create") {
			toks >> t;
			if (t == "deer") {
				static bool deer_created = false;
				if (deer_created) {
					error("line %d: Only one deer is allowed", i);
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
				error("line %d: Found '%s'. Valid entities to create are ['deer', 'wolf']", i, t.c_str());
			}
		} else if (t == "move") {
			toks >> t;
			// Failed to find a player with that name
			if (names.find(t) == names.end()) {
				error("line %d: No player named %s", i, t.c_str());
			}
			int id = names[t];
			toks >> t;
			coord c = atoc(t);
			move_list.emplace_back(make_pair(id, c), i);
		} else {
			error("line %d: No command named %s", i, t.c_str());
			exit(1);
		}
	}

	// Create the game and make moves
	Game *g = new Game(deer_turns_to_win, size, deer, wolves, player_names, deer_moves, wolf_moves);
	for (auto pp : move_list) {
		auto p = pp.first;
		int id = p.first;
		coord c = p.second;
		int line = pp.second;
		if (!g->move(id, c)) {
			error("line %d: Invalid move", line);
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

	g->print_deer_cover();
	g->print_wolf_cover();
	g->print_combined_cover();
	delete g;
}
