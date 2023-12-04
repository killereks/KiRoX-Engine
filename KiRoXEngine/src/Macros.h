#define PROPERTY()

#define CLASS()

#define PROFILE_FUNCTION() Stopwatch sw(__FUNCTION__);

#define SERIALIZE_VALUE(variableName) out << YAML::Key << #variableName << YAML::Value << variableName;