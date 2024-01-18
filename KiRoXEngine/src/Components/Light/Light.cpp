#include "Light.h"

Light::Light()
{

}

Light::~Light()
{

}

void Light::Render(std::vector<MeshComponent*>& meshes)
{
	shadowMap->Render(meshes, GetLightSpaceMatrix());
}

void Light::BindShadowMap(int slot)
{
	if (shadowMap == nullptr) {
		std::cout << "Shadow map is null" << std::endl;
		return;
	}

	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, shadowMap->GetDepthMap());
}
