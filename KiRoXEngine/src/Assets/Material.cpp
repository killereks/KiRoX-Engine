#include "Material.h"

void Material::Bind()
{
	if (!shader->HasValue()) {
		return;
	}

	Shader* shader = this->shader->Get<Shader>();

	if (this->mainTexture->HasValue()) {
		Texture* texture = this->mainTexture->Get<Texture>();

		texture->Bind(2);
		shader->setInt("albedoMap", 2);
	}
	else {
		Texture* defaultTex = AssetManager::GetInstance()->GetWhiteTexture();

		defaultTex->Bind(2);
		shader->setInt("albedoMap", 2);
	}
	

	if (this->normalMap->HasValue()) {
		Texture* normalMap = this->normalMap->Get<Texture>();
		normalMap->Bind(3);
		shader->setInt("normalMap", 3);
		shader->setBool("hasNormalMap", true);
	}
	else {
		shader->setBool("hasNormalMap", false);
	}

	if (this->metallicMap->HasValue()) {
		Texture* metallicMap = this->metallicMap->Get<Texture>();
		metallicMap->Bind(4);
		shader->setInt("metallicMap", 4);
		shader->setBool("hasMetallicMap", true);
	}
	else {
		shader->setBool("hasMetallicMap", false);
	}
}

Material::Material()
{
	shader = new ObjectPtr();
	mainTexture = new ObjectPtr();
	normalMap = new ObjectPtr();
	metallicMap = new ObjectPtr();
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