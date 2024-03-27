#pragma once
#include "Windows.h"
namespace rage { class ConstString; }

class dds {
public:
	static const DWORD DDS_MAGIC = 0x20534444; // "DDS "
	static const DWORD DDS_DX9_HEADER_SIZE = 128;

	DWORD dwMagic;


	DWORD dwSize;
	//union {
	//	struct {
	//		DWORD DDSD_CAPS : 1;
	//		DWORD DDSD_HEIGHT : 1;
	//		DWORD DDSD_WIDTH : 1;
	//		DWORD DDSD_PITCH : 1;
	//	};
	//	DWORD dwFlags;
	//};
	DWORD dwFlags;
	DWORD dwHeight;
	DWORD dwWidth;
	DWORD dwPitchOrLinearSize;
	DWORD dwDepth;
	DWORD dwMipMapCount;
	DWORD dwReserved1[11];
	struct DDS_PIXELFORMAT {
		DWORD dwSize;
		DWORD dwFlags;
		DWORD dwFourCC;
		DWORD dwRGBBitCount;
		DWORD dwRBitMask;
		DWORD dwGBitMask;
		DWORD dwBBitMask;
		DWORD dwABitMask;

		bool operator==(DDS_PIXELFORMAT rhs) {
			if (dwSize != rhs.dwSize) return 0;
			if (dwFlags != rhs.dwFlags) return 0;
			if (dwFourCC != rhs.dwFourCC) return 0;
			if (dwRGBBitCount != rhs.dwRGBBitCount) return 0;
			if (dwRBitMask != rhs.dwRBitMask) return 0;
			if (dwGBitMask != rhs.dwGBitMask) return 0;
			if (dwBBitMask != rhs.dwBBitMask) return 0;
			if (dwABitMask != rhs.dwABitMask) return 0;
			return 1;
		}
	} ddspf;
	DWORD dwCaps;
	DWORD dwCaps2;
	DWORD dwCaps3;
	DWORD dwCaps4;
	DWORD dwReserved2;

	BYTE*			pPixelData;

