#include "Config.h"
#include "Hooks.h"


namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			auto gameSetting = RE::GameSettingCollection::GetSingleton();
			auto maxArmorRating = gameSetting->GetSetting("fMaxArmorRating");
			maxArmorRating->data.f = static_cast<float>(*Config::OverrideArmorCap);

			INFO("Modified fMaxArmorRating"sv);
		}
	}
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface * a_skse, SKSE::PluginInfo * a_info)
{
	DKUtil::Logger::Init(Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		ERROR("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		ERROR("Unsupported runtime version {}", ver.string());
		return false;
	}

	return true;
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface * a_skse)
{
	INFO("{} v{} loaded", Version::PROJECT, Version::NAME);

	SKSE::Init(a_skse);

	Config::Load();

	SKSE::AllocTrampoline(39);
	Hooks::Install();

	if (*Config::OverrideArmorCap) {
		*Config::OverrideArmorCap = *Config::OverrideArmorCap > 100 ? 100 : *Config::OverrideArmorCap;

		if (SKSE::GetMessagingInterface()->RegisterListener("SKSE", MessageHandler)) {
			INFO("Messaging interface registration successful"sv);
		} else {
			ERROR("Messaging interface registration failed!\n"sv);
			return false;
		}
	}

	return true;
}
