#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <set>
#include <deque>
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
	sregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return { first, last };
}

vector<vector<string>> split(const vector<string>& input, const string& regex = "\n") {
	// passing -1 as the submatch index parameter performs splitting
	std::regex re(regex);
	vector<vector<string>> result;
	for (const auto& line : input) {
		sregex_token_iterator
			first{ line.begin(), line.end(), re, -1 },
			last;
		result.push_back({ first, last });
	}
	return result;
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
/*
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
*/
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
/*
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
}*/

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
/*
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
}*/

template<typename T>
set<T> intersect(const vector<set<T>> &sets) {
	set<T> result;
	if (sets.empty()) {
		return result;
	}
	result = sets[0];
	for (size_t i = 1; i < sets.size(); ++i) {
		set<T> next = result;
		for (const auto &element : result) {
			if (sets[i].count(element) == 0) {
				next.erase(element);
			}
		}
		result = next;
	}
	return result;
}

template<typename T, typename C>
set<T> intersect(const vector<C>& inputs, function<set<T>(const C&)> map) {
	vector<set<T>> sets;
	for (const auto& c : inputs) {
		sets.push_back(map(c));
	}
	return intersect(sets);
}

template<typename T>
set<T> unionize(const vector<set<T>>& sets) {
	set<T> result;
	if (sets.empty()) {
		return result;
	}
	result = sets[0];
	for (size_t i = 1; i < sets.size(); ++i) {
		for (const auto& element : sets[i]) {
			result.insert(element);
		}
	}
	return result;
}

template<typename T, typename C>
set<T> unionize(const vector<C>& inputs, function<set<T>(const C&)> map) {
	vector<set<T>> sets;
	for (const auto& c : inputs) {
		sets.push_back(map(c));
	}
	return unionize(sets);
}

template<typename T>
set<T> difference(const set<T>& a, const set<T>& b) {
	set<T> result = a;
	for (const auto& element : a) {
		if (b.count(element) > 0) {
			result.erase(element);
		}
	}
	return result;
}

template<typename T>
set<T> sym_difference(const set<T>& a, const set<T>& b) {
	return unionize<T>({ difference(a, b), difference(b, a) });
}

template<typename T, typename C>
T sum(const vector<C>& inputs, function<T(const C&)> map) {
	vector<T> nums;
	for (const auto& c : inputs) {
		nums.push_back(map(c));
	}
	return sum(nums);
}

template<typename T, typename C>
T product(const vector<C>& inputs, function<T(const C&)> map) {
	vector<T> nums;
	for (const auto& c : inputs) {
		nums.push_back(map(c));
	}
	return product(nums);
}

template<typename T, typename C>
set<T> into_set(const C& source) {
	return set<T>{ source.begin(), source.end() };
}
/*
void day6() {
	auto in = input("6");
	vector<set<char>> any_yes, all_yes;
	for (const auto& lines : split(slurp(in), "\n\n")) {
		any_yes.push_back(unionize<char, string>(split(lines), into_set<char,string>));
		all_yes.push_back(intersect<char, string>(split(lines), into_set<char,string>));
	}
	report(sum<size_t, set<char>>(any_yes, &set<char>::size));
	report(sum<size_t, set<char>>(all_yes, &set<char>::size));
}
*/

template<typename T>
class stack {
public:
	stack() : data() {}
	stack(initializer_list<T> init) : data(init) { }

	T pop() {
		T result = data[data.size() - 1];
		data.resize(data.size() - 1);
		return result;
	}
	void push(T what) {
		data.push_back(what);
	}
	size_t size() {
		return data.size();
	}
	bool empty() {
		return data.empty();
	}
private:
	vector<T> data;
};

template<typename T>
class queue {
public:
	queue() : data() {}
	queue(initializer_list<T> init) : data(init) { }

	T pop() {
		T result = data.front();
		data.pop_front();
		return result;
	}
	void push(T what) {
		data.push_back(what);
	}
	size_t size() {
		return data.size();
	}
	bool empty() {
		return data.empty();
	}
private:
	deque<T> data;
};
/*
int64_t count_day7(const string &name, map<string, map<string, int>>& rules, map<string, int64_t> &counts) {
	if (counts[name] > -1) {
		return counts[name];
	}
	int64_t result = 0;
	for (auto entry : rules[name]) {
		result += (count_day7(entry.first, rules, counts) + 1) * entry.second;
	}
	counts[name] = result;
	return result;

}
void day7() {
	auto in = input("7");
	map<string, map<string, int>> rules;
	map<string, set<string>> inverse;
	for (const auto& line : split(slurp(in))) {
		auto tokens = split(line, " ");
		auto color = tokens[0] + " " + tokens[1];
		if (tokens[4] == "no") {
			continue;
		}
		for (int i = 4; i < tokens.size(); i += 4) {
			auto inside = tokens[i + 1] + " " + tokens[i + 2];
			rules[color][inside] = stol(tokens[i]);
			inverse[inside].insert(color);
		}
	}
	set<string> known;
	stack<string> candidates{ "shiny gold" };
	while (!candidates.empty()) {
		auto candidate = candidates.pop();
		if (known.insert(candidate).second) {
			for (auto next : inverse[candidate]) {
				candidates.push(next);
			}
		}
	}
	report(known.size() - 1);
	map<string, int64_t> counts;
	for (auto entry : rules) {
		counts[entry.first] = -1;
	}
	report(count_day7("shiny gold", rules, counts));
}*/

class computer {
public:
	computer(const vector<string>& program) {
		this->program = split(program, " ");
	}
	computer(const vector<vector<string>>& program) {
		this->program = program;
	}
	void attach_input(int64_t which, queue<string> *input) {
		inputs[which] = input;
	}
	void attach_input(queue<string> *input) {
		attach_input(0, input);
	}
	void attach_output(int64_t which, queue<string> *output) {
		outputs[which] = output;
	}
	void attach_output(queue<string> *output) {
		attach_output(0, output);
	}
	void execute(function<bool()> condition) {
		while (condition() && ip < program.size()) {
			step();
		}
	}
	void step() {
		const auto& instruction = 
			patches.count(ip) > 0 ?
			patches[ip++] : program[ip++];
		ops[instruction[0]](*this, instruction);
	}
	string read() {
		return read(0);
	}
	string read(int64_t which) {
		return inputs[which]->pop();
	}
	void write(string what) {
		write(0, what);
	}
	void write(int64_t which, string what) {
		if (!outputs[which]) {
			outputs[which] = new queue<string>{};
		}
		outputs[which]->push(what);
	}
	void reset() {
		patches.clear();
		acc = 0;
		ip = 0;
	}

	static unordered_map<string, function<void(computer&, const vector<string>&)>> ops;

	unordered_map<int64_t, queue<string>*> inputs;
	unordered_map<int64_t, queue<string>*> outputs;
	int64_t acc = 0;
	vector<vector<string>> program;
	size_t ip = 0;
	unordered_map<size_t, vector<string>> patches;
};

unordered_map<string, function<void(computer&, const vector<string>&)>> computer::ops{
	{ "nop", [](computer& self, const vector<string>& args) { }},
	{ "acc", [](computer& self, const vector<string>& args) {self.acc += stoll(args[1]); } },
	{ "jmp", [](computer& self, const vector<string>& args) {self.ip += stoll(args[1]) - 1; }},
};
/*
void day8() {
	auto in = input("8");
	const auto lines = split(slurp(in));
	computer comp{ split(lines, " ") };
	auto& program = comp.program;
	set<size_t> seen;
	auto& ip = comp.ip;
	auto& acc = comp.acc;
	const auto& condition = [&]() {return seen.insert(ip).second; };
	comp.execute(condition);
	report(acc);
	for (size_t i = 0; i < program.size(); ++i) {
		comp.reset();
		auto& ins = program[i];
		if (ins[0] == "nop") {
			comp.patches.insert({ i, {"jmp", ins[1]} });
		} else if (ins[0] == "jmp") {
			comp.patches.insert({ i, {"nop", ins[1]} });
		}
		else {
			continue;
		}
		seen.clear();
		comp.execute(condition);
		if (ip == program.size()) {
			report(acc);
			break;
		}
	}
}*/

void day9() {
	auto in = input("9");
	vector<int64_t> nums = map_to_num(split(slurp(in)));
	int64_t target = 0;
	for (size_t i = 25; i < nums.size(); ++i) {
		bool found_one = false;
		for (size_t j = i < 25 ? 0 : i - 25; !found_one && j < i; ++j) {
			for (size_t k = j < 25 ? 0 : j - 25; !found_one && k < j; ++k) {
				if (nums[j] + nums[k] == nums[i]) {
					found_one = true;
					//cout << i << " = " << j << " + " << k << endl;
				}
			}
		}
		if (!found_one) {
			report(nums[i]);
			target = nums[i];
			break;
		}
	}
	for (size_t i = 0; i < nums.size(); ++i) {
		int64_t accum = nums[i];
		int64_t min = accum, max = accum;
		size_t j;
		for (j = i + 1; accum < target; ++j) {
			accum += nums[j];
			if (nums[j] < min) {
				min = nums[j];
			}
			if (nums[j] > max) {
				max = nums[j];
			}
		}
		if (accum == target) {
			report(min + max);
			break;
		}
	}
}

void day10() {
	auto in = input("10");
	for (const auto& line : split(slurp(in))) {

	}
}

int main() {
	day10();
}