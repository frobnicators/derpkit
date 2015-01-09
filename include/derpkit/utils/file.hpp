#ifndef FROB_UTILS_FILE_HPP
#define FROB_UTILS_FILE_HPP

#include <string>

/**
 * Tell if file exists (and is readable)
 */
bool file_exists(const std::string& filename);

/**
 * Tell if directory exists
 */
bool dir_exists(const std::string& filename);

/**
 * Replace platform specific path separator with /
 */
std::string normalize_path(std::string path);

/**
 * Create a platform specific filename (inverse of normalize)
 */
std::string platform_path(std::string path);

/**
 * File basename.
 * "/path/to/file" -> "file"
 * filename must be normalized
 *
 * @param strip_ext If true, remove the filename extension as well.
 */
std::string file_basename(std::string filename, bool strip_ext=false);

/**
 * File dirname.
 * "/path/to/file" -> "/path/to"
 * filename must be normalized
 */
std::string file_dirname(std::string filename);

/**
 * Get file extension.
 */
std::string file_ext(std::string filename);

/**
 * Create directory
 * Path must be normalized
 */
bool create_directory(std::string path, bool recursive = false);

/**
 * Join paths together separated by '/'.
 */
std::string path_join(const std::string& a, const std::string& b);

/**
 * path_join wrappers for char*.
 */
std::string path_join(const char* a, const char* b);
std::string path_join(const std::string& a, const char* b);
std::string path_join(const char* a, const std::string& b);

/**
 * path_join wrapper for variadic arguments by recursively joining arguments last to first.
 */
template<typename T1, typename T2, typename... Args>
std::string path_join(const T1& a, const T2& b, Args... args){ return path_join(a, path_join(b, args...)); }

#ifdef ENABLE_DEBUG

class FileData {
public:
	FileData();
	FileData(char* data, size_t size);
	FileData(FileData&& other);
	~FileData();

	bool valid() const { return m_data != nullptr; }

	const char* data() const { return m_data; }
	size_t size() const { return m_size; }
private:
	char* m_data;
	size_t m_size;
};

FileData load_file(const std::string& filename);

#endif

#endif
