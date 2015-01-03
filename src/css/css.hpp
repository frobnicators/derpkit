#ifndef DERPKIT_CSS_HPP
#define DERPKIT_CSS_HPP

#include <string>

#include <derpkit/export.hpp>
#include <derpkit/dom/document.hpp>
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

		DERPKIT_EXPORT void apply_to_document(dom::Document& doc);
	private:
		CSS(const std::string &filename);

		static void from_source_to_selector(const std::string& source, Selector& out);

		void parse(ANTLR3_INPUT_STREAM_struct * input);
		void traverse(ANTLR3_BASE_TREE_struct * node);
		void parse_rule(ANTLR3_BASE_TREE_struct * node);
		static void parse_selector(ANTLR3_BASE_TREE_struct* node, Selector& out);

		std::string m_filename;
		std::vector<Rule> m_rules;

		friend class Selector;
};

}

#endif
