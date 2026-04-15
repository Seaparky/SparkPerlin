#pragma once
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include "PerlinNoise.hpp"

#include <d3d11.h>

#pragma pack (push, 1)
struct BMPHeader
{
	std::uint16_t bfType;
	std::uint32_t bfSize;
	std::uint16_t bfReserved1;
	std::uint16_t bfReserved2;
	std::uint32_t bfOffBits;
	std::uint32_t biSize;
	std::int32_t  biWidth;
	std::int32_t  biHeight;
	std::uint16_t biPlanes;
	std::uint16_t biBitCount;
	std::uint32_t biCompression;
	std::uint32_t biSizeImage;
	std::int32_t  biXPelsPerMeter;
	std::int32_t  biYPelsPerMeter;
	std::uint32_t biClrUsed;
	std::uint32_t biClrImportant;
};

static_assert(sizeof(BMPHeader) == 54);
#pragma pack (pop)

struct PerlinRGB
{
	double r = 0.0;
	double g = 0.0;
	double b = 0.0;
	constexpr PerlinRGB() = default;
	explicit constexpr PerlinRGB(double _rgb) noexcept
		: r{ _rgb }, g{ _rgb }, b{ _rgb } {}
	constexpr PerlinRGB(double _r, double _g, double _b) noexcept
		: r{ _r }, g{ _g }, b{ _b } {}
};

struct RGBA
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	RGBA operator=(const PerlinRGB& aPRGB)
	{
		r = (uint8_t)(aPRGB.r * 255.0);
		g = (uint8_t)(aPRGB.g * 255.0);
		b = (uint8_t)(aPRGB.b * 255.0);
		a = 255;
		return *this;
	}
};

class Image
{
public:

	Image() = default;

	Image(std::size_t width, std::size_t height)
		: m_data(width* height)
		, m_width{ static_cast<std::int32_t>(width) }
		, m_height{ static_cast<std::int32_t>(height) } {}

	Image(Image& aCopy)
		: m_data(aCopy.m_data)
		, m_width(aCopy.m_width)
		, m_height(aCopy.m_height) {};

	void set(std::int32_t x, std::int32_t y, const PerlinRGB& color)
	{
		if (!inBounds(y, x))
		{
			return;
		}

		m_data[static_cast<std::size_t>(y) * m_width + x] = color;
	}

	void CreateDXTextureResource( ID3D11Device* aDevice, ID3D11Texture2D** outResource, ID3D11ShaderResourceView** outSrv )
	{
		ID3D11Texture2D* pResource = *outResource;
		ID3D11ShaderResourceView* pSRV = *outSrv;
		if (pResource) pResource->Release();
		if (pSRV) pSRV->Release();

		D3D11_TEXTURE2D_DESC t2dDesc = {};
		t2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		t2dDesc.ArraySize = 1;
		t2dDesc.CPUAccessFlags = 0;
		t2dDesc.Width = m_width;
		t2dDesc.Height = m_height;
		t2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		t2dDesc.MipLevels = 1;
		t2dDesc.MiscFlags = 0;
		t2dDesc.Usage = D3D11_USAGE_IMMUTABLE;
		t2dDesc.SampleDesc.Count = 1;
		t2dDesc.SampleDesc.Quality = 0;

		std::vector<RGBA> pixels;
		pixels.reserve(m_data.size());

		for (const auto& prgb : m_data)
		{
			pixels.emplace_back() = prgb;
		}

		D3D11_SUBRESOURCE_DATA imageData = {};
		imageData.pSysMem = pixels.data();
		imageData.SysMemPitch = sizeof(RGBA) * t2dDesc.Width;
		imageData.SysMemSlicePitch = 0;

		aDevice->CreateTexture2D(&t2dDesc, &imageData, &pResource);


		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		aDevice->CreateShaderResourceView(pResource, &srvDesc, &pSRV);

		(*outSrv) = pSRV;
		(*outResource) = pResource;
	}

	std::int32_t width() const noexcept { return m_width; }

	std::int32_t height() const noexcept { return m_height; }

	bool savePNG(const std::string& path)
	{
		const std::int32_t  rowSize = m_width * 3 + m_width % 4;
		const std::uint32_t bmpsize = rowSize * m_height;
		const BMPHeader header =
		{
			0x4d42,
			static_cast<std::uint32_t>(bmpsize + sizeof(BMPHeader)),
			0, 0, sizeof(BMPHeader), 40,
			m_width, m_height, 1, 24,
			0, bmpsize, 0, 0, 0, 0
		};

		if (std::ofstream ofs{ path, std::ios_base::binary })
		{
			ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));

			std::vector<std::uint8_t> line(rowSize);

			for (std::int32_t y = m_height - 1; -1 < y; --y)
			{
				size_t pos = 0;

				for (std::int32_t x = 0; x < m_width; ++x)
				{
					const PerlinRGB& col = m_data[static_cast<std::size_t>(y) * m_width + x];
					line[pos++] = ToUint8(col.b);
					line[pos++] = ToUint8(col.g);
					line[pos++] = ToUint8(col.r);
				}

				ofs.write(reinterpret_cast<const char*>(line.data()), line.size());
			}

			return true;
		}
		else
		{
			return false;
		}
	}

private:

	std::vector<PerlinRGB> m_data;

	std::int32_t m_width = 0, m_height = 0;

	bool inBounds(std::int32_t y, std::int32_t x) const noexcept
	{
		return (0 <= y) && (y < m_height) && (0 <= x) && (x < m_width);
	}

	static constexpr std::uint8_t ToUint8(double x) noexcept
	{
		return (x <= 0.0) ? 0 : (1.0 <= x) ? 255 : static_cast<std::uint8_t>(x * 255.0 + 0.5);
	}
};

