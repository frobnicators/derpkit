#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/dom/inspector.hpp>
#include <derpkit/css/rule.hpp>
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
	static void getDocument(InspectorImpl* inspector, json_object* params, JsonResponse& response) {
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

	static void highlightNode(InspectorImpl* inspector, json_object* params, JsonResponse& response) {
	}
}

namespace CSS {

	static json_object* serialize_selector(const css::Selector& selector) {
		json_object* selector_obj = json_object_new_object();

		set(selector_obj, "value", selector.to_string().c_str());
		// TODO: range: SourceRange

		return selector_obj;
	}

	static json_object* serialize_properties(const std::vector<css::Property>& properties) {
		json_object* prop_ary = json_object_new_array();
		for(const auto& prop : properties) {
			json_object* prop_obj = json_object_new_object();
			set(prop_obj, "name", prop.property.c_str());
			set(prop_obj, "value", str_join(prop.expressions.begin(), prop.expressions.end(), ", ").c_str());
			add(prop_ary, prop_obj);
		}
		return prop_ary;
	}

	static json_object* serialize_rule(const css::Rule& rule) {
		json_object* rule_obj = json_object_new_object();
		//set(rule_obj, "styleSheetId", 0); // TODO!
		set(rule_obj, "origin", "regular"); // TODO!

		json_object* selectorList = json_object_new_object();
		json_object* selectors = json_object_new_array();
		for(const auto& selector : rule.selectors()) {
			add(selectors, serialize_selector(selector));
		}
		set(selectorList, "selectors", selectors);
		set(selectorList, "text", str_join(rule.selectors().begin(), rule.selectors().end(), ", ").c_str());

		set(rule_obj, "selectorList", selectorList);
		json_object* style = json_object_new_object();
		set(style, "cssProperties", serialize_properties(rule.properties()));

		json_object* shorthand = json_object_new_array();
		set(style, "shorthandEntries", shorthand); // TODO

		set(rule_obj, "style", style);

		return rule_obj;
	}

	typedef std::pair<const css::Rule*, std::vector<int>> RuleMatch;

	static json_object* serialize_rule_match(const RuleMatch& rm) {
		json_object* rm_obj = json_object_new_object();
		set(rm_obj, "rule", serialize_rule(*rm.first));
		json_object* matches = json_object_new_array();
		for(int i : rm.second) {
			add(matches, i);
		}
		set(rm_obj, "matchingSelectors", matches);

		return rm_obj;
	}


