#pragma once
#include <iostream>
#include <stack>
#include <memory>

class ProjectState
{
public:
	virtual bool Update(std::stack<std::shared_ptr<ProjectState>>& aGameState) = 0;
	virtual void Render() = 0;
};