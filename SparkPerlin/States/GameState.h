#pragma once
#include "ProjectState.h"
class GameState : public ProjectState
{
	virtual bool Update(std::stack<std::shared_ptr<ProjectState>>& aProjectState) override;
	virtual void Render() override;

private:


};


