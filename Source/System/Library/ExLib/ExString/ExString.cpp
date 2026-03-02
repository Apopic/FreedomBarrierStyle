#include "ExString.hpp"

std::vector<std::string> split(const std::string& str, char del) {

	size_t first = 0;
	size_t last = str.find_first_of(del);

	std::vector<std::string> result;

	while (first < str.size()) {
		std::string subStr(str, first, last - first);

		result.push_back(subStr);

		if (last == std::string::npos) {
			break;
		}

		first = last + 1;
		last = str.find_first_of(del, first);
	}

	return result;

}

std::string strtrim(const std::string& str) {
	size_t s = 0, e = 0;
	for (int i = 0, size = str.size(); i < size; ++i) {
		switch (str[i])
		{
		case ' ':
			continue;
			break;
		case '	':
			continue;
			break;
		}
		s = i;
		break;
	}
	for (int i = str.size() - 1; i >= 0; --i) {
		switch (str[i])
		{
		case ' ':
			continue;
			break;
		case '	':
			continue;
			break;
		}
		e = i + 1;
		break;
	}

	return str.substr(s, e - s);
}

std::string ToUpper(const std::string& str) {
	std::string ret;
	for (size_t i = 0, size = str.size(); i < size; ++i) {
		ret += (char)toupper(str[i]);
	}
	return ret;
}

std::string ToLower(const std::string& str) {
	std::string ret;
	for (size_t i = 0, size = str.size(); i < size; ++i) {
		ret += (char)tolower(str[i]);
	}
	return ret;
}

