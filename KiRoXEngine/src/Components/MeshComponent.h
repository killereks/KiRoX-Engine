#pragma once

#include "Component.h"

#include <vector>
#include <glm/glm.hpp>
#include "../Assets/Shader.h"
#include "../Macros.h"
#include "../Entity.h"
#include "../Assets/MeshFilter.h"
#include "../Tools/Bounds.h"
#include "icons/IconsFontAwesome6.h"

CLASS()
class MeshComponent : public Component
{
	REFLECT()

	PROPERTY()
	int myNumber;

	// cached shader uniforms
	int modelMatrixLocation = -1;
	int viewMatrixLocation = -1;
	int projectionMatrixLocation = -1;

	std::string meshName;

	MeshFilter* meshFilter;

	Bounds* boundsRotated;

	public:
		MeshComponent();
		~MeshComponent();

		void SetMeshName(std::string name);

		void DrawInspector() override;
		void Serialize(YAML::Emitter& out) override;

		Bounds* GetBounds();

		const int GetVertexCount() const {
			if (meshFilter != nullptr) {
				return meshFilter->GetVertices().size();
			}

			return 0;
		}

		const int GetTriangleCount() const
		{
			if (meshFilter != nullptr)
			{
				return meshFilter->GetIndices().size() / 3;
			}

			return 0;
		}

		std::string GetIcon() override
		{
			return " " ICON_FA_CUBE;
		}

		void SimpleDraw(Shader* shader);
};

