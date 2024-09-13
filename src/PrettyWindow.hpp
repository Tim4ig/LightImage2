
#pragma once

#ifndef UNICODE
#define UNICODE 1
#endif // !UNICODE

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_2helper.h>
#include <dcomp.h>
#include <wrl/client.h>
#include <future>
#include <string>

using Microsoft::WRL::ComPtr;

enum struct WindowTheme
{
	LIGHT,
	DARK,
	AUTO,
};

enum struct WindowBackdrop
{
	ACRYLIC,
	MICA,
	MICAALT,
};

enum struct WindowControll
{
	SIZE,
	DROP,
	NEXT,
	PREV,
	ZOOM_IN,
	ZOOM_OUT,
	OFFSET,
};

struct WindowControllEx
{
	int argc = 0;
	const wchar_t** argv = nullptr;
	POINT offset = {};
};

class PrettyWindow
{
public:
	PrettyWindow();
	~PrettyWindow();

	bool OpenAsync();
	void Close();

	ID2D1DeviceContext* BeginDraw();
	HRESULT EndDraw();

	void SetTheme(WindowTheme theme);
	void SetBackdrop(WindowBackdrop backdrop);
	void SetText(const std::wstring& text);
	void SetMinSize(POINT size);
	void SetSize(POINT size);
	void SetControllCallback(std::function<void(WindowControll, WindowControllEx)> callback);

	bool IsCursorLocked() const;
	D2D1_POINT_2F GetScale() const;
	HWND GetHwnd() const;

	bool IsRunning() const;
private:
	HWND m_hwnd = nullptr;
	bool m_isRunning = false;
	bool m_isResized = false;
	bool m_isCursorLocked = false;
	std::future<void> m_wndFuture;

	POINT m_screen = {};
	POINT m_minSize = { 400, 400 };
	POINT m_size = { 800, 600 };
	D2D1_POINT_2F m_scale = { 1.0f, 1.0f };
	WindowTheme m_theme = WindowTheme::AUTO;
	WindowBackdrop m_backdrop = WindowBackdrop::MICA;
	std::wstring m_text = L"Pretty Window";

	std::function<void(WindowControll, WindowControllEx)> m_controllCallback;

	ComPtr<ID2D1DeviceContext> m_d2dContext;
	ComPtr<IDCompositionDesktopDevice> m_dcompDevice;
	ComPtr<IDCompositionSurface> m_dcompSurface;
	ComPtr<IDCompositionTarget> m_dcompTarget;

	bool m_InitDComp();
	bool m_ApplyBackdrop();
	bool m_ApplyTheme();
	bool m_IsSystemDark();
	bool m_Resize();
	void m_MouseOffset();
	static LRESULT __stdcall m_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
