// SparkPerlin.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "PerlinBase/PerlinNoise.hpp"
#include "PerlinImage.h"


int main()
{
	siv::PerlinNoise perlinA{ std::random_device{} };

	siv::BasicPerlinNoise<double> perlinB;

	for (std::int32_t y = 0; y < 10; ++y)
	{
		for (std::int32_t x = 0; x < 10; ++x)
		{
			const double noise = perlinA.octave2D_01(x * 0.1, y * 0.1, 6, 0.45);
			std::cout << static_cast<int>(std::floor(noise * 10) - 0.5) << "\t";
		}
		std::cout << '\n';
	}

	Image image{ 512,512 };

	double frequency = std::clamp(0.5, 0.1, 64.0);

	std::int32_t octaves = std::clamp(24, 1, 16);


	const double xFrequency = (frequency / image.width());
	const double yFrequency = (frequency / image.height());

	for (std::int32_t y = 0; y < image.height(); ++y)
	{
		for (std::int32_t x = 0; x < image.width(); ++x)
		{
			const RGB color(perlinA.octave2D_01((x * xFrequency), (y * yFrequency), octaves));
			image.set(x, y, color);
		}
	}

	std::stringstream ss;
	ss << 'f' << frequency << 'o' << octaves << ".bmp";

	if (image.saveBMP(ss.str()))
	{
		std::cout << "...saved \"" << ss.str() << "\"\n";
	}

	//for (std::int32_t x = 0; x < 10; ++x)
	//{
	//	const double noise = perlinA.octave1D_01(x * 0.1, 6, 0.35);
	//	std::cout << static_cast<int>(std::floor(noise * 10) - 0.5) << "\t";
	//}
	//std::cout << '\n';

}

