#ifndef DERPKIT_STRING_UTILS_HPP
#define DERPKIT_STRING_UTILS_HPP

#include <string>
#include <sstream>
#include <algorithm>

std::string str_trim(std::string s);
std::string str_trim(std::string s, const char* chars);

enum {
	SPLIT_DEFAULT        = 0,      /* default behavior */
	SPLIT_TRIM           = (1<<0), /* trim all elements (whitespace) */
	SPLIT_IGNORE_EMPTY   = (1<<1), /* ignore empty lines */
	SPLIT_KEEP_DELIM     = (1<<2), /* retain delimiter at end of element */
};
std::vector<std::string> str_split(const std::string &str, const std::string &search, int flags=0, int max_splits=0);

inline std::string lcase(const std::string& str) {
	std::string ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	return ret;
}

/**
 * Replace part of a string.
 **/
std::string str_replace(std::string search, std::string replace, std::string subject);


template <class iterator>
std::ostream& str_join(std::ostream& s, iterator begin, iterator end, const char* delimiter){
	if ( begin != end ){
		s << *begin++;
	}
	while ( begin != end ){
		s << delimiter;
		s << *begin++;
	}
	return s;
}

/**
 * Join array of strings together, separated by delimiter.
 */
template <class iterator>
std::string str_join(iterator begin, iterator end, const char* delimiter){
	std::ostringstream s;
	str_join(s, begin, end, delimiter);
	return s.str();
}

#endif /* DERPKIT_STRING_UTILS_HPP */
