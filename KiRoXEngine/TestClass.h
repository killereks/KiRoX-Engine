#include "src/Macros.h"
#include <string>

class TestClass {
public:

	PROPERTY()
	int m_TestInt = 3;

	PROPERTY()
	float m_TestFloat = 3.1415f;

	PROPERTY()
	std::string m_TestString = "hello";
};

/*#include <rttr/registration.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<TestClass>("TestClass")
		.property("m_TestInt", &TestClass::m_TestInt)
		.property("m_TestFloat", &TestClass::m_TestFloat)
		.property("m_TestString", &TestClass::m_TestString);
}*/