	static void getComputedStyleForNode(InspectorImpl* inspector, json_object* params, JsonResponse& response) {
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

	/*
	{
	   "name": "getMatchedStylesForNode",
	   "returns": [
		   { "name": "matchedCSSRules", "type": "array", "items": { "$ref": "RuleMatch" }, "optional": true, "description": "CSS rules matching this node, from all applicable stylesheets." },
		   { "name": "pseudoElements", "type": "array", "items": { "$ref": "PseudoIdMatches" }, "optional": true, "description": "Pseudo style matches for this node." },
		   { "name": "inherited", "type": "array", "items": { "$ref": "InheritedStyleEntry" }, "optional": true, "description": "A chain of inherited styles (from the immediate node parent up to the DOM tree root)." }
	   ],
	   "description": "Returns requested styles for a DOM node identified by <code>nodeId</code>."
   }
   */
	void getMatchedStylesForNode(InspectorImpl* inspector, json_object* params, JsonResponse& response) {
		Node* node = get_node(params);
		if(node) {
			bool excludePseudo = json_object_get_boolean(json_object_object_get(params, "excludePseudo"));
			bool excludeInherited = json_object_get_boolean(json_object_object_get(params, "excludeInherited"));

			json_object* matchedRules = json_object_new_array();
			for(const RuleMatch& match : node->matched_css_rules()) {
				add(matchedRules, serialize_rule_match(match));
			}
			response.set("matchedCSSRules", matchedRules);

			json_object* pseudoElements = json_object_new_array();
			json_object* inherited = json_object_new_array();

			if(!excludePseudo) {
				response.set("pseudoElements", pseudoElements);
			}
			if(!excludeInherited) {
				response.set("inherited", inherited);
			}


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
	{ "CSS.getMatchedStylesForNode", &json::CSS::getMatchedStylesForNode },
};

/**
 *
 *
 // From me:
 {"method":"CSS.styleSheetAdded","params":{"header":{"styleSheetId":"35","origin":"user","disabled":false,"sourceURL":"","title":"","frameId":"171.1","isInline":false,"startLine":0,"startColumn":0}}}


 */

// CSS Data:
/*

    {
                "id": "StyleSheetId",
                "type": "string"
            },
            {
                "id": "StyleSheetOrigin",
                "type": "string",
                "enum": ["injected", "user-agent", "inspector", "regular"],
                "description": "Stylesheet type: \"injected\" for stylesheets injected via extension, \"user-agent\" for user-agent stylesheets, \"inspector\" for stylesheets created by the inspector (i.e. those holding the \"via inspector\" rules), \"regular\" for regular stylesheets."
            },
            {
                "id": "PseudoIdMatches",
                "type": "object",
                "properties": [
                    { "name": "pseudoId", "type": "integer", "description": "Pseudo style identifier (see <code>enum PseudoId</code> in <code>RenderStyleConstants.h</code>)."},
                    { "name": "matches", "type": "array", "items": { "$ref": "RuleMatch" }, "description": "Matches of CSS rules applicable to the pseudo style."}
                ],
                "description": "CSS rule collection for a single pseudo style."
            },
            {
                "id": "InheritedStyleEntry",
                "type": "object",
                "properties": [
                    { "name": "inlineStyle", "$ref": "CSSStyle", "optional": true, "description": "The ancestor node's inline style, if any, in the style inheritance chain." },
                    { "name": "matchedCSSRules", "type": "array", "items": { "$ref": "RuleMatch" }, "description": "Matches of CSS rules matching the ancestor node in the style inheritance chain." }
                ],
                "description": "Inherited CSS rule collection from ancestor node."
            },
            {
                "id": "RuleMatch",
                "type": "object",
                "properties": [
                    { "name": "rule", "$ref": "CSSRule", "description": "CSS rule in the match." },
                    { "name": "matchingSelectors", "type": "array", "items": { "type": "integer" }, "description": "Matching selector indices in the rule's selectorList selectors (0-based)." }
                ],
                "description": "Match data for a CSS rule."
            },
            {
                "id": "Selector",
                "type": "object",
                "properties": [
                    { "name": "value", "type": "string", "description": "Selector text." },
                    { "name": "range", "$ref": "SourceRange", "optional": true, "description": "Selector range in the underlying resource (if available)." }
                ],
                "description": "Data for a simple selector (these are delimited by commas in a selector list)."
            },
            {
                "id": "SelectorList",
                "type": "object",
                "properties": [
                    { "name": "selectors", "type": "array", "items": { "$ref": "Selector" }, "description": "Selectors in the list." },
                    { "name": "text", "type": "string", "description": "Rule selector text." }
                ],
                "description": "Selector list data."
            },
            {
                "id": "CSSStyleSheetHeader",
                "type": "object",
                "properties": [
                    { "name": "styleSheetId", "$ref": "StyleSheetId", "description": "The stylesheet identifier."},
                    { "name": "frameId", "$ref": "Page.FrameId", "description": "Owner frame identifier."},
                    { "name": "sourceURL", "type": "string", "description": "Stylesheet resource URL."},
                    { "name": "sourceMapURL", "type": "string", "optional": true, "description": "URL of source map associated with the stylesheet (if any)." },
                    { "name": "origin", "$ref": "StyleSheetOrigin", "description": "Stylesheet origin."},
                    { "name": "title", "type": "string", "description": "Stylesheet title."},
                    { "name": "ownerNode", "$ref": "DOM.BackendNodeId", "optional": true, "description": "The backend id for the owner node of the stylesheet." },
                    { "name": "disabled", "type": "boolean", "description": "Denotes whether the stylesheet is disabled."},
                    { "name": "hasSourceURL", "type": "boolean", "optional": true, "description": "Whether the sourceURL field value comes from the sourceURL comment." },
                    { "name": "isInline", "type": "boolean", "description": "Whether this stylesheet is created for STYLE tag by parser. This flag is not set for document.written STYLE tags." },
                    { "name": "startLine", "type": "number", "description": "Line offset of the stylesheet within the resource (zero based)." },
                    { "name": "startColumn", "type": "number", "description": "Column offset of the stylesheet within the resource (zero based)." }
                ],
                "description": "CSS stylesheet metainformation."
            },
            {
                "id": "CSSRule",
                "type": "object",
                "properties": [
                    { "name": "styleSheetId", "$ref": "StyleSheetId", "optional": true, "description": "The css style sheet identifier (absent for user agent stylesheet and user-specified stylesheet rules) this rule came from." },
                    { "name": "selectorList", "$ref": "SelectorList", "description": "Rule selector data." },
                    { "name": "origin", "$ref": "StyleSheetOrigin", "description": "Parent stylesheet's origin."},
                    { "name": "style", "$ref": "CSSStyle", "description": "Associated style declaration." },
                    { "name": "media", "type": "array", "items": { "$ref": "CSSMedia" }, "optional": true, "description": "Media list array (for rules involving media queries). The array enumerates media queries starting with the innermost one, going outwards." }
                ],
                "description": "CSS rule representation."
            },
            {
                "id": "SourceRange",
                "type": "object",
                "properties": [
                    { "name": "startLine", "type": "integer", "description": "Start line of range." },
                    { "name": "startColumn", "type": "integer", "description": "Start column of range (inclusive)." },
                    { "name": "endLine", "type": "integer", "description": "End line of range" },
                    { "name": "endColumn", "type": "integer", "description": "End column of range (exclusive)." }
                ],
                "description": "Text range within a resource. All numbers are zero-based."
            },
            {
                "id": "ShorthandEntry",
                "type": "object",
                "properties": [
                    { "name": "name", "type": "string", "description": "Shorthand name." },
                    { "name": "value", "type": "string", "description": "Shorthand value." }
                ]
            },
            {
                "id": "CSSComputedStyleProperty",
                "type": "object",
                "properties": [
                    { "name": "name", "type": "string", "description": "Computed style property name." },
                    { "name": "value", "type": "string", "description": "Computed style property value." }
                ]
            },
            {
                "id": "CSSStyle",
                "type": "object",
                "properties": [
                    { "name": "styleSheetId", "$ref": "StyleSheetId", "optional": true, "description": "The css style sheet identifier (absent for user agent stylesheet and user-specified stylesheet rules) this rule came from." },
                    { "name": "cssProperties", "type": "array", "items": { "$ref": "CSSProperty" }, "description": "CSS properties in the style." },
                    { "name": "shorthandEntries", "type": "array", "items": { "$ref": "ShorthandEntry" }, "description": "Computed values for all shorthands found in the style." },
                    { "name": "cssText", "type": "string", "optional": true, "description": "Style declaration text (if available)." },
                    { "name": "range", "$ref": "SourceRange", "optional": true, "description": "Style declaration range in the enclosing stylesheet (if available)." }
                ],
                "description": "CSS style representation."
            },
            {
                "id": "CSSProperty",
                "type": "object",
                "properties": [
                    { "name": "name", "type": "string", "description": "The property name." },
                    { "name": "value", "type": "string", "description": "The property value." },
                    { "name": "important", "type": "boolean", "optional": true, "description": "Whether the property has \"!important\" annotation (implies <code>false</code> if absent)." },
                    { "name": "implicit", "type": "boolean", "optional": true, "description": "Whether the property is implicit (implies <code>false</code> if absent)." },
                    { "name": "text", "type": "string", "optional": true, "description": "The full property text as specified in the style." },
                    { "name": "parsedOk", "type": "boolean", "optional": true, "description": "Whether the property is understood by the browser (implies <code>true</code> if absent)." },
                    { "name": "disabled", "type": "boolean", "optional": true, "description": "Whether the property is disabled by the user (present for source-based properties only)." },
                    { "name": "range", "$ref": "SourceRange", "optional": true, "description": "The entire property range in the enclosing style declaration (if available)." }
                ],
                "description": "CSS property declaration data."
            },
            {
                "id": "CSSMedia",
                "type": "object",
                "properties": [
                    { "name": "text", "type": "string", "description": "Media query text." },
                    { "name": "source", "type": "string", "enum": ["mediaRule", "importRule", "linkedSheet", "inlineSheet"], "description": "Source of the media query: \"mediaRule\" if specified by a @media rule, \"importRule\" if specified by an @import rule, \"linkedSheet\" if specified by a \"media\" attribute in a linked stylesheet's LINK tag, \"inlineSheet\" if specified by a \"media\" attribute in an inline stylesheet's STYLE tag." },
                    { "name": "sourceURL", "type": "string", "optional": true, "description": "URL of the document containing the media query description." },
                    { "name": "range", "$ref": "SourceRange", "optional": true, "description": "The associated rule (@media or @import) header range in the enclosing stylesheet (if available)." },
                    { "name": "parentStyleSheetId", "$ref": "StyleSheetId", "optional": true, "description": "Identifier of the stylesheet containing this object (if exists)." },
                    { "name": "mediaList", "type": "array", "items": { "$ref": "MediaQuery" }, "optional": true, "hidden": true, "description": "Array of media queries." }
                ],
                "description": "CSS media rule descriptor."
            },
            {
                "id": "MediaQuery",
                "type": "object",
                "properties": [
                    { "name": "expressions", "type": "array", "items": { "$ref": "MediaQueryExpression" }, "description": "Array of media query expressions." },
                    { "name": "active", "type": "boolean", "description": "Whether the media query condition is satisfied." }
                ],
                "description": "Media query descriptor.",
                "hidden": true
            },
            {
                "id": "MediaQueryExpression",
                "type": "object",
                "properties": [
                    { "name": "value", "type": "number", "description": "Media query expression value."},
                    { "name": "unit", "type": "string", "description": "Media query expression units."},
                    { "name": "feature", "type": "string", "description": "Media query expression feature."},
                    { "name": "valueRange", "$ref": "SourceRange", "optional": true, "description": "The associated range of the value text in the enclosing stylesheet (if available)." },
                    { "name": "computedLength", "type": "number", "optional": true, "description": "Computed length of media query expression (if applicable)."}
                ],
                "description": "Media query expression descriptor.",
                "hidden": true
            },
            {
                "id": "PlatformFontUsage",
                "type": "object",
                "properties": [
                    { "name": "familyName", "type": "string", "description": "Font's family name reported by platform."},
                    { "name": "glyphCount", "type": "number", "description": "Amount of glyphs that were rendered with this font."}
                ],
                "description": "Information about amount of glyphs that were rendered with given font.",
                "hidden": true
            }
*/

}
