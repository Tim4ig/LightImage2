
#pragma once

#include <d2d1.h>
#include <d2d1_2helper.h>
#include <wincodec.h>
#include <string>
#include <wrl/client.h>

#include "image.hpp"

using namespace Microsoft::WRL;

namespace li2
{
	class ImageLoader
	{
	public:
		ImageLoader(ComPtr<ID2D1DeviceContext> target);
		~ImageLoader();

		Image LoadImageW(std::wstring path);
	private:
		ComPtr<IWICImagingFactory> m_WICFactory;
		ComPtr<ID2D1DeviceContext> m_RenderTarget;

		void m_InitWIC();
	};
}
