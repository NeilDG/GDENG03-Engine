#pragma once
#include <iostream>
#include <vector>
#include <stack>

class EditorAction;
class AGameObject;
class ActionHistory
{
public:
	typedef std::stack<EditorAction*> ActionStack;
	static ActionHistory* getInstance();
	static void initialize();
	static void destroy();

	void recordAction(AGameObject* gameObject);
	bool hasRemainingActions();
	EditorAction* undoAction();
	void clear();

private:
	ActionHistory();
	~ActionHistory();
	ActionHistory(ActionHistory const&) {};             // copy constructor is private
	ActionHistory& operator=(ActionHistory const&) {};  // assignment operator is private*/

	static ActionHistory* sharedInstance;

	ActionStack actionsPerformed;
};

