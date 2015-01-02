#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/dom/document.hpp>

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace dom;

class DOMTest: public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(DOMTest);
		CPPUNIT_TEST(test_find);
	CPPUNIT_TEST_SUITE_END();
public:

	Document get_document(const char* name){
			Document doc;

			Node root = doc.create_element("html");
			Node body = doc.create_element("body", root);
			body.set_attribute("id", "body-element");

			Node div1 = doc.create_element("div", body);
			div1.set_attribute("id", "my-div1");
			div1.set_attribute("class", "bar baz");

			Node div2 = doc.create_element("div");
			div2.set_attribute("id", "my-div2");

			Node div3 = doc.create_element("div", body);
			div3.set_attribute("id", "my-div3");

			doc.create_element("b", div1).set_attribute("id", "test-1");
			doc.create_element("b", div1).set_attribute("id", "test-2");
			doc.create_element("b", div3).set_attribute("id", "test-3");

			doc.set_root(root);
			return doc;
	}

	void test_find(){
		Document doc = get_document("");
		auto match = doc.find("#my-div1 b");

		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match.size());
		CPPUNIT_ASSERT_EQUAL(std::string("b"), std::string(match[0].tag_name()));
		CPPUNIT_ASSERT_EQUAL(std::string("b"), std::string(match[1].tag_name()));
		CPPUNIT_ASSERT_EQUAL(std::string("test-1"), std::string(match[0].get_attribute("id")));
		CPPUNIT_ASSERT_EQUAL(std::string("test-2"), std::string(match[1].get_attribute("id")));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(DOMTest);

int main(int argc, const char* argv[]){
	CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
	CppUnit::TextUi::TestRunner runner;

	runner.addTest(suite);
	runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr ));

	return runner.run() ? 0 : 1;
}
