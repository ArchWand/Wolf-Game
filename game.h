#ifndef __GAME_H__
#define __GAME_H__

#include <iostream>
#include <ostream>
#include <vector>
#include <unordered_set>
#include <utility>
using namespace std;

#define uset unordered_set

typedef struct CoordStruct {
	int r;
	int c;

	// Binary addition and subtraction operator for calculating valid moves
	struct CoordStruct operator+(const struct CoordStruct& other) const {
		return {r + other.r, c + other.c};
	}

	struct CoordStruct operator-(const struct CoordStruct& other) const {
		return {r - other.r, c - other.c};
	}

	// Equality operator for proper functioning of hash maps and sets
	bool operator==(const struct CoordStruct& other) const {
		return r == other.r && c == other.c;
	}

	// Ostream operator for debugging
	friend ostream &operator<<(ostream &os, const struct CoordStruct &obj) {
		os << "(" << obj.r << ", " << obj.c << ")";
		return os;
	}
} coord;

// Custom hash function
namespace std {
template <>
struct hash<coord> {
	size_t operator()(const coord& key) const {
		size_t h1 = std::hash<int>{}(key.r);
		size_t h2 = std::hash<int>{}(key.c);
		return h1 ^ (h2 << 1);
	}
};
}

class Game {
private:
	vector<coord> players;
	vector<string> player_names;
	vector<vector<int>> board;
	// This is how many turns the wolves have to catch the deer before the deer wins
	int turns_remaining;

	bool in_bounds(const coord &pos);
	bool valid_move(int id, const coord &pos);
	void update_mask(vector<vector<int>> &mask, const coord &orig,
				  const uset<coord> &moves, int step);

	// Use int instead of bool for compatibility with print_board helper
	vector<vector<int>> wolf_mask;
	vector<vector<int>> get_deer_mask(bool show_wolves);

	string int2player(int i);
	string int2deer_reach(int i);
	string int2bool(int i);

	const void print_board(const vector<vector<int>> &board, string (Game::*int_repr)(int));

public:

	// move lists
	uset<coord> deer_moves;
	uset<coord> wolf_moves;

	Game(int turns_remaining, const coord &size, const coord &deer, const vector<coord> &wolves, const vector<string> &player_names, const uset<coord> &deer_moves, const uset<coord> &wolf_moves);
	bool move(int id, const coord &pos);
	bool move_wolf(int wolf, const coord &pos);
	bool move_deer(const coord &pos);

	// Game state printers
	void print_board();
	void print_deer_cover();
	void print_wolf_cover();
	void print_combined_cover();
};

#endif
