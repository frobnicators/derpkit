#ifndef DERPKIT_TEST_WINDOW_HPP
#define DERPKIT_TEST_WINDOW_HPP

#include <derpkit/render/math.hpp>

namespace derpkit {

class Window {
	public:
		Window(int width, int height);
		~Window();

		void update();
		bool running() const { return m_running; }

		const render::mat3& screenortho() const { return m_screenortho; }
	private:
		bool m_running;
		render::mat3 m_screenortho;
};

};

#endif
