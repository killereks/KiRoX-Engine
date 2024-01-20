#pragma once
#include "Asset.h"
#include <Macros.h>

#include "Shader.h"
#include "Texture.h"

#include <Tools/ObjectPtr.h>

CLASS()
class Material : public Asset {
	REFLECT(Asset)

	unsigned int previewTextureID;

public:
	PROPERTY()
	ObjectPtr* shader;

	PROPERTY()
	ObjectPtr* mainTexture;

	PROPERTY()
	ObjectPtr* normalMap;

	PROPERTY()
	ObjectPtr* metallicMap;

	PROPERTY()
	ObjectPtr* heightMap;

	PROPERTY()
	glm::vec2 tiling;

	Shader* GetShader() const {
		return shader->Get<Shader>();
	}

	void Bind();

	unsigned int GetPreviewTextureID() const { return previewTextureID; }
	void SetPreviewTextureID(unsigned int val) { previewTextureID = val; }

	void CopyPreviewTextureFromID(unsigned int id, unsigned int width, unsigned int height);

	Material();
	Material(const Material& other) : Asset(other) {}
	~Material();

	virtual co::Coro BeginLoading() override;
};

