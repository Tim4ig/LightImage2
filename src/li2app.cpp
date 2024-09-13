
#include "li2app.hpp"

#include <filesystem>

namespace li2
{
	constexpr auto PRELOAD_DISTANCE = 2;
	constexpr auto RELEASE_DISTANCE = 4;
	constexpr auto TRANSLATE_SPEED = 50;

	LightImage2App::LightImage2App()
	{
	}

	LightImage2App::~LightImage2App()
	{
		this->m_Release();
	}

	void LightImage2App::Run(int argc, const wchar_t** argv)
	{
		this->m_Init();
		this->m_SetImages(argc - 1, ++argv);

		while (m_window->IsRunning())
		{
			auto context = m_window->BeginDraw();
			context->Clear(D2D1::ColorF(D2D1::ColorF::White, 0));
			ID2D1Bitmap* bitmap = nullptr;
			if (m_currentImageIndex < m_images.size())
			{
				li2::Image& img = m_images[m_currentImageIndex];
				if (img.isLoaded && !img.isBroken)
					bitmap = img.bitmap.Get();
				this->m_DrawBitmap(bitmap, context);
			}
			m_window->EndDraw();
			std::this_thread::sleep_for(std::chrono::milliseconds(8));
		}

		this->m_Release();
	}

	void LightImage2App::m_Init()
	{
		m_window = std::make_unique<PrettyWindow>();

		m_window->SetMinSize({ 300, 300 });
		m_window->SetSize({ 800, 600 });
		m_window->SetTheme(WindowTheme::AUTO);
		m_window->SetBackdrop(WindowBackdrop::MICA);
		m_window->SetText(L"LightImage2");
		m_window->SetControllCallback([this](WindowControll controll, WindowControllEx controllEx) {this->m_ConrollHandler(controll, controllEx); });

		m_window->OpenAsync();

		auto context = m_window->BeginDraw();
		m_loader = std::make_unique<ImageLoader>(context);
		m_targetSize = context->GetSize();
		m_window->EndDraw();
	}

	void LightImage2App::m_Release()
	{
		m_loader.reset();
		m_window.reset();
		m_loadedImages.clear();
		m_images.clear();
	}

	void LightImage2App::m_SetImages(int argc, const wchar_t** argv)
	{
		m_currentImageIndex = 0;
		m_loadedImages.clear();
		m_images.clear();
		m_images.resize(argc);
		for (auto& img : m_images)
		{
			img.path = *argv++;
		}
		this->m_LoadImages();
	}

	void LightImage2App::m_LoadImages()
	{
		if (m_images.empty()) return;

		if (m_loadingTask.valid())
			m_loadingTask.wait();

		static int prevIndex = 0;
		if (prevIndex != m_currentImageIndex)
		{
			m_scale = 1.0f;
			m_offset = { 0.0f, 0.0f };
			prevIndex = m_currentImageIndex;
		}

		m_loadingTask = std::async(std::launch::async, [&]()
			{
				int count = m_images.size();

				m_loadedImages.erase(std::remove_if(m_loadedImages.begin(), m_loadedImages.end(),
					[&](const std::pair<Image*, int>& entry)
					{
						int index = entry.second;
						int distance = std::abs(index - m_currentImageIndex);

						distance = min(distance, count - distance);

						if (distance > RELEASE_DISTANCE)
						{
							entry.first->isLoaded = false;
							entry.first->bitmap = nullptr;
							return true;
						}
						return false;
					}), m_loadedImages.end());

				for (int i = m_currentImageIndex - PRELOAD_DISTANCE; i <= m_currentImageIndex + PRELOAD_DISTANCE; ++i)
				{
					int normalizedIndex = (i % count + count) % count;
					if (m_images[normalizedIndex].isLoaded || m_images[normalizedIndex].isBroken)
						continue;
					m_images[normalizedIndex] = m_loader->LoadImageW(m_images[normalizedIndex].path);
					m_loadedImages.emplace_back(std::make_pair(&m_images[normalizedIndex], normalizedIndex));
				}
			});

		{
			std::wstring title = (L"Image ");
			title += std::to_wstring(m_currentImageIndex + 1) + L" of " + std::to_wstring(m_images.size());
			title += L", " + std::filesystem::path(m_images.at(m_currentImageIndex).path).filename().wstring();
			if (m_images.size() > 0)
				m_window->SetText(title + ((m_images.at(m_currentImageIndex).isBroken) ? L"(broken)" : L""));
			else
				m_window->SetText(L"No images loaded");
		}
	}

