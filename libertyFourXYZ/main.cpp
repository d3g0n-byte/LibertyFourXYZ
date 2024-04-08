// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <chrono>

#include "iv_pc_wtd.h"
#include "iv_pc_wdr.h"
#include "rage_string.h"
#include "settings.h"
#include "globals.h"
#include "dds.h"
#include "rage_grcTexture.h"
#include "memory_manager.h"
#include <filesystem>

auto g_timeOnStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

void _cdecl exitFunc() {
	auto timeOnEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	__int64 timeUsed = timeOnEnd - g_timeOnStart;
	printf("Elapsed %d.%03d second\n", (DWORD)timeUsed / 1000, (DWORD)timeUsed % 1000);
}

void printInfo() {
	printf("use: %s args...\n", rage::ConstString(libertyFourXYZ::g_szExePath).getFileName().c_str());
	printf("args:\n" );
	printf("r_wtd_v8 - rebuild IV PC rage::pgDictionary<rage::grcTexture>\n" );
	printf("m_wtd_v8 - merge txds\n" );
	printf("c_wtd_v8 - from fftdc\n" );
	printf("r_wdr_v110 - rebuild IV PC gtaDrawable\n" );
	//printf("r_wdd_v110 - rebuild IV PC rage::pgDictionary<gtaDrawable>\n" );
}

void main(int argc, rage::ConstString argv[]) {
	g_timeOnStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	atexit(exitFunc);
	libertyFourXYZ::initializeGlobals();
	libertyFourXYZ::readSettings();

	if (argc < 2) { printInfo(); return; }

	rage::ConstString c = argv[1];
	rage::ConstString *pFiles = argv + 2;

	if (c == "-r_wtd_v8") {
		iv_pc_wtd::processWTD(*pFiles);
	}
	else if (c == "-r_wdr_v110") {
		iv_pc_wdr::processWdr(*pFiles);
	}
	else if (c == "-r_wdd_v110") {
		iv_pc_wdr::processWdd(*pFiles);
	}
	else if (c == "-m_wtd_v8") {
		iv_pc_wtd::mergeWtdsToWmtd(argc - 2, pFiles);
	}
	else if (c == "-c_wtd_v8" ) {
		//iv_pc_wtd::wtdFromDdsFiles(argv[2], argc - 3, argv + 3);
		rage::ConstString* ppszDdsPath = NULL;

		rage::ConstString pszOutFile = argv[2];

		DWORD dwFileCount = 0;
		if (std::string(argv[3]) == "-d") {
			dwFileCount = argc - 4;
			if (dwFileCount < 1) { error("[main] no dds textures found"); return; }
			ppszDdsPath = new ("main, dds path 1") rage::ConstString[dwFileCount];
			for (int i = 4; i < argc; i++) ppszDdsPath[i - 4] = argv[i];
		}
		else if (std::string(argv[3]) == "-f") {
			dwFileCount = 0;
			for (const auto& entry : std::filesystem::directory_iterator(argv[4].c_str()))
				if (entry.path().extension() == ".dds")
					dwFileCount++;

			if (dwFileCount < 1) { error("[main] no dds textures found"); return; }
			ppszDdsPath = new ("main, dds path 2") rage::ConstString[dwFileCount];
			DWORD dwIndex = 0;
			for (const auto& entry : std::filesystem::directory_iterator(argv[4].c_str()))
				if (entry.path().extension() == ".dds")
					ppszDdsPath[dwIndex++] = entry.path().string().c_str();
		}
		iv_pc_wtd::wtdFromDdsFiles(pszOutFile, dwFileCount, ppszDdsPath);
		
		dealloc_arr(ppszDdsPath);
	}
	else printInfo();

}

// Run program: Alt + F4 or Ctrl + Alt + Delete -> Sign out
// Debug program: Win > Power > Shut down

// Tips for Getting Started: 
//   1. Pick up a pistol and preferably a large caliber
//   2. Point the gun at you head
//   3. Pull the trigger
//   4. If for some reason you're alive, sit down and work on this fucking project.
//   5. -
//   6. -
