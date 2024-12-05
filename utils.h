#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <string>
#include <vector>

#define error(...) printf("Error: "); printf(__VA_ARGS__); std::cout << std::endl; exit(1);
#define in_set(a, b) ((a).find(b) != (a).end())

typedef struct CoordStruct coord;

int log_floor(int base, int val);

// Convert a number into the appropriate alphabetic representation
// (e.g. 28 == "AC")
std::string itoc(int i);

// Covert an alphabetical column label into the column number (e.g. 'AC' == 28)
int Atoi(std::string alph);

// Convert alphanumeric coordinates into a coord struct (e.g. 'C11' == (10, 2))
// Note that alphanumeric coordinates have the column first and are 1-indexed,
// while coords have the row first and are 0-indexed.
coord atoc(std::string s);

/**
 * A basic Cartesian coordinate struct containing the corresponding row and
 * column.
 * We define +, -, ==, <<, and hash() on it for ease of use.
 */
struct CoordStruct {
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
	friend std::ostream &operator<<(std::ostream &os, const struct CoordStruct &obj) {
		os << itoc(obj.c) << obj.r+1;
		return os;
	}
};

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

#endif
