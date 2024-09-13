
#include "li2app.hpp"

#include <memory>
#include <shellapi.h>

int wmain(int argc, const wchar_t** argv) try
{
	if (FAILED(CoInitialize(nullptr))) THROW("CoInitialize error");

	auto app = std::make_unique<li2::LightImage2App>();
	app->Run(argc, argv);

	CoUninitialize();
	return 0;
}
catch (const std::exception& e)
{
	li2::logger.Log("EXCEPTION:", e.what());
	MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR);
	return 1;
}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	int argc = 0;
	WCHAR** argv = CommandLineToArgvW(lpCmdLine, &argc);
	int result = wmain(argc + 1, const_cast<const wchar_t**>(--argv));
	LocalFree(argv);
	return result;
}
