#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/utils/string_utils.hpp>
#include "parsers.hpp"
#include <string>

namespace css {
namespace parsers {

void display(dom::Display& out, const char* val) {
	if(val == nullptr) return;
	std::string v = lcase(val);

	if(v == "none") {
		out = dom::DISPLAY_NONE;
	} else if(v == "block") {
		out = dom::DISPLAY_BLOCK;
	} else if(v == "inline") {
		out = dom::DISPLAY_INLINE;
	} else if(v == "inline-block") {
		out = dom::DISPLAY_INLINE_BLOCK;
	}
}

void number(float& out, const char* val) {
	if(val == nullptr) return;

	out = 0.f;

	sscanf(val,"%f", &out);
}

void length(dom::Length& out, const char* val) {
	if(val == nullptr) return;
	std::string v = lcase(val);

	const float px_per_in = 96.f;
	const float px_per_pt = 96.f / 72.f;
	const float px_per_pc = 12 * px_per_pt;
	const float px_per_cm = (1.f/2.54f) * px_per_in;
	const float px_per_mm = px_per_cm / 10.f;

	size_t unit_start = v.find_first_not_of("0123456789.");
	if(unit_start == 0 && v == "auto") {
		out = {0, dom::UNIT_AUTO};
	} else if(unit_start != 0) {
		number(out.scalar, val);
		out.unit = dom::UNIT_PX;
		if(unit_start != std::string::npos) {
			std::string unit = v.substr(unit_start);
			if(unit == "px") {
				// Already set
			} else if(unit == "cm") {
				out.scalar *= px_per_cm;
			} else if(unit == "mm") {
				out.scalar *= px_per_mm;
			} else if(unit == "in") {
				out.scalar *= px_per_in;
			} else if(unit == "pt") {
				out.scalar *= px_per_pt;
			} else if(unit == "pc") {
				out.scalar *= px_per_pc;
			} else if(unit == "%") {
				out.unit = dom::UNIT_PERCENT;
			}
			// TODO: Relative units
		}
	}

}

void color(dom::Color& out, const char* val) {
	if(val == nullptr) return;

}

}
}
