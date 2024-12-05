#include "game.h"
#include <iomanip>
#include <queue>

#include "utils.h"

#define id2d(a) (a)
#define id2w(a) (a - deerc)
#define d2id(a) (a)
#define w2id(a) (a + deerc)

/**
 * Helpers
 */

bool Game::in_bounds(const coord &pos) {
	return 0 <= pos.r && pos.r < board.size()
		&& 0 <= pos.c && pos.c < board[0].size();
}

bool Game::is_wolf(int id) {
	return id >= deerc;
}

int Game::valid_move(int id, const coord &pos) {
	// Check that this is a valid player id
	if (!(0 <= id && id <= playersc)) { return INVALID_ID; }

	// Check that a wolf is not trying to move twice in one turn
	if (is_wolf(id) && moved[id]) { return ALREADY_MOVED; }

	// Check that the position is in the board
	if (!in_bounds(pos)) { return OUT_OF_BOUNDS; }

	// Check that the position is not occupied by someone else
	if (board[pos.r][pos.c] != -1) {
		if (board[pos.r][pos.c] == id) {
			// Occupied by self; no move
			return 0;
		} else {
			// Occupied by someone else
			return POSITION_OCCUPIED;
		}
	}

	// If the position is covered by a wolf, it is invalid for a deer
	if (id < deerc && wolf_mask[pos.r][pos.c]) { return DEER_IN_CHECK; }

	// Check that the player is allowed to move there
	uset<coord> *moves;
	moves = (id < deerc) ? &deer_moves : &wolf_moves;
	if (!in_set(*moves, pos - players[id])) { return INVALID_MOVE; }

	return SUCCESS;
}

// Print methods. Called for each position in the board.
// The board is grid of integers, so each method must convert an int into a
// string of length 3.
// These are member methods so they have access to player names.

string Game::int2player(int i) {
	switch (i) {
		case -1:
			return "   ";
		default:
			return "???";
	}
}

string Game::int2deer_reach(int i) {
	switch (i) {
		case -1: return "   ";
		case 0: return " O ";
		case 1: return " . ";
		case 2: return "(-)";
		case -2: return "<X>"; // Covered by wolf
		default: return "???";
	}
}

string Game::int2wolf_reach(int i) {
	return i ? "<X>" : "   ";
}

/**
 * Constructor and Destructor
 */

Game::Game(int turns_remaining, const coord &size, const coord &deer,
		   const vector<coord> &wolves, const vector<string> &player_names,
		   const uset<coord> &deer_moves, const uset<coord> &wolf_moves)
	: turns_remaining(turns_remaining), board(size.r, vector<int>(size.c, -1)),
	  wolf_mask(size.r, vector<int>(size.c, 0)), deer_moves(deer_moves),
	  wolf_moves(wolf_moves) {
	// Construct players array
	deerc = 1;
	wolvesc = wolves.size();
	playersc = deerc + wolvesc;
	// Allocate array
	this->players = new coord[deerc + wolvesc];
	this->deer = this->players;
	this->wolves = this->players + deerc;
	this->moved = new bool[deerc + wolvesc];

	// Copy names
	this->player_names = new string[deerc + wolvesc];
	for (int i = 0; i < player_names.size(); i++) {
		this->player_names[i] = player_names[i];
	}

	// Set position of deer
	this->players[0] = deer;
	if (!in_bounds(deer)) {
		cout << deer << " is not in bounds" << endl;
		throw DeerOutOfBounds;
	}
	board[deer.r][deer.c] = 0;

	// Set position of wolves
	for (int i = 0; i < wolves.size(); i++) {
		this->wolves[i] = wolves[i];
		if (!in_bounds(wolves[i])) {
			cout << wolves[i] << " is not in bounds" << endl;
			throw WolfOutOfBounds;
		}
		board[wolves[i].r][wolves[i].c] = i + 1;

		// Update the coverage information
		for (coord delta : wolf_moves) {
			coord c = wolves[i] + delta;
			if (!in_bounds(c)) { continue; }
			wolf_mask[c.r][c.c]++;
		}

		// Wolves move first
		moved[w2id(i)] = 0;
	}
}

Game::~Game() {
	delete[] players;
	delete[] moved;
	delete[] player_names;
}

// The deer cover is a calculated property because deer move in hard to
// calculate on-the-fly ways; in particular, the double move per turn is tricky.
// Furthermore, there is only a single deer to calculate for.
// NOTE: If multiple deer are added, this assumption may need to be revisited.
vector<vector<int>> Game::get_deer_mask(bool show_wolves) {
	// Use int instead of bool for compatibility with print_board helper
	vector<vector<int>> mask(board.size(), vector<int>(board[0].size(), -1));
	// Starting positions to use when calculating the mask
	queue<coord> locs;
	queue<coord> next;
	// Starting position
	mask[players[0].r][players[0].c] = 0;
	locs.push(players[0]);

	// Perform BFS
	int i = 1;
	while (!locs.empty()) {
		// The queue is assumed to only contain valid coordinates
		coord o = locs.front();
		locs.pop();

		// Explore all valid moves
		for (coord delta : deer_moves) {
			coord c = o + delta;
			if (!in_bounds(c)) { continue; }
			if (wolf_mask[c.r][c.c]) { continue; } // Covered by a wolf
			if (mask[c.r][c.c] != i) {
				// Not already reached in this iteration
				mask[c.r][c.c] = i;
				next.push(c);
			}
		}

		if (locs.empty()) {
			swap(locs, next);
			i++;
		}
		// Only search 2 moves at most
		if (i > 2) { break; }
	}

	if (show_wolves) {
		for (int r = 0; r < mask.size(); r++) {
			for (int c = 0; c < mask.size(); c++) {
				if (wolf_mask[r][c]) {
					// Mark the location of wolves in the mask
					mask[r][c] = -2;
				}
			}
		}
	}

	return mask;
}

