#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/dom/inspector.hpp>
#include <derpkit/utils/network/websocket.hpp>
#include <derpkit/utils/string_utils.hpp>
#include <derpkit/utils/logging.hpp>
#include <json/json.h>

#include <cstdlib>

#include <set>

namespace dom {

static long file_size(FILE* fp){
	const long cur = ftell(fp);
	fseek (fp , 0 , SEEK_END);
	const long bytes = ftell(fp);
	fseek(fp, cur, SEEK_SET);

	if ( bytes == -1 ){
		Logging::fatal("ftell(%d) failed: %s\n", fileno(fp), strerror(errno));
	}

	return bytes;
}

class InspectorImpl {
	public:
		InspectorImpl(int port);
		~InspectorImpl();

		void update();
		void register_document(const dom::Document& doc);
		void unregister_document(const dom::Document& doc);
	private:
		//std::set<const dom::Document&> m_documents;
		WebSocket m_ws;

		/* callbacks */
		void message(const std::string& data);
		void connected();

		void on_http(const std::map<std::string,std::string>& headers, const std::vector<std::string>&);
};

/* Implementation */

InspectorImpl::InspectorImpl(int port) : m_ws(port) {
	using namespace std::placeholders;
	m_ws.set_connected_callback(std::bind(&InspectorImpl::connected, this));
	m_ws.set_text_data_callback(std::bind(&InspectorImpl::message, this, _1));
	m_ws.set_http_callback(std::bind(&InspectorImpl::on_http, this, _1, _2));
	m_ws.listen();
}

InspectorImpl::~InspectorImpl() {
	m_ws.close();
}

void InspectorImpl::update() {
	m_ws.update();
}

void InspectorImpl::register_document(const dom::Document& doc) {
}

void InspectorImpl::unregister_document(const dom::Document& doc) {
}

void InspectorImpl::message(const std::string& data) {
	printf("Got message: %s\n", data.c_str());
}

void InspectorImpl::connected() {
	printf("Connected!\n");
}

static void serve_file(WebSocket& ws, std::string filename) {
	// TOTALY FUCKING UNSAFE
	if(filename == "/") filename = "/index.html";
	std::string _filename = srcdir "/data/inspector" + filename;
	FILE* file = fopen(_filename.c_str(), "rb");
	if(file) {
		size_t filelen = file_size(file);
		char buffer[256];
		sprintf(buffer, "HTTP/1.1 200 OK\r\n"
			"Connection: close\r\n"
			"Content-Length: %lu\r\n"
			"\r\n", filelen);

		ws.send_raw(buffer, strlen(buffer));

		char* tmp = (char*)malloc(filelen);
		if(fread(tmp, 1, filelen, file) == filelen) {
			ws.send_raw(tmp, filelen);
		} else {
			Logging::error("[Inspector] Failed to read all bytes from file %s.\n", _filename.c_str());
		}

		free(tmp);
	} else {
		Logging::error("[Inspector] Failed to open file %s\n", _filename.c_str());
	}
}

void InspectorImpl::on_http(const std::map<std::string,std::string>& headers, const std::vector<std::string>& data) {
	if(data.size() > 0) {
		std::vector<std::string> get = str_split(lcase(data[0]), " ");
		if(get[0] == "get" && get[2] == "http/1.1")  {
			serve_file(m_ws, get[1]);
		} else {
			Logging::error("[Inspector] Invalid http: %s\n", data[0].c_str());
		}
	} else {
		Logging::error("[Inspector] No http data\n");
	}
	m_ws.close();
}

/* Wrapper */

Inspector::Inspector(int port) {
	m_pimpl = new InspectorImpl(port);
}

Inspector::~Inspector() {
	delete m_pimpl;
}

void Inspector::update() {
	m_pimpl->update();
}

void Inspector::register_document(const dom::Document& doc) {
	m_pimpl->register_document(doc);
}
void Inspector::unregister_document(const dom::Document& doc) {
	m_pimpl->register_document(doc);
}

void Inspector::initialize() {
	Socket::initialize();
}
void Inspector::cleanup() {
	Socket::cleanup();
}

}
