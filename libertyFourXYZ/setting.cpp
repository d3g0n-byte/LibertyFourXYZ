#include <map>
#include <string>
#include <fstream>
#include <iostream>

#include "rage_string.h"
#include "globals.h"

namespace libertyFourXYZ {
	bool g_bForceUseRsc5 = 1;
	bool g_bMergeRscPages = 1;
	BYTE g_nbMaxPageSizeForMerge = 5;
	bool g_bUsePageMap = 1;


	void readSettings() {
		std::string sLine;
		// ToDo: переделать под ini reader
		rage::ConstString pszSettingsPath = rage::ConstString::format("%s\\settings.ini", rage::ConstString((const char*)libertyFourXYZ::g_szExePath).getFilePath());
		std::ifstream fIn(pszSettingsPath);
		std::map<std::string, std::string> settingsParam;
		if (fIn.is_open()) {
			while (std::getline(fIn, sLine)) {
				if (sLine.empty()) continue;
				if(int s = sLine.find_first_of(";") != -1)
					sLine.resize(s - 1);
				if (sLine.empty()) continue;
				while (true) { size_t start_pos = sLine.find(" "); if (start_pos != std::string::npos) sLine.replace(start_pos, 1, ""); else break; }
				while (true) { size_t start_pos = sLine.find("\t"); if (start_pos != std::string::npos) sLine.replace(start_pos, 1, ""); else break; }

				std::string sKey = sLine;
				sKey.resize(sLine.find_first_of("="));
				std::string sVal = sLine.substr(sLine.find_last_of("=") + 1);

				settingsParam.insert(std::make_pair(sKey, sVal));
			}
			fIn.close();
		}

		if (settingsParam.count("bForceUseRsc5"))
			g_bForceUseRsc5 = std::stoi(settingsParam["bForceUseRsc5"]);
		//else g_bForceUseRsc5 = 0;

		if (settingsParam.count("bMergeRscPages"))
			g_bMergeRscPages = std::stoi(settingsParam["bMergeRscPages"]);
		//else g_bMergeRscPages = 1;

		if (settingsParam.count("nbMaxPageSizeForMerge"))
			g_nbMaxPageSizeForMerge = std::stoi(settingsParam["nbMaxPageSizeForMerge"]);
		//else g_nbMaxPageSizeForMerge = 5;

		if (settingsParam.count("bUsePageMap"))
			g_bUsePageMap = std::stoi(settingsParam["bUsePageMap"]);
		//else g_bUsePageMap = 1;


	}
}