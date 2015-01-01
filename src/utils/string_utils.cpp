#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <vector>
#include <iostream>

#include <derpkit/utils/string_utils.hpp>

std::string str_trim(std::string s) {
	return str_trim(s, " \t\n\r");
}

std::string str_trim(std::string s, const char* chars){
	size_t begin_str = s.find_first_not_of(chars);
	if(begin_str != std::string::npos) {
		size_t last = s.find_last_not_of(chars);
		if(last >= begin_str) {
			std::string trimmed = s.substr(begin_str, last - begin_str + 1);
			return trimmed;
		} else {
			return "";
		}
	} else {
		return "";
	}
}

std::vector<std::string> str_split(const std::string &str, const std::string &search, int flags, int max_splits) {
	auto trim = [flags](std::string str) -> std::string { return (flags & SPLIT_TRIM) ? str_trim(str) : str; };
	const bool keep = 0 != (flags & SPLIT_KEEP_DELIM);
	const bool ignore_empty = 0 != (flags & SPLIT_IGNORE_EMPTY);

	size_t pos = 0;
	std::vector<std::string> res;
	size_t p;
	int n = 0;

	while((p = str.find_first_of(search, pos)) != std::string::npos && (max_splits == 0 || n++ < max_splits) ) {
		const size_t len = (p - pos) + (keep ? search.length() : 0);
		const std::string s = trim(str.substr(pos, len));
		if ( !(ignore_empty && s.empty()) ) res.push_back(s);
		pos = p + search.length();
	}
	if(pos < str.length()) {
		res.push_back(trim(str.substr(pos)));
	}
	return res;
}
