#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "coord.h"

#define error(...) printf("Error: "); printf(__VA_ARGS__); cout << endl; exit(1);

int log_floor(int base, int val);

// Convert a number into the appropriate alphabetic representation
// (e.g. 28 == "AC")
string itoc(int i);

// Covert an alphabetical column label into the column number (e.g. 'AC' == 28)
int Atoi(string alph);

// Convert alphanumeric coordinates into a coord struct (e.g. 'C11' == (10, 2))
// Note that alphanumeric coordinates have the column first and are 1-indexed,
// while coords have the row first and are 0-indexed.
coord atoc(string s);

#endif
