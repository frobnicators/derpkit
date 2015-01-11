#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "render/impl.hpp"
#include "gl.hpp"

#include <derpkit/utils/file.hpp>
#include <derpkit/utils/logging.hpp>
#include <derpkit/utils/string_utils.hpp>

#include <jpeglib.h>
#include <setjmp.h>
#include <cstring>

namespace derpkit {
namespace render {

namespace impl {

struct Texture2D {
	GLuint resource;
};

struct ImageData {
	unsigned char* data;
	ivec2 size;
	GLint internal_format;
	GLenum format;
};

static bool load_jpeg(const std::string& path, ImageData& img);

Texture2D* empty_texture() {
	Texture2D* texture = new Texture2D();

	glGenTextures(1, &texture->resource);
	glBindTexture(GL_TEXTURE_2D, texture->resource);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);   /** @todo add option for filter */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

Texture2D* load_texture(const std::string& path) {
	std::string ext = lcase(file_ext(path));
	ImageData img;
	if(ext == "jpg" || ext == "jpeg") {
		if(!load_jpeg(path, img)) {
			Logging::error("Could not load image %s\n", path.c_str());
			return nullptr;
		}
	} else {
		Logging::fatal("loader for file ext %s not implemented\n", ext.c_str());
	}

	Texture2D* texture = new Texture2D();

	glGenTextures(1, &texture->resource);
	glBindTexture(GL_TEXTURE_2D, texture->resource);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

	glTexImage2D(GL_TEXTURE_2D, 0, img.internal_format, img.size.x, img.size.y, 0, img.format, GL_UNSIGNED_BYTE, img.data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void free_texture(Texture2D* texture) {
	if(texture == nullptr) return;
	glDeleteTextures(1, &texture->resource);
	delete texture;
}

void texture_upload(Texture2D* texture, unsigned char* pixels, ivec2 size){
	glBindTexture(GL_TEXTURE_2D, texture->resource);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void bind_texture(Texture2D* tex, int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, tex->resource);
}

void unbind_texture(int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// JPEG error handling
struct derpkit_jpeg_error_mgr {
	struct jpeg_error_mgr pub;    /* "public" fields */

	jmp_buf setjmp_buffer;        /* for return to caller */
};

void jpeg_error_exit (j_common_ptr cinfo)
{
	derpkit_jpeg_error_mgr* err = (derpkit_jpeg_error_mgr*) cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(err->setjmp_buffer, 1);
}


static bool load_jpeg(const std::string& path, ImageData& img) {
	FILE * infile = nullptr;
	// TODO: Use correct file loading system

	jpeg_decompress_struct cinfo;
	derpkit_jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = jpeg_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 *      * We need to clean up the JPEG object, close the input file, and return.
		 *           */
		jpeg_destroy_decompress(&cinfo);
		if(infile != nullptr) fclose(infile);
		return false;
	}

	std::string filename = srcdir + path;

	// TODO: Use custom source
	if ((infile = fopen(filename.c_str(), "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", path.c_str());
		return false;
	}

	printf("opening %s\n", filename.c_str());

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);

	jpeg_read_header(&cinfo, 1);

	cinfo.out_color_space = JCS_RGB;

	jpeg_start_decompress(&cinfo);

	// TODO: Check for grayscale and other fun stuff?
	// read cinfo.output_components
	img.internal_format = GL_RGB8;
	img.format = GL_RGB;
	img.size = ivec2(cinfo.output_width, cinfo.output_height);

	int row_pitch = img.size.x * cinfo.output_components;

	img.data = new unsigned char[row_pitch * img.size.y];

	JSAMPARRAY work_buffer;
	work_buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_pitch, 1);

	while(cinfo.output_scanline < cinfo.output_height) {
		int pos = cinfo.output_scanline;
		jpeg_read_scanlines(&cinfo,work_buffer,1);
		memcpy(img.data + pos * row_pitch, work_buffer[0], row_pitch);
	}


	jpeg_finish_decompress(&cinfo);

	return true;
}

}

}
}
