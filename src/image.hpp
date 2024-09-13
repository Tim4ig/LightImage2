
#pragma once

#include <d2d1.h>
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;

namespace li2
{
	struct Image
	{
		ComPtr<ID2D1Bitmap> bitmap;
		std::wstring path;
		bool isLoaded = false;
		bool isBroken = false;
	};
}
