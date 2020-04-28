#include "version.h"
#include "Hooks.h"
#include "Settings.h"

#include "SKSE/API.h"
#include "RE/Skyrim.h"


namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			char command[36];
			std::sprintf(command, "setgs fMaxArmorRating %d.00", static_cast<int>(*Settings::overrideArmorCap));

			auto script = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>()->Create();
			script->SetCommand(command);
			script->CompileAndRun(nullptr);

			_MESSAGE("Modified fMaxArmorRating");
		}
	}
}


extern "C"
{
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\ArmorRatingRescaledRemake.log");
		SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::UseLogStamp(true);
		SKSE::Logger::TrackTrampolineStats(true);
		
		_MESSAGE("ArmorRatingRescaledRemake v%s", ARRR_VERSION_VERSTRING);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "BunnyHopperOfSkyrim";
		a_info->version = ARRR_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			_FATALERROR("Loaded in editor, marking as incompatible!\n");
			return false;
		}

		const auto ver = a_skse->RuntimeVersion();
		if (ver <= SKSE::RUNTIME_1_5_39) {
			_FATALERROR("Unsupported runtime version %s!", ver.GetString().c_str());
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
	{
		_MESSAGE("ArmorRatingRescaledRemake loaded");

		if (!Init(a_skse)) {
			return false;
		}
		
		if (Settings::LoadSettings()) {
			_MESSAGE("Successfully loaded json file");
		} else {
			_FATALERROR("Failed to load settings");

			return false;
		}
		
		if (*Settings::overrideArmorCap) {
			*Settings::overrideArmorCap = *Settings::overrideArmorCap > 100 ? 100 : *Settings::overrideArmorCap;
			
			if (SKSE::GetMessagingInterface()->RegisterListener("SKSE", MessageHandler)) {
				_MESSAGE("Messaging interface registration successful");
			} else {
				_FATALERROR("Messaging interface registration failed!\n");
				return false;
			}
		}
		
		if (!SKSE::AllocTrampoline(1 << 6)) {
			_FATALERROR("Failed to allocate trampoline");
			return false;
		}

		Hooks::InstallHooks();

		return true;
	}
}