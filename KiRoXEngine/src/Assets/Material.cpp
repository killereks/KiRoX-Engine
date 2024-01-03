#include "Material.h"

Material::Material()
{
	shader = new ObjectPtr();
	mainTexture = new ObjectPtr();
}

Material::~Material()
{
	delete shader;
	delete mainTexture;
}

co::Coro Material::BeginLoading(){
	loaded = true;
	return {};
}