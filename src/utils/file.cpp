#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/utils/file.hpp>
#include <algorithm>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STAT
#include <sys/types.h>
#include <sys/stat.h>
#endif

bool file_exists(const std::string& fullpath){
#ifdef HAVE_ACCESS
	return access(platform_path(fullpath).c_str(), R_OK) == 0;
#elif defined(WIN32)
	const DWORD dwAttrib = GetFileAttributes(platform_path(fullpath).c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
	        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
#error file_exists is not defined for this platform.
#endif
}

std::string normalize_path(std::string path) {
	std::replace(path.begin(), path.end(), __PATH_SEPARATOR__, '/');
	return path;
}

std::string platform_path(std::string path) {
	std::replace(path.begin(), path.end(), '/', __PATH_SEPARATOR__);
	return path;
}

bool dir_exists(const std::string& fullpath){
#ifdef HAVE_STAT
	struct stat st;
	stat(platform_path(fullpath).c_str(), &st);
	return S_ISDIR(st.st_mode);
#elif defined(WIN32)
	const DWORD dwAttrib = GetFileAttributes(platform_path(fullpath).c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
	        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
#error dir_exists is not defined for this platform.
#endif
}

std::string file_basename(std::string filename, bool strip_ext){
	using std::string;
	const size_t last_slash = filename.find_last_of("/");

	/* strip path */
	if ( last_slash != string::npos ){
		filename = filename.substr(last_slash+1);
	}

	/* strip extension */
	if ( strip_ext ){
		const size_t last_dot = filename.find_last_of(".");
		if ( last_dot != string::npos && last_dot != 0 ){
			filename = filename.substr(0, last_dot);
		}
	}

	return filename;
}

std::string file_dirname(std::string filename){
	using std::string;
	const size_t last_slash = filename.find_last_of("/");

	if ( last_slash == string::npos ){
		return "./";
	}

	return filename.substr(0, last_slash+1);
}

std::string file_ext(std::string filename){
	using std::string;
	const std::string basename = file_basename(filename);
	const size_t last_dot = basename.find_last_of(".");

	/* no filename was present, only directory */
	if ( basename == "" ){
		return "";
	}

	/* no extension */
	if ( last_dot == string::npos || last_dot == 0 ){
		return "";
	}

	return basename.substr(last_dot+1);
}

bool create_directory(std::string path, bool recursive) {
	if (dir_exists(path)) return true;

	if (path.back() == '/') {
		path = path.substr(0, path.size() - 1);
	}

	if (recursive) {
		size_t last_separator = path.find_last_of('/');
		if (last_separator != std::string::npos)
		{
			if (!create_directory(path.substr(0, last_separator), true)) return false;
		}
	}

#ifdef WIN32
	return (CreateDirectory(platform_path(path).c_str(), NULL) == TRUE);
#else
	return (mkdir(platform_path(path).c_str(), 755) != -1);
#endif
}

std::string path_join(const std::string& a, const std::string& b){
	std::string path;

	if ( a == "" ){
		path = ".";
	} else if ( a == "/" ){
		path = "";
	} else {
		path = a;
	}

	/* ensure path delimiter */
	if ( b[0] != '/' ){
		path += '/';
	}

	path += b;

	return path;
}

std::string path_join(const char* a, const char* b){
	return path_join(std::string(a), std::string(b));
}

std::string path_join(const std::string& a, const char* b){
	return path_join(a, std::string(b));
}

std::string path_join(const char* a, const std::string& b){
	return path_join(std::string(a), b);
}
