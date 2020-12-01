#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <set>
#include <sstream>

#define YEAR "20XX"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

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
vector<string> split(const string& input, const string& regex) {
	// passing -1 as the submatch index parameter performs splitting
	std::regex re(regex);
	std::sregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return { first, last };
}

// from https://stackoverflow.com/a/116083
string slurp(ifstream &in) {
	stringstream out;
	while (in >> out.rdbuf());
	string s = out.str();
	auto last_non_whitespace = s.find_last_not_of("\n\r ");
	return s.substr(0, last_non_whitespace + 1);
}

int main() {
}