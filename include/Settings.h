#pragma once

#include "Json2Settings.h"


namespace J2S = Json2Settings;


class Settings
{
public:
	Settings() = delete;

	static bool LoadSettings(bool a_dumpParse = false);

	static J2S::fSetting armorScalingFactor;
	static J2S::bSetting disableHiddenArmorRating;
	static J2S::iSetting overrideArmorCap;
	
private:
	static inline constexpr char FILE_NAME[] = "Data\\SKSE\\Plugins\\ArmorRatingRescaledRemake.json";
};
