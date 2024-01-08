#include "Volume.h"

Volume::Volume()
{
	testProcess = new ObjectPtr();
}

void Volume::Apply(unsigned int FBO)
{
	if (!testProcess->HasValue()) return;

	ComputeShader* shader = testProcess->Get<ComputeShader>();

	//shader->SetData
}
