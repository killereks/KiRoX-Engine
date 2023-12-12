#pragma once
#include "Asset.h"
class ComputeShader : public Asset
{
	unsigned int computeShaderID;

public:
	ComputeShader();
	~ComputeShader();

	void Dispatch(unsigned int x, unsigned int y, unsigned int z);

	virtual co::Coro BeginLoading() override;

	void LoadShader();
};

