#pragma once
#include "Component.h"
#include <Tools/ObjectPtr.h>
#include "icons/IconsFontAwesome6.h"

#include <Components/CameraComponent.h>
#include <Components/Light/DirectionalLight.h>

#include <Macros.h>

CLASS()
class Volume : public Component
{
	REFLECT(Component)

	unsigned int FBO;
	unsigned int textureColorBuffer;
	unsigned int RBO;

	unsigned int quadVAO, quadVBO;

	int width, height;

	PROPERTY()
	ObjectPtr* testProcess;

	void SetupFBO(int width, int height);

	void Draw();

public:
	Volume();
	~Volume();

	void Apply(unsigned int textureID, int width, int height, CameraComponent* camera, DirectionalLight* light);

	unsigned int GetTextureID() const { return textureColorBuffer; }

	virtual std::string GetIcon() override {
		// 
		return ICON_FA_DISPLAY;
	}

};

