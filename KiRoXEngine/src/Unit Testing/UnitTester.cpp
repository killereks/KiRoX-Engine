#include "UnitTester.h"

#include <iostream>

#include <Components/TransformComponent.h>
#include <Entity.h>

std::vector<std::string> UnitTester::failedTests;
std::vector<std::string> UnitTester::failedTestsDebugMessages;

int UnitTester::testCount = 0;
int UnitTester::testFailed = 0;
int UnitTester::testPassed = 0;

void UnitTester::PrintTestResult(bool result, std::string testName, std::string debugMessage)
{
	// colors
	const char* red = "\033[0;31m";
	const char* green = "\033[0;32m";

	const char* character = "o";

	if (result){
		std::cout << green << character;

		testPassed++;
	}
	else {
		std::cout << red << character;

		failedTests.push_back(testName);
		failedTestsDebugMessages.push_back(debugMessage);

		testFailed++;
	}

	testCount++;
}

void Test_Math() {
	// Mathf::Lerp Tests
	UnitTester::AreEqual<float>(Mathf::Lerp(0, 10, 0.5f), 5.0f, "Lerp 0.5");
	UnitTester::AreEqual<float>(Mathf::Lerp(0, 10, 0.0f), 0.0f, "Lerp 0.0");
	UnitTester::AreEqual<float>(Mathf::Lerp(0, 10, 1.0f), 10.0f, "Lerp 1.0");
	UnitTester::AreEqual<float>(Mathf::Lerp(0, 10, 2.5f), 10.0f, "Lerp 2.5");
	UnitTester::AreEqual<float>(Mathf::Lerp(0, 10, -2.5f), 0.0f, "Lerp -2.5");

	// Mathf::LerpUnclamped Tests
	UnitTester::AreEqual<float>(Mathf::LerpUnclamped(0, 10, 0.5f), 5.0f, "LerpUnclamped 0.5");
	UnitTester::AreEqual<float>(Mathf::LerpUnclamped(0, 10, 0.0f), 0.0f, "LerpUnclamped 0.0");
	UnitTester::AreEqual<float>(Mathf::LerpUnclamped(0, 10, 1.0f), 10.0f, "LerpUnclamped 1.0");
	UnitTester::AreEqual<float>(Mathf::LerpUnclamped(0, 10, 2.5f), 25.0f, "LerpUnclamped 2.5");
	UnitTester::AreEqual<float>(Mathf::LerpUnclamped(0, 10, -2.5f), -25.0f, "LerpUnclamped -2.5");

	// FormatWithCommas Tests
	UnitTester::AreEqual<std::string>(Mathf::FormatWithCommas(123), std::string("123"), "FormatWithCommas 123");
	UnitTester::AreEqual<std::string>(Mathf::FormatWithCommas(12345), std::string("12,345"), "FormatWithCommas 12345");
	UnitTester::AreEqual<std::string>(Mathf::FormatWithCommas(123456789), std::string("123,456,789"), "FormatWithCommas 123456789");
	UnitTester::AreEqual<std::string>(Mathf::FormatWithCommas(-987654321), std::string("-987,654,321"), "FormatWithCommas -987654321");
}

