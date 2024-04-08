#include "dds.h"
#include "rage_string.h"
#include <math.h>
#include <iostream>
#include "memory_manager.h"

#undef max, min

// dds formats
dds::DDS_PIXELFORMAT DDSPF_DXT1 = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('D','X','T','1'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_DXT2 = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('D','X','T','2'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_DXT3 = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('D','X','T','3'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_DXT4 = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('D','X','T','4'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_DXT5 = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('D','X','T','5'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_BC4_UNORM = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('B','C','4','U'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_BC4_SNORM = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('B','C','4','S'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_BC5_UNORM = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('B','C','5','U'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_BC5_SNORM = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('B','C','5','S'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_R8G8_B8G8 = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('R','G','B','G'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_G8R8_G8B8 = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('G','R','G','B'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_YUY2 = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('Y','U','Y','2'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_UYVY = { 0x20, dds::ddsFlags::FOURCC, MAKEFOURCC('U','Y','V','Y'), 0, 0, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_A16B16G16R16 = { 0x20, dds::ddsFlags::FOURCC, 0x00000024, 0, 0, 0, 0, 0 }; // why 64bit dds is fourcc
dds::DDS_PIXELFORMAT DDSPF_A8R8G8B8 = { 0x20, dds::ddsFlags::RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };
dds::DDS_PIXELFORMAT DDSPF_X8R8G8B8 = { 0x20, dds::ddsFlags::RGB,  0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0 };
dds::DDS_PIXELFORMAT DDSPF_A8B8G8R8 = { 0x20, dds::ddsFlags::RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };
dds::DDS_PIXELFORMAT DDSPF_X8B8G8R8 = { 0x20, dds::ddsFlags::RGB,  0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0 };
dds::DDS_PIXELFORMAT DDSPF_G16R16 = { 0x20, dds::ddsFlags::RGB,  0, 32, 0x0000ffff, 0xffff0000, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_R5G6B5 = { 0x20, dds::ddsFlags::RGB, 0, 16, 0xf800, 0x07e0, 0x001f, 0 };
dds::DDS_PIXELFORMAT DDSPF_A1R5G5B5 = { 0x20, dds::ddsFlags::RGBA, 0, 16, 0x7c00, 0x03e0, 0x001f, 0x8000 };
dds::DDS_PIXELFORMAT DDSPF_X1R5G5B5 = { 0x20, dds::ddsFlags::RGB, 0, 16, 0x7c00, 0x03e0, 0x001f, 0 };
dds::DDS_PIXELFORMAT DDSPF_A4R4G4B4 = { 0x20, dds::ddsFlags::RGBA, 0, 16, 0x0f00, 0x00f0, 0x000f, 0xf000 };
dds::DDS_PIXELFORMAT DDSPF_X4R4G4B4 = { 0x20, dds::ddsFlags::RGB, 0, 16, 0x0f00, 0x00f0, 0x000f, 0 };
dds::DDS_PIXELFORMAT DDSPF_R8G8B8 = { 0x20, dds::ddsFlags::RGB, 0, 24, 0xff0000, 0x00ff00, 0x0000ff, 0 };
dds::DDS_PIXELFORMAT DDSPF_A8R3G3B2 = { 0x20, dds::ddsFlags::RGBA, 0, 16, 0x00e0, 0x001c, 0x0003, 0xff00 };
dds::DDS_PIXELFORMAT DDSPF_R3G3B2 = { 0x20, dds::ddsFlags::RGB, 0, 8, 0xe0, 0x1c, 0x03, 0 };
dds::DDS_PIXELFORMAT DDSPF_A4L4 = { 0x20, dds::ddsFlags::LUMINANCEA, 0, 8, 0x0f, 0, 0, 0xf0 };
dds::DDS_PIXELFORMAT DDSPF_L8 = { 0x20, dds::ddsFlags::LUMINANCE, 0, 8, 0xff, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_L16 = { 0x20, dds::ddsFlags::LUMINANCE, 0, 16, 0xffff, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_A8L8 = { 0x20, dds::ddsFlags::LUMINANCEA, 0, 16, 0x00ff, 0, 0, 0xff00 };
dds::DDS_PIXELFORMAT DDSPF_A8L8_ALT = { 0x20, dds::ddsFlags::LUMINANCEA, 0, 8, 0x00ff, 0, 0, 0xff00 };
dds::DDS_PIXELFORMAT DDSPF_L8_NVTT1 = { 0x20, dds::ddsFlags::RGB, 0, 8, 0xff, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_L16_NVTT1 = { 0x20, dds::ddsFlags::RGB, 0, 16, 0xffff, 0, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_A8L8_NVTT1 = { 0x20, dds::ddsFlags::RGBA, 0, 16, 0x00ff, 0, 0, 0xff00 };
dds::DDS_PIXELFORMAT DDSPF_A8 = { 0x20, dds::ddsFlags::ALPHA, 0, 8, 0, 0, 0, 0xff };
dds::DDS_PIXELFORMAT DDSPF_V8U8 = { 0x20, dds::ddsFlags::BUMPDUDV, 0, 16, 0x00ff, 0xff00, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_Q8W8V8U8 = { 0x20, dds::ddsFlags::BUMPDUDV, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };
dds::DDS_PIXELFORMAT DDSPF_V16U16 = { 0x20, dds::ddsFlags::BUMPDUDV, 0, 32, 0x0000ffff, 0xffff0000, 0, 0 };
dds::DDS_PIXELFORMAT DDSPF_A2R10G10B10 = { 0x20, dds::ddsFlags::RGBA, 0, 32, 0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000 };
dds::DDS_PIXELFORMAT DDSPF_A2B10G10R10 = { 0x20, dds::ddsFlags::RGBA, 0, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 };


dds::ePixelFormat dds::getDdsFormat(DWORD dwRagePixelFormat) {
	ePixelFormat format;
	switch (dwRagePixelFormat) {
	case 827611204:
		format = dds::DXT1;
		break;
	case 844388420:
		format = dds::DXT2;
		break;
	case 861165636:
		format = dds::DXT3;
		break;
	case 877942852:
		format = dds::DXT4;
		break;
	case 894720068:
		format = dds::DXT5;
		break;
	case 21:
		format = dds::A8R8G8B8;
		break;
	case 22:
		format = dds::X8R8G8B8;
		break;
	case 50: // l8
		format = dds::L8;
		break;
	case 20: // r8g8b8
		format = dds::R8G8B8;
		break;
	case 23: // r5g6b5
		format = dds::R5G6B5;
		break;
	case 24: // x1r5g5b5
		format = dds::X1R5G5B5;
		break;
	case 25: // a1r5g5b5
		format = dds::A1R5G5B5;
		break;
	case 26: // a4r4g4b4
		format = dds::A4R4G4B4;
		break;
	case 27: // r3g3b2
		format = dds::R3G3B2;
		break;
	case 28: // a8
		format = dds::A8;
		break;
	case 29: // a8r3g3b2
		format = dds::A8R3G3B2;
		break;
	case 30: // x4r4g4b4
		format = dds::X4R4G4B4;
		break;
	case 31: // a2b10g10r10
		format = dds::A2B10G10R10;
		break;
	case 32: // a8b8g8r8
		format = dds::A8B8G8R8;
		break;
	case 33: // x8b8g8r8
		format = dds::X8B8G8R8;
		break;
	case 34: // g16r16
		format = dds::G16R16;
		break;
	case 35: // a2r10g10b10
		format = dds::A2R10G10B10;
		break;
	case 36: // a16b16g16r16
		format = dds::A16B16G16R16;
		break;
	case 51: // a8l8
		format = dds::A8L8;
		break;
	case 52: // a4l4
		format = dds::A4L4;
		break;
	default:
		return (ePixelFormat)0;
		break;
	}
	return format;
}

DWORD dds::getRagePixelFormat(ePixelFormat eDdsPixelFormat) {
	DWORD retVal;
	switch (eDdsPixelFormat) {
	case dds::DXT1:
		retVal = 827611204;
		break;
	case dds::DXT2:
		retVal = 844388420;
		break;
	case dds::DXT3:
		retVal = 861165636;
		break;
	case dds::DXT4:
		retVal = 877942852;
		break;
	case dds::DXT5:
		retVal = 894720068;
		break;
	case dds::A8R8G8B8:
		retVal = 21;
		break;
	case dds::X8R8G8B8:
		retVal = 22;
		break;
	case dds::L8: // l8
		retVal = 50;
		break;
	case dds::R8G8B8: // r8g8b8
		retVal = 20;
		break;
	case dds::R5G6B5: // r5g6b5
		retVal = 23;
		break;
	case dds::X1R5G5B5: // x1r5g5b5
		retVal = 24;
		break;
	case dds::A1R5G5B5: // a1r5g5b5
		retVal = 25;
		break;
	case dds::A4R4G4B4: // a4r4g4b4
		retVal = 26;
		break;
	case dds::R3G3B2: // r3g3b2
		retVal = 27;
		break;
	case dds::A8: // a8
		retVal = 28;
		break;
	case dds::A8R3G3B2: // a8r3g3b2
		retVal = 29;
		break;
	case dds::X4R4G4B4: // x4r4g4b4
		retVal = 30;
		break;
	case dds::A2B10G10R10: // a2b10g10r10
		retVal = 31;
		break;
	case dds::A8B8G8R8: // a8b8g8r8
		retVal = 32;
		break;
	case dds::X8B8G8R8: // x8b8g8r8
		retVal = 33;
		break;
	case dds::G16R16: // g16r16
		retVal = 34;
		break;
	case dds::A2R10G10B10: // a2r10g10b10
		retVal = 35;
		break;
	case dds::A16B16G16R16: // a16b16g16r16
		retVal = 36;
		break;
	case dds::A8L8: // a8l8
		retVal = 51;
		break;
	case dds::A4L4: // a4l4
		retVal = 52;
		break;
	default:
		return 0;
		break;
	}
	return retVal;
}

rage::ConstString dds::getFormat(DWORD ddsFormat) {
	switch (ddsFormat) {
	case DXT1: return "DXT1";
	case DXT2: return "DXT2";
	case DXT3: return "DXT3";
	case DXT4: return "DXT4";
	case DXT5: return "DXT5";
	case X8R8G8B8: return "X8R8G8B8";
	case A8R8G8B8: return "X8R8G8B8";
	case L8: return "L8";
	case R8G8B8: return "R8G8B8";
	case R5G6B5: return "R5G6B5";
	case X1R5G5B5: return "X1R5G5B5";
	case A1R5G5B5: return "A1R5G5B5";
	case A4R4G4B4: return "A4R4G4B4";
	case R3G3B2: return "R3G3B2";
	case A8: return "A8";
	case A8R3G3B2: return "A8R3G3B2";
	case X4R4G4B4: return "X4R4G4B4";
	case A2B10G10R10: return "A2B10G10R10";
	case A8B8G8R8: return "A8B8G8R8";
	case X8B8G8R8: return "X8B8G8R8";
	case G16R16: return "G16R16";
	case A2R10G10B10: return "A2R10G10B10";
	case A16B16G16R16: return "A16B16G16R16";
	case A8L8: return "A8L8";
	case A4L4: return "A4L4";
	default: return "unk";
	}
}

rage::ConstString dds::getThisDdsFormatAsText() {
	DWORD ddsFormat = this->getThisDdsFormat();
	switch (ddsFormat) {
	case DXT1: return "DXT1";
	case DXT2: return "DXT2";
	case DXT3: return "DXT3";
	case DXT4: return "DXT4";
	case DXT5: return "DXT5";
	case X8R8G8B8: return "X8R8G8B8";
	case A8R8G8B8: return "X8R8G8B8";
	case L8: return "L8";
	case R8G8B8: return "R8G8B8";
	case R5G6B5: return "R5G6B5";
	case X1R5G5B5: return "X1R5G5B5";
	case A1R5G5B5: return "A1R5G5B5";
	case A4R4G4B4: return "A4R4G4B4";
	case R3G3B2: return "R3G3B2";
	case A8: return "A8";
	case A8R3G3B2: return "A8R3G3B2";
	case X4R4G4B4: return "X4R4G4B4";
	case A2B10G10R10: return "A2B10G10R10";
	case A8B8G8R8: return "A8B8G8R8";
	case X8B8G8R8: return "X8B8G8R8";
	case G16R16: return "G16R16";
	case A2R10G10B10: return "A2R10G10B10";
	case A16B16G16R16: return "A16B16G16R16";
	case A8L8: return "A8L8";
	case A4L4: return "A4L4";
	default: return "unk";
	}
}

dds::ePixelFormat dds::getThisDdsFormat() {
	ePixelFormat format;
	if (ddspf == DDSPF_DXT1) format = ePixelFormat::DXT1;
	else if (ddspf == DDSPF_DXT2) format = ePixelFormat::DXT2;
	else if (ddspf == DDSPF_DXT3) format = ePixelFormat::DXT3;
	else if (ddspf == DDSPF_DXT4) format = ePixelFormat::DXT4;
	else if (ddspf == DDSPF_DXT5) format = ePixelFormat::DXT5;
	else if (ddspf == DDSPF_X8R8G8B8) format = ePixelFormat::X8R8G8B8;
	else if (ddspf == DDSPF_A8R8G8B8) format = ePixelFormat::A8R8G8B8;
	else if (ddspf == DDSPF_L8) format = ePixelFormat::L8;
	else if (ddspf == DDSPF_R8G8B8) format = ePixelFormat::R8G8B8;
	else if (ddspf == DDSPF_R5G6B5) format = ePixelFormat::R5G6B5;
	else if (ddspf == DDSPF_X1R5G5B5) format = ePixelFormat::X1R5G5B5;
	else if (ddspf == DDSPF_A1R5G5B5) format = ePixelFormat::A1R5G5B5;
	else if (ddspf == DDSPF_A4R4G4B4) format = ePixelFormat::A4R4G4B4;
	else if (ddspf == DDSPF_R3G3B2) format = ePixelFormat::R3G3B2;
	else if (ddspf == DDSPF_A8) format = ePixelFormat::A8;
	else if (ddspf == DDSPF_A8R3G3B2) format = ePixelFormat::A8R3G3B2;
	else if (ddspf == DDSPF_X4R4G4B4) format = ePixelFormat::X4R4G4B4;
	else if (ddspf == DDSPF_A2B10G10R10) format = ePixelFormat::A2B10G10R10;
	else if (ddspf == DDSPF_A8B8G8R8) format = ePixelFormat::A8B8G8R8;
	else if (ddspf == DDSPF_X8B8G8R8) format = ePixelFormat::X8B8G8R8;
	else if (ddspf == DDSPF_G16R16) format = ePixelFormat::G16R16;
	else if (ddspf == DDSPF_A2R10G10B10) format = ePixelFormat::A2R10G10B10;
	else if (ddspf == DDSPF_A16B16G16R16) format = ePixelFormat::A16B16G16R16;
	else if (ddspf == DDSPF_A8L8) format = ePixelFormat::A8L8;
	else if (ddspf == DDSPF_A4L4) format = ePixelFormat::A4L4;
	else return (ePixelFormat)0;
	return format;
}

DWORD dds::getStride() {
	ePixelFormat format = this->getThisDdsFormat();
	DWORD dwStride;
	switch (format) {
	case ePixelFormat::DXT1:
		dwStride = ((std::max(1ul, (this->dwWidth + 3) / 4) * 8) * std::max(1ul, (this->dwHeight + 3) / 4) / this->dwHeight);
		break;
	case ePixelFormat::DXT2:
	case ePixelFormat::DXT3:
	case ePixelFormat::DXT4:
	case ePixelFormat::DXT5:
		dwStride = ((std::max(1ul, (this->dwWidth + 3) / 4) * 16) * std::max(1ul, (this->dwHeight + 3) / 4) / this->dwHeight);
		break;
	case ePixelFormat::A16B16G16R16: // !! 64-битная идет как fourcc, но получать stride нужно как для обычной
		dwStride = ((this->dwWidth * 64 + 7) / 8);
		break;
	default:
		dwStride = ((this->dwWidth * ddspf.dwRGBBitCount + 7) / 8);
	}

	return dwStride;
}

DWORD dds::getFirstMipSize() { return this->getStride() * this->dwHeight; }

DWORD dds::getPixelDataSize() {
	DWORD base = this->getFirstMipSize();
	//DWORD retVal = 0;
	//for (BYTE i = 0; i < this->dwMipMapCount; i++) { retVal += base; base /= 4; }
	DWORD retVal = base;
	for (BYTE a = 1; a < this->dwMipMapCount; a++)
		retVal += ((base) >> (a * 2));
	return retVal;
}

bool dds::isLoadedSupportedFormat() {
	return this->getThisDdsFormat() > 0;
}

int dds::loadFromMemory(BYTE* pDdsBuf) {
	if (*(DWORD*)pDdsBuf != dds::DDS_MAGIC) return 1;
	memcpy(this, pDdsBuf, dds::DDS_DX9_HEADER_SIZE);
	if (!this->isLoadedSupportedFormat()) {
		this->~dds();
		error("[dds::loadFromMemory] detected unsupported texture");
		return 2;
	}

	if (this->dwMipMapCount == 0) this->dwMipMapCount = 1;

	DWORD dwDataSize = this->getPixelDataSize();
	
	this->pPixelData = new("load dds, pixels") BYTE[dwDataSize];
	memcpy(this->pPixelData, pDdsBuf + dds::DDS_DX9_HEADER_SIZE, dwDataSize);

	trace("[dds::loadFromMemory] loaded %s texture", this->getThisDdsFormatAsText());
	return 0;
}

int dds::loadFromFile(const char* pszFilePath) {
	FILE* fIn = fopen(pszFilePath, "rb");
	if (!fIn) return 1;
	fseek(fIn, 0, SEEK_END);
	DWORD dwFileSize = ftell(fIn);
	fseek(fIn, 0, SEEK_SET);
	if (!dwFileSize) return 2;
	BYTE* pDds = new("load dds, raw file") BYTE[dwFileSize];
	fread(pDds, 1, dwFileSize, fIn);
	auto result = this->loadFromMemory(pDds);
	if (result == 2)
		error("[dds::loadFromFile] texture '%s' is unsupported", pszFilePath);
	dealloc_arr(pDds);
	return result;
}

dds::dds() {
	memset(this, 0, DDS_DX9_HEADER_SIZE);
	this->pPixelData = NULL;
}

dds::dds(const char* pszPath) {
	this->loadFromFile(pszPath);
}

dds::dds(BYTE* pFile) {
	this->loadFromMemory(pFile);
}

dds::~dds() {
	dealloc_arr(this->pPixelData);
}

DWORD dds::getDdsSize() {
	return this->getPixelDataSize() + dds::DDS_DX9_HEADER_SIZE;
}

int dds::saveToMemory(BYTE* pDdsBuf) {
	memcpy(pDdsBuf, this, this->DDS_DX9_HEADER_SIZE);
	memcpy(pDdsBuf + this->DDS_DX9_HEADER_SIZE, this->pPixelData, this->getPixelDataSize());
	return 0;
}

int dds::saveToFile(const char* pszFilePath) {
	DWORD dwSize = this->getDdsSize();
	BYTE* pDdsBuf = new("save dds, raw file") BYTE[dwSize];
	if (this->saveToMemory(pDdsBuf)) return 1;
	FILE* fOut = fopen(pszFilePath, "wb");
	if (!fOut) return 2;
	fwrite(pDdsBuf, sizeof(BYTE), dwSize, fOut);
	fclose(fOut);
	dealloc_arr(pDdsBuf);
	return 0;
}

DWORD dds::getTotalPixelsCount() {
	DWORD count = 0;
	DWORD w = this->dwWidth;
	DWORD h = this->dwHeight;
	for (size_t i = 0; i < this->dwMipMapCount; i++) {
		count += w * h;
		w /= 2;
		h /= 2;
	}
	return count;
}

// from LibertyFourX
//void dds::convertDXT1ToUni() {
	//DWORD dwPosCurrentMipInDds = 0;
	//DWORD dwMipSize = 0;
	//dds::uni* newPixData = libertyFourXYZ::g_memory_manager.allocate<dds::uni>(this->getTotalPixelsCount());
	//DWORD width = this->dwWidth;
	//DWORD height = this->dwHeight;
	//for (size_t i = 0; i < this->dwMipMapCount; i++) {
	//	int xBlocks = width / 4;
	//	int yBlocks = height / 4;
	//	dwMipSize = width * height / 2;
	//	BYTE* data = this->pPixelData + dwPosCurrentMipInDds;
	//	for (int y = 0; y < yBlocks; y++) {
	//		for (int x = 0; x < xBlocks; x++) {
	//			int blockDataStart = ((y * xBlocks) + x) * 8;

	//			unsigned color0 = *(unsigned short*)&(data[blockDataStart]);
	//			unsigned color1 = *(unsigned short*)&(data[blockDataStart + 2]);

	//			unsigned code = *(unsigned*)&(data[blockDataStart + 4]);

	//			unsigned short r0 = 0, g0 = 0, b0 = 0, r1 = 0, g1 = 0, b1 = 0;
	//			r0 = (8 * (color0 & 31));
	//			g0 = (4 * ((color0 >> 5) & 63));
	//			b0 = (8 * ((color0 >> 11) & 31));

	//			r1 = (8 * (color1 & 31));
	//			g1 = (4 * ((color1 >> 5) & 63));
	//			b1 = (8 * ((color1 >> 11) & 31));

	//			for (int k = 0; k < 4; k++) {
	//				int j = k;

	//				for (int i = 0; i < 4; i++) {
	//					int pixDataStart = (width * (y * 4 + j) * 4) + ((x * 4 + i) * 4);
	//					unsigned codeDec = code & 0x3;

	//					switch (codeDec) {
	//					case 0:
	//						pixData[pixDataStart + 0] = (byte)r0;
	//						pixData[pixDataStart + 1] = (byte)g0;
	//						pixData[pixDataStart + 2] = (byte)b0;
	//						pixData[pixDataStart + 3] = 255;
	//						break;
	//					case 1:
	//						pixData[pixDataStart + 0] = (byte)r1;
	//						pixData[pixDataStart + 1] = (byte)g1;
	//						pixData[pixDataStart + 2] = (byte)b1;
	//						pixData[pixDataStart + 3] = 255;
	//						break;
	//					case 2:
	//						pixData[pixDataStart + 3] = 255;
	//						if (color0 > color1) {
	//							pixData[pixDataStart + 0] = (byte)((2 * r0 + r1) / 3);
	//							pixData[pixDataStart + 1] = (byte)((2 * g0 + g1) / 3);
	//							pixData[pixDataStart + 2] = (byte)((2 * b0 + b1) / 3);
	//						}
	//						else {
	//							pixData[pixDataStart + 0] = (byte)((r0 + r1) / 2);
	//							pixData[pixDataStart + 1] = (byte)((g0 + g1) / 2);
	//							pixData[pixDataStart + 2] = (byte)((b0 + b1) / 2);
	//						}
	//						break;
	//					case 3:
	//						if (color0 > color1) {
	//							pixData[pixDataStart + 0] = (byte)((r0 + 2 * r1) / 3);
	//							pixData[pixDataStart + 1] = (byte)((g0 + 2 * g1) / 3);
	//							pixData[pixDataStart + 2] = (byte)((b0 + 2 * b1) / 3);
	//							pixData[pixDataStart + 3] = 255;
	//						}
	//						else {
	//							pixData[pixDataStart + 0] = 0;
	//							pixData[pixDataStart + 1] = 0;
	//							pixData[pixDataStart + 2] = 0;
	//							pixData[pixDataStart + 3] = 0;
	//						}
	//						break;
	//					}

	//					code >>= 2;
	//				}
	//			}

	//		}
	//	}

	//	width /= 2;
	//	height /= 2;
	//}
	//return pixData;


//}