	enum ddsFlags : DWORD {
		FOURCC			= 0x00000004,  // DDPF_FOURCC
		RGB				= 0x00000040,  // DDPF_RGB
		RGBA			= 0x00000041,  // DDPF_RGB | DDPF_ALPHAPIXELS
		LUMINANCE		= 0x00020000,  // DDPF_LUMINANCE
		LUMINANCEA		= 0x00020001,  // DDPF_LUMINANCE | DDPF_ALPHAPIXELS
		ALPHAPIXELS		= 0x00000001,  // DDPF_ALPHAPIXELS
		ALPHA			= 0x00000002,  // DDPF_ALPHA
		PAL8			= 0x00000020,  // DDPF_PALETTEINDEXED8
		PAL8A			= 0x00000021,  // DDPF_PALETTEINDEXED8 | DDPF_ALPHAPIXELS
		BUMPDUDV		= 0x00080000,  // DDPF_BUMPDUDV
		BUMPLUMINANCE	= 0x00040000
	};

private:
	// dds formats
	/*
	DDS_PIXELFORMAT DDSPF_DXT1 = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('D','X','T','1'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_DXT2 = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('D','X','T','2'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_DXT3 = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('D','X','T','3'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_DXT4 = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('D','X','T','4'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_DXT5 = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('D','X','T','5'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_BC4_UNORM = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('B','C','4','U'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_BC4_SNORM = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('B','C','4','S'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_BC5_UNORM = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('B','C','5','U'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_BC5_SNORM = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('B','C','5','S'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_R8G8_B8G8 = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('R','G','B','G'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_G8R8_G8B8 = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('G','R','G','B'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_YUY2 = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('Y','U','Y','2'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_UYVY = { 0x20, ddsFlags::FOURCC, MAKEFOURCC('U','Y','V','Y'), 0, 0, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_A16B16G16R16 = { 0x20, ddsFlags::FOURCC, 0x00000024, 0, 0, 0, 0, 0 }; // why 64bit dds is fourcc
	DDS_PIXELFORMAT DDSPF_A8R8G8B8 = { 0x20, ddsFlags::RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };
	DDS_PIXELFORMAT DDSPF_X8R8G8B8 = { 0x20, ddsFlags::RGB,  0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0 };
	DDS_PIXELFORMAT DDSPF_A8B8G8R8 = { 0x20, ddsFlags::RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };
	DDS_PIXELFORMAT DDSPF_X8B8G8R8 = { 0x20, ddsFlags::RGB,  0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0 };
	DDS_PIXELFORMAT DDSPF_G16R16 = { 0x20, ddsFlags::RGB,  0, 32, 0x0000ffff, 0xffff0000, 0, 0 };
	DDS_PIXELFORMAT DDSPF_R5G6B5 = { 0x20, ddsFlags::RGB, 0, 16, 0xf800, 0x07e0, 0x001f, 0 };
	DDS_PIXELFORMAT DDSPF_A1R5G5B5 = { 0x20, ddsFlags::RGBA, 0, 16, 0x7c00, 0x03e0, 0x001f, 0x8000 };
	DDS_PIXELFORMAT DDSPF_X1R5G5B5 = { 0x20, ddsFlags::RGB, 0, 16, 0x7c00, 0x03e0, 0x001f, 0 };
	DDS_PIXELFORMAT DDSPF_A4R4G4B4 = { 0x20, ddsFlags::RGBA, 0, 16, 0x0f00, 0x00f0, 0x000f, 0xf000 };
	DDS_PIXELFORMAT DDSPF_X4R4G4B4 = { 0x20, ddsFlags::RGB, 0, 16, 0x0f00, 0x00f0, 0x000f, 0 };
	DDS_PIXELFORMAT DDSPF_R8G8B8 = { 0x20, ddsFlags::RGB, 0, 24, 0xff0000, 0x00ff00, 0x0000ff, 0 };
	DDS_PIXELFORMAT DDSPF_A8R3G3B2 = { 0x20, ddsFlags::RGBA, 0, 16, 0x00e0, 0x001c, 0x0003, 0xff00 };
	DDS_PIXELFORMAT DDSPF_R3G3B2 = { 0x20, ddsFlags::RGB, 0, 8, 0xe0, 0x1c, 0x03, 0 };
	DDS_PIXELFORMAT DDSPF_A4L4 = { 0x20, ddsFlags::LUMINANCEA, 0, 8, 0x0f, 0, 0, 0xf0 };
	DDS_PIXELFORMAT DDSPF_L8 = { 0x20, ddsFlags::LUMINANCE, 0, 8, 0xff, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_L16 = { 0x20, ddsFlags::LUMINANCE, 0, 16, 0xffff, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_A8L8 = { 0x20, ddsFlags::LUMINANCEA, 0, 16, 0x00ff, 0, 0, 0xff00 };
	DDS_PIXELFORMAT DDSPF_A8L8_ALT = { 0x20, ddsFlags::LUMINANCEA, 0, 8, 0x00ff, 0, 0, 0xff00 };
	DDS_PIXELFORMAT DDSPF_L8_NVTT1 = { 0x20, ddsFlags::RGB, 0, 8, 0xff, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_L16_NVTT1 = { 0x20, ddsFlags::RGB, 0, 16, 0xffff, 0, 0, 0 };
	DDS_PIXELFORMAT DDSPF_A8L8_NVTT1 = { 0x20, ddsFlags::RGBA, 0, 16, 0x00ff, 0, 0, 0xff00 };
	DDS_PIXELFORMAT DDSPF_A8 = { 0x20, ddsFlags::ALPHA, 0, 8, 0, 0, 0, 0xff };
	DDS_PIXELFORMAT DDSPF_V8U8 = { 0x20, ddsFlags::BUMPDUDV, 0, 16, 0x00ff, 0xff00, 0, 0 };
	DDS_PIXELFORMAT DDSPF_Q8W8V8U8 = { 0x20, ddsFlags::BUMPDUDV, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };
	DDS_PIXELFORMAT DDSPF_V16U16 = { 0x20, ddsFlags::BUMPDUDV, 0, 32, 0x0000ffff, 0xffff0000, 0, 0 };
	DDS_PIXELFORMAT DDSPF_A2R10G10B10 = { 0x20, ddsFlags::RGBA, 0, 32, 0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000 };
	DDS_PIXELFORMAT DDSPF_A2B10G10R10 = { 0x20, ddsFlags::RGBA, 0, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 };
	*/
	struct uni {
		BYTE r, g, b, a, l;
	};

public:
	enum ePixelFormat : int {
		DXT1 = 1,
		DXT2,
		DXT3,
		DXT4,
		DXT5,
		X8R8G8B8,
		A8R8G8B8,
		L8,
		R8G8B8,
		R5G6B5,
		X1R5G5B5,
		A1R5G5B5,
		A4R4G4B4,
		R3G3B2,
		A8,
		A8R3G3B2,
		X4R4G4B4,
		A2B10G10R10,
		A8B8G8R8,
		X8B8G8R8,
		G16R16,
		A2R10G10B10,
		A16B16G16R16,
		A8L8,
		A4L4
	};

	static ePixelFormat getDdsFormat(DWORD dwRagePixelFormat);
	static DWORD getRagePixelFormat(ePixelFormat eDdsPixelFormat);
	rage::ConstString getFormat(DWORD ddsFormat);
	DWORD getStride();
	DWORD getPixelDataSize();
	DWORD getFirstMipSize();
	ePixelFormat getThisDdsFormat();
	rage::ConstString getThisDdsFormatAsText();
	bool isLoadedSupportedFormat();

	int loadFromMemory(BYTE* pDdsBuf);
	int loadFromFile(const char* pszFilePath);
	int saveToMemory(BYTE* pDdsBuf);
	int saveToFile(const char* pszFilePath);
	DWORD getDdsSize();
	DWORD getTotalPixelsCount();

	void convertDXT1ToUni();

	dds();
	dds(const char *pszPath);
	dds(BYTE *pFile);
	~dds();
};

