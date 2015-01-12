#ifndef DERPKIT_TEST_WINDOW_HPP
#define DERPKIT_TEST_WINDOW_HPP

#include <derpkit/render/math.hpp>
#include <derpkit/render/rendertarget.hpp>

namespace derpkit {

namespace render {
	class RenderTarget;
	class Texture;
}

class Window {
	public:
		Window(int width, int height);
		~Window();

		void update();
		bool running() const { return m_running; }

		const render::mat3& screenortho() const { return m_screenortho; }

		void blit(const render::RenderTarget* rt);
	private:
		render::ivec2 m_size;
		bool m_running;
		render::mat3 m_screenortho;
};

};

#endif