/**
 * Getters and Setters
 */

const coord Game::get_player(int id) { return players[id]; }
const coord Game::get_deer() { return deer[0]; }
const coord Game::get_wolf(int id) {
	if (!(0 <= id && id <= playersc)) { error("Invalid ID for a wolf"); }
	return wolves[id];
}

// Given an id and a position, make the move if it is valid.
bool Game::move(int id, const coord &pos) {
	// Check that this is a valid move
	int e;
	if ((e = valid_move(id, pos)) != SUCCESS) {
		switch (e) {
			case INVALID_ID: cout << "Invalid ID" << endl; break;
			case ALREADY_MOVED: cout << player_names[w2id(id)] << " has already moved this turn" << endl; break;
			case OUT_OF_BOUNDS: cout << "Not in bounds" << endl; break;
			case POSITION_OCCUPIED: cout << "Position occupied" << endl; break;
			case DEER_IN_CHECK: cout << "Deer in check" << endl; break;
			case INVALID_MOVE: cout << "Not in move set" << endl; break;
		}
		return false;
	}

	// Move the player
	// Remove the player
	board[players[id].r][players[id].c] = -1;
	// If the player is a wolf, mark it as moved and remove its effect from the mask
	if (id >= deerc) {
		moved[id] = true;
		for (coord delta : wolf_moves) {
			coord c = players[id] + delta;
			if (!in_bounds(c)) { continue; }
			wolf_mask[c.r][c.c]--;
		}
	} else {
		// When the deer moves, reset wolf moves
		for (int i = deerc; i < deerc+wolvesc; i++) { moved[i] = 0; }
	}

	// Place the player back in the new spot
	players[id] = pos;
	board[pos.r][pos.c] = id;

	// If the player is a deer, update the number of turns left
	if (id < deerc) {
		static bool im = false;
		if (im) { turns_remaining--; } // Previous step was an intermediate step
		im = !im;
	} else {
		// Update the new wolf coverage
		for (coord delta : wolf_moves) {
			coord c = players[id] + delta;
			if (!in_bounds(c)) { continue; }
			wolf_mask[c.r][c.c]++;
		}
	}

	return true;
}
bool Game::move_wolf(int wolf, const coord &pos) { return move(wolf + deerc, pos); }
bool Game::move_deer(const coord &pos) { return move(0, pos); }

GameState Game::game_over() {
	// Deer wins if wolves run out of time
	if (turns_remaining <= 0) { return DEER_VICTORY; }

	// Wolves win if deer has nowhere to move
	vector<vector<int>> get_deer_mask(false);
	bool can_move = false;
	if (!can_move) { return WOLF_VICTORY; }

	return ONGOING;
}

// Given a grid of integers and a function that converts integers to strings,
// print out board with labels.
const void Game::print_board(const vector<vector<int>> &board, string (Game::*int_repr)(int)) {
	// Calculate row and column marker widths
	// Note that we actually need the ceiling, not the floor
	int r_mark_w = log_floor(10, board.size()) + 1;
	int c_mark_w = log_floor(26, board[0].size()-1) + 1;

	// Create row separator and column markers
	static string sep = "";
	if (sep == "") {
		sep = "   ";
		for (int i = 0; i < r_mark_w; i++) { sep += ' '; }
		sep += '+';
		for (int i = 0; i < board[0].size(); i++) {
			for (int j = 0; j < c_mark_w; j++) { sep += "-"; }
			sep += "--+";
		}
		sep += '\n';
	}
	static string col_mark = "";
	if (col_mark == "") {
		col_mark = "    ";
		for (int i = 0; i < r_mark_w; i++) { col_mark += ' '; }
		for (int i = 0; i < board[0].size(); i++) {
			col_mark += " " + itoc(i) + "  ";
		}
		col_mark += '\n';
	}

	// Info header
	cout << board.size() << "x" << board[0].size() << ", " << turns_remaining << " turns left\n";

	// Print board
	cout << col_mark << sep;
	for (int r = 0; r < board.size(); r++) {
		cout << setw(r_mark_w+2) << right << r+1 << " |";
		for (int c = 0; c < board[0].size(); c++) {
			int loc = this->board[r][c];
			if (loc != -1) {
				string name = player_names[loc].substr(0, 3);
				while (name.size() < 3) { name += ' '; }
				cout << name;
			} else {
				cout << (this->*int_repr)(board[r][c]);
			}
			cout << "|";
		}
		cout << setw(r_mark_w+1) << right << r+1 << "\n" << sep;
	}
	cout << col_mark << endl;
}
void Game::print_board() { print_board(board, &Game::int2player); }
void Game::print_deer_cover() { print_board(get_deer_mask(false), &Game::int2deer_reach); }
void Game::print_wolf_cover() { print_board(wolf_mask, &Game::int2wolf_reach); }
void Game::print_combined_cover() { print_board(get_deer_mask(true), &Game::int2deer_reach); }
