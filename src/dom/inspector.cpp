#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/dom/inspector.hpp>
#include <derpkit/utils/network/websocket.hpp>
#include <derpkit/utils/string_utils.hpp>
#include <derpkit/utils/logging.hpp>
#include <json.h>

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
		void log(const std::string& msg, const char* severity);

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
			} else {
				printf("Got message: %s\n", data.c_str());
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

void Inspector::log(const std::string& msg, Logging::Severity severity) {
	static const char* sstr[] = {
		"fatal",
		"error",
		"warning",
		"info",
		"verbose",
		"debug"
	};

	m_pimpl->log(msg, sstr[severity]);

}

/* helpers */

namespace json {

__UNUSED__ static void set(json_object* obj, const char* key, const char* val) {
	json_object_object_add(obj, key, json_object_new_string(val));
}
__UNUSED__ static void set(json_object* obj, const char* key, int val) {
	json_object_object_add(obj, key, json_object_new_int(val));
}
__UNUSED__ static void set(json_object* obj, const char* key, int64_t val) {
	json_object_object_add(obj, key, json_object_new_int64(val));
}
__UNUSED__ static void set(json_object* obj, const char* key, float val) {
	json_object_object_add(obj, key, json_object_new_double(val));
}
__UNUSED__ static void set(json_object* obj, const char* key, bool val) {
	json_object_object_add(obj, key, json_object_new_boolean(val));
}
__UNUSED__ static void set(json_object* obj, const char* key, json_object* val) {
	json_object_object_add(obj, key, val);
}

__UNUSED__ static void add(json_object* obj, const char* val) {
	json_object_array_add(obj, json_object_new_string(val));
}
__UNUSED__ static void add(json_object* obj, int val) {
	json_object_array_add(obj, json_object_new_int(val));
}
__UNUSED__ static void add(json_object* obj, int64_t val) {
	json_object_array_add(obj, json_object_new_int64(val));
}
__UNUSED__ static void add(json_object* obj, float val) {
	json_object_array_add(obj, json_object_new_double(val));
}
__UNUSED__ static void add(json_object* obj, bool val) {
	json_object_array_add(obj, json_object_new_boolean(val));
}
__UNUSED__ static void add(json_object* obj, json_object* val) {
	json_object_array_add(obj, val);
}


// NodeTypes, copy pasted from webkit
const int      ELEMENT_NODE                   = 1;
const int      ATTRIBUTE_NODE                 = 2;
const int      TEXT_NODE                      = 3;
const int      CDATA_SECTION_NODE             = 4;
const int      ENTITY_REFERENCE_NODE          = 5;
const int      ENTITY_NODE                    = 6;
const int      PROCESSING_INSTRUCTION_NODE    = 7;
const int      COMMENT_NODE                   = 8;
const int      DOCUMENT_NODE                  = 9;
const int      DOCUMENT_TYPE_NODE             = 10;
const int      DOCUMENT_FRAGMENT_NODE         = 11;
const int      NOTATION_NODE                  = 12;

json_object* serialize_node(dom::Node node, bool recurse) {
	int type = ELEMENT_NODE;
	if(strlen(node.tag_name()) == 0) {
		type = TEXT_NODE;
	}

	json_object* obj = json_object_new_object();
	set(obj, "nodeId", (int64_t)node.get_internal_id());
	set(obj, "nodeType", type);
	set(obj, "nodeName", node.tag_name());
	set(obj, "localName", node.tag_name()); // ???
	set(obj, "nodeValue", node.text_content());

	json_object* attribs = json_object_new_array();
	for(auto& attr : node.attributes()) {
		add(attribs, attr.first.c_str());
		add(attribs, attr.second.c_str());
	}

	set(obj, "attributes", attribs);

	set(obj, "childNodeCount", (int)node.children_count());

	if(recurse) {
		json_object* children_ary = json_object_new_array();
		std::vector<Node> children = node.children();
		for(auto& child : children) {
			add(children_ary, serialize_node(child, true));
		}

		set(obj, "children", children_ary);
	}

	return obj;
}

Node* get_node(json_object* params) {
	json_object* objid = json_object_object_get(params, "nodeId");
	if(objid != nullptr) {
		Node node = Node::from_internal_id(json_object_get_int64(objid));
		return new Node(node);
	} else {
		Logging::error("[Inspector] Missing param: nodeId\n");
		return nullptr;
	}
}

/* Methods */

namespace DOM {
	void getDocument(InspectorImpl* inspector, json_object* params, JsonResponse& response) {
		if(inspector->m_doc != nullptr) {
			json_object* obj = json_object_new_object();
			set(obj, "nodeId", 1);
			set(obj, "nodeType", DOCUMENT_NODE);
			set(obj, "nodeName", "#document");
			set(obj, "documentUrl", "http://localhost"); // TODO
			set(obj, "baseUrl", "http://localhost"); // TODO
			set(obj, "childNodeCount", 2);

			json_object* children = json_object_new_array();

			// Set type to html
			json_object* type = json_object_new_object();
			set(type, "nodeId", 0); // whatever
			set(type, "nodeType", DOCUMENT_TYPE_NODE);
			set(type, "nodeName", "html");
			json_object_array_add(children, type);

			json_object_array_add(children, serialize_node(inspector->m_doc->root(), true));

			set(obj, "children", children);

			response.set("root", obj);
		} else {
			Logging::error("[Inspector] No document set\n");
		}
	}

	void highlightNode(InspectorImpl* inspector, json_object* params, JsonResponse& response) {
	}
}

namespace CSS {
	void getComputedStyleForNode(InspectorImpl* inspector, json_object* params, JsonResponse& response) {
		Node* node = get_node(params);
		if(node) {
			json_object* computedStyle = json_object_new_array();
			for(auto& prop : node->css_properties()) {
				json_object* p = json_object_new_object();
				set(p, "name", prop.first.c_str());
				set(p, "value", prop.second.to_string().c_str());

				add(computedStyle, p);
			}

			response.set("computedStyle", computedStyle);
			delete node;
		}
	}

}

}

void InspectorImpl::log(const std::string& msg, const char* severity) {
	JsonRequest request("Console.messageAdded");
	json_object* msgobj = json_object_new_object();
	json::set(msgobj, "level", severity);
	json::set(msgobj, "source", "other");
	json::set(msgobj, "type", "log");
	json::set(msgobj, "text", msg.c_str());

	request.set("message", msgobj);

	send(nullptr, &request);
}


/* method mapping */

std::map<std::string,std::function<void(InspectorImpl*,json_object*,JsonResponse& response)>> InspectorImpl::s_method_mapping = {
	{ "DOM.getDocument", &json::DOM::getDocument },
	{ "DOM.highlightNode", &json::DOM::highlightNode },
	{ "CSS.getComputedStyleForNode", &json::CSS::getComputedStyleForNode },
};

/**
 *
 *
 // From me:
 {"method":"CSS.styleSheetAdded","params":{"header":{"styleSheetId":"35","origin":"user","disabled":false,"sourceURL":"","title":"","frameId":"171.1","isInline":false,"startLine":0,"startColumn":0}}}


 */

}
