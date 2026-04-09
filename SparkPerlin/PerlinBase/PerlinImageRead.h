#pragma once
#include "PerlinImage.h"

class PerlinImageReader
{
private:

	Image ImgCopy{ 512,512 };

	struct PerlinInfo
	{
		double frequency;
		int32_t octaves;
		uint32_t seed;
		double persistance;
	};

	PerlinInfo ImgCopyInfo;

	std::vector<int> m_data;
	std::array<int, 512 * 512> I_data;

public:
	void SetImage(const std::string& aDirectory)
	{
		int seedStart = aDirectory.find("_") + 1;
		int seedEnd = aDirectory.find(".png") - 4;

		int frequencyStart = aDirectory.find("f") + 1;
		int frequencyEnd = aDirectory.find("o") - 1;

		int octavesStart = aDirectory.find("o") + 1;
		int octavesEnd = aDirectory.find("_") - 1;


		ImgCopyInfo.seed = aDirectory[seedStart, seedEnd];
		ImgCopyInfo.frequency = aDirectory[frequencyStart, frequencyEnd];
		ImgCopyInfo.octaves = aDirectory[octavesStart, octavesEnd];

	}

	void SetImage(Image* aBaseImage)
	{
		ImgCopy = *aBaseImage;



		siv::PerlinNoise perlinA {};

		for (std::int32_t y = 0; y < ImgCopy.height(); ++y)
		{
			for (std::int32_t x = 0; x < ImgCopy.width(); ++x)
			{
				const double noise = perlinA.octave2D_01(x * 0.1, y * 0.1, 6);
				m_data[0] = static_cast<int>(std::floor(noise * 10) - 0.5);
			}
		}
	}


};


