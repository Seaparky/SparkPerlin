#include "GameState.h"

void GameState::Render()
{
	std::cout << "Welcome to Diablo";
}

bool GameState::Update(std::stack<std::shared_ptr<ProjectState>>& aProjectState)
{
	aProjectState.pop();
	return true;
}

