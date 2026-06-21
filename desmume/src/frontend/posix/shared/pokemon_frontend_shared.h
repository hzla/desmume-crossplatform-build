#ifndef DESMUME_POSIX_POKEMON_FRONTEND_SHARED_H_
#define DESMUME_POSIX_POKEMON_FRONTEND_SHARED_H_

#include <stdint.h>

#include <string>
#include <vector>

#include "../../../pokemon_http_export.h"
#include "../gtk/config.h"

namespace desmume {
namespace pokemon_frontend {

struct ExportProfileOption {
	desmume_pokemon_http_export::ExportGameProfile profile;
	const char *label;
	const char *target;
};

struct PlatinumTimeOption {
	desmume_pokemon_http_export::PlatinumTimeOverride mode;
	const char *label;
	const char *target;
};

struct IntervalOption {
	uint32_t milliseconds;
	const char *label;
	const char *target;
};

struct PartyStatusMenuEntry {
	uint32_t raw;
	const char *label;
};

struct PartyQuickStatusButtonEntry {
	uint32_t raw;
	const char *label;
};

void LoadPokemonBackendConfig(config::Config &config,
							  bool &clipboardAutoCopyEnabled,
							  uint64_t &clipboardAutoCopyIntervalMs);
void SavePokemonBackendConfig(config::Config &config,
							  bool clipboardAutoCopyEnabled,
							  uint64_t clipboardAutoCopyIntervalMs);

const std::vector<ExportProfileOption> &GetExportProfileOptions();
const std::vector<PlatinumTimeOption> &GetPlatinumTimeOptions();
const std::vector<IntervalOption> &GetHealthCheckIntervalOptions();
const std::vector<IntervalOption> &GetIdleRefreshIntervalOptions();
const std::vector<IntervalOption> &GetClipboardIntervalOptions();
const std::vector<PartyStatusMenuEntry> &GetPartyStatusMenuEntries();
const std::vector<PartyQuickStatusButtonEntry> &GetPartyQuickStatusButtons();

uint32_t GetDefaultClipboardIntervalMs();
bool NormalizeInterval(uint32_t requestedInterval,
					   const std::vector<IntervalOption> &options,
					   uint32_t fallbackInterval,
					   uint32_t &resolvedInterval);

bool ProfileFromTarget(const std::string &target,
					   desmume_pokemon_http_export::ExportGameProfile &profileOut);
bool PlatinumTimeFromTarget(const std::string &target,
							desmume_pokemon_http_export::PlatinumTimeOverride &modeOut);
bool IntervalFromTarget(const std::string &target,
						const std::vector<IntervalOption> &options,
						uint32_t &intervalOut);
const char *TargetForProfile(desmume_pokemon_http_export::ExportGameProfile profile);
const char *TargetForPlatinumTime(desmume_pokemon_http_export::PlatinumTimeOverride mode);
const char *TargetForInterval(uint32_t intervalMs, const std::vector<IntervalOption> &options);

std::string BuildPokemonStatusLabel(const desmume_pokemon_http_export::Status &status);
bool PrepareClipboardUtf8Text(const std::string &text, std::string &normalizedText, std::string &errorOut);

std::string HiddenPowerTypeForEditorIVs(const uint8_t ivs[6]);
std::string GetPartyDisplayName(const desmume_pokemon_http_export::PartyPokemonSummary &summary);
desmume_pokemon_http_export::PartyPokemonEditRequest BuildPartyStatusEditorRequest(
	const desmume_pokemon_http_export::PartyPokemonSummary &summary,
	int currentHp,
	uint32_t statusRaw,
	bool preserveEncodedStatusRaw);

}  // namespace pokemon_frontend
}  // namespace desmume

#endif  // DESMUME_POSIX_POKEMON_FRONTEND_SHARED_H_
