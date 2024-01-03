#pragma once

#include "Component.h"

#include <vector>
#include <glm/glm.hpp>
#include "../Assets/Shader.h"
#include "../Entity.h"
#include "../Assets/MeshFilter.h"
#include "../Tools/Bounds.h"
#include "icons/IconsFontAwesome6.h"

#include <Tools/ObjectPtr.h>

CLASS()
class MeshComponent : public Component
{
	REFLECT(Component)

	// cached shader uniforms
	int modelMatrixLocation = -1;
	int viewMatrixLocation = -1;
	int projectionMatrixLocation = -1;

	PROPERTY()
	ObjectPtr* mesh;

	PROPERTY()
	ObjectPtr* material;

	Shader* shader;

	Bounds* boundsRotated;

	public:
		MeshComponent();
		~MeshComponent();

		MeshFilter* GetMeshFilter() const { 
			return mesh->Get<MeshFilter>();
		}

		Bounds* GetBounds();

		const int GetVertexCount() const {
			if (GetMeshFilter() != nullptr) {
				return GetMeshFilter()->GetVertices().size();
			}

			return 0;
		}

		const size_t GetTriangleCount() const
		{
			if (GetMeshFilter() != nullptr)
			{
				return GetMeshFilter()->GetIndices().size() / 3;
			}

			return 0;
		}

		std::string GetIcon() override
		{
			return " " ICON_FA_DRAW_POLYGON;
		}

		void SimpleDraw(Shader* shader);
};

