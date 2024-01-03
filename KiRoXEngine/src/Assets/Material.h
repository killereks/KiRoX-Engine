#pragma once
#include "Asset.h"
#include <Macros.h>

#include "Shader.h"
#include "Texture.h"

#include <Tools/ObjectPtr.h>

CLASS()
class Material : public Asset {
	REFLECT(Asset)

public:
	PROPERTY()
	ObjectPtr* shader;

	PROPERTY()
	ObjectPtr* mainTexture;

	void Bind();

	Material();
	Material(const Material& other) : Asset(other) {}
	~Material();

	virtual co::Coro BeginLoading() override;
};

