#pragma once
#include "ProjectState.h"
#include "../PerlinBase/PerlinImage.h"
class VisualState : public ProjectState
{
	virtual bool Update(std::stack<std::shared_ptr<ProjectState>>& aProjectState) override;
	virtual void Render() override;

private:

	void Save();

	struct PerlinInfo
	{
		double frequency = 1;
		int32_t octaves = 1;
		uint32_t seed;
		double persistance;
	};

	int fakePersistance = 1;

	PerlinInfo myPerlinInfo;

	Image image{ 512,512 };
};


