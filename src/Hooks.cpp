#include "Hooks.h"
#include "Settings.h"

#include "DKUtil/Hook.h"

#include "RE/Skyrim.h"


namespace
{
	constexpr std::uint64_t FUNC_1_ID = 37605;
	constexpr std::uintptr_t FUNC_1_OFFSET_START = 0x82;
	constexpr std::uintptr_t FUNC_1_OFFSET_END = 0x99;
	
	// pre patch:
	// xorps xmm1, xmm1	; zero out xmm1
	// movss xmm1, xmm8	; pass vanillaResist as 2nd argument
	// post	patch:
	// movss xmm8, xmm0	; store RescaleArmor result to vanillaResist
	constexpr BranchInstruction FUNC_1_INSTRUCTION = {
		reinterpret_cast<std::uintptr_t>(&Hooks::Hook_RescaleArmor),
		"\x0F\x57\xC9\xF3\x0F\x10\x4D\x77",
		8,
		"\xF3\x0F\x11\x45\x77",
		5
	};

	constexpr std::uint64_t FUNC_2_ID = 42842;
	constexpr std::uintptr_t FUNC_2_OFFSET_START = 0x107;
	constexpr std::uintptr_t FUNC_2_OFFSET_END = 0x115;

	// pre patch:
	// xorps xmm1, xmm1		; zero out xmm1
	// movss xmm1, [rbp+0x77]	; pass vanillaResist as 2nd argument
	// post patch:
	// movss [rbp+0x77], xmm0	; store RescaleArmor result to vanillaResist
	constexpr BranchInstruction FUNC_2_INSTRUCTION = {
		reinterpret_cast<std::uintptr_t>(&Hooks::Hook_RescaleArmor),
		"\x0F\x57\xC9\xF3\x41\x0F\x10\xC8",
		8,
		"\xF3\x44\x0F\x10\xC0",
		5
	};
}


namespace Hooks
{
	// this is the original algorithm from underthesky (Armor Rating Rescaled LE)
	// vanillaResist = VisibleArmorValue/100.0 * fArmorScalingFactor (default 0.12)
	// hiddenResist = count_worn(helmet armor boots gauntlets shield) * fArmorBaseFactor (default 0.03)
	// result: part of damage that would be averted, before armor cap (1.0 = all)
	float __fastcall Hook_RescaleArmor(const float a_hidden, const float a_vanilla)
	{
		if (a_vanilla < 0) {
			return *Settings::disableHiddenArmorRating ? a_vanilla : a_vanilla + a_hidden;
		}
		float f = a_vanilla * *Settings::armorScalingFactor;
		f /= 1 + f;
		if (!*Settings::disableHiddenArmorRating) {
			f += a_hidden * (1 - f);
		}
		return f;
	}

	
	bool InstallHooks()
	{
		auto success = true;
		
		// scale armor resistance
		if (std::isfinite(*Settings::armorScalingFactor) && *Settings::armorScalingFactor > 0) {
			*Settings::armorScalingFactor *= 5.0f;
		}
		
		success &= DKUtil::Hook::BranchToFunction<FUNC_1_ID, FUNC_1_OFFSET_START, FUNC_1_OFFSET_END>(FUNC_1_INSTRUCTION);

		success &= DKUtil::Hook::BranchToFunction<FUNC_2_ID, FUNC_2_OFFSET_START, FUNC_2_OFFSET_END>(FUNC_2_INSTRUCTION);

		return success;
	}
}