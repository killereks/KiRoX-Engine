#pragma once

#include <rttr/registration>

using namespace rttr;

RTTR_REGISTRATION { 
}

static const rttr::type GetType(const std::string& name){
throw std::runtime_error("No type found for " + name);
}

static void RegisterTypes(){
}

