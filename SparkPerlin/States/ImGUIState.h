#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <tchar.h>
#include <dxgi.h>
#include "ProjectState.h"
#include "../PerlinBase/PerlinNoise.hpp"
#include "../PerlinBase/PerlinImage.h"

#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_dx11.h"
#include "../ImGUI/imgui_impl_win32.h"

class ImGUIState : public ProjectState
{
	virtual bool Update(std::stack<std::shared_ptr<ProjectState>>& aProjectState) override;
	virtual void Render() override;

private:

	void Save();

	bool myActive;


	struct PerlinInfo
	{
		double frequency;
		int32_t octaves;
		uint32_t seed;
		int persistance;
	};

	Image image;

	PerlinInfo myPerlinInfo;

	ImVec4 clear_color = ImVec4(0.45f, 0.85f, 0.60f, 1.00f);

};

