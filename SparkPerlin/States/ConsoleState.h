#pragma once
#include "ProjectState.h"
#include "../PerlinBase/PerlinNoise.hpp"
#include "../PerlinBase/PerlinImage.h"
class ConsoleState : public ProjectState
{
	virtual bool Update(std::stack<std::shared_ptr<ProjectState>>& aProjectState) override;
	virtual void Render() override;

private:

	void Save();

	struct PerlinInfo
	{
		double frequency;
		int32_t octaves;
		uint32_t seed;
		int persistance;
	};

	Image image;

	PerlinInfo myPerlinInfo;

	int myProgress = 0;

};

