// SparkPerlin.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "PerlinBase/PerlinNoise.hpp"


int main()
{
    siv::PerlinNoise perlinA{ std::random_device{} };

	siv::BasicPerlinNoise<double> perlinB;
    
	for (std::int32_t y = 0; y < 20; ++y)
	{
		for (std::int32_t x = 0; x < 20; ++x)
		{
			
			const double noise = perlinB.octave2D_01(x * 0.1, y * 0.1, 6, 0.35);
			std::cout << static_cast<int>(std::floor(noise * 10) - 0.5);
		}
		std::cout << '\n';
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