void Test_Transforms() {
	Entity* rootEntity = new Entity("RootEntity");

	TransformComponent& transform = rootEntity->GetTransform();

	// Test TransformPoint
	glm::vec3 localPoint(1.0f, 2.0f, 3.0f);
	glm::vec3 worldPoint = transform.TransformPoint(localPoint);
	// Perform the inverse transformation to check if it's working correctly
	glm::vec3 backToLocalPoint = transform.InverseTransformPoint(worldPoint);
	UnitTester::AreEqual(localPoint, backToLocalPoint, "TransformPoint and InverseTransformPoint");

	// Test TransformDirection
	glm::vec3 localDirection(1.0f, 0.0f, 0.0f);
	glm::vec3 worldDirection = transform.TransformDirection(localDirection);
	// Perform the inverse transformation to check if it's working correctly
	glm::vec3 backToLocalDirection = transform.InverseTransformDirection(worldDirection);
	UnitTester::AreEqual(localDirection, backToLocalDirection, "TransformDirection and InverseTransformDirection");

	// Test InverseTransformPoint
	glm::vec3 anotherWorldPoint(5.0f, 6.0f, 7.0f);
	glm::vec3 backToLocalPoint2 = transform.InverseTransformPoint(anotherWorldPoint);
	// Perform the forward transformation to check if it's working correctly
	glm::vec3 retransformedWorldPoint = transform.TransformPoint(backToLocalPoint2);
	UnitTester::AreEqual(anotherWorldPoint, retransformedWorldPoint, "InverseTransformPoint and TransformPoint");

	// Test InverseTransformDirection
	glm::vec3 anotherWorldDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 backToLocalDirection2 = transform.InverseTransformDirection(anotherWorldDirection);
	// Perform the forward transformation to check if it's working correctly
	glm::vec3 retransformedWorldDirection = transform.TransformDirection(backToLocalDirection2);
	UnitTester::AreEqual(anotherWorldDirection, retransformedWorldDirection, "InverseTransformDirection and TransformDirection");

	// Test LocalToWorldDirection and WorldToLocalDirection
	glm::vec3 localDir(1.0f, 0.0f, 0.0f);
	glm::vec3 worldDir = transform.LocalToWorldDirection(localDir);
	// Perform the inverse transformation to check if it's working correctly
	glm::vec3 backToLocalDir = transform.WorldToLocalDirection(worldDir);
	UnitTester::AreEqual(localDir, backToLocalDir, "LocalToWorldDirection and WorldToLocalDirection");

	delete rootEntity;
}

void Test_TransformsSettersGetters() {
	Entity* rootEntity = new Entity("RootEntity");

	TransformComponent& transform = rootEntity->GetTransform();

	// Test SetWorldPosition
	transform.SetWorldPosition(glm::vec3(1.0f, 2.0f, 3.0f));
	UnitTester::AreEqual<glm::vec3>(transform.GetWorldPosition(), glm::vec3(1.0f, 2.0f, 3.0f), "SetWorldPosition");

	transform.SetWorldPosition(glm::vec3(-5.0f, 0.0f, 10.0f));
	UnitTester::AreEqual<glm::vec3>(transform.GetWorldPosition(), glm::vec3(-5.0f, 0.0f, 10.0f), "SetWorldPosition with negative values");

	transform.SetWorldPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	UnitTester::AreEqual<glm::vec3>(transform.GetWorldPosition(), glm::vec3(0.0f, 0.0f, 0.0f), "SetWorldPosition with zero values");

	// Test SetWorldRotation with euler angles
	transform.SetWorldRotation(glm::vec3(-180.0f, 270.0f, 360.0f));
	UnitTester::AreEqual<glm::quat>(transform.GetWorldRotation(), glm::quat(glm::vec3(-180.0f, 270.0f, 360.0f)), "SetWorldRotation with out-of-range Euler angles");

	transform.SetWorldRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	UnitTester::AreEqual<glm::quat>(transform.GetWorldRotation(), glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), "SetWorldRotation with zero Euler angles");

	// Test SetWorldRotation with quaternion
	glm::quat quaternionRotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	transform.SetWorldRotation(quaternionRotation);
	UnitTester::AreEqual(transform.GetWorldRotation(), quaternionRotation, "SetWorldRotation (Quaternion)");

	quaternionRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	transform.SetWorldRotation(quaternionRotation);
	UnitTester::AreEqual(transform.GetWorldRotation(), quaternionRotation, "SetWorldRotation with negative angle Quaternion");

	quaternionRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	transform.SetWorldRotation(quaternionRotation);
	UnitTester::AreEqual(transform.GetWorldRotation(), quaternionRotation, "SetWorldRotation with identity Quaternion");

	// Test SetWorldScale
	transform.SetWorldScale(glm::vec3(2.0f, 0.5f, 1.0f));
	UnitTester::AreEqual(transform.GetWorldScale(), glm::vec3(2.0f, 0.5f, 1.0f), "SetWorldScale");

	transform.SetWorldScale(glm::vec3(-1.0f, 1.0f, 1.0f));
	UnitTester::AreEqual(transform.GetWorldScale(), glm::vec3(-1.0f, 1.0f, 1.0f), "SetWorldScale with negative values");

	transform.SetWorldScale(glm::vec3(0.0f, 0.0f, 0.0f));
	UnitTester::AreEqual(transform.GetWorldScale(), glm::vec3(0.0f, 0.0f, 0.0f), "SetWorldScale with zero values");

	// Test SetLocalPosition
	transform.SetLocalPosition(glm::vec3(1.0f, 2.0f, 3.0f));
	UnitTester::AreEqual(transform.GetLocalPosition(), glm::vec3(1.0f, 2.0f, 3.0f), "SetLocalPosition");

	transform.SetLocalPosition(glm::vec3(-5.0f, 0.0f, 10.0f));
	UnitTester::AreEqual(transform.GetLocalPosition(), glm::vec3(-5.0f, 0.0f, 10.0f), "SetLocalPosition with negative values");

	transform.SetLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	UnitTester::AreEqual(transform.GetLocalPosition(), glm::vec3(0.0f, 0.0f, 0.0f), "SetLocalPosition with zero values");

	// Test SetLocalRotation with euler angles
	transform.SetLocalRotation(glm::vec3(45.0f, 90.0f, 180.0f));
	UnitTester::AreEqual<glm::quat>(transform.GetLocalRotation(), glm::quat(glm::vec3(45.0f, 90.0f, 180.0f)), "SetLocalRotation (Euler angles)");

	transform.SetLocalRotation(glm::vec3(-180.0f, 270.0f, 360.0f));
	UnitTester::AreEqual<glm::quat>(transform.GetLocalRotation(), glm::quat(glm::vec3(-180.0f, 270.0f, 0.0f)), "SetLocalRotation with out-of-range Euler angles");

	transform.SetLocalRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	UnitTester::AreEqual<glm::quat>(transform.GetLocalRotation(), glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), "SetLocalRotation with zero Euler angles");

	// Test SetLocalRotation with quaternion
	quaternionRotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	transform.SetLocalRotation(quaternionRotation);
	UnitTester::AreEqual(transform.GetLocalRotation(), quaternionRotation, "SetLocalRotation (Quaternion)");

	quaternionRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	transform.SetLocalRotation(quaternionRotation);
	UnitTester::AreEqual(transform.GetLocalRotation(), quaternionRotation, "SetLocalRotation with negative angle Quaternion");

	quaternionRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	transform.SetLocalRotation(quaternionRotation);
	UnitTester::AreEqual(transform.GetLocalRotation(), quaternionRotation, "SetLocalRotation with identity Quaternion");

	delete rootEntity;
}

