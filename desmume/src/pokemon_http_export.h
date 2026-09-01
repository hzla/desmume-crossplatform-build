#ifndef DESMUME_POKEMON_HTTP_EXPORT_H
#define DESMUME_POKEMON_HTTP_EXPORT_H

#include <stddef.h>
#include <stdint.h>

#include "path.h"
#include "rtc.h"
#include "retro_miscellaneous.h"

#include <string>
#include <vector>

namespace desmume_pokemon_http_export {

enum ExportGameProfile {
	EXPORT_GAME_NONE = 0,
	EXPORT_GAME_POKEMON_PLATINUM = 1,
	EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER = 2,
	EXPORT_GAME_POKEMON_HEART_GOLD_ENGINE = 3,
	EXPORT_GAME_POKEMON_BLACK_WHITE = 4,
	EXPORT_GAME_POKEMON_BLACK2_WHITE2 = 5,
	EXPORT_GAME_POKEMON_PLATINUM_KAIZO = 6,
	EXPORT_GAME_POKEMON_STERLING_SILVER = 7,
	EXPORT_GAME_POKEMON_RENEGADE_PLATINUM = 8,
	EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX = 9
};

enum PokemonDataCodec {
	POKEMON_DATA_CODEC_NONE = 0,
	POKEMON_DATA_CODEC_GEN4 = 4,
	POKEMON_DATA_CODEC_GEN5 = 5
};

enum BattleDetectPollPreset {
	BATTLE_DETECT_POLL_FAST = 0,
	BATTLE_DETECT_POLL_NORMAL = 1,
	BATTLE_DETECT_POLL_SLOW = 2
};

enum PlatinumTimeOverride {
	PLATINUM_TIME_AUTO = 0,
	PLATINUM_TIME_MORNING = 1,
	PLATINUM_TIME_DAY = 2,
	PLATINUM_TIME_EVENING = 3,
	PLATINUM_TIME_NIGHT = 4,
	PLATINUM_TIME_LATE_NIGHT = 5
};

struct GameTextTables {
	const char *const *speciesNames;
	size_t speciesNamesCount;
	const char *const *itemNames;
	size_t itemNamesCount;
	const char *(*getItemNameOverride)(uint16_t itemId);
	const char *const *abilityNames;
	size_t abilityNamesCount;
	const char *(*getAbilityNameOverride)(uint16_t abilityId);
	const char *const *moveNames;
	size_t moveNamesCount;
	const char *(*getMoveNameOverride)(uint16_t moveId);
	const char *const *natureNames;
	size_t natureNamesCount;
	const char *(*getLocationName)(uint16_t locationId);
	const char *(*getFormSuffix)(uint16_t speciesId, uint8_t formIndex);
};

struct PokemonMechanics {
	int (*getLevelForExperience)(uint32_t experience, int speciesId);
	uint32_t (*getExperienceForLevel)(int level, int speciesId);
	uint8_t (*getMoveBasePp)(uint16_t moveId);
	bool (*lookupMoveIdByName)(const std::string &name, uint16_t &moveId, std::string &canonicalName);
};

struct PokemonMemoryLayout {
	PokemonDataCodec codec;
	uint32_t pidPointerAddr;
	uint32_t trainerIDsPointerAddr;
	const char *language;
	uint32_t partyCountOffset;
	uint32_t partyDataOffset;
	uint32_t currentBoxIndexOffset;
	uint32_t boxDataOffset;
	size_t partyMonSize;
	size_t boxMonSize;
	size_t coreBytes;
	size_t battleStatsBytes;
	size_t partySlotsPerBox;
	size_t exportBoxCount;
	size_t boxStrideBytes;
};

struct BattleMemoryLayout {
	bool supported;
	uint32_t battleSystemAnchorPointerAddr;
	uint32_t battleSystemBattleTypeOffset;
	uint32_t battleSystemBattleCtxOffset;
	uint32_t battleSystemMaxBattlersOffset;
	uint32_t battleSystemPartiesOffset;
	uint32_t battleSystemEnemyTrainerIdOffset;
	uint32_t battleContextCommandOffset;
	uint32_t battleContextCommandNextOffset;
	uint32_t battleContextSelectedPartySlotOffset;
	uint32_t battleContextBattleMonsOffset;
	uint32_t battleContextBattleMonSize;
	uint32_t battleTypeMask;
	uint32_t preferredBattleClusterStart;
	uint32_t preferredBattleClusterEnd;
	uint32_t preferredBattleCtxDelta;
	uint32_t nearbyLastKnownRadius;
	uint32_t fullBattleScanStart;
	uint32_t fullBattleScanEnd;
	uint32_t activeBattlePollMs;
	uint32_t fullScanRetryMs;
	uint32_t battleControlCalcTurnOrder;
};

struct BattleDetectionStatus {
	bool battleActive;
	bool trainerBattleActive;
	bool trainerBattleCanStartSession;
	bool trainerBattleSessionActive;
	uint32_t battleSystemAddr;
	uint32_t battleContextAddr;
	uint32_t battleType;
	uint16_t enemyTrainerId;
	int maxBattlers;
	uint64_t lastPollUnixMs;
	uint64_t lastStateChangeUnixMs;
	uint64_t lastSessionStartUnixMs;
	uint64_t lastBattleEndUnixMs;
	std::string detectionSource;
	int detectionSourceDepth;
	int detectionConfidence;
	std::string lastLifecycleEvent;
};

struct BattleLogPartyEntry {
	uint8_t slot;
	uint32_t pid;
	uint16_t species;
	uint16_t heldItem;
	uint8_t natureId;
	uint8_t abilitySlot;
	uint16_t ability;
	uint16_t moves[4];
};

struct BattleLogEvent {
	std::string type;
	uint16_t turn;
	uint8_t pSlot;
	uint16_t aiSpecies;
	uint64_t unixMs;
};

struct BattleLogSession {
	bool active;
	uint16_t playerTrainerId;
	uint16_t playerSecretId;
	uint16_t enemyTrainerIdA;
	uint16_t enemyTrainerIdB;
	uint8_t pPartyHighestLevel;
	bool badgeInfoResolved;
	uint8_t badgeCount;
	uint16_t badgeFlags;
	uint64_t startedAtUnixMs;
	uint64_t endedAtUnixMs;
	std::vector<BattleLogPartyEntry> pParty;
	std::vector<BattleLogEvent> events;
};

struct BattleLogStatus {
	bool active;
	uint32_t sessionCount;
	uint32_t activeSessionEventCount;
	uint16_t activeTrainerId;
	uint16_t currentTurn;
	uint64_t lastSessionStartUnixMs;
	uint64_t lastSessionEndUnixMs;
	std::string lastEventType;
	uint16_t lastEventTurn;
	uint8_t lastEventPlayerSlot;
	uint16_t lastEventAiSpecies;
	std::string outputDir;
	std::string attemptMapPath;
	std::string lastPersistedMasterPath;
	bool lastPersistSucceeded;
	std::string lastPersistError;
};

struct PlayerBadgeStatus {
	bool available;
	std::string source;
	uint32_t pidAddr;
	uint32_t profileAddr;
	uint8_t badgeCount;
	uint16_t badgeFlags;
	std::string lastError;
};

struct HgssBattleMonSample {
	bool valid;
	uint8_t battler;
	uint8_t partySlot;
	uint16_t speciesId;
	uint32_t curHp;
	uint32_t maxHp;
};

struct HgssDiscoveryStatus {
	bool active;
	bool trainerIdsResolved;
	std::string trainerIdsSource;
	uint32_t pidAddr;
	uint16_t trainerId;
	uint16_t secretId;
	std::string trainerIdsError;
	uint32_t battleAnchorAddr;
	uint32_t battleAnchorValue;
	bool battleCandidateValid;
	std::string battleProbeSource;
	uint32_t battleSystemCandidateAddr;
	uint32_t battleContextCandidateAddr;
	uint32_t battleType;
	uint16_t enemyTrainerId;
	int maxBattlers;
	uint32_t command;
	uint32_t commandNext;
	int battleConfidence;
	std::string battleProbeError;
	std::vector<HgssBattleMonSample> sampledMons;
};

struct Status {
	bool enabled;
	bool battleLoggingEnabled;
	bool running;
	bool ready;
	bool romLoaded;
	bool supportedGame;
	unsigned short port;
	uint64_t lastSuccessUnixMs;
	bool healthCheckEnabled;
	uint32_t healthCheckIntervalMs;
	bool idleRefreshEnabled;
	uint32_t idleRefreshIntervalMs;
	std::string url;
	std::string gameCode;
	std::string selectedProfile;
	std::string platinumTimeOverride;
	bool platinumTimeOverrideActive;
	std::string effectiveGameTime;
	std::string statusText;
	std::string lastError;
	BattleDetectPollPreset battleDetectPollPreset;
	uint32_t battleDetectPollMs;
	PlayerBadgeStatus playerBadges;
	BattleDetectionStatus battleDetection;
	BattleLogStatus battleLog;
	HgssDiscoveryStatus hgssDiscovery;
};

struct PartyMove {
	uint16_t id;
	std::string name;
};

struct PokemonEditorStats {
	uint8_t ivs[6];
	uint8_t evs[6];
};

struct PartyPokemonSummary {
	int slot;
	uint32_t monOffset;
	uint32_t pid;
	uint16_t speciesId;
	std::string species;
	uint16_t heldItemId;
	std::string heldItem;
	uint16_t abilityId;
	uint8_t abilitySlot;
	std::string ability;
	uint8_t friendship;
	PokemonEditorStats stats;
	std::string gender;
	std::string nickname;
	bool hasNickname;
	int level;
	uint32_t experience;
	uint32_t nextLevelExperience;
	int currentHp;
	int maxHp;
	uint32_t statusRaw;
	uint32_t encodedStatusRaw;
	std::string statusLabel;
	PartyMove moves[4];
	std::string debugSnapshot;
};

struct BoxPokemonSummary {
	int box;
	int slot;
	uint32_t monOffset;
	uint32_t pid;
	uint16_t speciesId;
	std::string species;
	uint16_t heldItemId;
	std::string heldItem;
	uint16_t abilityId;
	uint8_t abilitySlot;
	std::string ability;
	uint8_t friendship;
	PokemonEditorStats stats;
	std::string gender;
	std::string nickname;
	bool hasNickname;
	int level;
	uint32_t experience;
	uint32_t nextLevelExperience;
	PartyMove moves[4];
	std::string debugSnapshot;
};

struct PartyEditorSnapshot {
	bool success;
	std::string error;
	std::vector<PartyPokemonSummary> party;
};

struct BoxEditorSnapshot {
	bool success;
	std::string error;
	int currentBox;
	int boxCount;
	std::vector<BoxPokemonSummary> pokemon;
};

struct PartyPokemonEditRequest {
	int slot;
	int currentHp;
	uint32_t experience;
	uint32_t statusRaw;
	uint32_t encodedStatusRaw;
	bool preserveEncodedStatusRaw;
	uint16_t speciesId;
	uint16_t heldItemId;
	uint16_t abilityId;
	uint8_t friendship;
	uint8_t ivs[6];
	uint8_t evs[6];
	uint16_t moveIds[4];
};

struct PartyPokemonHpStatusEditRequest {
	int slot;
	int currentHp;
	bool preserveCurrentHp;
	uint32_t statusRaw;
	uint32_t encodedStatusRaw;
	bool preserveEncodedStatusRaw;
};

struct BoxPokemonEditRequest {
	int box;
	int slot;
	uint32_t experience;
	uint16_t speciesId;
	uint16_t heldItemId;
	uint16_t abilityId;
	uint8_t friendship;
	uint8_t ivs[6];
	uint8_t evs[6];
	uint16_t moveIds[4];
};

void SetEnabled(bool enabled);
bool IsEnabled();
void SetClipboardSyncEnabled(bool enabled);
bool IsClipboardSyncEnabled();
void SetBattleLoggingEnabled(bool enabled);
bool IsBattleLoggingEnabled();
void SetSelectedProfile(ExportGameProfile profile);
ExportGameProfile GetSelectedProfile();
bool IsSelectedProfilePlatinumVariant();
void SetPlatinumTimeOverride(PlatinumTimeOverride mode);
PlatinumTimeOverride GetPlatinumTimeOverride();
const char *GetProfileDisplayName(ExportGameProfile profile);
const char *GetPlatinumTimeOverrideDisplayName(PlatinumTimeOverride mode);
void SetHealthCheckEnabled(bool enabled);
bool IsHealthCheckEnabled();
void SetHealthCheckIntervalMs(uint32_t intervalMs);
uint32_t GetHealthCheckIntervalMs();
void SetIdleRefreshEnabled(bool enabled);
bool IsIdleRefreshEnabled();
void SetIdleRefreshIntervalMs(uint32_t intervalMs);
uint32_t GetIdleRefreshIntervalMs();
bool ForceRefresh();
bool GetShowdownExportText(std::string &text, std::string &error);
void SetBattleDetectPollPreset(BattleDetectPollPreset preset);
BattleDetectPollPreset GetBattleDetectPollPreset();
const char *GetBattleDetectPollPresetDisplayName(BattleDetectPollPreset preset);
Status GetStatus();
bool GetPartyEditorSnapshot(PartyEditorSnapshot &snapshot);
bool GetPartyEditorSnapshotForLoadedGame(PartyEditorSnapshot &snapshot);
bool UpdatePartyPokemon(const PartyPokemonEditRequest &request, std::string &error);
bool UpdatePartyPokemonHpStatus(const PartyPokemonHpStatusEditRequest &request, std::string &error);
bool IsPartyPokemonReadyForWrite(int slot, std::string &error);
bool IsEntirePartyReadyForWrite(std::string &error);
bool EdgePartyPokemon(int slot, uint32_t amount, std::string &error);
bool GetBoxEditorSnapshot(BoxEditorSnapshot &snapshot);
bool UpdateBoxPokemon(const BoxPokemonEditRequest &request, std::string &error);
bool EdgeBoxPokemon(int box, int slot, uint32_t amount, std::string &error);
bool WriteEditorDebugSnapshot(const std::string &fileName, const std::string &text, std::string &pathOut, std::string &errorOut);
bool WriteFullPartyRawBytesDebugSnapshot(std::string &pathOut, std::string &errorOut);
bool LookupMoveIdByName(const std::string &name, uint16_t &moveId, std::string &canonicalName);
bool LookupSpeciesIdByName(const std::string &name, uint16_t &speciesId, std::string &canonicalName);
bool LookupItemIdByName(const std::string &name, uint16_t &itemId, std::string &canonicalName);
bool LookupAbilityIdByName(const std::string &name, uint16_t &abilityId, std::string &canonicalName);
std::vector<std::string> GetSpeciesNameOptions();
std::vector<std::string> GetItemNameOptions();
std::vector<std::string> GetAbilityNameOptions();
std::vector<std::string> GetMoveNameOptions();
bool GetExperienceForSpeciesLevel(int speciesId, int level, uint32_t &experience);
bool SetEntirePartyStatus(uint32_t statusRaw, std::string &error);
bool EdgeEntireParty(uint32_t amount, std::string &error);
void Shutdown();

}  // namespace desmume_pokemon_http_export

#ifdef DESMUME_POKEMON_HTTP_EXPORT_IMPLEMENTATION

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#include "MMU.h"
#include "NDSSystem.h"
#include "pokemon_gen4_data.generated.h"
#include "pokemon_gen4_hgss_locations.generated.h"
#include "pokemon_gen4_hgss_rng_move_names.generated.h"
#include "pokemon_gen4_platinum_kaizo_data.generated.h"
#include "pokemon_gen4_sterling_silver_data.generated.h"
#include "pokemon_gen5_data.generated.h"
#include "pokemon_gen5_bb2_vw2_redux_data.generated.h"

#ifdef _WIN32
#include <direct.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace desmume_pokemon_http_export {
namespace {

static const unsigned short kHttpPort = 31124;
static const uint32_t kHttpClientTimeoutMs = 5000u;
static const uint32_t kHttpHealthCheckIntervalMs = 60000u;
static const uint32_t kHttpIdleRefreshIntervalMs = 180000u;
static const uint32_t kHttpWatchdogPollMs = 1000u;
static const uint32_t kHttpMinMaintenanceIntervalMs = 1000u;
static const size_t kPartyMonSize = 0xEC;
static const size_t kBoxMonSize = 0x88;
static const size_t kCoreBytes = 0x80;
static const size_t kBattleStatsBytes = 0x64;
static const size_t kGen5PartyMonSize = 0xDC;
static const size_t kGen5BoxMonSize = 0x88;
static const size_t kGen5CoreBytes = 0x80;
static const size_t kGen5PartyStatsBytes = 0x54;
static const size_t kPartySlotsPerBox = 30;
static const size_t kGen4TotalBoxCount = 18;
static const size_t kGen4LiveExportBoxCount = 13;
static const size_t kHgssExportBoxCount = 18;
static const size_t kBwLiveExportBoxCount = 7;
static const size_t kBwExportBoxCount = 24;
static const uint32_t kBwBrutalBlackRuntimeRootDelta = 0x40u;
static const int kDecryptRetryCount = 5;
static const int kPartyWriteReadRetryCount = 30;
static const int kPartyWriteReadRetryDelayMs = 16;
static const int kPartyEditorReadRetryCount = 10;
static const int kPartyEditorReadRetryDelayMs = 16;
static const int kPartyEditorSnapshotRetryCount = 60;
static const int kPartyEditorSnapshotRetryDelayMs = 16;
static const int kMaxReasonableMoveId = 700;
static const uint32_t kBattleSystemAnchorPointerAddr = 0x021BF680u;
static const uint32_t kBattleSystemBattleTypeOffset = 0x2Cu;
static const uint32_t kBattleSystemBattleCtxOffset = 0x30u;
static const uint32_t kBattleSystemMaxBattlersOffset = 0x44u;
static const uint32_t kBattleSystemPartiesOffset = 0x68u;
static const uint32_t kBattleSystemEnemyTrainerIdOffset = 0xA2u;
static const uint32_t kBattleContextCommandOffset = 0x08u;
static const uint32_t kBattleContextCommandNextOffset = 0x0Cu;
static const uint32_t kBattleTypeMask = 0x800007FFu;
static const uint32_t kPreferredBattleClusterStart = 0x022BF800u;
static const uint32_t kPreferredBattleClusterEnd = 0x022BFC00u;
static const uint32_t kPreferredBattleCtxDelta = 0x307Cu;
static const uint32_t kNearbyLastKnownRadius = 0x800u;
static const uint32_t kFullBattleScanStart = 0x02200000u;
static const uint32_t kFullBattleScanEnd = 0x02400000u;
static const uint32_t kBattleDetectActivePollMs = 50u;
static const uint32_t kBattleDetectFullScanRetryMs = 10000u;
static const size_t kMaxBattleLogSessions = 128;
static const uint32_t kBattleContextSelectedPartySlotOffset = 0x219Cu;
static const uint32_t kBattleContextBattleMonsOffset = 0x2D40u;
static const uint32_t kBattleContextBattleMonSize = 0xC0u;
static const uint32_t kBattleControlCalcTurnOrder = 6u;
static const uint32_t kHgssBattleAnchorPidOffset = 0x37970u;
static const uint32_t kHgssBattleFullScanStart = 0x02000000u;
static const uint32_t kHgssBattleFullScanEnd = 0x02400000u;
static const uint32_t kHgssBattleNearbyRadius = 0x3000u;
static const uint32_t kHgssBattlePartyBankOffset = 0x1C70u;
static const uint32_t kHgssBattlePartyBankOffsetAltA = kHgssBattlePartyBankOffset + 0x0A1Cu;
static const uint32_t kHgssBattlePartyBankOffsetAltB = kHgssBattlePartyBankOffset + 0x1438u;
static const uint32_t kHgssBattleDerivedBaseScanRadius = 0x200u;
static const uint32_t kBwBattleMainModuleOffsetFromCurrentBoxIndex = 0x14u;
static const uint32_t kBwBattleMainSetupParamOffset = 0x00u;
static const uint32_t kBwBattleMainServerOffset = 0x08u;
static const uint32_t kBwBattleMainPokeconForClientOffset = 0xC8u;
static const uint32_t kBwBattleMainPokeconSize = 0xE8u;
static const uint32_t kBwBattleMainPokeconForServerOffset = kBwBattleMainPokeconForClientOffset + kBwBattleMainPokeconSize;
static const uint32_t kBwBattleSetupCompetitorOffset = 0x00u;
static const uint32_t kBwBattleSetupRuleOffset = 0x04u;
static const uint32_t kBwBattleSetupTrainerDataOffset = 0x48u;
static const uint32_t kBwBattleServerMainModuleOffset = 0x0Cu;
static const uint32_t kBwBattleServerPokeConOffset = 0x10u;
static const uint32_t kBwBattleServerClientWorkOffset = 0x14u;
static const uint32_t kBwBattleServerClientWorkSize = 0x0Cu;
static const uint32_t kBwBattleServerFlowWorkOffset = 0x44u;
static const uint32_t kBwBattleFlowServerOffset = 0x00u;
static const uint32_t kBwBattleFlowMainModuleOffset = 0x04u;
static const uint32_t kBwBattlePokeConMainModuleOffset = 0x00u;
static const uint32_t kBwBattlePokeConPartyOffset = 0x04u;
static const uint32_t kBwBattlePartySize = 0x1Cu;
static const uint32_t kBwBattlePartyMemberCountOffset = 0x18u;
static const uint32_t kBwBattlePartyNumCoverPosOffset = 0x19u;
static const uint32_t kBwBattlePokeConSrcPartyOffset = 0x74u;
static const uint32_t kBwBattlePokeParamSourceOffset = 0x00u;
static const uint32_t kBwBattlePokeParamSpeciesOffset = 0x0Cu;
static const uint32_t kBwBattlePokeParamMaxHpOffset = 0x0Eu;
static const uint32_t kBwBattlePokeParamCurHpOffset = 0x10u;
static const uint32_t kBwBattlePokePartyFirstMonOffset = 0x08u;
static const uint32_t kBwBattleNearbyMainScanRadius = 0x40u;
static const uint32_t kBw2BattleMainModuleOffsetFromBattleWorkRoot = 0xF944u;
static const uint32_t kBw2RuntimePartyRootScanRadius = 0x40000u;
static const uint32_t kArm9MainRamStart = 0x02000000u;
static const uint32_t kArm9MainRamEnd = 0x02400000u;

static const int kShuffleTable[24][4] = {
	{0x08, 0x28, 0x48, 0x68}, {0x08, 0x28, 0x68, 0x48}, {0x08, 0x48, 0x28, 0x68}, {0x08, 0x68, 0x28, 0x48},
	{0x08, 0x48, 0x68, 0x28}, {0x08, 0x68, 0x48, 0x28}, {0x28, 0x08, 0x48, 0x68}, {0x28, 0x08, 0x68, 0x48},
	{0x48, 0x08, 0x28, 0x68}, {0x68, 0x08, 0x28, 0x48}, {0x48, 0x08, 0x68, 0x28}, {0x68, 0x08, 0x48, 0x28},
	{0x28, 0x48, 0x08, 0x68}, {0x28, 0x68, 0x08, 0x48}, {0x48, 0x28, 0x08, 0x68}, {0x68, 0x28, 0x08, 0x48},
	{0x48, 0x68, 0x08, 0x28}, {0x68, 0x48, 0x08, 0x28}, {0x28, 0x48, 0x68, 0x08}, {0x28, 0x68, 0x48, 0x08},
	{0x48, 0x28, 0x68, 0x08}, {0x68, 0x28, 0x48, 0x08}, {0x48, 0x68, 0x28, 0x08}, {0x68, 0x48, 0x28, 0x08}
};

static const uint8_t kMoveBasePp[] = {
	35, 25, 10, 15, 20, 20, 15, 15, 15, 35, 30, 8, 10, 30, 30, 35,
	35, 20, 15, 20, 8, 15, 20, 30, 8, 25, 15, 5, 15, 25, 20, 5,
	35, 15, 20, 20, 20, 8, 1, 35, 20, 15, 30, 25, 40, 20, 8, 20,
	20, 20, 30, 25, 15, 3, 25, 8, 15, 10, 5, 20, 20, 20, 8, 35,
	20, 8, 20, 20, 20, 15, 25, 15, 10, 5, 25, 10, 35, 30, 15, 20,
	40, 10, 15, 30, 15, 20, 10, 15, 10, 5, 10, 10, 25, 10, 20, 1,
	1, 30, 20, 20, 15, 10, 5, 6, 10, 5, 3, 5, 10, 5, 15, 5,
	2, 30, 2, 5, 10, 40, 20, 5, 10, 15, 20, 20, 20, 5, 15, 10,
	20, 15, 10, 15, 3, 15, 25, 20, 30, 15, 40, 8, 15, 10, 5, 10,
	30, 10, 15, 5, 15, 20, 1, 10, 5, 15, 10, 3, 10, 15, 3, 30,
	10, 10, 20, 3, 1, 1, 10, 10, 10, 5, 15, 25, 15, 3, 15, 30,
	5, 40, 15, 10, 25, 10, 15, 10, 20, 10, 1, 10, 10, 10, 20, 5,
	5, 5, 5, 15, 10, 10, 5, 15, 1, 15, 10, 3, 5, 40, 15, 10,
	20, 20, 25, 5, 15, 20, 1, 20, 15, 20, 5, 20, 5, 30, 5, 10,
	20, 40, 3, 15, 15, 2, 15, 35, 10, 5, 5, 5, 15, 5, 15, 1,
	1, 15, 20, 10, 8, 10, 15, 15, 15, 15, 10, 5, 10, 10, 15, 10,
	10, 1, 15, 15, 15, 10, 20, 20, 10, 20, 20, 1, 15, 20, 15, 10,
	10, 35, 1, 5, 15, 15, 10, 15, 10, 20, 5, 5, 2, 10, 10, 5,
	10, 20, 10, 15, 20, 20, 5, 5, 3, 20, 10, 15, 20, 15, 10, 10,
	15, 10, 5, 5, 10, 15, 10, 1, 3, 25, 5, 40, 10, 5, 3, 15,
	6, 6, 5, 15, 20, 30, 15, 15, 5, 10, 30, 20, 30, 3, 5, 3,
	15, 5, 3, 5, 15, 5, 15, 5, 15, 15, 3, 15, 1, 8, 15, 20,
	5, 5, 8, 5, 40, 10, 10, 8, 15, 10, 1, 10, 5, 30, 30, 10,
	20, 5, 10, 10, 15, 10, 10, 1, 15, 5, 10, 10, 30, 20, 20, 3,
	3, 5, 5, 10, 5, 20, 10, 20, 10, 8, 10, 10, 1, 20, 15, 15,
	10, 15, 20, 15, 10, 10, 10, 20, 5, 30, 5, 10, 15, 10, 15, 5,
	1, 10, 10, 10, 15, 15, 15, 15, 10, 10, 20, 15, 10, 10, 20, 1,
	1, 5, 15, 15, 5, 10, 8, 20, 8, 15, 20, 5, 3, 20, 20, 20,
	10, 20, 10, 15, 5, 15, 10, 10, 5, 10, 5, 5, 10, 5, 5, 5,
	5, 5, 5
};

static const char *GetGen4LocationNameAdapter(uint16_t locationId);
static const char *GetGen4FormSuffixAdapter(uint16_t speciesId, uint8_t formIndex);
static const char *GetPlatinumKaizoMoveNameOverride(uint16_t moveId);
static const char *GetSterlingSilverMoveNameOverride(uint16_t moveId);
static const char *GetRenegadePlatinumMoveNameOverride(uint16_t moveId);
static const char *GetBb2Vw2ReduxMoveNameOverride(uint16_t moveId);
static int GetLevelForExperienceGen4(uint32_t experience, int speciesId);
static uint32_t GetExperienceForLevelGen4(int level, int speciesId);
static int GetLevelForExperiencePlatinumKaizo(uint32_t experience, int speciesId);
static uint32_t GetExperienceForLevelPlatinumKaizo(int level, int speciesId);
static int GetLevelForExperienceSterlingSilver(uint32_t experience, int speciesId);
static uint32_t GetExperienceForLevelSterlingSilver(int level, int speciesId);
static uint8_t GetMoveBasePpGen4(uint16_t moveId);
static bool LookupMoveIdByNameGen4(const std::string &name, uint16_t &moveId, std::string &canonicalName);
struct ExportProfileDefinition;
static std::string NormalizeMoveName(const std::string &name);
static bool LookupSpeciesIdByNameForProfile(const ExportProfileDefinition *profile, const std::string &name, uint16_t &speciesId, std::string &canonicalName);
static bool LookupItemIdByNameForProfile(const ExportProfileDefinition *profile, const std::string &name, uint16_t &itemId, std::string &canonicalName);
static bool LookupAbilityIdByNameForProfile(const ExportProfileDefinition *profile, const std::string &name, uint16_t &abilityId, std::string &canonicalName);
static const char *GetGen5BwLocationNameAdapter(uint16_t locationId);
static const char *GetGen5Bw2LocationNameAdapter(uint16_t locationId);
static const char *GetHgssLocationNameAdapter(uint16_t locationId);
static const char *GetGen5FormSuffixAdapter(uint16_t speciesId, uint8_t formIndex);
static int GetLevelForExperienceGen5(uint32_t experience, int speciesId);
static uint32_t GetExperienceForLevelGen5(int level, int speciesId);
static int GetLevelForExperienceBb2Vw2Redux(uint32_t experience, int speciesId);
static uint32_t GetExperienceForLevelBb2Vw2Redux(int level, int speciesId);
static bool LookupMoveIdByNameGen5(const std::string &name, uint16_t &moveId, std::string &canonicalName);

static const GameTextTables kGen4TextTables = {
	desmume_pokemon_gen4::kSpeciesNames,
	desmume_pokemon_gen4::kSpeciesNamesCount,
	desmume_pokemon_gen4::kItemNames,
	desmume_pokemon_gen4::kItemNamesCount,
	NULL,
	desmume_pokemon_gen4::kAbilityNames,
	desmume_pokemon_gen4::kAbilityNamesCount,
	NULL,
	desmume_pokemon_gen4_hgss_rng::kMoveNames,
	desmume_pokemon_gen4_hgss_rng::kMoveNamesCount,
	NULL,
	desmume_pokemon_gen4::kNatureNames,
	desmume_pokemon_gen4::kNatureNamesCount,
	GetGen4LocationNameAdapter,
	GetGen4FormSuffixAdapter
};

static const GameTextTables kPlatinumKaizoTextTables = {
	desmume_pokemon_gen4::kSpeciesNames,
	desmume_pokemon_gen4::kSpeciesNamesCount,
	desmume_pokemon_gen4::kItemNames,
	desmume_pokemon_gen4::kItemNamesCount,
	NULL,
	desmume_pokemon_gen4::kAbilityNames,
	desmume_pokemon_gen4::kAbilityNamesCount,
	NULL,
	desmume_pokemon_gen4_hgss_rng::kMoveNames,
	desmume_pokemon_gen4_hgss_rng::kMoveNamesCount,
	GetPlatinumKaizoMoveNameOverride,
	desmume_pokemon_gen4::kNatureNames,
	desmume_pokemon_gen4::kNatureNamesCount,
	GetGen4LocationNameAdapter,
	GetGen4FormSuffixAdapter
};

static const GameTextTables kSterlingSilverTextTables = {
	desmume_pokemon_gen4::kSpeciesNames,
	desmume_pokemon_gen4::kSpeciesNamesCount,
	desmume_pokemon_gen4::kItemNames,
	desmume_pokemon_gen4::kItemNamesCount,
	NULL,
	desmume_pokemon_gen4::kAbilityNames,
	desmume_pokemon_gen4::kAbilityNamesCount,
	NULL,
	desmume_pokemon_gen4_hgss_rng::kMoveNames,
	desmume_pokemon_gen4_hgss_rng::kMoveNamesCount,
	GetSterlingSilverMoveNameOverride,
	desmume_pokemon_gen4::kNatureNames,
	desmume_pokemon_gen4::kNatureNamesCount,
	GetHgssLocationNameAdapter,
	GetGen4FormSuffixAdapter
};

static const GameTextTables kHgssTextTables = {
	desmume_pokemon_gen4::kSpeciesNames,
	desmume_pokemon_gen4::kSpeciesNamesCount,
	desmume_pokemon_gen4::kItemNames,
	desmume_pokemon_gen4::kItemNamesCount,
	NULL,
	desmume_pokemon_gen4::kAbilityNames,
	desmume_pokemon_gen4::kAbilityNamesCount,
	NULL,
	desmume_pokemon_gen4_hgss_rng::kMoveNames,
	desmume_pokemon_gen4_hgss_rng::kMoveNamesCount,
	NULL,
	desmume_pokemon_gen4::kNatureNames,
	desmume_pokemon_gen4::kNatureNamesCount,
	GetHgssLocationNameAdapter,
	GetGen4FormSuffixAdapter
};

static const GameTextTables kRenegadePlatinumTextTables = {
	desmume_pokemon_gen4::kSpeciesNames,
	desmume_pokemon_gen4::kSpeciesNamesCount,
	desmume_pokemon_gen4::kItemNames,
	desmume_pokemon_gen4::kItemNamesCount,
	NULL,
	desmume_pokemon_gen4::kAbilityNames,
	desmume_pokemon_gen4::kAbilityNamesCount,
	NULL,
	desmume_pokemon_gen4_hgss_rng::kMoveNames,
	desmume_pokemon_gen4_hgss_rng::kMoveNamesCount,
	GetRenegadePlatinumMoveNameOverride,
	desmume_pokemon_gen4::kNatureNames,
	desmume_pokemon_gen4::kNatureNamesCount,
	GetGen4LocationNameAdapter,
	GetGen4FormSuffixAdapter
};

static const PokemonMechanics kGen4Mechanics = {
	GetLevelForExperienceGen4,
	GetExperienceForLevelGen4,
	GetMoveBasePpGen4,
	LookupMoveIdByNameGen4
};

static const PokemonMechanics kPlatinumKaizoMechanics = {
	GetLevelForExperiencePlatinumKaizo,
	GetExperienceForLevelPlatinumKaizo,
	GetMoveBasePpGen4,
	LookupMoveIdByNameGen4
};

static const PokemonMechanics kSterlingSilverMechanics = {
	GetLevelForExperienceSterlingSilver,
	GetExperienceForLevelSterlingSilver,
	GetMoveBasePpGen4,
	LookupMoveIdByNameGen4
};

static const GameTextTables kGen5BwTextTables = {
	desmume_pokemon_gen5::kSpeciesNames,
	desmume_pokemon_gen5::kSpeciesNamesCount,
	desmume_pokemon_gen5::kItemNames,
	desmume_pokemon_gen5::kItemNamesCount,
	NULL,
	desmume_pokemon_gen5::kAbilityNames,
	desmume_pokemon_gen5::kAbilityNamesCount,
	NULL,
	desmume_pokemon_gen5::kMoveNames,
	desmume_pokemon_gen5::kMoveNamesCount,
	NULL,
	desmume_pokemon_gen5::kNatureNames,
	desmume_pokemon_gen5::kNatureNamesCount,
	GetGen5BwLocationNameAdapter,
	GetGen5FormSuffixAdapter
};

static const GameTextTables kGen5Bw2TextTables = {
	desmume_pokemon_gen5::kSpeciesNames,
	desmume_pokemon_gen5::kSpeciesNamesCount,
	desmume_pokemon_gen5::kItemNames,
	desmume_pokemon_gen5::kItemNamesCount,
	NULL,
	desmume_pokemon_gen5::kAbilityNames,
	desmume_pokemon_gen5::kAbilityNamesCount,
	NULL,
	desmume_pokemon_gen5::kMoveNames,
	desmume_pokemon_gen5::kMoveNamesCount,
	NULL,
	desmume_pokemon_gen5::kNatureNames,
	desmume_pokemon_gen5::kNatureNamesCount,
	GetGen5Bw2LocationNameAdapter,
	GetGen5FormSuffixAdapter
};

static const GameTextTables kBb2Vw2ReduxTextTables = {
	desmume_pokemon_gen5::kSpeciesNames,
	desmume_pokemon_gen5::kSpeciesNamesCount,
	desmume_pokemon_gen5::kItemNames,
	desmume_pokemon_gen5::kItemNamesCount,
	NULL,
	desmume_pokemon_gen5::kAbilityNames,
	desmume_pokemon_gen5::kAbilityNamesCount,
	NULL,
	desmume_pokemon_gen5::kMoveNames,
	desmume_pokemon_gen5::kMoveNamesCount,
	GetBb2Vw2ReduxMoveNameOverride,
	desmume_pokemon_gen5::kNatureNames,
	desmume_pokemon_gen5::kNatureNamesCount,
	GetGen5Bw2LocationNameAdapter,
	GetGen5FormSuffixAdapter
};

static const PokemonMechanics kGen5Mechanics = {
	GetLevelForExperienceGen5,
	GetExperienceForLevelGen5,
	GetMoveBasePpGen4,
	LookupMoveIdByNameGen5
};

static const PokemonMechanics kBb2Vw2ReduxMechanics = {
	GetLevelForExperienceBb2Vw2Redux,
	GetExperienceForLevelBb2Vw2Redux,
	GetMoveBasePpGen4,
	LookupMoveIdByNameGen5
};

static const PokemonMemoryLayout kGen4PokemonLayout = {
	POKEMON_DATA_CODEC_GEN4,
	0x02101D2Cu,
	0x021C0794u,
	"USA",
	0xD090u,
	0xD094u,
	0x19F20u,
	0x19F24u,
	kPartyMonSize,
	kBoxMonSize,
	kCoreBytes,
	kBattleStatsBytes,
	kPartySlotsPerBox,
	kGen4TotalBoxCount,
	kPartySlotsPerBox * kBoxMonSize
};

// Pokestats45.lua confirms the HGSS/HGE pid pointer and box storage base.
// The HGSS decomp defines PCStorage as 18 boxes followed by curBox at +0x12000.
static const PokemonMemoryLayout kHgssPokemonLayout = {
	POKEMON_DATA_CODEC_GEN4,
	0x0211186Cu,
	0x00000000u,
	"USA",
	0xD084u,
	0xD088u,
	0x2E6F0u,
	0x1C6F0u,
	kPartyMonSize,
	kBoxMonSize,
	kCoreBytes,
	kBattleStatsBytes,
	kPartySlotsPerBox,
	kHgssExportBoxCount,
	0x1000u
};

static const PokemonMemoryLayout kGen5BwPokemonLayout = {
	POKEMON_DATA_CODEC_GEN5,
	0x00000000u,
	0x00000000u,
	"",
	0x00000000u,
	0x00000000u,
	0x00000000u,
	0x00000000u,
	kGen5PartyMonSize,
	kGen5BoxMonSize,
	kGen5CoreBytes,
	kGen5PartyStatsBytes,
	kPartySlotsPerBox,
	kBwExportBoxCount,
	0x1000u
};

static const BattleMemoryLayout kGen4BattleLayout = {
	true,
	kBattleSystemAnchorPointerAddr,
	kBattleSystemBattleTypeOffset,
	kBattleSystemBattleCtxOffset,
	kBattleSystemMaxBattlersOffset,
	kBattleSystemPartiesOffset,
	kBattleSystemEnemyTrainerIdOffset,
	kBattleContextCommandOffset,
	kBattleContextCommandNextOffset,
	kBattleContextSelectedPartySlotOffset,
	kBattleContextBattleMonsOffset,
	kBattleContextBattleMonSize,
	kBattleTypeMask,
	kPreferredBattleClusterStart,
	kPreferredBattleClusterEnd,
	kPreferredBattleCtxDelta,
	kNearbyLastKnownRadius,
	kFullBattleScanStart,
	kFullBattleScanEnd,
	kBattleDetectActivePollMs,
	kBattleDetectFullScanRetryMs,
	kBattleControlCalcTurnOrder
};

static const BattleMemoryLayout kHgssBattleLayout = {
	true,
	0u,
	kBattleSystemBattleTypeOffset,
	kBattleSystemBattleCtxOffset,
	kBattleSystemMaxBattlersOffset,
	0x68u,
	0xA2u,
	kBattleContextCommandOffset,
	kBattleContextCommandNextOffset,
	kBattleContextSelectedPartySlotOffset,
	kBattleContextBattleMonsOffset,
	kBattleContextBattleMonSize,
	kBattleTypeMask,
	0u,
	0u,
	0u,
	kHgssBattleNearbyRadius,
	kHgssBattleFullScanStart,
	kHgssBattleFullScanEnd,
	kBattleDetectActivePollMs,
	kBattleDetectFullScanRetryMs,
	kBattleControlCalcTurnOrder
};

static const BattleMemoryLayout kBwBattleLayout = {
	true,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	kBattleDetectActivePollMs,
	kBattleDetectFullScanRetryMs,
	0xFFFFFFFFu
};

static const BattleMemoryLayout kUnsupportedBattleLayout = {
	false,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u
};

struct ExportProfileDefinition {
	ExportGameProfile profile;
	const char *displayName;
	const char *gameName;
	const GameTextTables *textTables;
	const PokemonMechanics *mechanics;
	const PokemonMemoryLayout *pokemonLayout;
	const BattleMemoryLayout *battleLayout;
};

struct PokemonRuntimeAddresses {
	uint32_t pidPointerAddr;
	uint32_t pidAddr;
	uint32_t partyCountAddr;
	uint32_t partyBaseAddr;
	uint32_t currentBoxIndexAddr;
	uint32_t currentBoxIndexFallbackAddr;
	uint32_t bwBattleMainAddr;
	uint32_t boxBaseAddr;
	uint32_t trainerIdsPointerAddr;
	uint32_t trainerIdsAddr;
	std::string language;
};

struct DecryptedPokemon {
	std::vector<uint16_t> words;
	uint32_t pid;
	uint16_t checksum;
	bool checksumValid;
};

enum PartyPokemonMemoryMode {
	PARTY_MEMORY_MODE_ENCRYPTED = 0,
	PARTY_MEMORY_MODE_ALREADY_DECRYPTED = 1
};

struct PartyPokemonReadMode {
	PartyPokemonMemoryMode storageMode;
	PartyPokemonMemoryMode battleMode;
};

enum PartyFrameClassification {
	PARTY_FRAME_INVALID = 0,
	PARTY_FRAME_TRANSIENT_PLAINTEXT = 1,
	PARTY_FRAME_VALID = 2
};

struct PartyFrameAnalysis {
	PartyFrameClassification classification;
	DecryptedPokemon pokemon;
	PartyPokemonReadMode readMode;
	bool hasDecodedPokemon;
};

struct Snapshot {
	bool success;
	uint64_t updatedAtUnixMs;
	std::string error;
	std::string showdown;
	std::string rawJson;
	bool trainerIdsResolved;
	uint16_t trainerId;
	uint16_t secretId;
};

struct PackedSnapshot {
	bool success;
	std::string error;
	std::string payload;
};

struct BattleLogSnapshot {
	bool success;
	std::string error;
	std::string payload;
};

struct ActiveBattleSnapshot {
	bool success;
	std::string error;
	std::string json;
};

struct BattleDetectionSnapshot {
	bool valid;
	bool battleActive;
	bool trainerBattleActive;
	bool trainerBattleCanStartSession;
	uint32_t battleSystemAddr;
	uint32_t battleContextAddr;
	uint32_t battleType;
	uint16_t enemyTrainerId;
	int maxBattlers;
	uint32_t command;
	uint32_t commandNext;
	std::string detectionSource;
	int detectionSourceDepth;
	int detectionConfidence;
};

struct BattleMonRuntimeSnapshot {
	bool valid;
	uint8_t battler;
	uint8_t partySlot;
	uint32_t pid;
	uint16_t speciesId;
	uint32_t curHp;
	uint32_t maxHp;
	uint32_t sourceMonAddr;
};

struct BattleLogEnemyPartySnapshot {
	bool valid;
	uint32_t partyPtr;
	uint8_t slot;
	uint32_t sourceMonAddr;
	uint8_t inferredPlayerSlot;
	uint16_t speciesId;
	uint32_t curHp;
	uint32_t maxHp;
};

struct BattleLogRecordedEnemyKo {
	uint32_t partyPtr;
	uint8_t slot;
	uint16_t speciesId;
};

struct AttemptMapData {
	uint32_t nextAttempt;
	std::map<std::string, uint32_t> map;
};

#ifdef _WIN32
typedef SOCKET SocketHandle;
static const SocketHandle kInvalidSocket = INVALID_SOCKET;
#else
typedef int SocketHandle;
static const SocketHandle kInvalidSocket = -1;
#endif

static uint64_t CurrentUnixMs()
{
	return static_cast<uint64_t>(std::time(NULL)) * 1000ULL;
}

static void CloseSocket(SocketHandle sock)
{
	if (sock == kInvalidSocket) {
		return;
	}
#ifdef _WIN32
	closesocket(sock);
#else
	close(sock);
#endif
}

static void ShutdownSocket(SocketHandle sock)
{
	if (sock == kInvalidSocket) {
		return;
	}
#ifdef _WIN32
	shutdown(sock, SD_BOTH);
#else
	shutdown(sock, SHUT_RDWR);
#endif
}

static void SetSocketTimeouts(SocketHandle sock, uint32_t timeoutMs)
{
	if (sock == kInvalidSocket) {
		return;
	}
#ifdef _WIN32
	const DWORD timeout = static_cast<DWORD>(timeoutMs);
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout), sizeof(timeout));
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&timeout), sizeof(timeout));
#else
	timeval timeout;
	timeout.tv_sec = static_cast<time_t>(timeoutMs / 1000u);
	timeout.tv_usec = static_cast<suseconds_t>((timeoutMs % 1000u) * 1000u);
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#endif
}

static std::string BuildUrl()
{
	return "http://127.0.0.1:" + std::to_string(kHttpPort) + "/box";
}

static std::string GameCodeString();
static const ExportProfileDefinition *GetExportProfileDefinition(ExportGameProfile profile);
static uint32_t AdvanceLcrng(uint32_t seed);
static uint8_t ReadArm9U8(uint32_t address);
static uint16_t ReadArm9U16(uint32_t address);
static uint32_t ReadArm9U32(uint32_t address);
static const char *LookupOneBasedName(const char *const *names, size_t count, int id);
static bool IsHgssLikeProfile(ExportGameProfile profile);
static bool HasConfiguredPidRoot(const PokemonMemoryLayout *layout);
static bool HasConfiguredTrainerIdsRoot(const PokemonMemoryLayout *layout);
static bool HasConfiguredBoxRoot(const PokemonMemoryLayout *layout);
static void BuildBattleMonRuntimeSnapshot(const BattleMemoryLayout &layout, uint32_t battleCtxAddr, std::vector<BattleMonRuntimeSnapshot> &outMons);
static void BuildBattleMonRuntimeSnapshotForProfile(
	const ExportProfileDefinition *profile,
	const BattleMemoryLayout &layout,
	uint32_t battleSystemAddr,
	uint32_t battleCtxAddr,
	std::vector<BattleMonRuntimeSnapshot> &outMons);
static void BuildEnemyPartyRuntimeSnapshotForProfile(
	const ExportProfileDefinition *profile,
	const BattleMemoryLayout &layout,
	uint32_t battleSystemAddr,
	const std::vector<BattleMonRuntimeSnapshot> &battleMons,
	const std::vector<BattleLogEnemyPartySnapshot> &previousEnemyPartyMons,
	std::vector<BattleLogEnemyPartySnapshot> &outMons);
static bool IsValidEnemyPartySpeciesForProfile(const ExportProfileDefinition *profile, uint16_t speciesId);
static bool ReadPartyPokemonRobustForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	DecryptedPokemon &outPokemon,
	PartyPokemonReadMode &modeOut);
static bool TryResolveHgssTrainerIds(const ExportProfileDefinition *profile, HgssDiscoveryStatus &probeStatus, uint16_t &trainerIdOut, uint16_t &secretIdOut);
static bool TryResolveCurrentBadgeStatus(ExportGameProfile selectedProfile, PlayerBadgeStatus &statusOut, std::string &errorOut);

static const GameTextTables *GetProfileTextTables(const ExportProfileDefinition *profile)
{
	return (profile != NULL) ? profile->textTables : NULL;
}

static const PokemonMechanics *GetProfileMechanics(const ExportProfileDefinition *profile)
{
	return (profile != NULL) ? profile->mechanics : NULL;
}

static const PokemonMemoryLayout *GetProfilePokemonLayout(const ExportProfileDefinition *profile)
{
	return (profile != NULL) ? profile->pokemonLayout : NULL;
}

static const BattleMemoryLayout *GetProfileBattleLayout(const ExportProfileDefinition *profile)
{
	return (profile != NULL) ? profile->battleLayout : NULL;
}

static bool IsGen5BwLikePokemonProfile(ExportGameProfile profile)
{
	return (profile == EXPORT_GAME_POKEMON_BLACK_WHITE)
		|| (profile == EXPORT_GAME_POKEMON_BLACK2_WHITE2)
		|| (profile == EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX);
}

static bool IsPlatinumLikeProfile(ExportGameProfile profile)
{
	return (profile == EXPORT_GAME_POKEMON_PLATINUM)
		|| (profile == EXPORT_GAME_POKEMON_PLATINUM_KAIZO)
		|| (profile == EXPORT_GAME_POKEMON_RENEGADE_PLATINUM);
}

static bool IsGen4LikeProfile(ExportGameProfile profile)
{
	return IsPlatinumLikeProfile(profile) || IsHgssLikeProfile(profile);
}

static bool IsPlausibleBwPartyRuntimeRoot(uint32_t partyCountAddr, uint32_t partyAddr)
{
	// In BW, partyAddr points to member[0]. The two preceding words are
	// PokeCountMax and PokeCount, respectively.
	if ((partyAddr < (kArm9MainRamStart + 0x8u)) ||
	    (partyAddr > (kArm9MainRamEnd - static_cast<uint32_t>(kGen5PartyMonSize))) ||
	    (partyCountAddr != (partyAddr - 0x4u))) {
		return false;
	}

	return (ReadArm9U32(partyAddr - 0x8u) == 6u) &&
	       (ReadArm9U32(partyCountAddr) <= 6u);
}

static bool IsValidGen5StoredPartyPokemon(uint32_t monAddr)
{
	if (monAddr < kArm9MainRamStart ||
	    monAddr > (kArm9MainRamEnd - static_cast<uint32_t>(kGen5PartyMonSize))) {
		return false;
	}

	const uint32_t pid = ReadArm9U32(monAddr);
	const uint16_t sanityFlags = ReadArm9U16(monAddr + 0x4u);
	const uint16_t expectedChecksum = ReadArm9U16(monAddr + 0x6u);
	const bool alreadyDecrypted = (sanityFlags & (1u << 1)) != 0u;
	const int shuffleKey = static_cast<int>(((pid & 0x3E000u) >> 13) % 24u);
	const uint32_t blockAOffset = static_cast<uint32_t>(kShuffleTable[shuffleKey][0]);

	uint32_t seed = expectedChecksum;
	uint32_t checksum = 0u;
	uint16_t species = 0u;
	for (uint32_t offset = 0x8u; offset < (0x8u + kGen5CoreBytes); offset += 2u) {
		uint16_t word = ReadArm9U16(monAddr + offset);
		if (!alreadyDecrypted) {
			seed = AdvanceLcrng(seed);
			word = static_cast<uint16_t>(word ^ (seed >> 16));
		}
		checksum = (checksum + word) & 0xFFFFu;
		if (offset == blockAOffset) {
			species = word;
		}
	}

	return checksum == expectedChecksum && species != 0u && species <= 1023u;
}

static bool IsValidatedBw2PartyRuntimeRoot(uint32_t partyAddr, bool allowEmptyParty)
{
	if (!IsPlausibleBwPartyRuntimeRoot(partyAddr - 0x4u, partyAddr)) {
		return false;
	}

	const uint32_t partyCount = ReadArm9U32(partyAddr - 0x4u);
	if (partyCount == 0u) {
		return allowEmptyParty;
	}

	for (uint32_t slot = 0u; slot < partyCount; ++slot) {
		const uint32_t monAddr = partyAddr
			+ slot * static_cast<uint32_t>(kGen5PartyMonSize);
		if (!IsValidGen5StoredPartyPokemon(monAddr)) {
			return false;
		}
	}
	return true;
}

static bool IsValidatedBw2SaveRuntimeRoot(uint32_t partyAddr)
{
	if (!IsValidatedBw2PartyRuntimeRoot(partyAddr, false) ||
	    partyAddr < (kArm9MainRamStart + 0x18A08u + 0x3E0u)) {
		return false;
	}

	const uint32_t partyObjectAddr = partyAddr - 0x8u;
	const uint32_t boxBaseAddr = partyObjectAddr - 0x18A00u;
	const uint32_t currentBoxIndexAddr = boxBaseAddr - 0x3E0u;
	return ReadArm9U8(currentBoxIndexAddr) < kBwExportBoxCount;
}

static bool TryDiscoverBw2PartyRuntimeRoot(
	uint32_t expectedPartyAddr,
	uint32_t &partyAddrOut)
{
	// The canonical address remains useful before a save is loaded, when an
	// empty party is legitimate. Relocated candidates must contain at least one
	// checksum-valid Pokemon so an unrelated { max=6, count<=6 } pair cannot be
	// mistaken for the save-data party object.
	const bool canonicalEmptyParty =
		IsValidatedBw2PartyRuntimeRoot(expectedPartyAddr, true) &&
		ReadArm9U32(expectedPartyAddr - 0x4u) == 0u;
	if (IsValidatedBw2PartyRuntimeRoot(expectedPartyAddr, false)) {
		partyAddrOut = expectedPartyAddr;
		return true;
	}

	static std::mutex cacheMutex;
	static std::map<uint32_t, uint32_t> cachedPartyRoots;
	uint32_t cachedPartyAddr = 0u;
	{
		std::lock_guard<std::mutex> lock(cacheMutex);
		const std::map<uint32_t, uint32_t>::const_iterator cached =
			cachedPartyRoots.find(expectedPartyAddr);
		if (cached != cachedPartyRoots.end()) {
			cachedPartyAddr = cached->second;
		}
	}
	if (cachedPartyAddr != 0u &&
	    IsValidatedBw2SaveRuntimeRoot(cachedPartyAddr)) {
		partyAddrOut = cachedPartyAddr;
		return true;
	}

	for (uint32_t distance = 4u; distance <= kBw2RuntimePartyRootScanRadius; distance += 4u) {
		const uint32_t higher = expectedPartyAddr + distance;
		if (higher > expectedPartyAddr &&
		    IsValidatedBw2SaveRuntimeRoot(higher)) {
			partyAddrOut = higher;
			std::lock_guard<std::mutex> lock(cacheMutex);
			cachedPartyRoots[expectedPartyAddr] = higher;
			return true;
		}

		if (expectedPartyAddr >= distance) {
			const uint32_t lower = expectedPartyAddr - distance;
			if (IsValidatedBw2SaveRuntimeRoot(lower)) {
				partyAddrOut = lower;
				std::lock_guard<std::mutex> lock(cacheMutex);
				cachedPartyRoots[expectedPartyAddr] = lower;
				return true;
			}
		}
	}

	if (canonicalEmptyParty) {
		partyAddrOut = expectedPartyAddr;
		return true;
	}

	return false;
}

static bool TryResolveBwRuntimeAddresses(const std::string &gameCode, PokemonRuntimeAddresses &addressesOut, std::string &errorOut)
{
	struct BwBaseRoots {
		char languageCode;
		const char *languageName;
		uint32_t boxAddr;
		uint32_t partyCountAddr;
		uint32_t partyAddr;
		uint32_t trainerIdsAddr;
		uint32_t currentBoxIndexAddr;
	};

	static const BwBaseRoots kBwBaseRoots[] = {
		{'D', "GER", 0x221BEECu, 0x22348F0u, 0x22348F4u, 0x2234F00u, 0x22696C0u},
		{'F', "FRE", 0x221BF2Cu, 0x2234930u, 0x2234934u, 0x2234F40u, 0x2269700u},
		{'I', "ITA", 0x221BEACu, 0x22348B0u, 0x22348B4u, 0x2234EC0u, 0x2269680u},
		{'J', "JPN", 0x221BE0Cu, 0x2234810u, 0x2234814u, 0x2234E20u, 0x22695E0u},
		{'K', "KOR", 0x221C6ACu, 0x22350B0u, 0x22350B4u, 0x22356C0u, 0x2269E80u},
		{'O', "USA", 0x221BFACu, 0x22349B0u, 0x22349B4u, 0x2234FC0u, 0x2269780u},
		{'S', "SPA", 0x221BF6Cu, 0x2234970u, 0x2234974u, 0x2234F80u, 0x2269740u}
	};

	addressesOut = PokemonRuntimeAddresses();
	errorOut.clear();

	if (gameCode.size() != 4u) {
		errorOut = "BW runtime roots require a loaded BW game code";
		return false;
	}

	const char versionCode = gameCode[2];
	const char languageCode = gameCode[3];
	if (versionCode != 'A' && versionCode != 'B') {
		errorOut = "Loaded ROM is not a supported Pokemon Black/White build";
		return false;
	}

	const BwBaseRoots *baseRoots = NULL;
	for (size_t i = 0; i < (sizeof(kBwBaseRoots) / sizeof(kBwBaseRoots[0])); ++i) {
		if (kBwBaseRoots[i].languageCode == languageCode) {
			baseRoots = &kBwBaseRoots[i];
			break;
		}
	}
	if (baseRoots == NULL) {
		errorOut = "Unsupported Pokemon Black/White language for live RAM export";
		return false;
	}

	const uint32_t whiteDelta = (versionCode == 'A' && languageCode != 'K') ? 0x20u : 0u;
	uint32_t runtimeDelta = whiteDelta;

	// Brutal Black retains IRBO, so the ROM header alone cannot distinguish it
	// from vanilla US Black. Its live save-data roots are relocated by +0x40.
	// Prefer the canonical roots whenever their party header is valid, then
	// probe the known relocation so vanilla BW behavior remains unchanged.
	const uint32_t canonicalPartyCountAddr = baseRoots->partyCountAddr + runtimeDelta;
	const uint32_t canonicalPartyAddr = baseRoots->partyAddr + runtimeDelta;
	if (!IsPlausibleBwPartyRuntimeRoot(canonicalPartyCountAddr, canonicalPartyAddr) &&
	    IsPlausibleBwPartyRuntimeRoot(
		    canonicalPartyCountAddr + kBwBrutalBlackRuntimeRootDelta,
		    canonicalPartyAddr + kBwBrutalBlackRuntimeRootDelta)) {
		runtimeDelta += kBwBrutalBlackRuntimeRootDelta;
	}

	addressesOut.partyCountAddr = baseRoots->partyCountAddr + runtimeDelta;
	addressesOut.partyBaseAddr = baseRoots->partyAddr + runtimeDelta;
	addressesOut.currentBoxIndexAddr = baseRoots->currentBoxIndexAddr + runtimeDelta;
	addressesOut.boxBaseAddr = baseRoots->boxAddr + runtimeDelta;
	addressesOut.trainerIdsAddr = baseRoots->trainerIdsAddr + runtimeDelta;
	addressesOut.language.assign(baseRoots->languageName);
	return true;
}

struct Bw2RuntimeRoot {
	const char *gameCode;
	const char *languageName;
	uint32_t partyAddr;
	uint32_t battleWorkRootAddr;
};

static bool TryResolveBw2RuntimeAddressesFromRoots(
	const std::string &gameCode,
	const Bw2RuntimeRoot *roots,
	size_t rootCount,
	const char *profileName,
	PokemonRuntimeAddresses &addressesOut,
	std::string &errorOut)
{
	addressesOut = PokemonRuntimeAddresses();
	errorOut.clear();

	const Bw2RuntimeRoot *baseRoots = NULL;
	for (size_t i = 0; i < rootCount; ++i) {
		if (gameCode == roots[i].gameCode) {
			baseRoots = &roots[i];
			break;
		}
	}
	if (baseRoots == NULL) {
		errorOut = std::string("Loaded ROM is not a supported US ") + profileName + " build";
		return false;
	}

	uint32_t resolvedPartyAddr = 0u;
	if (!TryDiscoverBw2PartyRuntimeRoot(baseRoots->partyAddr, resolvedPartyAddr)) {
		errorOut = std::string("Could not locate the live ") + profileName
			+ " party storage root";
		return false;
	}

	const int32_t runtimeDelta = static_cast<int32_t>(
		resolvedPartyAddr - baseRoots->partyAddr);
	const uint32_t battleWorkRootAddr = (baseRoots->battleWorkRootAddr != 0u)
		? static_cast<uint32_t>(baseRoots->battleWorkRootAddr + runtimeDelta)
		: 0u;
	const uint32_t partyObjectAddr = resolvedPartyAddr - 0x8u;
	addressesOut.partyCountAddr = resolvedPartyAddr - 0x4u;
	addressesOut.partyBaseAddr = resolvedPartyAddr;
	addressesOut.boxBaseAddr = partyObjectAddr - 0x18A00u;
	addressesOut.currentBoxIndexAddr = addressesOut.boxBaseAddr - 0x3E0u;
	addressesOut.currentBoxIndexFallbackAddr = (battleWorkRootAddr != 0u)
		? (battleWorkRootAddr + 0xF9A8u)
		: 0u;
	addressesOut.bwBattleMainAddr = (battleWorkRootAddr != 0u)
		? (battleWorkRootAddr + kBw2BattleMainModuleOffsetFromBattleWorkRoot)
		: 0u;
	addressesOut.trainerIdsAddr = partyObjectAddr + 0x614u;
	addressesOut.language.assign(baseRoots->languageName);
	return true;
}

static bool TryResolveBw2RuntimeAddresses(const std::string &gameCode, PokemonRuntimeAddresses &addressesOut, std::string &errorOut)
{
	static const Bw2RuntimeRoot kBw2BaseRoots[] = {
		{"IREO", "USA", 0x022214ECu, 0x0224795Cu}, // Black 2
		{"IRDO", "USA", 0x0221E42Cu, 0x0224799Cu}  // White 2
	};

	return TryResolveBw2RuntimeAddressesFromRoots(
		gameCode,
		kBw2BaseRoots,
		sizeof(kBw2BaseRoots) / sizeof(kBw2BaseRoots[0]),
		"Pokemon Black 2/White 2",
		addressesOut,
		errorOut);
}

static bool TryResolveBw2ReduxRuntimeAddresses(const std::string &gameCode, PokemonRuntimeAddresses &addressesOut, std::string &errorOut)
{
	static const Bw2RuntimeRoot kBw2ReduxBaseRoots[] = {
		{"IREO", "USA", 0x0221E3ECu, 0x0224795Cu}, // Blaze Black 2 Redux
		{"IRDO", "USA", 0x0221E42Cu, 0x0224799Cu}  // Volt White 2 Redux matches vanilla White 2 storage roots
	};

	return TryResolveBw2RuntimeAddressesFromRoots(
		gameCode,
		kBw2ReduxBaseRoots,
		sizeof(kBw2ReduxBaseRoots) / sizeof(kBw2ReduxBaseRoots[0]),
		"Pokemon Blaze Black 2 Redux/Volt White 2 Redux",
		addressesOut,
		errorOut);
}

static bool TryResolveGen5BwLikePokemonRuntimeAddresses(ExportGameProfile profile, const std::string &gameCode, PokemonRuntimeAddresses &addressesOut, std::string &errorOut)
{
	if (profile == EXPORT_GAME_POKEMON_BLACK_WHITE) {
		return TryResolveBwRuntimeAddresses(gameCode, addressesOut, errorOut);
	}
	if (profile == EXPORT_GAME_POKEMON_BLACK2_WHITE2) {
		return TryResolveBw2RuntimeAddresses(gameCode, addressesOut, errorOut);
	}
	if (profile == EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX) {
		return TryResolveBw2ReduxRuntimeAddresses(gameCode, addressesOut, errorOut);
	}

	addressesOut = PokemonRuntimeAddresses();
	errorOut = "Selected profile is not a supported Gen 5 Pokemon storage profile";
	return false;
}

static bool IsProfileCompatibleWithGameCode(ExportGameProfile profile, const std::string &gameCode)
{
	switch (profile) {
		case EXPORT_GAME_POKEMON_PLATINUM:
		case EXPORT_GAME_POKEMON_PLATINUM_KAIZO:
		case EXPORT_GAME_POKEMON_RENEGADE_PLATINUM:
			return (gameCode == "CPUE") || (gameCode == "JAK7");
		case EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER:
		case EXPORT_GAME_POKEMON_HEART_GOLD_ENGINE:
		case EXPORT_GAME_POKEMON_STERLING_SILVER:
			return (gameCode == "IPKE") || (gameCode == "IPGE");
		case EXPORT_GAME_POKEMON_BLACK_WHITE: {
			PokemonRuntimeAddresses addresses;
			std::string error;
			return TryResolveBwRuntimeAddresses(gameCode, addresses, error);
		}
		case EXPORT_GAME_POKEMON_BLACK2_WHITE2: {
			PokemonRuntimeAddresses addresses;
			std::string error;
			return TryResolveBw2RuntimeAddresses(gameCode, addresses, error);
		}
		case EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX: {
			PokemonRuntimeAddresses addresses;
			std::string error;
			return TryResolveBw2ReduxRuntimeAddresses(gameCode, addresses, error);
		}
		default:
			return false;
	}
}

static uint32_t GetHgssPidPointerAddrForGameCode(const std::string &gameCode)
{
	if (gameCode == "IPKE" || gameCode == "IPGE") return 0x0211186Cu;
	return 0u;
}

static uint32_t GetHgssTrainerIdsPointerAddrForGameCode(const std::string &gameCode)
{
	if (gameCode == "IPKE" || gameCode == "IPGE") return 0x021D2228u;
	return 0u;
}

static uint32_t GetEffectivePidPointerAddr(const ExportProfileDefinition *profile)
{
	if (profile == NULL) {
		return 0u;
	}

	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		return 0u;
	}

	if (IsHgssLikeProfile(profile->profile)) {
		const uint32_t overrideAddr = GetHgssPidPointerAddrForGameCode(GameCodeString());
		if (overrideAddr != 0u) {
			return overrideAddr;
		}
	}

	return layout->pidPointerAddr;
}

static uint32_t GetEffectiveTrainerIdsPointerAddr(const ExportProfileDefinition *profile)
{
	if (profile == NULL) {
		return 0u;
	}

	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		return 0u;
	}

	if (IsHgssLikeProfile(profile->profile)) {
		const uint32_t overrideAddr = GetHgssTrainerIdsPointerAddrForGameCode(GameCodeString());
		if (overrideAddr != 0u) {
			return overrideAddr;
		}
	}

	return layout->trainerIDsPointerAddr;
}

static uint32_t GetTrainerIdsValueOffset(const ExportProfileDefinition *profile)
{
	if (profile != NULL && IsHgssLikeProfile(profile->profile)) {
		return 0x84u;
	}

	return 0x8Cu;
}

static bool ResolvePokemonRuntimeAddresses(const ExportProfileDefinition *profile, PokemonRuntimeAddresses &addressesOut, std::string &errorOut)
{
	addressesOut = PokemonRuntimeAddresses();
	errorOut.clear();

	if (profile == NULL) {
		errorOut = "No Pokemon export game selected";
		return false;
	}

	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = std::string(profile->displayName) + " memory layout is unavailable";
		return false;
	}

	const std::string gameCode = GameCodeString();
	if (!IsProfileCompatibleWithGameCode(profile->profile, gameCode)) {
		errorOut = std::string(profile->displayName) + " does not match loaded game code " + gameCode;
		return false;
	}

	if (IsGen5BwLikePokemonProfile(profile->profile)) {
		return TryResolveGen5BwLikePokemonRuntimeAddresses(profile->profile, gameCode, addressesOut, errorOut);
	}

	if (!HasConfiguredPidRoot(layout)) {
		errorOut = std::string(profile->displayName) + " pid root is not configured";
		return false;
	}

	addressesOut.pidPointerAddr = GetEffectivePidPointerAddr(profile);
	addressesOut.pidAddr = ReadArm9U32((addressesOut.pidPointerAddr != 0u) ? addressesOut.pidPointerAddr : layout->pidPointerAddr);
	if (addressesOut.pidAddr == 0u) {
		errorOut = std::string(profile->displayName) + " pid pointer is null";
		return false;
	}

	addressesOut.partyCountAddr = addressesOut.pidAddr + layout->partyCountOffset;
	addressesOut.partyBaseAddr = addressesOut.pidAddr + layout->partyDataOffset;
	addressesOut.currentBoxIndexAddr = addressesOut.pidAddr + layout->currentBoxIndexOffset;
	addressesOut.boxBaseAddr = addressesOut.pidAddr + layout->boxDataOffset;
	addressesOut.trainerIdsPointerAddr = GetEffectiveTrainerIdsPointerAddr(profile);
	if (addressesOut.trainerIdsPointerAddr != 0u) {
		const uint32_t trainerIdsBase = ReadArm9U32(addressesOut.trainerIdsPointerAddr);
		if (trainerIdsBase != 0u) {
			addressesOut.trainerIdsAddr = trainerIdsBase + GetTrainerIdsValueOffset(profile);
		}
	}
	if (layout->language != NULL && layout->language[0] != '\0') {
		addressesOut.language.assign(layout->language);
	}

	return true;
}

static const char *LookupProfileSpeciesName(const ExportProfileDefinition *profile, int speciesId)
{
	const GameTextTables *tables = GetProfileTextTables(profile);
	return (tables != NULL) ? LookupOneBasedName(tables->speciesNames, tables->speciesNamesCount, speciesId) : "";
}

static const char *LookupProfileItemName(const ExportProfileDefinition *profile, int itemId)
{
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		return "";
	}
	if (tables->getItemNameOverride != NULL) {
		const char *overrideName = tables->getItemNameOverride(static_cast<uint16_t>(itemId));
		if (overrideName != NULL && overrideName[0] != '\0') {
			return overrideName;
		}
	}
	return LookupOneBasedName(tables->itemNames, tables->itemNamesCount, itemId);
}

static const char *LookupProfileAbilityName(const ExportProfileDefinition *profile, int abilityId)
{
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		return "";
	}
	if (tables->getAbilityNameOverride != NULL) {
		const char *overrideName = tables->getAbilityNameOverride(static_cast<uint16_t>(abilityId));
		if (overrideName != NULL && overrideName[0] != '\0') {
			return overrideName;
		}
	}
	return LookupOneBasedName(tables->abilityNames, tables->abilityNamesCount, abilityId);
}

static const char *LookupProfileMoveName(const ExportProfileDefinition *profile, int moveId)
{
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		return "";
	}
	if (tables->getMoveNameOverride != NULL) {
		const char *overrideName = tables->getMoveNameOverride(static_cast<uint16_t>(moveId));
		if (overrideName != NULL && overrideName[0] != '\0') {
			return overrideName;
		}
	}
	return LookupOneBasedName(tables->moveNames, tables->moveNamesCount, moveId);
}

static const char *LookupProfileNatureName(const ExportProfileDefinition *profile, int natureId)
{
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL || natureId < 0 || static_cast<size_t>(natureId) >= tables->natureNamesCount) {
		return "Hardy";
	}
	return tables->natureNames[natureId];
}

static const char *GetProfileLocationName(const ExportProfileDefinition *profile, uint16_t locationId)
{
	const GameTextTables *tables = GetProfileTextTables(profile);
	return (tables != NULL && tables->getLocationName != NULL) ? tables->getLocationName(locationId) : "";
}

static const char *GetProfileFormSuffix(const ExportProfileDefinition *profile, uint16_t speciesId, uint8_t formIndex)
{
	const GameTextTables *tables = GetProfileTextTables(profile);
	return (tables != NULL && tables->getFormSuffix != NULL) ? tables->getFormSuffix(speciesId, formIndex) : "";
}

static int GetProfileLevelForExperience(const ExportProfileDefinition *profile, uint32_t experience, int speciesId)
{
	const PokemonMechanics *mechanics = GetProfileMechanics(profile);
	return (mechanics != NULL && mechanics->getLevelForExperience != NULL) ? mechanics->getLevelForExperience(experience, speciesId) : 0;
}

static uint32_t GetProfileExperienceForLevel(const ExportProfileDefinition *profile, int level, int speciesId)
{
	const PokemonMechanics *mechanics = GetProfileMechanics(profile);
	return (mechanics != NULL && mechanics->getExperienceForLevel != NULL) ? mechanics->getExperienceForLevel(level, speciesId) : 0u;
}

static uint32_t GetProfileMaxLevel100Experience(const ExportProfileDefinition *profile)
{
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		return 0u;
	}

	uint32_t maxExperience = 0u;
	for (size_t speciesIndex = 1u; speciesIndex <= tables->speciesNamesCount; ++speciesIndex) {
		const uint32_t speciesMaxExperience = GetProfileExperienceForLevel(profile, 100, static_cast<int>(speciesIndex));
		if (speciesMaxExperience > maxExperience) {
			maxExperience = speciesMaxExperience;
		}
	}

	return maxExperience;
}

static uint8_t GetProfileMoveBasePp(const ExportProfileDefinition *profile, uint16_t moveId)
{
	const PokemonMechanics *mechanics = GetProfileMechanics(profile);
	return (mechanics != NULL && mechanics->getMoveBasePp != NULL) ? mechanics->getMoveBasePp(moveId) : 0u;
}

static bool LookupProfileMoveIdByName(const ExportProfileDefinition *profile, const std::string &name, uint16_t &moveId, std::string &canonicalName)
{
	const std::string normalized = NormalizeMoveName(name);
	if (normalized.empty()) {
		moveId = 0u;
		canonicalName.clear();
		return true;
	}

	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables != NULL) {
		for (size_t i = 0; i < tables->moveNamesCount; ++i) {
			const char *candidate = LookupProfileMoveName(profile, static_cast<int>(i + 1));
			if (candidate == NULL || candidate[0] == '\0') {
				continue;
			}
			if (NormalizeMoveName(candidate) == normalized) {
				moveId = static_cast<uint16_t>(i + 1);
				canonicalName.assign(candidate);
				return true;
			}
		}
	}

	const PokemonMechanics *mechanics = GetProfileMechanics(profile);
	if (mechanics == NULL || mechanics->lookupMoveIdByName == NULL) {
		moveId = 0u;
		canonicalName.clear();
		return false;
	}
	return mechanics->lookupMoveIdByName(name, moveId, canonicalName);
}

static BattleDetectionStatus MakeEmptyBattleDetectionStatus()
{
	BattleDetectionStatus status;
	status.battleActive = false;
	status.trainerBattleActive = false;
	status.trainerBattleCanStartSession = false;
	status.trainerBattleSessionActive = false;
	status.battleSystemAddr = 0;
	status.battleContextAddr = 0;
	status.battleType = 0;
	status.enemyTrainerId = 0;
	status.maxBattlers = 0;
	status.lastPollUnixMs = 0;
	status.lastStateChangeUnixMs = 0;
	status.lastSessionStartUnixMs = 0;
	status.lastBattleEndUnixMs = 0;
	status.detectionSource.clear();
	status.detectionSourceDepth = -1;
	status.detectionConfidence = 0;
	status.lastLifecycleEvent.clear();
	return status;
}

static BattleLogStatus MakeEmptyBattleLogStatus()
{
	BattleLogStatus status;
	status.active = false;
	status.sessionCount = 0;
	status.activeSessionEventCount = 0;
	status.activeTrainerId = 0;
	status.currentTurn = 0;
	status.lastSessionStartUnixMs = 0;
	status.lastSessionEndUnixMs = 0;
	status.lastEventType.clear();
	status.lastEventTurn = 0;
	status.lastEventPlayerSlot = 0;
	status.lastEventAiSpecies = 0;
	status.outputDir = "dumps";
	status.attemptMapPath = "dumps/attempt_map.json";
	status.lastPersistedMasterPath.clear();
	status.lastPersistSucceeded = false;
	status.lastPersistError.clear();
	return status;
}

static HgssDiscoveryStatus MakeEmptyHgssDiscoveryStatus()
{
	HgssDiscoveryStatus status;
	status.active = false;
	status.trainerIdsResolved = false;
	status.trainerIdsSource.clear();
	status.pidAddr = 0u;
	status.trainerId = 0u;
	status.secretId = 0u;
	status.trainerIdsError.clear();
	status.battleAnchorAddr = 0u;
	status.battleAnchorValue = 0u;
	status.battleCandidateValid = false;
	status.battleProbeSource.clear();
	status.battleSystemCandidateAddr = 0u;
	status.battleContextCandidateAddr = 0u;
	status.battleType = 0u;
	status.enemyTrainerId = 0u;
	status.maxBattlers = 0;
	status.command = 0u;
	status.commandNext = 0u;
	status.battleConfidence = 0;
	status.battleProbeError.clear();
	status.sampledMons.clear();
	return status;
}

static uint32_t GetBattleDetectPollIntervalMs(BattleDetectPollPreset preset)
{
	switch (preset) {
		case BATTLE_DETECT_POLL_FAST:
			return 100u;
		case BATTLE_DETECT_POLL_SLOW:
			return 500u;
		case BATTLE_DETECT_POLL_NORMAL:
		default:
			return 250u;
	}
}

static const char *GetBattleDetectPollPresetName(BattleDetectPollPreset preset)
{
	switch (preset) {
		case BATTLE_DETECT_POLL_FAST:
			return "Fast";
		case BATTLE_DETECT_POLL_SLOW:
			return "Slow";
		case BATTLE_DETECT_POLL_NORMAL:
		default:
			return "Normal";
	}
}

static int GetPlatinumTimeOverrideHour(PlatinumTimeOverride mode)
{
	switch (mode) {
		case PLATINUM_TIME_MORNING:
			return 8;
		case PLATINUM_TIME_DAY:
			return 13;
		case PLATINUM_TIME_EVENING:
			return 18;
		case PLATINUM_TIME_NIGHT:
			return 22;
		case PLATINUM_TIME_LATE_NIGHT:
			return 2;
		case PLATINUM_TIME_AUTO:
		default:
			return -1;
	}
}

static const char *GetPlatinumTimeOverrideName(PlatinumTimeOverride mode)
{
	switch (mode) {
		case PLATINUM_TIME_MORNING:
			return "Morning";
		case PLATINUM_TIME_DAY:
			return "Day";
		case PLATINUM_TIME_EVENING:
			return "Evening";
		case PLATINUM_TIME_NIGHT:
			return "Night";
		case PLATINUM_TIME_LATE_NIGHT:
			return "Late Night";
		case PLATINUM_TIME_AUTO:
		default:
			return "Auto";
	}
}

static std::string FormatHex(uint32_t value)
{
	char buffer[16] = {0};
	std::snprintf(buffer, sizeof(buffer), "0x%08X", value);
	return std::string(buffer);
}

static void AppendJsonEscaped(std::string &out, const std::string &value)
{
	out.push_back('"');
	for (size_t i = 0; i < value.size(); ++i) {
		const unsigned char ch = static_cast<unsigned char>(value[i]);
		switch (ch) {
			case '\\': out += "\\\\"; break;
			case '"': out += "\\\""; break;
			case '\b': out += "\\b"; break;
			case '\f': out += "\\f"; break;
			case '\n': out += "\\n"; break;
			case '\r': out += "\\r"; break;
			case '\t': out += "\\t"; break;
			default:
				if (ch < 0x20) {
					char buffer[8] = {0};
					std::snprintf(buffer, sizeof(buffer), "\\u%04X", static_cast<unsigned int>(ch));
					out += buffer;
				} else {
					out.push_back(static_cast<char>(ch));
				}
				break;
		}
	}
	out.push_back('"');
}

static bool IsBattleTrackingAllowedForGameCode(ExportGameProfile profile, const std::string &gameCode)
{
	switch (profile) {
		case EXPORT_GAME_POKEMON_PLATINUM:
		case EXPORT_GAME_POKEMON_PLATINUM_KAIZO:
		case EXPORT_GAME_POKEMON_RENEGADE_PLATINUM:
			return (gameCode == "CPUE") || (gameCode == "JAK7");
		case EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER:
		case EXPORT_GAME_POKEMON_HEART_GOLD_ENGINE:
		case EXPORT_GAME_POKEMON_STERLING_SILVER:
			return (gameCode == "IPKE") || (gameCode == "IPGE");
		case EXPORT_GAME_POKEMON_BLACK_WHITE: {
			PokemonRuntimeAddresses addresses;
			std::string error;
			return TryResolveBwRuntimeAddresses(gameCode, addresses, error);
		}
		case EXPORT_GAME_POKEMON_BLACK2_WHITE2: {
			PokemonRuntimeAddresses addresses;
			std::string error;
			return TryResolveBw2RuntimeAddresses(gameCode, addresses, error);
		}
		case EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX: {
			PokemonRuntimeAddresses addresses;
			std::string error;
			return TryResolveBw2ReduxRuntimeAddresses(gameCode, addresses, error);
		}
		default:
			return false;
	}
}

static bool IsHgssLikeProfile(ExportGameProfile profile)
{
	return (profile == EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER)
		|| (profile == EXPORT_GAME_POKEMON_HEART_GOLD_ENGINE)
		|| (profile == EXPORT_GAME_POKEMON_STERLING_SILVER);
}

static std::string GetCurrentWorkingDirectory()
{
	char buffer[4096] = {0};
#ifdef _WIN32
	if (_getcwd(buffer, sizeof(buffer)) == NULL) {
		return "";
	}
#else
	if (getcwd(buffer, sizeof(buffer)) == NULL) {
		return "";
	}
#endif
	return std::string(buffer);
}

static std::string BuildAbsolutePath(const std::string &relativePath)
{
	if (relativePath.empty()) {
		return "";
	}
	if (!relativePath.empty() && ((relativePath[0] == '/') || (relativePath[0] == '\\'))) {
		return relativePath;
	}
	if (relativePath.size() > 1 && relativePath[1] == ':') {
		return relativePath;
	}

	const std::string cwd = GetCurrentWorkingDirectory();
	if (cwd.empty()) {
		return relativePath;
	}
	return cwd + "/" + relativePath;
}

static std::string TrimTrailingDirectorySeparators(std::string path)
{
	while (!path.empty() && (path[path.size() - 1] == '/' || path[path.size() - 1] == '\\')) {
		path.erase(path.size() - 1);
	}
	return path;
}

static std::string GetOutputDumpsDir()
{
	const std::string configuredPath = GetBattleLogOutputPath();
	if (!configuredPath.empty()) {
		return configuredPath;
	}

	return TrimTrailingDirectorySeparators("dumps");
}

static std::string BuildAttemptMapPath()
{
	return GetOutputDumpsDir() + "/attempt_map.json";
}

static std::string BuildAttemptDir(uint32_t attempt)
{
	return GetOutputDumpsDir() + "/attempt_" + std::to_string(attempt);
}

static std::string BuildAttemptMasterPath(uint32_t attempt)
{
	return BuildAttemptDir(attempt) + "/attempt_" + std::to_string(attempt) + ".json";
}

static bool EnsureDirectoryExists(const std::string &path)
{
	return EnsureDirectoryExistsForWrite(path);
}

static bool ReadTextFileAll(const std::string &path, std::string &textOut)
{
	textOut.clear();
	FILE *file = std::fopen(path.c_str(), "rb");
	if (file == NULL) {
		return false;
	}

	if (std::fseek(file, 0, SEEK_END) != 0) {
		std::fclose(file);
		return false;
	}

	const long fileSize = std::ftell(file);
	if (fileSize < 0) {
		std::fclose(file);
		return false;
	}

	if (std::fseek(file, 0, SEEK_SET) != 0) {
		std::fclose(file);
		return false;
	}

	textOut.resize(static_cast<size_t>(fileSize));
	if (fileSize > 0) {
		const size_t bytesRead = std::fread(&textOut[0], 1, static_cast<size_t>(fileSize), file);
		if (bytesRead != static_cast<size_t>(fileSize)) {
			std::fclose(file);
			textOut.clear();
			return false;
		}
	}

	std::fclose(file);
	return true;
}

static bool WriteTextFileAtomically(const std::string &path, const std::string &text)
{
	return WriteTextFileAtomicallyForWrite(path, text);
}

static size_t SkipJsonWhitespace(const std::string &text, size_t pos)
{
	while ((pos < text.size()) && std::isspace(static_cast<unsigned char>(text[pos]))) {
		++pos;
	}
	return pos;
}

static size_t FindJsonStringEnd(const std::string &text, size_t quotePos)
{
	if ((quotePos >= text.size()) || (text[quotePos] != '"')) {
		return std::string::npos;
	}

	for (size_t pos = quotePos + 1; pos < text.size(); ++pos) {
		if (text[pos] == '\\') {
			++pos;
			continue;
		}
		if (text[pos] == '"') {
			return pos + 1;
		}
	}

	return std::string::npos;
}

static size_t FindJsonValueEnd(const std::string &text, size_t startPos)
{
	if (startPos >= text.size()) {
		return std::string::npos;
	}

	const char first = text[startPos];
	if (first == '"') {
		return FindJsonStringEnd(text, startPos);
	}

	if ((first == '{') || (first == '[')) {
		int depth = 0;
		bool inString = false;
		for (size_t pos = startPos; pos < text.size(); ++pos) {
			const char ch = text[pos];
			if (inString) {
				if (ch == '\\') {
					++pos;
				} else if (ch == '"') {
					inString = false;
				}
				continue;
			}

			if (ch == '"') {
				inString = true;
				continue;
			}

			if ((ch == '{') || (ch == '[')) {
				++depth;
			} else if ((ch == '}') || (ch == ']')) {
				--depth;
				if (depth == 0) {
					return pos + 1;
				}
			}
		}
		return std::string::npos;
	}

	size_t pos = startPos;
	while (pos < text.size()) {
		const char ch = text[pos];
		if ((ch == ',') || (ch == '}') || (ch == ']') || std::isspace(static_cast<unsigned char>(ch))) {
			break;
		}
		++pos;
	}
	return pos;
}

static bool ExtractObjectFieldRaw(const std::string &objectJson, const std::string &fieldName, std::string &valueOut)
{
	valueOut.clear();
	size_t pos = SkipJsonWhitespace(objectJson, 0);
	if ((pos >= objectJson.size()) || (objectJson[pos] != '{')) {
		return false;
	}

	++pos;
	while (pos < objectJson.size()) {
		pos = SkipJsonWhitespace(objectJson, pos);
		if (pos >= objectJson.size()) {
			return false;
		}
		if (objectJson[pos] == '}') {
			return false;
		}
		if (objectJson[pos] != '"') {
			return false;
		}

		const size_t keyEnd = FindJsonStringEnd(objectJson, pos);
		if (keyEnd == std::string::npos) {
			return false;
		}

		const std::string key = objectJson.substr(pos + 1, keyEnd - pos - 2);
		pos = SkipJsonWhitespace(objectJson, keyEnd);
		if ((pos >= objectJson.size()) || (objectJson[pos] != ':')) {
			return false;
		}
		++pos;
		pos = SkipJsonWhitespace(objectJson, pos);
		const size_t valueStart = pos;
		const size_t valueEnd = FindJsonValueEnd(objectJson, valueStart);
		if (valueEnd == std::string::npos) {
			return false;
		}

		if (key == fieldName) {
			valueOut = objectJson.substr(valueStart, valueEnd - valueStart);
			return true;
		}

		pos = SkipJsonWhitespace(objectJson, valueEnd);
		if ((pos < objectJson.size()) && (objectJson[pos] == ',')) {
			++pos;
		}
	}

	return false;
}

static bool ParseRawJsonUnsigned(const std::string &raw, uint32_t &valueOut)
{
	const size_t start = SkipJsonWhitespace(raw, 0);
	if (start >= raw.size()) {
		return false;
	}

	char *endPtr = NULL;
	const unsigned long value = std::strtoul(raw.c_str() + start, &endPtr, 10);
	if ((endPtr == NULL) || (endPtr == (raw.c_str() + start))) {
		return false;
	}

	valueOut = static_cast<uint32_t>(value);
	return true;
}

static bool ExtractRawJsonString(const std::string &raw, std::string &valueOut)
{
	valueOut.clear();
	if (raw.size() < 2 || raw[0] != '"' || raw[raw.size() - 1] != '"') {
		return false;
	}

	for (size_t i = 1; i + 1 < raw.size(); ++i) {
		const char ch = raw[i];
		if (ch == '\\' && (i + 1 < raw.size() - 1)) {
			++i;
			switch (raw[i]) {
				case '\\': valueOut.push_back('\\'); break;
				case '"': valueOut.push_back('"'); break;
				case '/': valueOut.push_back('/'); break;
				case 'b': valueOut.push_back('\b'); break;
				case 'f': valueOut.push_back('\f'); break;
				case 'n': valueOut.push_back('\n'); break;
				case 'r': valueOut.push_back('\r'); break;
				case 't': valueOut.push_back('\t'); break;
				default: valueOut.push_back(raw[i]); break;
			}
		} else {
			valueOut.push_back(ch);
		}
	}

	return true;
}

static void SplitJsonArrayItems(const std::string &arrayJson, std::vector<std::string> &itemsOut)
{
	itemsOut.clear();
	size_t pos = SkipJsonWhitespace(arrayJson, 0);
	if ((pos >= arrayJson.size()) || (arrayJson[pos] != '[')) {
		return;
	}

	++pos;
	while (pos < arrayJson.size()) {
		pos = SkipJsonWhitespace(arrayJson, pos);
		if ((pos >= arrayJson.size()) || (arrayJson[pos] == ']')) {
			return;
		}

		const size_t valueStart = pos;
		const size_t valueEnd = FindJsonValueEnd(arrayJson, valueStart);
		if (valueEnd == std::string::npos) {
			return;
		}

		itemsOut.push_back(arrayJson.substr(valueStart, valueEnd - valueStart));
		pos = SkipJsonWhitespace(arrayJson, valueEnd);
		if ((pos < arrayJson.size()) && (arrayJson[pos] == ',')) {
			++pos;
		}
	}
}

static void TrimIncompleteAttemptBattleEvents(std::vector<std::string> &eventItems)
{
	std::vector<std::string> trimmedItems;
	size_t currentSessionStart = static_cast<size_t>(-1);

	for (size_t i = 0; i < eventItems.size(); ++i) {
		std::string typeRaw;
		std::string type;
		if (ExtractObjectFieldRaw(eventItems[i], "type", typeRaw)) {
			ExtractRawJsonString(typeRaw, type);
		}

		if (type == "session_start") {
			if (currentSessionStart != static_cast<size_t>(-1)) {
				trimmedItems.resize(currentSessionStart);
			}
			currentSessionStart = trimmedItems.size();
			trimmedItems.push_back(eventItems[i]);
		} else if (type == "session_end") {
			trimmedItems.push_back(eventItems[i]);
			currentSessionStart = static_cast<size_t>(-1);
		} else {
			trimmedItems.push_back(eventItems[i]);
		}
	}

	if (currentSessionStart != static_cast<size_t>(-1)) {
		trimmedItems.resize(currentSessionStart);
	}

	eventItems.swap(trimmedItems);
}

static AttemptMapData LoadAttemptMap()
{
	AttemptMapData mapData;
	mapData.nextAttempt = 1;

	std::string rawText;
	if (!ReadTextFileAll(BuildAttemptMapPath(), rawText) || rawText.empty()) {
		return mapData;
	}

	std::string nextAttemptRaw;
	uint32_t nextAttempt = 0;
	if (ExtractObjectFieldRaw(rawText, "nextAttempt", nextAttemptRaw) && ParseRawJsonUnsigned(nextAttemptRaw, nextAttempt) && (nextAttempt >= 1u)) {
		mapData.nextAttempt = nextAttempt;
	}

	std::string mapRaw;
	if (!ExtractObjectFieldRaw(rawText, "map", mapRaw)) {
		return mapData;
	}

	size_t pos = SkipJsonWhitespace(mapRaw, 0);
	if ((pos >= mapRaw.size()) || (mapRaw[pos] != '{')) {
		return mapData;
	}

	++pos;
	while (pos < mapRaw.size()) {
		pos = SkipJsonWhitespace(mapRaw, pos);
		if ((pos >= mapRaw.size()) || (mapRaw[pos] == '}')) {
			break;
		}
		if (mapRaw[pos] != '"') {
			break;
		}

		const size_t keyEnd = FindJsonStringEnd(mapRaw, pos);
		if (keyEnd == std::string::npos) {
			break;
		}
		const std::string trainerKey = mapRaw.substr(pos + 1, keyEnd - pos - 2);
		pos = SkipJsonWhitespace(mapRaw, keyEnd);
		if ((pos >= mapRaw.size()) || (mapRaw[pos] != ':')) {
			break;
		}
		++pos;
		pos = SkipJsonWhitespace(mapRaw, pos);
		const size_t valueEnd = FindJsonValueEnd(mapRaw, pos);
		if (valueEnd == std::string::npos) {
			break;
		}

		uint32_t attempt = 0;
		if (ParseRawJsonUnsigned(mapRaw.substr(pos, valueEnd - pos), attempt) && (attempt >= 1u)) {
			mapData.map[trainerKey] = attempt;
			if (attempt >= mapData.nextAttempt) {
				mapData.nextAttempt = attempt + 1u;
			}
		}

		pos = SkipJsonWhitespace(mapRaw, valueEnd);
		if ((pos < mapRaw.size()) && (mapRaw[pos] == ',')) {
			++pos;
		}
	}

	return mapData;
}

static bool SaveAttemptMap(const AttemptMapData &mapData)
{
	if (!EnsureDirectoryExists(GetOutputDumpsDir())) {
		return false;
	}

	std::string json = "{";
	json += "\"version\":1";
	json += ",\"nextAttempt\":";
	json += std::to_string((mapData.nextAttempt >= 1u) ? mapData.nextAttempt : 1u);
	json += ",\"map\":{";

	bool first = true;
	for (std::map<std::string, uint32_t>::const_iterator it = mapData.map.begin(); it != mapData.map.end(); ++it) {
		if (!first) {
			json += ",";
		}
		first = false;
		AppendJsonEscaped(json, it->first);
		json += ":";
		json += std::to_string((it->second >= 1u) ? it->second : 1u);
	}

	json += "}}\n";
	return WriteTextFileAtomically(BuildAttemptMapPath(), json);
}

static bool EnsureBattleLogStorageInitialized(std::string &errorOut)
{
	errorOut.clear();
	if (!EnsureDirectoryExists(GetOutputDumpsDir())) {
		errorOut = "Failed to create dumps directory";
		return false;
	}

	std::string rawText;
	if (ReadTextFileAll(BuildAttemptMapPath(), rawText) && !rawText.empty()) {
		return true;
	}

	AttemptMapData mapData;
	mapData.nextAttempt = 1u;
	if (!SaveAttemptMap(mapData)) {
		errorOut = "Failed to initialize attempt_map.json";
		return false;
	}

	return true;
}

static uint32_t GetOrCreateAttemptForTrainerKey(const std::string &trainerKey, AttemptMapData &mapData)
{
	std::map<std::string, uint32_t>::const_iterator existing = mapData.map.find(trainerKey);
	if (existing != mapData.map.end() && existing->second >= 1u) {
		return existing->second;
	}

	const uint32_t attempt = (mapData.nextAttempt >= 1u) ? mapData.nextAttempt : 1u;
	mapData.map[trainerKey] = attempt;
	mapData.nextAttempt = attempt + 1u;
	return attempt;
}

static uint16_t GetBattleLogPartySpeciesForSlot(const BattleLogSession &session, uint8_t pSlot)
{
	for (size_t i = 0; i < session.pParty.size(); ++i) {
		if (session.pParty[i].slot == static_cast<uint8_t>(pSlot + 1u)) {
			return session.pParty[i].species;
		}
	}
	return 0u;
}

static bool HasBattleLogPartyPid(const BattleLogSession &session, uint32_t pid)
{
	if (pid == 0u) {
		return false;
	}

	for (size_t i = 0; i < session.pParty.size(); ++i) {
		if (session.pParty[i].pid == pid) {
			return true;
		}
	}

	return false;
}

static std::string SerializeBattleLogPartyEntryJson(const BattleLogPartyEntry &entry)
{
	std::string json = "{";
	json += "\"slot\":";
	json += std::to_string(entry.slot);
	json += ",\"species\":";
	json += std::to_string(entry.species);
	json += ",\"heldItem\":";
	json += std::to_string(entry.heldItem);
	json += ",\"natureId\":";
	json += std::to_string(entry.natureId);
	json += ",\"abilitySlot\":";
	json += std::to_string(entry.abilitySlot);
	json += ",\"ability\":";
	json += std::to_string(entry.ability);
	json += ",\"moves\":[";
	for (size_t i = 0; i < 4; ++i) {
		if (i != 0) {
			json += ",";
		}
		json += std::to_string(entry.moves[i]);
	}
	json += "]}";
	return json;
}

static std::string SerializeBattleLogEventJson(const BattleLogSession &session, const BattleLogEvent &event)
{
	std::string json = "{";
	json += "\"type\":";
	AppendJsonEscaped(json, event.type);

	if (event.type == "session_start") {
		json += ",\"enemyTrainerIdA\":";
		json += std::to_string(session.enemyTrainerIdA);
		json += ",\"enemyTrainerIdB\":";
		if (session.enemyTrainerIdB != 0u) {
			json += std::to_string(session.enemyTrainerIdB);
		} else {
			json += "null";
		}
		json += ",\"pPartyHighestLevel\":";
		json += std::to_string(session.pPartyHighestLevel);
		json += ",\"badgeCount\":";
		if (session.badgeInfoResolved) {
			json += std::to_string(session.badgeCount);
		} else {
			json += "null";
		}
		json += ",\"badgeFlags\":";
		if (session.badgeInfoResolved) {
			json += std::to_string(session.badgeFlags);
		} else {
			json += "null";
		}
		json += ",\"pParty\":[";
		for (size_t i = 0; i < session.pParty.size(); ++i) {
			if (i != 0) {
				json += ",";
			}
			json += SerializeBattleLogPartyEntryJson(session.pParty[i]);
		}
		json += "]";
	} else if (event.type == "session_end") {
	} else {
		json += ",\"turn\":";
		json += std::to_string(event.turn);
		json += ",\"pSlot\":";
		json += std::to_string(event.pSlot);
		const uint16_t pSpecies = GetBattleLogPartySpeciesForSlot(session, event.pSlot);
		if (pSpecies != 0u) {
			json += ",\"pSpecies\":";
			json += std::to_string(pSpecies);
		}
		if ((event.type == "pKo") && (event.aiSpecies != 0u)) {
			json += ",\"aiSpecies\":";
			json += std::to_string(event.aiSpecies);
		}
	}

	json += "}";
	return json;
}

static std::string BuildDefaultAttemptBoxJson()
{
	return "{\"updatedAt\":null,\"partyCount\":null,\"boxSlotsDumped\":null,\"payload\":null}";
}

static bool PersistCompletedBattleLogSession(const BattleLogSession &session, std::string &masterPathOut, std::string &errorOut)
{
	masterPathOut.clear();
	errorOut.clear();
	if (session.events.empty()) {
		errorOut = "No battle log events to persist";
		return false;
	}

	const std::string trainerKey = std::to_string(session.playerTrainerId) + ":" + std::to_string(session.playerSecretId);
	if (!EnsureDirectoryExists(GetOutputDumpsDir())) {
		errorOut = "Failed to create dumps directory";
		return false;
	}

	AttemptMapData mapData = LoadAttemptMap();
	const uint32_t attempt = GetOrCreateAttemptForTrainerKey(trainerKey, mapData);
	if (!SaveAttemptMap(mapData)) {
		errorOut = "Failed to write attempt_map.json";
		return false;
	}

	const std::string attemptDir = BuildAttemptDir(attempt);
	if (!EnsureDirectoryExists(attemptDir)) {
		errorOut = "Failed to create attempt directory";
		return false;
	}

	const std::string masterPath = BuildAttemptMasterPath(attempt);
	masterPathOut = masterPath;
	std::vector<std::string> persistedEventItems;
	std::string preservedBoxJson = BuildDefaultAttemptBoxJson();

	std::string existingMasterJson;
	if (ReadTextFileAll(masterPath, existingMasterJson) && !existingMasterJson.empty()) {
		std::string battlelogRaw;
		if (ExtractObjectFieldRaw(existingMasterJson, "battlelog", battlelogRaw)) {
			std::string eventsRaw;
			if (ExtractObjectFieldRaw(battlelogRaw, "events", eventsRaw)) {
				SplitJsonArrayItems(eventsRaw, persistedEventItems);
				TrimIncompleteAttemptBattleEvents(persistedEventItems);
			}
		}

		std::string boxRaw;
		if (ExtractObjectFieldRaw(existingMasterJson, "box", boxRaw) && !boxRaw.empty()) {
			preservedBoxJson = boxRaw;
		}
	}

	for (size_t i = 0; i < session.events.size(); ++i) {
		persistedEventItems.push_back(SerializeBattleLogEventJson(session, session.events[i]));
	}

	std::string json = "{";
	json += "\"version\":";
	AppendJsonEscaped(json, "platinum-qol");
	json += ",\"attempt\":";
	json += std::to_string(attempt);
	json += ",\"trainerId\":";
	json += std::to_string(session.playerTrainerId);
	json += ",\"secretId\":";
	json += std::to_string(session.playerSecretId);
	json += ",\"trainerKey\":";
	AppendJsonEscaped(json, trainerKey);
	json += ",\"battlelog\":{\"events\":[";
	for (size_t i = 0; i < persistedEventItems.size(); ++i) {
		if (i != 0) {
			json += ",";
		}
		json += persistedEventItems[i];
	}
	json += "],\"eventCount\":";
	json += std::to_string(persistedEventItems.size());
	json += "},\"box\":";
	json += preservedBoxJson;
	json += "}\n";

	if (!WriteTextFileAtomically(masterPath, json)) {
		errorOut = "Failed to write attempt master JSON";
		return false;
	}

	return true;
}

static std::string SerializeBattleLogSessionsJson(const std::vector<BattleLogSession> &sessions)
{
	std::string json = "{\"events\":[";
	bool first = true;
	for (size_t sessionIndex = 0; sessionIndex < sessions.size(); ++sessionIndex) {
		const BattleLogSession &session = sessions[sessionIndex];
		for (size_t eventIndex = 0; eventIndex < session.events.size(); ++eventIndex) {
			if (!first) {
				json += ",";
			}
			first = false;
			json += SerializeBattleLogEventJson(session, session.events[eventIndex]);
		}
	}
	json += "]}";
	return json;
}

static bool TryResolveCurrentTrainerIds(ExportGameProfile selectedProfile, uint16_t &trainerIdOut, uint16_t &secretIdOut, std::string &errorOut)
{
	trainerIdOut = 0u;
	secretIdOut = 0u;
	errorOut.clear();

	const ExportProfileDefinition *profile = GetExportProfileDefinition(selectedProfile);
	if (profile == NULL) {
		errorOut = "No Pokemon HTTP export game selected";
		return false;
	}

	if (gameInfo.romsize == 0) {
		errorOut = "No ROM loaded";
		return false;
	}

	if (!IsProfileCompatibleWithGameCode(selectedProfile, GameCodeString())) {
		errorOut = "Selected profile does not match loaded game code";
		return false;
	}

	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = std::string(profile->displayName) + " memory layout is unavailable";
		return false;
	}

	PokemonRuntimeAddresses addresses;
	if (!ResolvePokemonRuntimeAddresses(profile, addresses, errorOut)) {
		return false;
	}

	if (addresses.trainerIdsAddr != 0u) {
		const uint32_t trainerIDs = ReadArm9U32(addresses.trainerIdsAddr);
		trainerIdOut = static_cast<uint16_t>(trainerIDs & 0xFFFFu);
		secretIdOut = static_cast<uint16_t>((trainerIDs >> 16) & 0xFFFFu);
		return true;
	}

	if (IsHgssLikeProfile(selectedProfile)) {
		HgssDiscoveryStatus probeStatus;
		if (TryResolveHgssTrainerIds(profile, probeStatus, trainerIdOut, secretIdOut)) {
			return true;
		}
		errorOut = !probeStatus.trainerIdsError.empty()
			? probeStatus.trainerIdsError
			: (std::string(profile->displayName) + " trainer ID probe failed");
		return false;
	}

	errorOut = std::string(profile->displayName) + " trainer ID root is not configured";
	return false;
}

static bool TryGetExistingAttemptForTrainerKey(const std::string &trainerKey, const AttemptMapData &mapData, uint32_t &attemptOut)
{
	attemptOut = 0u;
	const std::map<std::string, uint32_t>::const_iterator it = mapData.map.find(trainerKey);
	if (it == mapData.map.end() || it->second < 1u) {
		return false;
	}

	attemptOut = it->second;
	return true;
}

static bool LoadAttemptBattleLogEventItems(const std::string &masterPath, std::vector<std::string> &eventItemsOut, std::string &errorOut)
{
	eventItemsOut.clear();
	errorOut.clear();

	std::string masterJson;
	if (!ReadTextFileAll(masterPath, masterJson) || masterJson.empty()) {
		errorOut = "Attempt master JSON not found";
		return false;
	}

	std::string battlelogRaw;
	if (!ExtractObjectFieldRaw(masterJson, "battlelog", battlelogRaw)) {
		errorOut = "Attempt master JSON missing battlelog";
		return false;
	}

	std::string eventsRaw;
	if (!ExtractObjectFieldRaw(battlelogRaw, "events", eventsRaw)) {
		errorOut = "Attempt master JSON missing battlelog.events";
		return false;
	}

	SplitJsonArrayItems(eventsRaw, eventItemsOut);
	TrimIncompleteAttemptBattleEvents(eventItemsOut);
	return true;
}

static std::string BuildEmptyPackedBattleLogPayload()
{
	std::string payload;
	payload.reserve(5);
	payload += "NBL1";
	payload.push_back(static_cast<char>(3));
	return payload;
}

static void WritePackedBattleLogBits(std::vector<uint8_t> &bytes, size_t &bitPos, uint32_t value, size_t width)
{
	for (size_t bit = 0; bit < width; ++bit) {
		const size_t absoluteBit = bitPos + bit;
		const size_t byteIndex = absoluteBit >> 3;
		const size_t bitIndex = absoluteBit & 7u;
		while (byteIndex >= bytes.size()) {
			bytes.push_back(0u);
		}
		if (((value >> bit) & 1u) != 0u) {
			bytes[byteIndex] = static_cast<uint8_t>(bytes[byteIndex] | static_cast<uint8_t>(1u << bitIndex));
		}
	}
	bitPos += width;
}

static uint32_t ParseBattleLogJsonUnsignedOrDefault(const std::string &raw, uint32_t defaultValue)
{
	uint32_t value = 0u;
	return ParseRawJsonUnsigned(raw, value) ? value : defaultValue;
}

static std::string BuildPackedBattleLogPayloadFromEventItems(const std::vector<std::string> &eventItems)
{
	std::vector<uint8_t> bytes;
	bytes.reserve(64);
	bytes.push_back(static_cast<uint8_t>('N'));
	bytes.push_back(static_cast<uint8_t>('B'));
	bytes.push_back(static_cast<uint8_t>('L'));
	bytes.push_back(static_cast<uint8_t>('1'));
	bytes.push_back(3u);
	size_t bitPos = 40u;
	std::vector<uint16_t> currentPartySpecies;

	for (size_t i = 0; i < eventItems.size(); ++i) {
		std::string typeRaw;
		std::string type;
		if (!ExtractObjectFieldRaw(eventItems[i], "type", typeRaw) || !ExtractRawJsonString(typeRaw, type)) {
			continue;
		}

		if (type == "session_start") {
			std::string enemyTrainerIdARaw;
			std::string enemyTrainerIdBRaw;
			std::string highestLevelRaw;
			std::string badgeCountRaw;
			std::string badgeFlagsRaw;
			std::string partyRaw;
			const uint32_t enemyTrainerIdA = ExtractObjectFieldRaw(eventItems[i], "enemyTrainerIdA", enemyTrainerIdARaw)
				? ParseBattleLogJsonUnsignedOrDefault(enemyTrainerIdARaw, 0xFFFFu)
				: 0xFFFFu;
			const uint32_t enemyTrainerIdB = ExtractObjectFieldRaw(eventItems[i], "enemyTrainerIdB", enemyTrainerIdBRaw)
				? ParseBattleLogJsonUnsignedOrDefault(enemyTrainerIdBRaw, 0xFFFFu)
				: 0xFFFFu;
			const uint32_t highestLevel = ExtractObjectFieldRaw(eventItems[i], "pPartyHighestLevel", highestLevelRaw)
				? std::min<uint32_t>(ParseBattleLogJsonUnsignedOrDefault(highestLevelRaw, 0u), 127u)
				: 0u;
			const uint32_t badgeCount = ExtractObjectFieldRaw(eventItems[i], "badgeCount", badgeCountRaw)
				? std::min<uint32_t>(ParseBattleLogJsonUnsignedOrDefault(badgeCountRaw, 0u), 31u)
				: 0u;
			const uint32_t badgeFlags = ExtractObjectFieldRaw(eventItems[i], "badgeFlags", badgeFlagsRaw)
				? std::min<uint32_t>(ParseBattleLogJsonUnsignedOrDefault(badgeFlagsRaw, 0u), 0xFFFFu)
				: 0u;

			std::vector<std::string> partyItems;
			if (ExtractObjectFieldRaw(eventItems[i], "pParty", partyRaw)) {
				SplitJsonArrayItems(partyRaw, partyItems);
			}

			if (partyItems.size() > 6u) {
				partyItems.resize(6u);
			}

			WritePackedBattleLogBits(bytes, bitPos, 0u, 2u);
			WritePackedBattleLogBits(bytes, bitPos, std::min<uint32_t>(enemyTrainerIdA, 0xFFFFu), 16u);
			WritePackedBattleLogBits(bytes, bitPos, std::min<uint32_t>(enemyTrainerIdB, 0xFFFFu), 16u);
			WritePackedBattleLogBits(bytes, bitPos, static_cast<uint32_t>(partyItems.size()), 3u);
			WritePackedBattleLogBits(bytes, bitPos, highestLevel, 7u);
			WritePackedBattleLogBits(bytes, bitPos, badgeCount, 5u);
			WritePackedBattleLogBits(bytes, bitPos, badgeFlags, 16u);

			currentPartySpecies.clear();
			for (size_t partyIndex = 0; partyIndex < partyItems.size(); ++partyIndex) {
				std::string speciesRaw;
				std::string heldItemRaw;
				std::string natureIdRaw;
				std::string abilitySlotRaw;
				std::string movesRaw;
				const uint32_t species = ExtractObjectFieldRaw(partyItems[partyIndex], "species", speciesRaw)
					? (ParseBattleLogJsonUnsignedOrDefault(speciesRaw, 0u) & 0x7FFu)
					: 0u;
				const uint32_t heldItem = ExtractObjectFieldRaw(partyItems[partyIndex], "heldItem", heldItemRaw)
					? (ParseBattleLogJsonUnsignedOrDefault(heldItemRaw, 0u) & 0x3FFu)
					: 0u;
				const uint32_t natureId = ExtractObjectFieldRaw(partyItems[partyIndex], "natureId", natureIdRaw)
					? (ParseBattleLogJsonUnsignedOrDefault(natureIdRaw, 0u) & 0x1Fu)
					: 0u;
				const uint32_t abilitySlot = ExtractObjectFieldRaw(partyItems[partyIndex], "abilitySlot", abilitySlotRaw)
					? (ParseBattleLogJsonUnsignedOrDefault(abilitySlotRaw, 0u) & 0x3u)
					: 0u;

				uint32_t moveValues[4] = {0u, 0u, 0u, 0u};
				std::vector<std::string> moveItems;
				if (ExtractObjectFieldRaw(partyItems[partyIndex], "moves", movesRaw)) {
					SplitJsonArrayItems(movesRaw, moveItems);
				}
				for (size_t moveIndex = 0; moveIndex < moveItems.size() && moveIndex < 4u; ++moveIndex) {
					moveValues[moveIndex] = ParseBattleLogJsonUnsignedOrDefault(moveItems[moveIndex], 0u) & 0x3FFu;
				}

				WritePackedBattleLogBits(bytes, bitPos, species, 11u);
				WritePackedBattleLogBits(bytes, bitPos, heldItem, 10u);
				WritePackedBattleLogBits(bytes, bitPos, natureId, 5u);
				WritePackedBattleLogBits(bytes, bitPos, abilitySlot, 2u);
				for (size_t moveIndex = 0; moveIndex < 4u; ++moveIndex) {
					WritePackedBattleLogBits(bytes, bitPos, moveValues[moveIndex], 10u);
				}

				currentPartySpecies.push_back(static_cast<uint16_t>(species));
			}
		} else if (type == "session_end") {
			WritePackedBattleLogBits(bytes, bitPos, 1u, 2u);
		} else if (type == "pKo") {
			std::string turnRaw;
			std::string pSlotRaw;
			std::string aiSpeciesRaw;
			const uint32_t turn = ExtractObjectFieldRaw(eventItems[i], "turn", turnRaw)
				? (ParseBattleLogJsonUnsignedOrDefault(turnRaw, 0u) & 0xFFFFu)
				: 0u;
			const uint32_t pSlot = ExtractObjectFieldRaw(eventItems[i], "pSlot", pSlotRaw)
				? (ParseBattleLogJsonUnsignedOrDefault(pSlotRaw, 7u) & 0x7u)
				: 7u;
			const uint32_t aiSpecies = ExtractObjectFieldRaw(eventItems[i], "aiSpecies", aiSpeciesRaw)
				? (ParseBattleLogJsonUnsignedOrDefault(aiSpeciesRaw, 0u) & 0x7FFu)
				: 0u;

			WritePackedBattleLogBits(bytes, bitPos, 2u, 2u);
			WritePackedBattleLogBits(bytes, bitPos, turn, 16u);
			WritePackedBattleLogBits(bytes, bitPos, pSlot, 3u);
			WritePackedBattleLogBits(bytes, bitPos, aiSpecies, 11u);
		} else if (type == "aiKo") {
			std::string turnRaw;
			std::string pSlotRaw;
			const uint32_t turn = ExtractObjectFieldRaw(eventItems[i], "turn", turnRaw)
				? (ParseBattleLogJsonUnsignedOrDefault(turnRaw, 0u) & 0xFFFFu)
				: 0u;
			const uint32_t pSlot = ExtractObjectFieldRaw(eventItems[i], "pSlot", pSlotRaw)
				? (ParseBattleLogJsonUnsignedOrDefault(pSlotRaw, 7u) & 0x7u)
				: 7u;

			WritePackedBattleLogBits(bytes, bitPos, 3u, 2u);
			WritePackedBattleLogBits(bytes, bitPos, turn, 16u);
			WritePackedBattleLogBits(bytes, bitPos, pSlot, 3u);
		}
	}

	return std::string(reinterpret_cast<const char *>(&bytes[0]), bytes.size());
}

static const char *LookupOneBasedName(const char *const *names, size_t count, int id)
{
	if (id <= 0) {
		return "";
	}
	const size_t index = static_cast<size_t>(id - 1);
	return (index < count) ? names[index] : "";
}

static std::string GameCodeString()
{
	if (gameInfo.romsize == 0) {
		return "";
	}
	return std::string(gameInfo.header.gameCode, gameInfo.header.gameCode + 4);
}

static bool HasConfiguredPidRoot(const PokemonMemoryLayout *layout)
{
	return layout != NULL && layout->pidPointerAddr != 0u;
}

static bool HasConfiguredTrainerIdsRoot(const PokemonMemoryLayout *layout)
{
	return layout != NULL && layout->trainerIDsPointerAddr != 0u;
}

static bool HasConfiguredBoxRoot(const PokemonMemoryLayout *layout)
{
	return layout != NULL && layout->boxDataOffset != 0u && layout->currentBoxIndexOffset != 0u;
}

static uint32_t GetBoxStrideBytes(const PokemonMemoryLayout *layout)
{
	if (layout == NULL) {
		return 0u;
	}

	const size_t fallbackStride = layout->partySlotsPerBox * layout->boxMonSize;
	const size_t configuredStride = (layout->boxStrideBytes != 0u) ? layout->boxStrideBytes : fallbackStride;
	return static_cast<uint32_t>(configuredStride);
}

static uint32_t GetBoxMonAddress(const PokemonMemoryLayout *layout, uint32_t boxBaseAddr, uint32_t boxIndex, uint32_t slotIndex)
{
	if (layout == NULL) {
		return 0u;
	}

	return boxBaseAddr
		+ (boxIndex * GetBoxStrideBytes(layout))
		+ (slotIndex * static_cast<uint32_t>(layout->boxMonSize));
}

static uint32_t GetLiveExportBoxCount(const PokemonMemoryLayout *layout)
{
	if (layout == NULL || layout->exportBoxCount == 0u) {
		return 0u;
	}

	if (layout == &kGen4PokemonLayout) {
		return std::min<uint32_t>(static_cast<uint32_t>(kGen4LiveExportBoxCount), static_cast<uint32_t>(layout->exportBoxCount));
	}

	if (layout == &kGen5BwPokemonLayout) {
		return std::min<uint32_t>(static_cast<uint32_t>(kBwLiveExportBoxCount), static_cast<uint32_t>(layout->exportBoxCount));
	}

	return static_cast<uint32_t>(layout->exportBoxCount - 1u);
}

static uint32_t GetDeadExportStartBoxIndex(const PokemonMemoryLayout *layout)
{
	return GetLiveExportBoxCount(layout);
}

static uint32_t GetDeadExportBoxCount(const PokemonMemoryLayout *layout)
{
	if (layout == NULL) {
		return 0u;
	}

	const uint32_t liveBoxCount = GetLiveExportBoxCount(layout);
	const uint32_t totalBoxCount = static_cast<uint32_t>(layout->exportBoxCount);
	if (liveBoxCount >= totalBoxCount) {
		return 0u;
	}

	return totalBoxCount - liveBoxCount;
}

static bool IsDeadExportBox(const PokemonMemoryLayout *layout, uint32_t boxIndex)
{
	if (layout == NULL) {
		return false;
	}

	return boxIndex >= GetDeadExportStartBoxIndex(layout) &&
	       boxIndex < static_cast<uint32_t>(layout->exportBoxCount);
}

static uint32_t GetEndpointCurrentBoxIndex(const PokemonMemoryLayout *layout, uint32_t currentBoxIndex)
{
	const uint32_t endpointBoxCount = GetLiveExportBoxCount(layout);
	if (endpointBoxCount == 0u) {
		return 0u;
	}

	return std::min(currentBoxIndex, endpointBoxCount - 1u);
}

static uint8_t ReadCurrentBoxIndexForProfile(const PokemonMemoryLayout *layout, const PokemonRuntimeAddresses &addresses)
{
	const uint32_t liveBoxCount = GetLiveExportBoxCount(layout);
	uint8_t primary = 0u;
	if (addresses.currentBoxIndexAddr != 0u) {
		primary = ReadArm9U8(addresses.currentBoxIndexAddr);
		if ((liveBoxCount == 0u) || (primary < liveBoxCount)) {
			return primary;
		}
	}

	if (addresses.currentBoxIndexFallbackAddr != 0u) {
		const uint8_t fallback = ReadArm9U8(addresses.currentBoxIndexFallbackAddr);
		if ((liveBoxCount == 0u) || (fallback < liveBoxCount)) {
			return fallback;
		}
	}

	return static_cast<uint8_t>(GetEndpointCurrentBoxIndex(layout, primary));
}

static const ExportProfileDefinition *GetExportProfileDefinition(ExportGameProfile profile)
{
	switch (profile) {
		case EXPORT_GAME_POKEMON_PLATINUM: {
			static const ExportProfileDefinition profileDefinition = {
				EXPORT_GAME_POKEMON_PLATINUM,
				"Pokemon Platinum",
				"Pokemon Platinum",
				&kGen4TextTables,
				&kGen4Mechanics,
				&kGen4PokemonLayout,
				&kGen4BattleLayout
			};
			return &profileDefinition;
		}
		case EXPORT_GAME_POKEMON_PLATINUM_KAIZO: {
			static const ExportProfileDefinition profileDefinition = {
				EXPORT_GAME_POKEMON_PLATINUM_KAIZO,
				"Platinum Kaizo",
				"Platinum Kaizo",
				&kPlatinumKaizoTextTables,
				&kPlatinumKaizoMechanics,
				&kGen4PokemonLayout,
				&kGen4BattleLayout
			};
			return &profileDefinition;
		}
		case EXPORT_GAME_POKEMON_RENEGADE_PLATINUM: {
			static const ExportProfileDefinition profileDefinition = {
				EXPORT_GAME_POKEMON_RENEGADE_PLATINUM,
				"Renegade Platinum",
				"Renegade Platinum",
				&kRenegadePlatinumTextTables,
				&kGen4Mechanics,
				&kGen4PokemonLayout,
				&kGen4BattleLayout
			};
			return &profileDefinition;
		}
		case EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER: {
			static const ExportProfileDefinition profileDefinition = {
				EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER,
				"Pokemon HeartGold/SoulSilver",
				"Pokemon HeartGold/SoulSilver",
				&kHgssTextTables,
				&kGen4Mechanics,
				&kHgssPokemonLayout,
				&kHgssBattleLayout
			};
			return &profileDefinition;
		}
		case EXPORT_GAME_POKEMON_STERLING_SILVER: {
			static const ExportProfileDefinition profileDefinition = {
				EXPORT_GAME_POKEMON_STERLING_SILVER,
				"Sterling Silver",
				"Sterling Silver",
				&kSterlingSilverTextTables,
				&kSterlingSilverMechanics,
				&kHgssPokemonLayout,
				&kHgssBattleLayout
			};
			return &profileDefinition;
		}
		case EXPORT_GAME_POKEMON_HEART_GOLD_ENGINE: {
			static const ExportProfileDefinition profileDefinition = {
				EXPORT_GAME_POKEMON_HEART_GOLD_ENGINE,
				"Heart Gold Engine",
				"Heart Gold Engine",
				&kHgssTextTables,
				&kGen4Mechanics,
				&kHgssPokemonLayout,
				&kHgssBattleLayout
			};
			return &profileDefinition;
		}
		case EXPORT_GAME_POKEMON_BLACK_WHITE: {
			static const ExportProfileDefinition profileDefinition = {
				EXPORT_GAME_POKEMON_BLACK_WHITE,
				"Pokemon Black/White",
				"Pokemon Black/White",
				&kGen5BwTextTables,
				&kGen5Mechanics,
				&kGen5BwPokemonLayout,
				&kBwBattleLayout
			};
			return &profileDefinition;
		}
		case EXPORT_GAME_POKEMON_BLACK2_WHITE2: {
			static const ExportProfileDefinition profileDefinition = {
				EXPORT_GAME_POKEMON_BLACK2_WHITE2,
				"Pokemon Black 2/White 2",
				"Pokemon Black 2/White 2",
				&kGen5Bw2TextTables,
				&kGen5Mechanics,
				&kGen5BwPokemonLayout,
				&kBwBattleLayout
			};
			return &profileDefinition;
		}
		case EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX: {
			static const ExportProfileDefinition profileDefinition = {
				EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX,
				"Blaze Black 2 Redux/Volt White 2 Redux",
				"Blaze Black 2 Redux/Volt White 2 Redux",
				&kBb2Vw2ReduxTextTables,
				&kBb2Vw2ReduxMechanics,
				&kGen5BwPokemonLayout,
				&kBwBattleLayout
			};
			return &profileDefinition;
		}
		case EXPORT_GAME_NONE:
		default:
			return NULL;
	}
}

static uint8_t ReadArm9U8(uint32_t address)
{
	return _MMU_read08<ARMCPU_ARM9>(address);
}

static uint16_t ReadArm9U16(uint32_t address)
{
	return _MMU_read16<ARMCPU_ARM9>(address);
}

static uint32_t ReadArm9U32(uint32_t address)
{
	return _MMU_read32<ARMCPU_ARM9>(address);
}

static bool SnapshotArm9Bytes(uint32_t address, size_t size, std::vector<uint8_t> &bytesOut)
{
	bytesOut.resize(size);
	for (size_t byteIndex = 0; byteIndex < size; ++byteIndex) {
		bytesOut[byteIndex] = ReadArm9U8(address + static_cast<uint32_t>(byteIndex));
	}

	return true;
}

static uint16_t ReadSnapshotU16(const std::vector<uint8_t> &bytes, size_t offset)
{
	if ((offset + 1u) >= bytes.size()) {
		return 0u;
	}

	return static_cast<uint16_t>(bytes[offset]) |
	       (static_cast<uint16_t>(bytes[offset + 1u]) << 8);
}

static uint32_t ReadSnapshotU32(const std::vector<uint8_t> &bytes, size_t offset)
{
	return static_cast<uint32_t>(ReadSnapshotU16(bytes, offset)) |
	       (static_cast<uint32_t>(ReadSnapshotU16(bytes, offset + 2u)) << 16);
}

static bool IsAllZeroBytes(const std::vector<uint8_t> &bytes)
{
	for (size_t i = 0; i < bytes.size(); ++i) {
		if (bytes[i] != 0u) {
			return false;
		}
	}
	return true;
}

static bool IsBoxPokemonSlotEmpty(const PokemonMemoryLayout *layout, uint32_t monOffset)
{
	if (layout == NULL || layout->boxMonSize == 0u) {
		return true;
	}

	std::vector<uint8_t> monBytes;
	SnapshotArm9Bytes(monOffset, layout->boxMonSize, monBytes);
	if (IsAllZeroBytes(monBytes)) {
		return true;
	}

	if ((layout->codec == POKEMON_DATA_CODEC_GEN4 || layout->codec == POKEMON_DATA_CODEC_GEN5) &&
		layout->coreBytes >= kCoreBytes &&
		monBytes.size() >= (0x8u + layout->coreBytes)) {
		const uint32_t pid = ReadSnapshotU32(monBytes, 0x0u);
		const uint16_t checksum = ReadSnapshotU16(monBytes, 0x6u);
		const size_t decodedWordCount = (0x8u + layout->coreBytes) / 2u;
		std::vector<uint16_t> words(decodedWordCount, 0);
		for (uint32_t byteOffset = 0u; byteOffset <= 0x6u; byteOffset += 2u) {
			words[byteOffset / 2u] = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
		}

		uint32_t seed = checksum;
		uint32_t decodedChecksum = 0u;
		for (uint32_t byteOffset = 0x8u; byteOffset < (0x8u + layout->coreBytes); byteOffset += 2u) {
			seed = AdvanceLcrng(seed);
			const uint16_t decodedWord = static_cast<uint16_t>(ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset)) ^ (seed >> 16));
			words[byteOffset / 2u] = decodedWord;
			decodedChecksum = (decodedChecksum + decodedWord) & 0xFFFFu;
		}

		const int shuffleKey = static_cast<int>(((pid & 0x3E000u) >> 13) % 24u);
		const int blockA = kShuffleTable[shuffleKey][0];
		const size_t speciesWordIndex = static_cast<size_t>(blockA / 2);
		if (decodedChecksum == checksum && speciesWordIndex < words.size() && words[speciesWordIndex] == 0u) {
			return true;
		}
	}

	return false;
}

static void WriteArm9U8(uint32_t address, uint8_t value)
{
	_MMU_write08<ARMCPU_ARM9>(address, value);
}

static void WriteArm9U16(uint32_t address, uint16_t value)
{
	_MMU_write16<ARMCPU_ARM9>(address, value);
}

static void WriteArm9U32(uint32_t address, uint32_t value)
{
	_MMU_write32<ARMCPU_ARM9>(address, value);
}

static bool IsMainRamAlignedPointer(uint32_t address)
{
	return (address >= 0x02000000u) && (address < 0x02400000u) && ((address & 0x3u) == 0u);
}

static bool IsMainRamAddress(uint32_t address)
{
	return (address >= 0x02000000u) && (address < 0x02400000u);
}

static PlayerBadgeStatus MakeEmptyPlayerBadgeStatus()
{
	PlayerBadgeStatus status;
	status.available = false;
	status.pidAddr = 0u;
	status.profileAddr = 0u;
	status.badgeCount = 0u;
	status.badgeFlags = 0u;
	return status;
}

static uint8_t Popcount8(uint8_t value)
{
	uint8_t count = 0u;
	for (; value != 0u; value >>= 1) {
		count = static_cast<uint8_t>(count + (value & 0x1u));
	}
	return count;
}

static uint8_t Popcount16(uint16_t value)
{
	uint8_t count = 0u;
	for (; value != 0u; value >>= 1) {
		count = static_cast<uint8_t>(count + (value & 0x1u));
	}
	return count;
}

static bool TryResolvePlatinumBadgeStatusFromPid(uint32_t pidAddr, PlayerBadgeStatus &statusOut, std::string &errorOut)
{
	statusOut = MakeEmptyPlayerBadgeStatus();
	errorOut.clear();

	static const uint32_t kPlatinumTrainerInfoOffsetFromPid = 0xD05Cu;
	static const uint32_t kPlatinumBadgeMaskOffsetFromPid = 0xD076u;

	if (!IsMainRamAlignedPointer(pidAddr)) {
		errorOut = "Platinum pid pointer is invalid";
		statusOut.lastError = errorOut;
		return false;
	}

	const uint32_t profileAddr = pidAddr + kPlatinumTrainerInfoOffsetFromPid;
	const uint32_t badgeAddr = pidAddr + kPlatinumBadgeMaskOffsetFromPid;
	if (!IsMainRamAlignedPointer(profileAddr) || !IsMainRamAddress(badgeAddr)) {
		errorOut = "Platinum badge profile address is invalid";
		statusOut.lastError = errorOut;
		return false;
	}

	const uint8_t badgeMask = ReadArm9U8(badgeAddr);
	statusOut.available = true;
	statusOut.source = "live_save_ram";
	statusOut.pidAddr = pidAddr;
	statusOut.profileAddr = profileAddr;
	statusOut.badgeCount = Popcount8(badgeMask);
	statusOut.badgeFlags = badgeMask;
	statusOut.lastError.clear();
	return true;
}

static bool TryResolveHgssBadgeStatusFromPid(uint32_t pidAddr, PlayerBadgeStatus &statusOut, std::string &errorOut)
{
	statusOut = MakeEmptyPlayerBadgeStatus();
	errorOut.clear();

	static const uint32_t kHgssProfileOffsetFromPid = 0xD054u;
	static const uint32_t kHgssJohtoBadgesOffsetFromPid = 0xD06Eu;
	static const uint32_t kHgssKantoBadgesOffsetFromPid = 0xD073u;

	if (!IsMainRamAlignedPointer(pidAddr)) {
		errorOut = "HGSS pid pointer is invalid";
		statusOut.lastError = errorOut;
		return false;
	}

	const uint32_t profileAddr = pidAddr + kHgssProfileOffsetFromPid;
	const uint32_t johtoBadgesAddr = pidAddr + kHgssJohtoBadgesOffsetFromPid;
	const uint32_t kantoBadgesAddr = pidAddr + kHgssKantoBadgesOffsetFromPid;
	if (!IsMainRamAlignedPointer(profileAddr) || !IsMainRamAddress(johtoBadgesAddr) || !IsMainRamAddress(kantoBadgesAddr)) {
		errorOut = "HGSS badge profile address is invalid";
		statusOut.lastError = errorOut;
		return false;
	}

	const uint8_t johtoBadges = ReadArm9U8(johtoBadgesAddr);
	const uint8_t kantoBadges = ReadArm9U8(kantoBadgesAddr);
	const uint16_t badgeFlags = static_cast<uint16_t>(johtoBadges | (static_cast<uint16_t>(kantoBadges) << 8));
	statusOut.available = true;
	statusOut.source = "live_save_ram";
	statusOut.pidAddr = pidAddr;
	statusOut.profileAddr = profileAddr;
	statusOut.badgeCount = Popcount16(badgeFlags);
	statusOut.badgeFlags = badgeFlags;
	statusOut.lastError.clear();
	return true;
}

static bool TryResolveCurrentBadgeStatus(ExportGameProfile selectedProfile, PlayerBadgeStatus &statusOut, std::string &errorOut)
{
	statusOut = MakeEmptyPlayerBadgeStatus();
	errorOut.clear();

	const ExportProfileDefinition *profile = GetExportProfileDefinition(selectedProfile);
	if (profile == NULL) {
		errorOut = "No Pokemon HTTP export game selected";
		statusOut.lastError = errorOut;
		return false;
	}

	if (gameInfo.romsize == 0) {
		errorOut = "No ROM loaded";
		statusOut.lastError = errorOut;
		return false;
	}

	if (!IsProfileCompatibleWithGameCode(selectedProfile, GameCodeString())) {
		errorOut = "Selected profile does not match loaded game code";
		statusOut.lastError = errorOut;
		return false;
	}

	PokemonRuntimeAddresses addresses;
	if (!ResolvePokemonRuntimeAddresses(profile, addresses, errorOut)) {
		statusOut.lastError = errorOut;
		return false;
	}

	if (IsPlatinumLikeProfile(selectedProfile)) {
		return TryResolvePlatinumBadgeStatusFromPid(addresses.pidAddr, statusOut, errorOut);
	}

	if (IsHgssLikeProfile(selectedProfile)) {
		return TryResolveHgssBadgeStatusFromPid(addresses.pidAddr, statusOut, errorOut);
	}

	errorOut = std::string(profile->displayName) + " badge export is not supported";
	statusOut.lastError = errorOut;
	return false;
}

static bool IsBattleEndingCommand(uint32_t command)
{
	return (command == 43u) || (command == 41u) || (command == 42u);
}

static bool IsTrainerBattleType(uint32_t battleType)
{
	return (battleType & 0x1u) != 0u;
}

static bool IsSaneBattleContextState(const BattleMemoryLayout &layout, uint32_t battleSysAddr, uint32_t battleCtxAddr)
{
	if (!IsMainRamAlignedPointer(battleCtxAddr)) {
		return false;
	}

	if (IsMainRamAlignedPointer(battleSysAddr) && (ReadArm9U32(battleSysAddr + layout.battleSystemBattleCtxOffset) != battleCtxAddr)) {
		return false;
	}

	const uint32_t command = ReadArm9U32(battleCtxAddr + layout.battleContextCommandOffset);
	const uint32_t commandNext = ReadArm9U32(battleCtxAddr + layout.battleContextCommandNextOffset);
	const uint32_t battleType = IsMainRamAlignedPointer(battleSysAddr) ? ReadArm9U32(battleSysAddr + layout.battleSystemBattleTypeOffset) : 0u;

	if ((command > 128u) || (commandNext > 128u)) {
		return false;
	}

	if ((battleType & layout.battleTypeMask) != battleType) {
		return false;
	}

	return true;
}

static bool TryReadPartyCandidateSummary(uint32_t partyPtr, int &capacityOut, int &currentCountOut)
{
	if (!IsMainRamAlignedPointer(partyPtr)) {
		return false;
	}

	const uint32_t capacity = ReadArm9U32(partyPtr);
	const uint32_t currentCount = ReadArm9U32(partyPtr + 0x4u);
	if ((capacity < 1u) || (capacity > 6u) || (currentCount > 6u)) {
		return false;
	}

	capacityOut = static_cast<int>(capacity);
	currentCountOut = static_cast<int>(currentCount);
	return true;
}

static void AddBwBattleMainCandidate(uint32_t candidate, uint32_t *candidates, size_t &candidateCount, size_t candidateCapacity)
{
	if (!IsMainRamAlignedPointer(candidate) || (candidateCount >= candidateCapacity)) {
		return;
	}

	for (size_t i = 0; i < candidateCount; ++i) {
		if (candidates[i] == candidate) {
			return;
		}
	}

	candidates[candidateCount++] = candidate;
}

static bool TryBuildBwBattleDetectionSnapshot(
	const PokemonRuntimeAddresses &runtimeAddresses,
	uint32_t lastKnownBattleSystemAddr,
	BattleDetectionSnapshot &outSnapshot)
{
	outSnapshot.valid = false;

	uint32_t candidates[4] = {};
	size_t candidateCount = 0;
	AddBwBattleMainCandidate(runtimeAddresses.bwBattleMainAddr, candidates, candidateCount, sizeof(candidates) / sizeof(candidates[0]));
	if (runtimeAddresses.currentBoxIndexAddr >= (kArm9MainRamStart + kBwBattleMainModuleOffsetFromCurrentBoxIndex)) {
		AddBwBattleMainCandidate(
			runtimeAddresses.currentBoxIndexAddr - kBwBattleMainModuleOffsetFromCurrentBoxIndex,
			candidates,
			candidateCount,
			sizeof(candidates) / sizeof(candidates[0]));
	}
	AddBwBattleMainCandidate(lastKnownBattleSystemAddr, candidates, candidateCount, sizeof(candidates) / sizeof(candidates[0]));

	for (size_t candidateIndex = 0; candidateIndex < candidateCount; ++candidateIndex) {
		const uint32_t baseCandidate = candidates[candidateIndex];
		const uint32_t startAddr = (baseCandidate > kBwBattleNearbyMainScanRadius)
			? (baseCandidate - kBwBattleNearbyMainScanRadius)
			: kArm9MainRamStart;
		const uint32_t endAddr = std::min(baseCandidate + kBwBattleNearbyMainScanRadius, kArm9MainRamEnd - 4u);
		for (uint32_t mainAddr = startAddr; mainAddr <= endAddr; mainAddr += 4u) {
			const uint32_t setupAddr = ReadArm9U32(mainAddr + kBwBattleMainSetupParamOffset);
			const uint32_t serverAddr = ReadArm9U32(mainAddr + kBwBattleMainServerOffset);
			const uint32_t clientPokeConAddr = mainAddr + kBwBattleMainPokeconForClientOffset;
			const uint32_t serverPokeConAddr = mainAddr + kBwBattleMainPokeconForServerOffset;
			if (!IsMainRamAlignedPointer(setupAddr) || !IsMainRamAlignedPointer(serverAddr)) {
				continue;
			}
			if (ReadArm9U32(clientPokeConAddr + kBwBattlePokeConMainModuleOffset) != mainAddr) {
				continue;
			}
			if (ReadArm9U32(serverPokeConAddr + kBwBattlePokeConMainModuleOffset) != mainAddr) {
				continue;
			}
			if (ReadArm9U32(serverAddr + kBwBattleServerMainModuleOffset) != mainAddr) {
				continue;
			}
			if (ReadArm9U32(serverAddr + kBwBattleServerPokeConOffset) != serverPokeConAddr) {
				continue;
			}

			const uint32_t flowWorkAddr = ReadArm9U32(serverAddr + kBwBattleServerFlowWorkOffset);
			if (!IsMainRamAlignedPointer(flowWorkAddr)) {
				continue;
			}
			if (ReadArm9U32(flowWorkAddr + kBwBattleFlowServerOffset) != serverAddr) {
				continue;
			}
			if (ReadArm9U32(flowWorkAddr + kBwBattleFlowMainModuleOffset) != mainAddr) {
				continue;
			}

			const uint32_t competitor = ReadArm9U32(setupAddr + kBwBattleSetupCompetitorOffset);
			const uint32_t rule = ReadArm9U32(setupAddr + kBwBattleSetupRuleOffset);
			if ((competitor >= 5u) || (rule > 3u)) {
				continue;
			}

			const uint32_t playerClientWorkAddr = serverAddr + kBwBattleServerClientWorkOffset;
			const uint32_t enemyClientWorkAddr = playerClientWorkAddr + kBwBattleServerClientWorkSize;
			const uint8_t playerMemberCount = ReadArm9U8(playerClientWorkAddr + 0x08u);
			const uint8_t enemyMemberCount = ReadArm9U8(enemyClientWorkAddr + 0x08u);
			const uint8_t playerNumCoverPos = ReadArm9U8(playerClientWorkAddr + 0x09u);
			const uint8_t enemyNumCoverPos = ReadArm9U8(enemyClientWorkAddr + 0x09u);
			if ((playerMemberCount < 1u) || (playerMemberCount > 6u) ||
				(enemyMemberCount < 1u) || (enemyMemberCount > 6u) ||
				(playerNumCoverPos < 1u) || (playerNumCoverPos > 3u) ||
				(enemyNumCoverPos < 1u) || (enemyNumCoverPos > 3u) ||
				(playerNumCoverPos > playerMemberCount) ||
				(enemyNumCoverPos > enemyMemberCount)) {
				continue;
			}

			const uint32_t playerPartyAddr = clientPokeConAddr + kBwBattlePokeConPartyOffset;
			const uint32_t enemyPartyAddr = clientPokeConAddr + kBwBattlePokeConPartyOffset + kBwBattlePartySize;
			const uint32_t firstPlayerBpp = ReadArm9U32(playerPartyAddr);
			const uint32_t firstEnemyBpp = ReadArm9U32(enemyPartyAddr);
			if (!IsMainRamAlignedPointer(firstPlayerBpp) || !IsMainRamAlignedPointer(firstEnemyBpp)) {
				continue;
			}

			const uint16_t firstPlayerSpecies = ReadArm9U16(firstPlayerBpp + kBwBattlePokeParamSpeciesOffset);
			const uint16_t firstEnemySpecies = ReadArm9U16(firstEnemyBpp + kBwBattlePokeParamSpeciesOffset);
			if ((firstPlayerSpecies == 0u) || (firstPlayerSpecies > 649u) ||
				(firstEnemySpecies == 0u) || (firstEnemySpecies > 649u)) {
				continue;
			}

			const bool trainerBattleActive = (competitor == 1u) || (competitor == 2u);
			uint16_t enemyTrainerId = 0u;
			if (trainerBattleActive) {
				const uint32_t enemyTrainerDataAddr = ReadArm9U32(setupAddr + kBwBattleSetupTrainerDataOffset + 4u);
				if (IsMainRamAlignedPointer(enemyTrainerDataAddr)) {
					enemyTrainerId = ReadArm9U16(enemyTrainerDataAddr);
				}
			}

			outSnapshot.valid = true;
			outSnapshot.battleActive = true;
			outSnapshot.trainerBattleActive = trainerBattleActive;
			outSnapshot.trainerBattleCanStartSession = trainerBattleActive;
			outSnapshot.battleSystemAddr = mainAddr;
			outSnapshot.battleContextAddr = flowWorkAddr;
			outSnapshot.battleType = (competitor << 8) | rule;
			outSnapshot.enemyTrainerId = enemyTrainerId;
			outSnapshot.maxBattlers = static_cast<int>(std::min<uint8_t>(playerNumCoverPos, enemyNumCoverPos) * 2u);
			outSnapshot.command = 0u;
			outSnapshot.commandNext = 0u;
			outSnapshot.detectionSource = (mainAddr == baseCandidate) ? "bw_main_module" : "bw_main_module_nearby";
			outSnapshot.detectionSourceDepth = (mainAddr == baseCandidate) ? 0 : 1;
			outSnapshot.detectionConfidence = 95;
			return true;
		}
	}

	return false;
}

static bool TryBuildBattleDetectionSnapshot(
	const BattleMemoryLayout &layout,
	uint32_t battleSysAddr,
	const char *sourceLabel,
	int sourceDepth,
	bool requireActiveBattle,
	bool requirePartyValidation,
	BattleDetectionSnapshot &outSnapshot)
{
	outSnapshot.valid = false;
	if (!IsMainRamAlignedPointer(battleSysAddr)) {
		return false;
	}

	const uint32_t battleType = ReadArm9U32(battleSysAddr + layout.battleSystemBattleTypeOffset);
	const uint32_t battleCtxAddr = ReadArm9U32(battleSysAddr + layout.battleSystemBattleCtxOffset);
	const uint32_t maxBattlers = ReadArm9U32(battleSysAddr + layout.battleSystemMaxBattlersOffset);
	if ((maxBattlers < 1u) || (maxBattlers > 4u)) {
		return false;
	}

	if (!IsSaneBattleContextState(layout, battleSysAddr, battleCtxAddr)) {
		return false;
	}

	if (requireActiveBattle && (battleType == 0u)) {
		return false;
	}

	int partyHits = 0;
	int partyScore = 0;
	for (int i = 0; i < 4; ++i) {
		const uint32_t partyPtr = ReadArm9U32(battleSysAddr + layout.battleSystemPartiesOffset + static_cast<uint32_t>(i * 4));
		int capacity = 0;
		int currentCount = 0;
		if (!TryReadPartyCandidateSummary(partyPtr, capacity, currentCount)) {
			continue;
		}
		++partyHits;
		partyScore += 2;
		if (currentCount > 0) {
			++partyScore;
		}
	}

	if (requirePartyValidation && (partyHits < 2)) {
		return false;
	}

	const uint32_t command = ReadArm9U32(battleCtxAddr + layout.battleContextCommandOffset);
	const uint32_t commandNext = ReadArm9U32(battleCtxAddr + layout.battleContextCommandNextOffset);
	const bool battleActive = (battleType != 0u);
	const bool trainerBattleActive = battleActive && IsTrainerBattleType(battleType);
	const uint16_t enemyTrainerId = trainerBattleActive ? ReadArm9U16(battleSysAddr + layout.battleSystemEnemyTrainerIdOffset) : 0u;

	outSnapshot.valid = true;
	outSnapshot.battleActive = battleActive;
	outSnapshot.trainerBattleActive = trainerBattleActive;
	outSnapshot.trainerBattleCanStartSession = trainerBattleActive && !(IsBattleEndingCommand(command) && IsBattleEndingCommand(commandNext));
	outSnapshot.battleSystemAddr = battleSysAddr;
	outSnapshot.battleContextAddr = battleCtxAddr;
	outSnapshot.battleType = battleType;
	outSnapshot.enemyTrainerId = enemyTrainerId;
	outSnapshot.maxBattlers = static_cast<int>(maxBattlers);
	outSnapshot.command = command;
	outSnapshot.commandNext = commandNext;
	outSnapshot.detectionSource = (sourceLabel != NULL) ? sourceLabel : "";
	outSnapshot.detectionSourceDepth = sourceDepth;
	outSnapshot.detectionConfidence = 60 + partyScore + (trainerBattleActive ? 20 : 0) + (battleActive ? 10 : 0) - (sourceDepth * 5);
	if (outSnapshot.detectionConfidence > 100) {
		outSnapshot.detectionConfidence = 100;
	}
	return true;
}

static bool ScanBattleSystemRange(
	const BattleMemoryLayout &layout,
	uint32_t startAddr,
	uint32_t endAddr,
	const char *sourceLabel,
	int sourceDepth,
	bool requireActiveBattle,
	bool requireBattleCtxDelta,
	uint32_t expectedBattleCtxDelta,
	BattleDetectionSnapshot &outSnapshot)
{
	BattleDetectionSnapshot bestSnapshot;
	bestSnapshot.valid = false;
	int bestScore = -1;

	const uint32_t clampedStart = std::max(startAddr, 0x02000000u);
	const uint32_t clampedEnd = std::min(endAddr, 0x02400000u);
	if ((clampedStart >= clampedEnd) || ((clampedStart & 0x3u) != 0u) || ((clampedEnd & 0x3u) != 0u)) {
		return false;
	}

	for (uint32_t addr = clampedStart; addr + 4u <= clampedEnd; addr += 4u) {
		if (requireBattleCtxDelta && (ReadArm9U32(addr + layout.battleSystemBattleCtxOffset) != (addr + expectedBattleCtxDelta))) {
			continue;
		}

		BattleDetectionSnapshot candidate;
		if (!TryBuildBattleDetectionSnapshot(layout, addr, sourceLabel, sourceDepth, requireActiveBattle, true, candidate)) {
			continue;
		}

		const int score = candidate.detectionConfidence;
		if (!bestSnapshot.valid || (score > bestScore) || ((score == bestScore) && (candidate.battleSystemAddr < bestSnapshot.battleSystemAddr))) {
			bestSnapshot = candidate;
			bestScore = score;
		}
	}

	if (!bestSnapshot.valid) {
		return false;
	}

	outSnapshot = bestSnapshot;
	return true;
}

static bool TryResolveHgssTrainerIds(
	const ExportProfileDefinition *profile,
	HgssDiscoveryStatus &probeStatus,
	uint16_t &trainerIdOut,
	uint16_t &secretIdOut)
{
	trainerIdOut = 0u;
	secretIdOut = 0u;
	probeStatus = MakeEmptyHgssDiscoveryStatus();
	probeStatus.active = true;

	if (profile == NULL) {
		probeStatus.trainerIdsError = "HGSS profile is unavailable";
		return false;
	}

	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		probeStatus.trainerIdsError = "HGSS memory layout is unavailable";
		return false;
	}

	const uint32_t pidPointerAddr = GetEffectivePidPointerAddr(profile);
	if (pidPointerAddr != 0u) {
		probeStatus.pidAddr = ReadArm9U32(pidPointerAddr);
	}

	const uint32_t trainerIdsPointerAddr = GetEffectiveTrainerIdsPointerAddr(profile);
	if (trainerIdsPointerAddr == 0u) {
		probeStatus.trainerIdsError = "HGSS trainer IDs pointer is not configured for this game code";
		return false;
	}

	probeStatus.trainerIdsSource = "hgss_rng_pointer";
	const uint32_t trainerIDsBase = ReadArm9U32(trainerIdsPointerAddr);
	if (trainerIDsBase == 0u) {
		probeStatus.trainerIdsError = "HGSS trainer IDs pointer is null";
		return false;
	}

	const uint32_t trainerIDs = ReadArm9U32(trainerIDsBase + GetTrainerIdsValueOffset(profile));
	trainerIdOut = static_cast<uint16_t>(trainerIDs & 0xFFFFu);
	secretIdOut = static_cast<uint16_t>((trainerIDs >> 16) & 0xFFFFu);
	probeStatus.trainerIdsResolved = true;
	probeStatus.trainerId = trainerIdOut;
	probeStatus.secretId = secretIdOut;
	probeStatus.trainerIdsError.clear();
	return true;
}

static void PopulateHgssBattleSamples(
	const BattleMemoryLayout &layout,
	uint32_t battleCtxAddr,
	HgssDiscoveryStatus &probeStatus)
{
	probeStatus.sampledMons.clear();
	if (!IsMainRamAlignedPointer(battleCtxAddr)) {
		return;
	}

	std::vector<BattleMonRuntimeSnapshot> runtimeMons;
	BuildBattleMonRuntimeSnapshot(layout, battleCtxAddr, runtimeMons);
	for (size_t i = 0; i < runtimeMons.size(); ++i) {
		HgssBattleMonSample sample;
		sample.valid = runtimeMons[i].valid;
		sample.battler = runtimeMons[i].battler;
		sample.partySlot = runtimeMons[i].partySlot;
		sample.speciesId = runtimeMons[i].speciesId;
		sample.curHp = runtimeMons[i].curHp;
		sample.maxHp = runtimeMons[i].maxHp;
		probeStatus.sampledMons.push_back(sample);
	}
}

static bool TryBuildHgssBattleSnapshotFromDerivedBase(
	const BattleMemoryLayout &battleLayout,
	uint32_t anchorValue,
	uint32_t baseAdjustment,
	const char *sourceLabel,
	int sourceDepth,
	bool previousBattleActive,
	BattleDetectionSnapshot &snapshotOut)
{
	if ((baseAdjustment != 0u) && (anchorValue < baseAdjustment)) {
		return false;
	}

	const uint32_t candidateBase = anchorValue - baseAdjustment;
	if (TryBuildBattleDetectionSnapshot(
			battleLayout,
			candidateBase,
			sourceLabel,
			sourceDepth,
			!previousBattleActive,
			false,
			snapshotOut)) {
		return true;
	}

	const uint32_t startAddr = (candidateBase > kHgssBattleDerivedBaseScanRadius)
		? (candidateBase - kHgssBattleDerivedBaseScanRadius)
		: 0x02000000u;
	return ScanBattleSystemRange(
		battleLayout,
		startAddr,
		candidateBase + kHgssBattleDerivedBaseScanRadius + 4u,
		sourceLabel,
		sourceDepth,
		!previousBattleActive,
		false,
		0u,
		snapshotOut);
}

static bool TryBuildHgssBattleDetectionSnapshot(
	const PokemonMemoryLayout &pokemonLayout,
	const BattleMemoryLayout &battleLayout,
	uint32_t lastKnownBattleSystemAddr,
	bool previousBattleActive,
	bool allowFullScan,
	BattleDetectionSnapshot &outSnapshot,
	HgssDiscoveryStatus &probeStatus)
{
	outSnapshot.valid = false;
	probeStatus.active = true;
	probeStatus.battleProbeSource.clear();
	probeStatus.battleProbeError.clear();
	probeStatus.battleCandidateValid = false;
	probeStatus.battleType = 0u;
	probeStatus.enemyTrainerId = 0u;
	probeStatus.maxBattlers = 0;
	probeStatus.command = 0u;
	probeStatus.commandNext = 0u;
	probeStatus.battleConfidence = 0;
	probeStatus.battleSystemCandidateAddr = 0u;
	probeStatus.battleContextCandidateAddr = 0u;

	const ExportProfileDefinition *hgssProfile = GetExportProfileDefinition(EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER);
	const uint32_t pidPointerAddr = GetEffectivePidPointerAddr(hgssProfile);
	const uint32_t pidAddr = ReadArm9U32((pidPointerAddr != 0u) ? pidPointerAddr : pokemonLayout.pidPointerAddr);
	probeStatus.pidAddr = pidAddr;
	if (!IsMainRamAlignedPointer(pidAddr)) {
		probeStatus.battleProbeError = "HGSS pid pointer is null or invalid";
		return false;
	}

	const uint32_t battleAnchorAddr = pidAddr + kHgssBattleAnchorPidOffset;
	probeStatus.battleAnchorAddr = battleAnchorAddr;
	if (IsMainRamAlignedPointer(battleAnchorAddr)) {
		probeStatus.battleAnchorValue = ReadArm9U32(battleAnchorAddr);
	}

	BattleDetectionSnapshot snapshot;
	snapshot.valid = false;
	bool foundSnapshot = false;

	if (IsMainRamAlignedPointer(probeStatus.battleAnchorValue)) {
		foundSnapshot = TryBuildHgssBattleSnapshotFromDerivedBase(
			battleLayout,
			probeStatus.battleAnchorValue,
			0u,
			"hgss_anchor_pointer",
			0,
			previousBattleActive,
			snapshot);
	}

	if (!foundSnapshot && IsMainRamAlignedPointer(probeStatus.battleAnchorValue)) {
		foundSnapshot = TryBuildHgssBattleSnapshotFromDerivedBase(
			battleLayout,
			probeStatus.battleAnchorValue,
			kHgssBattlePartyBankOffset,
			"hgss_anchor_minus_1c70",
			1,
			previousBattleActive,
			snapshot);
	}

	if (!foundSnapshot && IsMainRamAlignedPointer(probeStatus.battleAnchorValue)) {
		foundSnapshot = TryBuildHgssBattleSnapshotFromDerivedBase(
			battleLayout,
			probeStatus.battleAnchorValue,
			kHgssBattlePartyBankOffsetAltA,
			"hgss_anchor_minus_268c",
			2,
			previousBattleActive,
			snapshot);
	}

	if (!foundSnapshot && IsMainRamAlignedPointer(probeStatus.battleAnchorValue)) {
		foundSnapshot = TryBuildHgssBattleSnapshotFromDerivedBase(
			battleLayout,
			probeStatus.battleAnchorValue,
			kHgssBattlePartyBankOffsetAltB,
			"hgss_anchor_minus_30a8",
			2,
			previousBattleActive,
			snapshot);
	}

	if (!foundSnapshot && IsMainRamAlignedPointer(probeStatus.battleAnchorValue)) {
		const uint32_t startAddr = (probeStatus.battleAnchorValue > kHgssBattleNearbyRadius)
			? (probeStatus.battleAnchorValue - kHgssBattleNearbyRadius)
			: 0x02000000u;
		foundSnapshot = ScanBattleSystemRange(
			battleLayout,
			startAddr,
			probeStatus.battleAnchorValue + kHgssBattleNearbyRadius + 4u,
			"hgss_anchor_cluster",
			1,
			true,
			false,
			0u,
			snapshot);
	}

	if (!foundSnapshot && IsMainRamAlignedPointer(lastKnownBattleSystemAddr)) {
		const uint32_t startAddr = (lastKnownBattleSystemAddr > battleLayout.nearbyLastKnownRadius)
			? (lastKnownBattleSystemAddr - battleLayout.nearbyLastKnownRadius)
			: 0x02000000u;
		foundSnapshot = ScanBattleSystemRange(
			battleLayout,
			startAddr,
			lastKnownBattleSystemAddr + battleLayout.nearbyLastKnownRadius + 4u,
			"hgss_last_known",
			2,
			!previousBattleActive,
			false,
			0u,
			snapshot);
	}

	if (!foundSnapshot && allowFullScan) {
		foundSnapshot = ScanBattleSystemRange(
			battleLayout,
			battleLayout.fullBattleScanStart,
			battleLayout.fullBattleScanEnd,
			"hgss_full_ram",
			3,
			true,
			false,
			0u,
			snapshot);
	}

	if (!foundSnapshot) {
		probeStatus.battleProbeError = "No valid HGSS BattleSystem candidate found";
		return false;
	}

	outSnapshot = snapshot;
	probeStatus.battleCandidateValid = true;
	probeStatus.battleProbeSource = snapshot.detectionSource;
	probeStatus.battleSystemCandidateAddr = snapshot.battleSystemAddr;
	probeStatus.battleContextCandidateAddr = snapshot.battleContextAddr;
	probeStatus.battleType = snapshot.battleType;
	probeStatus.enemyTrainerId = snapshot.enemyTrainerId;
	probeStatus.maxBattlers = snapshot.maxBattlers;
	probeStatus.command = snapshot.command;
	probeStatus.commandNext = snapshot.commandNext;
	probeStatus.battleConfidence = snapshot.detectionConfidence;
	PopulateHgssBattleSamples(battleLayout, snapshot.battleContextAddr, probeStatus);
	return true;
}

static uint32_t AdvanceLcrng(uint32_t seed)
{
	return (seed * 0x41C64E6Du) + 0x6073u;
}

static uint16_t GetWord(const DecryptedPokemon &pokemon, int byteOffset)
{
	const size_t index = static_cast<size_t>(byteOffset / 2);
	return (index < pokemon.words.size()) ? pokemon.words[index] : 0;
}

static uint16_t GetDecryptedPokemonSpeciesId(const DecryptedPokemon &pokemon)
{
	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	return GetWord(pokemon, blockA + 0x0);
}

static bool IsPartyMoveSlotDisplayEmpty(const ExportProfileDefinition *profile, uint16_t moveId)
{
	if (moveId == 0u || moveId > static_cast<uint16_t>(kMaxReasonableMoveId)) {
		return true;
	}

	const char *moveName = LookupProfileMoveName(profile, moveId);
	return moveName == NULL || moveName[0] == '\0';
}

static bool AreAllPartyMoveSlotsDisplayEmpty(const ExportProfileDefinition *profile, const DecryptedPokemon &pokemon)
{
	if (GetDecryptedPokemonSpeciesId(pokemon) == 0u) {
		return false;
	}

	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockB = kShuffleTable[shuffleKey][1];
	for (int i = 0; i < 4; ++i) {
		if (!IsPartyMoveSlotDisplayEmpty(profile, GetWord(pokemon, blockB + (i * 2)))) {
			return false;
		}
	}

	return true;
}

static bool HasAnyPartyMoveIdAboveMax(const DecryptedPokemon &pokemon)
{
	if (GetDecryptedPokemonSpeciesId(pokemon) == 0u) {
		return false;
	}

	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockB = kShuffleTable[shuffleKey][1];
	for (int i = 0; i < 4; ++i) {
		if (GetWord(pokemon, blockB + (i * 2)) > static_cast<uint16_t>(kMaxReasonableMoveId)) {
			return true;
		}
	}

	return false;
}

static std::string FormatDebugHexU16(uint16_t value)
{
	char buffer[16] = {0};
	std::snprintf(buffer, sizeof(buffer), "0x%04X", value);
	return std::string(buffer);
}

static std::string FormatDebugHexU32(uint32_t value)
{
	char buffer[16] = {0};
	std::snprintf(buffer, sizeof(buffer), "0x%08X", value);
	return std::string(buffer);
}

static void AppendDebugBytes(std::ostringstream &out, const char *label, const std::vector<uint8_t> &bytes)
{
	out << label << ":\n";
	for (size_t byteIndex = 0; byteIndex < bytes.size(); ++byteIndex) {
		if ((byteIndex % 16u) == 0u) {
			char addressBuffer[16] = {0};
			std::snprintf(addressBuffer, sizeof(addressBuffer), "%04X", static_cast<unsigned int>(byteIndex));
			out << addressBuffer << ": ";
		}

		char byteBuffer[8] = {0};
		std::snprintf(byteBuffer, sizeof(byteBuffer), "%02X", static_cast<unsigned int>(bytes[byteIndex]));
		out << byteBuffer;
		if (((byteIndex % 16u) == 15u) || ((byteIndex + 1u) == bytes.size())) {
			out << "\n";
		} else {
			out << " ";
		}
	}
}

static void AppendDebugPokemonWords(std::ostringstream &out, const DecryptedPokemon &pokemon)
{
	out << "decoded_words:\n";
	for (size_t wordIndex = 0; wordIndex < pokemon.words.size(); ++wordIndex) {
		if ((wordIndex % 8u) == 0u) {
			char addressBuffer[16] = {0};
			std::snprintf(addressBuffer, sizeof(addressBuffer), "%04X", static_cast<unsigned int>(wordIndex * 2u));
			out << addressBuffer << ": ";
		}
		out << FormatDebugHexU16(pokemon.words[wordIndex]);
		if (((wordIndex % 8u) == 7u) || ((wordIndex + 1u) == pokemon.words.size())) {
			out << "\n";
		} else {
			out << " ";
		}
	}
}

static std::string BuildEditorDebugSnapshotText(
	const ExportProfileDefinition *profile,
	const DecryptedPokemon &pokemon,
	uint32_t monOffset,
	const char *source,
	int box,
	int slot)
{
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const int blockB = kShuffleTable[shuffleKey][1];
	const int blockC = kShuffleTable[shuffleKey][2];
	const uint16_t speciesId = GetWord(pokemon, blockA + 0x0);
	const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
		(static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);
	const uint32_t statusRaw = static_cast<uint32_t>(GetWord(pokemon, 0x88)) |
		(static_cast<uint32_t>(GetWord(pokemon, 0x8A)) << 16);

	std::ostringstream out;
	out << "Editor debug snapshot\n";
	out << "source: " << ((source != NULL) ? source : "") << "\n";
	if (box > 0) {
		out << "box: " << box << "\n";
	}
	out << "slot: " << slot << "\n";
	out << "profile: " << ((profile != NULL && profile->displayName != NULL) ? profile->displayName : "") << "\n";
	out << "game_code: " << GameCodeString() << "\n";
	out << "mon_offset: " << FormatDebugHexU32(monOffset) << "\n";
	out << "pid: " << FormatDebugHexU32(pokemon.pid) << "\n";
	out << "checksum: " << FormatDebugHexU16(pokemon.checksum) << "\n";
	out << "checksum_valid: " << (pokemon.checksumValid ? "true" : "false") << "\n";
	out << "shuffle_key: " << shuffleKey << "\n";
	out << "block_a: " << FormatDebugHexU16(static_cast<uint16_t>(blockA)) << "\n";
	out << "block_b: " << FormatDebugHexU16(static_cast<uint16_t>(blockB)) << "\n";
	out << "block_c: " << FormatDebugHexU16(static_cast<uint16_t>(blockC)) << "\n";
	out << "species_id: " << speciesId << "\n";
	out << "species: " << LookupProfileSpeciesName(profile, speciesId) << "\n";
	out << "held_item_id: " << GetWord(pokemon, blockA + 0x2) << "\n";
	out << "experience: " << experience << "\n";
	out << "level: " << GetProfileLevelForExperience(profile, experience, speciesId) << "\n";
	out << "friendship: " << static_cast<int>(GetWord(pokemon, blockA + 0xC) & 0x00FFu) << "\n";
	out << "ability_id: " << static_cast<int>((GetWord(pokemon, blockA + 0xC) >> 8) & 0x00FFu) << "\n";
	out << "current_hp: " << GetWord(pokemon, 0x8E) << "\n";
	out << "max_hp: " << GetWord(pokemon, 0x90) << "\n";
	out << "status_raw: " << FormatDebugHexU32(statusRaw) << "\n";
	out << "moves:\n";
	for (int i = 0; i < 4; ++i) {
		const uint16_t moveId = GetWord(pokemon, blockB + (i * 2));
		out << "  " << (i + 1) << ": id=" << moveId
			<< " name=\"" << LookupProfileMoveName(profile, moveId) << "\""
			<< " display_empty=" << (IsPartyMoveSlotDisplayEmpty(profile, moveId) ? "true" : "false")
			<< "\n";
	}
	out << "evs: "
		<< static_cast<int>(GetWord(pokemon, blockA + 0x10) & 0xFFu) << " "
		<< static_cast<int>((GetWord(pokemon, blockA + 0x10) >> 8) & 0xFFu) << " "
		<< static_cast<int>(GetWord(pokemon, blockA + 0x12) & 0xFFu) << " "
		<< static_cast<int>(GetWord(pokemon, blockA + 0x14) & 0xFFu) << " "
		<< static_cast<int>((GetWord(pokemon, blockA + 0x14) >> 8) & 0xFFu) << " "
		<< static_cast<int>((GetWord(pokemon, blockA + 0x12) >> 8) & 0xFFu) << "\n";

	const uint32_t ivData = static_cast<uint32_t>(GetWord(pokemon, blockB + 0x10)) |
		(static_cast<uint32_t>(GetWord(pokemon, blockB + 0x12)) << 16);
	out << "iv_data: " << FormatDebugHexU32(ivData) << "\n";
	out << "ivs: "
		<< static_cast<int>(ivData & 0x1Fu) << " "
		<< static_cast<int>((ivData >> 5) & 0x1Fu) << " "
		<< static_cast<int>((ivData >> 10) & 0x1Fu) << " "
		<< static_cast<int>((ivData >> 20) & 0x1Fu) << " "
		<< static_cast<int>((ivData >> 25) & 0x1Fu) << " "
		<< static_cast<int>((ivData >> 15) & 0x1Fu) << "\n";

	AppendDebugPokemonWords(out, pokemon);

	if (layout != NULL) {
		std::vector<uint8_t> rawBytes;
		SnapshotArm9Bytes(monOffset, (source != NULL && std::strcmp(source, "box") == 0) ? layout->boxMonSize : layout->partyMonSize, rawBytes);
		AppendDebugBytes(out, "raw_bytes_at_snapshot_build", rawBytes);
	}

	return out.str();
}

static uint8_t GetPokemonAbilitySlotDisplayIndex(const ExportProfileDefinition *profile, const DecryptedPokemon &pokemon)
{
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL || layout->codec != POKEMON_DATA_CODEC_GEN4) {
		return 0u;
	}

	return static_cast<uint8_t>((pokemon.pid & 0x1u) + 1u);
}

static std::string DecodePokemonGender(uint16_t genderFormeWord)
{
	if (((genderFormeWord >> 2) & 0x1) == 1) {
		return "N";
	}
	if (((genderFormeWord >> 1) & 0x1) == 1) {
		return "F";
	}
	return "M";
}

static const char *GetGen4LocationNameAdapter(uint16_t locationId)
{
	return desmume_pokemon_gen4::GetLocationName(locationId);
}

static const char *GetHgssLocationNameAdapter(uint16_t locationId)
{
	return desmume_pokemon_gen4_hgss_locations::GetLocationName(locationId);
}

static const char *GetGen4FormSuffixAdapter(uint16_t speciesId, uint8_t formIndex)
{
	return desmume_pokemon_gen4::GetFormSuffix(speciesId, formIndex);
}

static const char *GetGen5BwLocationNameAdapter(uint16_t locationId)
{
	return desmume_pokemon_gen5::GetBwLocationName(locationId);
}

static const char *GetGen5Bw2LocationNameAdapter(uint16_t locationId)
{
	return desmume_pokemon_gen5::GetBw2LocationName(locationId);
}

static const char *GetGen5FormSuffixAdapter(uint16_t speciesId, uint8_t formIndex)
{
	return desmume_pokemon_gen5::GetFormSuffix(speciesId, formIndex);
}

static int GetLevelForExperienceGen4WithGrowthCurves(
	uint32_t experience,
	int speciesId,
	const uint8_t *growthCurves,
	size_t growthCurvesCount)
{
	using namespace desmume_pokemon_gen4;

	if (speciesId <= 0 || static_cast<size_t>(speciesId) > growthCurvesCount) {
		return 0;
	}

	const uint8_t curve = growthCurves[speciesId - 1];
	if (curve >= 6) {
		return 0;
	}

	for (int level = 100; level >= 1; --level) {
		if (experience >= kLevelExpCurve[level - 1][curve]) {
			return level;
		}
	}

	return 0;
}

static uint32_t GetExperienceForLevelGen4WithGrowthCurves(
	int level,
	int speciesId,
	const uint8_t *growthCurves,
	size_t growthCurvesCount)
{
	using namespace desmume_pokemon_gen4;

	if (level < 1) {
		level = 1;
	} else if (level > 100) {
		level = 100;
	}

	if (speciesId <= 0 || static_cast<size_t>(speciesId) > growthCurvesCount) {
		return 0;
	}

	const uint8_t curve = growthCurves[speciesId - 1];
	if (curve >= 6) {
		return 0;
	}

	return kLevelExpCurve[level - 1][curve];
}

static int GetLevelForExperienceGen4(uint32_t experience, int speciesId)
{
	return GetLevelForExperienceGen4WithGrowthCurves(
		experience,
		speciesId,
		desmume_pokemon_gen4::kGrowthCurves,
		desmume_pokemon_gen4::kGrowthCurvesCount);
}

static uint32_t GetExperienceForLevelGen4(int level, int speciesId)
{
	return GetExperienceForLevelGen4WithGrowthCurves(
		level,
		speciesId,
		desmume_pokemon_gen4::kGrowthCurves,
		desmume_pokemon_gen4::kGrowthCurvesCount);
}

static int GetLevelForExperiencePlatinumKaizo(uint32_t experience, int speciesId)
{
	return GetLevelForExperienceGen4WithGrowthCurves(
		experience,
		speciesId,
		desmume_pokemon_gen4_platinum_kaizo::kGrowthCurves,
		desmume_pokemon_gen4_platinum_kaizo::kGrowthCurvesCount);
}

static uint32_t GetExperienceForLevelPlatinumKaizo(int level, int speciesId)
{
	return GetExperienceForLevelGen4WithGrowthCurves(
		level,
		speciesId,
		desmume_pokemon_gen4_platinum_kaizo::kGrowthCurves,
		desmume_pokemon_gen4_platinum_kaizo::kGrowthCurvesCount);
}

static int GetLevelForExperienceSterlingSilver(uint32_t experience, int speciesId)
{
	return GetLevelForExperienceGen4WithGrowthCurves(
		experience,
		speciesId,
		desmume_pokemon_gen4_sterling_silver::kGrowthCurves,
		desmume_pokemon_gen4_sterling_silver::kGrowthCurvesCount);
}

static uint32_t GetExperienceForLevelSterlingSilver(int level, int speciesId)
{
	return GetExperienceForLevelGen4WithGrowthCurves(
		level,
		speciesId,
		desmume_pokemon_gen4_sterling_silver::kGrowthCurves,
		desmume_pokemon_gen4_sterling_silver::kGrowthCurvesCount);
}

static int GetLevelForExperienceGen5WithGrowthCurves(
	uint32_t experience,
	int speciesId,
	const uint8_t *growthCurves,
	size_t growthCurvesCount)
{
	using namespace desmume_pokemon_gen5;

	if (speciesId <= 0 || static_cast<size_t>(speciesId) > growthCurvesCount) {
		return 0;
	}

	const uint8_t curve = growthCurves[speciesId - 1];
	if (curve >= 6) {
		return 0;
	}

	for (int level = 100; level >= 1; --level) {
		if (experience >= kLevelExpCurve[level - 1][curve]) {
			return level;
		}
	}

	return 0;
}

static uint32_t GetExperienceForLevelGen5WithGrowthCurves(
	int level,
	int speciesId,
	const uint8_t *growthCurves,
	size_t growthCurvesCount)
{
	using namespace desmume_pokemon_gen5;

	if (level < 1) {
		level = 1;
	} else if (level > 100) {
		level = 100;
	}

	if (speciesId <= 0 || static_cast<size_t>(speciesId) > growthCurvesCount) {
		return 0;
	}

	const uint8_t curve = growthCurves[speciesId - 1];
	if (curve >= 6) {
		return 0;
	}

	return kLevelExpCurve[level - 1][curve];
}

static int GetLevelForExperienceGen5(uint32_t experience, int speciesId)
{
	return GetLevelForExperienceGen5WithGrowthCurves(
		experience,
		speciesId,
		desmume_pokemon_gen5::kGrowthCurves,
		desmume_pokemon_gen5::kGrowthCurvesCount);
}

static uint32_t GetExperienceForLevelGen5(int level, int speciesId)
{
	return GetExperienceForLevelGen5WithGrowthCurves(
		level,
		speciesId,
		desmume_pokemon_gen5::kGrowthCurves,
		desmume_pokemon_gen5::kGrowthCurvesCount);
}

static int GetLevelForExperienceBb2Vw2Redux(uint32_t experience, int speciesId)
{
	return GetLevelForExperienceGen5WithGrowthCurves(
		experience,
		speciesId,
		desmume_pokemon_gen5_bb2_vw2_redux::kGrowthCurves,
		desmume_pokemon_gen5_bb2_vw2_redux::kGrowthCurvesCount);
}

static uint32_t GetExperienceForLevelBb2Vw2Redux(int level, int speciesId)
{
	return GetExperienceForLevelGen5WithGrowthCurves(
		level,
		speciesId,
		desmume_pokemon_gen5_bb2_vw2_redux::kGrowthCurves,
		desmume_pokemon_gen5_bb2_vw2_redux::kGrowthCurvesCount);
}

static std::string NormalizeMoveName(const std::string &name)
{
	std::string normalized;
	normalized.reserve(name.size());
	for (size_t i = 0; i < name.size(); ++i) {
		const unsigned char ch = static_cast<unsigned char>(name[i]);
		if (std::isalnum(ch)) {
			normalized.push_back(static_cast<char>(std::tolower(ch)));
		}
	}
	return normalized;
}

static bool AppendUniqueSpeciesOption(
	std::vector<std::string> &options,
	std::map<std::string, bool> &seenNormalizedNames,
	const std::string &candidate)
{
	if (candidate.empty()) {
		return false;
	}

	const std::string normalized = NormalizeMoveName(candidate);
	if (normalized.empty() || seenNormalizedNames.find(normalized) != seenNormalizedNames.end()) {
		return false;
	}

	seenNormalizedNames[normalized] = true;
	options.push_back(candidate);
	return true;
}

static bool LookupNormalizedSpeciesNameForProfile(
	const ExportProfileDefinition *profile,
	uint16_t speciesId,
	const std::string &normalizedName,
	std::string &canonicalName)
{
	const char *baseName = LookupProfileSpeciesName(profile, static_cast<int>(speciesId));
	if (baseName == NULL || baseName[0] == '\0') {
		return false;
	}

	if (NormalizeMoveName(baseName) == normalizedName) {
		canonicalName.assign(baseName);
		return true;
	}

	for (uint32_t formIndex = 1u; formIndex <= 63u; ++formIndex) {
		const char *formSuffix = GetProfileFormSuffix(profile, speciesId, static_cast<uint8_t>(formIndex));
		if (formSuffix == NULL || formSuffix[0] == '\0') {
			continue;
		}

		std::string formName(baseName);
		formName += formSuffix;
		if (NormalizeMoveName(formName) == normalizedName) {
			canonicalName = formName;
			return true;
		}
	}

	return false;
}

static bool LookupSpeciesIdByNameForProfile(const ExportProfileDefinition *profile, const std::string &name, uint16_t &speciesId, std::string &canonicalName)
{
	const std::string normalized = NormalizeMoveName(name);
	if (normalized.empty()) {
		speciesId = 0;
		canonicalName.clear();
		return true;
	}

	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		speciesId = 0;
		canonicalName.clear();
		return false;
	}

	for (size_t i = 1; i <= tables->speciesNamesCount; ++i) {
		if (LookupNormalizedSpeciesNameForProfile(profile, static_cast<uint16_t>(i), normalized, canonicalName)) {
			speciesId = static_cast<uint16_t>(i);
			return true;
		}
	}

	speciesId = 0;
	canonicalName.clear();
	return false;
}

static bool LookupItemIdByNameForProfile(const ExportProfileDefinition *profile, const std::string &name, uint16_t &itemId, std::string &canonicalName)
{
	const std::string normalized = NormalizeMoveName(name);
	if (normalized.empty()) {
		itemId = 0;
		canonicalName.clear();
		return true;
	}

	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		itemId = 0;
		canonicalName.clear();
		return false;
	}

	for (size_t i = 1; i <= tables->itemNamesCount; ++i) {
		const char *candidate = LookupProfileItemName(profile, static_cast<int>(i));
		if (candidate == NULL || candidate[0] == '\0') {
			continue;
		}
		if (NormalizeMoveName(candidate) == normalized) {
			itemId = static_cast<uint16_t>(i);
			canonicalName.assign(candidate);
			return true;
		}
	}

	itemId = 0;
	canonicalName.clear();
	return false;
}

static bool LookupAbilityIdByNameForProfile(const ExportProfileDefinition *profile, const std::string &name, uint16_t &abilityId, std::string &canonicalName)
{
	const std::string normalized = NormalizeMoveName(name);
	if (normalized.empty()) {
		abilityId = 0;
		canonicalName.clear();
		return true;
	}

	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		abilityId = 0;
		canonicalName.clear();
		return false;
	}

	for (size_t i = 1; i <= tables->abilityNamesCount; ++i) {
		const char *candidate = LookupProfileAbilityName(profile, static_cast<int>(i));
		if (candidate == NULL || candidate[0] == '\0') {
			continue;
		}
		if (NormalizeMoveName(candidate) == normalized) {
			abilityId = static_cast<uint16_t>(i);
			canonicalName.assign(candidate);
			return true;
		}
	}

	abilityId = 0;
	canonicalName.clear();
	return false;
}

static uint8_t GetMoveBasePpGen4(uint16_t moveId)
{
	if (moveId == 0) {
		return 0;
	}
	const size_t index = static_cast<size_t>(moveId - 1);
	return (index < (sizeof(kMoveBasePp) / sizeof(kMoveBasePp[0]))) ? kMoveBasePp[index] : 0;
}

static bool LookupMoveIdByNameGen4(const std::string &name, uint16_t &moveId, std::string &canonicalName)
{
	using namespace desmume_pokemon_gen4_hgss_rng;

	const std::string normalized = NormalizeMoveName(name);
	if (normalized.empty()) {
		moveId = 0;
		canonicalName.clear();
		return true;
	}

	for (size_t i = 0; i < kMoveNamesCount; ++i) {
		const char *candidate = kMoveNames[i];
		if (candidate == NULL || candidate[0] == '\0') {
			continue;
		}
		if (NormalizeMoveName(candidate) == normalized) {
			moveId = static_cast<uint16_t>(i + 1);
			canonicalName.assign(candidate);
			return true;
		}
	}

	moveId = 0;
	canonicalName.clear();
	return false;
}

static bool LookupMoveIdByNameGen5(const std::string &name, uint16_t &moveId, std::string &canonicalName)
{
	using namespace desmume_pokemon_gen5;

	const std::string normalized = NormalizeMoveName(name);
	if (normalized.empty()) {
		moveId = 0;
		canonicalName.clear();
		return true;
	}

	for (size_t i = 0; i < kMoveNamesCount; ++i) {
		const char *candidate = kMoveNames[i];
		if (candidate == NULL || candidate[0] == '\0') {
			continue;
		}
		if (NormalizeMoveName(candidate) == normalized) {
			moveId = static_cast<uint16_t>(i + 1);
			canonicalName.assign(candidate);
			return true;
		}
	}

	moveId = 0;
	canonicalName.clear();
	return false;
}

static const char *GetPlatinumKaizoMoveNameOverride(uint16_t moveId)
{
	switch (moveId) {
		case 4: return "Water Ball";
		case 20: return "Mystical Fire";
		case 30: return "Fire Ball";
		case 111: return "Rock Ball";
		case 130: return "Solar-Beam";
		case 131: return "Hail Ball";
		case 140: return "Hurricane";
		case 149: return "Triple Axel";
		case 150: return "HP Water";
		case 160: return "HP Ice";
		case 171: return "HP Ghost";
		case 209: return "Wild Charge";
		case 222: return "Bulldoze";
		case 229: return "HP Fire";
		case 255: return "HP Rock";
		case 269: return "HP Dark";
		case 271: return "HP Psychic";
		case 289: return "Drill Run";
		case 292: return "HP Fighting";
		case 300: return "HP Ground";
		case 343: return "HP Grass";
		case 346: return "Scald";
		case 361: return "HP Electric";
		case 377: return "Aqua Cutter";
		case 415: return "HP Flying";
		default: return NULL;
	}
}

static const char *GetSterlingSilverMoveNameOverride(uint16_t moveId)
{
	switch (moveId) {
		case 1: return "HP Normal";
		case 16: return "Hurricane";
		case 21: return "Night Daze";
		case 22: return "HP Grass";
		case 32: return "Drill Run";
		case 52: return "HP Fire";
		case 66: return "HP Fighting";
		case 84: return "HP Electric";
		case 88: return "HP Rock";
		case 90: return "Headlong Rush";
		case 124: return "HP Poison";
		case 149: return "HP Psychic";
		case 154: return "Dual Chop";
		case 169: return "Electroweb";
		case 181: return "HP Ice";
		case 205: return "Accelerock";
		case 222: return "Bulldoze";
		case 239: return "HP Dragon";
		case 297: return "HP Flying";
		case 300: return "HP Ground";
		case 310: return "HP Ghost";
		case 334: return "HP Steel";
		case 346: return "HP Water";
		case 451: return "Volt Switch";
		case 455: return "HP Bug";
		case 464: return "HP Dark";
		default: return NULL;
	}
}

static const char *GetRenegadePlatinumMoveNameOverride(uint16_t moveId)
{
	switch (moveId) {
		case 140: return "Draining Kiss";
		case 362: return "Scald";
		case 132: return "Icicle Crash";
		case 32: return "Drill Run";
		case 461: return "Moonblast";
		case 295: return "Dazzling Gleam";
		case 296: return "Disarming Voice";
		case 328: return "Bulldoze";
		case 66: return "Play Rough";
		case 239: return "Hurricane";
		case 344: return "Wild Charge";
		default: return NULL;
	}
}

static const char *GetBb2Vw2ReduxMoveNameOverride(uint16_t moveId)
{
	switch (moveId) {
		case 4: return "Power-Up Punch";
		case 5: return "Smart Strike";
		case 11: return "Dual Wingbeat";
		case 13: return "Fire Lash";
		case 21: return "Brutal Swing";
		case 25: return "High Horsepower";
		case 27: return "Headlong Rush";
		case 32: return "Boomburst";
		case 37: return "Raging Fury";
		case 66: return "Play Rough";
		case 121: return "Psychic Fangs";
		case 131: return "Breaking Swipe";
		case 132: return "Infestation";
		case 140: return "Lunge";
		case 145: return "Esper Wing";
		case 159: return "Nuzzle";
		case 167: return "Triple Axel";
		case 186: return "Draining Kiss";
		case 221: return "Mystical Fire";
		case 265: return "Psyshield Bash";
		case 295: return "Dazzling Gleam";
		case 296: return "Moonblast";
		case 319: return "Scorching Sands";
		case 328: return "Accelerock";
		case 386: return "Wave Crash";
		case 496: return "Disarming Voice";
		default: return NULL;
	}
}

static uint32_t CanonicalizePartyStatusForEditor(const ExportProfileDefinition *profile, uint32_t statusRaw)
{
	if (profile != NULL && IsGen5BwLikePokemonProfile(profile->profile)) {
		switch (statusRaw & 0xFFFFu) {
			case 0u: return 0u;
			case 1u: return 64u;
			case 2u: return 1u;
			case 3u: return 32u;
			case 4u: return 16u;
			case 5u: return 8u;
			case 6u: return 128u;
			default: return 0u;
		}
	}

	return statusRaw;
}

static uint32_t EncodePartyStatusForProfile(const ExportProfileDefinition *profile, uint32_t editorStatusRaw)
{
	if (profile != NULL && IsGen5BwLikePokemonProfile(profile->profile)) {
		switch (editorStatusRaw) {
			case 0u: return 0u;
			case 8u: return 5u;
			case 16u: return 4u;
			case 32u: return 3u;
			case 64u: return 1u;
			case 128u: return 6u;
			default:
				if ((editorStatusRaw & 0x7u) != 0u) {
					return 2u;
				}
				return 0u;
		}
	}

	return editorStatusRaw;
}

static uint32_t GetPartyStatusRawFromPokemon(const DecryptedPokemon &pokemon)
{
	return static_cast<uint32_t>(GetWord(pokemon, 0x88)) |
	       (static_cast<uint32_t>(GetWord(pokemon, 0x8A)) << 16);
}

static bool IsPlausiblePartyCoreForProfile(const ExportProfileDefinition *profile, const DecryptedPokemon &pokemon)
{
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		return false;
	}

	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const uint16_t speciesId = GetWord(pokemon, blockA + 0x0);
	if (speciesId == 0u || static_cast<size_t>(speciesId) > tables->speciesNamesCount) {
		return false;
	}

	const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
		(static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);
	const uint32_t maxExperience = GetProfileMaxLevel100Experience(profile);
	if (experience > maxExperience) {
		return false;
	}

	const int coreLevel = GetProfileLevelForExperience(profile, experience, speciesId);
	return coreLevel >= 1 && coreLevel <= 100;
}

static bool IsPokemonValidEditorFrameForProfile(const ExportProfileDefinition *profile, const DecryptedPokemon &pokemon)
{
	return pokemon.checksumValid &&
		IsPlausiblePartyCoreForProfile(profile, pokemon) &&
		!HasAnyPartyMoveIdAboveMax(pokemon) &&
		!AreAllPartyMoveSlotsDisplayEmpty(profile, pokemon);
}

static bool IsPartyPokemonValidFrameForWrite(const ExportProfileDefinition *profile, const DecryptedPokemon &pokemon)
{
	return IsPokemonValidEditorFrameForProfile(profile, pokemon);
}

static bool PartyCoreLevelMatchesTailForProfile(
	const ExportProfileDefinition *profile,
	const DecryptedPokemon &pokemon,
	const std::vector<uint16_t> &tailWords)
{
	if (tailWords.size() < 3u || !IsPlausiblePartyCoreForProfile(profile, pokemon)) {
		return false;
	}

	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const uint16_t speciesId = GetWord(pokemon, blockA + 0x0);
	const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
		(static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);
	const int coreLevel = GetProfileLevelForExperience(profile, experience, speciesId);
	const int tailLevel = static_cast<int>(tailWords[2] & 0x00FFu);
	return coreLevel == tailLevel;
}

static bool IsRecognizedPartyStatusRaw(const ExportProfileDefinition *profile, uint32_t statusRaw)
{
	if (profile != NULL && IsGen5BwLikePokemonProfile(profile->profile)) {
		switch (statusRaw & 0xFFFFu) {
			case 0u:
			case 1u:
			case 2u:
			case 3u:
			case 4u:
			case 5u:
			case 6u:
				return (statusRaw & 0xFFFF0000u) == 0u;
			default:
				return false;
		}
	}

	if ((statusRaw & 0xFFFFFF00u) != 0u) {
		return false;
	}

	const uint32_t lowByte = statusRaw & 0xFFu;
	if (lowByte == 0u) {
		return true;
	}

	const uint32_t sleepTurns = lowByte & 0x7u;
	const uint32_t ailmentFlags = lowByte & 0xF8u;
	if (sleepTurns != 0u) {
		return ailmentFlags == 0u;
	}

	switch (ailmentFlags) {
		case 0x08u:
		case 0x10u:
		case 0x20u:
		case 0x40u:
		case 0x80u:
			return true;
		default:
			return false;
	}
}

static void SetPartyStatusRawOnPokemon(DecryptedPokemon &pokemon, uint32_t statusRaw)
{
	pokemon.words[0x88 / 2] = static_cast<uint16_t>(statusRaw & 0xFFFFu);
	pokemon.words[0x8A / 2] = static_cast<uint16_t>((statusRaw >> 16) & 0xFFFFu);
}

static void SetPokemonMovePpAtIndex(DecryptedPokemon &pokemon, int blockB, int moveIndex, uint8_t pp)
{
	const int ppWordOffset = blockB + ((moveIndex < 2) ? 0x8 : 0xA);
	const int shift = (moveIndex % 2) * 8;
	uint16_t packedPp = GetWord(pokemon, ppWordOffset);
	packedPp &= static_cast<uint16_t>(~(0xFFu << shift));
	packedPp |= static_cast<uint16_t>(pp) << shift;
	pokemon.words[ppWordOffset / 2] = packedPp;
}

static void ApplyRequestedMovesPreservingPp(
	const ExportProfileDefinition *profile,
	DecryptedPokemon &pokemon,
	int blockB,
	const uint16_t moveIds[4])
{
	for (int i = 0; i < 4; ++i) {
		const uint16_t originalMoveId = GetWord(pokemon, blockB + (i * 2));
		const uint16_t requestedMoveId = moveIds[i];
		pokemon.words[(blockB + (i * 2)) / 2] = requestedMoveId;
		if (requestedMoveId != originalMoveId) {
			SetPokemonMovePpAtIndex(pokemon, blockB, i, GetProfileMoveBasePp(profile, requestedMoveId));
		}
	}
}

static std::string GetPartyStatusLabel(const ExportProfileDefinition *profile, uint32_t statusRaw)
{
	if (profile != NULL && IsGen5BwLikePokemonProfile(profile->profile)) {
		switch (statusRaw & 0xFFFFu) {
			case 1u: return "Paralysis";
			case 2u: return "Sleep";
			case 3u: return "Freeze";
			case 4u: return "Burn";
			case 5u: return "Poison";
			case 6u: return "Badly Poisoned";
			default: return "Healthy";
		}
	}

	const uint32_t sleepTurns = statusRaw & 0x7u;
	if (sleepTurns != 0) {
		return "Sleep(" + std::to_string(sleepTurns) + ")";
	}
	if ((statusRaw & 0x80u) != 0) {
		return "Badly Poisoned";
	}
	if ((statusRaw & 0x40u) != 0) {
		return "Paralysis";
	}
	if ((statusRaw & 0x20u) != 0) {
		return "Freeze";
	}
	if ((statusRaw & 0x10u) != 0) {
		return "Burn";
	}
	if ((statusRaw & 0x8u) != 0) {
		return "Poison";
	}
	return "Healthy";
}

static bool DecryptPokemonAt(uint32_t monOffset, bool boxPokemon, DecryptedPokemon &outPokemon)
{
	std::vector<uint8_t> monBytes;
	const size_t monSize = boxPokemon ? kBoxMonSize : kPartyMonSize;
	SnapshotArm9Bytes(monOffset, monSize, monBytes);

	outPokemon.words.assign(kPartyMonSize / 2, 0);
	outPokemon.pid = ReadSnapshotU32(monBytes, 0x0u);
	outPokemon.checksum = ReadSnapshotU16(monBytes, 0x6u);
	outPokemon.checksumValid = false;

	for (int byteOffset = 0; byteOffset <= 0x6; byteOffset += 2) {
		outPokemon.words[byteOffset / 2] = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
	}

	uint32_t seed = outPokemon.checksum;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		seed = AdvanceLcrng(seed);
		outPokemon.words[byteOffset / 2] = static_cast<uint16_t>(ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset)) ^ (seed >> 16));
	}

	if (!boxPokemon) {
		seed = outPokemon.pid;
		for (int byteOffset = 0x88; byteOffset <= 0xEA; byteOffset += 2) {
			seed = AdvanceLcrng(seed);
			outPokemon.words[byteOffset / 2] = static_cast<uint16_t>(ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset)) ^ (seed >> 16));
		}
	}

	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		checksum = (checksum + outPokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	outPokemon.checksumValid = (checksum == outPokemon.checksum);
	return outPokemon.checksumValid;
}

static bool TryDecryptPokemonWithRetries(uint32_t monOffset, bool boxPokemon, DecryptedPokemon &outPokemon)
{
	for (int attempt = 0; attempt < kDecryptRetryCount; ++attempt) {
		if (DecryptPokemonAt(monOffset, boxPokemon, outPokemon)) {
			return true;
		}
	}

	return false;
}

static bool ReadAlreadyDecryptedPartyPokemon(uint32_t monOffset, DecryptedPokemon &outPokemon)
{
	std::vector<uint8_t> monBytes;
	SnapshotArm9Bytes(monOffset, kPartyMonSize, monBytes);

	outPokemon.words.assign(kPartyMonSize / 2, 0);
	outPokemon.pid = ReadSnapshotU32(monBytes, 0x0u);
	outPokemon.checksum = ReadSnapshotU16(monBytes, 0x6u);
	outPokemon.checksumValid = false;

	for (int byteOffset = 0; byteOffset <= 0xEA; byteOffset += 2) {
		outPokemon.words[byteOffset / 2] = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
	}

	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		checksum = (checksum + outPokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	outPokemon.checksumValid = (checksum == outPokemon.checksum);
	return outPokemon.checksumValid;
}

static bool ReadAlreadyDecryptedBoxPokemon(uint32_t monOffset, DecryptedPokemon &outPokemon)
{
	std::vector<uint8_t> monBytes;
	SnapshotArm9Bytes(monOffset, kBoxMonSize, monBytes);

	outPokemon.words.assign(kPartyMonSize / 2, 0);
	outPokemon.pid = ReadSnapshotU32(monBytes, 0x0u);
	outPokemon.checksum = ReadSnapshotU16(monBytes, 0x6u);
	outPokemon.checksumValid = false;

	for (int byteOffset = 0; byteOffset <= 0x86; byteOffset += 2) {
		outPokemon.words[byteOffset / 2] = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
	}

	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		checksum = (checksum + outPokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	outPokemon.checksumValid = (checksum == outPokemon.checksum);
	return outPokemon.checksumValid;
}

static bool ReadPokemonWithFallback(uint32_t monOffset, bool boxPokemon, bool allowPreDecryptedFallback, DecryptedPokemon &outPokemon)
{
	if (TryDecryptPokemonWithRetries(monOffset, boxPokemon, outPokemon)) {
		return true;
	}

	if (allowPreDecryptedFallback) {
		if (boxPokemon) {
			return ReadAlreadyDecryptedBoxPokemon(monOffset, outPokemon);
		}

		return ReadAlreadyDecryptedPartyPokemon(monOffset, outPokemon);
	}

	return false;
}

static const int kMaxReasonablePartyBattleHp = 750;
static const int kMaxReasonablePartyNonHpStat = 600;

static bool HasOverlargePartyNonHpStat(const std::vector<uint16_t> &battleWords)
{
	// Gen 5 party tail words 5..9 are Atk/Def/Spe/SpA/SpD. Word 10 starts mail data.
	for (size_t wordIndex = 5u; wordIndex <= 9u && wordIndex < battleWords.size(); ++wordIndex) {
		if (static_cast<int>(battleWords[wordIndex]) > kMaxReasonablePartyNonHpStat) {
			return true;
		}
	}

	return false;
}

static int ScorePlausiblePartyBattleTailVitals(const std::vector<uint16_t> &battleWords)
{
	if (battleWords.size() < 11u) {
		return 0;
	}

	int score = 0;
	const int level = static_cast<int>(battleWords[2] & 0x00FFu);
	const int currentHp = static_cast<int>(battleWords[3]);
	const int maxHp = static_cast<int>(battleWords[4]);
	if (level >= 1 && level <= 100) {
		score += 2;
	}
	if (currentHp >= 0 && currentHp <= kMaxReasonablePartyBattleHp) {
		score += 2;
	}
	if (maxHp >= 1 && maxHp <= kMaxReasonablePartyBattleHp) {
		score += 2;
	}
	if (currentHp <= maxHp) {
		score += 2;
	}
	if (HasOverlargePartyNonHpStat(battleWords)) {
		return 0;
	}

	for (size_t wordIndex = 5u; wordIndex <= 9u; ++wordIndex) {
		const int statValue = static_cast<int>(battleWords[wordIndex]);
		if (statValue >= 1 && statValue <= kMaxReasonablePartyNonHpStat) {
			++score;
		}
	}

	return score;
}

static bool ShouldDeferToRawPartyBattleTail(
	const std::vector<uint16_t> &rawBattleWords,
	const std::vector<uint16_t> &decryptedBattleWords)
{
	const int rawScore = ScorePlausiblePartyBattleTailVitals(rawBattleWords);
	const int decryptedScore = ScorePlausiblePartyBattleTailVitals(decryptedBattleWords);
	return rawScore >= 10 && rawScore >= (decryptedScore + 4);
}

static bool IsReasonableGen5PartyTailWords(const std::vector<uint16_t> &battleWords)
{
	if (battleWords.size() < 5u) {
		return false;
	}

	const int level = static_cast<int>(battleWords[2] & 0x00FFu);
	const int currentHp = static_cast<int>(battleWords[3]);
	const int maxHp = static_cast<int>(battleWords[4]);

	if (level < 1 || level > 100) {
		return false;
	}
	if (currentHp < 0 || currentHp > kMaxReasonablePartyBattleHp) {
		return false;
	}
	if (maxHp < 1 || maxHp > kMaxReasonablePartyBattleHp) {
		return false;
	}
	if (currentHp > maxHp) {
		return false;
	}
	if (HasOverlargePartyNonHpStat(battleWords)) {
		return false;
	}

	return true;
}

static bool DecryptPokemonAtGen5(const PokemonMemoryLayout *layout, uint32_t monOffset, bool boxPokemon, DecryptedPokemon &outPokemon)
{
	if (layout == NULL) {
		return false;
	}

	std::vector<uint8_t> monBytes;
	const size_t monSize = boxPokemon ? layout->boxMonSize : layout->partyMonSize;
	SnapshotArm9Bytes(monOffset, monSize, monBytes);

	const size_t wordCount = (boxPokemon ? layout->boxMonSize : layout->partyMonSize) / 2u;
	outPokemon.words.assign(wordCount, 0);
	outPokemon.pid = ReadSnapshotU32(monBytes, 0x0u);
	outPokemon.checksum = ReadSnapshotU16(monBytes, 0x6u);
	outPokemon.checksumValid = false;

	for (int byteOffset = 0; byteOffset <= 0x6; byteOffset += 2) {
		outPokemon.words[byteOffset / 2] = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
	}

	const int coreEndOffset = static_cast<int>(0x8u + layout->coreBytes - 2u);
	uint32_t seed = outPokemon.checksum;
	for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
		seed = AdvanceLcrng(seed);
		outPokemon.words[byteOffset / 2] = static_cast<uint16_t>(ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset)) ^ (seed >> 16));
	}

	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
		checksum = (checksum + outPokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	if (!boxPokemon) {
		const uint32_t tailStart = static_cast<uint32_t>(0x8u + layout->coreBytes);
		const size_t tailWordCount = (layout->partyMonSize - tailStart) / 2u;
		std::vector<uint16_t> rawTailWords(tailWordCount, 0);
		std::vector<uint16_t> decryptedTailWords(tailWordCount, 0);
		uint32_t tailSeed = outPokemon.pid;
		for (uint32_t byteOffset = tailStart; byteOffset < layout->partyMonSize; byteOffset += 2u) {
			const size_t localIndex = static_cast<size_t>((byteOffset - tailStart) / 2u);
			const uint16_t rawWord = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
			rawTailWords[localIndex] = rawWord;

			tailSeed = AdvanceLcrng(tailSeed);
			decryptedTailWords[localIndex] = static_cast<uint16_t>(rawWord ^ (tailSeed >> 16));
		}

		const bool decryptedTailLooksReasonable = IsReasonableGen5PartyTailWords(decryptedTailWords);
		const bool rawTailLooksReasonable = IsReasonableGen5PartyTailWords(rawTailWords);
		const std::vector<uint16_t> &selectedTailWords =
			(decryptedTailLooksReasonable || !rawTailLooksReasonable) ? decryptedTailWords : rawTailWords;

		for (uint32_t byteOffset = tailStart; byteOffset < layout->partyMonSize; byteOffset += 2u) {
			const size_t localIndex = static_cast<size_t>((byteOffset - tailStart) / 2u);
			outPokemon.words[byteOffset / 2u] = selectedTailWords[localIndex];
		}
	}

	outPokemon.checksumValid = (checksum == outPokemon.checksum);
	return outPokemon.checksumValid;
}

static bool TryDecryptPokemonWithRetriesGen5(const PokemonMemoryLayout *layout, uint32_t monOffset, bool boxPokemon, DecryptedPokemon &outPokemon)
{
	for (int attempt = 0; attempt < kDecryptRetryCount; ++attempt) {
		if (DecryptPokemonAtGen5(layout, monOffset, boxPokemon, outPokemon)) {
			return true;
		}
	}

	return false;
}

static bool ReadAlreadyDecryptedPartyPokemonGen5(const PokemonMemoryLayout *layout, uint32_t monOffset, DecryptedPokemon &outPokemon)
{
	if (layout == NULL) {
		return false;
	}

	std::vector<uint8_t> monBytes;
	SnapshotArm9Bytes(monOffset, layout->partyMonSize, monBytes);

	outPokemon.words.assign(layout->partyMonSize / 2u, 0);
	outPokemon.pid = ReadSnapshotU32(monBytes, 0x0u);
	outPokemon.checksum = ReadSnapshotU16(monBytes, 0x6u);
	outPokemon.checksumValid = false;

	for (uint32_t byteOffset = 0; byteOffset < layout->partyMonSize; byteOffset += 2u) {
		outPokemon.words[byteOffset / 2u] = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
	}

	const int coreEndOffset = static_cast<int>(0x8u + layout->coreBytes - 2u);
	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
		checksum = (checksum + outPokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	outPokemon.checksumValid = (checksum == outPokemon.checksum);
	return outPokemon.checksumValid;
}

static bool ReadPartyPokemonWithModesGen5(
	const PokemonMemoryLayout *layout,
	uint32_t monOffset,
	bool allowStorageRawFallback,
	bool allowBattleRawFallback,
	DecryptedPokemon &outPokemon,
	PartyPokemonReadMode &modeOut);

static bool ReadPokemonWithFallbackGen5(const PokemonMemoryLayout *layout, uint32_t monOffset, bool boxPokemon, bool allowPreDecryptedFallback, DecryptedPokemon &outPokemon)
{
	if (!boxPokemon) {
		PartyPokemonReadMode mode;
		return ReadPartyPokemonWithModesGen5(layout, monOffset, allowPreDecryptedFallback, allowPreDecryptedFallback, outPokemon, mode);
	}

	if (TryDecryptPokemonWithRetriesGen5(layout, monOffset, boxPokemon, outPokemon)) {
		return true;
	}

	if (!boxPokemon && allowPreDecryptedFallback) {
		return ReadAlreadyDecryptedPartyPokemonGen5(layout, monOffset, outPokemon);
	}

	return false;
}

static bool ReadPartyPokemonWithModesGen5(
	const PokemonMemoryLayout *layout,
	uint32_t monOffset,
	bool allowStorageRawFallback,
	bool allowBattleRawFallback,
	DecryptedPokemon &outPokemon,
	PartyPokemonReadMode &modeOut)
{
	if (layout == NULL) {
		return false;
	}

	std::vector<uint8_t> monBytes;
	SnapshotArm9Bytes(monOffset, layout->partyMonSize, monBytes);

	outPokemon.words.assign(layout->partyMonSize / 2u, 0);
	outPokemon.pid = ReadSnapshotU32(monBytes, 0x0u);
	outPokemon.checksum = ReadSnapshotU16(monBytes, 0x6u);
	outPokemon.checksumValid = false;

	for (int byteOffset = 0; byteOffset <= 0x6; byteOffset += 2) {
		outPokemon.words[byteOffset / 2] = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
	}

	std::vector<uint16_t> rawCoreWords(layout->coreBytes / 2u, 0);
	std::vector<uint16_t> decryptedCoreWords(layout->coreBytes / 2u, 0);
	uint32_t checksumRawCore = 0;
	uint32_t checksumDecryptedCore = 0;
	uint32_t coreSeed = outPokemon.checksum;
	const int coreEndOffset = static_cast<int>(0x8u + layout->coreBytes - 2u);
	for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
		const size_t localIndex = static_cast<size_t>((byteOffset - 0x8) / 2);
		const uint16_t rawWord = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
		rawCoreWords[localIndex] = rawWord;
		checksumRawCore = (checksumRawCore + rawWord) & 0xFFFFu;

		coreSeed = AdvanceLcrng(coreSeed);
		const uint16_t decryptedWord = static_cast<uint16_t>(rawWord ^ (coreSeed >> 16));
		decryptedCoreWords[localIndex] = decryptedWord;
		checksumDecryptedCore = (checksumDecryptedCore + decryptedWord) & 0xFFFFu;
	}

	if (checksumDecryptedCore == outPokemon.checksum) {
		modeOut.storageMode = PARTY_MEMORY_MODE_ENCRYPTED;
		for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
			const size_t localIndex = static_cast<size_t>((byteOffset - 0x8) / 2);
			outPokemon.words[byteOffset / 2] = decryptedCoreWords[localIndex];
		}
	} else if (allowStorageRawFallback && checksumRawCore == outPokemon.checksum) {
		modeOut.storageMode = PARTY_MEMORY_MODE_ALREADY_DECRYPTED;
		for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
			const size_t localIndex = static_cast<size_t>((byteOffset - 0x8) / 2);
			outPokemon.words[byteOffset / 2] = rawCoreWords[localIndex];
		}
	} else {
		return false;
	}

	const uint32_t tailStart = static_cast<uint32_t>(0x8u + layout->coreBytes);
	const size_t tailWordCount = (layout->partyMonSize - tailStart) / 2u;
	std::vector<uint16_t> rawTailWords(tailWordCount, 0);
	std::vector<uint16_t> decryptedTailWords(tailWordCount, 0);
	uint32_t tailSeed = outPokemon.pid;
	for (uint32_t byteOffset = tailStart; byteOffset < layout->partyMonSize; byteOffset += 2u) {
		const size_t localIndex = static_cast<size_t>((byteOffset - tailStart) / 2u);
		const uint16_t rawWord = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
		rawTailWords[localIndex] = rawWord;

		tailSeed = AdvanceLcrng(tailSeed);
		decryptedTailWords[localIndex] = static_cast<uint16_t>(rawWord ^ (tailSeed >> 16));
	}

	const bool decryptedTailLooksReasonable = IsReasonableGen5PartyTailWords(decryptedTailWords);
	const bool rawTailLooksReasonable = IsReasonableGen5PartyTailWords(rawTailWords);
	const bool preferRawTail = !decryptedTailLooksReasonable &&
	                          !rawTailLooksReasonable &&
	                          ShouldDeferToRawPartyBattleTail(rawTailWords, decryptedTailWords);
	if ((rawTailLooksReasonable || preferRawTail) && !allowBattleRawFallback && !decryptedTailLooksReasonable) {
		return false;
	}
	if (decryptedTailLooksReasonable && (!rawTailLooksReasonable || !allowBattleRawFallback)) {
		modeOut.battleMode = PARTY_MEMORY_MODE_ENCRYPTED;
		for (uint32_t byteOffset = tailStart; byteOffset < layout->partyMonSize; byteOffset += 2u) {
			const size_t localIndex = static_cast<size_t>((byteOffset - tailStart) / 2u);
			outPokemon.words[byteOffset / 2u] = decryptedTailWords[localIndex];
		}
	} else if (allowBattleRawFallback && (rawTailLooksReasonable || preferRawTail)) {
		modeOut.battleMode = PARTY_MEMORY_MODE_ALREADY_DECRYPTED;
		for (uint32_t byteOffset = tailStart; byteOffset < layout->partyMonSize; byteOffset += 2u) {
			const size_t localIndex = static_cast<size_t>((byteOffset - tailStart) / 2u);
			outPokemon.words[byteOffset / 2u] = rawTailWords[localIndex];
		}
	} else {
		return false;
	}

	outPokemon.checksumValid = true;
	return true;
}

static bool IsReasonableBattleTailWords(const std::vector<uint16_t> &battleWords)
{
	if (battleWords.size() < ((0xEA - 0x88) / 2 + 1)) {
		return false;
	}

	const int level = static_cast<int>(battleWords[2] & 0x00FFu);
	const int currentHp = static_cast<int>(battleWords[3]);
	const int maxHp = static_cast<int>(battleWords[4]);

	if (level < 1 || level > 100) {
		return false;
	}
	if (currentHp < 0 || currentHp > kMaxReasonablePartyBattleHp) {
		return false;
	}
	if (maxHp < 1 || maxHp > kMaxReasonablePartyBattleHp) {
		return false;
	}
	if (currentHp > maxHp) {
		return false;
	}
	if (HasOverlargePartyNonHpStat(battleWords)) {
		return false;
	}

	return true;
}

static bool ReadPartyPokemonWithModes(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	bool allowStorageRawFallback,
	bool allowBattleRawFallback,
	DecryptedPokemon &outPokemon,
	PartyPokemonReadMode &modeOut)
{
	std::vector<uint8_t> monBytes;
	SnapshotArm9Bytes(monOffset, kPartyMonSize, monBytes);

	outPokemon.words.assign(kPartyMonSize / 2, 0);
	outPokemon.pid = ReadSnapshotU32(monBytes, 0x0u);
	outPokemon.checksum = ReadSnapshotU16(monBytes, 0x6u);
	outPokemon.checksumValid = false;

	for (int byteOffset = 0; byteOffset <= 0x6; byteOffset += 2) {
		outPokemon.words[byteOffset / 2] = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
	}

	std::vector<uint16_t> rawCoreWords(kCoreBytes / 2, 0);
	std::vector<uint16_t> decryptedCoreWords(kCoreBytes / 2, 0);
	uint32_t checksumRawCore = 0;
	uint32_t checksumDecryptedCore = 0;
	uint32_t coreSeed = outPokemon.checksum;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		const size_t localIndex = static_cast<size_t>((byteOffset - 0x8) / 2);
		const uint16_t rawWord = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
		rawCoreWords[localIndex] = rawWord;
		checksumRawCore = (checksumRawCore + rawWord) & 0xFFFFu;

		coreSeed = AdvanceLcrng(coreSeed);
		const uint16_t decryptedWord = static_cast<uint16_t>(rawWord ^ (coreSeed >> 16));
		decryptedCoreWords[localIndex] = decryptedWord;
		checksumDecryptedCore = (checksumDecryptedCore + decryptedWord) & 0xFFFFu;
	}

	std::vector<uint16_t> rawBattleWords(kBattleStatsBytes / 2, 0);
	std::vector<uint16_t> decryptedBattleWords(kBattleStatsBytes / 2, 0);
	uint32_t battleSeed = outPokemon.pid;
	for (int byteOffset = 0x88; byteOffset <= 0xEA; byteOffset += 2) {
		const size_t localIndex = static_cast<size_t>((byteOffset - 0x88) / 2);
		const uint16_t rawWord = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
		rawBattleWords[localIndex] = rawWord;

		battleSeed = AdvanceLcrng(battleSeed);
		decryptedBattleWords[localIndex] = static_cast<uint16_t>(rawWord ^ (battleSeed >> 16));
	}

	const bool decryptedBattleLooksReasonable = IsReasonableBattleTailWords(decryptedBattleWords);
	const bool rawBattleLooksReasonable = IsReasonableBattleTailWords(rawBattleWords);
	const bool preferRawBattle = !decryptedBattleLooksReasonable &&
	                            !rawBattleLooksReasonable &&
	                            ShouldDeferToRawPartyBattleTail(rawBattleWords, decryptedBattleWords);
	const std::vector<uint16_t> *selectedBattleWords = NULL;
	if ((rawBattleLooksReasonable || preferRawBattle) && !allowBattleRawFallback && !decryptedBattleLooksReasonable) {
		return false;
	}
	if (decryptedBattleLooksReasonable && (!rawBattleLooksReasonable || !allowBattleRawFallback)) {
		selectedBattleWords = &decryptedBattleWords;
	} else if (allowBattleRawFallback && (rawBattleLooksReasonable || preferRawBattle)) {
		selectedBattleWords = &rawBattleWords;
	} else {
		return false;
	}

	bool storageChecksumAccepted = true;
	if (checksumDecryptedCore == outPokemon.checksum) {
		modeOut.storageMode = PARTY_MEMORY_MODE_ENCRYPTED;
		for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
			const size_t localIndex = static_cast<size_t>((byteOffset - 0x8) / 2);
			outPokemon.words[byteOffset / 2] = decryptedCoreWords[localIndex];
		}
	} else if (allowStorageRawFallback && checksumRawCore == outPokemon.checksum) {
		modeOut.storageMode = PARTY_MEMORY_MODE_ALREADY_DECRYPTED;
		for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
			const size_t localIndex = static_cast<size_t>((byteOffset - 0x8) / 2);
			outPokemon.words[byteOffset / 2] = rawCoreWords[localIndex];
		}
	} else {
		if (!allowStorageRawFallback || selectedBattleWords == NULL) {
			return false;
		}

		modeOut.storageMode = PARTY_MEMORY_MODE_ALREADY_DECRYPTED;
		for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
			const size_t localIndex = static_cast<size_t>((byteOffset - 0x8) / 2);
			outPokemon.words[byteOffset / 2] = rawCoreWords[localIndex];
		}
		if (!PartyCoreLevelMatchesTailForProfile(profile, outPokemon, *selectedBattleWords)) {
			modeOut.storageMode = PARTY_MEMORY_MODE_ENCRYPTED;
			for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
				const size_t localIndex = static_cast<size_t>((byteOffset - 0x8) / 2);
				outPokemon.words[byteOffset / 2] = decryptedCoreWords[localIndex];
			}
			if (!PartyCoreLevelMatchesTailForProfile(profile, outPokemon, *selectedBattleWords)) {
				return false;
			}
		}
		storageChecksumAccepted = false;
	}

	if (decryptedBattleLooksReasonable && (!rawBattleLooksReasonable || !allowBattleRawFallback)) {
		modeOut.battleMode = PARTY_MEMORY_MODE_ENCRYPTED;
		for (int byteOffset = 0x88; byteOffset <= 0xEA; byteOffset += 2) {
			const size_t localIndex = static_cast<size_t>((byteOffset - 0x88) / 2);
			outPokemon.words[byteOffset / 2] = decryptedBattleWords[localIndex];
		}
	} else if (allowBattleRawFallback && (rawBattleLooksReasonable || preferRawBattle)) {
		modeOut.battleMode = PARTY_MEMORY_MODE_ALREADY_DECRYPTED;
		for (int byteOffset = 0x88; byteOffset <= 0xEA; byteOffset += 2) {
			const size_t localIndex = static_cast<size_t>((byteOffset - 0x88) / 2);
			outPokemon.words[byteOffset / 2] = rawBattleWords[localIndex];
		}
	} else {
		return false;
	}

	outPokemon.checksumValid = storageChecksumAccepted;
	return true;
}

static void WritePartyPokemonWithModes(uint32_t monOffset, DecryptedPokemon &pokemon, const PartyPokemonReadMode &mode);
static void WriteEncryptedBoxPokemon(uint32_t monOffset, DecryptedPokemon &pokemon);
static void WritePartyPokemonWithModesGen5(const PokemonMemoryLayout *layout, uint32_t monOffset, DecryptedPokemon &pokemon, const PartyPokemonReadMode &mode);
static void WriteEncryptedBoxPokemonGen5(const PokemonMemoryLayout *layout, uint32_t monOffset, DecryptedPokemon &pokemon);

static bool ReadPokemonWithFallbackForProfile(const ExportProfileDefinition *profile, uint32_t monOffset, bool boxPokemon, bool allowPreDecryptedFallback, DecryptedPokemon &outPokemon)
{
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		return false;
	}

	switch (layout->codec) {
		case POKEMON_DATA_CODEC_GEN4:
			return ReadPokemonWithFallback(monOffset, boxPokemon, allowPreDecryptedFallback, outPokemon);
		case POKEMON_DATA_CODEC_GEN5:
			return ReadPokemonWithFallbackGen5(layout, monOffset, boxPokemon, allowPreDecryptedFallback, outPokemon);
		case POKEMON_DATA_CODEC_NONE:
		default:
			return false;
	}
}

static bool ReadPartyPokemonWithModesForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	bool allowStorageRawFallback,
	bool allowBattleRawFallback,
	DecryptedPokemon &outPokemon,
	PartyPokemonReadMode &modeOut)
{
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		return false;
	}

	switch (layout->codec) {
		case POKEMON_DATA_CODEC_GEN4:
			return ReadPartyPokemonWithModes(profile, monOffset, allowStorageRawFallback, allowBattleRawFallback, outPokemon, modeOut);
		case POKEMON_DATA_CODEC_GEN5:
			(void)allowBattleRawFallback;
			return ReadPartyPokemonWithModesGen5(layout, monOffset, allowStorageRawFallback, allowBattleRawFallback, outPokemon, modeOut);
		case POKEMON_DATA_CODEC_NONE:
		default:
			return false;
	}
}

static bool ReadPartyPokemonRobustForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	DecryptedPokemon &outPokemon,
	PartyPokemonReadMode &modeOut)
{
	static const bool kFallbackCombos[4][2] = {
		{false, false},
		{true, false},
		{false, true},
		{true, true}
	};

	for (size_t comboIndex = 0; comboIndex < 4u; ++comboIndex) {
		if (ReadPartyPokemonWithModesForProfile(
				profile,
				monOffset,
				kFallbackCombos[comboIndex][0],
				kFallbackCombos[comboIndex][1],
				outPokemon,
				modeOut)) {
			return true;
		}
	}

	return false;
}

static bool IsTransientPlaintextPartyFrameForProfile(
	const ExportProfileDefinition *profile,
	const DecryptedPokemon &pokemon,
	const PartyPokemonReadMode &mode)
{
	return !pokemon.checksumValid &&
		IsPlausiblePartyCoreForProfile(profile, pokemon) &&
		!HasAnyPartyMoveIdAboveMax(pokemon) &&
		!AreAllPartyMoveSlotsDisplayEmpty(profile, pokemon) &&
		mode.storageMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED &&
		mode.battleMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED;
}

static bool HasDisplayablePartyMovesForProfile(
	const ExportProfileDefinition *profile,
	const DecryptedPokemon &pokemon)
{
	return !HasAnyPartyMoveIdAboveMax(pokemon) &&
		!AreAllPartyMoveSlotsDisplayEmpty(profile, pokemon);
}

static void AnalyzePartyPokemonFrameForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	PartyFrameAnalysis &analysisOut)
{
	analysisOut.classification = PARTY_FRAME_INVALID;
	analysisOut.hasDecodedPokemon = false;
	analysisOut.pokemon.words.clear();
	analysisOut.pokemon.pid = 0u;
	analysisOut.pokemon.checksum = 0u;
	analysisOut.pokemon.checksumValid = false;
	analysisOut.readMode.storageMode = PARTY_MEMORY_MODE_ENCRYPTED;
	analysisOut.readMode.battleMode = PARTY_MEMORY_MODE_ENCRYPTED;

	static const bool kFallbackCombos[4][2] = {
		{false, false},
		{true, false},
		{false, true},
		{true, true}
	};

	bool sawHybridRawCoreCandidate = false;
	bool sawHybridEncryptedMoveCandidate = false;
	DecryptedPokemon hybridReportPokemon;
	PartyPokemonReadMode hybridReportMode;
	bool hybridHasReportPokemon = false;

	for (size_t comboIndex = 0; comboIndex < 4u; ++comboIndex) {
		DecryptedPokemon candidate;
		PartyPokemonReadMode candidateMode;
		if (!ReadPartyPokemonWithModesForProfile(
				profile,
				monOffset,
				kFallbackCombos[comboIndex][0],
				kFallbackCombos[comboIndex][1],
				candidate,
				candidateMode)) {
			continue;
		}

		if (!analysisOut.hasDecodedPokemon) {
			analysisOut.pokemon = candidate;
			analysisOut.readMode = candidateMode;
			analysisOut.hasDecodedPokemon = true;
		}

		if (IsPartyPokemonValidFrameForWrite(profile, candidate)) {
			analysisOut.classification = PARTY_FRAME_VALID;
			analysisOut.pokemon = candidate;
			analysisOut.readMode = candidateMode;
			analysisOut.hasDecodedPokemon = true;
			return;
		}

		if (analysisOut.classification != PARTY_FRAME_TRANSIENT_PLAINTEXT &&
			IsTransientPlaintextPartyFrameForProfile(profile, candidate, candidateMode)) {
			analysisOut.classification = PARTY_FRAME_TRANSIENT_PLAINTEXT;
			analysisOut.pokemon = candidate;
			analysisOut.readMode = candidateMode;
			analysisOut.hasDecodedPokemon = true;
		}

		if (!candidate.checksumValid &&
			candidateMode.storageMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED &&
			candidateMode.battleMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED &&
			IsPlausiblePartyCoreForProfile(profile, candidate)) {
			sawHybridRawCoreCandidate = true;
			if (!hybridHasReportPokemon) {
				hybridReportPokemon = candidate;
				hybridReportMode = candidateMode;
				hybridHasReportPokemon = true;
			}
		}

		if (!candidate.checksumValid &&
			candidateMode.storageMode == PARTY_MEMORY_MODE_ENCRYPTED &&
			candidateMode.battleMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED &&
			HasDisplayablePartyMovesForProfile(profile, candidate)) {
			sawHybridEncryptedMoveCandidate = true;
			if (!hybridHasReportPokemon) {
				hybridReportPokemon = candidate;
				hybridReportMode = candidateMode;
				hybridHasReportPokemon = true;
			}
		}
	}

	if (analysisOut.classification != PARTY_FRAME_TRANSIENT_PLAINTEXT &&
		sawHybridRawCoreCandidate &&
		sawHybridEncryptedMoveCandidate) {
		analysisOut.classification = PARTY_FRAME_TRANSIENT_PLAINTEXT;
		if (hybridHasReportPokemon) {
			analysisOut.pokemon = hybridReportPokemon;
			analysisOut.readMode = hybridReportMode;
			analysisOut.hasDecodedPokemon = true;
		}
	}
}

static bool ReadPartyPokemonStrictForWriteForProfileWithAttempts(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	DecryptedPokemon &outPokemon,
	PartyPokemonReadMode &modeOut,
	int attemptCount,
	int retryDelayMs,
	PartyFrameAnalysis *lastAnalysisOut)
{
	PartyFrameAnalysis lastAnalysis;
	lastAnalysis.classification = PARTY_FRAME_INVALID;
	lastAnalysis.hasDecodedPokemon = false;

	for (int attempt = 0; attempt < attemptCount; ++attempt) {
		PartyFrameAnalysis analysis;
		AnalyzePartyPokemonFrameForProfile(profile, monOffset, analysis);
		lastAnalysis = analysis;
		if (analysis.classification == PARTY_FRAME_VALID) {
			outPokemon = analysis.pokemon;
			modeOut = analysis.readMode;
			if (lastAnalysisOut != NULL) {
				*lastAnalysisOut = analysis;
			}
			return true;
		}

		if ((attempt + 1) < attemptCount && retryDelayMs > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
		}
	}

	if (lastAnalysisOut != NULL) {
		*lastAnalysisOut = lastAnalysis;
	}
	return false;
}

static bool ReadPartyPokemonStrictForWriteForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	DecryptedPokemon &outPokemon,
	PartyPokemonReadMode &modeOut)
{
	return ReadPartyPokemonStrictForWriteForProfileWithAttempts(
		profile,
		monOffset,
		outPokemon,
		modeOut,
		kPartyWriteReadRetryCount,
		kPartyWriteReadRetryDelayMs,
		NULL);
}

static std::string DescribePartyReadFailureContextForReport(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	const char *baseContext,
	const PartyFrameAnalysis *analysis,
	DecryptedPokemon &reportPokemonStorage,
	const DecryptedPokemon *&reportPokemonOut)
{
	std::string context = (baseContext != NULL) ? baseContext : "party_read";
	reportPokemonOut = NULL;

	if (analysis != NULL && analysis->hasDecodedPokemon) {
		reportPokemonStorage = analysis->pokemon;
		reportPokemonOut = &reportPokemonStorage;
	}

	if (analysis != NULL && analysis->classification == PARTY_FRAME_TRANSIENT_PLAINTEXT) {
		return context + "_transient_plaintext_timeout";
	}

	DecryptedPokemon failedPokemon;
	PartyPokemonReadMode failedReadMode;
	if (ReadPartyPokemonRobustForProfile(profile, monOffset, failedPokemon, failedReadMode)) {
		reportPokemonStorage = failedPokemon;
		reportPokemonOut = &reportPokemonStorage;
		if (!failedPokemon.checksumValid) {
			return context + "_invalid_checksum";
		}
		if (AreAllPartyMoveSlotsDisplayEmpty(profile, failedPokemon)) {
			return context + "_empty_moves";
		}
		if (HasAnyPartyMoveIdAboveMax(failedPokemon)) {
			return context + "_move_id_over_700";
		}
	}

	return context;
}

static bool TryDecryptPokemonWithRetriesForProfile(const ExportProfileDefinition *profile, uint32_t monOffset, bool boxPokemon, DecryptedPokemon &outPokemon)
{
	return ReadPokemonWithFallbackForProfile(profile, monOffset, boxPokemon, true, outPokemon);
}

static bool ReadBoxPokemonStrictForEditorForProfileWithAttempts(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	DecryptedPokemon &outPokemon,
	int attemptCount,
	int retryDelayMs)
{
	for (int attempt = 0; attempt < attemptCount; ++attempt) {
		DecryptedPokemon candidate;
		if (TryDecryptPokemonWithRetriesForProfile(profile, monOffset, true, candidate) &&
			IsPokemonValidEditorFrameForProfile(profile, candidate)) {
			outPokemon = candidate;
			return true;
		}

		if ((attempt + 1) < attemptCount && retryDelayMs > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
		}
	}

	return false;
}

static bool ReadBoxPokemonStrictForEditorForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	DecryptedPokemon &outPokemon)
{
	return ReadBoxPokemonStrictForEditorForProfileWithAttempts(
		profile,
		monOffset,
		outPokemon,
		kPartyWriteReadRetryCount,
		kPartyWriteReadRetryDelayMs);
}

static void WritePartyPokemonWithModesForProfile(const ExportProfileDefinition *profile, uint32_t monOffset, DecryptedPokemon &pokemon, const PartyPokemonReadMode &mode)
{
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		return;
	}

	switch (layout->codec) {
		case POKEMON_DATA_CODEC_GEN4:
			WritePartyPokemonWithModes(monOffset, pokemon, mode);
			return;
		case POKEMON_DATA_CODEC_GEN5:
			WritePartyPokemonWithModesGen5(layout, monOffset, pokemon, mode);
			return;
		case POKEMON_DATA_CODEC_NONE:
		default:
			return;
	}
}

static void WriteEncryptedBoxPokemonForProfile(const ExportProfileDefinition *profile, uint32_t monOffset, DecryptedPokemon &pokemon)
{
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		return;
	}

	switch (layout->codec) {
		case POKEMON_DATA_CODEC_GEN4:
			WriteEncryptedBoxPokemon(monOffset, pokemon);
			return;
		case POKEMON_DATA_CODEC_GEN5:
			WriteEncryptedBoxPokemonGen5(layout, monOffset, pokemon);
			return;
		case POKEMON_DATA_CODEC_NONE:
		default:
			return;
	}
}

static bool ReadBackPartyStatusRawForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	uint32_t &statusRawOut)
{
	DecryptedPokemon verifyPokemon;
	PartyPokemonReadMode verifyMode;
	if (!ReadPartyPokemonStrictForWriteForProfile(profile, monOffset, verifyPokemon, verifyMode)) {
		return false;
	}

	statusRawOut = GetPartyStatusRawFromPokemon(verifyPokemon);
	return true;
}

static bool ReadBackPartyHpStatusForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	int &currentHpOut,
	uint32_t &statusRawOut)
{
	DecryptedPokemon verifyPokemon;
	PartyPokemonReadMode verifyMode;
	if (!ReadPartyPokemonStrictForWriteForProfile(profile, monOffset, verifyPokemon, verifyMode)) {
		return false;
	}

	currentHpOut = static_cast<int>(GetWord(verifyPokemon, 0x8E));
	statusRawOut = GetPartyStatusRawFromPokemon(verifyPokemon);
	return true;
}

static bool WriteVerifiedPartyPokemonForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	int slot,
	DecryptedPokemon &pokemon,
	const PartyPokemonReadMode &readMode,
	uint32_t originalStatusRaw,
	uint32_t expectedStatusRaw,
	std::string &errorOut)
{
	WritePartyPokemonWithModesForProfile(profile, monOffset, pokemon, readMode);

	uint32_t verifiedStatusRaw = 0u;
	if (ReadBackPartyStatusRawForProfile(profile, monOffset, verifiedStatusRaw) &&
	    verifiedStatusRaw == expectedStatusRaw) {
		errorOut.clear();
		return true;
	}

	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	const bool allowGen4BattleModeRetry =
		(layout != NULL) &&
		(layout->codec == POKEMON_DATA_CODEC_GEN4);
	if (!allowGen4BattleModeRetry) {
		errorOut = "Party status write verification failed for slot " + std::to_string(slot);
		return false;
	}

	PartyPokemonReadMode retryMode = readMode;
	retryMode.battleMode = (readMode.battleMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED)
		? PARTY_MEMORY_MODE_ENCRYPTED
		: PARTY_MEMORY_MODE_ALREADY_DECRYPTED;

	SetPartyStatusRawOnPokemon(pokemon, originalStatusRaw);
	WritePartyPokemonWithModesForProfile(profile, monOffset, pokemon, retryMode);
	SetPartyStatusRawOnPokemon(pokemon, expectedStatusRaw);
	WritePartyPokemonWithModesForProfile(profile, monOffset, pokemon, retryMode);

	if (ReadBackPartyStatusRawForProfile(profile, monOffset, verifiedStatusRaw) &&
	    verifiedStatusRaw == expectedStatusRaw) {
		errorOut.clear();
		return true;
	}

	errorOut = "Party status write verification failed for slot " + std::to_string(slot);
	return false;
}

static bool WriteVerifiedPartyHpStatusForProfile(
	const ExportProfileDefinition *profile,
	uint32_t monOffset,
	int slot,
	DecryptedPokemon &pokemon,
	const PartyPokemonReadMode &readMode,
	int originalCurrentHp,
	uint32_t originalStatusRaw,
	int expectedCurrentHp,
	uint32_t expectedStatusRaw,
	std::string &errorOut)
{
	WritePartyPokemonWithModesForProfile(profile, monOffset, pokemon, readMode);

	int verifiedCurrentHp = 0;
	uint32_t verifiedStatusRaw = 0u;
	if (ReadBackPartyHpStatusForProfile(profile, monOffset, verifiedCurrentHp, verifiedStatusRaw) &&
	    verifiedCurrentHp == expectedCurrentHp &&
	    verifiedStatusRaw == expectedStatusRaw) {
		errorOut.clear();
		return true;
	}

	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	const bool allowGen4BattleModeRetry =
		(layout != NULL) &&
		(layout->codec == POKEMON_DATA_CODEC_GEN4);
	if (!allowGen4BattleModeRetry) {
		errorOut = "Party HP/status write verification failed for slot " + std::to_string(slot);
		return false;
	}

	PartyPokemonReadMode retryMode = readMode;
	retryMode.battleMode = (readMode.battleMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED)
		? PARTY_MEMORY_MODE_ENCRYPTED
		: PARTY_MEMORY_MODE_ALREADY_DECRYPTED;

	pokemon.words[0x8E / 2] = static_cast<uint16_t>(originalCurrentHp);
	SetPartyStatusRawOnPokemon(pokemon, originalStatusRaw);
	WritePartyPokemonWithModesForProfile(profile, monOffset, pokemon, retryMode);
	pokemon.words[0x8E / 2] = static_cast<uint16_t>(expectedCurrentHp);
	SetPartyStatusRawOnPokemon(pokemon, expectedStatusRaw);
	WritePartyPokemonWithModesForProfile(profile, monOffset, pokemon, retryMode);

	if (ReadBackPartyHpStatusForProfile(profile, monOffset, verifiedCurrentHp, verifiedStatusRaw) &&
	    verifiedCurrentHp == expectedCurrentHp &&
	    verifiedStatusRaw == expectedStatusRaw) {
		errorOut.clear();
		return true;
	}

	errorOut = "Party HP/status write verification failed for slot " + std::to_string(slot);
	return false;
}

static uint8_t GetBattlePartySlotForBattler(const BattleMemoryLayout &layout, uint32_t battleCtxAddr, uint8_t battler)
{
	const uint8_t selectedPartySlot = ReadArm9U8(battleCtxAddr + layout.battleContextSelectedPartySlotOffset + battler);
	if (selectedPartySlot <= 5u) {
		return selectedPartySlot;
	}

	if ((battler & 0x1u) != 0u) {
		return (battler == 3u) ? 1u : 0u;
	}

	return battler / 2u;
}

static void BuildBattleLogPartySnapshotFromAbsoluteParty(
	const ExportProfileDefinition *profile,
	uint32_t partyCountAddr,
	uint32_t partyBaseAddr,
	std::vector<BattleLogPartyEntry> &outParty,
	uint8_t &highestLevelOut)
{
	outParty.clear();
	highestLevelOut = 0;
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if ((partyCountAddr == 0u) || (partyBaseAddr == 0u) || layout == NULL) {
		return;
	}

	const uint8_t partyCount = static_cast<uint8_t>(std::min<uint32_t>(ReadArm9U8(partyCountAddr), 6u));
	for (uint8_t slot = 0; slot < partyCount; ++slot) {
		const uint32_t monOffset = partyBaseAddr + (static_cast<uint32_t>(slot) * static_cast<uint32_t>(layout->partyMonSize));
		DecryptedPokemon pokemon;
		PartyPokemonReadMode readMode;
		if (!ReadPartyPokemonRobustForProfile(profile, monOffset, pokemon, readMode)) {
			continue;
		}

		const uint32_t pid = pokemon.pid;
		const int shuffleKey = static_cast<int>(((pid & 0x3E000u) >> 13) % 24u);
		const int blockA = kShuffleTable[shuffleKey][0];
		const int blockB = kShuffleTable[shuffleKey][1];
		const int speciesId = GetWord(pokemon, blockA + 0x0);
		const int heldItemId = GetWord(pokemon, blockA + 0x2);
		const int abilityId = static_cast<int>(GetWord(pokemon, blockA + 0xC) >> 8);
		const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
			(static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);

			BattleLogPartyEntry entry;
			entry.slot = static_cast<uint8_t>(slot + 1u);
			entry.pid = pid;
			entry.species = static_cast<uint16_t>(speciesId);
			entry.heldItem = static_cast<uint16_t>(heldItemId);
			entry.natureId = static_cast<uint8_t>(pid % 25u);
			entry.abilitySlot = 1u;
			entry.ability = static_cast<uint16_t>(abilityId);
		for (int moveIndex = 0; moveIndex < 4; ++moveIndex) {
			entry.moves[moveIndex] = static_cast<uint16_t>(GetWord(pokemon, blockB + (moveIndex * 2)));
		}
		outParty.push_back(entry);

		const int level = GetProfileLevelForExperience(profile, experience, speciesId);
		if ((level > 0) && (level <= 100) && (level > static_cast<int>(highestLevelOut))) {
			highestLevelOut = static_cast<uint8_t>(level);
		}
	}
}

static void BuildBattleLogPartySnapshot(const ExportProfileDefinition *profile, std::vector<BattleLogPartyEntry> &outParty, uint8_t &highestLevelOut)
{
	PokemonRuntimeAddresses addresses;
	std::string error;
	if (!ResolvePokemonRuntimeAddresses(profile, addresses, error)) {
		outParty.clear();
		highestLevelOut = 0;
		return;
	}

	BuildBattleLogPartySnapshotFromAbsoluteParty(profile, addresses.partyCountAddr, addresses.partyBaseAddr, outParty, highestLevelOut);
}

static void BuildBattleMonRuntimeSnapshot(const BattleMemoryLayout &layout, uint32_t battleCtxAddr, std::vector<BattleMonRuntimeSnapshot> &outMons)
{
	outMons.assign(4, BattleMonRuntimeSnapshot());
	if (!IsMainRamAlignedPointer(battleCtxAddr)) {
		return;
	}

	for (uint8_t battler = 0; battler < 4u; ++battler) {
		BattleMonRuntimeSnapshot snapshot;
		snapshot.valid = true;
		snapshot.battler = battler;
		snapshot.partySlot = GetBattlePartySlotForBattler(layout, battleCtxAddr, battler);
		snapshot.pid = 0u;

		const uint32_t monAddr = battleCtxAddr + layout.battleContextBattleMonsOffset + (static_cast<uint32_t>(battler) * layout.battleContextBattleMonSize);
		snapshot.speciesId = ReadArm9U16(monAddr + 0x00u);
		snapshot.curHp = ReadArm9U32(monAddr + 0x4Cu);
		snapshot.maxHp = ReadArm9U32(monAddr + 0x50u);
		outMons[battler] = snapshot;
	}
}

static void BuildBattleMonRuntimeSnapshotForProfile(
	const ExportProfileDefinition *profile,
	const BattleMemoryLayout &layout,
	uint32_t battleSystemAddr,
	uint32_t battleCtxAddr,
	std::vector<BattleMonRuntimeSnapshot> &outMons)
{
	if (!IsGen5BwLikePokemonProfile((profile != NULL) ? profile->profile : EXPORT_GAME_NONE)) {
		BuildBattleMonRuntimeSnapshot(layout, battleCtxAddr, outMons);

		const PokemonMemoryLayout *pokemonLayout = GetProfilePokemonLayout(profile);
		if ((pokemonLayout != NULL) && IsMainRamAlignedPointer(battleSystemAddr)) {
			for (size_t i = 0; i < outMons.size(); ++i) {
				BattleMonRuntimeSnapshot &snapshot = outMons[i];
				if (!snapshot.valid || (snapshot.partySlot >= 6u)) {
					continue;
				}

				const uint32_t partyPtr = ReadArm9U32(battleSystemAddr + layout.battleSystemPartiesOffset + (static_cast<uint32_t>(snapshot.battler) * 4u));
				if (!IsMainRamAlignedPointer(partyPtr)) {
					continue;
				}

				snapshot.sourceMonAddr = partyPtr + 0x8u + (static_cast<uint32_t>(snapshot.partySlot) * static_cast<uint32_t>(pokemonLayout->partyMonSize));
				snapshot.pid = ReadArm9U32(snapshot.sourceMonAddr);
			}
		}
		return;
	}

	outMons.clear();
	if (!IsMainRamAlignedPointer(battleSystemAddr)) {
		return;
	}

	const uint32_t clientPokeConAddr = battleSystemAddr + kBwBattleMainPokeconForClientOffset;
	if (ReadArm9U32(clientPokeConAddr + kBwBattlePokeConMainModuleOffset) != battleSystemAddr) {
		return;
	}

	const uint32_t serverAddr = ReadArm9U32(battleSystemAddr + kBwBattleMainServerOffset);
	if (!IsMainRamAlignedPointer(serverAddr)) {
		return;
	}

	const uint32_t playerPartyAddr = clientPokeConAddr + kBwBattlePokeConPartyOffset;
	const uint32_t enemyPartyAddr = clientPokeConAddr + kBwBattlePokeConPartyOffset + kBwBattlePartySize;
	const uint32_t playerSrcPartyAddr = ReadArm9U32(clientPokeConAddr + kBwBattlePokeConSrcPartyOffset);
	const uint32_t enemySrcPartyAddr = ReadArm9U32(clientPokeConAddr + kBwBattlePokeConSrcPartyOffset + 4u);
	const uint32_t playerClientWorkAddr = serverAddr + kBwBattleServerClientWorkOffset;
	const uint32_t enemyClientWorkAddr = playerClientWorkAddr + kBwBattleServerClientWorkSize;
	const uint8_t playerNumCoverPos = ReadArm9U8(playerClientWorkAddr + 0x09u);
	const uint8_t enemyNumCoverPos = ReadArm9U8(enemyClientWorkAddr + 0x09u);
	const uint8_t frontCount = std::max<uint8_t>(playerNumCoverPos, enemyNumCoverPos);

	for (uint8_t posIndex = 0u; posIndex < frontCount; ++posIndex) {
		const uint32_t playerBppAddr = ReadArm9U32(playerPartyAddr + (static_cast<uint32_t>(posIndex) * 4u));
		const uint32_t enemyBppAddr = ReadArm9U32(enemyPartyAddr + (static_cast<uint32_t>(posIndex) * 4u));
		const uint32_t bppAddrs[2] = {playerBppAddr, enemyBppAddr};
		const uint32_t srcPartyAddrs[2] = {playerSrcPartyAddr, enemySrcPartyAddr};
		for (uint8_t side = 0u; side < 2u; ++side) {
			const uint32_t bppAddr = bppAddrs[side];
			if (!IsMainRamAlignedPointer(bppAddr)) {
				continue;
			}

			BattleMonRuntimeSnapshot snapshot;
			snapshot.valid = true;
			snapshot.battler = static_cast<uint8_t>((posIndex * 2u) + side);
			snapshot.partySlot = posIndex;
			snapshot.pid = 0u;
			snapshot.speciesId = ReadArm9U16(bppAddr + kBwBattlePokeParamSpeciesOffset);
			snapshot.curHp = ReadArm9U16(bppAddr + kBwBattlePokeParamCurHpOffset);
			snapshot.maxHp = ReadArm9U16(bppAddr + kBwBattlePokeParamMaxHpOffset);
			snapshot.sourceMonAddr = 0u;

			const uint32_t ppSrcAddr = ReadArm9U32(bppAddr + kBwBattlePokeParamSourceOffset);
			const uint32_t srcPartyAddr = srcPartyAddrs[side];
			if (IsMainRamAlignedPointer(ppSrcAddr) && IsMainRamAlignedPointer(srcPartyAddr)) {
				const uint32_t firstMonAddr = srcPartyAddr + kBwBattlePokePartyFirstMonOffset;
				if ((ppSrcAddr >= firstMonAddr) && ((ppSrcAddr - firstMonAddr) % static_cast<uint32_t>(kGen5PartyMonSize) == 0u)) {
					const uint32_t slot = (ppSrcAddr - firstMonAddr) / static_cast<uint32_t>(kGen5PartyMonSize);
					if (slot < 6u) {
						snapshot.partySlot = static_cast<uint8_t>(slot);
						snapshot.sourceMonAddr = ppSrcAddr;
						snapshot.pid = ReadArm9U32(ppSrcAddr);
					}
				}
			}

			if ((snapshot.speciesId == 0u) || (snapshot.speciesId > 649u)) {
				continue;
			}

			outMons.push_back(snapshot);
		}
	}
}

static int FindEnemyPartyRuntimeSnapshotIndex(
	const std::vector<BattleLogEnemyPartySnapshot> &mons,
	uint32_t partyPtr,
	uint8_t slot)
{
	for (size_t i = 0; i < mons.size(); ++i) {
		if (mons[i].valid && (mons[i].partyPtr == partyPtr) && (mons[i].slot == slot)) {
			return static_cast<int>(i);
		}
	}

	return -1;
}

static uint8_t GetAdjacentPlayerSlotForEnemyBattler(const std::vector<BattleMonRuntimeSnapshot> &battleMons, uint8_t enemyBattler)
{
	if ((enemyBattler & 0x1u) == 0u || enemyBattler == 0u) {
		return 0xFFu;
	}

	const size_t playerIndex = static_cast<size_t>(enemyBattler - 1u);
	if (playerIndex >= battleMons.size()) {
		return 0xFFu;
	}

	const BattleMonRuntimeSnapshot &playerMon = battleMons[playerIndex];
	if (!playerMon.valid || ((playerMon.battler & 0x1u) != 0u)) {
		return 0xFFu;
	}

	return playerMon.partySlot;
}

static bool TryGetBwSourcePartySlot(uint32_t srcPartyAddr, uint32_t sourceMonAddr, uint8_t &slotOut)
{
	slotOut = 0xFFu;
	if (!IsMainRamAlignedPointer(srcPartyAddr) || !IsMainRamAlignedPointer(sourceMonAddr)) {
		return false;
	}

	const uint32_t firstMonAddr = srcPartyAddr + kBwBattlePokePartyFirstMonOffset;
	if (sourceMonAddr < firstMonAddr) {
		return false;
	}

	const uint32_t sourceOffset = sourceMonAddr - firstMonAddr;
	if ((sourceOffset % static_cast<uint32_t>(kGen5PartyMonSize)) != 0u) {
		return false;
	}

	const uint32_t slot = sourceOffset / static_cast<uint32_t>(kGen5PartyMonSize);
	if (slot >= 6u) {
		return false;
	}

	slotOut = static_cast<uint8_t>(slot);
	return true;
}

static void BuildBwEnemyPartyRuntimeSnapshotForProfile(
	const ExportProfileDefinition *profile,
	uint32_t battleSystemAddr,
	const std::vector<BattleMonRuntimeSnapshot> &battleMons,
	const std::vector<BattleLogEnemyPartySnapshot> &previousEnemyPartyMons,
	std::vector<BattleLogEnemyPartySnapshot> &outMons)
{
	outMons.clear();
	if ((profile == NULL) || !IsGen5BwLikePokemonProfile(profile->profile) || !IsMainRamAlignedPointer(battleSystemAddr)) {
		return;
	}

	const uint32_t clientPokeConAddr = battleSystemAddr + kBwBattleMainPokeconForClientOffset;
	if (ReadArm9U32(clientPokeConAddr + kBwBattlePokeConMainModuleOffset) != battleSystemAddr) {
		return;
	}

	const uint32_t enemyBattlePartyAddr = clientPokeConAddr + kBwBattlePokeConPartyOffset + kBwBattlePartySize;
	const uint32_t enemySrcPartyAddr = ReadArm9U32(clientPokeConAddr + kBwBattlePokeConSrcPartyOffset + 4u);
	const uint32_t partyKey = IsMainRamAlignedPointer(enemySrcPartyAddr) ? enemySrcPartyAddr : enemyBattlePartyAddr;
	const uint8_t memberCount = ReadArm9U8(enemyBattlePartyAddr + kBwBattlePartyMemberCountOffset);
	if ((memberCount < 1u) || (memberCount > 6u)) {
		return;
	}

	bool seenSlots[6] = {false, false, false, false, false, false};
	for (uint8_t memberIndex = 0u; memberIndex < memberCount; ++memberIndex) {
		const uint32_t bppAddr = ReadArm9U32(enemyBattlePartyAddr + (static_cast<uint32_t>(memberIndex) * 4u));
		if (!IsMainRamAlignedPointer(bppAddr)) {
			continue;
		}

		const uint16_t speciesId = ReadArm9U16(bppAddr + kBwBattlePokeParamSpeciesOffset);
		const uint32_t currentHp = ReadArm9U16(bppAddr + kBwBattlePokeParamCurHpOffset);
		const uint32_t maxHp = ReadArm9U16(bppAddr + kBwBattlePokeParamMaxHpOffset);
		if (!IsValidEnemyPartySpeciesForProfile(profile, speciesId) || (maxHp == 0u) || (currentHp > maxHp)) {
			continue;
		}

		const uint32_t sourceMonAddr = ReadArm9U32(bppAddr + kBwBattlePokeParamSourceOffset);
		uint8_t sourceSlot = memberIndex;
		uint8_t resolvedSourceSlot = 0xFFu;
		if (TryGetBwSourcePartySlot(enemySrcPartyAddr, sourceMonAddr, resolvedSourceSlot)) {
			sourceSlot = resolvedSourceSlot;
		}
		if (sourceSlot >= 6u || seenSlots[sourceSlot]) {
			continue;
		}
		seenSlots[sourceSlot] = true;

		BattleLogEnemyPartySnapshot snapshot;
		snapshot.valid = true;
		snapshot.partyPtr = partyKey;
		snapshot.slot = sourceSlot;
		snapshot.sourceMonAddr = IsMainRamAlignedPointer(sourceMonAddr) ? sourceMonAddr : bppAddr;
		snapshot.inferredPlayerSlot = 0xFFu;
		snapshot.speciesId = speciesId;
		snapshot.curHp = currentHp;
		snapshot.maxHp = maxHp;
		outMons.push_back(snapshot);
	}

	for (size_t i = 0; i < outMons.size(); ++i) {
		const int previousIndex = FindEnemyPartyRuntimeSnapshotIndex(previousEnemyPartyMons, outMons[i].partyPtr, outMons[i].slot);
		if (previousIndex >= 0) {
			outMons[i].inferredPlayerSlot = previousEnemyPartyMons[static_cast<size_t>(previousIndex)].inferredPlayerSlot;
		}
	}

	for (size_t i = 0; i < battleMons.size(); ++i) {
		const BattleMonRuntimeSnapshot &battleMon = battleMons[i];
		if (!battleMon.valid || ((battleMon.battler & 0x1u) == 0u)) {
			continue;
		}

		uint8_t sourceSlot = battleMon.partySlot;
		uint8_t resolvedSourceSlot = 0xFFu;
		if (TryGetBwSourcePartySlot(enemySrcPartyAddr, battleMon.sourceMonAddr, resolvedSourceSlot)) {
			sourceSlot = resolvedSourceSlot;
		}

		const int snapshotIndex = FindEnemyPartyRuntimeSnapshotIndex(outMons, partyKey, sourceSlot);
		if (snapshotIndex < 0) {
			continue;
		}

		const uint8_t inferredPlayerSlot = GetAdjacentPlayerSlotForEnemyBattler(battleMons, battleMon.battler);
		if (inferredPlayerSlot != 0xFFu) {
			outMons[static_cast<size_t>(snapshotIndex)].inferredPlayerSlot = inferredPlayerSlot;
		}
	}
}

static void AppendEnemyPartyRuntimeSnapshotFromPartyPtr(
	const ExportProfileDefinition *profile,
	uint32_t partyPtr,
	std::vector<BattleLogEnemyPartySnapshot> &outMons)
{
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if ((layout == NULL) || !IsMainRamAlignedPointer(partyPtr)) {
		return;
	}

	int capacity = 0;
	int currentCount = 0;
	if (!TryReadPartyCandidateSummary(partyPtr, capacity, currentCount)) {
		return;
	}

	const uint8_t partyCount = static_cast<uint8_t>(std::min(currentCount, 6));
	for (uint8_t slot = 0; slot < partyCount; ++slot) {
		const uint32_t monOffset = partyPtr + 0x8u + (static_cast<uint32_t>(slot) * static_cast<uint32_t>(layout->partyMonSize));
		DecryptedPokemon pokemon;
		PartyPokemonReadMode readMode;
		if (!ReadPartyPokemonRobustForProfile(profile, monOffset, pokemon, readMode)) {
			continue;
		}

		const uint32_t pid = pokemon.pid;
		if (pid == 0u) {
			continue;
		}

		const int shuffleKey = static_cast<int>(((pid & 0x3E000u) >> 13) % 24u);
		const int blockA = kShuffleTable[shuffleKey][0];
		const uint16_t speciesId = GetWord(pokemon, blockA + 0x0);
		const uint32_t currentHp = GetWord(pokemon, 0x8E);
		const uint32_t maxHp = GetWord(pokemon, 0x90);
		if ((speciesId == 0u) || (speciesId > 493u) || (maxHp == 0u)) {
			continue;
		}

		BattleLogEnemyPartySnapshot snapshot;
		snapshot.valid = true;
		snapshot.partyPtr = partyPtr;
		snapshot.slot = slot;
		snapshot.sourceMonAddr = monOffset;
		snapshot.inferredPlayerSlot = 0xFFu;
		snapshot.speciesId = speciesId;
		snapshot.curHp = currentHp;
		snapshot.maxHp = maxHp;
		outMons.push_back(snapshot);
	}
}

static bool IsValidEnemyPartySpeciesForProfile(const ExportProfileDefinition *profile, uint16_t speciesId)
{
	if (speciesId == 0u) {
		return false;
	}

	const uint16_t maxSpeciesId =
		(profile != NULL && IsGen5BwLikePokemonProfile(profile->profile))
			? 649u
			: 493u;
	return speciesId <= maxSpeciesId;
}

static void BuildBwActiveEnemyPartySnapshot(
	const ExportProfileDefinition *profile,
	const std::vector<BattleMonRuntimeSnapshot> &battleMons,
	std::vector<BattleLogEnemyPartySnapshot> &outMons)
{
	outMons.clear();
	if ((profile == NULL) || !IsGen5BwLikePokemonProfile(profile->profile)) {
		return;
	}

	bool seenSlots[6] = {false, false, false, false, false, false};
	for (size_t i = 0; i < battleMons.size(); ++i) {
		const BattleMonRuntimeSnapshot &battleMon = battleMons[i];
		if (!battleMon.valid || ((battleMon.battler & 0x1u) == 0u) || (battleMon.partySlot >= 6u)) {
			continue;
		}
		if (!IsValidEnemyPartySpeciesForProfile(profile, battleMon.speciesId) || (battleMon.maxHp == 0u)) {
			continue;
		}
		if (seenSlots[battleMon.partySlot]) {
			continue;
		}
		seenSlots[battleMon.partySlot] = true;

		BattleLogEnemyPartySnapshot snapshot;
		snapshot.valid = true;
		snapshot.partyPtr = 0u;
		snapshot.slot = battleMon.partySlot;
		snapshot.sourceMonAddr = battleMon.sourceMonAddr;
		snapshot.inferredPlayerSlot = 0xFFu;
		snapshot.speciesId = battleMon.speciesId;
		snapshot.curHp = battleMon.curHp;
		snapshot.maxHp = battleMon.maxHp;
		outMons.push_back(snapshot);
	}
}

static void BuildActiveSnapshotEnemyPartyForProfile(
	const ExportProfileDefinition *profile,
	const BattleMemoryLayout &layout,
	uint32_t battleSystemAddr,
	const std::vector<BattleMonRuntimeSnapshot> &battleMons,
	std::vector<BattleLogEnemyPartySnapshot> &outMons)
{
	if ((profile != NULL) && IsGen5BwLikePokemonProfile(profile->profile)) {
		BuildBwActiveEnemyPartySnapshot(profile, battleMons, outMons);
		return;
	}

	const std::vector<BattleLogEnemyPartySnapshot> emptyPreviousEnemyPartyMons;
	BuildEnemyPartyRuntimeSnapshotForProfile(
		profile,
		layout,
		battleSystemAddr,
		battleMons,
		emptyPreviousEnemyPartyMons,
		outMons);
}

static void BuildEnemyPartyRuntimeSnapshotForProfile(
	const ExportProfileDefinition *profile,
	const BattleMemoryLayout &layout,
	uint32_t battleSystemAddr,
	const std::vector<BattleMonRuntimeSnapshot> &battleMons,
	const std::vector<BattleLogEnemyPartySnapshot> &previousEnemyPartyMons,
	std::vector<BattleLogEnemyPartySnapshot> &outMons)
{
	outMons.clear();
	if ((profile == NULL) || !IsMainRamAlignedPointer(battleSystemAddr)) {
		return;
	}
	if (IsGen5BwLikePokemonProfile(profile->profile)) {
		BuildBwEnemyPartyRuntimeSnapshotForProfile(
			profile,
			battleSystemAddr,
			battleMons,
			previousEnemyPartyMons,
			outMons);
		return;
	}

	std::vector<uint32_t> seenPartyPtrs;
	for (uint8_t battler = 1u; battler < 4u; battler += 2u) {
		const uint32_t partyPtr = ReadArm9U32(battleSystemAddr + layout.battleSystemPartiesOffset + (static_cast<uint32_t>(battler) * 4u));
		if (!IsMainRamAlignedPointer(partyPtr)) {
			continue;
		}

		bool alreadySeen = false;
		for (size_t i = 0; i < seenPartyPtrs.size(); ++i) {
			if (seenPartyPtrs[i] == partyPtr) {
				alreadySeen = true;
				break;
			}
		}
		if (alreadySeen) {
			continue;
		}

		seenPartyPtrs.push_back(partyPtr);
		AppendEnemyPartyRuntimeSnapshotFromPartyPtr(profile, partyPtr, outMons);
	}

	for (size_t i = 0; i < outMons.size(); ++i) {
		const int previousIndex = FindEnemyPartyRuntimeSnapshotIndex(previousEnemyPartyMons, outMons[i].partyPtr, outMons[i].slot);
		if (previousIndex >= 0) {
			outMons[i].inferredPlayerSlot = previousEnemyPartyMons[previousIndex].inferredPlayerSlot;
		}
	}

	for (size_t i = 0; i < battleMons.size(); ++i) {
		const BattleMonRuntimeSnapshot &battleMon = battleMons[i];
		if (!battleMon.valid || ((battleMon.battler & 0x1u) == 0u) || (battleMon.partySlot > 5u)) {
			continue;
		}

		const uint32_t partyPtr = ReadArm9U32(
			battleSystemAddr + layout.battleSystemPartiesOffset + (static_cast<uint32_t>(battleMon.battler) * 4u));
		if (!IsMainRamAlignedPointer(partyPtr)) {
			continue;
		}

		const int snapshotIndex = FindEnemyPartyRuntimeSnapshotIndex(outMons, partyPtr, battleMon.partySlot);
		if (snapshotIndex < 0) {
			continue;
		}

		const uint8_t inferredPlayerSlot = GetAdjacentPlayerSlotForEnemyBattler(battleMons, battleMon.battler);
		if (inferredPlayerSlot != 0xFFu) {
			outMons[static_cast<size_t>(snapshotIndex)].inferredPlayerSlot = inferredPlayerSlot;
		}
	}
}

static bool HasRecordedEnemyKo(
	const std::vector<BattleLogRecordedEnemyKo> &recordedKos,
	uint32_t partyPtr,
	uint8_t slot,
	uint16_t speciesId)
{
	for (size_t i = 0; i < recordedKos.size(); ++i) {
		if ((recordedKos[i].partyPtr == partyPtr) &&
			(recordedKos[i].slot == slot) &&
			(recordedKos[i].speciesId == speciesId)) {
			return true;
		}
	}

	return false;
}

static void RecordEnemyKo(
	std::vector<BattleLogRecordedEnemyKo> &recordedKos,
	uint32_t partyPtr,
	uint8_t slot,
	uint16_t speciesId)
{
	if (!IsMainRamAlignedPointer(partyPtr) || HasRecordedEnemyKo(recordedKos, partyPtr, slot, speciesId)) {
		return;
	}

	BattleLogRecordedEnemyKo record;
	record.partyPtr = partyPtr;
	record.slot = slot;
	record.speciesId = speciesId;
	recordedKos.push_back(record);
}

static void RecordEnemyKoForBattler(
	const BattleMemoryLayout &layout,
	uint32_t battleSystemAddr,
	const BattleMonRuntimeSnapshot &enemyMon,
	std::vector<BattleLogRecordedEnemyKo> &recordedKos)
{
	if (!enemyMon.valid || ((enemyMon.battler & 0x1u) == 0u) || (enemyMon.partySlot > 5u) || !IsMainRamAlignedPointer(battleSystemAddr)) {
		return;
	}

	const uint32_t partyPtr = ReadArm9U32(
		battleSystemAddr + layout.battleSystemPartiesOffset + (static_cast<uint32_t>(enemyMon.battler) * 4u));
	RecordEnemyKo(recordedKos, partyPtr, enemyMon.partySlot, enemyMon.speciesId);
}

static std::string DecodeNickname(const DecryptedPokemon &pokemon, int blockC)
{
	using namespace desmume_pokemon_gen4;

	std::string nickname;
	for (int byteOffset = 0x0; byteOffset <= 0x16; byteOffset += 2) {
		const uint16_t value = GetWord(pokemon, blockC + byteOffset);
		if (value == 0xFFFF) {
			break;
		}
		nickname += DecodeGen4Char(value);
	}

	return nickname;
}

static bool EncodeGen4NicknameWordSequence(const std::string &text, std::vector<uint16_t> &wordsOut)
{
	using namespace desmume_pokemon_gen4;

	wordsOut.clear();
	size_t pos = 0;
	while (pos < text.size() && wordsOut.size() < 11u) {
		uint16_t bestWord = 0u;
		size_t bestLength = 0u;

		for (uint32_t value = 0u; value <= 0xFFFEu; ++value) {
			const char *token = DecodeGen4Char(static_cast<uint16_t>(value));
			if (token == NULL || token[0] == '\0') {
				continue;
			}

			const std::string tokenText(token);
			if (tokenText.empty()) {
				continue;
			}
			if (text.compare(pos, tokenText.size(), tokenText) != 0) {
				continue;
			}
			if (tokenText.size() <= bestLength) {
				continue;
			}

			bestWord = static_cast<uint16_t>(value);
			bestLength = tokenText.size();
		}

		if (bestLength == 0u) {
			return false;
		}

		wordsOut.push_back(bestWord);
		pos += bestLength;
	}

	return (pos == text.size());
}

static bool UpdateDefaultSpeciesNicknameIfNeeded(const ExportProfileDefinition *profile, DecryptedPokemon &pokemon, int blockB, int blockC, uint16_t speciesId)
{
	if (profile == NULL) {
		return false;
	}

	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL || layout->codec != POKEMON_DATA_CODEC_GEN4) {
		return false;
	}

	uint32_t ivData = static_cast<uint32_t>(GetWord(pokemon, blockB + 0x10)) |
		(static_cast<uint32_t>(GetWord(pokemon, blockB + 0x12)) << 16);
	const bool hasNickname = (ivData >> 31) == 1u;
	if (hasNickname) {
		return true;
	}

	const char *speciesName = LookupProfileSpeciesName(profile, speciesId);
	if (speciesName == NULL || speciesName[0] == '\0') {
		return false;
	}

	std::vector<uint16_t> nicknameWords;
	if (!EncodeGen4NicknameWordSequence(speciesName, nicknameWords)) {
		return false;
	}

	for (int byteOffset = 0; byteOffset <= 0x16; byteOffset += 2) {
		pokemon.words[(blockC + byteOffset) / 2] = 0xFFFFu;
	}
	for (size_t i = 0; i < nicknameWords.size() && i < 11u; ++i) {
		pokemon.words[(blockC + static_cast<int>(i * 2u)) / 2] = nicknameWords[i];
	}

	return true;
}

static std::string BuildShowdownExport(
	const std::string &species,
	const std::string &nickname,
	bool hasNickname,
	const std::string &gender,
	const std::string &heldItem,
	const std::string &ability,
	const std::string &location,
	uint32_t experience,
	const std::string &nature,
	int hp,
	int atk,
	int def,
	int spa,
	int spd,
	int spe,
	const std::vector<std::string> &moveNames)
{
	std::string paste;

	if (hasNickname && !nickname.empty()) {
		paste += nickname;
		paste += " (";
		paste += species;
		paste += ")";
	} else {
		paste += species;
	}

	if (!gender.empty() && gender != "N") {
		paste += " (";
		paste += gender;
		paste += ")";
	}

	if (!heldItem.empty()) {
		paste += " @ ";
		paste += heldItem;
	}

	paste += "\nAbility: ";
	paste += ability;
	paste += "\n";

	paste += "Level: ";
	paste += std::to_string(experience);
	paste += " EXP";
	paste += "\n";
	paste += nature;
	paste += " Nature\n";
	paste += "IVs: ";
	paste += std::to_string(hp);
	paste += " HP / ";
	paste += std::to_string(atk);
	paste += " Atk / ";
	paste += std::to_string(def);
	paste += " Def / ";
	paste += std::to_string(spa);
	paste += " SpA / ";
	paste += std::to_string(spd);
	paste += " SpD / ";
	paste += std::to_string(spe);
	paste += " Spe\n";

	for (size_t i = 0; i < moveNames.size(); ++i) {
		if (!moveNames[i].empty()) {
			paste += "- ";
			paste += moveNames[i];
			paste += "\n";
		}
	}

	if (!location.empty()) {
		paste += "Met: ";
		paste += location;
		paste += "\n";
	}

	paste += "\n";
	return paste;
}

static std::string BuildDeadShowdownExport(
	const std::string &species,
	const std::string &nickname,
	bool hasNickname,
	const std::string &location)
{
	std::string paste;

	if (hasNickname && !nickname.empty() && nickname != species) {
		paste += nickname;
		paste += " (";
		paste += species;
		paste += ")";
	} else {
		paste += species;
	}

	paste += "\nDead: Yes\n";
	if (!location.empty()) {
		paste += "Met: ";
		paste += location;
		paste += "\n";
	}

	paste += "\n";
	return paste;
}

static std::string BuildDeadPokemonJson(
	const ExportProfileDefinition *profile,
	const DecryptedPokemon &pokemon,
	uint32_t monOffset,
	int boxNumber,
	int slotNumber,
	uint16_t &speciesIdOut,
	uint16_t &locationIdOut,
	std::string &speciesOut,
	std::string &nicknameOut,
	bool &hasNicknameOut,
	std::string &locationOut,
	std::string &showdownOut)
{
	const uint32_t pid = pokemon.pid;
	const int shuffleKey = static_cast<int>(((pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const int blockB = kShuffleTable[shuffleKey][1];
	const int blockC = kShuffleTable[shuffleKey][2];
	const int blockD = kShuffleTable[shuffleKey][3];

	const uint16_t speciesId = static_cast<uint16_t>(GetWord(pokemon, blockA + 0x0));
	const uint16_t genderFormeWord = GetWord(pokemon, blockB + 0x18);
	const uint8_t formIndex = static_cast<uint8_t>(genderFormeWord >> 3);
	const char *baseSpeciesName = LookupProfileSpeciesName(profile, speciesId);
	std::string species = (baseSpeciesName[0] != '\0') ? baseSpeciesName : ("Species#" + std::to_string(speciesId));
	const char *formSuffix = GetProfileFormSuffix(profile, speciesId, formIndex);
	if (formSuffix[0] != '\0') {
		species += formSuffix;
	}

	const uint32_t ivData = static_cast<uint32_t>(GetWord(pokemon, blockB + 0x10)) |
	                        (static_cast<uint32_t>(GetWord(pokemon, blockB + 0x12)) << 16);
	const bool isEgg = ((ivData >> 30) & 0x1) == 1;
	const bool hasNickname = (ivData >> 31) == 1;
	const uint16_t locationId = static_cast<uint16_t>(isEgg ? 0 : (
		(profile != NULL && IsGen5BwLikePokemonProfile(profile->profile))
			? GetWord(pokemon, blockD + 0x18)
			: GetWord(pokemon, blockB + 0x1E)));
	const char *locationName = isEgg ? "" : GetProfileLocationName(profile, locationId);
	const std::string location = (locationName[0] != '\0') ? locationName : "";

	std::string nickname;
	if (hasNickname) {
		nickname = DecodeNickname(pokemon, blockC);
	}

	showdownOut = BuildDeadShowdownExport(species, nickname, hasNickname, location);

	speciesIdOut = speciesId;
	locationIdOut = locationId;
	speciesOut = species;
	nicknameOut = nickname;
	hasNicknameOut = hasNickname;
	locationOut = location;

	std::string json = "{";
	json += "\"box\":";
	json += std::to_string(boxNumber);
	json += ",\"slot\":";
	json += std::to_string(slotNumber);
	json += ",\"address\":";
	AppendJsonEscaped(json, FormatHex(monOffset));
	json += ",\"speciesId\":";
	json += std::to_string(speciesId);
	json += ",\"species\":";
	AppendJsonEscaped(json, species);
	json += ",\"nickname\":";
	AppendJsonEscaped(json, nickname);
	json += ",\"hasNickname\":";
	json += hasNickname ? "true" : "false";
	json += ",\"locationId\":";
	json += std::to_string(locationId);
	json += ",\"location\":";
	AppendJsonEscaped(json, location);
	json += "}";
	return json;
}

static std::string BuildPokemonJson(
	const ExportProfileDefinition *profile,
	const DecryptedPokemon &pokemon,
	uint32_t monOffset,
	const char *kind,
	int boxNumber,
	int slotNumber,
	std::string &showdownOut)
{
	const uint32_t pid = pokemon.pid;
	const int shuffleKey = static_cast<int>(((pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const int blockB = kShuffleTable[shuffleKey][1];
	const int blockC = kShuffleTable[shuffleKey][2];
	const int blockD = kShuffleTable[shuffleKey][3];

	const int speciesId = GetWord(pokemon, blockA + 0x0);
	const uint16_t genderFormeWord = GetWord(pokemon, blockB + 0x18);
	const uint8_t formIndex = static_cast<uint8_t>(genderFormeWord >> 3);
	const char *baseSpeciesName = LookupProfileSpeciesName(profile, speciesId);
	std::string species = (baseSpeciesName[0] != '\0') ? baseSpeciesName : ("Species#" + std::to_string(speciesId));
	const char *formSuffix = GetProfileFormSuffix(profile, static_cast<uint16_t>(speciesId), formIndex);
	if (formSuffix[0] != '\0') {
		species += formSuffix;
	}

	const int heldItemId = GetWord(pokemon, blockA + 0x2);
	const char *heldItemName = LookupProfileItemName(profile, heldItemId);
	const int abilityId = static_cast<int>(GetWord(pokemon, blockA + 0xC) >> 8);
	const char *abilityName = LookupProfileAbilityName(profile, abilityId);
	const uint32_t ivData = static_cast<uint32_t>(GetWord(pokemon, blockB + 0x10)) |
	                        (static_cast<uint32_t>(GetWord(pokemon, blockB + 0x12)) << 16);
	const bool isEgg = ((ivData >> 30) & 0x1) == 1;
	const bool hasNickname = (ivData >> 31) == 1;
	const int locationId = isEgg ? 0 : (
		(profile != NULL && IsGen5BwLikePokemonProfile(profile->profile))
			? GetWord(pokemon, blockD + 0x18)
			: GetWord(pokemon, blockB + 0x1E));
	const char *locationName = isEgg ? "" : GetProfileLocationName(profile, static_cast<uint16_t>(locationId));
	const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
	                           (static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);
	const int level = GetProfileLevelForExperience(profile, experience, speciesId);
	const int natureIndex = static_cast<int>(pid % 25u);
	const char *natureName = LookupProfileNatureName(profile, natureIndex);
	const int ivHp = ivData & 0x1F;
	const int ivAtk = (ivData >> 5) & 0x1F;
	const int ivDef = (ivData >> 10) & 0x1F;
	const int ivSpe = (ivData >> 15) & 0x1F;
	const int ivSpa = (ivData >> 20) & 0x1F;
	const int ivSpd = (ivData >> 25) & 0x1F;

	std::vector<int> moveIds(4, 0);
	std::vector<std::string> moveNames(4);
	for (int i = 0; i < 4; ++i) {
		moveIds[i] = GetWord(pokemon, blockB + (i * 2));
		const char *moveName = LookupProfileMoveName(profile, moveIds[i]);
		if (moveName[0] != '\0') {
			moveNames[i] = moveName;
		}
	}

	std::string nickname;
	if (hasNickname) {
		nickname = DecodeNickname(pokemon, blockC);
	}

	std::string gender = "M";
	if (((genderFormeWord >> 1) & 0x1) == 1) {
		gender = "F";
	}
	if (((genderFormeWord >> 2) & 0x1) == 1) {
		gender = "N";
	}

	const std::string heldItem = (heldItemName[0] != '\0') ? heldItemName : "";
	const std::string ability = (abilityName[0] != '\0') ? abilityName : ("Ability#" + std::to_string(abilityId));
	const std::string location = (locationName[0] != '\0') ? locationName : "";

	showdownOut = BuildShowdownExport(
		species,
		nickname,
		hasNickname,
		gender,
		heldItem,
		ability,
		location,
		experience,
		natureName,
		ivHp,
		ivAtk,
		ivDef,
		ivSpa,
		ivSpd,
		ivSpe,
		moveNames);

	std::string json = "{";
	json += "\"kind\":";
	AppendJsonEscaped(json, kind);
	json += ",\"box\":";
	json += std::to_string(boxNumber);
	json += ",\"slot\":";
	json += std::to_string(slotNumber);
	json += ",\"address\":";
	AppendJsonEscaped(json, FormatHex(monOffset));
	json += ",\"pid\":";
	json += std::to_string(pid);
	json += ",\"checksum\":";
	json += std::to_string(pokemon.checksum);
	json += ",\"checksumValid\":";
	json += pokemon.checksumValid ? "true" : "false";
	json += ",\"speciesId\":";
	json += std::to_string(speciesId);
	json += ",\"species\":";
	AppendJsonEscaped(json, species);
	json += ",\"formIndex\":";
	json += std::to_string(formIndex);
	json += ",\"nickname\":";
	AppendJsonEscaped(json, nickname);
	json += ",\"hasNickname\":";
	json += hasNickname ? "true" : "false";
	json += ",\"gender\":";
	AppendJsonEscaped(json, gender);
	json += ",\"heldItemId\":";
	json += std::to_string(heldItemId);
	json += ",\"heldItem\":";
	AppendJsonEscaped(json, heldItem);
	json += ",\"abilityId\":";
	json += std::to_string(abilityId);
	json += ",\"ability\":";
	AppendJsonEscaped(json, ability);
	json += ",\"locationId\":";
	json += std::to_string(locationId);
	json += ",\"location\":";
	AppendJsonEscaped(json, location);
	json += ",\"isEgg\":";
	json += isEgg ? "true" : "false";
	json += ",\"level\":";
	json += std::to_string(level);
	json += ",\"nature\":";
	AppendJsonEscaped(json, natureName);
	json += ",\"ivs\":{";
	json += "\"hp\":" + std::to_string(ivHp);
	json += ",\"atk\":" + std::to_string(ivAtk);
	json += ",\"def\":" + std::to_string(ivDef);
	json += ",\"spa\":" + std::to_string(ivSpa);
	json += ",\"spd\":" + std::to_string(ivSpd);
	json += ",\"spe\":" + std::to_string(ivSpe);
	json += "},\"moves\":[";
	for (size_t i = 0; i < moveIds.size(); ++i) {
		if (i != 0) {
			json += ",";
		}
		json += "{";
		json += "\"id\":";
		json += std::to_string(moveIds[i]);
		json += ",\"name\":";
		AppendJsonEscaped(json, moveNames[i]);
		json += "}";
	}
	json += "],\"showdown\":";
	AppendJsonEscaped(json, showdownOut);
	json += "}";

	return json;
}

static bool AppendActiveSnapshotPokemonJson(
	std::string &json,
	const ExportProfileDefinition *profile,
	const DecryptedPokemon &pokemon,
	uint32_t currentHp)
{
	if (profile == NULL || pokemon.pid == 0u) {
		return false;
	}

	const uint32_t pid = pokemon.pid;
	const int shuffleKey = static_cast<int>(((pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const int blockB = kShuffleTable[shuffleKey][1];
	const uint16_t speciesId = GetWord(pokemon, blockA + 0x0);
	const uint16_t heldItemId = GetWord(pokemon, blockA + 0x2);
	const uint16_t abilityId = static_cast<uint16_t>((GetWord(pokemon, blockA + 0xC) >> 8) & 0xFFu);
	const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
	                           (static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);
	const int level = GetProfileLevelForExperience(profile, experience, speciesId);
	const uint32_t statusRaw = static_cast<uint32_t>(GetWord(pokemon, 0x88)) |
	                          (static_cast<uint32_t>(GetWord(pokemon, 0x8A)) << 16);
	const uint16_t hpStat = GetWord(pokemon, 0x90);
	const uint16_t atkStat = GetWord(pokemon, 0x92);
	const uint16_t defStat = GetWord(pokemon, 0x94);
	const uint16_t speStat = GetWord(pokemon, 0x96);
	const uint16_t spaStat = GetWord(pokemon, 0x98);
	const uint16_t spdStat = GetWord(pokemon, 0x9A);
	const int natureIndex = static_cast<int>(pid % 25u);
	const char *speciesName = LookupProfileSpeciesName(profile, speciesId);
	const char *heldItemName = LookupProfileItemName(profile, heldItemId);
	const char *abilityName = LookupProfileAbilityName(profile, abilityId);
	const char *natureName = LookupProfileNatureName(profile, natureIndex);
	const std::string statusLabel = GetPartyStatusLabel(profile, statusRaw);
	const std::string speciesLabel = (speciesName != NULL && speciesName[0] != '\0')
		? std::string(speciesName)
		: ("Species#" + std::to_string(speciesId));
	const std::string abilityLabel = (abilityName != NULL && abilityName[0] != '\0')
		? std::string(abilityName)
		: ("Ability#" + std::to_string(abilityId));
	const std::string heldItemLabel = (heldItemName != NULL && heldItemName[0] != '\0')
		? std::string(heldItemName)
		: "";
	const uint16_t pp12 = GetWord(pokemon, blockB + 0x8);
	const uint16_t pp34 = GetWord(pokemon, blockB + 0xA);

	json += ",\"pid\":";
	json += std::to_string(pid);
	json += ",\"speciesId\":";
	json += std::to_string(speciesId);
	json += ",\"species\":";
	AppendJsonEscaped(json, speciesLabel);
	json += ",\"level\":";
	json += std::to_string(level);
	json += ",\"nature\":";
	AppendJsonEscaped(json, (natureName != NULL) ? natureName : "");
	json += ",\"abilityId\":";
	json += std::to_string(abilityId);
	json += ",\"ability\":";
	AppendJsonEscaped(json, abilityLabel);
	json += ",\"heldItemId\":";
	json += std::to_string(heldItemId);
	json += ",\"heldItem\":";
	AppendJsonEscaped(json, heldItemLabel);
	json += ",\"statusRaw\":";
	json += std::to_string(CanonicalizePartyStatusForEditor(profile, statusRaw));
	json += ",\"status\":";
	AppendJsonEscaped(json, statusLabel);
	json += ",\"currentHp\":";
	json += std::to_string(currentHp);
	json += ",\"stats\":{";
	json += "\"hp\":" + std::to_string(hpStat);
	json += ",\"attack\":" + std::to_string(atkStat);
	json += ",\"defense\":" + std::to_string(defStat);
	json += ",\"spAttack\":" + std::to_string(spaStat);
	json += ",\"spDefense\":" + std::to_string(spdStat);
	json += ",\"speed\":" + std::to_string(speStat);
	json += "},\"moves\":[";
	for (int moveIndex = 0; moveIndex < 4; ++moveIndex) {
		if (moveIndex != 0) {
			json += ",";
		}
		const uint16_t moveId = GetWord(pokemon, blockB + (moveIndex * 2));
		const char *moveName = LookupProfileMoveName(profile, moveId);
		const uint8_t movePp = (moveIndex < 2)
			? static_cast<uint8_t>((pp12 >> (moveIndex * 8)) & 0xFFu)
			: static_cast<uint8_t>((pp34 >> ((moveIndex - 2) * 8)) & 0xFFu);
		json += "{";
		json += "\"id\":";
		json += std::to_string(moveId);
		json += ",\"name\":";
		AppendJsonEscaped(json, (moveName != NULL && moveName[0] != '\0') ? moveName : "");
		json += ",\"pp\":";
		json += std::to_string(movePp);
		json += "}";
	}
	json += "]";
	return true;
}

static bool BuildActiveBattleSnapshotJson(
	const ExportProfileDefinition *profile,
	const BattleDetectionStatus &battleStatus,
	std::string &jsonOut,
	std::string &errorOut)
{
	std::string json = "{";
	json += "\"gameCode\":";
	AppendJsonEscaped(json, GameCodeString());
	json += ",\"selectedProfile\":";
	AppendJsonEscaped(json, (profile != NULL) ? profile->displayName : "");
	json += ",\"battleActive\":";
	json += battleStatus.battleActive ? "true" : "false";
	json += ",\"trainerBattleActive\":";
	json += battleStatus.trainerBattleActive ? "true" : "false";
	if (battleStatus.trainerBattleActive && battleStatus.enemyTrainerId != 0u) {
		json += ",\"enemyTrainerId\":";
		json += std::to_string(battleStatus.enemyTrainerId);
	}
	json += ",\"battleSystemAddr\":";
	if (battleStatus.battleSystemAddr != 0u) {
		AppendJsonEscaped(json, FormatHex(battleStatus.battleSystemAddr));
	} else {
		json += "null";
	}
	json += ",\"battleContextAddr\":";
	if (battleStatus.battleContextAddr != 0u) {
		AppendJsonEscaped(json, FormatHex(battleStatus.battleContextAddr));
	} else {
		json += "null";
	}
	json += ",\"battleType\":";
	if (battleStatus.battleType != 0u) {
		AppendJsonEscaped(json, FormatHex(battleStatus.battleType));
	} else {
		json += "null";
	}
	json += ",\"maxBattlers\":";
	json += std::to_string(std::max(0, battleStatus.maxBattlers));
	json += ",\"party\":[";

	if (profile != NULL) {
		const PokemonMemoryLayout *pokemonLayout = GetProfilePokemonLayout(profile);
		if (pokemonLayout == NULL) {
			errorOut = std::string(profile->displayName) + " memory layout is unavailable";
			return false;
		}

		PokemonRuntimeAddresses addresses;
		if (!ResolvePokemonRuntimeAddresses(profile, addresses, errorOut)) {
			return false;
		}

		const uint32_t partyCount = std::min<uint32_t>(ReadArm9U8(addresses.partyCountAddr), 6u);
		const uint32_t partyBaseAddr = addresses.partyBaseAddr;
		for (uint32_t slot = 0; slot < partyCount; ++slot) {
			if (slot != 0u) {
				json += ",";
			}

			const uint32_t monOffset = partyBaseAddr + (slot * static_cast<uint32_t>(pokemonLayout->partyMonSize));
			DecryptedPokemon pokemon;
			PartyPokemonReadMode readMode;
			if (!ReadPartyPokemonRobustForProfile(profile, monOffset, pokemon, readMode)) {
				errorOut = "Failed to decrypt party slot " + std::to_string(slot + 1u);
				return false;
			}

			json += "{";
			json += "\"partySlot\":";
			json += std::to_string(slot + 1u);
			json += ",\"sourceMonAddr\":";
			AppendJsonEscaped(json, FormatHex(monOffset));
			if (!AppendActiveSnapshotPokemonJson(json, profile, pokemon, GetWord(pokemon, 0x8E))) {
				errorOut = "Failed to serialize party slot " + std::to_string(slot + 1u);
				return false;
			}
			json += "}";
		}
	}

	json += "]";
	std::vector<BattleMonRuntimeSnapshot> runtimeMons;
	const BattleMemoryLayout *battleLayout = NULL;
	if (battleStatus.battleActive && (profile != NULL)) {
		battleLayout = GetProfileBattleLayout(profile);
		if ((battleLayout != NULL) && battleLayout->supported) {
			BuildBattleMonRuntimeSnapshotForProfile(
				profile,
				*battleLayout,
				battleStatus.battleSystemAddr,
				battleStatus.battleContextAddr,
				runtimeMons);
		}
	}

	json += ",\"enemyTrainer\":{";
	json += "\"trainerId\":";
	if (battleStatus.trainerBattleActive && battleStatus.enemyTrainerId != 0u) {
		json += std::to_string(battleStatus.enemyTrainerId);
	} else {
		json += "null";
	}
	json += ",\"party\":[";

	if (battleStatus.trainerBattleActive && (profile != NULL) && (battleLayout != NULL) && battleLayout->supported) {
		std::vector<BattleLogEnemyPartySnapshot> enemyPartyMons;
		BuildActiveSnapshotEnemyPartyForProfile(
			profile,
			*battleLayout,
			battleStatus.battleSystemAddr,
			runtimeMons,
			enemyPartyMons);

		bool firstEnemyMon = true;
		for (size_t i = 0; i < enemyPartyMons.size(); ++i) {
			const BattleLogEnemyPartySnapshot &enemyMon = enemyPartyMons[i];
			if (!enemyMon.valid) {
				continue;
			}

			if (!firstEnemyMon) {
				json += ",";
			}
			firstEnemyMon = false;

			json += "{";
			json += "\"partySlot\":";
			json += std::to_string(static_cast<uint32_t>(enemyMon.slot) + 1u);
			json += ",\"sourceMonAddr\":";
			if (enemyMon.sourceMonAddr != 0u) {
				AppendJsonEscaped(json, FormatHex(enemyMon.sourceMonAddr));
			} else {
				json += "null";
			}

			DecryptedPokemon pokemon;
			PartyPokemonReadMode readMode;
			if ((enemyMon.sourceMonAddr != 0u) &&
				ReadPartyPokemonRobustForProfile(profile, enemyMon.sourceMonAddr, pokemon, readMode) &&
				IsValidEnemyPartySpeciesForProfile(profile, GetDecryptedPokemonSpeciesId(pokemon)) &&
				AppendActiveSnapshotPokemonJson(json, profile, pokemon, enemyMon.curHp)) {
			} else {
				const char *speciesName = LookupProfileSpeciesName(profile, enemyMon.speciesId);
				const std::string speciesLabel = (speciesName != NULL && speciesName[0] != '\0')
					? std::string(speciesName)
					: ("Species#" + std::to_string(enemyMon.speciesId));
				json += ",\"speciesId\":";
				json += std::to_string(enemyMon.speciesId);
				json += ",\"species\":";
				AppendJsonEscaped(json, speciesLabel);
				json += ",\"currentHp\":";
				json += std::to_string(enemyMon.curHp);
				json += ",\"statusRaw\":0,\"status\":\"\"";
				json += ",\"stats\":{";
				json += "\"hp\":" + std::to_string(enemyMon.maxHp);
				json += ",\"attack\":0,\"defense\":0,\"spAttack\":0,\"spDefense\":0,\"speed\":0}";
				json += ",\"moves\":[]";
			}

			json += "}";
		}
	}

	json += "]}";
	json += ",\"battlers\":[";

	if (battleStatus.battleActive && (battleLayout != NULL) && battleLayout->supported) {
			bool firstBattler = true;
			for (size_t i = 0; i < runtimeMons.size(); ++i) {
				const BattleMonRuntimeSnapshot &runtimeMon = runtimeMons[i];
				if (!runtimeMon.valid || (runtimeMon.speciesId == 0u)) {
					continue;
				}

				if (!firstBattler) {
					json += ",";
				}
				firstBattler = false;

				json += "{";
				json += "\"battler\":";
				json += std::to_string(runtimeMon.battler);
				json += ",\"partySlot\":";
				json += std::to_string(static_cast<uint32_t>(runtimeMon.partySlot) + 1u);
				json += ",\"sourceMonAddr\":";
				if (runtimeMon.sourceMonAddr != 0u) {
					AppendJsonEscaped(json, FormatHex(runtimeMon.sourceMonAddr));
				} else {
					json += "null";
				}

				DecryptedPokemon pokemon;
				PartyPokemonReadMode readMode;
				if ((runtimeMon.sourceMonAddr != 0u) && ReadPartyPokemonRobustForProfile(profile, runtimeMon.sourceMonAddr, pokemon, readMode) &&
					AppendActiveSnapshotPokemonJson(json, profile, pokemon, runtimeMon.curHp)) {
				} else {
					const char *speciesName = LookupProfileSpeciesName(profile, runtimeMon.speciesId);
					const std::string speciesLabel = (speciesName != NULL && speciesName[0] != '\0')
						? std::string(speciesName)
						: ("Species#" + std::to_string(runtimeMon.speciesId));
					json += ",\"speciesId\":";
					json += std::to_string(runtimeMon.speciesId);
					json += ",\"species\":";
					AppendJsonEscaped(json, speciesLabel);
					json += ",\"currentHp\":";
					json += std::to_string(runtimeMon.curHp);
					json += ",\"stats\":{";
					json += "\"hp\":" + std::to_string(runtimeMon.maxHp);
					json += ",\"attack\":0,\"defense\":0,\"spAttack\":0,\"spDefense\":0,\"speed\":0}";
					json += ",\"moves\":[]";
				}

				json += "}";
			}
	}

	json += "]}";
	jsonOut = json;
	errorOut.clear();
	return true;
}

static bool ResolvePartyContext(
	ExportGameProfile selectedProfile,
	const ExportProfileDefinition *&profileOut,
	uint32_t &pidAddrOut,
	uint32_t &partyBaseAddrOut,
	uint8_t &partyCountOut,
	std::string &errorOut)
{
	profileOut = GetExportProfileDefinition(selectedProfile);
	if (profileOut == NULL) {
		errorOut = "No Pokemon export game selected";
		return false;
	}

	if (gameInfo.romsize == 0) {
		errorOut = "No ROM loaded";
		return false;
	}

	PokemonRuntimeAddresses addresses;
	if (!ResolvePokemonRuntimeAddresses(profileOut, addresses, errorOut)) {
		return false;
	}

	pidAddrOut = addresses.pidAddr;
	partyBaseAddrOut = addresses.partyBaseAddr;
	partyCountOut = static_cast<uint8_t>(std::min<uint32_t>(ReadArm9U8(addresses.partyCountAddr), 6u));
	errorOut.clear();
	return true;
}

static bool ResolveBoxContext(
	ExportGameProfile selectedProfile,
	const ExportProfileDefinition *&profileOut,
	uint32_t &pidAddrOut,
	uint32_t &boxBaseAddrOut,
	uint8_t &currentBoxIndexOut,
	std::string &errorOut)
{
	profileOut = GetExportProfileDefinition(selectedProfile);
	if (profileOut == NULL) {
		errorOut = "No Pokemon export game selected";
		return false;
	}

	if (gameInfo.romsize == 0) {
		errorOut = "No ROM loaded";
		return false;
	}

	PokemonRuntimeAddresses addresses;
	if (!ResolvePokemonRuntimeAddresses(profileOut, addresses, errorOut)) {
		return false;
	}

	pidAddrOut = addresses.pidAddr;
	boxBaseAddrOut = addresses.boxBaseAddr;
	currentBoxIndexOut = ReadCurrentBoxIndexForProfile(GetProfilePokemonLayout(profileOut), addresses);
	errorOut.clear();
	return true;
}

static bool BuildPartyPokemonSummary(
	const ExportProfileDefinition *profile,
	const DecryptedPokemon &pokemon,
	uint32_t monOffset,
	int slot,
	PartyPokemonSummary &summary)
{
	const uint32_t pid = pokemon.pid;
	const int shuffleKey = static_cast<int>(((pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const int blockB = kShuffleTable[shuffleKey][1];
	const int blockC = kShuffleTable[shuffleKey][2];

	const uint16_t speciesId = GetDecryptedPokemonSpeciesId(pokemon);
	if (speciesId == 0u) {
		return false;
	}
	const uint16_t genderFormeWord = GetWord(pokemon, blockB + 0x18);
	const uint8_t formIndex = static_cast<uint8_t>(genderFormeWord >> 3);
	const std::string gender = DecodePokemonGender(genderFormeWord);
	const uint16_t heldItemId = GetWord(pokemon, blockA + 0x2);
	const uint16_t abilityFriendship = GetWord(pokemon, blockA + 0xC);
	const uint16_t abilityId = static_cast<uint16_t>((abilityFriendship >> 8) & 0xFFu);
	const uint8_t friendship = static_cast<uint8_t>(abilityFriendship & 0xFFu);
	const char *baseSpeciesName = LookupProfileSpeciesName(profile, speciesId);
	std::string species = (baseSpeciesName[0] != '\0') ? baseSpeciesName : ("Species#" + std::to_string(speciesId));
	const char *heldItemName = LookupProfileItemName(profile, heldItemId);
	const char *abilityName = LookupProfileAbilityName(profile, abilityId);
	const char *formSuffix = GetProfileFormSuffix(profile, speciesId, formIndex);
	if (formSuffix[0] != '\0') {
		species += formSuffix;
	}

	const uint32_t ivData = static_cast<uint32_t>(GetWord(pokemon, blockB + 0x10)) |
	                        (static_cast<uint32_t>(GetWord(pokemon, blockB + 0x12)) << 16);
	const bool hasNickname = (ivData >> 31) == 1;
	const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
	                           (static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);
	const int level = GetProfileLevelForExperience(profile, experience, speciesId);
	const uint32_t nextLevelExperience = (level >= 100) ? experience : GetProfileExperienceForLevel(profile, level + 1, speciesId);
	const uint32_t statusRaw = static_cast<uint32_t>(GetWord(pokemon, 0x88)) |
	                          (static_cast<uint32_t>(GetWord(pokemon, 0x8A)) << 16);
	const bool hasRecognizedStatus = IsRecognizedPartyStatusRaw(profile, statusRaw);
	const uint32_t canonicalStatusRaw = hasRecognizedStatus
		? CanonicalizePartyStatusForEditor(profile, statusRaw)
		: 0u;

	summary.slot = slot;
	summary.monOffset = monOffset;
	summary.pid = pid;
	summary.speciesId = speciesId;
	summary.species = species;
	summary.heldItemId = heldItemId;
	summary.heldItem = (heldItemName[0] != '\0') ? heldItemName : "";
	summary.abilityId = abilityId;
	summary.abilitySlot = GetPokemonAbilitySlotDisplayIndex(profile, pokemon);
	summary.ability = (abilityName[0] != '\0') ? abilityName : "";
	summary.friendship = friendship;
	summary.gender = gender;
	summary.nickname = hasNickname ? DecodeNickname(pokemon, blockC) : "";
	summary.hasNickname = hasNickname;
	summary.level = level;
	summary.experience = experience;
	summary.nextLevelExperience = nextLevelExperience;
	summary.currentHp = GetWord(pokemon, 0x8E);
	summary.maxHp = GetWord(pokemon, 0x90);
	summary.statusRaw = canonicalStatusRaw;
	summary.encodedStatusRaw = statusRaw;
	summary.statusLabel = hasRecognizedStatus ? GetPartyStatusLabel(profile, statusRaw) : "Healthy";
	summary.stats.evs[0] = static_cast<uint8_t>(GetWord(pokemon, blockA + 0x10) & 0xFFu);
	summary.stats.evs[1] = static_cast<uint8_t>((GetWord(pokemon, blockA + 0x10) >> 8) & 0xFFu);
	summary.stats.evs[2] = static_cast<uint8_t>(GetWord(pokemon, blockA + 0x12) & 0xFFu);
	summary.stats.evs[3] = static_cast<uint8_t>(GetWord(pokemon, blockA + 0x14) & 0xFFu);
	summary.stats.evs[4] = static_cast<uint8_t>((GetWord(pokemon, blockA + 0x14) >> 8) & 0xFFu);
	summary.stats.evs[5] = static_cast<uint8_t>((GetWord(pokemon, blockA + 0x12) >> 8) & 0xFFu);
	summary.stats.ivs[0] = static_cast<uint8_t>(ivData & 0x1Fu);
	summary.stats.ivs[1] = static_cast<uint8_t>((ivData >> 5) & 0x1Fu);
	summary.stats.ivs[2] = static_cast<uint8_t>((ivData >> 10) & 0x1Fu);
	summary.stats.ivs[3] = static_cast<uint8_t>((ivData >> 20) & 0x1Fu);
	summary.stats.ivs[4] = static_cast<uint8_t>((ivData >> 25) & 0x1Fu);
	summary.stats.ivs[5] = static_cast<uint8_t>((ivData >> 15) & 0x1Fu);

	for (int i = 0; i < 4; ++i) {
		const uint16_t moveId = GetWord(pokemon, blockB + (i * 2));
		summary.moves[i].id = moveId;
		const char *moveName = LookupProfileMoveName(profile, moveId);
		summary.moves[i].name = (moveName[0] != '\0') ? moveName : "";
	}
	summary.debugSnapshot = BuildEditorDebugSnapshotText(profile, pokemon, monOffset, "party", 0, slot);

	return true;
}

static bool BuildBoxPokemonSummary(
	const ExportProfileDefinition *profile,
	const DecryptedPokemon &pokemon,
	uint32_t monOffset,
	int box,
	int slot,
	BoxPokemonSummary &summary)
{
	const uint32_t pid = pokemon.pid;
	const int shuffleKey = static_cast<int>(((pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const int blockB = kShuffleTable[shuffleKey][1];
	const int blockC = kShuffleTable[shuffleKey][2];

	const uint16_t speciesId = GetDecryptedPokemonSpeciesId(pokemon);
	if (speciesId == 0u) {
		return false;
	}
	const uint16_t genderFormeWord = GetWord(pokemon, blockB + 0x18);
	const uint8_t formIndex = static_cast<uint8_t>(genderFormeWord >> 3);
	const std::string gender = DecodePokemonGender(genderFormeWord);
	const uint16_t heldItemId = GetWord(pokemon, blockA + 0x2);
	const uint16_t abilityFriendship = GetWord(pokemon, blockA + 0xC);
	const uint16_t abilityId = static_cast<uint16_t>((abilityFriendship >> 8) & 0xFFu);
	const uint8_t friendship = static_cast<uint8_t>(abilityFriendship & 0xFFu);
	const char *baseSpeciesName = LookupProfileSpeciesName(profile, speciesId);
	std::string species = (baseSpeciesName[0] != '\0') ? baseSpeciesName : ("Species#" + std::to_string(speciesId));
	const char *heldItemName = LookupProfileItemName(profile, heldItemId);
	const char *abilityName = LookupProfileAbilityName(profile, abilityId);
	const char *formSuffix = GetProfileFormSuffix(profile, speciesId, formIndex);
	if (formSuffix[0] != '\0') {
		species += formSuffix;
	}

	const uint32_t ivData = static_cast<uint32_t>(GetWord(pokemon, blockB + 0x10)) |
	                        (static_cast<uint32_t>(GetWord(pokemon, blockB + 0x12)) << 16);
	const bool hasNickname = (ivData >> 31) == 1;
	const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
	                           (static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);
	const int level = GetProfileLevelForExperience(profile, experience, speciesId);
	const uint32_t nextLevelExperience = (level >= 100) ? experience : GetProfileExperienceForLevel(profile, level + 1, speciesId);

	summary.box = box;
	summary.slot = slot;
	summary.monOffset = monOffset;
	summary.pid = pid;
	summary.speciesId = speciesId;
	summary.species = species;
	summary.heldItemId = heldItemId;
	summary.heldItem = (heldItemName[0] != '\0') ? heldItemName : "";
	summary.abilityId = abilityId;
	summary.abilitySlot = GetPokemonAbilitySlotDisplayIndex(profile, pokemon);
	summary.ability = (abilityName[0] != '\0') ? abilityName : "";
	summary.friendship = friendship;
	summary.gender = gender;
	summary.nickname = hasNickname ? DecodeNickname(pokemon, blockC) : "";
	summary.hasNickname = hasNickname;
	summary.level = level;
	summary.experience = experience;
	summary.nextLevelExperience = nextLevelExperience;
	summary.stats.evs[0] = static_cast<uint8_t>(GetWord(pokemon, blockA + 0x10) & 0xFFu);
	summary.stats.evs[1] = static_cast<uint8_t>((GetWord(pokemon, blockA + 0x10) >> 8) & 0xFFu);
	summary.stats.evs[2] = static_cast<uint8_t>(GetWord(pokemon, blockA + 0x12) & 0xFFu);
	summary.stats.evs[3] = static_cast<uint8_t>(GetWord(pokemon, blockA + 0x14) & 0xFFu);
	summary.stats.evs[4] = static_cast<uint8_t>((GetWord(pokemon, blockA + 0x14) >> 8) & 0xFFu);
	summary.stats.evs[5] = static_cast<uint8_t>((GetWord(pokemon, blockA + 0x12) >> 8) & 0xFFu);
	summary.stats.ivs[0] = static_cast<uint8_t>(ivData & 0x1Fu);
	summary.stats.ivs[1] = static_cast<uint8_t>((ivData >> 5) & 0x1Fu);
	summary.stats.ivs[2] = static_cast<uint8_t>((ivData >> 10) & 0x1Fu);
	summary.stats.ivs[3] = static_cast<uint8_t>((ivData >> 20) & 0x1Fu);
	summary.stats.ivs[4] = static_cast<uint8_t>((ivData >> 25) & 0x1Fu);
	summary.stats.ivs[5] = static_cast<uint8_t>((ivData >> 15) & 0x1Fu);

	for (int i = 0; i < 4; ++i) {
		const uint16_t moveId = GetWord(pokemon, blockB + (i * 2));
		summary.moves[i].id = moveId;
		const char *moveName = LookupProfileMoveName(profile, moveId);
		summary.moves[i].name = (moveName[0] != '\0') ? moveName : "";
	}
	summary.debugSnapshot = BuildEditorDebugSnapshotText(profile, pokemon, monOffset, "box", box, slot);

	return true;
}

static void WriteEncryptedPartyPokemon(uint32_t monOffset, DecryptedPokemon &pokemon)
{
	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		checksum = (checksum + pokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	pokemon.checksum = static_cast<uint16_t>(checksum);
	pokemon.checksumValid = true;
	WriteArm9U16(monOffset + 0x6, pokemon.checksum);

	uint32_t seed = pokemon.checksum;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		seed = AdvanceLcrng(seed);
		const uint16_t encrypted = static_cast<uint16_t>(pokemon.words[byteOffset / 2] ^ (seed >> 16));
		WriteArm9U16(monOffset + byteOffset, encrypted);
	}

	seed = pokemon.pid;
	for (int byteOffset = 0x88; byteOffset <= 0xEA; byteOffset += 2) {
		seed = AdvanceLcrng(seed);
		const uint16_t encrypted = static_cast<uint16_t>(pokemon.words[byteOffset / 2] ^ (seed >> 16));
		WriteArm9U16(monOffset + byteOffset, encrypted);
	}
}

static void WriteEncryptedBoxPokemon(uint32_t monOffset, DecryptedPokemon &pokemon)
{
	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		checksum = (checksum + pokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	pokemon.checksum = static_cast<uint16_t>(checksum);
	pokemon.checksumValid = true;
	WriteArm9U16(monOffset + 0x6, pokemon.checksum);

	uint32_t seed = pokemon.checksum;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		seed = AdvanceLcrng(seed);
		const uint16_t encrypted = static_cast<uint16_t>(pokemon.words[byteOffset / 2] ^ (seed >> 16));
		WriteArm9U16(monOffset + byteOffset, encrypted);
	}
}

static void WriteAlreadyDecryptedPartyPokemon(uint32_t monOffset, DecryptedPokemon &pokemon)
{
	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		checksum = (checksum + pokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	pokemon.checksum = static_cast<uint16_t>(checksum);
	pokemon.checksumValid = true;

	for (int byteOffset = 0; byteOffset <= 0x4; byteOffset += 2) {
		WriteArm9U16(monOffset + byteOffset, pokemon.words[byteOffset / 2]);
	}
	WriteArm9U16(monOffset + 0x6, pokemon.checksum);
	for (int byteOffset = 0x8; byteOffset <= 0xEA; byteOffset += 2) {
		WriteArm9U16(monOffset + byteOffset, pokemon.words[byteOffset / 2]);
	}
}

static void WritePartyPokemonWithModes(uint32_t monOffset, DecryptedPokemon &pokemon, const PartyPokemonReadMode &mode)
{
	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
		checksum = (checksum + pokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	pokemon.checksum = static_cast<uint16_t>(checksum);
	pokemon.checksumValid = true;
	WriteArm9U16(monOffset + 0x6, pokemon.checksum);

	if (mode.storageMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED) {
		for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
			WriteArm9U16(monOffset + byteOffset, pokemon.words[byteOffset / 2]);
		}
	} else {
		uint32_t seed = pokemon.checksum;
		for (int byteOffset = 0x8; byteOffset <= 0x86; byteOffset += 2) {
			seed = AdvanceLcrng(seed);
			const uint16_t encrypted = static_cast<uint16_t>(pokemon.words[byteOffset / 2] ^ (seed >> 16));
			WriteArm9U16(monOffset + byteOffset, encrypted);
		}
	}

	if (mode.battleMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED) {
		for (int byteOffset = 0x88; byteOffset <= 0xEA; byteOffset += 2) {
			WriteArm9U16(monOffset + byteOffset, pokemon.words[byteOffset / 2]);
		}
	} else {
		uint32_t seed = pokemon.pid;
		for (int byteOffset = 0x88; byteOffset <= 0xEA; byteOffset += 2) {
			seed = AdvanceLcrng(seed);
			const uint16_t encrypted = static_cast<uint16_t>(pokemon.words[byteOffset / 2] ^ (seed >> 16));
			WriteArm9U16(monOffset + byteOffset, encrypted);
		}
	}
}

static void WriteEncryptedBoxPokemonGen5(const PokemonMemoryLayout *layout, uint32_t monOffset, DecryptedPokemon &pokemon)
{
	if (layout == NULL) {
		return;
	}

	const int coreEndOffset = static_cast<int>(0x8u + layout->coreBytes - 2u);
	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
		checksum = (checksum + pokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	pokemon.checksum = static_cast<uint16_t>(checksum);
	pokemon.checksumValid = true;
	WriteArm9U16(monOffset + 0x6, pokemon.checksum);

	uint32_t seed = pokemon.checksum;
	for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
		seed = AdvanceLcrng(seed);
		const uint16_t encrypted = static_cast<uint16_t>(pokemon.words[byteOffset / 2] ^ (seed >> 16));
		WriteArm9U16(monOffset + byteOffset, encrypted);
	}
}

static void WritePartyPokemonWithModesGen5(const PokemonMemoryLayout *layout, uint32_t monOffset, DecryptedPokemon &pokemon, const PartyPokemonReadMode &mode)
{
	if (layout == NULL) {
		return;
	}

	const int coreEndOffset = static_cast<int>(0x8u + layout->coreBytes - 2u);
	uint32_t checksum = 0;
	for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
		checksum = (checksum + pokemon.words[byteOffset / 2]) & 0xFFFFu;
	}

	pokemon.checksum = static_cast<uint16_t>(checksum);
	pokemon.checksumValid = true;
	WriteArm9U16(monOffset + 0x6, pokemon.checksum);

	if (mode.storageMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED) {
		for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
			WriteArm9U16(monOffset + byteOffset, pokemon.words[byteOffset / 2]);
		}
	} else {
		uint32_t seed = pokemon.checksum;
		for (int byteOffset = 0x8; byteOffset <= coreEndOffset; byteOffset += 2) {
			seed = AdvanceLcrng(seed);
			const uint16_t encrypted = static_cast<uint16_t>(pokemon.words[byteOffset / 2] ^ (seed >> 16));
			WriteArm9U16(monOffset + byteOffset, encrypted);
		}
	}

	const uint32_t tailStart = static_cast<uint32_t>(0x8u + layout->coreBytes);
	if (mode.battleMode == PARTY_MEMORY_MODE_ALREADY_DECRYPTED) {
		for (uint32_t byteOffset = tailStart; byteOffset < layout->partyMonSize; byteOffset += 2u) {
			WriteArm9U16(monOffset + byteOffset, pokemon.words[byteOffset / 2u]);
		}
	} else {
		uint32_t seed = pokemon.pid;
		for (uint32_t byteOffset = tailStart; byteOffset < layout->partyMonSize; byteOffset += 2u) {
			seed = AdvanceLcrng(seed);
			const uint16_t encrypted = static_cast<uint16_t>(pokemon.words[byteOffset / 2u] ^ (seed >> 16));
			WriteArm9U16(monOffset + byteOffset, encrypted);
		}
	}
}

static void AppendHexU32(std::ostringstream &out, uint32_t value)
{
	char buffer[16] = {0};
	std::snprintf(buffer, sizeof(buffer), "0x%08X", value);
	out << buffer;
}

static void AppendHexU16(std::ostringstream &out, uint16_t value)
{
	char buffer[16] = {0};
	std::snprintf(buffer, sizeof(buffer), "0x%04X", value);
	out << buffer;
}

static std::string BuildPartyReadBugReportDir()
{
	std::string batteryPath = TrimTrailingDirectorySeparators(path.pathToBattery);
	if (batteryPath.empty()) {
		batteryPath = TrimTrailingDirectorySeparators(path.pathToModule);
	}
	if (batteryPath.empty()) {
		batteryPath = ".";
	}
	const std::string modulePath = TrimTrailingDirectorySeparators(path.pathToModule);
	if (!batteryPath.empty() && !Path::IsPathRooted(batteryPath) && !modulePath.empty()) {
		batteryPath = modulePath + DIRECTORY_DELIMITER_CHAR + batteryPath;
	}

	return batteryPath + DIRECTORY_DELIMITER_CHAR + std::string("bugreports");
}

static std::string SanitizeDebugSnapshotFileName(const std::string &fileName)
{
	std::string safe;
	for (size_t i = 0; i < fileName.size(); ++i) {
		const unsigned char ch = static_cast<unsigned char>(fileName[i]);
		if (std::isalnum(ch) || ch == '_' || ch == '-') {
			safe.push_back(static_cast<char>(ch));
		}
	}

	return safe.empty() ? "debug_snapshot" : safe;
}

static bool WriteEditorDebugSnapshotInternal(
	const std::string &fileName,
	const std::string &text,
	std::string &pathOut,
	std::string &errorOut)
{
	pathOut.clear();
	errorOut.clear();
	if (text.empty()) {
		errorOut = "Debug snapshot is empty";
		return false;
	}

	const std::string reportDir = BuildPartyReadBugReportDir();
	if (!EnsureDirectoryExistsForWrite(reportDir)) {
		errorOut = "Failed to create bugreports folder";
		return false;
	}

	const std::string reportPath = reportDir + DIRECTORY_DELIMITER_CHAR + SanitizeDebugSnapshotFileName(fileName);
	if (!WriteTextFileAtomicallyForWrite(reportPath, text)) {
		errorOut = "Failed to write debug snapshot";
		return false;
	}

	pathOut = reportPath;
	return true;
}

static void AppendPartyReadBugReportWords(
	std::ostringstream &out,
	const char *label,
	const std::vector<uint16_t> &words)
{
	out << label << ":\n";
	for (size_t wordIndex = 0; wordIndex < words.size(); ++wordIndex) {
		if ((wordIndex % 8u) == 0u) {
			out << "  ";
		}
		AppendHexU16(out, words[wordIndex]);
		if (((wordIndex % 8u) == 7u) || ((wordIndex + 1u) == words.size())) {
			out << "\n";
		} else {
			out << " ";
		}
	}
}

static void AppendPartyReadBugReportBytes(std::ostringstream &out, const std::vector<uint8_t> &bytes)
{
	out << "raw_bytes:\n";
	for (size_t byteIndex = 0; byteIndex < bytes.size(); ++byteIndex) {
		if ((byteIndex % 16u) == 0u) {
			char addressBuffer[16] = {0};
			std::snprintf(addressBuffer, sizeof(addressBuffer), "%04X", static_cast<unsigned int>(byteIndex));
			out << addressBuffer << ": ";
		}

		char byteBuffer[8] = {0};
		std::snprintf(byteBuffer, sizeof(byteBuffer), "%02X", static_cast<unsigned int>(bytes[byteIndex]));
		out << byteBuffer;
		if (((byteIndex % 16u) == 15u) || ((byteIndex + 1u) == bytes.size())) {
			out << "\n";
		} else {
			out << " ";
		}
	}
}

static bool WriteFullPartyRawBytesDebugSnapshotInternal(
	ExportGameProfile selectedProfile,
	std::string &pathOut,
	std::string &errorOut)
{
	pathOut.clear();
	errorOut.clear();

	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0u;
	uint32_t partyBaseAddr = 0u;
	uint8_t partyCount = 0u;
	std::string resolveError;
	if (!ResolvePartyContext(selectedProfile, profile, pidAddr, partyBaseAddr, partyCount, resolveError)) {
		errorOut = resolveError;
		return false;
	}

	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;

	std::ostringstream report;
	report << "Full party raw dump\n";
	report << "profile: " << ((profile != NULL && profile->displayName != NULL) ? profile->displayName : "") << "\n";
	report << "game_code: " << GameCodeString() << "\n";
	report << "party_count: " << static_cast<int>(partyCount) << "\n";
	report << "party_base_addr: ";
	AppendHexU32(report, partyBaseAddr);
	report << "\nparty_mon_size: " << layout->partyMonSize << "\n";
	report << "codec: " << static_cast<int>(layout->codec) << "\n";

	static const uint32_t kFullPartyDumpSlotCount = 6u;
	for (uint32_t slot = 0; slot < kFullPartyDumpSlotCount; ++slot) {
		const uint32_t monOffset = partyBaseAddr + (slot * static_cast<uint32_t>(layout->partyMonSize));
		std::vector<uint8_t> monBytes;
		SnapshotArm9Bytes(monOffset, layout->partyMonSize, monBytes);

		report << "\nslot: " << (slot + 1u) << "\n";
		report << "mon_offset: ";
		AppendHexU32(report, monOffset);
		report << "\n";
		if (monBytes.size() >= 8u) {
			report << "raw_pid: ";
			AppendHexU32(report, ReadSnapshotU32(monBytes, 0x0u));
			report << "\nraw_checksum: ";
			AppendHexU16(report, ReadSnapshotU16(monBytes, 0x6u));
			report << "\n";
		}
		AppendPartyReadBugReportBytes(report, monBytes);
	}

	return WriteEditorDebugSnapshotInternal("error_log_full_party_raw", report.str(), pathOut, errorOut);
}

static void WriteFailedPartySlotReadBugReport(
	const ExportProfileDefinition *profile,
	uint32_t partyBaseAddr,
	uint8_t partyCount,
	int slot,
	uint32_t monOffset,
	const char *context,
	const DecryptedPokemon *decodedPokemon = NULL)
{
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL || slot < 1) {
		return;
	}

	const std::string reportDir = BuildPartyReadBugReportDir();
	if (!EnsureDirectoryExistsForWrite(reportDir)) {
		return;
	}

	std::vector<uint8_t> monBytes;
	SnapshotArm9Bytes(monOffset, layout->partyMonSize, monBytes);

	std::ostringstream report;
	report << "Failed to read party slot " << slot << "\n";
	report << "context: " << ((context != NULL) ? context : "") << "\n";
	report << "profile: " << ((profile != NULL && profile->displayName != NULL) ? profile->displayName : "") << "\n";
	report << "game_code: " << GameCodeString() << "\n";
	report << "party_count: " << static_cast<int>(partyCount) << "\n";
	report << "party_base_addr: ";
	AppendHexU32(report, partyBaseAddr);
	report << "\nmon_offset: ";
	AppendHexU32(report, monOffset);
	report << "\nparty_mon_size: " << layout->partyMonSize << "\n";
	report << "codec: " << static_cast<int>(layout->codec) << "\n";

	if (monBytes.size() >= 8u) {
		report << "raw_pid: ";
		AppendHexU32(report, ReadSnapshotU32(monBytes, 0x0u));
		report << "\nraw_checksum: ";
		AppendHexU16(report, ReadSnapshotU16(monBytes, 0x6u));
		report << "\n";
	}

	if (decodedPokemon != NULL) {
		const int shuffleKey = static_cast<int>(((decodedPokemon->pid & 0x3E000u) >> 13) % 24u);
		const int blockB = kShuffleTable[shuffleKey][1];
		report << "decoded_moves_all_display_empty: "
			<< (AreAllPartyMoveSlotsDisplayEmpty(profile, *decodedPokemon) ? "true" : "false") << "\n";
		report << "decoded_move_ids:";
		for (int i = 0; i < 4; ++i) {
			report << " " << GetWord(*decodedPokemon, blockB + (i * 2));
		}
		report << "\n";
		report << "decoded_move_display_empty:";
		for (int i = 0; i < 4; ++i) {
			report << " "
				<< (IsPartyMoveSlotDisplayEmpty(profile, GetWord(*decodedPokemon, blockB + (i * 2))) ? "true" : "false");
		}
		report << "\n";
	}

	const uint32_t tailStart = static_cast<uint32_t>(0x8u + layout->coreBytes);
	if (tailStart < layout->partyMonSize) {
		const size_t tailWordCount = (layout->partyMonSize - tailStart) / 2u;
		std::vector<uint16_t> rawTailWords(tailWordCount, 0);
		std::vector<uint16_t> decryptedTailWords(tailWordCount, 0);
		uint32_t tailSeed = (monBytes.size() >= 4u) ? ReadSnapshotU32(monBytes, 0x0u) : 0u;
		for (uint32_t byteOffset = tailStart; byteOffset + 1u < layout->partyMonSize; byteOffset += 2u) {
			const size_t localIndex = static_cast<size_t>((byteOffset - tailStart) / 2u);
			const uint16_t rawWord = ReadSnapshotU16(monBytes, static_cast<size_t>(byteOffset));
			rawTailWords[localIndex] = rawWord;
			tailSeed = AdvanceLcrng(tailSeed);
			decryptedTailWords[localIndex] = static_cast<uint16_t>(rawWord ^ (tailSeed >> 16));
		}

		report << "raw_tail_reasonable: "
			<< ((layout->codec == POKEMON_DATA_CODEC_GEN5)
				? (IsReasonableGen5PartyTailWords(rawTailWords) ? "true" : "false")
				: (IsReasonableBattleTailWords(rawTailWords) ? "true" : "false"))
			<< "\n";
		report << "decrypted_tail_reasonable: "
			<< ((layout->codec == POKEMON_DATA_CODEC_GEN5)
				? (IsReasonableGen5PartyTailWords(decryptedTailWords) ? "true" : "false")
				: (IsReasonableBattleTailWords(decryptedTailWords) ? "true" : "false"))
			<< "\n";
		report << "raw_tail_has_stat_over_" << kMaxReasonablePartyNonHpStat << ": "
			<< (HasOverlargePartyNonHpStat(rawTailWords) ? "true" : "false") << "\n";
		report << "decrypted_tail_has_stat_over_" << kMaxReasonablePartyNonHpStat << ": "
			<< (HasOverlargePartyNonHpStat(decryptedTailWords) ? "true" : "false") << "\n";
		AppendPartyReadBugReportWords(report, "raw_tail_words", rawTailWords);
		AppendPartyReadBugReportWords(report, "decrypted_tail_words", decryptedTailWords);
	}

	AppendPartyReadBugReportBytes(report, monBytes);

	const std::string reportPath = reportDir + DIRECTORY_DELIMITER_CHAR +
		std::string("error_log_slot_") + std::to_string(slot);
	WriteTextFileAtomicallyForWrite(reportPath, report.str());
}

static void WriteFailedBoxSlotReadBugReport(
	const ExportProfileDefinition *profile,
	uint32_t boxBaseAddr,
	uint32_t liveBoxCount,
	int box,
	int slot,
	uint32_t monOffset,
	const char *context)
{
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL || box < 1 || slot < 1 || layout->boxMonSize == 0u || layout->partySlotsPerBox == 0u) {
		return;
	}

	const std::string reportDir = BuildPartyReadBugReportDir();
	if (!EnsureDirectoryExistsForWrite(reportDir)) {
		return;
	}

	std::ostringstream report;
	report << "Failed to read box slot " << box << ":" << slot << "\n";
	report << "context: " << ((context != NULL) ? context : "") << "\n";
	report << "profile: " << ((profile != NULL && profile->displayName != NULL) ? profile->displayName : "") << "\n";
	report << "game_code: " << GameCodeString() << "\n";
	report << "updated_at_unix_ms: " << CurrentUnixMs() << "\n";
	report << "box: " << box << "\n";
	report << "slot: " << slot << "\n";
	report << "live_box_count: " << liveBoxCount << "\n";
	report << "box_base_addr: ";
	AppendHexU32(report, boxBaseAddr);
	report << "\nfailed_mon_offset: ";
	AppendHexU32(report, monOffset);
	report << "\nbox_mon_size: " << layout->boxMonSize << "\n";
	report << "box_slots_per_box: " << layout->partySlotsPerBox << "\n";
	report << "box_stride_bytes: " << GetBoxStrideBytes(layout) << "\n";
	report << "codec: " << static_cast<int>(layout->codec) << "\n";

	for (uint32_t slotIndex = 0; slotIndex < static_cast<uint32_t>(layout->partySlotsPerBox); ++slotIndex) {
		const uint32_t slotOffset = GetBoxMonAddress(layout, boxBaseAddr, static_cast<uint32_t>(box - 1), slotIndex);
		std::vector<uint8_t> monBytes;
		SnapshotArm9Bytes(slotOffset, layout->boxMonSize, monBytes);
		const bool empty = IsAllZeroBytes(monBytes);

		report << "\nbox_slot: " << box << ":" << (slotIndex + 1u) << "\n";
		report << "mon_offset: ";
		AppendHexU32(report, slotOffset);
		report << "\nempty: " << (empty ? "true" : "false") << "\n";
		if (monBytes.size() >= 8u) {
			report << "raw_pid: ";
			AppendHexU32(report, ReadSnapshotU32(monBytes, 0x0u));
			report << "\nraw_checksum: ";
			AppendHexU16(report, ReadSnapshotU16(monBytes, 0x6u));
			report << "\n";
		}

		if (!empty) {
			DecryptedPokemon decodedPokemon;
			const bool decoded = TryDecryptPokemonWithRetriesForProfile(profile, slotOffset, true, decodedPokemon);
			report << "decoded: " << (decoded ? "true" : "false") << "\n";
			if (decoded) {
				report << "decoded_pid: ";
				AppendHexU32(report, decodedPokemon.pid);
				report << "\ndecoded_checksum: ";
				AppendHexU16(report, decodedPokemon.checksum);
				report << "\ndecoded_checksum_valid: " << (decodedPokemon.checksumValid ? "true" : "false") << "\n";
				report << "decoded_species_id: " << GetDecryptedPokemonSpeciesId(decodedPokemon) << "\n";
			}
		}

		AppendPartyReadBugReportBytes(report, monBytes);
	}

	const std::string reportPath = reportDir + DIRECTORY_DELIMITER_CHAR +
		std::string("box") + std::to_string(box) + "slot" + std::to_string(slot) + "failreport";
	WriteTextFileAtomicallyForWrite(reportPath, report.str());
}

static bool BuildPartyEditorSnapshotAttempt(ExportGameProfile selectedProfile, PartyEditorSnapshot &snapshot)
{
	snapshot.success = false;
	snapshot.error.clear();
	snapshot.party.clear();

	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t partyBaseAddr = 0;
	uint8_t partyCount = 0;
	std::string error;
	if (!ResolvePartyContext(selectedProfile, profile, pidAddr, partyBaseAddr, partyCount, error)) {
		snapshot.error = error;
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		snapshot.error = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;

	int failingSlot = 0;
	PartyFrameAnalysis failingAnalysis;
	failingAnalysis.classification = PARTY_FRAME_INVALID;
	failingAnalysis.hasDecodedPokemon = false;

	for (int attempt = 0; attempt < kPartyEditorSnapshotRetryCount; ++attempt) {
		std::vector<PartyPokemonSummary> candidateParty;
		candidateParty.reserve(partyCount);
		bool shouldRetry = false;
		failingSlot = 0;
		failingAnalysis.classification = PARTY_FRAME_INVALID;
		failingAnalysis.hasDecodedPokemon = false;

		for (uint32_t slot = 0; slot < partyCount; ++slot) {
			const uint32_t monOffset = partyBaseAddr + (slot * static_cast<uint32_t>(layout->partyMonSize));
			PartyFrameAnalysis analysis;
			AnalyzePartyPokemonFrameForProfile(profile, monOffset, analysis);
			if (analysis.classification != PARTY_FRAME_VALID) {
				shouldRetry = true;
				failingSlot = static_cast<int>(slot + 1);
				failingAnalysis = analysis;
				break;
			}

			PartyPokemonSummary summary;
			if (!BuildPartyPokemonSummary(profile, analysis.pokemon, monOffset, static_cast<int>(slot + 1), summary)) {
				snapshot.error = "Failed to summarize party slot " + std::to_string(slot + 1);
				return false;
			}

			candidateParty.push_back(summary);
		}

		if (!shouldRetry) {
			snapshot.party = candidateParty;
			snapshot.success = true;
			return true;
		}

		if ((attempt + 1) < kPartyEditorSnapshotRetryCount) {
			std::this_thread::sleep_for(std::chrono::milliseconds(kPartyEditorSnapshotRetryDelayMs));
		}
	}

	if (failingSlot >= 1) {
		const uint32_t monOffset = partyBaseAddr + (static_cast<uint32_t>(failingSlot - 1) * static_cast<uint32_t>(layout->partyMonSize));
		DecryptedPokemon reportPokemonStorage;
		const DecryptedPokemon *failedPokemonForReport = NULL;
		const std::string failureContext = DescribePartyReadFailureContextForReport(
			profile,
			monOffset,
			"party_editor_snapshot",
			&failingAnalysis,
			reportPokemonStorage,
			failedPokemonForReport);

		WriteFailedPartySlotReadBugReport(
			profile,
			partyBaseAddr,
			partyCount,
			failingSlot,
			monOffset,
			failureContext.c_str(),
			failedPokemonForReport);
		snapshot.error = "Failed to read party slot " + std::to_string(failingSlot);
		return false;
	}

	snapshot.error = "Failed to read party snapshot";
	return false;
}

static bool BuildPartyEditorSnapshotInternal(ExportGameProfile selectedProfile, PartyEditorSnapshot &snapshot)
{
	return BuildPartyEditorSnapshotAttempt(selectedProfile, snapshot);
}

static bool BuildBoxEditorSnapshotInternal(ExportGameProfile selectedProfile, BoxEditorSnapshot &snapshot)
{
	snapshot.success = false;
	snapshot.error.clear();
	snapshot.currentBox = 1;
	snapshot.boxCount = 0;
	snapshot.pokemon.clear();

	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t boxBaseAddr = 0;
	uint8_t currentBoxIndex = 0;
	if (!ResolveBoxContext(selectedProfile, profile, pidAddr, boxBaseAddr, currentBoxIndex, snapshot.error)) {
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		snapshot.error = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;

	snapshot.currentBox = static_cast<int>(currentBoxIndex) + 1;
	snapshot.boxCount = static_cast<int>(layout->exportBoxCount);

	for (uint32_t box = 0; box < layout->exportBoxCount; ++box) {
		for (uint32_t slot = 0; slot < layout->partySlotsPerBox; ++slot) {
			const uint32_t monOffset = GetBoxMonAddress(layout, boxBaseAddr, box, slot);
			if (IsBoxPokemonSlotEmpty(layout, monOffset)) {
				continue;
			}

			DecryptedPokemon pokemon;
			if (!ReadBoxPokemonStrictForEditorForProfileWithAttempts(
					profile,
					monOffset,
					pokemon,
					kPartyEditorReadRetryCount,
					kPartyEditorReadRetryDelayMs)) {
				snapshot.error = "Failed to read box " + std::to_string(box + 1) + " slot " + std::to_string(slot + 1);
				return false;
			}
			if (GetDecryptedPokemonSpeciesId(pokemon) == 0u) {
				continue;
			}

			BoxPokemonSummary summary;
			if (!BuildBoxPokemonSummary(profile, pokemon, monOffset, static_cast<int>(box + 1), static_cast<int>(slot + 1), summary)) {
				snapshot.error = "Failed to summarize box " + std::to_string(box + 1) + " slot " + std::to_string(slot + 1);
				return false;
			}

			snapshot.pokemon.push_back(summary);
		}
	}

	snapshot.success = true;
	return true;
}

static bool IsPartyPokemonReadyForWriteInternal(ExportGameProfile selectedProfile, int slot, std::string &errorOut)
{
	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t partyBaseAddr = 0;
	uint8_t partyCount = 0;
	if (!ResolvePartyContext(selectedProfile, profile, pidAddr, partyBaseAddr, partyCount, errorOut)) {
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;

	if (slot < 1 || slot > static_cast<int>(partyCount)) {
		errorOut = "Party slot is out of range";
		return false;
	}

	const uint32_t monOffset = partyBaseAddr + (static_cast<uint32_t>(slot - 1) * static_cast<uint32_t>(layout->partyMonSize));
	DecryptedPokemon pokemon;
	PartyPokemonReadMode readMode;
	if (!ReadPartyPokemonStrictForWriteForProfileWithAttempts(profile, monOffset, pokemon, readMode, 1, 0, NULL)) {
		errorOut = "Waiting for stable party slot " + std::to_string(slot);
		return false;
	}

	errorOut.clear();
	return true;
}

static bool IsEntirePartyReadyForWriteInternal(ExportGameProfile selectedProfile, std::string &errorOut)
{
	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t partyBaseAddr = 0;
	uint8_t partyCount = 0;
	if (!ResolvePartyContext(selectedProfile, profile, pidAddr, partyBaseAddr, partyCount, errorOut)) {
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;

	for (int slot = 1; slot <= static_cast<int>(partyCount); ++slot) {
		const uint32_t monOffset = partyBaseAddr + (static_cast<uint32_t>(slot - 1) * static_cast<uint32_t>(layout->partyMonSize));
		DecryptedPokemon pokemon;
		PartyPokemonReadMode readMode;
		if (!ReadPartyPokemonStrictForWriteForProfileWithAttempts(profile, monOffset, pokemon, readMode, 1, 0, NULL)) {
			errorOut = "Waiting for stable party slot " + std::to_string(slot);
			return false;
		}
	}

	errorOut.clear();
	return true;
}

static bool UpdatePartyPokemonHpStatusInternal(ExportGameProfile selectedProfile, const PartyPokemonHpStatusEditRequest &request, std::string &errorOut)
{
	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t partyBaseAddr = 0;
	uint8_t partyCount = 0;
	if (!ResolvePartyContext(selectedProfile, profile, pidAddr, partyBaseAddr, partyCount, errorOut)) {
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;

	if (request.slot < 1 || request.slot > static_cast<int>(partyCount)) {
		errorOut = "Party slot is out of range";
		return false;
	}

	const uint32_t monOffset = partyBaseAddr + (static_cast<uint32_t>(request.slot - 1) * static_cast<uint32_t>(layout->partyMonSize));
	DecryptedPokemon pokemon;
	PartyPokemonReadMode readMode;
	PartyFrameAnalysis failedAnalysis;
	if (!ReadPartyPokemonStrictForWriteForProfileWithAttempts(
			profile,
			monOffset,
			pokemon,
			readMode,
			kPartyWriteReadRetryCount,
			kPartyWriteReadRetryDelayMs,
			&failedAnalysis)) {
		if (failedAnalysis.classification == PARTY_FRAME_TRANSIENT_PLAINTEXT) {
			errorOut = "Waiting for stable party slot " + std::to_string(request.slot);
			return false;
		}

		DecryptedPokemon reportPokemonStorage;
		const DecryptedPokemon *failedPokemonForReport = NULL;
		const std::string failureContext = DescribePartyReadFailureContextForReport(
			profile,
			monOffset,
			"update_party_hp_status",
			&failedAnalysis,
			reportPokemonStorage,
			failedPokemonForReport);
		WriteFailedPartySlotReadBugReport(
			profile,
			partyBaseAddr,
			partyCount,
			request.slot,
			monOffset,
			failureContext.c_str(),
			failedPokemonForReport);
		errorOut = "Failed to read party slot " + std::to_string(request.slot);
		return false;
	}

	const int maxHp = GetWord(pokemon, 0x90);
	if (maxHp <= 0) {
		errorOut = "Party Pokemon max HP is unavailable";
		return false;
	}

	const int originalCurrentHp = static_cast<int>(GetWord(pokemon, 0x8E));
	const uint32_t originalStatusRaw = GetPartyStatusRawFromPokemon(pokemon);
	const int expectedCurrentHp = request.preserveCurrentHp
		? originalCurrentHp
		: std::max(0, std::min(request.currentHp, maxHp));
	const uint32_t expectedStatusRaw = request.preserveEncodedStatusRaw
		? request.encodedStatusRaw
		: EncodePartyStatusForProfile(profile, request.statusRaw);

	pokemon.words[0x8E / 2] = static_cast<uint16_t>(expectedCurrentHp);
	SetPartyStatusRawOnPokemon(pokemon, expectedStatusRaw);

	return WriteVerifiedPartyHpStatusForProfile(
		profile,
		monOffset,
		request.slot,
		pokemon,
		readMode,
		originalCurrentHp,
		originalStatusRaw,
		expectedCurrentHp,
		expectedStatusRaw,
		errorOut);
}

static bool UpdatePartyPokemonInternal(ExportGameProfile selectedProfile, const PartyPokemonEditRequest &request, std::string &errorOut)
{
	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t partyBaseAddr = 0;
	uint8_t partyCount = 0;
	if (!ResolvePartyContext(selectedProfile, profile, pidAddr, partyBaseAddr, partyCount, errorOut)) {
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;

	if (request.slot < 1 || request.slot > static_cast<int>(partyCount)) {
		errorOut = "Party slot is out of range";
		return false;
	}

	const uint32_t monOffset = partyBaseAddr + (static_cast<uint32_t>(request.slot - 1) * static_cast<uint32_t>(layout->partyMonSize));
	DecryptedPokemon pokemon;
	PartyPokemonReadMode readMode;
	PartyFrameAnalysis failedAnalysis;
	if (!ReadPartyPokemonStrictForWriteForProfileWithAttempts(
			profile,
			monOffset,
			pokemon,
			readMode,
			kPartyWriteReadRetryCount,
			kPartyWriteReadRetryDelayMs,
			&failedAnalysis)) {
		if (failedAnalysis.classification == PARTY_FRAME_TRANSIENT_PLAINTEXT) {
			errorOut = "Waiting for stable party slot " + std::to_string(request.slot);
			return false;
		}

		DecryptedPokemon reportPokemonStorage;
		const DecryptedPokemon *failedPokemonForReport = NULL;
		const std::string failureContext = DescribePartyReadFailureContextForReport(
			profile,
			monOffset,
			"update_party_pokemon",
			&failedAnalysis,
			reportPokemonStorage,
			failedPokemonForReport);
		WriteFailedPartySlotReadBugReport(
			profile,
			partyBaseAddr,
			partyCount,
			request.slot,
			monOffset,
			failureContext.c_str(),
			failedPokemonForReport);
		errorOut = "Failed to read party slot " + std::to_string(request.slot);
		return false;
	}

	const int maxHp = GetWord(pokemon, 0x90);
	if (maxHp <= 0) {
		errorOut = "Party Pokemon max HP is unavailable";
		return false;
	}

	const int clampedHp = std::max(0, std::min(request.currentHp, maxHp));
	const uint32_t encodedStatusRaw = request.preserveEncodedStatusRaw
		? request.encodedStatusRaw
		: EncodePartyStatusForProfile(profile, request.statusRaw);
	const uint32_t originalStatusRaw = GetPartyStatusRawFromPokemon(pokemon);
	pokemon.words[0x8E / 2] = static_cast<uint16_t>(clampedHp);
	SetPartyStatusRawOnPokemon(pokemon, encodedStatusRaw);

	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const int blockB = kShuffleTable[shuffleKey][1];
	const int blockC = kShuffleTable[shuffleKey][2];
	const uint16_t originalSpeciesId = GetWord(pokemon, blockA + 0x0);
	const uint16_t speciesId = (request.speciesId != 0u) ? request.speciesId : originalSpeciesId;
	const uint16_t heldItemId = request.heldItemId;
	const uint16_t abilityFriendship = static_cast<uint16_t>((static_cast<uint16_t>(request.abilityId & 0xFFu) << 8) | request.friendship);

	pokemon.words[(blockA + 0x0) / 2] = speciesId;
	pokemon.words[(blockA + 0x2) / 2] = heldItemId;
	pokemon.words[(blockA + 0xC) / 2] = abilityFriendship;
	pokemon.words[(blockA + 0x10) / 2] = static_cast<uint16_t>((static_cast<uint16_t>(request.evs[1] & 0xFFu) << 8) | request.evs[0]);
	pokemon.words[(blockA + 0x12) / 2] = static_cast<uint16_t>((static_cast<uint16_t>(request.evs[5] & 0xFFu) << 8) | request.evs[2]);
	pokemon.words[(blockA + 0x14) / 2] = static_cast<uint16_t>((static_cast<uint16_t>(request.evs[4] & 0xFFu) << 8) | request.evs[3]);

	ApplyRequestedMovesPreservingPp(profile, pokemon, blockB, request.moveIds);

	uint32_t ivData = static_cast<uint32_t>(GetWord(pokemon, blockB + 0x10)) |
		(static_cast<uint32_t>(GetWord(pokemon, blockB + 0x12)) << 16);
	ivData &= 0xC0000000u;
	ivData |= static_cast<uint32_t>(request.ivs[0] & 0x1Fu);
	ivData |= static_cast<uint32_t>(request.ivs[1] & 0x1Fu) << 5;
	ivData |= static_cast<uint32_t>(request.ivs[2] & 0x1Fu) << 10;
	ivData |= static_cast<uint32_t>(request.ivs[5] & 0x1Fu) << 15;
	ivData |= static_cast<uint32_t>(request.ivs[3] & 0x1Fu) << 20;
	ivData |= static_cast<uint32_t>(request.ivs[4] & 0x1Fu) << 25;
	pokemon.words[(blockB + 0x10) / 2] = static_cast<uint16_t>(ivData & 0xFFFFu);
	pokemon.words[(blockB + 0x12) / 2] = static_cast<uint16_t>((ivData >> 16) & 0xFFFFu);
	if (speciesId != originalSpeciesId && !UpdateDefaultSpeciesNicknameIfNeeded(profile, pokemon, blockB, blockC, speciesId)) {
		const PokemonMemoryLayout *layoutInfo = GetProfilePokemonLayout(profile);
		if (layoutInfo != NULL && layoutInfo->codec == POKEMON_DATA_CODEC_GEN4) {
			errorOut = "Failed to update default nickname for new species";
			return false;
		}
	}

	const uint32_t minExperienceForLevel = GetProfileExperienceForLevel(profile, 1, speciesId);
	const uint32_t maxExperienceForLevel = GetProfileExperienceForLevel(profile, 100, speciesId);
	const uint32_t clampedExperience = std::max(minExperienceForLevel, std::min(request.experience, maxExperienceForLevel));
	pokemon.words[(blockA + 0x8) / 2] = static_cast<uint16_t>(clampedExperience & 0xFFFFu);
	pokemon.words[(blockA + 0xA) / 2] = static_cast<uint16_t>((clampedExperience >> 16) & 0xFFFFu);

	return WriteVerifiedPartyPokemonForProfile(
		profile,
		monOffset,
		request.slot,
		pokemon,
		readMode,
		originalStatusRaw,
		encodedStatusRaw,
		errorOut);
}

static bool UpdateBoxPokemonInternal(ExportGameProfile selectedProfile, const BoxPokemonEditRequest &request, std::string &errorOut)
{
	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t boxBaseAddr = 0;
	uint8_t currentBoxIndex = 0;
	if (!ResolveBoxContext(selectedProfile, profile, pidAddr, boxBaseAddr, currentBoxIndex, errorOut)) {
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;
	(void)currentBoxIndex;

	if (request.box < 1 || request.box > static_cast<int>(layout->exportBoxCount)) {
		errorOut = "Box number is out of range";
		return false;
	}
	if (request.slot < 1 || request.slot > static_cast<int>(layout->partySlotsPerBox)) {
		errorOut = "Box slot is out of range";
		return false;
	}

	const uint32_t monOffset = GetBoxMonAddress(
		layout,
		boxBaseAddr,
		static_cast<uint32_t>(request.box - 1),
		static_cast<uint32_t>(request.slot - 1));
	DecryptedPokemon pokemon;
	if (!ReadBoxPokemonStrictForEditorForProfile(profile, monOffset, pokemon)) {
		errorOut = "Failed to read box " + std::to_string(request.box) + " slot " + std::to_string(request.slot);
		return false;
	}

	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const int blockB = kShuffleTable[shuffleKey][1];
	const int blockC = kShuffleTable[shuffleKey][2];
	const uint16_t originalSpeciesId = GetWord(pokemon, blockA + 0x0);
	if (originalSpeciesId == 0u) {
		errorOut = "Box " + std::to_string(request.box) + " slot " + std::to_string(request.slot) + " is empty";
		return false;
	}
	const uint16_t speciesId = (request.speciesId != 0u) ? request.speciesId : originalSpeciesId;
	const uint16_t heldItemId = request.heldItemId;
	const uint16_t abilityFriendship = static_cast<uint16_t>((static_cast<uint16_t>(request.abilityId & 0xFFu) << 8) | request.friendship);

	pokemon.words[(blockA + 0x0) / 2] = speciesId;
	pokemon.words[(blockA + 0x2) / 2] = heldItemId;
	pokemon.words[(blockA + 0xC) / 2] = abilityFriendship;
	pokemon.words[(blockA + 0x10) / 2] = static_cast<uint16_t>((static_cast<uint16_t>(request.evs[1] & 0xFFu) << 8) | request.evs[0]);
	pokemon.words[(blockA + 0x12) / 2] = static_cast<uint16_t>((static_cast<uint16_t>(request.evs[5] & 0xFFu) << 8) | request.evs[2]);
	pokemon.words[(blockA + 0x14) / 2] = static_cast<uint16_t>((static_cast<uint16_t>(request.evs[4] & 0xFFu) << 8) | request.evs[3]);

	const uint32_t minExperienceForLevel = GetProfileExperienceForLevel(profile, 1, speciesId);
	const uint32_t maxExperienceForLevel = GetProfileExperienceForLevel(profile, 100, speciesId);
	const uint32_t clampedExperience = std::max(minExperienceForLevel, std::min(request.experience, maxExperienceForLevel));
	pokemon.words[(blockA + 0x8) / 2] = static_cast<uint16_t>(clampedExperience & 0xFFFFu);
	pokemon.words[(blockA + 0xA) / 2] = static_cast<uint16_t>((clampedExperience >> 16) & 0xFFFFu);

	ApplyRequestedMovesPreservingPp(profile, pokemon, blockB, request.moveIds);

	uint32_t ivData = static_cast<uint32_t>(GetWord(pokemon, blockB + 0x10)) |
		(static_cast<uint32_t>(GetWord(pokemon, blockB + 0x12)) << 16);
	ivData &= 0xC0000000u;
	ivData |= static_cast<uint32_t>(request.ivs[0] & 0x1Fu);
	ivData |= static_cast<uint32_t>(request.ivs[1] & 0x1Fu) << 5;
	ivData |= static_cast<uint32_t>(request.ivs[2] & 0x1Fu) << 10;
	ivData |= static_cast<uint32_t>(request.ivs[5] & 0x1Fu) << 15;
	ivData |= static_cast<uint32_t>(request.ivs[3] & 0x1Fu) << 20;
	ivData |= static_cast<uint32_t>(request.ivs[4] & 0x1Fu) << 25;
	pokemon.words[(blockB + 0x10) / 2] = static_cast<uint16_t>(ivData & 0xFFFFu);
	pokemon.words[(blockB + 0x12) / 2] = static_cast<uint16_t>((ivData >> 16) & 0xFFFFu);
	if (speciesId != originalSpeciesId && !UpdateDefaultSpeciesNicknameIfNeeded(profile, pokemon, blockB, blockC, speciesId)) {
		const PokemonMemoryLayout *layoutInfo = GetProfilePokemonLayout(profile);
		if (layoutInfo != NULL && layoutInfo->codec == POKEMON_DATA_CODEC_GEN4) {
			errorOut = "Failed to update default nickname for new species";
			return false;
		}
	}

	WriteEncryptedBoxPokemonForProfile(profile, monOffset, pokemon);
	errorOut.clear();
	return true;
}

static bool EdgePartyPokemonInternal(ExportGameProfile selectedProfile, int slot, uint32_t amount, std::string &errorOut)
{
	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t partyBaseAddr = 0;
	uint8_t partyCount = 0;
	if (!ResolvePartyContext(selectedProfile, profile, pidAddr, partyBaseAddr, partyCount, errorOut)) {
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;

	if (slot < 1 || slot > static_cast<int>(partyCount)) {
		errorOut = "Party slot is out of range";
		return false;
	}

	const uint32_t monOffset = partyBaseAddr + (static_cast<uint32_t>(slot - 1) * static_cast<uint32_t>(layout->partyMonSize));
	DecryptedPokemon pokemon;
	PartyPokemonReadMode readMode;
	PartyFrameAnalysis failedAnalysis;
	if (!ReadPartyPokemonStrictForWriteForProfileWithAttempts(
			profile,
			monOffset,
			pokemon,
			readMode,
			kPartyWriteReadRetryCount,
			kPartyWriteReadRetryDelayMs,
			&failedAnalysis)) {
		if (failedAnalysis.classification == PARTY_FRAME_TRANSIENT_PLAINTEXT) {
			errorOut = "Waiting for stable party slot " + std::to_string(slot);
			return false;
		}

		DecryptedPokemon reportPokemonStorage;
		const DecryptedPokemon *failedPokemonForReport = NULL;
		const std::string failureContext = DescribePartyReadFailureContextForReport(
			profile,
			monOffset,
			"edge_party_pokemon",
			&failedAnalysis,
			reportPokemonStorage,
			failedPokemonForReport);
		WriteFailedPartySlotReadBugReport(
			profile,
			partyBaseAddr,
			partyCount,
			slot,
			monOffset,
			failureContext.c_str(),
			failedPokemonForReport);
		errorOut = "Failed to read party slot " + std::to_string(slot);
		return false;
	}

	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const uint16_t speciesId = GetWord(pokemon, blockA + 0x0);
	const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
	                           (static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);
	const int currentLevel = GetProfileLevelForExperience(profile, experience, speciesId);
	if (currentLevel >= 100) {
		errorOut = "Cannot edge a level 100 Pokemon";
		return false;
	}

	const uint32_t targetExperience = GetProfileExperienceForLevel(profile, currentLevel + 1, speciesId);
	if (targetExperience <= amount) {
		errorOut = "Invalid edge amount";
		return false;
	}

	const uint32_t edgedExperience = targetExperience - amount;
	if (edgedExperience < GetProfileExperienceForLevel(profile, currentLevel, speciesId)) {
		errorOut = "Invalid edge amount";
		return false;
	}

	pokemon.words[(blockA + 0x8) / 2] = static_cast<uint16_t>(edgedExperience & 0xFFFFu);
	pokemon.words[(blockA + 0xA) / 2] = static_cast<uint16_t>((edgedExperience >> 16) & 0xFFFFu);
	WritePartyPokemonWithModesForProfile(profile, monOffset, pokemon, readMode);
	errorOut.clear();
	return true;
}

static bool EdgeBoxPokemonInternal(ExportGameProfile selectedProfile, int box, int slot, uint32_t amount, std::string &errorOut)
{
	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t boxBaseAddr = 0;
	uint8_t currentBoxIndex = 0;
	if (!ResolveBoxContext(selectedProfile, profile, pidAddr, boxBaseAddr, currentBoxIndex, errorOut)) {
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;
	(void)currentBoxIndex;

	if (box < 1 || box > static_cast<int>(layout->exportBoxCount)) {
		errorOut = "Box number is out of range";
		return false;
	}
	if (slot < 1 || slot > static_cast<int>(layout->partySlotsPerBox)) {
		errorOut = "Box slot is out of range";
		return false;
	}

	const uint32_t monOffset = GetBoxMonAddress(
		layout,
		boxBaseAddr,
		static_cast<uint32_t>(box - 1),
		static_cast<uint32_t>(slot - 1));
	DecryptedPokemon pokemon;
	if (!ReadBoxPokemonStrictForEditorForProfile(profile, monOffset, pokemon)) {
		errorOut = "Failed to read box " + std::to_string(box) + " slot " + std::to_string(slot);
		return false;
	}

	const int shuffleKey = static_cast<int>(((pokemon.pid & 0x3E000u) >> 13) % 24u);
	const int blockA = kShuffleTable[shuffleKey][0];
	const uint16_t speciesId = GetWord(pokemon, blockA + 0x0);
	if (speciesId == 0u) {
		errorOut = "Box " + std::to_string(box) + " slot " + std::to_string(slot) + " is empty";
		return false;
	}
	const uint32_t experience = static_cast<uint32_t>(GetWord(pokemon, blockA + 0x8)) |
	                           (static_cast<uint32_t>(GetWord(pokemon, blockA + 0xA)) << 16);
	const int currentLevel = GetProfileLevelForExperience(profile, experience, speciesId);
	if (currentLevel >= 100) {
		errorOut = "Cannot edge a level 100 Pokemon";
		return false;
	}

	const uint32_t targetExperience = GetProfileExperienceForLevel(profile, currentLevel + 1, speciesId);
	if (targetExperience <= amount) {
		errorOut = "Invalid edge amount";
		return false;
	}

	const uint32_t edgedExperience = targetExperience - amount;
	if (edgedExperience < GetProfileExperienceForLevel(profile, currentLevel, speciesId)) {
		errorOut = "Invalid edge amount";
		return false;
	}

	pokemon.words[(blockA + 0x8) / 2] = static_cast<uint16_t>(edgedExperience & 0xFFFFu);
	pokemon.words[(blockA + 0xA) / 2] = static_cast<uint16_t>((edgedExperience >> 16) & 0xFFFFu);
	WriteEncryptedBoxPokemonForProfile(profile, monOffset, pokemon);
	errorOut.clear();
	return true;
}

static bool SetEntirePartyStatusInternal(ExportGameProfile selectedProfile, uint32_t statusRaw, std::string &errorOut)
{
	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t partyBaseAddr = 0;
	uint8_t partyCount = 0;
	if (!ResolvePartyContext(selectedProfile, profile, pidAddr, partyBaseAddr, partyCount, errorOut)) {
		return false;
	}
	const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
	if (layout == NULL) {
		errorOut = "Pokemon memory layout unavailable";
		return false;
	}
	(void)pidAddr;

	for (uint8_t slot = 1; slot <= partyCount; ++slot) {
		const uint32_t monOffset = partyBaseAddr + (static_cast<uint32_t>(slot - 1) * static_cast<uint32_t>(layout->partyMonSize));
		DecryptedPokemon pokemon;
		PartyPokemonReadMode readMode;
		PartyFrameAnalysis failedAnalysis;
		if (!ReadPartyPokemonStrictForWriteForProfileWithAttempts(
				profile,
				monOffset,
				pokemon,
				readMode,
				kPartyWriteReadRetryCount,
				kPartyWriteReadRetryDelayMs,
				&failedAnalysis)) {
			if (failedAnalysis.classification == PARTY_FRAME_TRANSIENT_PLAINTEXT) {
				errorOut = "Waiting for stable party slot " + std::to_string(slot);
				return false;
			}

			DecryptedPokemon reportPokemonStorage;
			const DecryptedPokemon *failedPokemonForReport = NULL;
			const std::string failureContext = DescribePartyReadFailureContextForReport(
				profile,
				monOffset,
				"set_entire_party_status",
				&failedAnalysis,
				reportPokemonStorage,
				failedPokemonForReport);
			WriteFailedPartySlotReadBugReport(
				profile,
				partyBaseAddr,
				partyCount,
				static_cast<int>(slot),
				monOffset,
				failureContext.c_str(),
				failedPokemonForReport);
			errorOut = "Failed to read party slot " + std::to_string(slot);
			return false;
		}

		const uint32_t encodedStatusRaw = EncodePartyStatusForProfile(profile, statusRaw);
		const uint32_t originalStatusRaw = GetPartyStatusRawFromPokemon(pokemon);
		SetPartyStatusRawOnPokemon(pokemon, encodedStatusRaw);
		if (!WriteVerifiedPartyPokemonForProfile(
			profile,
			monOffset,
			static_cast<int>(slot),
			pokemon,
			readMode,
			originalStatusRaw,
			encodedStatusRaw,
			errorOut)) {
			return false;
		}
	}

	errorOut.clear();
	return true;
}

static bool EdgeEntirePartyInternal(ExportGameProfile selectedProfile, uint32_t amount, std::string &errorOut)
{
	const ExportProfileDefinition *profile = NULL;
	uint32_t pidAddr = 0;
	uint32_t partyBaseAddr = 0;
	uint8_t partyCount = 0;
	if (!ResolvePartyContext(selectedProfile, profile, pidAddr, partyBaseAddr, partyCount, errorOut)) {
		return false;
	}
	(void)profile;
	(void)pidAddr;

	bool updatedAny = false;
	for (uint8_t slot = 1; slot <= partyCount; ++slot) {
		std::string slotError;
		if (!EdgePartyPokemonInternal(selectedProfile, slot, amount, slotError)) {
			if (slotError == "Cannot edge a level 100 Pokemon") {
				continue;
			}
			errorOut = slotError;
			return false;
		}
		updatedAny = true;
	}

	if (!updatedAny && partyCount != 0) {
		errorOut = "No party Pokemon could be edged";
		return false;
	}

	errorOut.clear();
	return true;
}

class HttpExportBackend {
public:
	static HttpExportBackend &Instance()
	{
		static HttpExportBackend backend;
		return backend;
	}

	void SetEnabled(bool enabled)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		const bool wasActive = IsBackendActiveLocked();
		_enabled = enabled;
		const bool isActive = IsBackendActiveLocked();
		if (isActive && !wasActive) {
			StartThreadLocked(lock, true);
		} else if (!isActive && wasActive) {
			StopThreadLocked(lock, true);
		}

		if (enabled) {
			StartWatchdogLocked(lock);
		} else {
			StopWatchdogLocked(lock);
		}
	}

	bool IsEnabled()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _enabled;
	}

	void SetBattleLoggingEnabled(bool enabled)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		const bool wasActive = IsBackendActiveLocked();
		_battleLoggingEnabled = enabled;
		const bool isActive = IsBackendActiveLocked();
		if (isActive && !wasActive) {
			StartThreadLocked(lock, true);
		} else if (!isActive && wasActive) {
			StopThreadLocked(lock, true);
		}
		if (!enabled) {
			ResetBattleLogStateLocked();
		}
	}

	bool IsBattleLoggingEnabled()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _battleLoggingEnabled;
	}

	void SetClipboardSyncEnabled(bool enabled)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		const bool wasActive = IsBackendActiveLocked();
		_clipboardSyncEnabled = enabled;
		const bool isActive = IsBackendActiveLocked();
		if (isActive && !wasActive) {
			StartThreadLocked(lock, true);
		} else if (!isActive && wasActive) {
			StopThreadLocked(lock, true);
		}
	}

	bool IsClipboardSyncEnabled()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _clipboardSyncEnabled;
	}

	void SetSelectedProfile(ExportGameProfile profile)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_selectedProfile = profile;
		ApplyRtcOverridesLocked();
		_lastError.clear();
		_battleDetectionStatus = MakeEmptyBattleDetectionStatus();
		ResetBattleLogStateLocked();
		_hgssDiscoveryStatus = MakeEmptyHgssDiscoveryStatus();
		_lastKnownBattleSystemAddr = 0;
		_lastFullScanUnixMs = 0;
	}

	ExportGameProfile GetSelectedProfile()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _selectedProfile;
	}

	void SetPlatinumTimeOverride(PlatinumTimeOverride mode)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		switch (mode) {
			case PLATINUM_TIME_MORNING:
			case PLATINUM_TIME_DAY:
			case PLATINUM_TIME_EVENING:
			case PLATINUM_TIME_NIGHT:
			case PLATINUM_TIME_LATE_NIGHT:
			case PLATINUM_TIME_AUTO:
				_platinumTimeOverride = mode;
				break;
			default:
				_platinumTimeOverride = PLATINUM_TIME_AUTO;
				break;
		}
		ApplyRtcOverridesLocked();
	}

	PlatinumTimeOverride GetPlatinumTimeOverride()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _platinumTimeOverride;
	}

	void SetHealthCheckEnabled(bool enabled)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_healthCheckEnabled = enabled;
	}

	bool IsHealthCheckEnabled()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _healthCheckEnabled;
	}

	void SetHealthCheckIntervalMs(uint32_t intervalMs)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_healthCheckIntervalMs = ClampMaintenanceIntervalMs(intervalMs, kHttpHealthCheckIntervalMs);
	}

	uint32_t GetHealthCheckIntervalMs()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _healthCheckIntervalMs;
	}

	void SetIdleRefreshEnabled(bool enabled)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_idleRefreshEnabled = enabled;
	}

	bool IsIdleRefreshEnabled()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _idleRefreshEnabled;
	}

	void SetIdleRefreshIntervalMs(uint32_t intervalMs)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_idleRefreshIntervalMs = ClampMaintenanceIntervalMs(intervalMs, kHttpIdleRefreshIntervalMs);
	}

	uint32_t GetIdleRefreshIntervalMs()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _idleRefreshIntervalMs;
	}

	bool ForceRefresh()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (!_enabled) {
			return false;
		}
		RestartThreadLocked(lock);
		return true;
	}

	bool GetShowdownExportText(std::string &textOut, std::string &errorOut)
	{
		Snapshot snapshot = BuildSnapshot();
		if (!snapshot.success) {
			textOut.clear();
			errorOut = snapshot.error;
			return false;
		}

		textOut.clear();
		if (snapshot.trainerIdsResolved) {
			textOut += "TID: ";
			textOut += std::to_string(snapshot.trainerId);
			textOut += ":";
			textOut += std::to_string(snapshot.secretId);
			textOut += "\n";
		}
		textOut += snapshot.showdown;
		errorOut.clear();
		return true;
	}

	void SetBattleDetectPollPreset(BattleDetectPollPreset preset)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		switch (preset) {
			case BATTLE_DETECT_POLL_FAST:
			case BATTLE_DETECT_POLL_SLOW:
			case BATTLE_DETECT_POLL_NORMAL:
				_battleDetectPollPreset = preset;
				break;
			default:
				_battleDetectPollPreset = BATTLE_DETECT_POLL_NORMAL;
				break;
		}
	}

	BattleDetectPollPreset GetBattleDetectPollPreset()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _battleDetectPollPreset;
	}

	Status GetStatus()
	{
		Status status;

		std::lock_guard<std::mutex> lock(_mutex);
		const ExportProfileDefinition *profile = GetExportProfileDefinition(_selectedProfile);
		const PokemonMemoryLayout *pokemonLayout = GetProfilePokemonLayout(profile);
		status.enabled = _enabled;
		status.battleLoggingEnabled = _battleLoggingEnabled;
		status.running = _threadRunning;
		status.port = kHttpPort;
		status.url = BuildUrl();
		status.lastSuccessUnixMs = _lastSuccessUnixMs;
		status.healthCheckEnabled = _healthCheckEnabled;
		status.healthCheckIntervalMs = _healthCheckIntervalMs;
		status.idleRefreshEnabled = _idleRefreshEnabled;
		status.idleRefreshIntervalMs = _idleRefreshIntervalMs;
		status.lastError = _lastError;
		status.gameCode = GameCodeString();
		status.selectedProfile = (profile != NULL) ? profile->displayName : "";
		status.platinumTimeOverride = GetPlatinumTimeOverrideName(_platinumTimeOverride);
		status.platinumTimeOverrideActive = IsGen4LikeProfile(_selectedProfile) && (GetPlatinumTimeOverrideHour(_platinumTimeOverride) >= 0);
		char rtcTimeBuffer[32] = {0};
		rtcGetTimeAsString(rtcTimeBuffer);
		status.effectiveGameTime.assign(rtcTimeBuffer);
		status.romLoaded = (gameInfo.romsize != 0);
		status.supportedGame = (profile != NULL);
		status.ready = status.enabled && status.running && status.romLoaded && status.supportedGame && pokemonLayout != NULL && status.lastError.empty();
		status.battleDetectPollPreset = _battleDetectPollPreset;
		status.battleDetectPollMs = GetBattleDetectPollIntervalMs(_battleDetectPollPreset);
		std::string badgeResolveError;
		const bool badgeResolved = TryResolveCurrentBadgeStatus(_selectedProfile, status.playerBadges, badgeResolveError);
		(void)badgeResolved;
		(void)badgeResolveError;
		status.battleDetection = _battleDetectionStatus;
		status.battleLog = _battleLogStatus;
		status.hgssDiscovery = _hgssDiscoveryStatus;
		status.battleLog.outputDir = BuildAbsolutePath(GetOutputDumpsDir());
		status.battleLog.attemptMapPath = BuildAbsolutePath(BuildAttemptMapPath());
		if (!status.battleLog.lastPersistedMasterPath.empty()) {
			status.battleLog.lastPersistedMasterPath = BuildAbsolutePath(status.battleLog.lastPersistedMasterPath);
		}

		if (!status.enabled) {
			status.statusText = status.battleLoggingEnabled
				? "Pokemon HTTP API disabled (battle logging enabled)"
				: "Pokemon HTTP API disabled";
		} else if (!status.running && !status.lastError.empty()) {
			status.statusText = "Pokemon HTTP API error: " + status.lastError;
		} else if (!status.running) {
			status.statusText = "Pokemon HTTP API starting";
		} else if (!status.romLoaded) {
			status.statusText = "Pokemon HTTP API running (no ROM loaded)";
		} else if (!status.supportedGame) {
			status.statusText = "Pokemon HTTP API running (select export game)";
		} else if (pokemonLayout == NULL) {
			status.statusText = status.selectedProfile + " selected (memory layout not configured yet)";
		} else if (!status.lastError.empty()) {
			status.statusText = "Pokemon HTTP API error: " + status.lastError;
		} else {
			status.statusText = status.selectedProfile + " ready at " + status.url;
		}

		return status;
	}

	void Shutdown()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_enabled = false;
		_clipboardSyncEnabled = false;
		_battleLoggingEnabled = false;
		StopWatchdogLocked(lock);
		StopThreadLocked(lock, true);
	}

private:
	HttpExportBackend()
		: _enabled(false)
		, _clipboardSyncEnabled(false)
		, _battleLoggingEnabled(false)
		, _stopRequested(false)
		, _threadRunning(false)
		, _listenSocket(kInvalidSocket)
		, _clientSocket(kInvalidSocket)
		, _lastSuccessUnixMs(0)
		, _selectedProfile(EXPORT_GAME_POKEMON_PLATINUM)
		, _platinumTimeOverride(PLATINUM_TIME_AUTO)
		, _healthCheckEnabled(true)
		, _healthCheckIntervalMs(kHttpHealthCheckIntervalMs)
		, _idleRefreshEnabled(true)
		, _idleRefreshIntervalMs(kHttpIdleRefreshIntervalMs)
		, _battleDetectPollPreset(BATTLE_DETECT_POLL_NORMAL)
		, _battleDetectionStatus(MakeEmptyBattleDetectionStatus())
		, _battleLogStatus(MakeEmptyBattleLogStatus())
		, _hgssDiscoveryStatus(MakeEmptyHgssDiscoveryStatus())
		, _lastKnownBattleSystemAddr(0)
		, _lastFullScanUnixMs(0)
		, _battleLogLastCommand(0xFFFFFFFFu)
		, _watchdogStopRequested(false)
		, _watchdogThreadRunning(false)
		{
			ApplyRtcOverridesLocked();
		}

	~HttpExportBackend()
	{
		Shutdown();
	}

	void ResetRuntimeStateLocked()
	{
		_battleDetectionStatus = MakeEmptyBattleDetectionStatus();
		ResetBattleLogStateLocked();
		_hgssDiscoveryStatus = MakeEmptyHgssDiscoveryStatus();
		_lastKnownBattleSystemAddr = 0;
		_lastFullScanUnixMs = 0;
	}

	void StartThreadLocked(std::unique_lock<std::mutex> &lock, bool resetRuntimeState)
	{
		if (_threadRunning) {
			return;
		}
		if (_thread.joinable()) {
			std::thread oldThread = std::move(_thread);
			lock.unlock();
			oldThread.join();
			lock.lock();
		}

		_stopRequested = false;
		_lastError.clear();
		if (resetRuntimeState) {
			ResetRuntimeStateLocked();
		}
		_thread = std::thread(&HttpExportBackend::ThreadMain, this);
		_threadRunning = true;
	}

	void StopThreadLocked(std::unique_lock<std::mutex> &lock, bool resetRuntimeState)
	{
		_stopRequested = true;
		const SocketHandle clientSocket = _clientSocket;
		_listenSocket = kInvalidSocket;
		_clientSocket = kInvalidSocket;
		ShutdownSocket(clientSocket);

		if (_thread.joinable()) {
			std::thread thread = std::move(_thread);
			lock.unlock();
			thread.join();
			lock.lock();
		}

		_threadRunning = false;
		if (resetRuntimeState) {
			ResetRuntimeStateLocked();
		}
	}

	void RestartThreadLocked(std::unique_lock<std::mutex> &lock)
	{
		StopThreadLocked(lock, false);
		if (IsBackendActiveLocked()) {
			StartThreadLocked(lock, false);
		}
	}

	bool IsBackendActiveLocked() const
	{
		return _enabled || _clipboardSyncEnabled || _battleLoggingEnabled;
	}

	bool ShouldStop()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _stopRequested;
	}

	void StartWatchdogLocked(std::unique_lock<std::mutex> &lock)
	{
		if (_watchdogThreadRunning) {
			return;
		}
		if (_watchdogThread.joinable()) {
			std::thread oldThread = std::move(_watchdogThread);
			lock.unlock();
			oldThread.join();
			lock.lock();
		}

		_watchdogStopRequested = false;
		_watchdogThread = std::thread(&HttpExportBackend::WatchdogMain, this);
		_watchdogThreadRunning = true;
	}

	void StopWatchdogLocked(std::unique_lock<std::mutex> &lock)
	{
		_watchdogStopRequested = true;

		if (_watchdogThread.joinable()) {
			std::thread thread = std::move(_watchdogThread);
			lock.unlock();
			thread.join();
			lock.lock();
		}

		_watchdogThreadRunning = false;
	}

	bool ShouldStopWatchdog()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _watchdogStopRequested;
	}

	void SetError(const std::string &error)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_lastError = error;
	}

	void SetLastSuccess(uint64_t unixMs)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_lastSuccessUnixMs = unixMs;
		_lastError.clear();
	}

	void ApplyRtcOverridesLocked()
	{
		int forcedHour = -1;
		if (IsGen4LikeProfile(_selectedProfile)) {
			forcedHour = GetPlatinumTimeOverrideHour(_platinumTimeOverride);
		}
		rtcSetForcedHour(forcedHour);
	}

	void ResetBattleLogStateLocked()
	{
		_battleLogStatus = MakeEmptyBattleLogStatus();
		_battleLogSessions.clear();
		_battleLogLastBattleMons.clear();
		_battleLogLastEnemyPartyMons.clear();
		_battleLogRecordedEnemyKos.clear();
		_battleLogLastCommand = 0xFFFFFFFFu;
	}

	static uint32_t ClampMaintenanceIntervalMs(uint32_t intervalMs, uint32_t fallbackMs)
	{
		if (intervalMs == 0u) {
			return fallbackMs;
		}
		return std::max<uint32_t>(intervalMs, kHttpMinMaintenanceIntervalMs);
	}

	void PollBattleDetectionState()
	{
		const uint64_t now = CurrentUnixMs();
		ExportGameProfile selectedProfile = EXPORT_GAME_NONE;
		bool battleLoggingEnabled = false;
		BattleDetectPollPreset pollPreset = BATTLE_DETECT_POLL_NORMAL;
		BattleDetectionStatus previousStatus = MakeEmptyBattleDetectionStatus();
		BattleLogStatus previousBattleLogStatus = MakeEmptyBattleLogStatus();
		std::vector<BattleMonRuntimeSnapshot> previousBattleMons;
		std::vector<BattleLogEnemyPartySnapshot> previousEnemyPartyMons;
		std::vector<BattleLogRecordedEnemyKo> previousRecordedEnemyKos;
		uint32_t previousBattleLogLastCommand = 0xFFFFFFFFu;
		uint32_t lastKnownBattleSystemAddr = 0;
		uint64_t lastFullScanUnixMs = 0;
		BattleLogSession completedSessionToPersist;
		bool shouldPersistCompletedSession = false;
		bool attemptedBattleLogStorageInit = false;
		bool battleLogStorageInitSucceeded = false;
		std::string battleLogStorageInitError;

		{
			std::lock_guard<std::mutex> lock(_mutex);
			selectedProfile = _selectedProfile;
			battleLoggingEnabled = _battleLoggingEnabled;
			pollPreset = _battleDetectPollPreset;
			previousStatus = _battleDetectionStatus;
			previousBattleLogStatus = _battleLogStatus;
			previousBattleMons = _battleLogLastBattleMons;
			previousEnemyPartyMons = _battleLogLastEnemyPartyMons;
			previousRecordedEnemyKos = _battleLogRecordedEnemyKos;
			previousBattleLogLastCommand = _battleLogLastCommand;
			lastKnownBattleSystemAddr = _lastKnownBattleSystemAddr;
			lastFullScanUnixMs = _lastFullScanUnixMs;
		}

		BattleDetectionStatus nextStatus = previousStatus;
		nextStatus.lastPollUnixMs = now;
		nextStatus.battleActive = false;
		nextStatus.trainerBattleActive = false;
		nextStatus.trainerBattleCanStartSession = false;
		nextStatus.battleSystemAddr = 0;
		nextStatus.battleContextAddr = 0;
		nextStatus.battleType = 0;
		nextStatus.enemyTrainerId = 0;
		nextStatus.maxBattlers = 0;
		nextStatus.detectionSource.clear();
		nextStatus.detectionSourceDepth = -1;
		nextStatus.detectionConfidence = 0;

		BattleLogStatus nextBattleLogStatus = previousBattleLogStatus;
		HgssDiscoveryStatus nextHgssDiscoveryStatus = MakeEmptyHgssDiscoveryStatus();
		std::vector<BattleMonRuntimeSnapshot> nextBattleMons;
		std::vector<BattleLogEnemyPartySnapshot> nextEnemyPartyMons;
		std::vector<BattleLogRecordedEnemyKo> nextRecordedEnemyKos = previousRecordedEnemyKos;
		uint32_t nextBattleLogLastCommand = previousBattleLogLastCommand;
		std::vector<BattleLogPartyEntry> sessionStartParty;
		uint8_t sessionStartHighestLevel = 0;
		uint16_t sessionStartPlayerTrainerId = 0u;
		uint16_t sessionStartPlayerSecretId = 0u;
		const std::string gameCode = GameCodeString();
		const ExportProfileDefinition *profile = GetExportProfileDefinition(selectedProfile);
		const PokemonMemoryLayout *pokemonLayout = GetProfilePokemonLayout(profile);
		const BattleMemoryLayout *battleLayout = GetProfileBattleLayout(profile);
		if (IsHgssLikeProfile(selectedProfile)) {
			nextHgssDiscoveryStatus.active = true;
		}
		const bool battleTrackingAllowed = IsBattleTrackingAllowedForGameCode(selectedProfile, gameCode);
		const uint32_t activeBattlePollMs = ((battleLayout != NULL) && (battleLayout->activeBattlePollMs != 0u))
			? battleLayout->activeBattlePollMs
			: kBattleDetectActivePollMs;
		const uint32_t pollIntervalMs = previousStatus.battleActive ? activeBattlePollMs : GetBattleDetectPollIntervalMs(pollPreset);
		if ((previousStatus.lastPollUnixMs != 0) && ((now - previousStatus.lastPollUnixMs) < pollIntervalMs)) {
			return;
		}

		if ((gameInfo.romsize == 0) || (profile == NULL) || (pokemonLayout == NULL) || (battleLayout == NULL) || !battleLayout->supported || !battleTrackingAllowed) {
			const bool battleWasActive = previousStatus.battleActive;
			nextStatus.trainerBattleSessionActive = false;
			if (battleWasActive) {
				nextStatus.lastBattleEndUnixMs = now;
				nextStatus.lastStateChangeUnixMs = now;
				nextStatus.lastLifecycleEvent = "battle_end";
			} else if (!battleTrackingAllowed && (gameInfo.romsize != 0) && (nextStatus.lastLifecycleEvent != "game_code_not_whitelisted")) {
				nextStatus.lastStateChangeUnixMs = now;
				nextStatus.lastLifecycleEvent = "game_code_not_whitelisted";
			}

			std::lock_guard<std::mutex> lock(_mutex);
			_battleDetectionStatus = nextStatus;
			_hgssDiscoveryStatus = nextHgssDiscoveryStatus;
			ResetBattleLogStateLocked();
			_lastKnownBattleSystemAddr = 0;
			return;
		}

		BattleDetectionSnapshot snapshot;
		snapshot.valid = false;
		bool foundSnapshot = false;
		if (IsHgssLikeProfile(selectedProfile)) {
			uint16_t probeTrainerId = 0u;
			uint16_t probeSecretId = 0u;
			TryResolveHgssTrainerIds(profile, nextHgssDiscoveryStatus, probeTrainerId, probeSecretId);
			const bool allowFullScan = true;
			foundSnapshot = TryBuildHgssBattleDetectionSnapshot(
				*pokemonLayout,
				*battleLayout,
				lastKnownBattleSystemAddr,
				previousStatus.battleActive,
				allowFullScan,
				snapshot,
				nextHgssDiscoveryStatus);
			if (allowFullScan) {
				lastFullScanUnixMs = now;
			}
		} else if (IsGen5BwLikePokemonProfile(selectedProfile)) {
			PokemonRuntimeAddresses runtimeAddresses;
			std::string runtimeError;
			if (ResolvePokemonRuntimeAddresses(profile, runtimeAddresses, runtimeError)) {
				foundSnapshot = TryBuildBwBattleDetectionSnapshot(runtimeAddresses, lastKnownBattleSystemAddr, snapshot);
			}
		} else {
			if (IsMainRamAlignedPointer(battleLayout->battleSystemAnchorPointerAddr)) {
				const uint32_t battleSysAddr = ReadArm9U32(battleLayout->battleSystemAnchorPointerAddr);
				foundSnapshot = TryBuildBattleDetectionSnapshot(*battleLayout, battleSysAddr, "anchor_pointer", 0, false, false, snapshot);
			}

			if (!foundSnapshot) {
				foundSnapshot = ScanBattleSystemRange(
					*battleLayout,
					battleLayout->preferredBattleClusterStart,
					battleLayout->preferredBattleClusterEnd,
					"preferred_cluster",
					1,
					false,
					true,
					battleLayout->preferredBattleCtxDelta,
					snapshot);
			}

			if (!foundSnapshot && IsMainRamAlignedPointer(lastKnownBattleSystemAddr)) {
				const uint32_t startAddr = (lastKnownBattleSystemAddr > battleLayout->nearbyLastKnownRadius)
					? (lastKnownBattleSystemAddr - battleLayout->nearbyLastKnownRadius)
					: 0x02000000u;
				foundSnapshot = ScanBattleSystemRange(
					*battleLayout,
					startAddr,
					lastKnownBattleSystemAddr + battleLayout->nearbyLastKnownRadius + 4u,
					"nearby_last_known",
					2,
					false,
					false,
					0u,
					snapshot);
			}

			if (!foundSnapshot && ((lastFullScanUnixMs == 0) || ((now - lastFullScanUnixMs) >= battleLayout->fullScanRetryMs))) {
				foundSnapshot = ScanBattleSystemRange(
					*battleLayout,
					battleLayout->fullBattleScanStart,
					battleLayout->fullBattleScanEnd,
					"full_ram",
					3,
					false,
					false,
					0u,
					snapshot);
				lastFullScanUnixMs = now;
			}
		}

		if (foundSnapshot) {
			nextStatus.battleActive = snapshot.battleActive;
			nextStatus.trainerBattleActive = snapshot.trainerBattleActive;
			nextStatus.trainerBattleCanStartSession = snapshot.trainerBattleCanStartSession;
			nextStatus.battleSystemAddr = snapshot.battleSystemAddr;
			nextStatus.battleContextAddr = snapshot.battleContextAddr;
			nextStatus.battleType = snapshot.battleType;
			nextStatus.enemyTrainerId = snapshot.enemyTrainerId;
			nextStatus.maxBattlers = snapshot.maxBattlers;
			nextStatus.detectionSource = snapshot.detectionSource;
			nextStatus.detectionSourceDepth = snapshot.detectionSourceDepth;
			nextStatus.detectionConfidence = snapshot.detectionConfidence;
			lastKnownBattleSystemAddr = snapshot.battleSystemAddr;
		}

		const bool battleStarted = !previousStatus.battleActive && nextStatus.battleActive;
		const bool battleEnded = previousStatus.battleActive && !nextStatus.battleActive;
		const bool shouldStartTrainerSession = !previousStatus.trainerBattleSessionActive && nextStatus.trainerBattleCanStartSession;
		bool sessionStartBadgeInfoResolved = false;
		uint8_t sessionStartBadgeCount = 0u;
		uint16_t sessionStartBadgeFlags = 0u;

		if (shouldStartTrainerSession) {
			nextStatus.trainerBattleSessionActive = true;
			nextStatus.lastSessionStartUnixMs = now;
			nextStatus.lastStateChangeUnixMs = now;
			nextStatus.lastLifecycleEvent = "trainer_session_start";
		} else if (battleEnded) {
			nextStatus.trainerBattleSessionActive = false;
			nextStatus.lastBattleEndUnixMs = now;
			nextStatus.lastStateChangeUnixMs = now;
			nextStatus.lastLifecycleEvent = "battle_end";
		} else if (battleStarted) {
			nextStatus.lastStateChangeUnixMs = now;
			nextStatus.lastLifecycleEvent = nextStatus.trainerBattleActive ? "trainer_battle_start" : "battle_start";
		} else if (previousStatus.trainerBattleSessionActive && nextStatus.battleActive) {
			nextStatus.trainerBattleSessionActive = true;
		} else if (!nextStatus.battleActive) {
			nextStatus.trainerBattleSessionActive = false;
		}
		if (!battleLoggingEnabled) {
			nextStatus.trainerBattleSessionActive = false;
		}

		const bool shouldTrackBattleLog = battleLoggingEnabled && nextStatus.trainerBattleSessionActive && IsMainRamAlignedPointer(nextStatus.battleContextAddr);
		if (battleLoggingEnabled && shouldStartTrainerSession) {
			if (profile != NULL) {
				BuildBattleLogPartySnapshot(profile, sessionStartParty, sessionStartHighestLevel);
				std::string trainerResolveError;
				TryResolveCurrentTrainerIds(selectedProfile, sessionStartPlayerTrainerId, sessionStartPlayerSecretId, trainerResolveError);
			}
			PlayerBadgeStatus sessionStartBadges;
			std::string badgeResolveError;
			if (TryResolveCurrentBadgeStatus(selectedProfile, sessionStartBadges, badgeResolveError) && sessionStartBadges.available) {
				sessionStartBadgeInfoResolved = true;
				sessionStartBadgeCount = sessionStartBadges.badgeCount;
				sessionStartBadgeFlags = sessionStartBadges.badgeFlags;
			}

			attemptedBattleLogStorageInit = true;
			battleLogStorageInitSucceeded = EnsureBattleLogStorageInitialized(battleLogStorageInitError);
		}

		const bool shouldTrackBwBattleLog = battleLoggingEnabled && IsGen5BwLikePokemonProfile(selectedProfile) && nextStatus.trainerBattleSessionActive && IsMainRamAlignedPointer(nextStatus.battleSystemAddr);
		if (shouldTrackBattleLog || shouldTrackBwBattleLog) {
			BuildBattleMonRuntimeSnapshotForProfile(profile, *battleLayout, nextStatus.battleSystemAddr, nextStatus.battleContextAddr, nextBattleMons);
			BuildEnemyPartyRuntimeSnapshotForProfile(
				profile,
				*battleLayout,
				nextStatus.battleSystemAddr,
				nextBattleMons,
				previousEnemyPartyMons,
				nextEnemyPartyMons);
			nextBattleLogLastCommand = shouldTrackBwBattleLog
				? 0xFFFFFFFFu
				: ReadArm9U32(nextStatus.battleContextAddr + battleLayout->battleContextCommandOffset);
		} else {
			nextBattleMons.clear();
			nextEnemyPartyMons.clear();
			nextRecordedEnemyKos.clear();
			nextBattleLogLastCommand = 0xFFFFFFFFu;
		}

		{
			std::lock_guard<std::mutex> lock(_mutex);
			_battleDetectionStatus = nextStatus;
			_hgssDiscoveryStatus = nextHgssDiscoveryStatus;
			_lastKnownBattleSystemAddr = lastKnownBattleSystemAddr;
			_lastFullScanUnixMs = lastFullScanUnixMs;

				if (battleLoggingEnabled && shouldStartTrainerSession) {
					nextRecordedEnemyKos.clear();
					BattleLogSession session;
					session.active = true;
					session.playerTrainerId = sessionStartPlayerTrainerId;
					session.playerSecretId = sessionStartPlayerSecretId;
					session.enemyTrainerIdA = nextStatus.enemyTrainerId;
					session.enemyTrainerIdB = 0u;
					session.pPartyHighestLevel = sessionStartHighestLevel;
					session.badgeInfoResolved = sessionStartBadgeInfoResolved;
					session.badgeCount = sessionStartBadgeCount;
					session.badgeFlags = sessionStartBadgeFlags;
					session.startedAtUnixMs = now;
					session.endedAtUnixMs = 0;
					session.pParty = sessionStartParty;
					session.events.clear();

					BattleLogEvent event;
					event.type = "session_start";
					event.turn = 0u;
					event.pSlot = 0u;
					event.aiSpecies = 0u;
					event.unixMs = now;
					session.events.push_back(event);
					_battleLogSessions.push_back(session);
					if (_battleLogSessions.size() > kMaxBattleLogSessions) {
						_battleLogSessions.erase(_battleLogSessions.begin(), _battleLogSessions.begin() + (_battleLogSessions.size() - kMaxBattleLogSessions));
					}

					nextBattleLogStatus.active = true;
					nextBattleLogStatus.activeTrainerId = nextStatus.enemyTrainerId;
					nextBattleLogStatus.currentTurn = 0u;
					nextBattleLogStatus.lastSessionStartUnixMs = now;
					nextBattleLogStatus.lastEventType = "session_start";
					nextBattleLogStatus.lastEventTurn = 0u;
					nextBattleLogStatus.lastEventPlayerSlot = 0u;
					nextBattleLogStatus.lastEventAiSpecies = 0u;
					if (attemptedBattleLogStorageInit) {
						nextBattleLogStatus.outputDir = BuildAbsolutePath(GetOutputDumpsDir());
						nextBattleLogStatus.attemptMapPath = BuildAbsolutePath(BuildAttemptMapPath());
						nextBattleLogStatus.lastPersistSucceeded = battleLogStorageInitSucceeded;
						nextBattleLogStatus.lastPersistError = battleLogStorageInitError;
					}
				}

			if (battleLoggingEnabled &&
				!IsGen5BwLikePokemonProfile(selectedProfile) &&
				nextStatus.trainerBattleSessionActive &&
				(nextBattleLogLastCommand == battleLayout->battleControlCalcTurnOrder) &&
				(previousBattleLogLastCommand != battleLayout->battleControlCalcTurnOrder)) {
				if (nextBattleLogStatus.currentTurn < 0xFFFFu) {
					++nextBattleLogStatus.currentTurn;
				}
			}

			if (battleLoggingEnabled && nextStatus.trainerBattleSessionActive && !_battleLogSessions.empty()) {
				BattleLogSession &activeSession = _battleLogSessions.back();
				if (activeSession.active && (previousBattleLogStatus.active || shouldStartTrainerSession) &&
					(previousBattleMons.size() == nextBattleMons.size()) && !nextBattleMons.empty()) {
					const bool trackEnemyKosFromPartySnapshot = IsGen5BwLikePokemonProfile(selectedProfile);
					const uint16_t effectiveTurn = (nextBattleLogStatus.currentTurn != 0u) ? nextBattleLogStatus.currentTurn : 1u;
					for (size_t i = 0; i < nextBattleMons.size(); ++i) {
						const BattleMonRuntimeSnapshot &previousMon = previousBattleMons[i];
						const BattleMonRuntimeSnapshot &currentMon = nextBattleMons[i];
						if (!previousMon.valid || !currentMon.valid) {
							continue;
						}
						if ((previousMon.curHp <= 0u) || (currentMon.curHp > 0u)) {
							continue;
						}

						BattleLogEvent event;
						event.turn = effectiveTurn;
						event.unixMs = now;
						if ((currentMon.battler & 0x1u) == 0u) {
							if (!HasBattleLogPartyPid(activeSession, currentMon.pid)) {
								continue;
							}
							event.type = "aiKo";
							event.pSlot = static_cast<uint8_t>(currentMon.partySlot);
							event.aiSpecies = 0u;
						} else {
							if (trackEnemyKosFromPartySnapshot) {
								continue;
							}
							event.type = "pKo";
							event.pSlot = (i > 0u) ? nextBattleMons[i - 1u].partySlot : 0u;
							event.aiSpecies = currentMon.speciesId;
							RecordEnemyKoForBattler(*battleLayout, nextStatus.battleSystemAddr, currentMon, nextRecordedEnemyKos);
						}
						activeSession.events.push_back(event);
						nextBattleLogStatus.lastEventType = event.type;
						nextBattleLogStatus.lastEventTurn = event.turn;
						nextBattleLogStatus.lastEventPlayerSlot = event.pSlot;
						nextBattleLogStatus.lastEventAiSpecies = event.aiSpecies;
					}
				}
			}

			if (battleLoggingEnabled && nextStatus.trainerBattleSessionActive && !_battleLogSessions.empty()) {
				BattleLogSession &activeSession = _battleLogSessions.back();
				if (activeSession.active && !nextEnemyPartyMons.empty()) {
					const uint16_t effectiveTurn = (nextBattleLogStatus.currentTurn != 0u) ? nextBattleLogStatus.currentTurn : 1u;
					for (size_t i = 0; i < nextEnemyPartyMons.size(); ++i) {
						const BattleLogEnemyPartySnapshot &currentEnemyMon = nextEnemyPartyMons[i];
						if (!currentEnemyMon.valid) {
							continue;
						}

						const int previousIndex = FindEnemyPartyRuntimeSnapshotIndex(
							previousEnemyPartyMons,
							currentEnemyMon.partyPtr,
							currentEnemyMon.slot);
						if (previousIndex < 0) {
							continue;
						}

						const BattleLogEnemyPartySnapshot &previousEnemyMon = previousEnemyPartyMons[static_cast<size_t>(previousIndex)];
						if (!previousEnemyMon.valid ||
							(previousEnemyMon.curHp == 0u) ||
							(currentEnemyMon.curHp != 0u) ||
							HasRecordedEnemyKo(nextRecordedEnemyKos, currentEnemyMon.partyPtr, currentEnemyMon.slot, currentEnemyMon.speciesId)) {
							continue;
						}

						BattleLogEvent event;
						event.type = "pKo";
						event.turn = effectiveTurn;
						event.pSlot = (currentEnemyMon.inferredPlayerSlot != 0xFFu) ? currentEnemyMon.inferredPlayerSlot : 0u;
						event.aiSpecies = currentEnemyMon.speciesId;
						event.unixMs = now;
						activeSession.events.push_back(event);
						nextBattleLogStatus.lastEventType = event.type;
						nextBattleLogStatus.lastEventTurn = event.turn;
						nextBattleLogStatus.lastEventPlayerSlot = event.pSlot;
						nextBattleLogStatus.lastEventAiSpecies = event.aiSpecies;
						RecordEnemyKo(nextRecordedEnemyKos, currentEnemyMon.partyPtr, currentEnemyMon.slot, currentEnemyMon.speciesId);
					}
				}
			}

			if (battleLoggingEnabled && battleEnded && !_battleLogSessions.empty()) {
				BattleLogSession &activeSession = _battleLogSessions.back();
				if (activeSession.active) {
					BattleLogEvent event;
					event.type = "session_end";
					event.turn = nextBattleLogStatus.currentTurn;
					event.pSlot = 0u;
					event.aiSpecies = 0u;
					event.unixMs = now;
					activeSession.events.push_back(event);
					activeSession.active = false;
					activeSession.endedAtUnixMs = now;
					nextBattleLogStatus.lastEventType = "session_end";
					nextBattleLogStatus.lastEventTurn = event.turn;
					nextBattleLogStatus.lastEventPlayerSlot = 0u;
					nextBattleLogStatus.lastEventAiSpecies = 0u;
					nextBattleLogStatus.lastSessionEndUnixMs = now;
					completedSessionToPersist = activeSession;
					shouldPersistCompletedSession = true;
				}
				nextBattleLogStatus.active = false;
				nextBattleLogStatus.activeTrainerId = 0u;
				nextBattleLogStatus.currentTurn = 0u;
				nextBattleMons.clear();
				nextEnemyPartyMons.clear();
				nextRecordedEnemyKos.clear();
				nextBattleLogLastCommand = 0xFFFFFFFFu;
			}

			if (!battleLoggingEnabled || (!nextStatus.trainerBattleSessionActive && !battleEnded)) {
				nextBattleMons.clear();
				nextEnemyPartyMons.clear();
				nextRecordedEnemyKos.clear();
				nextBattleLogLastCommand = 0xFFFFFFFFu;
				nextBattleLogStatus.active = false;
				nextBattleLogStatus.activeTrainerId = 0u;
				nextBattleLogStatus.currentTurn = 0u;
				if (!battleLoggingEnabled) {
					_battleLogSessions.clear();
					nextBattleLogStatus.sessionCount = 0u;
					nextBattleLogStatus.activeSessionEventCount = 0u;
				}
			}

			nextBattleLogStatus.sessionCount = static_cast<uint32_t>(_battleLogSessions.size());
			nextBattleLogStatus.activeSessionEventCount =
				(!_battleLogSessions.empty() && _battleLogSessions.back().active)
					? static_cast<uint32_t>(_battleLogSessions.back().events.size())
					: 0u;
			_battleLogStatus = nextBattleLogStatus;
			_battleLogLastBattleMons = nextBattleMons;
			_battleLogLastEnemyPartyMons = nextEnemyPartyMons;
			_battleLogRecordedEnemyKos = nextRecordedEnemyKos;
			_battleLogLastCommand = nextBattleLogLastCommand;
		}

		if (shouldPersistCompletedSession) {
			std::string persistedMasterPath;
			std::string persistError;
			const bool persistSucceeded = PersistCompletedBattleLogSession(completedSessionToPersist, persistedMasterPath, persistError);
			std::lock_guard<std::mutex> lock(_mutex);
			_battleLogStatus.outputDir = BuildAbsolutePath(GetOutputDumpsDir());
			_battleLogStatus.attemptMapPath = BuildAbsolutePath(BuildAttemptMapPath());
			_battleLogStatus.lastPersistedMasterPath = persistedMasterPath;
			_battleLogStatus.lastPersistSucceeded = persistSucceeded;
			_battleLogStatus.lastPersistError = persistError;
		}
	}

	bool ProbeHealth(std::string &errorOut)
	{
		SocketHandle sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == kInvalidSocket) {
			errorOut = "failed to create probe socket";
			return false;
		}

		SetSocketTimeouts(sock, kHttpClientTimeoutMs);

		sockaddr_in address;
		std::memset(&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_port = htons(kHttpPort);
		address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

		if (connect(sock, reinterpret_cast<sockaddr *>(&address), sizeof(address)) != 0) {
			errorOut = "failed to connect to localhost health endpoint";
			CloseSocket(sock);
			return false;
		}

		static const char kRequest[] = "GET /health HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n";
		static const int kRequestLength = static_cast<int>(sizeof(kRequest) - 1u);
		size_t sent = 0u;
		while (sent < static_cast<size_t>(kRequestLength)) {
			const int remaining = kRequestLength - static_cast<int>(sent);
			const int chunk = send(sock, kRequest + sent, remaining, 0);
			if (chunk <= 0) {
				errorOut = "failed to send health probe";
				ShutdownSocket(sock);
				CloseSocket(sock);
				return false;
			}
			sent += static_cast<size_t>(chunk);
		}

		char response[256] = {0};
		const int received = recv(sock, response, sizeof(response) - 1, 0);
		ShutdownSocket(sock);
		CloseSocket(sock);
		if (received <= 0) {
			errorOut = "health endpoint did not respond";
			return false;
		}

		const std::string responseText(response, response + received);
		if (responseText.find("HTTP/1.1 200") != 0u) {
			errorOut = "health endpoint returned a non-200 response";
			return false;
		}

		errorOut.clear();
		return true;
	}

	void WatchdogMain()
	{
		uint64_t lastHealthCheckUnixMs = CurrentUnixMs();
		uint64_t lastIdleRefreshUnixMs = lastHealthCheckUnixMs;
		while (!ShouldStopWatchdog()) {
			const uint64_t now = CurrentUnixMs();
			bool shouldProbe = false;
			bool shouldForceIdleRefresh = false;
			uint32_t healthCheckIntervalMs = kHttpHealthCheckIntervalMs;
			uint32_t idleRefreshIntervalMs = kHttpIdleRefreshIntervalMs;
			{
				std::lock_guard<std::mutex> lock(_mutex);
				shouldProbe = _enabled && _healthCheckEnabled;
				healthCheckIntervalMs = _healthCheckIntervalMs;
				idleRefreshIntervalMs = _idleRefreshIntervalMs;
				shouldForceIdleRefresh =
					_enabled &&
					_idleRefreshEnabled &&
					((now - lastIdleRefreshUnixMs) >= idleRefreshIntervalMs) &&
					(_clientSocket == kInvalidSocket) &&
					!_battleDetectionStatus.battleActive &&
					!_battleDetectionStatus.trainerBattleSessionActive;
			}

			if (shouldProbe && ((lastHealthCheckUnixMs == 0u) || ((now - lastHealthCheckUnixMs) >= healthCheckIntervalMs))) {
				lastHealthCheckUnixMs = now;
				std::string probeError;
				const bool healthy = ProbeHealth(probeError);
				if (!healthy) {
					std::unique_lock<std::mutex> lock(_mutex);
					if (_enabled && !_watchdogStopRequested) {
						RestartThreadLocked(lock);
						lastIdleRefreshUnixMs = CurrentUnixMs();
					}
				}
			}

			if (shouldForceIdleRefresh) {
				std::unique_lock<std::mutex> lock(_mutex);
				if (_enabled &&
					!_watchdogStopRequested &&
					(_clientSocket == kInvalidSocket) &&
					!_battleDetectionStatus.battleActive &&
					!_battleDetectionStatus.trainerBattleSessionActive) {
					RestartThreadLocked(lock);
					lastIdleRefreshUnixMs = CurrentUnixMs();
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(kHttpWatchdogPollMs));
		}

		std::lock_guard<std::mutex> lock(_mutex);
		_watchdogThreadRunning = false;
	}

	void ThreadMain()
	{
#ifdef _WIN32
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			SetError("WSAStartup failed");
			std::lock_guard<std::mutex> lock(_mutex);
			_threadRunning = false;
			return;
		}
#endif

		SocketHandle listenSocket = kInvalidSocket;

		while (!ShouldStop()) {
			PollBattleDetectionState();

			bool httpEnabled = false;
			{
				std::lock_guard<std::mutex> lock(_mutex);
				httpEnabled = _enabled;
			}

			if (!httpEnabled) {
				if (listenSocket != kInvalidSocket) {
					CloseSocket(listenSocket);
					listenSocket = kInvalidSocket;
					std::lock_guard<std::mutex> lock(_mutex);
					_listenSocket = kInvalidSocket;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			if (listenSocket == kInvalidSocket) {
				listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (listenSocket == kInvalidSocket) {
					SetError("Failed to create socket");
					std::this_thread::sleep_for(std::chrono::milliseconds(250));
					continue;
				}

				const int reuseValue = 1;
				setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&reuseValue), sizeof(reuseValue));

				sockaddr_in address;
				std::memset(&address, 0, sizeof(address));
				address.sin_family = AF_INET;
				address.sin_port = htons(kHttpPort);
				address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

				if (bind(listenSocket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) != 0) {
					SetError("Failed to bind 127.0.0.1:31124");
					CloseSocket(listenSocket);
					listenSocket = kInvalidSocket;
					std::this_thread::sleep_for(std::chrono::milliseconds(250));
					continue;
				}

				if (listen(listenSocket, 8) != 0) {
					SetError("Failed to listen on 127.0.0.1:31124");
					CloseSocket(listenSocket);
					listenSocket = kInvalidSocket;
					std::this_thread::sleep_for(std::chrono::milliseconds(250));
					continue;
				}

				{
					std::lock_guard<std::mutex> lock(_mutex);
					_listenSocket = listenSocket;
					_lastError.clear();
				}
			}

			fd_set readSet;
			FD_ZERO(&readSet);
			FD_SET(listenSocket, &readSet);

			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 50000;

			const int selectResult = select(static_cast<int>(listenSocket + 1), &readSet, NULL, NULL, &timeout);
			if (selectResult <= 0) {
				continue;
			}

			sockaddr_in clientAddress;
#ifdef _WIN32
			int clientAddressLength = sizeof(clientAddress);
#else
			socklen_t clientAddressLength = sizeof(clientAddress);
#endif
			SocketHandle clientSocket = accept(listenSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressLength);
			if (clientSocket == kInvalidSocket) {
				continue;
			}

			SetSocketTimeouts(clientSocket, kHttpClientTimeoutMs);
			{
				std::lock_guard<std::mutex> lock(_mutex);
				_clientSocket = clientSocket;
			}
			HandleClient(clientSocket);
			{
				std::lock_guard<std::mutex> lock(_mutex);
				if (_clientSocket == clientSocket) {
					_clientSocket = kInvalidSocket;
				}
			}
			CloseSocket(clientSocket);
		}

		if (listenSocket != kInvalidSocket) {
			CloseSocket(listenSocket);
		}
		CleanupThreadSocketState();
	}

	void CleanupThreadSocketState()
	{
#ifdef _WIN32
		WSACleanup();
#endif
		std::lock_guard<std::mutex> lock(_mutex);
		_listenSocket = kInvalidSocket;
		_clientSocket = kInvalidSocket;
		_threadRunning = false;
	}

	static std::string ExtractPathFromRequest(const std::string &request, std::string &methodOut)
	{
		const size_t lineEnd = request.find("\r\n");
		const std::string line = request.substr(0, lineEnd);
		const size_t methodEnd = line.find(' ');
		if (methodEnd == std::string::npos) {
			return "";
		}
		const size_t pathEnd = line.find(' ', methodEnd + 1);
		methodOut = line.substr(0, methodEnd);
		if (pathEnd == std::string::npos) {
			return line.substr(methodEnd + 1);
		}
		return line.substr(methodEnd + 1, pathEnd - methodEnd - 1);
	}

	static std::string NormalizePath(const std::string &path)
	{
		const size_t queryPos = path.find('?');
		return (queryPos == std::string::npos) ? path : path.substr(0, queryPos);
	}

static std::string BuildHttpResponse(int statusCode, const std::string &body, const char *contentType)
	{
		const char *statusText = "OK";
		switch (statusCode) {
			case 404: statusText = "Not Found"; break;
			case 405: statusText = "Method Not Allowed"; break;
			case 500: statusText = "Internal Server Error"; break;
			case 503: statusText = "Service Unavailable"; break;
		}

		std::string response;
		response += "HTTP/1.1 ";
		response += std::to_string(statusCode);
		response += " ";
		response += statusText;
		response += "\r\nConnection: close\r\nAccess-Control-Allow-Origin: *\r\n";
		response += "Access-Control-Allow-Methods: GET, OPTIONS\r\n";
		response += "Access-Control-Allow-Headers: Content-Type\r\n";
		response += "Content-Type: ";
		response += contentType;
		response += "\r\nContent-Length: ";
		response += std::to_string(body.size());
		response += "\r\n\r\n";
		response += body;
		return response;
	}

	static std::string BuildArm9MainRamDump()
	{
		std::string dump;
		dump.resize(static_cast<size_t>(kArm9MainRamEnd - kArm9MainRamStart));
		for (uint32_t addr = kArm9MainRamStart; addr < kArm9MainRamEnd; ++addr) {
			dump[static_cast<size_t>(addr - kArm9MainRamStart)] = static_cast<char>(ReadArm9U8(addr));
		}
		return dump;
	}

	static void SendAll(SocketHandle clientSocket, const std::string &payload)
	{
		size_t sent = 0;
		while (sent < payload.size()) {
			const int chunk = send(clientSocket, payload.data() + sent, static_cast<int>(payload.size() - sent), 0);
			if (chunk <= 0) {
				return;
			}
			sent += static_cast<size_t>(chunk);
		}
	}

	void HandleClient(SocketHandle clientSocket)
	{
		char buffer[2048] = {0};
		const int received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (received <= 0) {
			return;
		}

		std::string method;
		std::string path = NormalizePath(ExtractPathFromRequest(std::string(buffer, buffer + received), method));
		if (method == "OPTIONS") {
			SendAll(clientSocket, BuildHttpResponse(200, "", "text/plain; charset=utf-8"));
			return;
		}

		if (method != "GET") {
			SendAll(clientSocket, BuildHttpResponse(405, "Method not allowed\n", "text/plain; charset=utf-8"));
			return;
		}

		if (path == "/health") {
			SendAll(clientSocket, BuildHttpResponse(200, BuildHealthJson(), "application/json; charset=utf-8"));
			return;
		}
		if (path == "/snapshot") {
			ActiveBattleSnapshot snapshot = BuildActiveBattleSnapshot();
			if (!snapshot.success) {
				SendAll(clientSocket, BuildHttpResponse(503, BuildJsonError(snapshot.error), "application/json; charset=utf-8"));
				return;
			}
			SendAll(clientSocket, BuildHttpResponse(200, snapshot.json, "application/json; charset=utf-8"));
			return;
		}
		if (path == "/ram_dump") {
			SendAll(clientSocket, BuildHttpResponse(200, BuildArm9MainRamDump(), "application/octet-stream"));
			return;
		}
			if (path == "/battle_log") {
				BattleLogSnapshot battleLogSnapshot = BuildBattleLogSnapshot();
				if (!battleLogSnapshot.success) {
					SendAll(clientSocket, BuildHttpResponse(503, BuildJsonError(battleLogSnapshot.error), "application/json; charset=utf-8"));
					return;
				}
				SendAll(clientSocket, BuildHttpResponse(200, battleLogSnapshot.payload, "application/octet-stream"));
				return;
			}
		if (path == "/box/packed") {
			PackedSnapshot packedSnapshot = BuildPackedSnapshot();
			if (!packedSnapshot.success) {
				SendAll(clientSocket, BuildHttpResponse(503, "Pokemon HTTP export unavailable: " + packedSnapshot.error + "\n", "text/plain; charset=utf-8"));
				return;
			}
			SendAll(clientSocket, BuildHttpResponse(200, packedSnapshot.payload, "application/octet-stream"));
			return;
		}
		if (path != "/box" && path != "/box/raw") {
			SendAll(clientSocket, BuildHttpResponse(404, "Not found\n", "text/plain; charset=utf-8"));
			return;
		}

		Snapshot snapshot = BuildSnapshot();
		if (!snapshot.success) {
			const std::string errorBody = (path == "/box/raw")
				? BuildJsonError(snapshot.error)
				: ("Pokemon HTTP export unavailable: " + snapshot.error + "\n");
			const char *contentType = (path == "/box/raw") ? "application/json; charset=utf-8" : "text/plain; charset=utf-8";
			SendAll(clientSocket, BuildHttpResponse(503, errorBody, contentType));
			return;
		}

		if (path == "/box") {
			SendAll(clientSocket, BuildHttpResponse(200, snapshot.showdown, "text/plain; charset=utf-8"));
			return;
		}

		if (path == "/box/raw") {
			SendAll(clientSocket, BuildHttpResponse(200, snapshot.rawJson, "application/json; charset=utf-8"));
			return;
		}

		SendAll(clientSocket, BuildHttpResponse(404, "Not found\n", "text/plain; charset=utf-8"));
	}

	std::string BuildHealthJson()
	{
		Status status = GetStatus();
		std::string json = "{";
		json += "\"enabled\":";
		json += status.enabled ? "true" : "false";
		json += ",\"battleLoggingEnabled\":";
		json += status.battleLoggingEnabled ? "true" : "false";
		json += ",\"running\":";
		json += status.running ? "true" : "false";
		json += ",\"ready\":";
		json += status.ready ? "true" : "false";
		json += ",\"romLoaded\":";
		json += status.romLoaded ? "true" : "false";
		json += ",\"supportedGame\":";
		json += status.supportedGame ? "true" : "false";
		json += ",\"port\":";
		json += std::to_string(status.port);
		json += ",\"url\":";
		AppendJsonEscaped(json, status.url);
		json += ",\"gameCode\":";
		AppendJsonEscaped(json, status.gameCode);
		json += ",\"selectedProfile\":";
		AppendJsonEscaped(json, status.selectedProfile);
		json += ",\"platinumTimeOverride\":";
		AppendJsonEscaped(json, status.platinumTimeOverride);
		json += ",\"platinumTimeOverrideActive\":";
		json += status.platinumTimeOverrideActive ? "true" : "false";
		json += ",\"effectiveGameTime\":";
		AppendJsonEscaped(json, status.effectiveGameTime);
		json += ",\"status\":";
		AppendJsonEscaped(json, status.statusText);
		json += ",\"lastError\":";
		AppendJsonEscaped(json, status.lastError);
		json += ",\"lastSuccessUnixMs\":";
		json += std::to_string(status.lastSuccessUnixMs);
		json += ",\"healthCheckEnabled\":";
		json += status.healthCheckEnabled ? "true" : "false";
		json += ",\"healthCheckIntervalMs\":";
		json += std::to_string(status.healthCheckIntervalMs);
		json += ",\"idleRefreshEnabled\":";
		json += status.idleRefreshEnabled ? "true" : "false";
		json += ",\"idleRefreshIntervalMs\":";
		json += std::to_string(status.idleRefreshIntervalMs);
		json += ",\"battleDetectPollPreset\":";
		AppendJsonEscaped(json, GetBattleDetectPollPresetName(status.battleDetectPollPreset));
		json += ",\"battleDetectPollMs\":";
		json += std::to_string(status.battleDetectPollMs);
		json += ",\"playerBadges\":{";
		json += "\"available\":";
		json += status.playerBadges.available ? "true" : "false";
		json += ",\"source\":";
		if (!status.playerBadges.source.empty()) {
			AppendJsonEscaped(json, status.playerBadges.source);
		} else {
			json += "null";
		}
		json += ",\"pidAddr\":";
		if (status.playerBadges.pidAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(status.playerBadges.pidAddr));
		} else {
			json += "null";
		}
		json += ",\"profileAddr\":";
		if (status.playerBadges.profileAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(status.playerBadges.profileAddr));
		} else {
			json += "null";
		}
		json += ",\"badgeCount\":";
		if (status.playerBadges.available) {
			json += std::to_string(status.playerBadges.badgeCount);
		} else {
			json += "null";
		}
		json += ",\"badgeFlags\":";
		if (status.playerBadges.available) {
			json += std::to_string(status.playerBadges.badgeFlags);
		} else {
			json += "null";
		}
		json += ",\"badgeFlagsHex\":";
		if (status.playerBadges.available) {
			AppendJsonEscaped(json, FormatHex(status.playerBadges.badgeFlags));
		} else {
			json += "null";
		}
		json += ",\"lastError\":";
		AppendJsonEscaped(json, status.playerBadges.lastError);
		json += "}";
		json += ",\"ramDumpUrl\":";
		AppendJsonEscaped(json, std::string("http://127.0.0.1:") + std::to_string(status.port) + "/ram_dump");
		json += ",\"ramDumpStart\":";
		AppendJsonEscaped(json, FormatHex(kArm9MainRamStart));
		json += ",\"ramDumpEnd\":";
		AppendJsonEscaped(json, FormatHex(kArm9MainRamEnd));
		json += ",\"battleDetection\":{";
		json += "\"battleActive\":";
		json += status.battleDetection.battleActive ? "true" : "false";
		json += ",\"trainerBattleActive\":";
		json += status.battleDetection.trainerBattleActive ? "true" : "false";
		json += ",\"trainerBattleCanStartSession\":";
		json += status.battleDetection.trainerBattleCanStartSession ? "true" : "false";
		json += ",\"trainerBattleSessionActive\":";
		json += status.battleDetection.trainerBattleSessionActive ? "true" : "false";
		json += ",\"battleSystemAddr\":";
		if (status.battleDetection.battleSystemAddr != 0) {
			AppendJsonEscaped(json, FormatHex(status.battleDetection.battleSystemAddr));
		} else {
			json += "null";
		}
		json += ",\"battleContextAddr\":";
		if (status.battleDetection.battleContextAddr != 0) {
			AppendJsonEscaped(json, FormatHex(status.battleDetection.battleContextAddr));
		} else {
			json += "null";
		}
		json += ",\"battleType\":";
		if (status.battleDetection.battleType != 0) {
			AppendJsonEscaped(json, FormatHex(status.battleDetection.battleType));
		} else {
			json += "null";
		}
		json += ",\"enemyTrainerId\":";
		if (status.battleDetection.enemyTrainerId != 0) {
			json += std::to_string(status.battleDetection.enemyTrainerId);
		} else {
			json += "null";
		}
		json += ",\"maxBattlers\":";
		json += std::to_string(status.battleDetection.maxBattlers);
		json += ",\"detectionSource\":";
		AppendJsonEscaped(json, status.battleDetection.detectionSource);
		json += ",\"detectionSourceDepth\":";
		json += std::to_string(status.battleDetection.detectionSourceDepth);
		json += ",\"detectionConfidence\":";
		json += std::to_string(status.battleDetection.detectionConfidence);
		json += ",\"lastPollUnixMs\":";
		json += std::to_string(status.battleDetection.lastPollUnixMs);
		json += ",\"lastStateChangeUnixMs\":";
		json += std::to_string(status.battleDetection.lastStateChangeUnixMs);
		json += ",\"lastSessionStartUnixMs\":";
		json += std::to_string(status.battleDetection.lastSessionStartUnixMs);
		json += ",\"lastBattleEndUnixMs\":";
		json += std::to_string(status.battleDetection.lastBattleEndUnixMs);
		json += ",\"lastLifecycleEvent\":";
		AppendJsonEscaped(json, status.battleDetection.lastLifecycleEvent);
		json += "}";
		json += ",\"battleLog\":{";
		json += "\"active\":";
		json += status.battleLog.active ? "true" : "false";
		json += ",\"sessionCount\":";
		json += std::to_string(status.battleLog.sessionCount);
		json += ",\"activeSessionEventCount\":";
		json += std::to_string(status.battleLog.activeSessionEventCount);
		json += ",\"activeTrainerId\":";
		if (status.battleLog.activeTrainerId != 0u) {
			json += std::to_string(status.battleLog.activeTrainerId);
		} else {
			json += "null";
		}
		json += ",\"currentTurn\":";
		json += std::to_string(status.battleLog.currentTurn);
		json += ",\"lastSessionStartUnixMs\":";
		json += std::to_string(status.battleLog.lastSessionStartUnixMs);
		json += ",\"lastSessionEndUnixMs\":";
		json += std::to_string(status.battleLog.lastSessionEndUnixMs);
		json += ",\"lastEventType\":";
		AppendJsonEscaped(json, status.battleLog.lastEventType);
		json += ",\"lastEventTurn\":";
		json += std::to_string(status.battleLog.lastEventTurn);
		json += ",\"lastEventPlayerSlot\":";
		json += std::to_string(status.battleLog.lastEventPlayerSlot);
		json += ",\"lastEventAiSpecies\":";
		if (status.battleLog.lastEventAiSpecies != 0u) {
			json += std::to_string(status.battleLog.lastEventAiSpecies);
		} else {
			json += "null";
		}
		json += ",\"outputDir\":";
		AppendJsonEscaped(json, status.battleLog.outputDir);
		json += ",\"attemptMapPath\":";
		AppendJsonEscaped(json, status.battleLog.attemptMapPath);
		json += ",\"lastPersistedMasterPath\":";
		if (!status.battleLog.lastPersistedMasterPath.empty()) {
			AppendJsonEscaped(json, status.battleLog.lastPersistedMasterPath);
		} else {
			json += "null";
		}
		json += ",\"lastPersistSucceeded\":";
		json += status.battleLog.lastPersistSucceeded ? "true" : "false";
		json += ",\"lastPersistError\":";
		AppendJsonEscaped(json, status.battleLog.lastPersistError);
		json += "}";
		json += ",\"hgssDiscovery\":{";
		json += "\"active\":";
		json += status.hgssDiscovery.active ? "true" : "false";
		json += ",\"trainerIdsResolved\":";
		json += status.hgssDiscovery.trainerIdsResolved ? "true" : "false";
		json += ",\"trainerIdsSource\":";
		AppendJsonEscaped(json, status.hgssDiscovery.trainerIdsSource);
		json += ",\"pidAddr\":";
		if (status.hgssDiscovery.pidAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(status.hgssDiscovery.pidAddr));
		} else {
			json += "null";
		}
		json += ",\"trainerId\":";
		if (status.hgssDiscovery.trainerIdsResolved) {
			json += std::to_string(status.hgssDiscovery.trainerId);
		} else {
			json += "null";
		}
		json += ",\"secretId\":";
		if (status.hgssDiscovery.trainerIdsResolved) {
			json += std::to_string(status.hgssDiscovery.secretId);
		} else {
			json += "null";
		}
		json += ",\"trainerIdsError\":";
		AppendJsonEscaped(json, status.hgssDiscovery.trainerIdsError);
		json += ",\"battleAnchorAddr\":";
		if (status.hgssDiscovery.battleAnchorAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(status.hgssDiscovery.battleAnchorAddr));
		} else {
			json += "null";
		}
		json += ",\"battleAnchorValue\":";
		if (status.hgssDiscovery.battleAnchorValue != 0u) {
			AppendJsonEscaped(json, FormatHex(status.hgssDiscovery.battleAnchorValue));
		} else {
			json += "null";
		}
		json += ",\"battleCandidateValid\":";
		json += status.hgssDiscovery.battleCandidateValid ? "true" : "false";
		json += ",\"battleProbeSource\":";
		AppendJsonEscaped(json, status.hgssDiscovery.battleProbeSource);
		json += ",\"battleSystemCandidateAddr\":";
		if (status.hgssDiscovery.battleSystemCandidateAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(status.hgssDiscovery.battleSystemCandidateAddr));
		} else {
			json += "null";
		}
		json += ",\"battleContextCandidateAddr\":";
		if (status.hgssDiscovery.battleContextCandidateAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(status.hgssDiscovery.battleContextCandidateAddr));
		} else {
			json += "null";
		}
		json += ",\"battleType\":";
		if (status.hgssDiscovery.battleType != 0u) {
			AppendJsonEscaped(json, FormatHex(status.hgssDiscovery.battleType));
		} else {
			json += "null";
		}
		json += ",\"enemyTrainerId\":";
		if (status.hgssDiscovery.enemyTrainerId != 0u) {
			json += std::to_string(status.hgssDiscovery.enemyTrainerId);
		} else {
			json += "null";
		}
		json += ",\"maxBattlers\":";
		json += std::to_string(status.hgssDiscovery.maxBattlers);
		json += ",\"command\":";
		if (status.hgssDiscovery.battleCandidateValid) {
			json += std::to_string(status.hgssDiscovery.command);
		} else {
			json += "null";
		}
		json += ",\"commandNext\":";
		if (status.hgssDiscovery.battleCandidateValid) {
			json += std::to_string(status.hgssDiscovery.commandNext);
		} else {
			json += "null";
		}
		json += ",\"battleConfidence\":";
		json += std::to_string(status.hgssDiscovery.battleConfidence);
		json += ",\"battleProbeError\":";
		AppendJsonEscaped(json, status.hgssDiscovery.battleProbeError);
		json += ",\"sampledMons\":[";
		for (size_t i = 0; i < status.hgssDiscovery.sampledMons.size(); ++i) {
			if (i != 0u) {
				json += ",";
			}
			const HgssBattleMonSample &sample = status.hgssDiscovery.sampledMons[i];
			json += "{";
			json += "\"valid\":";
			json += sample.valid ? "true" : "false";
			json += ",\"battler\":";
			json += std::to_string(sample.battler);
			json += ",\"partySlot\":";
			json += std::to_string(sample.partySlot);
			json += ",\"speciesId\":";
			if (sample.speciesId != 0u) {
				json += std::to_string(sample.speciesId);
			} else {
				json += "null";
			}
			json += ",\"curHp\":";
			json += std::to_string(sample.curHp);
			json += ",\"maxHp\":";
			json += std::to_string(sample.maxHp);
			json += "}";
		}
		json += "]";
		json += "}";
		json += "}";
		return json;
	}

	ActiveBattleSnapshot BuildActiveBattleSnapshot()
	{
		ActiveBattleSnapshot snapshot;
		snapshot.success = false;

		ExportGameProfile selectedProfile = EXPORT_GAME_NONE;
		BattleDetectionStatus battleStatus = MakeEmptyBattleDetectionStatus();
		{
			std::lock_guard<std::mutex> lock(_mutex);
			selectedProfile = _selectedProfile;
			battleStatus = _battleDetectionStatus;
		}

		const ExportProfileDefinition *profile = GetExportProfileDefinition(selectedProfile);
		if (profile == NULL) {
			snapshot.error = "No Pokemon export game selected";
			return snapshot;
		}

		const PokemonMemoryLayout *pokemonLayout = GetProfilePokemonLayout(profile);
		if (pokemonLayout == NULL) {
			snapshot.error = std::string(profile->displayName) + " memory layout is unavailable";
			return snapshot;
		}

		if (!BuildActiveBattleSnapshotJson(profile, battleStatus, snapshot.json, snapshot.error)) {
			return snapshot;
		}
		snapshot.success = true;
		return snapshot;
	}

	static std::string BuildJsonError(const std::string &message)
	{
		std::string json = "{\"error\":";
		AppendJsonEscaped(json, message);
		json += "}";
		return json;
	}

static void AppendU16LE(std::string &out, uint16_t value)
{
	out.push_back(static_cast<char>(value & 0xFFu));
	out.push_back(static_cast<char>((value >> 8) & 0xFFu));
}

static void AppendU32LE(std::string &out, uint32_t value)
{
	out.push_back(static_cast<char>(value & 0xFFu));
	out.push_back(static_cast<char>((value >> 8) & 0xFFu));
	out.push_back(static_cast<char>((value >> 16) & 0xFFu));
	out.push_back(static_cast<char>((value >> 24) & 0xFFu));
}

static void AppendZeroBytes(std::string &out, size_t count)
{
	out.append(count, '\0');
}

	Snapshot BuildSnapshot()
	{
		Snapshot snapshot;
		snapshot.success = false;
		snapshot.updatedAtUnixMs = CurrentUnixMs();
		snapshot.trainerIdsResolved = false;
		snapshot.trainerId = 0u;
		snapshot.secretId = 0u;

		ExportGameProfile selectedProfile = EXPORT_GAME_NONE;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			selectedProfile = _selectedProfile;
		}

		const ExportProfileDefinition *profile = GetExportProfileDefinition(selectedProfile);
		if (profile == NULL) {
			snapshot.error = "No Pokemon HTTP export game selected";
			SetError(snapshot.error);
			return snapshot;
		}

		const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
		if (layout == NULL) {
			snapshot.error = std::string(profile->displayName) + " memory layout is unavailable";
			SetError(snapshot.error);
			return snapshot;
		}

		PokemonRuntimeAddresses addresses;
		if (!ResolvePokemonRuntimeAddresses(profile, addresses, snapshot.error)) {
			SetError(snapshot.error);
			return snapshot;
		}

		uint16_t trainerId = 0u;
		uint16_t secretId = 0u;
		if (!TryResolveCurrentTrainerIds(selectedProfile, trainerId, secretId, snapshot.error)) {
			SetError(snapshot.error);
			return snapshot;
		}
		const uint32_t partyCount = std::min<uint32_t>(ReadArm9U8(addresses.partyCountAddr), 6u);
		const uint32_t partyBaseAddr = addresses.partyBaseAddr;
		const uint32_t boxBaseAddr = addresses.boxBaseAddr;
		const uint32_t currentBoxIndex = ReadCurrentBoxIndexForProfile(layout, addresses);
		const uint32_t liveBoxCount = GetLiveExportBoxCount(layout);
		const uint32_t deadBoxStartIndex = GetDeadExportStartBoxIndex(layout);
		const uint32_t deadBoxCount = GetDeadExportBoxCount(layout);
		const uint32_t endpointCurrentBoxIndex = GetEndpointCurrentBoxIndex(layout, currentBoxIndex);

		std::string showdown;
		std::string json = "{";
		json += "\"game\":";
		AppendJsonEscaped(json, profile->gameName);
		json += ",\"language\":";
		AppendJsonEscaped(json, addresses.language);
		json += ",\"gameCode\":";
		AppendJsonEscaped(json, GameCodeString());
		json += ",\"selectedProfile\":";
		AppendJsonEscaped(json, profile->displayName);
		json += ",\"updatedAtUnixMs\":";
		json += std::to_string(snapshot.updatedAtUnixMs);
		json += ",\"pidPointerAddr\":";
		if (addresses.pidPointerAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(addresses.pidPointerAddr));
		} else {
			json += "null";
		}
		json += ",\"trainerIDsPointerAddr\":";
		if (addresses.trainerIdsPointerAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(addresses.trainerIdsPointerAddr));
		} else {
			json += "null";
		}
		json += ",\"pidAddr\":";
		if (addresses.pidAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(addresses.pidAddr));
		} else {
			json += "null";
		}
		json += ",\"trainerIDsAddr\":";
		if (addresses.trainerIdsAddr != 0u) {
			AppendJsonEscaped(json, FormatHex(addresses.trainerIdsAddr));
		} else {
			json += "null";
		}
		json += ",\"partyCount\":";
		json += std::to_string(partyCount);
		json += ",\"partyBaseAddr\":";
		AppendJsonEscaped(json, FormatHex(partyBaseAddr));
		json += ",\"boxBaseAddr\":";
		AppendJsonEscaped(json, FormatHex(boxBaseAddr));
		json += ",\"currentBoxIndex\":";
		json += std::to_string(endpointCurrentBoxIndex + 1);
		json += ",\"boxSlotsDumped\":";
		json += std::to_string(liveBoxCount * layout->partySlotsPerBox);
		json += ",\"deadBoxSlotsDumped\":";
		json += std::to_string(deadBoxCount * layout->partySlotsPerBox);
		json += ",\"trainerId\":";
		json += std::to_string(trainerId);
		json += ",\"secretId\":";
		json += std::to_string(secretId);
		json += ",\"party\":[";

		for (uint32_t slot = 0; slot < partyCount; ++slot) {
			if (slot != 0) {
				json += ",";
			}

			DecryptedPokemon pokemon;
			const uint32_t monOffset = partyBaseAddr + (slot * static_cast<uint32_t>(layout->partyMonSize));
			PartyPokemonReadMode readMode;
			if (!ReadPartyPokemonRobustForProfile(profile, monOffset, pokemon, readMode)) {
				json += "null";
				continue;
			}
			(void)readMode;

			PartyPokemonSummary summary;
			if (!BuildPartyPokemonSummary(profile, pokemon, monOffset, static_cast<int>(slot + 1), summary)) {
				json += "null";
				continue;
			}

			std::string showdownEntry;
			json += BuildPokemonJson(profile, pokemon, monOffset, "party", 0, static_cast<int>(slot + 1), showdownEntry);
			showdown += showdownEntry;
		}

		json += "],\"boxes\":[";
		for (uint32_t box = 0; box < liveBoxCount; ++box) {
			if (box != 0) {
				json += ",";
			}

			json += "{\"box\":";
			json += std::to_string(box + 1);
			json += ",\"slots\":[";

			for (uint32_t slot = 0; slot < layout->partySlotsPerBox; ++slot) {
				if (slot != 0) {
					json += ",";
				}

				const uint32_t monOffset = GetBoxMonAddress(layout, boxBaseAddr, box, slot);
				if (IsBoxPokemonSlotEmpty(layout, monOffset)) {
					json += "null";
					continue;
				}

				DecryptedPokemon pokemon;
				if (!TryDecryptPokemonWithRetriesForProfile(profile, monOffset, true, pokemon)) {
					WriteFailedBoxSlotReadBugReport(
						profile,
						boxBaseAddr,
						liveBoxCount,
						static_cast<int>(box + 1u),
						static_cast<int>(slot + 1u),
						monOffset,
						"box_snapshot_decrypt_failed");
					json += "null";
					continue;
				}

				BoxPokemonSummary summary;
				if (!BuildBoxPokemonSummary(profile, pokemon, monOffset, static_cast<int>(box + 1), static_cast<int>(slot + 1), summary)) {
					json += "null";
					continue;
				}

				std::string showdownEntry;
				json += BuildPokemonJson(profile, pokemon, monOffset, "box", static_cast<int>(box + 1), static_cast<int>(slot + 1), showdownEntry);
				if (summary.speciesId != 0u) {
					showdown += showdownEntry;
				}
			}

			json += "]}";
		}
		json += "]";
		json += ",\"deadBoxes\":[";
		for (uint32_t box = 0; box < deadBoxCount; ++box) {
			if (box != 0) {
				json += ",";
			}

			const uint32_t actualBoxIndex = deadBoxStartIndex + box;
			json += "{\"box\":";
			json += std::to_string(actualBoxIndex + 1);
			json += ",\"slots\":[";

			for (uint32_t slot = 0; slot < layout->partySlotsPerBox; ++slot) {
				if (slot != 0) {
					json += ",";
				}

				const uint32_t monOffset = GetBoxMonAddress(layout, boxBaseAddr, actualBoxIndex, slot);
				if (IsBoxPokemonSlotEmpty(layout, monOffset)) {
					json += "null";
					continue;
				}

				DecryptedPokemon pokemon;
				if (!TryDecryptPokemonWithRetriesForProfile(profile, monOffset, true, pokemon)) {
					json += "null";
					continue;
				}

				BoxPokemonSummary summary;
				if (!BuildBoxPokemonSummary(profile, pokemon, monOffset, static_cast<int>(actualBoxIndex + 1), static_cast<int>(slot + 1), summary)) {
					json += "null";
					continue;
				}

				uint16_t deadSpeciesId = 0u;
				uint16_t deadLocationId = 0u;
				std::string deadSpecies;
				std::string deadNickname;
				bool deadHasNickname = false;
				std::string deadLocation;
				std::string deadShowdownEntry;
				json += BuildDeadPokemonJson(
					profile,
					pokemon,
					monOffset,
					static_cast<int>(actualBoxIndex + 1),
					static_cast<int>(slot + 1),
					deadSpeciesId,
					deadLocationId,
					deadSpecies,
					deadNickname,
					deadHasNickname,
					deadLocation,
					deadShowdownEntry);
				if (deadSpeciesId != 0u) {
					showdown += deadShowdownEntry;
				}
			}

			json += "]}";
		}
		json += "]";
		json += ",\"showdown\":";
		AppendJsonEscaped(json, showdown);
		json += "}";

		snapshot.success = true;
		snapshot.showdown = showdown;
		snapshot.rawJson = json;
		snapshot.trainerIdsResolved = true;
		snapshot.trainerId = trainerId;
		snapshot.secretId = secretId;
		SetLastSuccess(snapshot.updatedAtUnixMs);
		return snapshot;
	}

	PackedSnapshot BuildPackedSnapshot()
	{
		PackedSnapshot snapshot;
		snapshot.success = false;

		ExportGameProfile selectedProfile = EXPORT_GAME_NONE;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			selectedProfile = _selectedProfile;
		}

		const ExportProfileDefinition *profile = GetExportProfileDefinition(selectedProfile);
		if (profile == NULL) {
			snapshot.error = "No Pokemon HTTP export game selected";
			SetError(snapshot.error);
			return snapshot;
		}

		if (gameInfo.romsize == 0) {
			snapshot.error = "No ROM loaded";
			SetError(snapshot.error);
			return snapshot;
		}

		const PokemonMemoryLayout *layout = GetProfilePokemonLayout(profile);
		if (layout == NULL) {
			snapshot.error = std::string(profile->displayName) + " memory layout is unavailable";
			SetError(snapshot.error);
			return snapshot;
		}

		PokemonRuntimeAddresses addresses;
		if (!ResolvePokemonRuntimeAddresses(profile, addresses, snapshot.error)) {
			SetError(snapshot.error);
			return snapshot;
		}

		uint16_t trainerId = 0u;
		uint16_t secretId = 0u;
		if (!TryResolveCurrentTrainerIds(selectedProfile, trainerId, secretId, snapshot.error)) {
			SetError(snapshot.error);
			return snapshot;
		}
		struct PackedDeadMonRecord {
			uint8_t box;
			uint8_t slot;
			uint16_t speciesId;
			uint16_t metLocationId;
		};
		const uint8_t partyCount = static_cast<uint8_t>(std::min<uint32_t>(ReadArm9U8(addresses.partyCountAddr), 6u));
		const uint16_t partyStructSize = static_cast<uint16_t>(layout->partyMonSize);
		const uint16_t boxStructSize = static_cast<uint16_t>(layout->boxMonSize);
		const uint8_t liveBoxCount = static_cast<uint8_t>(GetLiveExportBoxCount(layout));
		const uint16_t boxSlotsDumped = static_cast<uint16_t>(liveBoxCount * layout->partySlotsPerBox);
		const uint32_t deadBoxStartIndex = GetDeadExportStartBoxIndex(layout);
		const uint32_t deadBoxCount = GetDeadExportBoxCount(layout);
		const uint8_t currentBoxIndex = static_cast<uint8_t>(GetEndpointCurrentBoxIndex(layout, ReadCurrentBoxIndexForProfile(layout, addresses)));
		const uint32_t partyBaseAddr = addresses.partyBaseAddr;
		const uint32_t boxBaseAddr = addresses.boxBaseAddr;
		std::vector<PackedDeadMonRecord> deadRecords;
		deadRecords.reserve(static_cast<size_t>(deadBoxCount) * layout->partySlotsPerBox);
		struct SaveBattleLogRange {
			uint32_t offset;
			uint32_t size;
		};
		std::vector<u8> rawSaveData;
		std::vector<SaveBattleLogRange> saveBattleLogRanges;
		uint8_t saveBattleLogBaseVersion = 0u;
		if (IsGen5BwLikePokemonProfile(selectedProfile) && MMU_new.backupDevice.copyRawData(rawSaveData)) {
			static const SaveBattleLogRange kBaseRanges[] = {
				{0x19600u, 0x1338u},
				{0x1AA00u, 0x07C4u},
				{0x1B200u, 0x0D54u}
			};
			const uint32_t secondCopyOffset = (selectedProfile == EXPORT_GAME_POKEMON_BLACK_WHITE)
				? 0x24000u
				: 0x26000u;
			saveBattleLogBaseVersion = (selectedProfile == EXPORT_GAME_POKEMON_BLACK_WHITE) ? 1u : 2u;
			for (size_t copyIndex = 0; copyIndex < 2u; ++copyIndex) {
				const uint32_t copyOffset = (copyIndex == 0u) ? 0u : secondCopyOffset;
				for (size_t rangeIndex = 0; rangeIndex < (sizeof(kBaseRanges) / sizeof(kBaseRanges[0])); ++rangeIndex) {
					SaveBattleLogRange range = kBaseRanges[rangeIndex];
					range.offset += copyOffset;
					if (range.offset > rawSaveData.size() || range.size > (rawSaveData.size() - range.offset)) {
						saveBattleLogRanges.clear();
						break;
					}
					saveBattleLogRanges.push_back(range);
				}
				if (saveBattleLogRanges.empty()) {
					break;
				}
			}
			if (saveBattleLogRanges.size() != 6u) {
				saveBattleLogRanges.clear();
				saveBattleLogBaseVersion = 0u;
			}
		}

		for (uint32_t box = 0; box < deadBoxCount; ++box) {
			const uint32_t actualBoxIndex = deadBoxStartIndex + box;
			for (uint32_t slot = 0; slot < layout->partySlotsPerBox; ++slot) {
				const uint32_t monOffset = GetBoxMonAddress(layout, boxBaseAddr, actualBoxIndex, slot);
				if (IsBoxPokemonSlotEmpty(layout, monOffset)) {
					continue;
				}

				DecryptedPokemon pokemon;
				BoxPokemonSummary summary;
				if (!TryDecryptPokemonWithRetriesForProfile(profile, monOffset, true, pokemon) ||
					!BuildBoxPokemonSummary(profile, pokemon, monOffset, static_cast<int>(actualBoxIndex + 1u), static_cast<int>(slot + 1u), summary)) {
					continue;
				}

				uint16_t deadSpeciesId = 0u;
				uint16_t deadLocationId = 0u;
				std::string deadSpecies;
				std::string deadNickname;
				bool deadHasNickname = false;
				std::string deadLocation;
				std::string deadShowdownEntry;
				(void)BuildDeadPokemonJson(
					profile,
					pokemon,
					monOffset,
					static_cast<int>(actualBoxIndex + 1u),
					static_cast<int>(slot + 1u),
					deadSpeciesId,
					deadLocationId,
					deadSpecies,
					deadNickname,
					deadHasNickname,
					deadLocation,
					deadShowdownEntry);
				if (deadSpeciesId == 0u) {
					continue;
				}

				PackedDeadMonRecord record;
				record.box = static_cast<uint8_t>(actualBoxIndex + 1u);
				record.slot = static_cast<uint8_t>(slot + 1u);
				record.speciesId = deadSpeciesId;
				record.metLocationId = deadLocationId;
				deadRecords.push_back(record);
			}
		}

		std::string payload;
		payload.reserve(18 + (static_cast<size_t>(partyCount) * layout->partyMonSize) + (static_cast<size_t>(boxSlotsDumped) * layout->boxMonSize) + (deadRecords.size() * 6u));
		payload += saveBattleLogRanges.empty() ? "DPB2" : "DPB3";
		AppendU16LE(payload, trainerId);
		AppendU16LE(payload, secretId);
		payload.push_back(static_cast<char>(partyCount));
		payload.push_back(static_cast<char>(liveBoxCount));
		AppendU16LE(payload, partyStructSize);
		AppendU16LE(payload, boxStructSize);
		AppendU16LE(payload, boxSlotsDumped);
		payload.push_back(static_cast<char>(currentBoxIndex));
		payload.push_back(static_cast<char>(0));

		for (uint32_t slot = 0; slot < partyCount; ++slot) {
			const uint32_t monOffset = partyBaseAddr + (static_cast<uint32_t>(slot) * static_cast<uint32_t>(partyStructSize));
			DecryptedPokemon pokemon;
			PartyPokemonReadMode readMode;
			PartyPokemonSummary summary;
			if (!ReadPartyPokemonRobustForProfile(profile, monOffset, pokemon, readMode) ||
				!BuildPartyPokemonSummary(profile, pokemon, monOffset, static_cast<int>(slot + 1u), summary)) {
				AppendZeroBytes(payload, partyStructSize);
				continue;
			}
			(void)readMode;

			for (uint32_t byteIndex = 0; byteIndex < static_cast<uint32_t>(partyStructSize); ++byteIndex) {
				payload.push_back(static_cast<char>(ReadArm9U8(monOffset + byteIndex)));
			}
		}
		for (uint32_t box = 0; box < liveBoxCount; ++box) {
			for (uint32_t slot = 0; slot < layout->partySlotsPerBox; ++slot) {
				const uint32_t monOffset = GetBoxMonAddress(layout, boxBaseAddr, box, slot);
				if (IsBoxPokemonSlotEmpty(layout, monOffset)) {
					AppendZeroBytes(payload, boxStructSize);
					continue;
				}

				DecryptedPokemon pokemon;
				if (!TryDecryptPokemonWithRetriesForProfile(profile, monOffset, true, pokemon)) {
					WriteFailedBoxSlotReadBugReport(
						profile,
						boxBaseAddr,
						liveBoxCount,
						static_cast<int>(box + 1u),
						static_cast<int>(slot + 1u),
						monOffset,
						"packed_box_snapshot_decrypt_failed");
					AppendZeroBytes(payload, boxStructSize);
					continue;
				}

				BoxPokemonSummary summary;
				if (!BuildBoxPokemonSummary(profile, pokemon, monOffset, static_cast<int>(box + 1u), static_cast<int>(slot + 1u), summary)) {
					AppendZeroBytes(payload, boxStructSize);
					continue;
				}

				for (uint32_t byteIndex = 0; byteIndex < static_cast<uint32_t>(boxStructSize); ++byteIndex) {
					payload.push_back(static_cast<char>(ReadArm9U8(monOffset + byteIndex)));
				}
			}
		}
		AppendU16LE(payload, static_cast<uint16_t>(deadRecords.size()));
		for (size_t i = 0; i < deadRecords.size(); ++i) {
			payload.push_back(static_cast<char>(deadRecords[i].box));
			payload.push_back(static_cast<char>(deadRecords[i].slot));
			AppendU16LE(payload, deadRecords[i].speciesId);
			AppendU16LE(payload, deadRecords[i].metLocationId);
		}
		if (!saveBattleLogRanges.empty()) {
			payload += "GBL1";
			payload.push_back(static_cast<char>(saveBattleLogBaseVersion));
			payload.push_back(static_cast<char>(saveBattleLogRanges.size()));
			AppendU16LE(payload, 0u);
			for (size_t i = 0; i < saveBattleLogRanges.size(); ++i) {
				const SaveBattleLogRange &range = saveBattleLogRanges[i];
				AppendU32LE(payload, range.offset);
				AppendU32LE(payload, range.size);
				payload.append(
					reinterpret_cast<const char *>(&rawSaveData[range.offset]),
					static_cast<size_t>(range.size));
			}
		}

		snapshot.success = true;
		snapshot.payload = payload;
		SetLastSuccess(CurrentUnixMs());
		return snapshot;
	}

	BattleLogSnapshot BuildBattleLogSnapshot()
	{
		BattleLogSnapshot snapshot;
		snapshot.success = false;

		ExportGameProfile selectedProfile = EXPORT_GAME_NONE;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			selectedProfile = _selectedProfile;
		}

		uint16_t trainerId = 0u;
		uint16_t secretId = 0u;
		if (!TryResolveCurrentTrainerIds(selectedProfile, trainerId, secretId, snapshot.error)) {
			SetError(snapshot.error);
			return snapshot;
		}

		const std::string trainerKey = std::to_string(trainerId) + ":" + std::to_string(secretId);
		const AttemptMapData mapData = LoadAttemptMap();
		uint32_t attempt = 0u;
		if (!TryGetExistingAttemptForTrainerKey(trainerKey, mapData, attempt)) {
			snapshot.success = true;
			snapshot.payload = BuildEmptyPackedBattleLogPayload();
			SetLastSuccess(CurrentUnixMs());
			return snapshot;
		}

		const std::string masterPath = BuildAttemptMasterPath(attempt);
		std::vector<std::string> eventItems;
		if (!LoadAttemptBattleLogEventItems(masterPath, eventItems, snapshot.error)) {
			SetError(snapshot.error);
			return snapshot;
		}

		snapshot.success = true;
		snapshot.payload = BuildPackedBattleLogPayloadFromEventItems(eventItems);
		SetLastSuccess(CurrentUnixMs());
		return snapshot;
	}

private:
	std::mutex _mutex;
	bool _enabled;
	bool _clipboardSyncEnabled;
	bool _battleLoggingEnabled;
	bool _stopRequested;
	bool _threadRunning;
	SocketHandle _listenSocket;
	SocketHandle _clientSocket;
	uint64_t _lastSuccessUnixMs;
	std::string _lastError;
	ExportGameProfile _selectedProfile;
	PlatinumTimeOverride _platinumTimeOverride;
	bool _healthCheckEnabled;
	uint32_t _healthCheckIntervalMs;
	bool _idleRefreshEnabled;
	uint32_t _idleRefreshIntervalMs;
	BattleDetectPollPreset _battleDetectPollPreset;
	BattleDetectionStatus _battleDetectionStatus;
	BattleLogStatus _battleLogStatus;
	HgssDiscoveryStatus _hgssDiscoveryStatus;
	std::vector<BattleLogSession> _battleLogSessions;
	std::vector<BattleMonRuntimeSnapshot> _battleLogLastBattleMons;
	std::vector<BattleLogEnemyPartySnapshot> _battleLogLastEnemyPartyMons;
	std::vector<BattleLogRecordedEnemyKo> _battleLogRecordedEnemyKos;
	uint32_t _lastKnownBattleSystemAddr;
	uint64_t _lastFullScanUnixMs;
	uint32_t _battleLogLastCommand;
	std::thread _thread;
	bool _watchdogStopRequested;
	bool _watchdogThreadRunning;
	std::thread _watchdogThread;
};

}  // namespace

void SetEnabled(bool enabled)
{
	HttpExportBackend::Instance().SetEnabled(enabled);
}

bool IsEnabled()
{
	return HttpExportBackend::Instance().IsEnabled();
}

void SetClipboardSyncEnabled(bool enabled)
{
	HttpExportBackend::Instance().SetClipboardSyncEnabled(enabled);
}

bool IsClipboardSyncEnabled()
{
	return HttpExportBackend::Instance().IsClipboardSyncEnabled();
}

void SetBattleLoggingEnabled(bool enabled)
{
	HttpExportBackend::Instance().SetBattleLoggingEnabled(enabled);
}

bool IsBattleLoggingEnabled()
{
	return HttpExportBackend::Instance().IsBattleLoggingEnabled();
}

void SetSelectedProfile(ExportGameProfile profile)
{
	HttpExportBackend::Instance().SetSelectedProfile(profile);
}

ExportGameProfile GetSelectedProfile()
{
	return HttpExportBackend::Instance().GetSelectedProfile();
}

bool IsSelectedProfilePlatinumVariant()
{
	switch (HttpExportBackend::Instance().GetSelectedProfile())
	{
		case EXPORT_GAME_POKEMON_PLATINUM:
		case EXPORT_GAME_POKEMON_PLATINUM_KAIZO:
		case EXPORT_GAME_POKEMON_RENEGADE_PLATINUM:
			return true;

		default:
			return false;
	}
}

void SetPlatinumTimeOverride(PlatinumTimeOverride mode)
{
	HttpExportBackend::Instance().SetPlatinumTimeOverride(mode);
}

PlatinumTimeOverride GetPlatinumTimeOverride()
{
	return HttpExportBackend::Instance().GetPlatinumTimeOverride();
}

const char *GetProfileDisplayName(ExportGameProfile profile)
{
	const ExportProfileDefinition *definition = GetExportProfileDefinition(profile);
	return (definition != NULL) ? definition->displayName : "";
}

const char *GetPlatinumTimeOverrideDisplayName(PlatinumTimeOverride mode)
{
	return GetPlatinumTimeOverrideName(mode);
}

void SetHealthCheckEnabled(bool enabled)
{
	HttpExportBackend::Instance().SetHealthCheckEnabled(enabled);
}

bool IsHealthCheckEnabled()
{
	return HttpExportBackend::Instance().IsHealthCheckEnabled();
}

void SetHealthCheckIntervalMs(uint32_t intervalMs)
{
	HttpExportBackend::Instance().SetHealthCheckIntervalMs(intervalMs);
}

uint32_t GetHealthCheckIntervalMs()
{
	return HttpExportBackend::Instance().GetHealthCheckIntervalMs();
}

void SetIdleRefreshEnabled(bool enabled)
{
	HttpExportBackend::Instance().SetIdleRefreshEnabled(enabled);
}

bool IsIdleRefreshEnabled()
{
	return HttpExportBackend::Instance().IsIdleRefreshEnabled();
}

void SetIdleRefreshIntervalMs(uint32_t intervalMs)
{
	HttpExportBackend::Instance().SetIdleRefreshIntervalMs(intervalMs);
}

uint32_t GetIdleRefreshIntervalMs()
{
	return HttpExportBackend::Instance().GetIdleRefreshIntervalMs();
}

bool ForceRefresh()
{
	return HttpExportBackend::Instance().ForceRefresh();
}

bool GetShowdownExportText(std::string &text, std::string &error)
{
	return HttpExportBackend::Instance().GetShowdownExportText(text, error);
}

void SetBattleDetectPollPreset(BattleDetectPollPreset preset)
{
	HttpExportBackend::Instance().SetBattleDetectPollPreset(preset);
}

BattleDetectPollPreset GetBattleDetectPollPreset()
{
	return HttpExportBackend::Instance().GetBattleDetectPollPreset();
}

const char *GetBattleDetectPollPresetDisplayName(BattleDetectPollPreset preset)
{
	return GetBattleDetectPollPresetName(preset);
}

Status GetStatus()
{
	return HttpExportBackend::Instance().GetStatus();
}

bool GetPartyEditorSnapshot(PartyEditorSnapshot &snapshot)
{
	return BuildPartyEditorSnapshotInternal(HttpExportBackend::Instance().GetSelectedProfile(), snapshot);
}

bool GetPartyEditorSnapshotForLoadedGame(PartyEditorSnapshot &snapshot)
{
	const ExportGameProfile selectedProfile = HttpExportBackend::Instance().GetSelectedProfile();
	const std::string gameCode = GameCodeString();

	ExportGameProfile profileToUse = selectedProfile;
	if (!IsProfileCompatibleWithGameCode(profileToUse, gameCode)) {
		if ((gameCode == "CPUE") || (gameCode == "JAK7")) {
			profileToUse = EXPORT_GAME_POKEMON_PLATINUM;
		} else if ((gameCode == "IPKE") || (gameCode == "IPGE")) {
			profileToUse = EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER;
		} else {
			PokemonRuntimeAddresses addresses;
			std::string error;
			if (TryResolveBw2RuntimeAddresses(gameCode, addresses, error)) {
				profileToUse = EXPORT_GAME_POKEMON_BLACK2_WHITE2;
			} else if (TryResolveBwRuntimeAddresses(gameCode, addresses, error)) {
				profileToUse = EXPORT_GAME_POKEMON_BLACK_WHITE;
			} else {
				profileToUse = EXPORT_GAME_NONE;
			}
		}
	}

	return BuildPartyEditorSnapshotInternal(profileToUse, snapshot);
}

bool UpdatePartyPokemon(const PartyPokemonEditRequest &request, std::string &error)
{
	return UpdatePartyPokemonInternal(HttpExportBackend::Instance().GetSelectedProfile(), request, error);
}

bool UpdatePartyPokemonHpStatus(const PartyPokemonHpStatusEditRequest &request, std::string &error)
{
	return UpdatePartyPokemonHpStatusInternal(HttpExportBackend::Instance().GetSelectedProfile(), request, error);
}

bool IsPartyPokemonReadyForWrite(int slot, std::string &error)
{
	return IsPartyPokemonReadyForWriteInternal(HttpExportBackend::Instance().GetSelectedProfile(), slot, error);
}

bool IsEntirePartyReadyForWrite(std::string &error)
{
	return IsEntirePartyReadyForWriteInternal(HttpExportBackend::Instance().GetSelectedProfile(), error);
}

bool EdgePartyPokemon(int slot, uint32_t amount, std::string &error)
{
	return EdgePartyPokemonInternal(HttpExportBackend::Instance().GetSelectedProfile(), slot, amount, error);
}

bool GetBoxEditorSnapshot(BoxEditorSnapshot &snapshot)
{
	return BuildBoxEditorSnapshotInternal(HttpExportBackend::Instance().GetSelectedProfile(), snapshot);
}

bool UpdateBoxPokemon(const BoxPokemonEditRequest &request, std::string &error)
{
	return UpdateBoxPokemonInternal(HttpExportBackend::Instance().GetSelectedProfile(), request, error);
}

bool EdgeBoxPokemon(int box, int slot, uint32_t amount, std::string &error)
{
	return EdgeBoxPokemonInternal(HttpExportBackend::Instance().GetSelectedProfile(), box, slot, amount, error);
}

bool WriteEditorDebugSnapshot(const std::string &fileName, const std::string &text, std::string &pathOut, std::string &errorOut)
{
	return WriteEditorDebugSnapshotInternal(fileName, text, pathOut, errorOut);
}

bool WriteFullPartyRawBytesDebugSnapshot(std::string &pathOut, std::string &errorOut)
{
	return WriteFullPartyRawBytesDebugSnapshotInternal(
		HttpExportBackend::Instance().GetSelectedProfile(),
		pathOut,
		errorOut);
}

bool LookupMoveIdByName(const std::string &name, uint16_t &moveId, std::string &canonicalName)
{
	return LookupProfileMoveIdByName(GetExportProfileDefinition(HttpExportBackend::Instance().GetSelectedProfile()), name, moveId, canonicalName);
}

bool LookupSpeciesIdByName(const std::string &name, uint16_t &speciesId, std::string &canonicalName)
{
	return LookupSpeciesIdByNameForProfile(GetExportProfileDefinition(HttpExportBackend::Instance().GetSelectedProfile()), name, speciesId, canonicalName);
}

bool LookupItemIdByName(const std::string &name, uint16_t &itemId, std::string &canonicalName)
{
	return LookupItemIdByNameForProfile(GetExportProfileDefinition(HttpExportBackend::Instance().GetSelectedProfile()), name, itemId, canonicalName);
}

bool LookupAbilityIdByName(const std::string &name, uint16_t &abilityId, std::string &canonicalName)
{
	return LookupAbilityIdByNameForProfile(GetExportProfileDefinition(HttpExportBackend::Instance().GetSelectedProfile()), name, abilityId, canonicalName);
}

std::vector<std::string> GetSpeciesNameOptions()
{
	std::vector<std::string> options;
	const ExportProfileDefinition *profile = GetExportProfileDefinition(HttpExportBackend::Instance().GetSelectedProfile());
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		return options;
	}

	std::map<std::string, bool> seenNormalizedNames;
	options.reserve(tables->speciesNamesCount);
	for (size_t i = 1; i <= tables->speciesNamesCount; ++i) {
		const char *name = LookupProfileSpeciesName(profile, static_cast<int>(i));
		if (name != NULL && name[0] != '\0') {
			AppendUniqueSpeciesOption(options, seenNormalizedNames, name);
			for (uint32_t formIndex = 1u; formIndex <= 63u; ++formIndex) {
				const char *formSuffix = GetProfileFormSuffix(profile, static_cast<uint16_t>(i), static_cast<uint8_t>(formIndex));
				if (formSuffix == NULL || formSuffix[0] == '\0') {
					continue;
				}

				std::string formName(name);
				formName += formSuffix;
				AppendUniqueSpeciesOption(options, seenNormalizedNames, formName);
			}
		}
	}
	return options;
}

std::vector<std::string> GetItemNameOptions()
{
	std::vector<std::string> options;
	const ExportProfileDefinition *profile = GetExportProfileDefinition(HttpExportBackend::Instance().GetSelectedProfile());
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		return options;
	}

	options.reserve(tables->itemNamesCount);
	for (size_t i = 1; i <= tables->itemNamesCount; ++i) {
		const char *name = LookupProfileItemName(profile, static_cast<int>(i));
		if (name != NULL && name[0] != '\0') {
			options.push_back(name);
		}
	}
	return options;
}

std::vector<std::string> GetAbilityNameOptions()
{
	std::vector<std::string> options;
	const ExportProfileDefinition *profile = GetExportProfileDefinition(HttpExportBackend::Instance().GetSelectedProfile());
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		return options;
	}

	options.reserve(tables->abilityNamesCount);
	for (size_t i = 1; i <= tables->abilityNamesCount; ++i) {
		const char *name = LookupProfileAbilityName(profile, static_cast<int>(i));
		if (name != NULL && name[0] != '\0') {
			options.push_back(name);
		}
	}
	return options;
}

std::vector<std::string> GetMoveNameOptions()
{
	std::vector<std::string> options;
	const ExportProfileDefinition *profile = GetExportProfileDefinition(HttpExportBackend::Instance().GetSelectedProfile());
	const GameTextTables *tables = GetProfileTextTables(profile);
	if (tables == NULL) {
		return options;
	}

	options.reserve(tables->moveNamesCount);
	for (size_t i = 1; i <= tables->moveNamesCount; ++i) {
		const char *name = LookupProfileMoveName(profile, static_cast<int>(i));
		if (name != NULL && name[0] != '\0') {
			options.push_back(name);
		}
	}
	return options;
}

bool GetExperienceForSpeciesLevel(int speciesId, int level, uint32_t &experience)
{
	if (speciesId <= 0) {
		experience = 0;
		return false;
	}

	experience = GetProfileExperienceForLevel(GetExportProfileDefinition(HttpExportBackend::Instance().GetSelectedProfile()), level, speciesId);
	return (experience != 0);
}

bool SetEntirePartyStatus(uint32_t statusRaw, std::string &error)
{
	return SetEntirePartyStatusInternal(HttpExportBackend::Instance().GetSelectedProfile(), statusRaw, error);
}

bool EdgeEntireParty(uint32_t amount, std::string &error)
{
	return EdgeEntirePartyInternal(HttpExportBackend::Instance().GetSelectedProfile(), amount, error);
}

void Shutdown()
{
	HttpExportBackend::Instance().Shutdown();
}

}  // namespace desmume_pokemon_http_export

#endif  // DESMUME_POKEMON_HTTP_EXPORT_IMPLEMENTATION

#endif  // DESMUME_POKEMON_HTTP_EXPORT_H
