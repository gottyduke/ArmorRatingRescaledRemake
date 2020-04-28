#include "Hooks.h"
#include "Settings.h"

#include "RE/Skyrim.h"
#include "SKSE/CodeGenerator.h"
#include "SKSE/API.h"


// this is the original algorithm from underthesky (Armor Rating Rescaled LE)
// vanillaResist = VisibleArmorValue/100.0 * fArmorScalingFactor (default 0.12)
// hiddenResist = count_worn(helmet armor boots gauntlets shield) * fArmorBaseFactor (default 0.03)
// result: part of damage that would be averted, before armor cap (1.0 = all)
float __fastcall RescaleArmor(const float a_hidden, const float a_vanilla)
{
	if (a_vanilla < 0) {
		if (*Settings::disableHiddenArmorRating) {
			return a_vanilla;
		}
		return a_vanilla + a_hidden;
	}
	float f = a_vanilla * *Settings::armorScalingFactor;
	f /= 1 + f;
	if (!*Settings::disableHiddenArmorRating) {
		f += a_hidden * (1 - f);
	}
	return f;
}


namespace
{
	constexpr auto NOP = 0x90;
	constexpr auto JMP = 0xE9;
	constexpr auto branchSize = 12;


	bool Branch(const std::uintptr_t a_from, const std::uintptr_t a_to)
	{
		SKSE::CodeGenerator codeGen(branchSize);
		codeGen.mov(codeGen.rax, a_to);
		codeGen.call(codeGen.rax);
		codeGen.ready();

		return SKSE::SafeWriteBuf(a_from, codeGen.getCode(), branchSize);
	}


	template <std::uintptr_t FUNC_ID, std::size_t START, std::uintptr_t END>
	void InjectAt(const char* a_pre_patch, const char* a_post_patch)
	{
		if (!a_pre_patch || !a_post_patch) {
			_ERROR("Failed to retrieve code");
			assert(false);
			std::abort();
		}

		const auto codeSize = END - START;
		const auto injectAddr = REL::ID(FUNC_ID).GetAddress() + START;
		const auto continueAddr = injectAddr + codeSize;
		const auto hookAddr = reinterpret_cast<std::uintptr_t>(&RescaleArmor);

		const auto preSize = std::strlen(a_pre_patch);
		const auto postSize = std::strlen(a_post_patch);

		const auto trampoline = SKSE::GetTrampoline()->Allocate(branchSize + preSize + postSize + 5);
		auto trampolinePtr = reinterpret_cast<std::uintptr_t>(trampoline);

		// NOP ftw
		for (auto i = 0; i < codeSize; ++i) {
			SKSE::SafeWrite8(injectAddr + i, NOP);
		}

		// rel jmp to trampoline
		SKSE::SafeWrite8(injectAddr, JMP);
		SKSE::SafeWrite32(injectAddr + 1, trampolinePtr - injectAddr - 5);

		// write pre patch
		SKSE::SafeWriteBuf(trampolinePtr, a_pre_patch, preSize);
		trampolinePtr += preSize;

		// branch to function
		if (!Branch(trampolinePtr, hookAddr)) {
			_ERROR("Failed to branch to memory location");
			assert(false);
			std::abort();
		}
		trampolinePtr += branchSize;

		// append post patch
		SKSE::SafeWriteBuf(trampolinePtr, a_post_patch, postSize);
		trampolinePtr += postSize;

		// rel jmp to orginal
		SKSE::SafeWrite8(trampolinePtr, JMP);
		SKSE::SafeWrite32(trampolinePtr + 1, continueAddr - trampolinePtr - 5);

		_MESSAGE("Injected trampoline hook at <%p>", injectAddr);
	}
}


namespace Hooks
{
	void InstallHooks()
	{
		// scale armor resistance
		if (std::isfinite(*Settings::armorScalingFactor) && *Settings::armorScalingFactor > 0) {
			*Settings::armorScalingFactor *= 5.0f;
		}
		
		// trampoline hook
		/* pre patch:
			xorps xmm1, xmm1		; zero out xmm1
			movss xmm1, [rbp+0x77]	; pass vanillaResist as 2nd argument
		 * post patch:
			movss [rbp+0x77], xmm0	; store RescaleArmor result to vanillaResist
		*/
		InjectAt<42842, 0x107, 0x115>("\x0F\x57\xC9\xF3\x0F\x10\x4D\x77", "\xF3\x0F\x11\x45\x77");

		/* pre patch:
			xorps xmm1, xmm1		; zero out xmm1
			movss xmm1, xmm8		; pass vanillaResist as 2nd argument
		 * post	patch:
			movss xmm8, xmm0		; store RescaleArmor result to vanillaResist
		*/
		InjectAt<37605, 0x82, 0x99>("\x0F\x57\xC9\xF3\x41\x0F\x10\xC8", "\xF3\x44\x0F\x10\xC0");

		_MESSAGE("Successfully installed hooks");
	}
}