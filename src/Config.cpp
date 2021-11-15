#include "Config.h"


namespace Config
{
	Double ScalingFactor{ "ArmorScalingFactor" };
	Boolean DisableHidden{ "DisableHiddenArmorRating" };
	Integer OverrideArmorCap{ "OverrideArmorCap" };


	void Load()
	{
		auto Main = COMPILE_PROXY("ArmorRatingRescaledRemake.toml"sv);

		Main.Bind(ScalingFactor, 1.0);
		Main.Bind(DisableHidden, false);
		Main.Bind(OverrideArmorCap, 0);

		Main.Load();

		INFO("Config Loaded"sv);
	}
}