#ifndef DERPKIT_STRING_UTILS_HPP
#define DERPKIT_STRING_UTILS_HPP

#include <string>

std::string str_trim(std::string s);
std::string str_trim(std::string s, const char* chars);

enum {
	SPLIT_DEFAULT        = 0,      /* default behavior */
	SPLIT_TRIM           = (1<<0), /* trim all elements (whitespace) */
	SPLIT_IGNORE_EMPTY   = (1<<1), /* ignore empty lines */
	SPLIT_KEEP_DELIM     = (1<<2), /* retain delimiter at end of element */
};
std::vector<std::string> str_split(const std::string &str, const std::string &search, int flags=0, int max_splits=0);

#endif /* DERPKIT_STRING_UTILS_HPP */
