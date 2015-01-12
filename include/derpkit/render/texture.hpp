#ifndef DERPKIT_TEXTURE_HPP
#define DERPKIT_TEXTURE_HPP

#include <derpkit/render/math.hpp>
#include <string>

namespace derpkit {
namespace render {

enum TextureFormat {
	TextureFormat_RED,
	TextureFormat_RGB,
	TextureFormat_RGBA,
};

namespace impl {
	struct Texture2D;
};

class DERPKIT_EXPORT Texture {
	public:
		Texture();
		Texture(const std::string& path);
		~Texture();

		void upload(unsigned char* pixels, ivec2 size, TextureFormat format = TextureFormat_RGB, int unpack_alignment=4);

		void bind(int unit=0) const;
		void unbind() const;
	private:
		impl::Texture2D* m_impl;
		mutable int m_unit;

		friend class RenderTarget;
		Texture(impl::Texture2D* impl);
		void set_impl(impl::Texture2D* impl);
};

}
}

#endif
