#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <set>
#include <sstream>
#include <algorithm>
extern "C" {
#include <Windows.h>
}

#define YEAR "2020"

using namespace std;
using namespace httplib;

// from https://stackoverflow.com/a/116083
string slurp(ifstream& in) {
	stringstream out;
	while (in >> out.rdbuf());
	string s = out.str();
	auto last_non_whitespace = s.find_last_not_of("\n\r ");
	return s.substr(0, last_non_whitespace + 1);
}

void fetch(string day, string where) {
	ifstream session("session.txt");
	Client cli("https://adventofcode.com");
	cli.set_default_headers({
		{"cookie", "session=" + slurp(session)}
		});
	string path = "/" YEAR "/day/" + day + "/input";
	if (auto res = cli.Get(path.c_str())) {
		if (res->status == 200) {
			ofstream fout(where);
			fout << res->body;
			fout.close();
		}
		else {
			cout << "Status was: " << res->status << endl;
		}
	}
	else {
		auto err = res.error();
		cout << err << endl;
	}
}

ifstream input(string day) {
	auto filename = day + ".txt";
	if (!filesystem::exists(filename)) {
		fetch(day, filename);
	}
	return ifstream(filename);
}

// from https://stackoverflow.com/a/9437426
vector<string> split(const string& input, const string& regex = "\n") {
	// passing -1 as the submatch index parameter performs splitting
	std::regex re(regex);
	std::sregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return { first, last };
}

vector<int64_t> map_to_num(const vector<string>& vec) {
	vector<int64_t> result;
	result.resize(vec.size());
	transform(vec.begin(), vec.end(), result.begin(), [](const string& s) { return s.empty() ? 0 : stoll(s); });
	return result;
}

vector<vector<size_t>> permutations(size_t start, size_t end) {
	if (start >= end) {
		return vector<vector<size_t>>{vector<size_t>{}};
	}
	auto base = permutations(start, end - 1);
	vector<vector<size_t>> result;
	for (auto elem : base) {
		for (int i = 0; i <= elem.size(); ++i) {
			auto next = elem;
			next.insert(next.begin() + i, end - 1);
			result.push_back(move(next));
		}
	}
	return result;
}

vector<vector<size_t>> permutations(size_t n) {
	return permutations(0, n);
}

template <typename T, typename C>
unordered_map<T, int> histogram(C container) {
	unordered_map<T, int> result;
	for (T one : container) {
		++result[one];
	}
	return result;
}

void copy(string s)
{
	if (s.size() > 200) {
		// probably don't want to copy this...
		cout << "String is too long: " << s.size() << endl;
		return;
	}

	// Open the clipboard, and empty it. 
	if (!OpenClipboard(NULL))
		return;
	EmptyClipboard();

	// Copy text using the CF_TEXT format. 
	// Allocate a global memory object for the text. 
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE,
		(s.size() + 1) * sizeof(TCHAR));
	if (hglbCopy == NULL) {
		// failed to get memory
		CloseClipboard();
		return;
	}

	// Lock the handle and copy the text to the buffer. 
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
	if (lptstrCopy == NULL) {
		// failed to lock memory
		CloseClipboard();
		return;
	}

	memcpy(lptstrCopy, s.c_str(), s.size() * sizeof(TCHAR));
	lptstrCopy[s.size()] = '\0';
	GlobalUnlock(hglbCopy);

	// Place the handle on the clipboard. 
	SetClipboardData(CF_TEXT, hglbCopy);

	// Close the clipboard. 
	CloseClipboard();
}

void report(const string& s) {
	cout << s << endl;
	copy(s);
}

template <typename T>
void report(T n) {
	report(to_string(n));
}

void day1() {
	auto in = input("1");
	auto nums = map_to_num(split(slurp(in)));
	int64_t part1 = 0;
	int64_t part2 = 0;
	for (auto num : nums) {
		for (auto num2 : nums) {
			if (num == num2) {
				continue;
			}
			if (num + num2 == 2020) {
				part1 = num * num2;
			}
			for (auto num3 : nums) {
				if (num3 == num2 || num3 == num) {
					continue;
				}
				if (num + num2 + num3 == 2020) {
					part2 = num * num2 * num3;
				}
			}
		}
	}
	report(part1);
	report(part2);
}

void day2() {
	auto in = input("2");
	auto valid = 0;
	auto valid2 = 0;
	for (const auto& line : split(slurp(in))) {
		int min = 0, max = 0;
		auto tokens = split(line, " ");
		min = stol(tokens[0]);
		max = stol(tokens[0].substr(tokens[0].find('-') + 1));
		char which = tokens[1][0];
		int count = 0;
		for (int i = 0; i < tokens[2].size(); ++i) {
			if (tokens[2][i] == which) {
				++count;
			}
		}
		if (count >= min && count <= max) {
			++valid;
		}
		if ((tokens[2][min - 1] == which) ^ (tokens[2][max - 1] == which)) {
			++valid2;
		}
	}
	report(valid);
	report(valid2);
}

void day3() {
	auto in = input("3");
	vector<vector<bool>> grid;
	for (const auto& line : split(slurp(in))) {
		vector<bool> one;
		for (auto c : line) {
			one.push_back(c == '#');
		}
		grid.push_back(one);
	}
	size_t pos = 0, pos3 = 0, pos5 = 0, pos7 = 0;
	int64_t count = 0, count3 = 0, count5 = 0, count7 = 0, count2 = 0;
	for (int i = 0; i < grid.size(); ++i) {
		if (grid[i][pos % grid[i].size()]) {
			++count;
		}
		if (grid[i][pos3 % grid[i].size()]) {
			++count3;
		}
		if (grid[i][pos5 % grid[i].size()]) {
			++count5;
		}
		if (grid[i][pos7 % grid[i].size()]) {
			++count7;
		}
		if (i%2 == 0 && grid[i][i/2 % grid[i].size()]) {
			++count2;
		}
		pos += 1;
		pos3 += 3;
		pos5 += 5;
		pos7 += 7;
	}
	report(count3);
	report(count);
	report(count3);
	report(count5);
	report(count7);
	report(count2);
	report(count * count3 * count5 * count7 * count2);
}

void day4() {
	auto in = input("4");
	for (const auto& line : split(slurp(in))) {

	}
}

int main() {
	day4();
}