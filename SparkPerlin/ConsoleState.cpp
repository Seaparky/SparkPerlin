#include "States/ConsoleState.h"

bool ConsoleState::Update(std::stack<std::shared_ptr<ProjectState>>& aProjectState)
{
	switch (myProgress)
	{
	case 0:
		std::cin >> myPerlinInfo.frequency;
		myPerlinInfo.frequency = std::clamp(myPerlinInfo.frequency, 0.1, 64.0);
		myProgress++;
		return false;
	case 1:
		std::cin >> myPerlinInfo.octaves;
		myPerlinInfo.octaves = std::clamp(myPerlinInfo.octaves, 1, 16);
		myProgress++;
		return false;
	case 2:
		std::cin >> myPerlinInfo.persistance;
		myPerlinInfo.persistance = std::lerp(0, 1, (myPerlinInfo.persistance / 100));
		myProgress++;
		return false;
	case 3:
		std::cin >> myPerlinInfo.seed;
		Save();
		myProgress++;
		return false;
	case 4:
		system("pause");
		myProgress = 0;
		aProjectState.pop();
		return false;
	}
}

void ConsoleState::Render()
{
	system("cls");
	std::cout << "--- CONSOLE ---\n";

	switch (myProgress)
	{
	case 0:
		std::cout << "double frequency = ";
		break;
	case 1:
		std::cout << "int32 octaves    = ";
		break;
	case 2:
		std::cout << "0-100: The smaller the number the higher the correlation between values\nEnter Persistance      = ";
		break;
	case 3:
		std::cout << "Enter '0' for a random uint32 seed = ";
		break;
	case 4:
		std::cout << "\n--- IMAGE SAVED ---\n\n";
		break;
	}

}


void ConsoleState::Save()
{
	if (myPerlinInfo.seed == 0) { myPerlinInfo.seed = std::random_device{}(); }

	siv::PerlinNoise perlinA{ myPerlinInfo.seed };

	const double xFrequency = (myPerlinInfo.frequency / image.width());
	const double yFrequency = (myPerlinInfo.frequency / image.height());

	for (std::int32_t y = 0; y < image.height(); ++y)
	{
		for (std::int32_t x = 0; x < image.width(); ++x)
		{
			const RGB color(perlinA.octave2D_01((x * xFrequency), (y * yFrequency), myPerlinInfo.octaves, myPerlinInfo.persistance));
			image.set(x, y, color);
		}
	}

	std::stringstream ss;
	ss << 'f' << myPerlinInfo.frequency << 'o' << myPerlinInfo.octaves << '_' << myPerlinInfo.seed << ".png";

	image.savePNG(ss.str());

	myPerlinInfo.seed = std::random_device{}();
}


