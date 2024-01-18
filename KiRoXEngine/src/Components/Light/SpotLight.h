#pragma once
#include "Light.h"

#include <Macros.h>

CLASS()
class SpotLight : public Light
{
	REFLECT(Light)

	PROPERTY()
	float angle;

public:
	SpotLight();
	~SpotLight();

	inline float GetAngle() { return angle; }

	virtual void OnDrawGizmos() override;
	virtual bool DrawInspector() override;

	virtual std::string GetIcon() override;
	virtual glm::mat4 GetLightSpaceMatrix() override;
};

