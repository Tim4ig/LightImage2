
#pragma once

#include "PrettyWindow.hpp"
#include "img_loader.hpp"
#include "shared.hpp"

#include <vector>
#include <future>

namespace li2
{
	class LightImage2App
	{
	public:
		LightImage2App();
		~LightImage2App();

		void Run(int argc, const wchar_t** argv);
	private:
		std::unique_ptr<PrettyWindow> m_window;
		std::unique_ptr<li2::ImageLoader> m_loader;
		D2D1_SIZE_F m_targetSize = { 0.0f, 0.0f };

		int m_currentImageIndex = 0;
		std::vector<li2::Image> m_images;
		std::vector<std::pair<li2::Image*, int>> m_loadedImages;
		std::future<void> m_loadingTask;

		float m_scale = 1.0f;
		D2D1_POINT_2F m_offset = { 0.0f, 0.0f };

		void m_Init();
		void m_Release();
		void m_SetImages(int argc, const wchar_t** argv);
		void m_LoadImages();
		void m_DrawBitmap(ID2D1Bitmap* bitmap, ID2D1DeviceContext* context);
		void m_ConrollHandler(WindowControll controll, WindowControllEx controllEx);

		void m_SetScale(float scale);
		void m_SetOffset(D2D1_POINT_2F offset);
	};
}
