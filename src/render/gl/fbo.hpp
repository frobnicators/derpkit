#ifndef FBO_H
#define FBO_H

#include "demo.h"

#ifndef ENABLE_FBOS
#error "Fbos must be enabled in demo.h before fbo.h is included"
#endif

#include "klister.h"

struct fbo_t {
	GLuint fbo;
	GLuint texture;
};

void create_fbo(int w, int h, GLenum format, struct fbo_t * fbo);

#endif