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

class MeshComponent : public Component
{
	// cached shader uniforms
	int modelMatrixLocation = -1;
	int viewMatrixLocation = -1;
	int projectionMatrixLocation = -1;

	std::string meshUUID;

	MeshFilter* meshFilter;

	Bounds* boundsRotated;

	public:
		MeshComponent();
		~MeshComponent();

		void DrawInspector() override;

		void SetMeshUUID(std::string uuid);

		void Serialize(YAML::Emitter& out) override;

		Bounds* GetBounds();

		const int GetVertexCount() const {
			if (meshFilter != nullptr) {
				return meshFilter->GetVertices().size();
			}

			return 0;
		}

		const size_t GetTriangleCount() const
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

