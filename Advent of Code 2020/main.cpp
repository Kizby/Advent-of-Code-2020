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

void fetch(string day, string where) {
	Client cli("https://adventofcode.com");
	cli.set_default_headers({
		{"cookie", "session=REDACTED"}
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

// from https://stackoverflow.com/a/116083
string slurp(ifstream& in) {
	stringstream out;
	while (in >> out.rdbuf());
	string s = out.str();
	auto last_non_whitespace = s.find_last_not_of("\n\r ");
	return s.substr(0, last_non_whitespace + 1);
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

	// Lock the handle and copy the text to the buffer. 
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
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
	for (const auto& line : split(slurp(in))) {

	}
}

int main() {
	day2();
}