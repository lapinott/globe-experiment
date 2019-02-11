#pragma once
#include <G3D/G3D.h>

namespace NYNE {

	// entities under UID control
	class UserControlledEntity : public virtual Entity {

	private:

		// currently controlled entities
		static				shared_ptr<Array<shared_ptr<UserControlledEntity>>> m_user_controlled_entities;

		// UserInput* as of last call to App::onUserInput
		static				UserInput* m_user_input;

		// MouseScroll2DEvent as of last mouse scroll event
		static				MouseScroll2DEvent m_mouse_scroll_input;

	protected:

		// add onUserInput handler to entities
		virtual void		onUserInput();

		// same for mouse scroll input
		virtual void		onMouseScrollInput();

		// getters
		UserInput*			userInput() const;
		MouseScroll2DEvent& mouseScrollInput() const;

		UserControlledEntity() {};

	public:

		// is instance currently being controlled ?
		bool isUserControlled() const;

		// dispatch func
		static void onUserInput(UserInput* const ui);

		// dispatch fund
		static void onMouseScrollInput(const MouseScroll2DEvent& input);

		// manage controlled entities
		static void	set(const Array<shared_ptr<UserControlledEntity>>& ee);
		static void add(const shared_ptr<UserControlledEntity>& e);
		static void remove(const shared_ptr<UserControlledEntity>& e);
		static void	clear();
	};
}