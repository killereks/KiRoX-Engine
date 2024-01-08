#pragma once
#include "Asset.h"
#include <vector>
#include <glm/glm.hpp>

class ComputeShader : public Asset
{
	unsigned int computeShaderID;

	std::vector<unsigned int> buffers;

	void CreateBuffer(unsigned int index);

	void CheckCompileErrors(unsigned int shader, std::string type);

public:
	ComputeShader();
	~ComputeShader();

	void Bind();
	void Unbind();

	void Recompile();

	void Dispatch(unsigned int x, unsigned int y, unsigned int z);

	void SetData(unsigned int index, void* data, unsigned int size);

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;

	void SetTexture(unsigned int textureID, unsigned int slot = 0);
	void SetTextureLayout(unsigned int textureID, unsigned int slot = 0);

	virtual co::Coro BeginLoading() override;

	void LoadShader();
};

