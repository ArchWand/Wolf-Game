#ifndef __GAME_H__
#define __GAME_H__

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
using namespace std;

#include "utils.h"

enum GameException {
	DeerOutOfBounds = 0,
	WolfOutOfBounds = 1,
};

enum MoveError {
	SUCCESS = 0,
	INVALID_ID,
	ALREADY_MOVED,
	OUT_OF_BOUNDS,
	POSITION_OCCUPIED,
	DEER_IN_CHECK,
	INVALID_MOVE,
};

enum GameState {
	ONGOING = 0,
	DEER_VICTORY,
	WOLF_VICTORY,
};

#define umap unordered_map
#define uset unordered_set

class Game {
public:
	Game(int turns_remaining, const coord &size, const coord &deer,
	  const vector<coord> &wolves, const vector<string> &player_names,
	  const uset<coord> &deer_moves, const uset<coord> &wolf_moves);
	~Game();

private:
	// A representation of the game board, where each position contains the id
	// of the player there, with -1 representing empty positions.
	vector<vector<int>> board;

	// Arrays containing the location (and name) of every player. deer and
	// wolves are pointers into the main players array; in players, all deer are
	// contiguous and stored at the start, and all wolves are contiguous and
	// stored at the end.
	coord *players;
	string *player_names;
	size_t playersc;

	coord *deer;
	size_t deerc;
	coord *wolves;
	size_t wolvesc;

	// This is how many turns the wolves have to catch the deer
	int turns_remaining;

	// Tracks whether or not a particular wolf has moved already
	bool *moved;

	// All possible moves for each type of player
	uset<coord> deer_moves;
	uset<coord> wolf_moves;

	// Use int instead of bool for compatibility with print_board helper
	vector<vector<int>> wolf_mask;
	vector<vector<int>> get_deer_mask(bool show_wolves);

public:
	// Getters and setters

	const coord get_player(int id);
	const coord get_deer();
	const coord get_wolf(int id);

	bool move(int id, const coord &pos);
	bool move_wolf(int wolf, const coord &pos);
	bool move_deer(const coord &pos);

	GameState game_over();

	// Game state printers
	void print_board();
	void print_deer_cover();
	void print_wolf_cover();
	void print_combined_cover();

private:
	// Helper functions
	bool is_wolf(int id);
	bool in_bounds(const coord &pos);
	int valid_move(int id, const coord &pos);

	string int2player(int i);
	string int2deer_reach(int i);
	string int2wolf_reach(int i);

	const void print_board(const vector<vector<int>> &board, string (Game::*int_repr)(int));
};

#endif
