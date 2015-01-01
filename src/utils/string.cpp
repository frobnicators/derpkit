#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/utils/string.hpp>
#include <string>
#include <map>
#include <cstring>

namespace utils {

static std::map<std::string, const char*> string_pool;

String::String(const char* str, bool pool)
	: s(str)
	, pooled(false) {

	if ( pool ){
		auto it = string_pool.find(str);
		if ( it != string_pool.end() ){
			s = it->second;
		} else {
			string_pool[str] = str;
		}
	}
}

String::String(String&& str)
	: s(str.s)
	, pooled(str.pooled){

}

String String::make_pooled() const {
	return String(s, true);
}

bool String::operator<(const String& rhs) const {
	if ( pooled ){
		return s < rhs.s;
	} else {
		return strcmp(s, rhs.s) < 0;
	}
}

bool String::operator==(const String& rhs) const {
	if ( pooled ){
		return s == rhs.s;
	} else {
		return strcmp(s, rhs.s) == 0;
	}
}

}
