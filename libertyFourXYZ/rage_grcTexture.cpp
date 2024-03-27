#include "rage_grcTexture.h"

#include "rsc85_layout.h"
#include "dds.h"
#include "utils.h"

namespace rage {

	//rage::grcTextureReferenceBase& grcTextureReferenceBase::operator=(rage::grcTextureReferenceBase& txd){
	//	pgBase::operator=(txd);
	//	this->m_nbResourceType = 2;
	//	this->_f9 = txd._f9;
	//	this->m_wUsageCount = 1;
	//	this->_fc = txd._fc;
	//	return *this;
	//}

	void grcTextureReferenceBase::place(datResource *pRsc) {
		pgBase::place(pRsc);
	}

	void grcTextureReferenceBase::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		pgBase::addToLayout(pLayout, dwDepth);
	}

	void grcTextureReferenceBase::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, datResource* pRsc, DWORD dwDepth) {
		pgBase::replacePtrs(pLayout,pRsc, dwDepth);
	}

	//rage::grcTextureReference& grcTextureReference::operator=(rage::grcTextureReference& txd) {
	//	grcTextureReferenceBase::operator=(txd);
	//	this->_f10 = txd._f10;
	//	this->m_pszName = txd.m_pszName;
	//	this->m_pTexture = NULL;
	//	return *this;
	//}

	void grcTextureReferenceBase::clearRefCount() {
		pgBase::clearRefCount();
		this->m_wUsageCount = 0;
	}

	void grcTextureReferenceBase::setRefCount() {
		pgBase::setRefCount();
		this->m_wUsageCount++;
	}

	void grcTextureReference::place(datResource* pRsc) {
		grcTextureReferenceBase::place(pRsc);
		this->m_pszName.place(pRsc);
		this->m_pTexture = NULL;
	}

	void grcTextureReference::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		grcTextureReferenceBase::addToLayout(pLayout, dwDepth);
		this->m_pszName.addToLayout(pLayout, dwDepth);
	}

	void grcTextureReference::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, datResource* pRsc, DWORD dwDepth) {
		grcTextureReferenceBase::replacePtrs(pLayout, pRsc, dwDepth);
		this->m_pszName.replacePtrs(pLayout, pRsc, dwDepth);
	}

	void grcTextureReference::clearRefCount() {
		grcTextureReferenceBase::clearRefCount();
	}

	void grcTextureReference::setRefCount() {
		grcTextureReferenceBase::setRefCount();
	}

	grcTexture::grcTexture() : m_nbResourceType(0), m_nbDepth(0), m_wUsageCount(1), _fc(0), _f10(0) { }

	inline void grcTexture::place(rage::datResource* rsc) { pgBase::place(rsc); }

	//rage::grcTexture& rage::grcTexture::operator=(rage::grcTexture& txd) {
	//	pgBase::operator=(txd);

	//	this->m_nbResourceType = txd.m_nbResourceType;
	//	this->m_nbDepth = txd.m_nbDepth;
	//	this->m_wUsageCount = txd.m_wUsageCount;
	//	this->_fc = txd._fc;
	//	this->_f10 = txd._f10;
	//	return *this;
	//}

	void grcTexture::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) { pgBase::addToLayout(pLayout, dwDepth); }
	void grcTexture::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) { pgBase::replacePtrs(pLayout, pRsc, dwDepth); }

	void grcTexture::clearRefCount() {
		pgBase::clearRefCount();
		this->m_wUsageCount = 0;
	}

	void grcTexture::setRefCount() {
		pgBase::setRefCount();
		this->m_wUsageCount++;
	}

	void grcTexturePC::recreateName() {
		if (!this->m_pszName) return;
		this->m_pszName = ConstString::format("memory:$%08x,%u,%u:%s.dds",this, this->getPixelDataSize() + 156,0, this->getClearName());
	}

	DWORD grcTexturePC::getFirstMipSize() {
		return this->m_wMipStride * this->m_wHeight;
	}

	DWORD grcTexturePC::getPixelDataSize() {
		DWORD base = this->getFirstMipSize();
		DWORD retVal = base;
		for (BYTE a = 1; a < this->m_nbLevels; a++)
			retVal += ((base) >> (a * 2));
		//for (BYTE i = 0; i < this->m_nbLevels; i++) { retVal += base; base /= 4; }
		return retVal;
	}

	void grcTexturePC::place(rage::datResource* rsc) {
		grcTexture::place(rsc);

		this->m_pPrev = this->m_pNext = 0; // ?

		this->m_pszName.place(rsc);
		this->recreateName();

		DWORD pds = this->getPixelDataSize();
		BYTE* realDataPtr = rsc->getFixup<BYTE>(this->m_pPixelData, pds);
		this->m_pPixelData = libertyFourXYZ::g_memory_manager.allocate<BYTE>("txdPc, place, data", pds);
		memcpy(this->m_pPixelData, realDataPtr, pds);
	}

	void grcTexturePC::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		grcTexture::addToLayout(pLayout, dwDepth);
		this->m_pszName.addToLayout(pLayout, dwDepth + 1);
		pLayout->addObject<BYTE>(this->m_pPixelData, 6, this->getPixelDataSize());
	}

	void grcTexturePC::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		grcTexture::replacePtrs(pLayout, pRsc, dwDepth);
		this->m_pszName.replacePtrs(pLayout, pRsc, dwDepth + 1);
		DWORD dwSize = this->getPixelDataSize();
		auto pTxd = this->m_pPixelData;
		pLayout->setPtr(this->m_pPixelData);
		memcpy(pRsc->getFixup<BYTE>(this->m_pPixelData, dwSize), pTxd, dwSize);
	}

	void grcTexturePC::clearRefCount() {
		grcTexture::clearRefCount();
	}

	void grcTexturePC::setRefCount() {
		grcTexture::setRefCount();
	}

	// ToDo: remove memset
	grcTexturePC::grcTexturePC() {
		this->m_pszName = NULL;
		this->m_piTexture = NULL; //txd.m_piTexture;
		this->m_wWidth = 0;
		this->m_wHeight = 0;
		this->m_dwPixelFormat = 0;
		this->m_wMipStride = 0;
		this->m_nbTextureType = 0;
		this->m_nbLevels = 0;
		this->m_fColorExpR = 1.f;
		this->m_fColorExpG = 1.f;
		this->m_fColorExpB = 1.f;
		this->m_fColorOfsR = 0.f;
		this->m_fColorOfsG = 0.f;
		this->m_fColorOfsB = 0.f;
		this->m_pPrev = NULL; //txd.m_pPrev;
		this->m_pNext = NULL; //txd.m_pNext;
		this->m_pPixelData = NULL;
		*(DWORD*)this->__4c = 0;

	}

	grcTexturePC::~grcTexturePC() {
		if(this->m_pPixelData)
			libertyFourXYZ::g_memory_manager.release<BYTE>(this->m_pPixelData);
	}

	//rage::grcTexturePC& rage::grcTexturePC::operator=(grcTexturePC& txd) {
	//	*(grcTexture*)this = *(grcTexture*)&txd;

	//	this->m_pszName = txd.m_pszName; // check
	//	this->m_piTexture = NULL; //txd.m_piTexture;
	//	this->m_wWidth = txd.m_wWidth;
	//	this->m_wHeight = txd.m_wHeight;
	//	this->m_dwPixelFormat = txd.m_dwPixelFormat;
	//	this->m_wMipStride = txd.m_wMipStride;
	//	this->m_nbTextureType = txd.m_nbTextureType;
	//	this->m_nbLevels = txd.m_nbLevels;
	//	this->m_fColorExpR = txd.m_fColorExpR;
	//	this->m_fColorExpG = txd.m_fColorExpG;
	//	this->m_fColorExpB = txd.m_fColorExpB;
	//	this->m_fColorOfsR = txd.m_fColorOfsR;
	//	this->m_fColorOfsG = txd.m_fColorOfsG;
	//	this->m_fColorOfsB = txd.m_fColorOfsB;
	//	this->m_pPrev = NULL; //txd.m_pPrev;
	//	this->m_pNext = NULL; //txd.m_pNext;

	//	DWORD dwPSize = this->getPixelDataSize();
	//	if (txd.m_pPixelData) {
	//		this->m_pPixelData = libertyFourXYZ::g_memory_manager.allocate<BYTE>("txdPc, op=, data", dwPSize);
	//		memcpy(this->m_pPixelData, txd.m_pPixelData, dwPSize);
	//	}
	//	else {
	//		this->m_pPixelData = NULL;
	//	}
	//	for (BYTE i = 0; i < 4; this->__4c[i++] = 0);

	//	return *this;
	//}

	int grcTexturePC::ToDds(dds* pDds) {
		

		return 0;
	}

	int grcTexturePC::fromDds(dds* pDds, const char* pszName) {
		this->m_pPageMap = NULL;
		this->m_nbResourceType = 0;
		this->m_nbDepth = 0;
		this->m_wUsageCount = 1;
		this->_fc = 0;
		this->_f10 = 0;
		this->m_wWidth = pDds->dwWidth;
		this->m_wHeight = pDds->dwHeight;
		this->m_dwPixelFormat = pDds->getRagePixelFormat(pDds->getThisDdsFormat());
		this->m_wMipStride = pDds->getStride();
		this->m_nbTextureType = 0;
		this->m_nbLevels = pDds->dwMipMapCount? pDds->dwMipMapCount : 1;
		this->m_fColorExpR = 1.0f;
		this->m_fColorExpG = 1.0f;
		this->m_fColorExpB = 1.0f;
		this->m_fColorOfsR = 0.0f;
		this->m_fColorOfsG = 0.0f;
		this->m_fColorOfsB = 0.0f;
		this->m_pPrev = NULL;
		this->m_pNext = NULL;
		DWORD dwSize = pDds->getPixelDataSize();
		this->m_pPixelData = libertyFourXYZ::g_memory_manager.allocate<BYTE>("txdPc, fromDds, data", dwSize);
		memcpy(this->m_pPixelData, pDds->pPixelData, dwSize);
		memset(this->__4c, 0x0, 4);

		if (pszName) { this->m_pszName = pszName; }
		else this->m_pszName = NULL;

		return 0;
	}

	int grcTexturePC::fromDdsMemory(BYTE* pData, const char* pszName) {
		dds tmpDds;
		auto result = tmpDds.loadFromMemory(pData);
		if (!result)
			return this->fromDds(&tmpDds, pszName);
		else return result;

	}

	int grcTexturePC::fromDdsPath(const char* pszPath, const char* pszName) {
		dds tmpDds;
		auto result = tmpDds.loadFromFile(pszPath);
		if (!result)
			return this->fromDds(&tmpDds, pszName ? pszName : ConstString(pszPath).getFileName().c_str());
		else return result;
	}

	DWORD grcTexturePC::getHash() { return atStringHash(this->getClearName(), 0); }

	ConstString grcTexturePC::getClearName() {
		DWORD dwSlash = 0;
		if (!this->m_pszName) return "";
		for (DWORD i = 0; i < this->m_pszName.length(); i++) if (this->m_pszName[i] == '/' || this->m_pszName[i] == '\\' || this->m_pszName[i] == ':') dwSlash = i + 1;
		return rage::ConstString(this->m_pszName + dwSlash).getFileNameWithoutExt();
	}

}

