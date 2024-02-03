#include <rttr/registration>

#include <Tools/Profiling/Profiler.h>
#include <Tools/Profiling/ProfileScope.h>

class ProfileScope;

#define PROPERTY()

#define CLASS(...)

#define ENUM(...)

#define REFLECT(X) \
	RTTR_ENABLE(X) \
	RTTR_REGISTRATION_FRIEND

#define FUNCTION()

#if EDITOR
#define PROFILE_FUNCTION() ProfileScope ps(__FUNCTION__);
#define PROFILE_FUNCTION_NAME(x) ProfileScope ps(x);
#else
#define PROFILE_FUNCTION()
#endif

#define SERIALIZE_VALUE(variableName) out << YAML::Key << #variableName << YAML::Value << variableName;