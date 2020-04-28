#include "Settings.h"


bool Settings::LoadSettings(const bool a_dumpParse)
{
	auto [log, success] = Json2Settings::load_settings(FILE_NAME, a_dumpParse);
	if (!log.empty()) {
		_ERROR("%s", log.c_str());
	}
		
	return success;
}


decltype(Settings::armorScalingFactor) Settings::armorScalingFactor("ArmorScalingFactor", 1.0f);
decltype(Settings::disableHiddenArmorRating) Settings::disableHiddenArmorRating("DisableHiddenArmorRating", false);
decltype(Settings::overrideArmorCap) Settings::overrideArmorCap("OverrideArmorCap", 0);