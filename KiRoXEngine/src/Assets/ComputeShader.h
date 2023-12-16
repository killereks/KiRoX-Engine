#pragma once
#include "Asset.h"
#include <vector>

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

	virtual co::Coro BeginLoading() override;

	void LoadShader();
};

