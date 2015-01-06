#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/css/datatype.hpp>
#include <derpkit/css/expression.hpp>
#include <derpkit/utils/string_utils.hpp>

namespace css {

bool Position::parse(const Expression* val){
	if ( val == nullptr || val->terms.empty() ) return false;

	const Term& term = val->terms[0];
	if ( term.type != Term::TYPE_STRING ) return false;

	const std::string v = lcase(term.value);

	if ( v == "static" ) {
		value = STATIC;
	} else if ( v == "relative" ) {
		value = RELATIVE;
	} else if(v == "absolute") {
		value = ABSOLUTE;
	} else {
		return false;
	}

	return true;
}

std::string Position::to_string() const {
	switch ( value ){
	case STATIC:   return "static";
	case RELATIVE: return "relative";
	case ABSOLUTE: return "absolute";
	}
	return "unknown";
}

}
