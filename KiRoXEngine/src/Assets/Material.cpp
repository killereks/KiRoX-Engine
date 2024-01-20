#include "Material.h"

void Material::Bind()
{
	if (!shader->HasValue()) {
		return;
	}

	Shader* shader = this->shader->Get<Shader>();

	shader->use();

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

	if (this->heightMap->HasValue()) {
		Texture* heightMap = this->heightMap->Get<Texture>();
		heightMap->Bind(5);
		shader->setInt("heightMap", 5);
		shader->setBool("hasHeightMap", true);
	}
	else {
		shader->setBool("hasHeightMap", false);
	}

	shader->setFloat("roughness", glm::clamp(roughness, 0.0f, 1.0f));
	shader->setVec2("tiling", tiling);
}

void Material::CopyPreviewTextureFromID(unsigned int id, unsigned int width, unsigned int height)
{
	glGenTextures(1, &previewTextureID);
	glBindTexture(GL_TEXTURE_2D, previewTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glCopyImageSubData( id,					GL_TEXTURE_2D, 0, 0, 0, 0,
						previewTextureID,	GL_TEXTURE_2D, 0, 0, 0, 0,
						width, height, 1);
}

Material::Material()
{
	shader = new ObjectPtr();
	mainTexture = new ObjectPtr();
	normalMap = new ObjectPtr();
	metallicMap = new ObjectPtr();
	heightMap = new ObjectPtr();

	tiling = glm::vec2(1.0f, 1.0f);
	roughness = 0.0;
}

Material::~Material()
{
	delete shader;
	delete mainTexture;
	delete normalMap;
	delete metallicMap;
	delete heightMap;

	glDeleteTextures(1, &previewTextureID);
}

co::Coro Material::BeginLoading(){
	loaded = true;
	return {};
}