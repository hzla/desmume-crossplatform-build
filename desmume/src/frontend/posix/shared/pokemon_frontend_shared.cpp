#include "pokemon_frontend_shared.h"

#include <glib.h>

#include <algorithm>
#include <string.h>

#include "../../../path.h"

namespace desmume {
namespace pokemon_frontend {
namespace {

const char kPokemonHttpExportSection[] = "PokemonHttpExport";
const char kBattleLoggingKey[] = "BattleLoggingEnabled";

const ExportProfileOption kExportProfiles[] = {
	{desmume_pokemon_http_export::EXPORT_GAME_POKEMON_PLATINUM, "Pokemon Platinum", "platinum"},
	{desmume_pokemon_http_export::EXPORT_GAME_POKEMON_PLATINUM_KAIZO, "Platinum Kaizo", "platinum_kaizo"},
	{desmume_pokemon_http_export::EXPORT_GAME_POKEMON_RENEGADE_PLATINUM, "Renegade Platinum", "renegade_platinum"},
	{desmume_pokemon_http_export::EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER, "Pokemon HeartGold/SoulSilver", "heartgold_soulsilver"},
	{desmume_pokemon_http_export::EXPORT_GAME_POKEMON_HEART_GOLD_ENGINE, "Heart Gold Engine", "heart_gold_engine"},
	{desmume_pokemon_http_export::EXPORT_GAME_POKEMON_STERLING_SILVER, "Sterling Silver", "sterling_silver"},
	{desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK_WHITE, "Pokemon Black/White", "black_white"},
	{desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK2_WHITE2, "Pokemon Black 2/White 2", "black2_white2"},
	{desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX, "Blaze Black 2 Redux/Volt White 2 Redux", "bb2_vw2_redux"}
};

const PlatinumTimeOption kPlatinumTimes[] = {
	{desmume_pokemon_http_export::PLATINUM_TIME_AUTO, "Auto", "auto"},
	{desmume_pokemon_http_export::PLATINUM_TIME_MORNING, "Morning", "morning"},
	{desmume_pokemon_http_export::PLATINUM_TIME_DAY, "Day", "day"},
	{desmume_pokemon_http_export::PLATINUM_TIME_EVENING, "Evening", "evening"},
	{desmume_pokemon_http_export::PLATINUM_TIME_NIGHT, "Night", "night"},
	{desmume_pokemon_http_export::PLATINUM_TIME_LATE_NIGHT, "Late Night", "late_night"}
};

const IntervalOption kHealthCheckIntervals[] = {
	{15000u, "15 Seconds", "15000"},
	{30000u, "30 Seconds", "30000"},
	{60000u, "60 Seconds", "60000"},
	{120000u, "120 Seconds", "120000"},
	{300000u, "300 Seconds", "300000"}
};

const IntervalOption kIdleRefreshIntervals[] = {
	{60000u, "60 Seconds", "60000"},
	{180000u, "180 Seconds", "180000"},
	{300000u, "300 Seconds", "300000"},
	{600000u, "600 Seconds", "600000"}
};

const IntervalOption kClipboardIntervals[] = {
	{5000u, "5 Seconds", "5000"},
	{10000u, "10 Seconds", "10000"},
	{30000u, "30 Seconds", "30000"},
	{60000u, "60 Seconds", "60000"}
};

const PartyStatusMenuEntry kPartyStatusEntries[] = {
	{0u, "Healthy"},
	{1u, "Sleep (1)"},
	{2u, "Sleep (2)"},
	{3u, "Sleep (3)"},
	{4u, "Sleep (4)"},
	{5u, "Sleep (5)"},
	{6u, "Sleep (6)"},
	{7u, "Sleep (7)"},
	{8u, "Poison"},
	{16u, "Burn"},
	{32u, "Freeze"},
	{64u, "Paralysis"},
	{128u, "Badly Poisoned"}
};

const PartyQuickStatusButtonEntry kPartyQuickStatuses[] = {
	{0u, "OK"},
	{1u, "Slp"},
	{8u, "Psn"},
	{16u, "Brn"},
	{32u, "Frz"},
	{64u, "Par"},
	{128u, "Tox"}
};

template <typename T, size_t N>
std::vector<T> MakeVector(const T (&items)[N]) {
	return std::vector<T>(items, items + N);
}

void CopyBattleLogPathSetting(const std::string &configuredPath) {
	strncpy(path.pathToBattleLogs, configuredPath.c_str(), sizeof(path.pathToBattleLogs) - 1);
	path.pathToBattleLogs[sizeof(path.pathToBattleLogs) - 1] = '\0';
}

}  // namespace

void LoadPokemonBackendConfig(config::Config &config,
							  bool &clipboardAutoCopyEnabled,
							  uint64_t &clipboardAutoCopyIntervalMs) {
	CopyBattleLogPathSetting(config.path_battle_logs.get());

	desmume_pokemon_http_export::SetSelectedProfile(
		static_cast<desmume_pokemon_http_export::ExportGameProfile>(config.pokemon_http_profile.get()));
	desmume_pokemon_http_export::SetPlatinumTimeOverride(
		static_cast<desmume_pokemon_http_export::PlatinumTimeOverride>(config.pokemon_platinum_time_override.get()));
	desmume_pokemon_http_export::SetHealthCheckEnabled(config.pokemon_health_check_enabled.get());
	desmume_pokemon_http_export::SetHealthCheckIntervalMs(
		static_cast<uint32_t>(config.pokemon_health_check_interval_ms.get()));
	desmume_pokemon_http_export::SetIdleRefreshEnabled(config.pokemon_idle_refresh_enabled.get());
	desmume_pokemon_http_export::SetIdleRefreshIntervalMs(
		static_cast<uint32_t>(config.pokemon_idle_refresh_interval_ms.get()));
	desmume_pokemon_http_export::SetEnabled(config.pokemon_http_enabled.get());

	const bool battleLoggingConfigured = config.hasKey(kPokemonHttpExportSection, kBattleLoggingKey);
	const bool battleLoggingEnabled = battleLoggingConfigured
		? config.pokemon_battle_logging_enabled.get()
		: config.pokemon_http_enabled.get();
	desmume_pokemon_http_export::SetBattleLoggingEnabled(battleLoggingEnabled);

	clipboardAutoCopyEnabled = config.pokemon_clipboard_auto_copy_enabled.get();
	clipboardAutoCopyIntervalMs = static_cast<uint64_t>(config.pokemon_clipboard_auto_copy_interval_ms.get());
	if (clipboardAutoCopyIntervalMs == 0u) {
		clipboardAutoCopyIntervalMs = GetDefaultClipboardIntervalMs();
	}
	desmume_pokemon_http_export::SetClipboardSyncEnabled(clipboardAutoCopyEnabled);
}

void SavePokemonBackendConfig(config::Config &config,
							  bool clipboardAutoCopyEnabled,
							  uint64_t clipboardAutoCopyIntervalMs) {
	config.pokemon_http_enabled = desmume_pokemon_http_export::IsEnabled();
	config.pokemon_battle_logging_enabled = desmume_pokemon_http_export::IsBattleLoggingEnabled();
	config.pokemon_http_profile = static_cast<int>(desmume_pokemon_http_export::GetSelectedProfile());
	config.pokemon_platinum_time_override = static_cast<int>(desmume_pokemon_http_export::GetPlatinumTimeOverride());
	config.pokemon_health_check_enabled = desmume_pokemon_http_export::IsHealthCheckEnabled();
	config.pokemon_health_check_interval_ms = static_cast<int>(desmume_pokemon_http_export::GetHealthCheckIntervalMs());
	config.pokemon_idle_refresh_enabled = desmume_pokemon_http_export::IsIdleRefreshEnabled();
	config.pokemon_idle_refresh_interval_ms = static_cast<int>(desmume_pokemon_http_export::GetIdleRefreshIntervalMs());
	config.pokemon_clipboard_auto_copy_enabled = clipboardAutoCopyEnabled;
	config.pokemon_clipboard_auto_copy_interval_ms = static_cast<int>(clipboardAutoCopyIntervalMs);
	config.path_battle_logs = path.pathToBattleLogs;
}

const std::vector<ExportProfileOption> &GetExportProfileOptions() {
	static const std::vector<ExportProfileOption> options = MakeVector(kExportProfiles);
	return options;
}

const std::vector<PlatinumTimeOption> &GetPlatinumTimeOptions() {
	static const std::vector<PlatinumTimeOption> options = MakeVector(kPlatinumTimes);
	return options;
}

const std::vector<IntervalOption> &GetHealthCheckIntervalOptions() {
	static const std::vector<IntervalOption> options = MakeVector(kHealthCheckIntervals);
	return options;
}

const std::vector<IntervalOption> &GetIdleRefreshIntervalOptions() {
	static const std::vector<IntervalOption> options = MakeVector(kIdleRefreshIntervals);
	return options;
}

const std::vector<IntervalOption> &GetClipboardIntervalOptions() {
	static const std::vector<IntervalOption> options = MakeVector(kClipboardIntervals);
	return options;
}

const std::vector<PartyStatusMenuEntry> &GetPartyStatusMenuEntries() {
	static const std::vector<PartyStatusMenuEntry> options = MakeVector(kPartyStatusEntries);
	return options;
}

const std::vector<PartyQuickStatusButtonEntry> &GetPartyQuickStatusButtons() {
	static const std::vector<PartyQuickStatusButtonEntry> options = MakeVector(kPartyQuickStatuses);
	return options;
}

uint32_t GetDefaultClipboardIntervalMs() {
	return 10000u;
}

bool NormalizeInterval(uint32_t requestedInterval,
					   const std::vector<IntervalOption> &options,
					   uint32_t fallbackInterval,
					   uint32_t &resolvedInterval) {
	for (size_t i = 0; i < options.size(); ++i) {
		if (options[i].milliseconds == requestedInterval) {
			resolvedInterval = requestedInterval;
			return true;
		}
	}

	resolvedInterval = fallbackInterval;
	return false;
}

bool ProfileFromTarget(const std::string &target,
					   desmume_pokemon_http_export::ExportGameProfile &profileOut) {
	const std::vector<ExportProfileOption> &options = GetExportProfileOptions();
	for (size_t i = 0; i < options.size(); ++i) {
		if (target == options[i].target) {
			profileOut = options[i].profile;
			return true;
		}
	}

	return false;
}

bool PlatinumTimeFromTarget(const std::string &target,
							desmume_pokemon_http_export::PlatinumTimeOverride &modeOut) {
	const std::vector<PlatinumTimeOption> &options = GetPlatinumTimeOptions();
	for (size_t i = 0; i < options.size(); ++i) {
		if (target == options[i].target) {
			modeOut = options[i].mode;
			return true;
		}
	}

	return false;
}

bool IntervalFromTarget(const std::string &target,
						const std::vector<IntervalOption> &options,
						uint32_t &intervalOut) {
	for (size_t i = 0; i < options.size(); ++i) {
		if (target == options[i].target) {
			intervalOut = options[i].milliseconds;
			return true;
		}
	}

	return false;
}

const char *TargetForProfile(desmume_pokemon_http_export::ExportGameProfile profile) {
	const std::vector<ExportProfileOption> &options = GetExportProfileOptions();
	for (size_t i = 0; i < options.size(); ++i) {
		if (profile == options[i].profile) {
			return options[i].target;
		}
	}

	return kExportProfiles[0].target;
}

const char *TargetForPlatinumTime(desmume_pokemon_http_export::PlatinumTimeOverride mode) {
	const std::vector<PlatinumTimeOption> &options = GetPlatinumTimeOptions();
	for (size_t i = 0; i < options.size(); ++i) {
		if (mode == options[i].mode) {
			return options[i].target;
		}
	}

	return kPlatinumTimes[0].target;
}

const char *TargetForInterval(uint32_t intervalMs, const std::vector<IntervalOption> &options) {
	for (size_t i = 0; i < options.size(); ++i) {
		if (intervalMs == options[i].milliseconds) {
			return options[i].target;
		}
	}

	return options.empty() ? "" : options[0].target;
}

std::string BuildPokemonStatusLabel(const desmume_pokemon_http_export::Status &status) {
	std::string statusLabel = "Pokemon HTTP API: Disabled";

	if (status.enabled) {
		if (!status.running && !status.lastError.empty()) {
			statusLabel = "Pokemon HTTP API: Error - " + status.lastError;
		} else if (!status.running) {
			statusLabel = "Pokemon HTTP API: Starting";
		} else if (!status.romLoaded) {
			statusLabel = "Pokemon HTTP API: Running (no ROM loaded)";
		} else if (!status.supportedGame) {
			statusLabel = "Pokemon HTTP API: Running (select export game)";
		} else if (!status.lastError.empty()) {
			statusLabel = "Pokemon HTTP API: Error - " + status.lastError;
		} else {
			statusLabel = "Pokemon HTTP API: " + status.url;
		}
	}

	return statusLabel;
}

bool PrepareClipboardUtf8Text(const std::string &text, std::string &normalizedText, std::string &errorOut) {
	if (text.empty()) {
		errorOut = "Pokemon export text is empty";
		return false;
	}

	if (!g_utf8_validate(text.c_str(), static_cast<gssize>(text.size()), NULL)) {
		errorOut = "Pokemon export text is not valid UTF-8";
		return false;
	}

	normalizedText = text;
	errorOut.clear();
	return true;
}

std::string HiddenPowerTypeForEditorIVs(const uint8_t ivs[6]) {
	static const char *kHiddenPowerTypes[16] = {
		"Fighting", "Flying", "Poison", "Ground",
		"Rock", "Bug", "Ghost", "Steel",
		"Fire", "Water", "Grass", "Electric",
		"Psychic", "Ice", "Dragon", "Dark"
	};

	const int hpTypeX =
		(ivs[0] & 1) +
		((ivs[1] & 1) << 1) +
		((ivs[2] & 1) << 2) +
		((ivs[5] & 1) << 3) +
		((ivs[3] & 1) << 4) +
		((ivs[4] & 1) << 5);
	const int typeIndex = (hpTypeX * 15) / 63;
	return kHiddenPowerTypes[typeIndex];
}

std::string GetPartyDisplayName(const desmume_pokemon_http_export::PartyPokemonSummary &summary) {
	if (summary.hasNickname && !summary.nickname.empty()) {
		return summary.nickname;
	}

	return summary.species;
}

desmume_pokemon_http_export::PartyPokemonEditRequest BuildPartyStatusEditorRequest(
	const desmume_pokemon_http_export::PartyPokemonSummary &summary,
	int currentHp,
	uint32_t statusRaw,
	bool preserveEncodedStatusRaw) {
	desmume_pokemon_http_export::PartyPokemonEditRequest request = {};
	request.slot = summary.slot;
	request.currentHp = currentHp;
	request.statusRaw = statusRaw;
	if (preserveEncodedStatusRaw && statusRaw == summary.statusRaw) {
		request.encodedStatusRaw = summary.encodedStatusRaw;
		request.preserveEncodedStatusRaw = true;
	}
	request.experience = summary.experience;
	request.speciesId = summary.speciesId;
	request.heldItemId = summary.heldItemId;
	request.abilityId = summary.abilityId;
	request.friendship = summary.friendship;
	for (int i = 0; i < 6; ++i) {
		request.ivs[i] = summary.stats.ivs[i];
		request.evs[i] = summary.stats.evs[i];
	}
	for (int i = 0; i < 4; ++i) {
		request.moveIds[i] = summary.moves[i].id;
	}
	return request;
}

}  // namespace pokemon_frontend
}  // namespace desmume
