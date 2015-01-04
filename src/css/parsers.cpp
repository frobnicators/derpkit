#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/utils/string_utils.hpp>
#include "parsers.hpp"
#include "colors.hpp"
#include <string>

namespace css {
namespace parsers {

static uint8_t parse_color_val(const char* v, size_t count, int base) {
	return static_cast<uint8_t>(stoi(std::string(v, count), nullptr, base));
}

void display(dom::Display& out, const Expression* val) {
	if(val == nullptr || val->terms.empty()) return;

	const Term& term = val->terms[0];
	if(term.type != Term::TYPE_STRING) return;

	std::string v = lcase(term.value);

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

void length(dom::Length& out, const Expression* val) {
	if(val == nullptr || val->terms.empty()) return;

	const float px_per_in = 96.f;
	const float px_per_pt = 96.f / 72.f;
	const float px_per_pc = 12 * px_per_pt;
	const float px_per_cm = (1.f/2.54f) * px_per_in;
	const float px_per_mm = px_per_cm / 10.f;

	const Term& term = val->terms[0];

	std::string v = lcase(term.value);

	if(term.type == Term::TYPE_STRING && v == "auto") {
		out = {0, dom::UNIT_AUTO};
	} else if(term.type == Term::TYPE_NUMBER) {
		number(out.scalar, term);
		out.unit = dom::UNIT_PX;
		size_t unit_start = v.find_first_not_of("0123456789.");
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

void color(dom::Color& out, const Expression* val) {
	if(val == nullptr || val->terms.empty()) return;

	const Term& term = val->terms[0];

	if(term.type == Term::TYPE_HEXCOLOR) {
		const char* str = term.value.c_str();
		if(term.value.length() == 3) {
			uint8_t tmp = parse_color_val(str, 1, 16);
			out.r = tmp * 16 + tmp;
			tmp = parse_color_val(str+1, 1, 16);
			out.g = tmp * 16 + tmp;
			tmp = parse_color_val(str+2, 1, 16);
			out.b = tmp * 16 + tmp;
			out.a = 255;
		} else if(term.value.length() == 6) {
			out.r = parse_color_val(str, 2, 16);
			out.g = parse_color_val(str+2, 2, 16);
			out.b = parse_color_val(str+4, 2, 16);
			out.a = 255;
		}
	} else if(term.type == Term::TYPE_STRING) {
		std::string str = lcase(term.value);
		if(str == "transparent") {
			out.r = 0;
			out.g = 0;
			out.b = 0;
			out.a = 0;
		} else {
			auto it = webcolors.find(str);
			if(it != webcolors.end()) {
				out.r = it->second.r;
				out.g = it->second.g;
				out.b = it->second.b;
				out.a = 0;
			} else {
				printf("[CSS] Warning: Unknown color %s\n", str.c_str());
			}
		}
	} else if(term.type == Term::TYPE_FUNCTION) {
		printf("TODO: func color %s\n", term.value.c_str());
	} else {
		printf("[CSS] Error: Invalid color type (for %s)\n", term.value.c_str());
	}
}

void number(float& out, const Term& val) {
	if(val.type != Term::TYPE_NUMBER) return;

	sscanf(val.value.c_str(),"%f", &out);
}

}
}
