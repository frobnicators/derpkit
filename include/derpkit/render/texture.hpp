#ifndef DERPKIT_TEXTURE_HPP
#define DERPKIT_TEXTURE_HPP

#include <string>

namespace derpkit {
namespace render {

namespace impl {
	struct Texture2D;
};

class DERPKIT_EXPORT Texture {
	public:
		Texture(const std::string& path);
		~Texture();

		void bind(int unit=0);
		void unbind();
	private:
		impl::Texture2D* m_impl;
		int m_unit;
};

}
}

#endif
