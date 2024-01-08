#pragma once
#include "Asset.h"
#include <vector>
#include <glm/glm.hpp>

class ComputeShader : public Asset
{
	unsigned int computeShaderID;

	std::vector<unsigned int> buffers;

	void CreateBuffer(unsigned int index);

public:
	ComputeShader();
	~ComputeShader();

	void Dispatch(unsigned int x, unsigned int y, unsigned int z);

	void SetData(unsigned int index, void* data, unsigned int size);

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;

	virtual co::Coro BeginLoading() override;

	void LoadShader();
};

