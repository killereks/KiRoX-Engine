#include <rttr/registration>

#define PROPERTY()

#define CLASS(...)

#define REFLECT() \
	RTTR_ENABLE() \
	RTTR_REGISTRATION_FRIEND

#define REFLECT(X) \
	RTTR_ENABLE(X) \
	RTTR_REGISTRATION_FRIEND

#define FUNCTION()

#define PROFILE_FUNCTION() Stopwatch sw(__FUNCTION__);

#define SERIALIZE_VALUE(variableName) out << YAML::Key << #variableName << YAML::Value << variableName;