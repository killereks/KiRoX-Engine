#pragma once

#include <string>
#include <vector>
#include <Assets/Texture.h>

class CameraComponent;

class SkyBox {
	unsigned int texID;

	unsigned int VAO;
	unsigned int VBO;

public:
	SkyBox(std::vector<Texture*>& textures);
	~SkyBox();

	void Bind(int slot);
	void Draw(CameraComponent* camera);
	void Unbind();

	unsigned int GetTextureID() const { return texID; }
};

