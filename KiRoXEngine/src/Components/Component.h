#pragma once

class Entity;

class Component
{

protected:
	Entity* owner;

public:
	Component();
	~Component();

	void SetOwner(Entity* owner);

	virtual void Init() {}
	virtual void Update() {}

	virtual void DrawInspector() = 0;

};

