#ifndef CSS_HPP
#define CSS_HPP

#include <string>

#include "derpkit/export.hpp"
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

		const std::string& filename() const {
			return m_filename;
		}

		const std::vector<Rule>& rules() const {
			return m_rules;
		}

		DERPKIT_EXPORT void print() const;
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
