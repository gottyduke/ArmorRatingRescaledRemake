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


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	DKUtil::Logger::Init(Plugin::NAME, REL::Module::get().version().string());

	SKSE::Init(a_skse);

	INFO("{} v{} loaded", Plugin::NAME, Plugin::Version);
	Config::Load();

	SKSE::AllocTrampoline(static_cast<size_t>(1) << 7);
	Hooks::Install();

	if (*Config::OverrideArmorCap) {
		*Config::OverrideArmorCap = *Config::OverrideArmorCap > 100 ? 100 : *Config::OverrideArmorCap;

		if (SKSE::GetMessagingInterface()->RegisterListener(MessageHandler)) {
			INFO("Messaging interface registration successful");
		} else {
			ERROR("Messaging interface registration failed!\n");
			return false;
		}
	}

	return true;
}