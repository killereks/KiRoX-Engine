#include "PhysicsEvents.h"

void PhysicsEvents::onContact(const reactphysics3d::CollisionCallback::CallbackData& data)
{
	int number = data.getNbContactPairs();

for (int i = 0; i < number; i++) {
		auto contactPair = data.getContactPair(i);

		auto body1 = contactPair.getBody1();
		auto body2 = contactPair.getBody2();

		bool isTrigger1 = contactPair.getCollider1()->getIsTrigger();
		bool isTrigger2 = contactPair.getCollider2()->getIsTrigger();

		if (contactPair.getEventType() == reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStart) {
			std::cout << "Contact Start" << std::endl;
		}
		else if (contactPair.getEventType() == reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStay) {
			std::cout << "Contact Stay" << std::endl;
		}
		else if (contactPair.getEventType() == reactphysics3d::CollisionCallback::ContactPair::EventType::ContactExit) {
			std::cout << "Contact End" << std::endl;
		}
		else {
			std::cout << "Unknown" << std::endl;
		}
	}
}

void PhysicsEvents::onTrigger(const reactphysics3d::OverlapCallback::CallbackData& data)
{
	int number = data.getNbOverlappingPairs();

	for (int i = 0; i < number; i++) {
		auto overlapPair = data.getOverlappingPair(i);
		
		auto body1 = overlapPair.getBody1();
		auto body2 = overlapPair.getBody2();

		bool isTrigger1 = overlapPair.getCollider1()->getIsTrigger();
		bool isTrigger2 = overlapPair.getCollider2()->getIsTrigger();

		if (overlapPair.getEventType() == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStart) {
			std::cout << "Overlap Start" << std::endl;
		}
		else if (overlapPair.getEventType() == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStay) {
			std::cout << "Overlap Stay" << std::endl;
		}
		else if (overlapPair.getEventType() == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapExit) {
			std::cout << "Overlap End" << std::endl;
		}
		else {
			std::cout << "Unknown" << std::endl;
		}
	}
}
