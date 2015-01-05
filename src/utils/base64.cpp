#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/utils/base64.hpp>
#include <sstream>

extern "C"
{
	#include <b64/cencode.h>
	#include <b64/cdecode.h>
}


namespace base64 {

	std::string encode(const char* data, size_t size) {
		std::stringstream ss;
		char* code = new char[size*2];
		base64_encodestate b64state;
		base64_init_encodestate(&b64state);

		int len = base64_encode_block(data, static_cast<int>(size), code, &b64state);
		ss.write(code, len);

		len = base64_encode_blockend(code, &b64state);
		ss.write(code, len);

		delete[] code;
		return ss.str();
	}

	std::string decode(const std::string& str) {
		char* plaintext = new char[str.length()+1];
		base64_decodestate b64state;
		base64_init_decodestate(&b64state);

		int len = base64_decode_block(str.c_str(), static_cast<int>(str.length()+1), plaintext, &b64state);

		std::string ret(plaintext, len);
		delete[] plaintext;
		return ret;
	}

};
