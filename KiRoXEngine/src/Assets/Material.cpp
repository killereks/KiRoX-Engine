#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

co::Coro Material::BeginLoading(){
	loaded = true;
	return {};
}