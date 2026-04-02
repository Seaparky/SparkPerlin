#include <iostream>
#include "PerlinBase/PerlinNoise.hpp"
#include "PerlinBase/PerlinImage.h"
#include <Windows.h>
#include <d3d11.h>
#include <tchar.h>
#include <dxgi.h>

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"


// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

Image image{ 512,512 };

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Forward declerations of custom functions
void Save(double aFrequency, int32_t aOctaves, uint32_t& aSeed);

int main()
{
	// Make process DPI aware and obtain main monitor scale
	ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

	// Create application window
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, (int)(1280 * main_scale), (int)(800 * main_scale), nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ImVec4 clear_color = ImVec4(0.45f, 0.85f, 0.60f, 1.00f);

	double frequency = 0;
	std::uint32_t seed = std::random_device{}();
	std::int32_t octaves = 0;

	bool done = false;
	while (!done)
	{

		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}

		// Handle window being minimized or screen locked
		if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			continue;
		}
		g_SwapChainOccluded = false;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Image Editor", &done, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
				if (ImGui::MenuItem("Save", "Ctrl+S")) { Save(frequency, octaves, seed); }
				if (ImGui::MenuItem("Close", "Ctrl+W")) { done = true; }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::InputDouble("Frequency", &frequency);
		ImGui::InputInt("Octaves", &octaves);
		ImGui::Text("Seed: %d", seed);
		if (ImGui::Button("ReSeed"))
		{
			seed = std::random_device{}();
		}

		ImGui::End();

		{
			ImVec2 window_pos = ImGui::GetWindowPos();
			ImVec2 window_size = ImGui::GetWindowSize();
			ImGui::GetForegroundDrawList()->AddRectFilled(window_pos, ImVec2{ window_pos.x + window_size.x, window_pos.y + window_size.y }, IM_COL32(255, 0, 0, 200));
			//ImGui::GetForegroundDrawList()->
		}

		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Present
		HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
		//HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
		g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

	}


	{
		//		std::cout << frequency << std::endl;
		//		std::cout << "double frequency = ";
		//		std::cin >> frequency;
		//		frequency = std::clamp(frequency, 0.1, 64.0);
		//
		//		std::cout << "int32 octaves    = ";
		//		std::cin >> octaves;
		//		octaves = std::clamp(octaves, 1, 16);
		//
		//		std::cout << "Enter '0' for a random uint32 seed      = ";
		//		std::cin >> seed;
		//
		//		double persistance;
		//		std::cout << "0-100: The smaller the number the higher the correlation between values\nEnter Persistance      = ";
		//		std::cin >> persistance;
		//		persistance = std::lerp(0, 1, (persistance / 100));
		//
		//
		//		if (seed == 0) { seed = std::random_device{}(); }
		//
		//		siv::PerlinNoise perlinA{ seed };
		//
		//		//	for (std::int32_t y = 0; y < 10; ++y)
		//		//	{
		//		//		for (std::int32_t x = 0; x < 10; ++x)
		//		//		{
		//		//			const double noise = perlinA.octave2D_01(x * 0.1, y * 0.1, octaves, persistance);
		//		//			std::cout << static_cast<int>(std::floor(noise * 10) - 0.5) << "\t";
		//		//		}
		//		//		std::cout << '\n';
		//		//	}
		//
		//		Image image{ 512,512 };
		//
		//		const double xFrequency = (frequency / image.width());
		//		const double yFrequency = (frequency / image.height());
		//
		//		for (std::int32_t y = 0; y < image.height(); ++y)
		//		{
		//			for (std::int32_t x = 0; x < image.width(); ++x)
		//			{
		//				const RGB color(perlinA.octave2D_01((x * xFrequency), (y * yFrequency), octaves));
		//				image.set(x, y, color);
		//			}
		//		}
		//
		//		std::stringstream ss;
		//		ss << 'f' << frequency << 'o' << octaves << '_' << seed << ".bmp";
		//
		//		if (image.saveBMP(ss.str()))
		//		{
		//			std::cout << "...saved \"" << ss.str() << "\"\n";
		//		}
		//
	}

}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	// This is a basic setup. Optimally could use e.g. DXGI_SWAP_EFFECT_FLIP_DISCARD and handle fullscreen mode differently. See #8979 for suggestions.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


// Custom functions

void Save(double aFrequency, int32_t aOctaves, uint32_t& aSeed)
{
	if (aSeed == 0) { aSeed = std::random_device{}(); }

	siv::PerlinNoise perlinA{ aSeed };

	const double xFrequency = (aFrequency / image.width());
	const double yFrequency = (aFrequency / image.height());

	for (std::int32_t y = 0; y < image.height(); ++y)
	{
		for (std::int32_t x = 0; x < image.width(); ++x)
		{
			const RGB color(perlinA.octave2D_01((x * xFrequency), (y * yFrequency), aOctaves));
			image.set(x, y, color);
		}
	}

	std::stringstream ss;
	ss << 'f' << aFrequency << 'o' << aOctaves << '_' << aSeed << ".png";

	image.savePNG(ss.str());

	aSeed = std::random_device{}();
}


