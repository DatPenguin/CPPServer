#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <utility>

using namespace std;

bool startsWith(string s, string pattern) {
	const char *str = s.c_str(), *pat = pattern.c_str();
	for (unsigned int i = 0; i < strlen(pat); i++) {
		if (str[i] != pat[i])
			return false;
	}
	return true;
}

vector<std::string> split(std::string const &s, char delim) {
	std::vector<std::string> result;
	std::istringstream iss(s);

	for (std::string token; std::getline(iss, token, delim);)
		result.push_back(std::move(token));

	return result;
}