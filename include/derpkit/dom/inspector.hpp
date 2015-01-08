#ifndef DERPKIT_INSPECTOR_HPP
#define DERPKIT_INSPECTOR_HPP

#include <derpkit/dom/document.hpp>
#include <derpkit/utils/logging.hpp>

namespace dom {

class InspectorImpl;

class DERPKIT_EXPORT Inspector {
	public:
		Inspector(int port=9222);
		~Inspector();

		static void initialize();
		static void cleanup();

		void log(const std::string& msg, Logging::Severity severity = Logging::INFO);

		void update();

		void set_document(Document* doc);

		void document_update();
	private:
		InspectorImpl* m_pimpl;
};

}

#endif
