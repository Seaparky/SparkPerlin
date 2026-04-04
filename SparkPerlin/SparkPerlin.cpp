#include <iostream>
#include "States/MainState.h"



int main()
{

	std::stack<std::shared_ptr<ProjectState>> projectStack;

	projectStack.push(std::make_shared<MainState>());

	while (!projectStack.empty())
	{
		projectStack.top()->Render();
		projectStack.top()->Update(projectStack);
	}

}

