#pragma once

#include <rttr/registration>

RTTR_REGISTRATION { 
registration::class_<Class1>("Class1")
.constructor<>()
.property("property1", &Class1::property1)
.property("property2", &Class1::property2)
.method("print_properties", &Class1::print_properties)
;}

RTTR_REGISTRATION { 
registration::class_<Class3>("Class3")
.constructor<>()
.property("property5", &Class3::property5)
.property("property6", &Class3::property6)
.method("print_properties", &Class3::print_properties)
;}

RTTR_REGISTRATION { 
registration::class_<Class2>("Class2")
.constructor<>()
.property("property3", &Class2::property3)
.property("property4", &Class2::property4)
.method("print_properties", &Class2::print_properties)
;}

