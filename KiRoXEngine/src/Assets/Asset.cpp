#include "Asset.h"
#include <Tools/SavingLoading.h>
#include "refl.gen.h"

void Asset::SaveMetaFile()
{
	rttr::variant var(this);

	const rttr::type type = Reflection::GetType(GetTypeName());
	if (type != rttr::type::get<void>()) {
		if (!var.convert(type)) {
			std::cout << "Failed to convert variant to type: " << type.get_name().c_str() << "\n";
		}
		else {
			SavingLoading::SaveYAMLFile(filePath + ".meta", var);
		}
	} else {
		std::cout << "No type found for: " << GetTypeName() << "\n";
	}
}

void Asset::LoadMetaFile()
{
	const std::string path = filePath + ".meta";

	rttr::variant var(this);

	const rttr::type type = Reflection::GetType(GetTypeName());

	if (type != rttr::type::get<void>()) {
		if (!var.convert(type)) {
			std::cout << "Failed to convert variant to type: " << type.get_name().c_str() << "\n";
		}
		else {
			SavingLoading::LoadYAMLObject(path, var);
		}
	}
	else {
		std::cout << "No type found for: " << GetTypeName() << "\n";
	}

}
