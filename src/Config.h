#pragma once

#include "DKUtil/Config.hpp"


namespace Config
{
	using namespace DKUtil::Alias;

	extern Double ScalingFactor;
	extern Boolean DisableHidden;
	extern Integer OverrideArmorCap;


	void Load();
}