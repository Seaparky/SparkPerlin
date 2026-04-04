#pragma once
#include "ProjectState.h"
class MainState : public ProjectState
{
	virtual bool Update(std::stack<std::shared_ptr<ProjectState>>& aProjectState) override;
	virtual void Render() override;

private:
	enum class Choice
	{
		Empty,
		Console,
		ImGUI,
		Quit
	};

	Choice chosen;

};


