#ifndef DERPKIT_CSS_HPP
#define DERPKIT_CSS_HPP

#include <string>

#include "derpkit/export.hpp"
#include "dom/node.hpp"
#include "rule.hpp"

struct ANTLR3_INPUT_STREAM_struct;
struct ANTLR3_BASE_TREE_struct;
struct ANTLR3_BASE_TREE_struct;

namespace css {

class CSS {
	public:
		~CSS();

		DERPKIT_EXPORT static CSS * from_source(const std::string &source);
		DERPKIT_EXPORT static CSS * from_file(const std::string &filename);

		DERPKIT_EXPORT const std::string& filename() const {
			return m_filename;
		}

		DERPKIT_EXPORT const std::vector<Rule>& rules() const {
			return m_rules;
		}

		DERPKIT_EXPORT void print() const;

		DERPKIT_EXPORT void apply_to_tree(dom::Node root);
	private:
		CSS(const std::string &filename);

		void parse(ANTLR3_INPUT_STREAM_struct * input);
		void traverse(ANTLR3_BASE_TREE_struct * node);
		void parse_rule(ANTLR3_BASE_TREE_struct * node);

		std::string m_filename;
		std::vector<Rule> m_rules;
};

}

#endif