	void LightImage2App::m_DrawBitmap(ID2D1Bitmap* bitmap, ID2D1DeviceContext* context)
	{
		if (!bitmap) return;

		auto size = bitmap->GetSize();
		auto wndScale = m_window->GetScale();

		size.width /= wndScale.x;
		size.height /= wndScale.y;

		float bitmapAspectRatio = size.width / size.height;
		float wndAspectRatio = m_targetSize.width / m_targetSize.height;

		float newWidth, newHeight;

		if (bitmapAspectRatio > wndAspectRatio)
		{
			newWidth = m_targetSize.width;
			newHeight = newWidth / bitmapAspectRatio;
		}
		else
		{
			newHeight = m_targetSize.height;
			newWidth = newHeight * bitmapAspectRatio;
		}

		float offsetX = (m_targetSize.width - newWidth) / 2.0f;
		float offsetY = (m_targetSize.height - newHeight) / 2.0f;

		auto dstRect = D2D1::RectF(
			offsetX,
			offsetY,
			offsetX + newWidth,
			offsetY + newHeight
		);

		auto destRect = D2D1::RectF(offsetX, offsetY, offsetX + newWidth, offsetY + newHeight);
		auto center = D2D1::Point2F((destRect.left + destRect.right) / 2, (destRect.top + destRect.bottom) / 2);
		auto mscale = D2D1::Matrix3x2F::Scale(D2D1::Size(m_scale, m_scale), center);

		auto offset = D2D1::Matrix3x2F::Translation(m_offset.x, m_offset.y);
		offset = mscale * offset;

		auto p1 = offset.TransformPoint(D2D1::Point2F(destRect.left, destRect.top));
		auto p2 = offset.TransformPoint(D2D1::Point2F(destRect.right, destRect.bottom));

		if (dstRect.left < p1.x)
			m_offset.x += dstRect.left - p1.x;
		if (dstRect.top < p1.y)
			m_offset.y += dstRect.top - p1.y;
		if (dstRect.right > p2.x)
			m_offset.x += dstRect.right - p2.x;
		if (dstRect.bottom > p2.y)
			m_offset.y += dstRect.bottom - p2.y;
		m_offset.x = (int)m_offset.x;
		m_offset.y = (int)m_offset.y;

		offset = D2D1::Matrix3x2F::Translation(m_offset.x, m_offset.y);
		mscale = mscale * offset;

		context->SetTransform(mscale);
		context->DrawBitmap(bitmap, dstRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
		context->SetTransform(D2D1::Matrix3x2F::Identity());
	}

	void LightImage2App::m_ConrollHandler(WindowControll controll, WindowControllEx controllEx)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_window->GetHwnd(), &pt);

		auto scale = m_window->GetScale();

		D2D1_POINT_2F cursorPos = D2D1::Point2F(
			static_cast<float>(pt.x) - m_targetSize.width * scale.x / 2.0f,
			static_cast<float>(pt.y) - m_targetSize.height * scale.y / 2.0f
		);

		D2D1_POINT_2F cursorRelativePos = D2D1::Point2F(
			(cursorPos.x - m_offset.x) / m_scale,
			(cursorPos.y - m_offset.y) / m_scale
		);

		switch (controll)
		{
		case WindowControll::SIZE:
			break;
		case WindowControll::DROP:
			this->m_SetImages(controllEx.argc, controllEx.argv);
			break;
		case WindowControll::NEXT:
			if (m_images.empty()) break;
			m_currentImageIndex = (m_currentImageIndex + 1) % m_images.size();
			break;
		case WindowControll::PREV:
			if (m_images.empty()) break;
			m_currentImageIndex = (m_currentImageIndex - 1 + m_images.size()) % m_images.size();
			break;
		case WindowControll::ZOOM_IN:
			this->m_SetScale(m_scale *= 1.1f);

			this->m_SetOffset(D2D1::Point2F(
				cursorPos.x - cursorRelativePos.x * m_scale,
				cursorPos.y - cursorRelativePos.y * m_scale
			));
			break;
		case WindowControll::ZOOM_OUT:
			this->m_SetScale(m_scale /= 1.1f);
			if (m_scale < 1)
				m_scale = 1;

			this->m_SetOffset(D2D1::Point2F(
				cursorPos.x - cursorRelativePos.x * m_scale,
				cursorPos.y - cursorRelativePos.y * m_scale
			));
			break;
		case WindowControll::OFFSET:
			this->m_SetOffset(D2D1::Point2F(m_offset.x + controllEx.offset.x / scale.x, m_offset.y + controllEx.offset.y / scale.y));
			break;
		}

		this->m_LoadImages();
	}

	void LightImage2App::m_SetScale(float scale)
	{
		m_scale = scale;
	}

	void LightImage2App::m_SetOffset(D2D1_POINT_2F offset)
	{
		m_offset = offset;
	}
}
