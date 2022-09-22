#include "Hooks.h"
#include "Config.h"


namespace
{
	using namespace DKUtil::Alias;

	// 1-6-323 7FF63457A390
	constexpr std::uint64_t AE_Func1 = 38558;
	constexpr std::uintptr_t AE_Func1_OffsetL = 0x80;
	constexpr std::uintptr_t AE_Func1_OffsetH = 0x94;

	// pre patch:
	// xorps xmm1, xmm1
	// movss xmm1, xmm7
	// post patch:
	// movss xmm7, xmm0
	constexpr Patch AE_Func1_Prolog = {
		"\x0F\x57\xC9\xF3\x0F\x10\xCF",
		7
	};

	constexpr Patch AE_Func1_Epilog = {
		"\xF3\x0F\x10\xF8",
		4
	};

	// 1-5-97 7FF710854FA2
	constexpr std::uint64_t SE_Func1 = 37605;
	constexpr std::uintptr_t SE_Func1_OffsetL = 0x82;
	constexpr std::uintptr_t SE_Func1_OffsetH = 0x99;

	// pre patch:
	// xorps xmm1, xmm1
	// movss xmm1, xmm8
	// post	patch:
	// movss xmm8, xmm0
	constexpr Patch SE_Func1_Prolog = {
		"\x0F\x57\xC9\xF3\x41\x0F\x10\xC8",
		8
	};
	
	constexpr Patch SE_Func1_Epilog = {
		"\xF3\x44\x0F\x10\xC0",
		5
	};

	// 1-6-323
	constexpr std::uint64_t AE_Func2 = 44014;
	constexpr std::uintptr_t AE_Func2_OffsetL = 0xFF;
	constexpr std::uintptr_t AE_Func2_OffsetH = 0x10D;

	// 1-5-97 7FF710973617
	constexpr std::uint64_t SE_Func2 = 42842;
	constexpr std::uintptr_t SE_Func2_OffsetL = 0x107;
	constexpr std::uintptr_t SE_Func2_OffsetH = 0x115;

	// pre patch:
	// xorps xmm1, xmm1
	// movss xmm1, [rbp+0x77]
	// post patch:
	// movss [rbp+0x77], xmm0
	constexpr Patch Func2_Prolog = {
		"\x0F\x57\xC9\xF3\x0F\x10\x4D\x77",
		8
	};

	constexpr Patch Func2_Epilog = {
		"\xF3\x0F\x11\x45\x77",
		5
	};
}


namespace Hooks
{
	// this is the original algorithm from underthesky (Armor Rating Rescaled LE)
	// vanillaResist = VisibleArmorValue/100.0 * fArmorScalingFactor (default 0.12)
	// hiddenResist = count_worn(helmet armor boots gauntlets shield) * fArmorBaseFactor (default 0.03)
	// result: part of damage that would be averted, before armor cap (1.0 = all)
	float __cdecl Hook_RescaleArmor(const float a_hidden, const float a_vanilla)
	{
		if (a_vanilla < 0) {
			return *Config::DisableHidden ? a_vanilla : a_vanilla + a_hidden;
		}

		float armorRating = a_vanilla * static_cast<float>(*Config::ScalingFactor);
		armorRating /= 1.0f + armorRating;

		if (!*Config::DisableHidden) {
			armorRating += a_hidden * (1.0f - armorRating);
		}

		return armorRating;
	}


	void Install()
	{
		// scale armor resistance
		if (std::isfinite(*Config::ScalingFactor) && *Config::ScalingFactor > 0) {
			*Config::ScalingFactor *= 5.0f;
		}

		auto handle1 = DKUtil::Hook::AddCaveHook(
			DKUtil::Hook::IDToAbs(AE_Func1, SE_Func1),
			DKUtil::Hook::RuntimeOffset(AE_Func1_OffsetL, AE_Func1_OffsetH, SE_Func1_OffsetL, SE_Func1_OffsetH),
			FUNC_INFO(Hook_RescaleArmor),
			DKUtil::Hook::RuntimePatch(&AE_Func1_Prolog, &SE_Func1_Prolog),
			DKUtil::Hook::RuntimePatch(&AE_Func1_Epilog, &SE_Func1_Epilog));

		auto handle2 = DKUtil::Hook::AddCaveHook(
			DKUtil::Hook::IDToAbs(AE_Func2, SE_Func2),
			DKUtil::Hook::RuntimeOffset(AE_Func2_OffsetL, AE_Func2_OffsetH, SE_Func2_OffsetL, SE_Func2_OffsetH),
			FUNC_INFO(Hook_RescaleArmor),
			&Func2_Prolog, &Func2_Epilog);

		handle1->Enable();
		handle2->Enable();

		INFO("Hooks installed"sv);
	}
}
