#include "ActionHistory.h"
#include "EditorAction.h"

ActionHistory* ActionHistory::sharedInstance = NULL;

ActionHistory* ActionHistory::getInstance()
{
	return sharedInstance;
}

void ActionHistory::initialize()
{
	sharedInstance = new ActionHistory();
}

void ActionHistory::destroy()
{
	delete sharedInstance;
}

void ActionHistory::recordAction(AGameObject* gameObject)
{
	EditorAction* editorAction = new EditorAction(gameObject);
	this->actionsPerformed.push(editorAction);
}

bool ActionHistory::hasRemainingActions()
{
	return this->actionsPerformed.empty();
}

EditorAction* ActionHistory::undoAction()
{
	if (this->hasRemainingActions()) {
		EditorAction* action = this->actionsPerformed.top();
		this->actionsPerformed.pop();
		return action;
	}
	else {
		std::cout << "No more actions remaining. \n";
		return NULL;
	}
}

void ActionHistory::clear()
{
	while (this->hasRemainingActions()) {
		EditorAction* action = this->actionsPerformed.top();
		delete action;
		this->actionsPerformed.pop();
	}
}

ActionHistory::ActionHistory()
{
}

ActionHistory::~ActionHistory()
{
	this->clear();
}
