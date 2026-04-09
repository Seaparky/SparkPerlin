#include "MainState.h"
#include "ConsoleState.h"
#include "VisualState.h"
#include "GameState.h"

bool MainState::Update(std::stack<std::shared_ptr<ProjectState>>& aProjectState)
{
	int x;
	std::cin >> x;
	chosen = static_cast<Choice>(x);

	switch (chosen)
	{
	case MainState::Choice::Empty:

		return true;
	case MainState::Choice::Console:
		aProjectState.push(std::make_shared<ConsoleState>());
		return true;
	case MainState::Choice::ImGUI:
		aProjectState.push(std::make_shared<VisualState>());
		return true;
	case MainState::Choice::Play:
		aProjectState.push(std::make_shared<GameState>());
		return true;
	case MainState::Choice::Quit:
		aProjectState.pop();
		return false;
	}
		return false;


}

void MainState::Render()
{
	system("cls");
	std::cout << "--- Perlin MAIN MENU ---\n";
	std::cout << "1. Console\n";
	std::cout << "2. ImGUI\n";
	std::cout << "3. Play\n";
	std::cout << "4. Quit\n";
}
