#include "UserControlledEntity.h"

using namespace NYNE;

shared_ptr<Array<shared_ptr<UserControlledEntity>>> UserControlledEntity::m_user_controlled_entities = shared_ptr<Array<shared_ptr<UserControlledEntity>>>(new Array<shared_ptr<UserControlledEntity>>());
UserInput* UserControlledEntity::m_user_input = nullptr;
MouseScroll2DEvent UserControlledEntity::m_mouse_scroll_input = MouseScroll2DEvent();

bool UserControlledEntity::isUserControlled() const {
	for (shared_ptr<UserControlledEntity>& p : *m_user_controlled_entities) {
		if (p.get() == this) return true;
	}
	return false;
}

UserInput* UserControlledEntity::userInput() const {
	return m_user_input;
}

MouseScroll2DEvent& UserControlledEntity::mouseScrollInput() const {
	return m_mouse_scroll_input;
}

void UserControlledEntity::onUserInput() { // virtual
	// default = empty
}

void UserControlledEntity::onMouseScrollInput() { // virtual
	// default = empty
}

void UserControlledEntity::onUserInput(UserInput* const ui) { // static
	m_user_input = ui;
	if (!isNull(m_user_input)) {
		for (shared_ptr<UserControlledEntity>& e : *m_user_controlled_entities) {
			e->onUserInput();
		}
	}
}

void UserControlledEntity::onMouseScrollInput(const MouseScroll2DEvent& input) { // static
	m_mouse_scroll_input = input;
	for (shared_ptr<UserControlledEntity>& e : *m_user_controlled_entities) {
		e->onMouseScrollInput();
	}
}

void UserControlledEntity::set(const Array<shared_ptr<UserControlledEntity>>& ee) {
	m_user_controlled_entities.reset(new Array<shared_ptr<UserControlledEntity>>(ee));
}

void UserControlledEntity::add(const shared_ptr<UserControlledEntity>& e) {
	for (int i = 0; i < m_user_controlled_entities->length(); i++) {
		if ((*m_user_controlled_entities)[i] == e) {
			return;
		}
	}
	m_user_controlled_entities->append(e);
}
void UserControlledEntity::remove(const shared_ptr<UserControlledEntity>& e) {
	for (int i = 0; i < (*m_user_controlled_entities).length(); i++) {
		if ((*m_user_controlled_entities)[i] == e) {
			m_user_controlled_entities->remove(i, 1);
			return;
		}
	}
}

void UserControlledEntity::clear() {
	m_user_controlled_entities->clear();
}