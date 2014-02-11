#ifndef CSS_HPP
#define CSS_HPP

#include <string>

class CSS {
	public:
		~CSS();

		static CSS * from_source(const std::string &source);
		static CSS * from_file(const std::string &filename);
	private:
		CSS(const std::string &filename);

		std::string m_filename;
};

#endif
