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

template<typename T>
class cyclic_vector : public vector<T> {
public:
	typename vector<T>::reference operator[](const size_t n) {
		return vector<T>::operator[](n % vector<T>::size());
	}
	typename vector<T>::const_reference operator[](const size_t n) const {
		return vector<T>::operator[](n % vector<T>::size());
	}
};

template<typename T>
T sum(vector<T> vec) {
	T result = 0;
	for (auto element : vec) {
		result += element;
	}
	return result;
}

template<typename T>
T product(vector<T> vec) {
	T result = 1;
	for (auto element : vec) {
		result *= element;
	}
	return result;
}

template<typename T>
T gcd(T a, T b) {
	if (a > b) {
		return gcd(b, a);
	}
	if (a == 0) {
		return b;
	}
	return gcd(b % a, a);
}

unordered_map<char, size_t> count_obstacles(size_t dx, size_t dy, const vector<cyclic_vector<char>> &field) {
	{
		auto divisor = gcd(dx, dy);
		if (divisor > 1) {
			return count_obstacles(dx / divisor, dy / divisor, field);
		}
	}
	unordered_map<char, size_t> result;
	for (size_t i = 0; i * dy < field.size(); ++i) {
		++result[field[i * dy][i * dx]];
	}
	return result;
}

void day3() {
	auto in = input("3");
	vector<cyclic_vector<char>> grid;
	for (const auto& line : split(slurp(in))) {
		cyclic_vector<char> one;
		for (auto c : line) {
			one.push_back(c);
		}
		grid.push_back(one);
	}
	vector<size_t> counts = { 
		count_obstacles(1, 1, grid)['#'],
		count_obstacles(3, 1, grid)['#'],
		count_obstacles(5, 1, grid)['#'],
		count_obstacles(7, 1, grid)['#'],
		count_obstacles(1, 2, grid)['#']
	};
	report(counts[1]);
	report(product(counts));
}

unordered_map<string, string> process_batch(vector<string> lines) {
	unordered_map<string, string> result;
	for (const auto& line : lines) {
		for (const auto& elem : split(line, " ")) {
			result[elem.substr(0, elem.find(":"))] = elem.substr(elem.find(":") + 1);
		}
	}
	return result;
}

void day4() {
	auto in = input("4");
	vector<string> lines;
	vector<unordered_map<string, string>> passports;
	for (const auto& line : split(slurp(in))) {
		if (line.empty()) {
			passports.push_back(process_batch(lines));
			lines.clear();
		}
		else {
			lines.push_back(line);
		}
	}
	passports.push_back(process_batch(lines));
	unordered_map<string, function<bool(string)>> fields{
		{"byr", [](string s) {return s.size() == 4 && stol(s) >= 1920 && stol(s) <= 2002; }},
		{"iyr", [](string s) {return s.size() == 4 && stol(s) >= 2010 && stol(s) <= 2020; }},
		{"eyr", [](string s) {return s.size() == 4 && stol(s) >= 2020 && stol(s) <= 2030; }},
		{"hgt", [](string s) {return s.find("cm") != -1 ? stol(s) >= 150 && stol(s) <= 193 : s.find("in") != -1 && stol(s) >= 59 && stol(s) <= 76; }},
		{"hcl", [](string s) {return !s.empty() && regex_match(s, regex("#[0-9a-f]{6}")); }},
		{"ecl", [](string s) {return set<string>{"amb", "blu", "brn", "gry", "grn", "hzl", "oth"}.count(s) == 1; }},
		{"pid", [](string s) {return regex_match(s, regex("[0-9]{9}")); } } };
	int valid_count = 0;
	int valid_count2 = 0;
	for (auto &passport : passports) {
		bool valid = true, valid2 = true;;
		for (auto field : fields) {
			if (passport[field.first].empty()) {
				valid = false;
				valid2 = false;
				break;
			}
			else if (!field.second(passport[field.first])) {
				cout << field.first << passport[field.first] << endl;
				valid2 = false;
			}
		}
		if (valid) {
			++valid_count;
		}
		if (valid2) {
			++valid_count2;
		}
	}
	report(valid_count);
	report(valid_count2);
}

void day5() {
	auto in = input("5");
	for (const auto& line : split(slurp(in))) {

	}
}

int main() {
	day5();
}