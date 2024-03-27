#pragma once
#include <Windows.h>

namespace libertyFourXYZ { class rsc85_layout; };
namespace rage { class datResource; }

namespace rage {

	// the class is based on a c-style string
	class ConstString {
		const char* pszString;

	public:
		const static size_t npos = -1;

		~ConstString();
		ConstString();
		ConstString(char* pszStr);
		ConstString(const char* pszStr);
		
		DWORD length();
		bool cleared();
		bool empty();
		ConstString getFileName();
		ConstString getFileExt();
		ConstString getClearFileExt();
		ConstString getFileNameWithoutExt();
		ConstString getFilePath();
		size_t findLastOf(char ch);
		size_t findLastOf(char ch1, char ch2);
		size_t findLastOf(char ch1, char ch2, char ch3);
		size_t findFirstOf(char ch);
		size_t findFirstOf(char ch1, char ch2);
		size_t findFirstOf(char ch1, char ch2, char ch3);
		void resize(DWORD dwNewSize); // ToDo: delete
		void destroy();

		char& operator[](DWORD dwCharIndex);
		operator const char* () const;
		const char* c_str();
		ConstString& operator=(const char* psz);
		ConstString& operator=(ConstString& psz);
		ConstString(const ConstString &copy);
		ConstString static format(const char* _format, ...);
		bool operator==(const char* str);
		bool operator==(rage::ConstString& str);

		void place(rage::datResource* rsc);
		void addToLayout(libertyFourXYZ::rsc85_layout* pLayout, DWORD dwDepth);
		void replacePtrs(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, DWORD dwDepth);
		void clearRefCount();
		void setRefCount();
	};
}