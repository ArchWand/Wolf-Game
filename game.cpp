#include "game.h"
#include <iomanip>
#include <queue>

bool Game::in_bounds(const coord &pos) {
	return 0 <= pos.r && pos.r < board.size()
		&& 0 <= pos.c && pos.c < board[0].size();
}

bool Game::valid_move(int id, const coord &pos) {
	// Check that this is a valid player id
	if (!(0 <= id && id <= players.size())) { return false; }

	// Check that the position is in the board
	if (!in_bounds(pos)) { return false; }

	// Check that the position is not occupied by someone else
	if (board[pos.r][pos.c] != -1) {
		if (board[pos.r][pos.c] == id) {
			// Occupied by self; no move
			return true;
		} else {
			// Occupied by someone else
			return false;
		}
	}

	// Check that the player is allowed to move there
	uset<coord> *moves;
	if (id == 0) {
		moves = &deer_moves;
		// If the position is covered by a wolf, it is invalid for a deer
		if (wolf_mask[pos.r][pos.c]) {
			return false;
		}
	} else {
		moves = &wolf_moves;
	}
	if (moves->find(pos - players[id]) == moves->end()) {
		return false;
	}

	return true;
}

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
			i++;
			swap(locs, next);
		}
		// Only search 2 moves at most
		if (i > 2) {
			break;
		}
	}

	if (show_wolves) {
		for (int r = 0; r < mask.size(); r++) {
			for (int c = 0; c < mask.size(); c++) {
				if (wolf_mask[r][c]) {
					mask[r][c] = -2;
				}
			}
		}
	}

	return mask;
}

int log_floor(int base, int val) {
	int i = 0;
	while (val /= base) i++;
	return i;
}

string itoc(int i) {
	string s = "";
	do {
		s += 'A' + i;
	} while (i /= 26);

	string rev = "";
	for (int i = s.size(); i >= 0; i--) rev += s[i];
	return rev;
}

string Game::int2player(int i) {
	switch (i) {
		case -1:
			return "   ";
			break;
		default:
			if (0 <= i && i < players.size()) {
				return player_names[i].substr(0, 3);
			} else {
				// Unindentifiable
				return "???";
			}
	}
}

string Game::int2deer_reach(int i) {
	switch (i) {
		case -1:
			return "   ";
			break;
		case 0:
			return " O ";
			break;
		case 1:
			return " . ";
			break;
		case 2:
			return "(-)";
			break;
		case -2:
			// Covered by wolf
			return "<X>";
		default:
			return "???";
	}
}

string Game::int2bool(int i) {
	return i ? "<X>" : "   ";
}

const void Game::print_board(const vector<vector<int>> &board, string (Game::*int_repr)(int)) {
	// Info header
	cout << board.size() << "x" << board[0].size() << ", " << turns_remaining << " turns left\n";

	// Calculate row and column marker widths
	// Note that we actually need the ceiling, not the floor
	int r_mark_w = log_floor(10, board.size()) + 1;
	int c_mark_w = log_floor(26, board[0].size()) + 1;

	// Create row separator and column markers
	string sep = "   ";
	string col_mark = "    ";
	for (int i = 0; i < r_mark_w; i++) {
		sep += ' ';
		col_mark += ' ';
	}
	sep += '-';
	for (int i = 0; i < board[0].size(); i++) {
		sep += "---";
		for (int j = 0; j < c_mark_w; j++, sep += "-");
		col_mark += " " + itoc(i) + "  ";
	}
	sep += '\n';
	col_mark += '\n';
	cout << col_mark << sep;
	for (int r = 0; r < board.size(); r++) {
		cout << setw(r_mark_w+2) << right << r+1 << " |";
		for (int c = 0; c < board[0].size(); c++) {
			cout << (this->*int_repr)(board[r][c]) << "|";
		}
		cout << "\n" << sep;
	}
	cout << col_mark << endl;
}

void Game::update_mask(vector<vector<int>> &mask, const coord &orig, const uset<coord> &moves, int step) {
	for (coord delta : moves) {
		coord c = orig + delta;
		if (!in_bounds(c)) { continue; }
		mask[c.r][c.c] += step;
	}
}

Game::Game(int turns_remaining, const coord &size, const coord &deer,
		   const vector<coord> &wolves, const vector<string> &player_names,
		   const uset<coord> &deer_moves, const uset<coord> &wolf_moves)
	: turns_remaining(turns_remaining), players(wolves),
	  player_names(player_names), board(size.r, vector<int>(size.c, -1)),
	  wolf_mask(size.r, vector<int>(size.c, 0)), deer_moves(deer_moves),
	  wolf_moves(wolf_moves) {
	// Set position of deer
	players.insert(players.begin(), deer);
	if (!in_bounds(deer)) {
		cout << deer << " is not in bounds" << endl;
	}

	board[deer.r][deer.c] = 0;

	// Set position of wolves
	for (int i = 0; i < wolves.size(); i++) {
		if (!in_bounds(wolves[i])) {
			cout << wolves[i] << " is not in bounds" << endl;
		}
		board[wolves[i].r][wolves[i].c] = i + 1;
		// Update the coverage information
		update_mask(wolf_mask, wolves[i], wolf_moves, 1);
	}
}

bool Game::move(int id, const coord &pos) {
	// Check that this is a valid move
	if (!valid_move(id, pos)) { return false; }

	// Move the player
	// Remove the player
	board[players[id].r][players[id].c] = -1;
	// If the player is a wolf, remove its effect from the mask
	if (id != 0) { update_mask(wolf_mask, players[id], wolf_moves, -1); }

	// Place the player back in the new spot
	players[id] = pos;
	board[pos.r][pos.c] = id;

	// If the player is the deer, update the number of turns left
	if (id == 0) {
		// Last step was an intermediate step
		static bool im = false;
		if (im) {
			turns_remaining--;
		}
		im = !im;
	} else {
		// Update the new wolf coverage
		if (id != 0) { update_mask(wolf_mask, players[id], wolf_moves, 1); }
	}


	return true;
}
bool Game::move_wolf(int wolf, const coord &pos) { return move(wolf + 1, pos); }
bool Game::move_deer(const coord &pos) { return move(0, pos); }

void Game::print_board() { print_board(board, &Game::int2player); }
void Game::print_deer_cover() { print_board(get_deer_mask(false), &Game::int2deer_reach); }
void Game::print_wolf_cover() { print_board(wolf_mask, &Game::int2bool); }
void Game::print_combined_cover() { print_board(get_deer_mask(true), &Game::int2deer_reach); }
