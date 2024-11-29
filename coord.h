#ifndef __COORD_H__
#define __COORD_H__

#include <ostream>
#include <utility>
using namespace std;

/**
 * A basic Cartesian coordinate struct containing the corresponding row and
 * column.
 * We define +, -, ==, <<, and hash() on it for ease of use.
 */
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


#endif
