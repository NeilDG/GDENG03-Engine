#include "ActionHistory.h"
#include "EditorAction.h"
#include "EngineBackend.h"

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
	if (EngineBackend::getInstance()->getMode() == EngineBackend::EditorMode::EDITOR) {
		EditorAction* editorAction = new EditorAction(gameObject);
		this->actionsPerformed.push(editorAction);

		std::cout << "Stored action " << gameObject->getName() << "\n";
	}
}

bool ActionHistory::hasRemainingUndoActions()
{
	return !this->actionsPerformed.empty();
}

bool ActionHistory::hasRemainingRedoActions()
{
	return !this->actionsCancelled.empty();
}


EditorAction* ActionHistory::undoAction()
{
	//do not undo actions during play mode.
	if (EngineBackend::getInstance()->getMode() != EngineBackend::EditorMode::EDITOR) {
		std::cout << "Cannot perform any undo during play. \n";
		return NULL;
	}

	if (this->hasRemainingUndoActions()) {
		EditorAction* action = this->actionsPerformed.top();
		this->actionsPerformed.pop();
		this->actionsCancelled.push(action);
		return action;
	}
	else {
		std::cout << "No more actions remaining. \n";
		return NULL;
	}
}

EditorAction* ActionHistory::redoAction()
{
	//do not undo actions during play mode.
	if (EngineBackend::getInstance()->getMode() != EngineBackend::EditorMode::EDITOR) {
		std::cout << "Cannot perform any redo during play. \n";
		return NULL;
	}

	if (this->hasRemainingRedoActions()) {
		EditorAction* action = this->actionsCancelled.top();
		this->actionsCancelled.pop();
		this->actionsPerformed.push(action);
		return action;
	}
	else {
		std::cout << "No more actions remaining. \n";
		return NULL;
	}
}

void ActionHistory::clear()
{
	while (this->hasRemainingUndoActions()) {
		EditorAction* action = this->actionsPerformed.top();
		delete action;
		this->actionsPerformed.pop();
	}

	while (this->hasRemainingRedoActions()) {
		EditorAction* action = this->actionsCancelled.top();
		delete action;
		this->actionsCancelled.pop();
	}
}

ActionHistory::ActionHistory()
{
}

ActionHistory::~ActionHistory()
{
	this->clear();
}
