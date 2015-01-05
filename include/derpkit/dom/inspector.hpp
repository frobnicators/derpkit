#ifndef DERPKIT_INSPECTOR_HPP
#define DERPKIT_INSPECTOR_HPP

#include <derpkit/dom/document.hpp>

namespace dom {

class InspectorImpl;

class DERPKIT_EXPORT Inspector {
	public:
		Inspector(int port=9222);
		~Inspector();

		static void initialize();
		static void cleanup();

		void update();

		void register_document(const Document& doc);
		void unregister_document(const Document& doc);
	private:
		InspectorImpl* m_pimpl;
};

}

#endif
