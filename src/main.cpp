
#include "li2app.hpp"

#include <memory>

int wmain(int argc, const wchar_t ** argv) try
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
