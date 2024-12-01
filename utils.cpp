#include "utils.h"

int log_floor(int base, int val) {
	int i = 0;
	while (val /= base) i++;
	return i;
}

// Convert a number into the appropriate alphabetic representation
// (e.g. 28 == "AC")
string itoc(int i) {
	string s = "";
	do {
		s += 'A' + (i % 26);
	} while (i /= 26);

	// Reverse
	for (int i = 0; i < s.size()/2; i++) {
		char tmp = s[i];
		s[i] = s[s.size()-i-1];
		s[s.size()-i-1] = tmp;
	}
	return s;
}

// Covert an alphabetical column label into the column number (e.g. 'AC' == 28)
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

// Convert alphanumeric coordinates into a coord struct (e.g. 'C11' == (10, 2))
// Note that alphanumeric coordinates have the column first and are 1-indexed,
// while coords have the row first and are 0-indexed.
coord atoc(string s) {
	coord c;
	string alph = "";
	int i;
	for (i = 0; i < s.size(); i++) {
		// Switch over to the number portion
		if ('0' <= s[i] && s[i] <= '9') { break; }
		alph.push_back(s[i]);
	}

	// Make sure that a number is given
	if (i == s.size()) {
		error("atoc: tried to convert '%s' to coordinates, but number is missing.", s.c_str());
	}

	// We use 0-indexed arrays
	c.r = stoi(s.substr(i)) - 1;
	c.c = Atoi(alph);
	return c;
}