void UnitTester::RunTests()
{
	std::cout << "\n===================== Unit Test Summary =====================\n";

	Test_Math();
	Test_Transforms();
	Test_TransformsSettersGetters();



	Summary();
}

void UnitTester::Summary()
{
	// normal color
	std::cout << "\033[0m";

	std::cout << "\nTests passed: " << testPassed << " / " << testCount << "\n";

	if (testFailed == 0) {
		std::cout << "All tests passed!\n";
		std::cout << "=============================================================\n";
		return;
	}

	std::cout << "\nFailed tests: " << failedTests.size() << "\n";

	for (int i = 0; i < failedTests.size(); i++) {
		std::cout << "\n" << failedTests[i] << "\n";
		std::cout << failedTestsDebugMessages[i] << "\n";
	}

	std::cout << "=============================================================\n";
}

bool UnitTester::IsTrue(bool actual, const char* testName)
{
	bool result = actual;

	PrintTestResult(result, testName, "Expected: true Actual: false");

	return result;
}

bool UnitTester::IsFalse(bool actual, const char* testName)
{
	bool result = !actual;

	PrintTestResult(result, testName, "Expected: false Actual: true");

	return result;
}

bool UnitTester::IsNull(void* actual, const char* testName)
{
	bool result = actual == nullptr;

	PrintTestResult(result, testName, "Expected: null Actual: not null");

	return result;
}

bool UnitTester::IsNotNull(void* actual, const char* testName)
{
	bool result = actual != nullptr;

	PrintTestResult(result, testName, "Expected: not null Actual: null");

	return result;
}
