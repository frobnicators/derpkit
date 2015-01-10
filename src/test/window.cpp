#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "window.hpp"
#include "render/impl.hpp"

#include <SDL/SDL.h>
#include <GL/glew.h>

namespace derpkit {

Window::Window(int width, int height) : m_running(true) {
	// Initialize SDL

	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ){
		printf("Failed to initalize SDL: %s\n", SDL_GetError());
		abort();
	}

	SDL_SetVideoMode(1280, 720, 0, SDL_OPENGL|SDL_DOUBLEBUF);
	SDL_WM_SetCaption("derpKit Test", NULL);

	// Load glew
	GLenum ret;
	if ( (ret=glewInit()) != GLEW_OK ){
		printf("Failed to initialize GLEW: %s\n", glewGetErrorString(ret));
		abort();
	}

	m_screenortho = render::ortho(render::ivec2(width, height));

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	render::impl::initialize(); // TMP, move to library init
}

Window::~Window() {
	render::impl::cleanup(); // TMP, move to library cleanup
	SDL_Quit();
}

void Window::update() {
	SDL_Event event;
	while ( SDL_PollEvent(&event) ){
		switch ( event.type ){
		case SDL_QUIT:
			m_running = false;
			break;

		case SDL_KEYDOWN:
			if ( event.key.keysym.sym == SDLK_q && event.key.keysym.mod & KMOD_CTRL ){
				m_running = false;
			}
		}
	}

	SDL_GL_SwapBuffers();
}

}
