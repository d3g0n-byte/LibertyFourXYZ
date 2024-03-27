
#include "rage_string.h"

#include "rage_datResource.h"

#pragma warning (disable : 4996)

namespace rage {


	void ConstString::destroy() {
		if (this->pszString) {
			libertyFourXYZ::g_memory_manager.release<char>((char*&)this->pszString); // ���������
		}
	}

	ConstString::~ConstString() { this->destroy(); }
	ConstString::ConstString() : pszString(NULL) {};

	ConstString::ConstString(char* pszStr) : pszString(NULL) {
		//this->~ConstString();
		if (pszStr == NULL) { this->pszString = NULL; return; }
		DWORD dwSize = strlen(pszStr) + 1;
		this->pszString = libertyFourXYZ::g_memory_manager.allocate<char>("rage_conststr, new1", dwSize);
		strcpy((char*)this->pszString, pszStr);
	}

	void ConstString::resize(DWORD dwNewSize) {
		auto pszOld = this->pszString;
		DWORD dwOldSize = this->length();
		this->pszString = libertyFourXYZ::g_memory_manager.allocate<char>("rage_conststr, resize", dwNewSize + 1);
		memset((char*)this->pszString, 0x0, dwNewSize + 1);
		memcpy((char*)this->pszString, pszOld, dwNewSize > dwOldSize ? dwOldSize : dwNewSize);
		libertyFourXYZ::g_memory_manager.release<char>((char*&)pszOld);
	}

	ConstString::ConstString(const ConstString& copy) : pszString(NULL) {
		(*this) = copy;
	}


	bool ConstString::operator==(const char* str) { return strcmp(str, this->pszString) == 0; }
	bool ConstString::operator==(rage::ConstString& str) { return strcmp(str, this->pszString) == 0; }

	ConstString::ConstString(const char* pszStr) : pszString(NULL) {
		//this->~ConstString();
		if (pszStr == NULL) { this->pszString = NULL; return; }

		DWORD dwSize = strlen(pszStr) + 1;
		this->pszString = libertyFourXYZ::g_memory_manager.allocate<char>("rage_conststr, new2", dwSize);
		strcpy((char*)this->pszString, (char*)pszStr);
	}

	ConstString ConstString::getFileName() {
		auto index = this->findLastOf('/', '\\');
		return index == npos ? this->pszString : this->pszString + index + 1;
	}

	ConstString ConstString::getFileExt() {
		auto index = this->findLastOf('.');
		return index == npos ? "" : this->pszString + index;
	}

	ConstString ConstString::getClearFileExt() {
		auto index = this->findLastOf('.');
		return index == npos ? "" : this->pszString + index + 1;
	}

	ConstString ConstString::getFilePath() {
		auto index = this->findLastOf('/', '\\');
		if(index == npos ) return "";
		
		ConstString ret = this->pszString;
		ret.resize(index);
		//ret = libertyFourXYZ::g_memory_manager.allocate<const char>("conststr, getfilepath", index + 1);
		//ret[index] = '\0'; // t
		//strncpy((char*)ret.pszString, this->pszString, index);

		return ret;

		//// To-Do: ����������
		//__int64 dwSlash = -1;
		//char slash;
		//for (DWORD i = 0; i < this->length(); i++) if (this->pszString[i] == '/' || this->pszString[i] == '\\') { dwSlash = i; slash = this->pszString[i]; }
		//rage::ConstString pszRet;
		//if (dwSlash != -1) {
		//	(*this)[dwSlash] = '\0';
		//	pszRet = rage::ConstString(this->pszString);
		//	(*this)[dwSlash] = slash;
		//}
		//else pszRet = rage::ConstString(this->pszString);
		//return pszRet;
	}

	ConstString ConstString::getFileNameWithoutExt() {
		auto ret = this->getFileName();
		auto index = ret.findLastOf('.');
		if (index == npos) return ret;
		
		ret.resize(index);
		return ret;



		//rage::ConstString tmp = this->getFileName();

		//// To-Do: ���������
		//int dwDot = -1;
		//for (DWORD i = 0; i < tmp.length(); i++) if (tmp.c_str()[i] == '.') dwDot = i;
		//if (dwDot == -1) return this->c_str();

		//char* chars = (char*)tmp.c_str();
		//chars[dwDot] = '\0';
		//rage::ConstString retVal = chars;
		//chars[dwDot] = '.';

		//return retVal;

	}

