#include "rule.hpp"

namespace css {

static void print_indent(int levels) {
	for(int i=0; i<levels; ++i) {
		printf("\t");
	}
}

void Rule::print(int indent) const {
	print_indent(indent);
	printf("RULE (");
	for(const auto & sg : m_selectors) {
		for(const auto & s : sg) {
			printf(" %s=%s", s.type_as_string(), s.value.c_str());
		}
		printf(",");
	}
	print_indent(indent);
	printf(") {\n");
	for(const auto & p : m_properties) {
		print_indent(indent+1);
		p.print();
	}
	print_indent(indent);
	printf("}\n");
}

}
