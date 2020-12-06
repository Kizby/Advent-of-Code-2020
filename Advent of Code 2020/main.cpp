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
class lazy_vector : public vector<T> {
public:
	lazy_vector() {}
	lazy_vector(const vector<T>& source) : vector<T>(source) {}
	typename vector<T>::reference operator[](const size_t n) {
		if (n >= vector<T>::size()) {
			vector<T>::resize(n + 1);
		}
		return vector<T>::operator[](n);
	}
	typename vector<T>::const_reference operator[](const size_t n) const {
		if (n >= vector<T>::size()) {
			vector<T>::resize(n + 1);
		}
		return vector<T>::operator[](n);
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

bool in_range(const string& s, int64_t min, int64_t max) {
	size_t end = 0;
	int64_t val = stoll(s, &end);
	return val >= min && val <= max && end == s.size();
}

bool ends_with(const string& s, const string& what, string* rest = nullptr) {
	if (what.size() > s.size() || s.substr(s.size() - what.size()) != what) {
		return false;
	}
	if (rest) {
		*rest = s.substr(0, s.size() - what.size());
	}
	return true;
}

bool starts_with(const string& s, const string& what, string* rest = nullptr) {
	if (what.size() > s.size() || s.substr(0, what.size()) != what) {
		return false;
	}
	if (rest) {
		*rest = s.substr(what.size());
	}
	return true;
}

void day4() {
	auto in = input("4");
	vector<unordered_map<string, string>> passports;
	for (const auto& lines : split(slurp(in), "\n\n")) {
		passports.push_back(process_batch(split(lines)));
	}
	unordered_map<string, function<bool(string)>> fields{
		{"byr", [](string s) {return in_range(s, 1920, 2002); }},
		{"iyr", [](string s) {return in_range(s, 2010, 2020); }},
		{"eyr", [](string s) {return in_range(s, 2020, 2030); }},
		{"hgt", [](string s) {return ends_with(s, "cm", &s) && in_range(s, 150, 193) || ends_with(s, "in", &s) && in_range(s, 59, 76); }},
		{"hcl", [](string s) {return regex_match(s, regex("#[0-9a-f]{6}")); }},
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
				cout << "Invalid " << field.first << ":" << passport[field.first] << endl;
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

class computer {
public:
	computer(const vector<int64_t>& program, bool von_neumann) : program(von_neumann ? memory : program_memory) {
		this->program = program;
	}
	void attach_input(int64_t which, unique_ptr<istream> &&input) {
		inputs[which] = move(input);
	}
	void attach_input(unique_ptr<istream> &&input) {
		attach_input(0, move(input));
	}
	void attach_output(int64_t which, unique_ptr<ostream> &&output) {
		outputs[which] = move(output);
	}
	void attach_output(unique_ptr<ostream> &&output) {
		attach_output(0, move(output));
	}
	void execute() {
		while (!stop) {
			const auto &op = ops[program[ip++]];
			vector<int64_t> args;
			for (auto i = 0; i < op.arg_count; ++i) {
				args.push_back(program[ip++]);
			}
			op.handle(*this, args);
		}
	}
private:
	int64_t read() {
		int64_t result;
		*inputs[0] >> result;
		return result;
	}
	void write(int64_t what) {
		*outputs[0] << what;
	}

	struct descriptor {
		int arg_count;
		function<void(computer&, vector<int64_t>)> handle;
	};
	static unordered_map<int64_t, descriptor> ops;

	unordered_map<int64_t, unique_ptr<istream>> inputs;
	unordered_map<int64_t, unique_ptr<ostream>> outputs;
	unordered_map<int64_t, int64_t> registers;
	lazy_vector<int64_t> program_memory;
	lazy_vector<int64_t> memory;
	lazy_vector<int64_t>& program;
	size_t ip = 0;
	bool stop = false;
};

unordered_map<int64_t, computer::descriptor> computer::ops{
	{-1, {0, [](computer& self, vector<int64_t> args) {self.stop = true; }}}, // HALT
	{ 0, {3, [](computer& self, vector<int64_t> args) {self.registers[args[2]] = self.registers[args[0]] + self.registers[args[1]]; }}}, // ADD
	{ 1, {3, [](computer& self, vector<int64_t> args) {self.registers[args[2]] = self.registers[args[0]] - self.registers[args[1]]; }}}, // SUBTRACT
	{ 2, {3, [](computer& self, vector<int64_t> args) {self.registers[args[2]] = self.registers[args[0]] * self.registers[args[1]]; }}}, // MULTIPLY
	{ 3, {3, [](computer& self, vector<int64_t> args) {self.registers[args[2]] = self.registers[args[0]] / self.registers[args[1]]; }}}, // DIVIDE
	{ 4, {3, [](computer& self, vector<int64_t> args) {self.registers[args[2]] = self.registers[args[0]] % self.registers[args[1]]; }}}, // MODULUS
	{ 5, {2, [](computer& self, vector<int64_t> args) {self.registers[args[1]] = -self.registers[args[0]]; }}}, // NEGATE
	{ 6, {2, [](computer& self, vector<int64_t> args) {self.memory[args[1]] = self.registers[args[0]]; }}}, // STORE
	{ 7, {2, [](computer& self, vector<int64_t> args) {self.registers[args[1]] = self.memory[args[0]]; }}}, // LOAD
	{ 8, {1, [](computer& self, vector<int64_t> args) {self.registers[args[0]] = self.read(); }}}, // READ
	{ 9, {1, [](computer& self, vector<int64_t> args) {self.write(self.registers[args[0]]); }}}, // WRITE
	{10, {1, [](computer& self, vector<int64_t> args) {self.ip = args[0]; }}}, // JUMP
	{11, {2, [](computer& self, vector<int64_t> args) {if (self.registers[args[0]] < 0) self.ip = args[1]; }}}, // JUMP-IF-NEGATIVE
	{12, {2, [](computer& self, vector<int64_t> args) {if (self.registers[args[0]] > 0) self.ip = args[1]; }}}, // JUMP-IF-POSITIVE
	{13, {2, [](computer& self, vector<int64_t> args) {self.ip = self.registers[args[0]]; }}}, // INDIRECT-JUMP
};

void day5() {
	auto in = input("5");
	int max = 0;
	set<int> seats;
	for (const auto& line : split(slurp(in))) {
		int result = 0;
		for (auto c : line) {
			result *= 2;
			if (c == 'B' || c == 'R') {
				++result;
			}
		}
		seats.insert(result);
		if (result > max) {
			max = result;
		}
	}
	report(max);
	int last = -1;
	for (auto n : seats) {
		if (n - last == 2) {
			report(n - 1);
			break;
		}
		last = n;
	}
}

set<char> process_lines(vector<string> lines) {
	set<char> result;
	for (auto line : lines) {
		for (auto c : line) {
			result.insert(c);
		}
	}
	return result;
}

set<char> intersect(vector<string> lines) {
	set<char> result;
	for (char c = 'a'; c <= 'z'; c++) {
		result.insert(c);
	}
	for (auto line : lines) {
		set<char> cur;
		for (auto c : line) {
			cur.insert(c);
		}
		set<char> sub;
		for (auto c : result) {
			if (cur.count(c) == 0) {
				sub.insert(c);
			}
		}
		for (auto c : sub) {
			result.erase(c);
		}
	}
	return result;
}

void day6() {
	auto in = input("6");
	vector<set<char>> answers, answers2;
	for (const auto& lines : split(slurp(in), "\n\n")) {
		answers.push_back(process_lines(split(lines)));
		answers2.push_back(intersect(split(lines)));
	}
	size_t result = 0;
	for (auto a : answers) {
		result += a.size();
	}
	size_t result2 = 0;
	for (auto a : answers2) {
		result2 += a.size();
	}
	report(result);
	report(result2);
}

void day7() {
	auto in = input("7");
	for (const auto& line : split(slurp(in))) {

	}
}

int main() {
	day4();
}