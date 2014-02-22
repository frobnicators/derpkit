#ifndef CSS_HPP
#define CSS_HPP

#include <string>

#include "derpkit/export.hpp"
#include "cssrule.hpp"

class CSS {
	public:
		~CSS();

		DERPKIT_EXPORT static CSS * from_source(const std::string &source);
		DERPKIT_EXPORT static CSS * from_file(const std::string &filename);

		DERPKIT_EXPORT void print() const;
	private:
		CSS(const std::string &filename);

		void parse(struct ANTLR3_INPUT_STREAM_struct * input);
		void traverse(struct ANTLR3_BASE_TREE_struct * node);
		void parse_rule(struct ANTLR3_BASE_TREE_struct * node);

		std::string m_filename;
		std::vector<CSSRule> m_rules;
};

#endif
