#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <gtx/quaternion.hpp>

#include <Math/Mathf.h>

class UnitTester {

	static int testCount;
	static int testPassed;
	static int testFailed;

	static std::vector<std::string> failedTests;
	static std::vector<std::string> failedTestsDebugMessages;

	static void PrintTestResult(bool result, std::string testName, std::string debugMessage);

	template <typename T>
	static std::string ToString(T value) {
		return std::to_string(value);
	}

	static std::string ToString(std::string value) {
		return value;
	}

	static std::string ToString(glm::vec2 value) {
		return "(" + std::to_string(value.x) + ", " + std::to_string(value.y) + ")";
	}

	static std::string ToString(glm::vec3 value) {
		return "(" + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z) + ")";
	}

	static std::string ToString(glm::vec4 value) {
		return "(" + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z) + ", " + std::to_string(value.w) + ")";
	}

	static std::string ToString(glm::quat value) {
		return "(" + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z) + ", " + std::to_string(value.w) + ")";
	}

public:
	static void RunTests();

	static void Summary();

	template <typename T>
	static bool AreEqual(T expected, T actual, const char* testName) {
		bool result = expected == actual;

		std::string message = "Expected: " + ToString(expected) + " Actual: " + ToString(actual);

		PrintTestResult(result, testName, message);

		return result;
	}

	template <>
	bool AreEqual<std::string>(std::string expected, std::string actual, const char* testName) {
		bool result = expected == actual;

		std::string formattedMessage = "Expected: " + ToString(expected) + " Actual: " + ToString(actual);

		PrintTestResult(result, testName, formattedMessage);

		return result;
	}

	template<>
	bool AreEqual<glm::quat>(glm::quat expected, glm::quat actual, const char* testName) {
		float dot = glm::min(glm::abs(glm::dot(expected, actual)), 1.0f);
		bool result = Mathf::Approximately(dot, 1.0f);

		std::string formattedMessage = "Expected: " + ToString(expected) + " Actual: " + ToString(actual);

		PrintTestResult(result, testName, formattedMessage);

		return result;
	}

	template<>
	bool AreEqual<glm::vec2>(glm::vec2 expected, glm::vec2 actual, const char* testName) {
		bool result =	Mathf::Approximately(expected.x, actual.x) &&
						Mathf::Approximately(expected.y, actual.y);

		std::string formattedMessage = "Expected: " + ToString(expected) + " Actual: " + ToString(actual);

		PrintTestResult(result, testName, formattedMessage);

		return result;
	}

	template<>
	bool AreEqual<glm::vec3>(glm::vec3 expected, glm::vec3 actual, const char* testName) {
		bool result =	Mathf::Approximately(expected.x, actual.x) && 
						Mathf::Approximately(expected.y, actual.y) && 
						Mathf::Approximately(expected.z, actual.z);

		std::string formattedMessage = "Expected: " + ToString(expected) + " Actual: " + ToString(actual);

		PrintTestResult(result, testName, formattedMessage);

		return result;
	}

	template<>
	bool AreEqual<glm::vec4>(glm::vec4 expected, glm::vec4 actual, const char* testName) {
		bool result =	Mathf::Approximately(expected.x, actual.x) && 
						Mathf::Approximately(expected.y, actual.y) && 
						Mathf::Approximately(expected.z, actual.z) && 
						Mathf::Approximately(expected.w, actual.w);

		std::string formattedMessage = "Expected: " + ToString(expected) + " Actual: " + ToString(actual);

		PrintTestResult(result, testName, formattedMessage);

		return result;
	}

	template <typename T>
	static bool AreNotEqual(T expected, T actual, const char* testName) {
		bool result = !AreEqual(expected, actual, testName);

		std::string message = "Expected: " + ToString(expected) + " Actual: " + ToString(actual);

		PrintTestResult(result, testName, message);

		return result;
	}

	template <>
	bool AreNotEqual<std::string>(std::string expected, std::string actual, const char* testName) {
		bool result = !AreEqual(expected, actual, testName);

		std::string formattedMessage = "Expected: " + expected + " Actual: " + actual;

		PrintTestResult(result, testName, formattedMessage);

		return result;
	}

	static bool IsTrue(bool actual, const char* message);
	static bool IsFalse(bool actual, const char* message);

	static bool IsNull(void* actual, const char* message);
	static bool IsNotNull(void* actual, const char* message);
};

