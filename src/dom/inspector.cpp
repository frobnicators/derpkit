#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/dom/inspector.hpp>
#include <derpkit/utils/network/websocket.hpp>
#include <derpkit/utils/string_utils.hpp>
#include <derpkit/utils/logging.hpp>
#include <json/json.h>

#include <cstdlib>

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
class JsonCall {
	public:
		JsonCall();
		JsonCall(const JsonCall&) = delete;
		virtual ~JsonCall();

		void set(const char* key, const char* val);
		void set(const char* key, int val);
		void set(const char* key, float val);
		void set(const char* key, bool val);
		void set(const char* key, json_object* obj);

		const char* str() const;
	protected:
		json_object* m_json;
		json_object* m_data;

		void create_data();

		virtual const char* data_name() const = 0;
};

class JsonRequest : public JsonCall {
	public:
		JsonRequest(const std::string& method_name);
	protected:
		virtual const char* data_name() const { return "params"; }
};

class JsonResponse : public JsonCall {
	public:
		JsonResponse(int id);
	protected:
		virtual const char* data_name() const { return "result"; }
};

class InspectorImpl {
	public:
		InspectorImpl(int port);
		~InspectorImpl();

		void update();
		void set_document(const dom::Document* doc);

		void send(WebSocket::Client* client, const JsonCall* call);

		const dom::Document* m_doc;
		WebSocket m_ws;
		int m_port;

		/* callbacks */
		void message(WebSocket::Client* client, const std::string& data);
		void connected(WebSocket::Client* client);

		void on_http(WebSocket::Client* client, const std::map<std::string,std::string>& headers, const std::string&);

		static std::map<std::string,std::function<void(InspectorImpl*,json_object*,JsonResponse& response)>> s_method_mapping;
};

JsonRequest::JsonRequest(const std::string& method_name) {
	json_object_object_add(m_json, "method", json_object_new_string(method_name.c_str()));
}

JsonResponse::JsonResponse(int id) {
	json_object_object_add(m_json, "id", json_object_new_int(id));
	create_data(); // Always send result object
}

JsonCall::JsonCall() : m_data(nullptr) {
	m_json = json_object_new_object();
}

JsonCall::~JsonCall() {
	json_object_put(m_json);
}

const char* JsonCall::str() const {
	return json_object_to_json_string(m_json);
}

void JsonCall::create_data() {
	if(m_data == nullptr) {
		m_data = json_object_new_object();
		json_object_object_add(m_json, data_name(), m_data);
	}
}

void JsonCall::set(const char* key, const char* val) {
	create_data();
	json_object_object_add(m_data, key, json_object_new_string(val));
}

void JsonCall::set(const char* key, int val) {
	create_data();
	json_object_object_add(m_data, key, json_object_new_int(val));
}

void JsonCall::set(const char* key, float val) {
	create_data();
	json_object_object_add(m_data, key, json_object_new_double(val));
}

void JsonCall::set(const char* key, bool val) {
	create_data();
	json_object_object_add(m_data, key, json_object_new_boolean(val));
}

void JsonCall::set(const char* key, json_object* obj) {
	create_data();
	json_object_object_add(m_data, key, obj);
}

/* Implementation */

InspectorImpl::InspectorImpl(int port) : m_doc(nullptr), m_ws(port), m_port(port) {
	using namespace std::placeholders;
	m_ws.set_connected_callback(std::bind(&InspectorImpl::connected, this, _1));
	m_ws.set_text_data_callback(std::bind(&InspectorImpl::message, this, _1, _2));
	m_ws.set_http_callback(std::bind(&InspectorImpl::on_http, this, _1, _2, _3));
	m_ws.listen();
}

InspectorImpl::~InspectorImpl() {
}

void InspectorImpl::update() {
	m_ws.update();
}

void InspectorImpl::set_document(const dom::Document* doc) {
	m_doc = doc;
}

void InspectorImpl::message(WebSocket::Client* client, const std::string& data) {
	printf("Got message: %s\n", data.c_str());

	json_object* json = json_tokener_parse(data.c_str());
	if(json != nullptr) {
		json_object* idobj = json_object_object_get(json, "id");
		json_object* methodobj = json_object_object_get(json, "method");
		if(idobj == nullptr) {
			Logging::error("[Inspector] Id is null\n");
		} else if(methodobj == nullptr) {
			Logging::error("[Inspector] Method is null\n");
		} else {
			int id = json_object_get_int(idobj);
			std::string method(json_object_get_string(methodobj));
			JsonResponse response(id);
			auto it = s_method_mapping.find(method);
			if(it != s_method_mapping.end()) {
				json_object* params = json_object_object_get(json, "params");
				it->second(this, params, response);
			}
			send(client, &response);
		}
	} else {
		Logging::error("[Inspector] Invalid json: %s\n", data.c_str());
	}
	json_object_put(json);
}

void InspectorImpl::connected(WebSocket::Client* client) {
}

static void serve_file(WebSocket& ws, WebSocket::Client* client, std::string filename) {
	size_t start = filename.find_first_of("/")+1;
	size_t end = filename.find_first_of("?");
	if(end == std::string::npos) end = filename.length();
	int len = end - start;
	if(start > end || len < 0) {
		Logging::error("Invalid filename: %s\n", filename.c_str());
		return;
	}

	filename = filename.substr(start, len);

	std::string _filename = srcdir "/data/inspector/" + filename;
	FILE* file = fopen(_filename.c_str(), "rb");
	if(file) {
		size_t filelen = file_size(file);
		char buffer[256];
		sprintf(buffer, "HTTP/1.1 200 OK\r\n"
			"Connection: close\r\n"
			"Content-Length: %lu\r\n"
			"\r\n", filelen);

		ws.send_raw(client, buffer, strlen(buffer));

		char* tmp = (char*)malloc(filelen);
		if(fread(tmp, 1, filelen, file) == filelen) {
			ws.send_raw(client, tmp, filelen);
		} else {
			Logging::error("[Inspector] Failed to read all bytes from file %s.\n", _filename.c_str());
		}

		free(tmp);
	} else {
		Logging::error("[Inspector] Failed to open file %s\n", _filename.c_str());
	}
}

void InspectorImpl::on_http(WebSocket::Client* client, const std::map<std::string,std::string>& headers, const std::string& request) {
	std::vector<std::string> get = str_split(request, " ");
	if(get[0] == "GET" && get[2] == "HTTP/1.1")  {
		if(get[1] == "/") {
			char buffer[256];
			// TODO: replace localhost
			sprintf(buffer, "HTTP/1.1 307 Temprary Redirect\r\n"
				"Location: http://localhost:%d/devtools.html?ws=localhost:%d/\r\n"
				"Connection: close\r\n"
				"\r\n", m_port, m_port);

			m_ws.send_raw(client, buffer, strlen(buffer));
		} else {
			serve_file(m_ws, client, get[1]);
		}
	} else {
		Logging::error("[Inspector] Not a get request: %s\n", request.c_str());
	}
	m_ws.close(client);
}

void InspectorImpl::send(WebSocket::Client* client, const JsonCall* call) {
	m_ws.send_text(client, call->str());
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

void Inspector::set_document(const dom::Document* doc) {
	m_pimpl->set_document(doc);
}
void Inspector::initialize() {
	Socket::initialize();
}
void Inspector::cleanup() {
	Socket::cleanup();
}

/* Methods */

/* method mapping */

std::map<std::string,std::function<void(InspectorImpl*,json_object*,JsonResponse& response)>> InspectorImpl::s_method_mapping = {
};

}
