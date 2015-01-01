#ifndef DERPKIT_UTILS_STRING_HPP
#define DERPKIT_UTILS_STRING_HPP

namespace utils {

class String {
public:
	String(const char* str, bool pool=false);
	String(String&& s);

	String make_pooled() const;

	bool operator<(const String& rhs) const;
	bool operator==(const String& rhs) const;

private:
	const char* s;
	bool pooled;
};

}

#endif /* DERPKIT_UTILS_STRING_HPP */
