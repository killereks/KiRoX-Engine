#include "MouseSelecting.h"
#include <Components/MeshComponent.h>
#include "imgui/imgui.h"

MouseSelecting::MouseSelecting(){
	renderTexture = new RenderTexture(1, 1);
	renderTexture->UseInts(true);
}

MouseSelecting::~MouseSelecting()
{
	delete renderTexture;
}

void MouseSelecting::Resize(unsigned int width, unsigned int height)
{
	renderTexture->Resize(width, height);
}

glm::vec3 MouseSelecting::MouseClicked(std::vector<MeshComponent*>& meshes, CameraComponent* sceneCamera, glm::vec2 mousePos)
{
	Shader* shader = AssetManager::GetInstance()->Get<Shader>("Click.shader");

	if (shader == nullptr) {
		std::cout << "[MousePicking] Shader not found, it either hasn't loaded yet or does not exist." << std::endl;
		return glm::vec3(0.0f);
	}

	renderTexture->Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	renderTexture->Clear();

	shader->use();

	for (unsigned int i = 0; i < meshes.size(); i++) {
		glm::mat4 model = meshes[i]->GetOwner()->GetTransform().GetModelMatrix();
		glm::mat4 PVM = sceneCamera->GetProjectionMatrix() * sceneCamera->GetViewMatrix() * model;

		shader->setMat4("PVM", PVM);
		shader->setUInt("entityIndex", i + 1);

		if (meshes[i]->GetMeshFilter() != nullptr) {
			meshes[i]->GetMeshFilter()->DrawCall();
		}
	}

	int readX = static_cast<int>(mousePos.x);
	int readY = static_cast<int>(mousePos.y);

	if (readX < 0) readX = 0;
	if (readY < 0) readY = 0;

	if (readX > renderTexture->GetWidth()) readX = renderTexture->GetWidth() - 1;
	if (readY > renderTexture->GetHeight()) readY = renderTexture->GetHeight() - 1;

	renderTexture->Unbind();

	return renderTexture->GetPixel(readX, readY);
}
