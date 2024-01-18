#include "SceneShadowMapper.h"

#include <Assets/AssetManager.h>

SceneShadowMapper::SceneShadowMapper()
{
	glGenTextures(1, &shadowMapArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, 2048, 2048, maxLights);

	glGenFramebuffers(1, &shadowMapFBO);

	shadowShader = AssetManager::GetInstance()->Get<Shader>("Shadow.shader");
}

SceneShadowMapper::~SceneShadowMapper()
{
	glDeleteTextures(1, &shadowMapArray);
	glDeleteFramebuffers(1, &shadowMapFBO);
}

void SceneShadowMapper::DrawDebug()
{
	ImGui::Begin("Shadow Mapper");
	
	for (int i = 0; i < lightSpaceMatrices.size(); i++) {
		unsigned int layerTexture;

		glGenTextures(1, &layerTexture);
		glBindTexture(GL_TEXTURE_2D, layerTexture);

		glCopyImageSubData(shadowMapArray, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, layerTexture, GL_TEXTURE_2D, 0, 0, 0, 0, 2048, 2048, 1);

		ImGui::Image((void*)layerTexture, ImVec2(256, 256));

		glDeleteTextures(1, &layerTexture);
	}

	ImGui::End();
}

void SceneShadowMapper::BeginShadowMapping()
{
	lightSpaceMatrices.clear();
	spotLightData.clear();

	currentDrawIndex = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void SceneShadowMapper::RenderSpotLight(SpotLight* spotLight, std::vector<MeshComponent*>& meshComponents)
{
	if (currentDrawIndex >= maxLights) {
		std::cout << "Max lights exceeded" << std::endl;
		return;
	}

	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapArray, 0, currentDrawIndex);

	glViewport(0, 0, 2048, 2048);
	glClear(GL_DEPTH_BUFFER_BIT);

	shadowShader->use();

	glm::mat4 lightSpaceMatrix = spotLight->GetLightSpaceMatrix();

	shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	lightSpaceMatrices.push_back(lightSpaceMatrix);

	SpotLightData data = {
		.pos = spotLight->GetOwner()->GetTransform().GetWorldPosition(),
		.dir = spotLight->GetOwner()->GetTransform().GetForward(),
		.angle = spotLight->GetAngle(),
	};

	spotLightData.push_back(data);

	for (auto& mesh : meshComponents) {
		mesh->SimpleDraw(shadowShader);
	}

	currentDrawIndex++;
}

void SceneShadowMapper::EndShadowMapping()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneShadowMapper::BindShadowMapArray(Shader* shader)
{
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray);

	shader->setInt("shadowMapArray", 10);
	shader->setInt("numLights", currentDrawIndex);

	for (int i = 0; i < currentDrawIndex; i++) {
		std::string uniformName = "lightSpaceMatrices[" + std::to_string(i) + "]";
		shader->setMat4(uniformName, lightSpaceMatrices[i]);

		shader->setVec3("spotLightData[" + std::to_string(i) + "].pos", spotLightData[i].pos);
		shader->setVec3("spotLightData[" + std::to_string(i) + "].dir", spotLightData[i].dir);
		shader->setFloat("spotLightData[" + std::to_string(i) + "].angle", spotLightData[i].angle);
	}
}
