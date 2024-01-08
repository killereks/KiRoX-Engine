#pragma once
#include "Asset.h"
#include <Macros.h>

#include "Shader.h"
#include "Texture.h"

#include "coroutines/CoroExtensions.h"

#include <Tools/ObjectPtr.h>

CLASS()
class Material : public Asset {
	REFLECT(Asset)

public:
	PROPERTY()
	ObjectPtr* shader;

	PROPERTY()
	ObjectPtr* mainTexture;

	PROPERTY()
	ObjectPtr* normalMap;

	PROPERTY()
	ObjectPtr* metallicMap;

	Shader* GetShader() const {
		return shader->Get<Shader>();
	}

	void Bind();

	Material();
	Material(const Material& other) : Asset(other) {}
	~Material();

	virtual co::Coro BeginLoading() override;
};

