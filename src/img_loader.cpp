
#include "img_loader.hpp"

#include "shared.hpp"

namespace li2
{
	ImageLoader::ImageLoader(ComPtr<ID2D1DeviceContext> target)
	{
		this->m_InitWIC();
		m_RenderTarget = target;
	}

	ImageLoader::~ImageLoader()
	{
	}

	Image ImageLoader::LoadImageW(std::wstring path)
	{
		logger.Log("ImageLoader::LoadImageW() called, with args:", logger.WtoA(path));

		Image img;
		HRESULT hr = S_OK;

		img.path = path;

		try
		{
			ComPtr<IWICBitmapDecoder> decoder;
			ComPtr<IWICBitmapFrameDecode> frame;
			ComPtr<IWICFormatConverter> converter;

			hr = m_WICFactory->CreateDecoderFromFilename(
				path.c_str(),
				nullptr,
				GENERIC_READ,
				WICDecodeMetadataCacheOnLoad,
				&decoder
			) TIF;

			hr = decoder->GetFrame(0, &frame) TIF;

			hr = m_WICFactory->CreateFormatConverter(&converter) TIF;

			hr = converter->Initialize(
				frame.Get(),
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				nullptr,
				0.0f,
				WICBitmapPaletteTypeMedianCut
			) TIF;

			hr = m_RenderTarget->CreateBitmapFromWicBitmap(
				converter.Get(),
				nullptr,
				&img.bitmap
			) TIF;
		}
		catch (const std::exception& e)
		{
			logger.Error("Cant load image: ", e.what());
			img.bitmap = nullptr;
			img.isBroken = true;
			return img;
		}

		img.isLoaded = true;
		return img;
	}

	void ImageLoader::m_InitWIC()
	{
		HRESULT hr = S_OK;
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			&m_WICFactory
		) TIF;
	}
}
