#ifndef DERPKIT_CSS_HPP
#define DERPKIT_CSS_HPP

#include <string>

#include <derpkit/css/rule.hpp>
#include <derpkit/css/expression.hpp>
#include <derpkit/dom/document.hpp>
#include <derpkit/export.hpp>

struct ANTLR3_INPUT_STREAM_struct;
struct ANTLR3_BASE_TREE_struct;
struct ANTLR3_BASE_TREE_struct;

namespace css {

class DERPKIT_EXPORT CSS {
	public:
		~CSS();

		static CSS * from_source(const std::string &source);
		static CSS * from_file(const std::string &filename);

		const std::string& filename() const {
			return m_filename;
		}

		const std::vector<Rule>& rules() const {
			return m_rules;
		}

		void print() const;

		void apply_to_document(dom::Document& doc) const;

		/* Indicate that this is a user agent style */
		bool user_agent_style;
	private:
		CSS(const std::string &filename);

		static void from_source_to_selector(const std::string& source, Selector& out);

		void parse(ANTLR3_INPUT_STREAM_struct * input);
		void traverse(ANTLR3_BASE_TREE_struct * node);
		void parse_rule(ANTLR3_BASE_TREE_struct * node);
		void parse_expr(ANTLR3_BASE_TREE_struct * node, Expression& expr);
		static void parse_selector(ANTLR3_BASE_TREE_struct* node, Selector& out);

		std::string m_filename;
		std::vector<Rule> m_rules;

		friend class Selector;
};

}

#endif