	ConstString::operator const char* () const { return this->pszString; }
	const char* ConstString::c_str() { return this->pszString; }
	DWORD ConstString::length() { if (!this->pszString) return 0; else return strlen(this->pszString); }
	char& ConstString::operator[](DWORD dwCharIndex) { return *((char*)this->c_str() + dwCharIndex); }

	ConstString ConstString::format(const char* _format, ...) {
		char tmpBuffer[0xff];
		va_list	arg;
		va_start(arg, _format);
		vsprintf(tmpBuffer, _format, arg);
		va_end(arg);
		return rage::ConstString(tmpBuffer);
	}

	ConstString& ConstString::operator=(const char* psz) {
		this->~ConstString();
		if (psz == NULL) { this->pszString = NULL; return *this; }

		DWORD dwSize = strlen(psz) + 1;
		this->pszString = libertyFourXYZ::g_memory_manager.allocate<char>("rage_conststr, op=1", dwSize);
		strcpy((char*)this->pszString, (char*)psz);

		return (ConstString&)*this;
	}

	ConstString& ConstString::operator=(ConstString& psz) {
		this->~ConstString();

		if (psz.length()) {
			this->pszString = libertyFourXYZ::g_memory_manager.allocate<char>("rage_conststr, op=2", psz.length() + 1);
			strcpy((char*)this->pszString, psz.pszString);
		}

		return /*(ConstString&)*/*this;
	}

	bool ConstString::cleared() {
		return this->pszString == NULL;
	}

	bool ConstString::empty() {
		if (this->cleared())
			return 1;
		else return this->length() == 0;
	}

	size_t ConstString::findLastOf(char ch) {
		if (this->empty()) return npos;
		int pos = this->length() - 1;
		while (this->pszString[pos] != ch) { pos--; if (pos == -1) break; }
		return pos;
	}

	size_t ConstString::findLastOf(char ch1, char ch2) {
		if (this->empty()) return npos;
		int pos = this->length() - 1;
		while (this->pszString[pos] != ch1 && this->pszString[pos] != ch2) { pos--; if (pos == -1) break; }
		return pos;
	}

	size_t ConstString::findLastOf(char ch1, char ch2, char ch3) {
		if (this->empty()) return npos;
		int pos = this->length() - 1;
		while (this->pszString[pos] != ch1 && this->pszString[pos] != ch2 && this->pszString[pos] != ch3) { pos--; if (pos == -1) break; }
		return pos;
	}

	size_t ConstString::findFirstOf(char ch) {
		if (this->empty()) return npos;
		int pos = 0;
		int end = this->length();
		while (this->pszString[pos] != ch) { pos++; if (pos == end) return npos; }
		return pos;
	}

	size_t ConstString::findFirstOf(char ch1, char ch2) {
		if (this->empty()) return npos;
		int pos = 0;
		int end = this->length();
		while (this->pszString[pos] != ch1 && this->pszString[pos] != ch2) { pos++; if (pos == end) return npos; }
		return pos;
	}

	size_t ConstString::findFirstOf(char ch1, char ch2, char ch3) {
		if (this->empty()) return npos;
		int pos = 0;
		int end = this->length();
		while (this->pszString[pos] != ch1 && this->pszString[pos] != ch2 && this->pszString[pos] != ch3) { pos++; if (pos == end) return npos; }
		return pos;
	}

	void ConstString::place(rage::datResource* pRsc) {
		if (this->pszString == NULL)
			return;
		const char* tmpString = pRsc->getFixup<const char>(this->pszString, -1);
		DWORD dwSize = strlen(tmpString) + 1;
		tmpString = pRsc->getFixup<const char>(this->pszString, dwSize); // for check bounds

		this->pszString = libertyFourXYZ::g_memory_manager.allocate<char>("rage_conststr, place", dwSize);
		strcpy((char*)this->pszString, (char*)tmpString);
	}

	void ConstString::addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth) {
		pLayout->addObject<char const>(this->pszString, 5, this->length() + 1);
	}

	void ConstString::replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth) {
		auto pszObj = this->pszString;
		DWORD dwSize = this->length();
		pLayout->setPtr<const char>(this->pszString);
		memcpy((char*)pRsc->getFixup<const char>(this->pszString, dwSize), pszObj, strlen(pszObj) + 1);

	}

	void ConstString::clearRefCount() {}
	void ConstString::setRefCount() {}


}