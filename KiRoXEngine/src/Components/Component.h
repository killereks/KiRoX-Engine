#pragma once

#include <typeinfo>
#include <string_view>

#include <yaml-cpp/yaml.h>
#include "../Tools/Serialization.h"
#include "../Macros.h"

class Entity;

CLASS()
class Component {
	REFLECT()

protected:
	Entity* owner;

	std::string name;

public:
	Component();
	virtual ~Component();

	void SetOwner(Entity* owner);
	Entity* GetOwner() { return owner; }

	virtual void OnDrawGizmos() {}

	virtual bool DrawInspector();

	virtual void Init() {}
	virtual void Update(float dt) {}

	virtual std::string GetIcon() = 0;

	virtual const char* GetName() {
		if (!name.empty())
		{
			return name.c_str();
		}

		std::string_view name_view = typeid(*this).name();
		name_view.remove_prefix(name_view.find_first_of(' ') + 1);
		//if (name_view.ends_with("Component"))
		//{
		//	name_view.remove_suffix(strlen("Component"));
		//}

		name = name_view;

		return name.c_str();
	}
};

