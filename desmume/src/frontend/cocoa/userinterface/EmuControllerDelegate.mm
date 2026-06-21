/*
	Copyright (C) 2013-2026 DeSmuME Team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#import "EmuControllerDelegate.h"
#import "DisplayWindowController.h"
#import "InputManager.h"
#import "cheatWindowDelegate.h"
#import "CheatDatabaseWindowController.h"
#import "Slot2WindowDelegate.h"
#import "MacAVCaptureTool.h"
#import "MacScreenshotCaptureTool.h"
#import "preferencesWindowDelegate.h"

#import "CocoaAudioController.h"
#import "CocoaDisplayView.h"
#import "CocoaGraphicsController.h"

#import "cocoa_globals.h"
#import "cocoa_cheat.h"
#import "cocoa_core.h"
#import "cocoa_file.h"
#import "cocoa_firmware.h"
#import "cocoa_GPU.h"
#import "cocoa_input.h"
#import "cocoa_rom.h"
#import "cocoa_slot2.h"

#include <algorithm>

#include "../../armcpu.h"
#include "../../interface/interface.h"
#include "../../NDSSystem.h"
#include "../../pokemon_more_scripts.h"

#define DESMUME_POKEMON_HTTP_EXPORT_IMPLEMENTATION
#include "../../pokemon_http_export.h"

#if HAVE_OSAVAILABLE && defined(MAC_OS_X_VERSION_10_14) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_14)
	#import <AVFoundation/AVFoundation.h>
#endif

static NSString *const PokemonHttpExportDefaultsKey = @"PokemonHttpExport_EnableLocalApi";
static NSString *const PokemonBattleLoggingDefaultsKey = @"PokemonHttpExport_BattleLoggingEnabled";
static NSString *const PokemonHttpExportProfileDefaultsKey = @"PokemonHttpExport_Profile";
static NSString *const PokemonPlatinumTimeOverrideDefaultsKey = @"PokemonHttpExport_PlatinumTimeOverride";
static NSString *const PokemonHttpExportHealthCheckEnabledDefaultsKey = @"PokemonHttpExport_HealthCheckEnabled";
static NSString *const PokemonHttpExportHealthCheckIntervalDefaultsKey = @"PokemonHttpExport_HealthCheckIntervalMs";
static NSString *const PokemonHttpExportIdleRefreshEnabledDefaultsKey = @"PokemonHttpExport_IdleRefreshEnabled";
static NSString *const PokemonHttpExportIdleRefreshIntervalDefaultsKey = @"PokemonHttpExport_IdleRefreshIntervalMs";
static NSString *const PokemonClipboardAutoCopyEnabledDefaultsKey = @"PokemonHttpExport_ClipboardAutoCopyEnabled";
static NSString *const PokemonClipboardAutoCopyIntervalDefaultsKey = @"PokemonHttpExport_ClipboardAutoCopyIntervalMs";
static NSString *const PokemonDisplayPartyAbilitySlotsDefaultsKey = @"PokemonMoreScripts_DisplayPartyAbilitySlots";
static NSString *const PokemonSpinInPlaceDefaultsKey = @"PokemonMoreScripts_SpinInPlace";
static NSString *const PokemonOneFrameTouchGuardDefaultsKey = @"PokemonInput_OneFrameTouchGuardEnabled";
static NSString *const LoadStateWritesBackupSaveDefaultsKey = @"Emulation_LoadStateWritesBackupSave";
static const NSInteger PokemonMenuRootTag = 3112400;
static const NSInteger PokemonHttpExportToggleTag = 3112401;
static const NSInteger PokemonHttpExportStatusTag = 3112402;
static const NSInteger PokemonHttpExportProfileMenuTag = 3112403;
static const NSInteger PokemonHttpExportProfilePlatinumTag = 3112404;
static const NSInteger PokemonHttpExportProfileHgssTag = 3112405;
static const NSInteger PokemonHttpExportProfilePlatinumKaizoTag = 3112406;
static const NSInteger PokemonHttpExportProfileSterlingSilverTag = 3112407;
static const NSInteger PokemonHttpExportProfileRenegadePlatinumTag = 3112408;
static const NSInteger PokemonHttpExportProfileBlackWhiteTag = 3112409;
static const NSInteger PokemonHttpExportProfileBlack2White2Tag = 3112449;
static const NSInteger PokemonHttpExportProfileBb2Vw2ReduxTag = 3112450;
static const NSInteger LoadStateWritesBackupSaveMenuTag = 3112451;
static const NSInteger PokemonPartyEditorMenuTag = 3112410;
static const NSInteger PokemonPartyStatusEditorMenuTag = 3112418;
static const NSInteger PokemonPlatinumTimeMenuTag = 3112411;
static const NSInteger PokemonPlatinumTimeAutoTag = 3112412;
static const NSInteger PokemonPlatinumTimeMorningTag = 3112413;
static const NSInteger PokemonPlatinumTimeDayTag = 3112414;
static const NSInteger PokemonPlatinumTimeEveningTag = 3112415;
static const NSInteger PokemonPlatinumTimeNightTag = 3112416;
static const NSInteger PokemonPlatinumTimeLateNightTag = 3112417;
static const NSInteger PokemonHttpExportMaintenanceMenuTag = 3112419;
static const NSInteger PokemonHttpExportForceRefreshTag = 3112420;
static const NSInteger PokemonHttpExportHealthCheckToggleTag = 3112421;
static const NSInteger PokemonHttpExportHealthCheckIntervalMenuTag = 3112422;
static const NSInteger PokemonHttpExportHealthCheckInterval15sTag = 3112423;
static const NSInteger PokemonHttpExportHealthCheckInterval30sTag = 3112424;
static const NSInteger PokemonHttpExportHealthCheckInterval60sTag = 3112425;
static const NSInteger PokemonHttpExportHealthCheckInterval120sTag = 3112426;
static const NSInteger PokemonHttpExportHealthCheckInterval300sTag = 3112427;
static const NSInteger PokemonHttpExportIdleRefreshToggleTag = 3112428;
static const NSInteger PokemonHttpExportIdleRefreshIntervalMenuTag = 3112429;
static const NSInteger PokemonHttpExportIdleRefreshInterval60sTag = 3112430;
static const NSInteger PokemonHttpExportIdleRefreshInterval180sTag = 3112431;
static const NSInteger PokemonHttpExportIdleRefreshInterval300sTag = 3112432;
static const NSInteger PokemonHttpExportIdleRefreshInterval600sTag = 3112433;
static const NSInteger PokemonBattleLoggingToggleTag = 3112434;
static const NSInteger PokemonCopyExportMenuTag = 3112435;
static const NSInteger PokemonClipboardAutoCopyToggleTag = 3112436;
static const NSInteger PokemonClipboardAutoCopyIntervalMenuTag = 3112437;
static const NSInteger PokemonClipboardAutoCopyInterval5sTag = 3112438;
static const NSInteger PokemonClipboardAutoCopyInterval10sTag = 3112439;
static const NSInteger PokemonClipboardAutoCopyInterval30sTag = 3112440;
static const NSInteger PokemonClipboardAutoCopyInterval60sTag = 3112441;
static const NSInteger PokemonPlatinumTrainerBattleOverrideStatusTag = 3112442;
static const NSInteger PokemonPlatinumTrainerBattleOverrideSetTag = 3112443;
static const NSInteger PokemonPlatinumTrainerBattleOverrideClearTag = 3112444;
static const NSInteger PokemonMoreScriptsMenuTag = 3112445;
static const NSInteger PokemonDisplayPartyAbilitySlotsTag = 3112446;
static const NSInteger PokemonSpinInPlaceTag = 3112447;
static const NSInteger PokemonOneFrameTouchGuardTag = 3112448;
static const uint64_t PokemonClipboardOutOfBattleRefreshDefaultMs = 10000u;
static const uint32_t PokemonPlatinumKnownLinkEncounterAddr = 0x020516C8u;
static const uint32_t PokemonPlatinumTrainerHookSearchLowerBound = 0x02051400u;
static const char *PokemonPartyApplyFailedTryAgainMessage = "Apply Failed, please try again";
static const int PokemonPartyApplyQueueMaxAttempts = 20;
static const NSTimeInterval PokemonPartyApplyQueueIntervalSeconds = 0.016;

enum PokemonPendingPartyWriteOperation
{
	PokemonPendingPartyWriteNone = 0,
	PokemonPendingPartyWriteApply,
	PokemonPendingPartyWriteHpStatus,
	PokemonPendingPartyWriteEdgeSlot,
	PokemonPendingPartyWriteBedtime,
	PokemonPendingPartyWriteEdgeParty
};

struct PlatinumTrainerBattleOverrideState
{
	bool installed;
	bool active;
	bool hasPendingTrainerId;
	uint32_t hookAddress;
	uint16_t pendingTrainerId;
};

static PlatinumTrainerBattleOverrideState gPokemonPlatinumTrainerBattleOverride = {};

static bool IsPokemonPlatinumRev1GameCode(const char *gameCode)
{
	return (memcmp(gameCode, "CPUE", 4) == 0) || (memcmp(gameCode, "JAK7", 4) == 0);
}

static bool IsPokemonPlatinumRev1Loaded()
{
	return (gameInfo.romsize != 0) && (gameInfo.header.romversion == 1) && IsPokemonPlatinumRev1GameCode(gameInfo.header.gameCode);
}

static bool IsPokemonPlatinumTrainerOverrideAvailable()
{
#ifdef TARGET_INTERFACE
	return IsPokemonPlatinumRev1Loaded() && !CommonSettings.use_jit;
#else
	return false;
#endif
}

static void ClearPokemonPlatinumTrainerBattleOverridePendingState()
{
	gPokemonPlatinumTrainerBattleOverride.hasPendingTrainerId = false;
	gPokemonPlatinumTrainerBattleOverride.pendingTrainerId = 0u;
}

static bool FindPreviousArmFunctionStart(uint32_t fromAddrExclusive, uint32_t lowerBoundInclusive, uint32_t &startAddrOut)
{
	if (fromAddrExclusive <= lowerBoundInclusive)
	{
		return false;
	}

	for (uint32_t addr = fromAddrExclusive - 4u;; addr -= 4u)
	{
		const uint32_t instruction = desmume_pokemon_http_export::ReadArm9U32(addr);
		if ((instruction & 0xFFFF0000u) == 0xE92D0000u)
		{
			startAddrOut = addr;
			return true;
		}

		if (addr <= lowerBoundInclusive)
		{
			break;
		}
	}

	return false;
}

static bool ResolvePokemonPlatinumTrainerHookAddress(uint32_t &hookAddrOut)
{
	uint32_t linkEncounterAddr = 0u;
	uint32_t trainerEncounterAddr = 0u;
	if (!FindPreviousArmFunctionStart(PokemonPlatinumKnownLinkEncounterAddr, PokemonPlatinumTrainerHookSearchLowerBound, linkEncounterAddr))
	{
		return false;
	}

	if (!FindPreviousArmFunctionStart(linkEncounterAddr, PokemonPlatinumTrainerHookSearchLowerBound, trainerEncounterAddr))
	{
		return false;
	}

	const uint32_t linkDistance = PokemonPlatinumKnownLinkEncounterAddr - linkEncounterAddr;
	const uint32_t trainerDistance = linkEncounterAddr - trainerEncounterAddr;
	if (linkDistance < 0x10u || linkDistance > 0x80u || trainerDistance < 0x20u || trainerDistance > 0x200u)
	{
		return false;
	}

	hookAddrOut = trainerEncounterAddr;
	return true;
}

#ifdef TARGET_INTERFACE
static BOOL PokemonPlatinumTrainerBattleOverride_OnEncounterExec(unsigned int address, int size)
{
	(void)address;
	(void)size;

	if (!gPokemonPlatinumTrainerBattleOverride.active ||
		!gPokemonPlatinumTrainerBattleOverride.hasPendingTrainerId ||
		(NDS_ARM9.CPSR.bits.T != 0))
	{
		return FALSE;
	}

	const uint16_t trainerId = gPokemonPlatinumTrainerBattleOverride.pendingTrainerId;
	const uint32_t enemyTrainer2 = NDS_ARM9.R[2];
	NDS_ARM9.R[1] = trainerId;
	NDS_ARM9.R[2] = (enemyTrainer2 != 0u) ? trainerId : 0u;
	ClearPokemonPlatinumTrainerBattleOverridePendingState();
	return FALSE;
}
#endif

static void RefreshPokemonPlatinumTrainerBattleOverrideHook()
{
	gPokemonPlatinumTrainerBattleOverride.active = false;

	if (!IsPokemonPlatinumTrainerOverrideAvailable())
	{
		return;
	}

#ifdef TARGET_INTERFACE
	if (!gPokemonPlatinumTrainerBattleOverride.installed)
	{
		uint32_t hookAddr = 0u;
		if (!ResolvePokemonPlatinumTrainerHookAddress(hookAddr))
		{
			return;
		}

		desmume_memory_register_exec((int)hookAddr, 4, PokemonPlatinumTrainerBattleOverride_OnEncounterExec);
		gPokemonPlatinumTrainerBattleOverride.installed = true;
		gPokemonPlatinumTrainerBattleOverride.hookAddress = hookAddr;
	}

	gPokemonPlatinumTrainerBattleOverride.active = true;
#endif
}

static uint32_t PokemonHttpHealthCheckIntervalForTag(NSInteger tag)
{
	switch (tag)
	{
		case PokemonHttpExportHealthCheckInterval15sTag: return 15000u;
		case PokemonHttpExportHealthCheckInterval30sTag: return 30000u;
		case PokemonHttpExportHealthCheckInterval60sTag: return 60000u;
		case PokemonHttpExportHealthCheckInterval120sTag: return 120000u;
		case PokemonHttpExportHealthCheckInterval300sTag: return 300000u;
		default: return 60000u;
	}
}

static uint32_t PokemonHttpIdleRefreshIntervalForTag(NSInteger tag)
{
	switch (tag)
	{
		case PokemonHttpExportIdleRefreshInterval60sTag: return 60000u;
		case PokemonHttpExportIdleRefreshInterval180sTag: return 180000u;
		case PokemonHttpExportIdleRefreshInterval300sTag: return 300000u;
		case PokemonHttpExportIdleRefreshInterval600sTag: return 600000u;
		default: return 180000u;
	}
}

static uint64_t PokemonClipboardAutoCopyIntervalForTag(NSInteger tag)
{
	switch (tag)
	{
		case PokemonClipboardAutoCopyInterval5sTag: return 5000u;
		case PokemonClipboardAutoCopyInterval10sTag: return 10000u;
		case PokemonClipboardAutoCopyInterval30sTag: return 30000u;
		case PokemonClipboardAutoCopyInterval60sTag: return 60000u;
		default: return PokemonClipboardOutOfBattleRefreshDefaultMs;
	}
}

static void ShowPokemonClipboardCopyError(NSString *message)
{
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert setMessageText:@"Failed to copy Pokemon export"];
	[alert setInformativeText:((message != nil) && ([message length] != 0)) ? message : @"Unknown error."];
	[alert runModal];
}

static uint64_t PokemonClipboardCurrentUnixMs()
{
	return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
}

static NSString *PokemonClipboardStringFromUtf8(const std::string &showdownText, NSString **errorOut)
{
	NSString *clipboardText = [[[NSString alloc] initWithBytes:showdownText.data()
	                                                    length:showdownText.size()
	                                                  encoding:NSUTF8StringEncoding] autorelease];
	if ((clipboardText == nil) && (errorOut != nil))
	{
		*errorOut = @"Failed to decode Pokemon export text as UTF-8.";
	}

	return clipboardText;
}

static BOOL WritePokemonClipboardString(NSString *clipboardText, NSString **errorOut)
{
	if (clipboardText == nil)
	{
		if (errorOut != nil)
		{
			*errorOut = @"Failed to decode Pokemon export text as UTF-8.";
		}
		return NO;
	}

	NSPasteboard *pboard = [NSPasteboard generalPasteboard];
	[pboard declareTypes:[NSArray arrayWithObjects:PASTEBOARDTYPE_STRING, nil] owner:nil];
	if (![pboard setString:clipboardText forType:PASTEBOARDTYPE_STRING])
	{
		if (errorOut != nil)
		{
			*errorOut = @"Failed to write Pokemon export text to the clipboard.";
		}
		return NO;
	}

	return YES;
}

@interface EmuControllerDelegate ()
- (void)installLoadStateBackupSaveMenuItem;
- (void)resetPokemonClipboardAutoSyncState;
- (void)notePokemonClipboardCopyWithText:(NSString *)clipboardText status:(const desmume_pokemon_http_export::Status &)status nowMs:(uint64_t)nowMs;
- (void)updatePokemonClipboardAutoSync:(NSTimer *)timer;
@end

typedef struct {
	uint32_t raw;
	const char *label;
} PokemonPartyStatusMenuEntry;

static const PokemonPartyStatusMenuEntry kPokemonPartyStatusMenuEntries[] = {
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

typedef struct {
	uint32_t raw;
	const char *label;
} PokemonPartyQuickStatusButtonEntry;

static const PokemonPartyQuickStatusButtonEntry kPokemonPartyQuickStatusButtons[] = {
	{0u, "Healthy"},
	{1u, "Sleep"},
	{8u, "Poison"},
	{16u, "Burn"},
	{32u, "Freeze"},
	{64u, "Paralysis"},
	{128u, "Toxic"}
};

static NSTextField *CreateStaticLabel(NSRect frame, NSString *text)
{
	NSTextField *label = [[[NSTextField alloc] initWithFrame:frame] autorelease];
	[label setStringValue:(text != nil) ? text : @""];
	[label setBezeled:NO];
	[label setDrawsBackground:NO];
	[label setEditable:NO];
	[label setSelectable:NO];
	return label;
}

static NSTextField *CreateEditableField(NSRect frame)
{
	NSTextField *field = [[[NSTextField alloc] initWithFrame:frame] autorelease];
	[field setBezeled:YES];
	[field setEditable:YES];
	[field setSelectable:YES];
	return field;
}

static NSTextField *CreateReadOnlyField(NSRect frame)
{
	NSTextField *field = [[[NSTextField alloc] initWithFrame:frame] autorelease];
	[field setBezeled:YES];
	[field setEditable:NO];
	[field setSelectable:YES];
	return field;
}

static NSComboBox *CreateAutocompleteComboBox(NSRect frame)
{
	NSComboBox *comboBox = [[[NSComboBox alloc] initWithFrame:frame] autorelease];
	[comboBox setUsesDataSource:NO];
	[comboBox setCompletes:YES];
	[comboBox setNumberOfVisibleItems:12];
	return comboBox;
}

static NSButton *CreateActionButton(NSRect frame, NSString *title, SEL action, id target)
{
	NSButton *button = [[[NSButton alloc] initWithFrame:frame] autorelease];
	[button setTitle:title];
	[button setButtonType:NSButtonTypeMomentaryPushIn];
	[button setBezelStyle:NSBezelStyleRounded];
	[button setTarget:target];
	[button setAction:action];
	return button;
}

static NSString *HiddenPowerTypeForEditorIVs(const uint8_t ivs[6])
{
	static const char *kHiddenPowerTypes[16] = {
		"Fighting", "Flying", "Poison", "Ground",
		"Rock",     "Bug",    "Ghost",  "Steel",
		"Fire",     "Water",  "Grass",  "Electric",
		"Psychic",  "Ice",    "Dragon", "Dark"
	};

	const int hpTypeX =
		(ivs[0] & 1) +
		((ivs[1] & 1) << 1) +
		((ivs[2] & 1) << 2) +
		((ivs[5] & 1) << 3) +
		((ivs[3] & 1) << 4) +
		((ivs[4] & 1) << 5);
	const int typeIndex = (hpTypeX * 15) / 63;
	return [NSString stringWithUTF8String:kHiddenPowerTypes[typeIndex]];
}

static NSString *PokemonPartyDisplayNameForSummary(const desmume_pokemon_http_export::PartyPokemonSummary &summary)
{
	if (summary.hasNickname && !summary.nickname.empty())
	{
		return [NSString stringWithUTF8String:summary.nickname.c_str()];
	}

	return [NSString stringWithUTF8String:summary.species.c_str()];
}

static bool PokemonPartySummariesMatchForApplyPreflight(
	const desmume_pokemon_http_export::PartyPokemonSummary &expected,
	const desmume_pokemon_http_export::PartyPokemonSummary &actual)
{
	if (expected.slot != actual.slot ||
	    expected.monOffset != actual.monOffset ||
	    expected.speciesId != actual.speciesId ||
	    expected.heldItemId != actual.heldItemId ||
	    expected.abilityId != actual.abilityId ||
	    expected.abilitySlot != actual.abilitySlot ||
	    expected.friendship != actual.friendship ||
	    expected.level != actual.level ||
	    expected.experience != actual.experience ||
	    expected.currentHp != actual.currentHp ||
	    expected.maxHp != actual.maxHp ||
	    expected.statusRaw != actual.statusRaw ||
	    expected.encodedStatusRaw != actual.encodedStatusRaw)
	{
		return false;
	}

	for (int i = 0; i < 6; ++i)
	{
		if (expected.stats.ivs[i] != actual.stats.ivs[i] ||
		    expected.stats.evs[i] != actual.stats.evs[i])
		{
			return false;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		if (expected.moves[i].id != actual.moves[i].id)
		{
			return false;
		}
	}

	return true;
}

@interface PokemonPartyEditorWindowController : NSWindowController <NSWindowDelegate, NSTextFieldDelegate>
{
	NSTextField *_statusTextField;
	NSPopUpButton *_sourcePopUpButton;
	NSPopUpButton *_boxPopUpButton;
	NSPopUpButton *_slotPopUpButton;
	NSTextField *_summaryTextField;
	NSComboBox *_speciesComboBox;
	NSComboBox *_heldItemComboBox;
	NSComboBox *_abilityComboBox;
	NSTextField *_friendshipTextField;
	NSTextField *_levelTextField;
	NSTextField *_hpTextField;
	NSTextField *_expTextField;
	NSTextField *_hiddenPowerTypeTextField;
	NSTextField *_abilitySlotTextField;
	NSPopUpButton *_statusPopUpButton;
	NSTextField *_ivTextFields[6];
	NSTextField *_evTextFields[6];
	NSComboBox *_moveComboBoxes[4];
	NSButton *_applyButton;
	NSButton *_edgeButton;
	NSButton *_bedtimeButton;
	NSButton *_edgePartyButton;
	NSButton *_refreshButton;
	NSButton *_debugSnapshotButton;
	NSButton *_dumpFullPartyRawButton;
	NSTimer *_pendingPartyApplyTimer;
	BOOL _pendingPartyApplyActive;
	int _pendingPartyApplyAttempts;
	int _pendingPartyApplySlot;
	PokemonPendingPartyWriteOperation _pendingPartyWriteOperation;
	desmume_pokemon_http_export::PartyPokemonEditRequest _pendingPartyApplyRequest;
	desmume_pokemon_http_export::PartyPokemonHpStatusEditRequest _pendingPartyHpStatusRequest;
	desmume_pokemon_http_export::PartyEditorSnapshot _partySnapshot;
	desmume_pokemon_http_export::BoxEditorSnapshot _boxSnapshot;
}

- (void)reloadEditorData;
@end

@implementation PokemonPartyEditorWindowController

- (id)init
{
	NSWindow *window = [[[NSWindow alloc] initWithContentRect:NSMakeRect(120.0, 120.0, 920.0, 580.0)
	                                                styleMask:(NSWindowStyleMaskTitled |
	                                                           NSWindowStyleMaskClosable |
	                                                           NSWindowStyleMaskMiniaturizable)
	                                                  backing:NSBackingStoreBuffered
	                                                    defer:NO] autorelease];
	self = [super initWithWindow:window];
	if (self == nil)
	{
		return nil;
	}

	[[self window] setTitle:@"Party/Box Editor"];
	[[self window] setDelegate:self];
	[[self window] setReleasedWhenClosed:NO];

	NSView *contentView = [window contentView];
	[contentView addSubview:CreateStaticLabel(NSMakeRect(20.0, 524.0, 880.0, 20.0), @"This editor works best while the player is in the in-game party menu.")];
	_statusTextField = [CreateStaticLabel(NSMakeRect(20.0, 544.0, 880.0, 20.0), @"No ROM loaded") retain];
	[contentView addSubview:_statusTextField];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(20.0, 488.0, 36.0, 20.0), @"View")];
	_sourcePopUpButton = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(60.0, 484.0, 140.0, 26.0) pullsDown:NO];
	[_sourcePopUpButton addItemWithTitle:@"Party"];
	[_sourcePopUpButton addItemWithTitle:@"Box"];
	[_sourcePopUpButton setTarget:self];
	[_sourcePopUpButton setAction:@selector(sourceSelectionChanged:)];
	[contentView addSubview:_sourcePopUpButton];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(220.0, 488.0, 28.0, 20.0), @"Box")];
	_boxPopUpButton = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(252.0, 484.0, 120.0, 26.0) pullsDown:NO];
	for (NSInteger box = 1; box <= 14; ++box)
	{
		[_boxPopUpButton addItemWithTitle:[NSString stringWithFormat:@"Box %ld", (long)box]];
		[[_boxPopUpButton lastItem] setTag:box];
	}
	[_boxPopUpButton setTarget:self];
	[_boxPopUpButton setAction:@selector(boxSelectionChanged:)];
	[contentView addSubview:_boxPopUpButton];

	_refreshButton = [CreateActionButton(NSMakeRect(796.0, 484.0, 104.0, 26.0), @"Refresh", @selector(refreshButtonPressed:), self) retain];
	[_refreshButton setKeyEquivalent:@"r"];
	[contentView addSubview:_refreshButton];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(20.0, 452.0, 36.0, 20.0), @"Slot")];
	_slotPopUpButton = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(60.0, 448.0, 840.0, 26.0) pullsDown:NO];
	[_slotPopUpButton setTarget:self];
	[_slotPopUpButton setAction:@selector(slotSelectionChanged:)];
	[contentView addSubview:_slotPopUpButton];

	_summaryTextField = [CreateStaticLabel(NSMakeRect(20.0, 416.0, 880.0, 20.0), @"") retain];
	[contentView addSubview:_summaryTextField];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(20.0, 380.0, 100.0, 20.0), @"Species")];
	_speciesComboBox = [CreateAutocompleteComboBox(NSMakeRect(124.0, 376.0, 220.0, 26.0)) retain];
	[contentView addSubview:_speciesComboBox];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(360.0, 380.0, 100.0, 20.0), @"Ability")];
	_abilityComboBox = [CreateAutocompleteComboBox(NSMakeRect(424.0, 376.0, 220.0, 26.0)) retain];
	[contentView addSubview:_abilityComboBox];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(660.0, 380.0, 100.0, 20.0), @"Held Item")];
	_heldItemComboBox = [CreateAutocompleteComboBox(NSMakeRect(724.0, 376.0, 176.0, 26.0)) retain];
	[contentView addSubview:_heldItemComboBox];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(20.0, 344.0, 100.0, 20.0), @"Friendship")];
	_friendshipTextField = [CreateEditableField(NSMakeRect(124.0, 340.0, 80.0, 24.0)) retain];
	[contentView addSubview:_friendshipTextField];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(220.0, 344.0, 100.0, 20.0), @"Level")];
	_levelTextField = [CreateEditableField(NSMakeRect(284.0, 340.0, 80.0, 24.0)) retain];
	[_levelTextField setTarget:self];
	[_levelTextField setAction:@selector(levelFieldChanged:)];
	[contentView addSubview:_levelTextField];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(380.0, 344.0, 100.0, 20.0), @"Current HP")];
	_hpTextField = [CreateEditableField(NSMakeRect(444.0, 340.0, 92.0, 24.0)) retain];
	[contentView addSubview:_hpTextField];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(552.0, 344.0, 100.0, 20.0), @"Experience")];
	_expTextField = [CreateEditableField(NSMakeRect(620.0, 340.0, 100.0, 24.0)) retain];
	[contentView addSubview:_expTextField];

	[contentView addSubview:CreateStaticLabel(NSMakeRect(736.0, 344.0, 100.0, 20.0), @"Status")];
	_statusPopUpButton = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(780.0, 340.0, 120.0, 26.0) pullsDown:NO];
	for (size_t i = 0; i < (sizeof(kPokemonPartyStatusMenuEntries) / sizeof(kPokemonPartyStatusMenuEntries[0])); ++i)
	{
		[_statusPopUpButton addItemWithTitle:[NSString stringWithUTF8String:kPokemonPartyStatusMenuEntries[i].label]];
		[[_statusPopUpButton lastItem] setTag:(NSInteger)kPokemonPartyStatusMenuEntries[i].raw];
	}
	[contentView addSubview:_statusPopUpButton];

	static const char *kStatLabels[6] = {"HP", "Atk", "Def", "SpA", "SpD", "Spe"};
	[contentView addSubview:CreateStaticLabel(NSMakeRect(20.0, 308.0, 32.0, 20.0), @"IVs")];
	[contentView addSubview:CreateStaticLabel(NSMakeRect(20.0, 272.0, 32.0, 20.0), @"EVs")];
	for (int i = 0; i < 6; ++i)
	{
		const CGFloat columnX = 60.0 + (i * 140.0);
		[contentView addSubview:CreateStaticLabel(NSMakeRect(columnX, 308.0, 34.0, 20.0), [NSString stringWithUTF8String:kStatLabels[i]])];
		_ivTextFields[i] = [CreateEditableField(NSMakeRect(columnX + 36.0, 304.0, 56.0, 24.0)) retain];
		[_ivTextFields[i] setDelegate:self];
		[contentView addSubview:_ivTextFields[i]];
		[contentView addSubview:CreateStaticLabel(NSMakeRect(columnX, 272.0, 34.0, 20.0), [NSString stringWithUTF8String:kStatLabels[i]])];
		_evTextFields[i] = [CreateEditableField(NSMakeRect(columnX + 36.0, 268.0, 56.0, 24.0)) retain];
		[contentView addSubview:_evTextFields[i]];
	}

	[contentView addSubview:CreateStaticLabel(NSMakeRect(20.0, 236.0, 100.0, 20.0), @"HP TYPE")];
	_hiddenPowerTypeTextField = [CreateReadOnlyField(NSMakeRect(124.0, 232.0, 140.0, 24.0)) retain];
	[contentView addSubview:_hiddenPowerTypeTextField];
	[contentView addSubview:CreateStaticLabel(NSMakeRect(300.0, 236.0, 100.0, 20.0), @"Ability Slot")];
	_abilitySlotTextField = [CreateReadOnlyField(NSMakeRect(404.0, 232.0, 80.0, 24.0)) retain];
	[contentView addSubview:_abilitySlotTextField];

	static const CGFloat moveLabelX = 20.0;
	static const CGFloat moveFieldX = 124.0;
	static const CGFloat moveFieldWidth = 776.0;
	static const CGFloat moveStartY = 198.0;
	static const CGFloat moveStepY = 30.0;
	for (int i = 0; i < 4; ++i)
	{
		NSString *labelText = [NSString stringWithFormat:@"Move %d", i + 1];
		[contentView addSubview:CreateStaticLabel(NSMakeRect(moveLabelX, moveStartY - (i * moveStepY), 100.0, 20.0), labelText)];
		_moveComboBoxes[i] = [CreateAutocompleteComboBox(NSMakeRect(moveFieldX, (moveStartY - 4.0) - (i * moveStepY), moveFieldWidth, 26.0)) retain];
		[contentView addSubview:_moveComboBoxes[i]];
	}

	_bedtimeButton = [CreateActionButton(NSMakeRect(20.0, 20.0, 120.0, 30.0), @"Bedtime", @selector(bedtimeButtonPressed:), self) retain];
	[contentView addSubview:_bedtimeButton];

	_edgePartyButton = [CreateActionButton(NSMakeRect(152.0, 20.0, 120.0, 30.0), @"Edge Party", @selector(edgePartyButtonPressed:), self) retain];
	[contentView addSubview:_edgePartyButton];

	_debugSnapshotButton = [CreateActionButton(NSMakeRect(288.0, 20.0, 128.0, 30.0), @"Debug Snapshot", @selector(debugSnapshotButtonPressed:), self) retain];
	[contentView addSubview:_debugSnapshotButton];

	_dumpFullPartyRawButton = [CreateActionButton(NSMakeRect(432.0, 20.0, 140.0, 30.0), @"Dump Party Raw", @selector(dumpFullPartyRawButtonPressed:), self) retain];
	[contentView addSubview:_dumpFullPartyRawButton];

	_applyButton = [CreateActionButton(NSMakeRect(588.0, 20.0, 100.0, 30.0), @"Apply", @selector(applyButtonPressed:), self) retain];
	[contentView addSubview:_applyButton];

	_edgeButton = [CreateActionButton(NSMakeRect(704.0, 20.0, 116.0, 30.0), @"Edge (-1 XP)", @selector(edgeButtonPressed:), self) retain];
	[_edgeButton setKeyEquivalent:@"e"];
	[contentView addSubview:_edgeButton];

	_partySnapshot.success = false;
	_boxSnapshot.success = false;
	_boxSnapshot.currentBox = 1;
	_boxSnapshot.boxCount = 14;
	_pendingPartyApplyTimer = nil;
	_pendingPartyApplyActive = NO;
	_pendingPartyApplyAttempts = 0;
	_pendingPartyApplySlot = 0;
	_pendingPartyWriteOperation = PokemonPendingPartyWriteNone;
	[self refreshControlStates];
	return self;
}

- (void)dealloc
{
	[_statusTextField release];
	[_sourcePopUpButton release];
	[_boxPopUpButton release];
	[_slotPopUpButton release];
	[_summaryTextField release];
	[_speciesComboBox release];
	[_heldItemComboBox release];
	[_abilityComboBox release];
	[_friendshipTextField release];
	[_levelTextField release];
	[_hpTextField release];
	[_expTextField release];
	[_hiddenPowerTypeTextField release];
	[_abilitySlotTextField release];
	[_statusPopUpButton release];
	for (int i = 0; i < 6; ++i)
	{
		[_ivTextFields[i] release];
		[_evTextFields[i] release];
	}
	for (int i = 0; i < 4; ++i)
	{
		[_moveComboBoxes[i] release];
	}
	[_applyButton release];
	[_edgeButton release];
	[_bedtimeButton release];
	[_edgePartyButton release];
	[_refreshButton release];
	[_debugSnapshotButton release];
	[_dumpFullPartyRawButton release];
	[_pendingPartyApplyTimer invalidate];
	[_pendingPartyApplyTimer release];
	[super dealloc];
}

- (void)populateComboBox:(NSComboBox *)comboBox withOptions:(const std::vector<std::string> &)options preservingStringValue:(NSString *)value
{
	NSString *preservedValue = (value != nil) ? [[value copy] autorelease] : @"";
	[comboBox removeAllItems];
	for (size_t i = 0; i < options.size(); ++i)
	{
		[comboBox addItemWithObjectValue:[NSString stringWithUTF8String:options[i].c_str()]];
	}
	[comboBox setStringValue:preservedValue];
}

- (void)reloadAutocompleteOptions
{
	[self populateComboBox:_speciesComboBox
	            withOptions:desmume_pokemon_http_export::GetSpeciesNameOptions()
	       preservingStringValue:[_speciesComboBox stringValue]];
	[self populateComboBox:_heldItemComboBox
	            withOptions:desmume_pokemon_http_export::GetItemNameOptions()
	       preservingStringValue:[_heldItemComboBox stringValue]];
	[self populateComboBox:_abilityComboBox
	            withOptions:desmume_pokemon_http_export::GetAbilityNameOptions()
	       preservingStringValue:[_abilityComboBox stringValue]];
	const std::vector<std::string> moveOptions = desmume_pokemon_http_export::GetMoveNameOptions();
	for (int i = 0; i < 4; ++i)
	{
		[self populateComboBox:_moveComboBoxes[i] withOptions:moveOptions preservingStringValue:[_moveComboBoxes[i] stringValue]];
	}
}

- (void)showWindow:(id)sender
{
	[super showWindow:sender];
	[[self window] makeKeyAndOrderFront:sender];
	[self reloadEditorData];
}

- (void)windowWillClose:(NSNotification *)notification
{
	(void)notification;
}

- (BOOL)isBoxMode
{
	return ([_sourcePopUpButton indexOfSelectedItem] == 1);
}

- (NSInteger)selectedPartySlotNumber
{
	NSMenuItem *item = [_slotPopUpButton selectedItem];
	return (item != nil) ? [item tag] : -1;
}

- (NSInteger)selectedPartyIndex
{
	const NSInteger selectedSlot = [self selectedPartySlotNumber];
	if (selectedSlot < 1)
	{
		return -1;
	}

	for (size_t i = 0; i < _partySnapshot.party.size(); ++i)
	{
		if (_partySnapshot.party[i].slot == selectedSlot)
		{
			return (NSInteger)i;
		}
	}
	return -1;
}

- (NSInteger)selectedBoxNumber
{
	NSMenuItem *item = [_boxPopUpButton selectedItem];
	return (item != nil) ? [item tag] : -1;
}

- (NSInteger)selectedBoxSlotNumber
{
	NSMenuItem *item = [_slotPopUpButton selectedItem];
	return (item != nil) ? [item tag] : -1;
}

- (NSInteger)selectedBoxIndex
{
	const NSInteger selectedBox = [self selectedBoxNumber];
	const NSInteger selectedSlot = [self selectedBoxSlotNumber];
	if (selectedBox < 1 || selectedSlot < 1)
	{
		return -1;
	}

	for (size_t i = 0; i < _boxSnapshot.pokemon.size(); ++i)
	{
		const desmume_pokemon_http_export::BoxPokemonSummary &summary = _boxSnapshot.pokemon[i];
		if (summary.box == selectedBox && summary.slot == selectedSlot)
		{
			return (NSInteger)i;
		}
	}
	return -1;
}

- (void)updateHiddenPowerTypeField
{
	const BOOL boxMode = [self isBoxMode];
	const NSInteger selectedIndex = boxMode ? [self selectedBoxIndex] : [self selectedPartyIndex];
	if (selectedIndex < 0)
	{
		[_hiddenPowerTypeTextField setStringValue:@""];
		return;
	}

	uint8_t ivs[6] = {};
	for (int i = 0; i < 6; ++i)
	{
		int iv = [_ivTextFields[i] intValue];
		if (iv < 0)
		{
			iv = 0;
		}
		else if (iv > 31)
		{
			iv = 31;
		}
		ivs[i] = static_cast<uint8_t>(iv);
	}

	[_hiddenPowerTypeTextField setStringValue:HiddenPowerTypeForEditorIVs(ivs)];
}

- (void)refreshControlStates
{
	const BOOL boxMode = [self isBoxMode];
	const BOOL hasPartySelection = ([self selectedPartyIndex] >= 0);
	const BOOL hasBoxSelection = ([self selectedBoxIndex] >= 0);
	const BOOL hasSelection = boxMode ? hasBoxSelection : hasPartySelection;
	const BOOL partyWritePending = _pendingPartyApplyActive;

	[_sourcePopUpButton setEnabled:YES];
	[_boxPopUpButton setEnabled:boxMode];
	[_slotPopUpButton setEnabled:hasSelection];
	[_levelTextField setEnabled:hasSelection && boxMode];
	[_expTextField setEnabled:hasSelection && boxMode];
	[_hpTextField setEnabled:hasSelection && !boxMode];
	[_statusPopUpButton setEnabled:hasSelection && !boxMode];
	[_speciesComboBox setEnabled:hasSelection && boxMode];
	[_heldItemComboBox setEnabled:hasSelection];
	[_abilityComboBox setEnabled:hasSelection];
	[_friendshipTextField setEnabled:hasSelection];
	for (int i = 0; i < 6; ++i)
	{
		[_ivTextFields[i] setEnabled:(hasSelection && boxMode)];
		[_evTextFields[i] setEnabled:(hasSelection && boxMode)];
	}
	for (int i = 0; i < 4; ++i)
	{
		[_moveComboBoxes[i] setEnabled:hasSelection];
	}
	[_applyButton setEnabled:(hasSelection && !partyWritePending)];
	[_edgeButton setEnabled:(hasSelection && (!partyWritePending || boxMode))];
	[_debugSnapshotButton setEnabled:hasSelection];
	[_dumpFullPartyRawButton setEnabled:YES];
	[_bedtimeButton setEnabled:(!boxMode && !_partySnapshot.party.empty() && !partyWritePending)];
	[_edgePartyButton setEnabled:(!boxMode && !_partySnapshot.party.empty() && !partyWritePending)];
}

- (NSInteger)currentBoxCount
{
	if (_boxSnapshot.boxCount > 0)
	{
		return _boxSnapshot.boxCount;
	}

	const NSInteger count = [_boxPopUpButton numberOfItems];
	return (count > 0) ? count : 1;
}

- (NSString *)currentProfileDisplayName
{
	const char *profileName = desmume_pokemon_http_export::GetProfileDisplayName(desmume_pokemon_http_export::GetSelectedProfile());
	if (profileName == NULL || profileName[0] == '\0')
	{
		return @"Pokemon";
	}

	return [NSString stringWithUTF8String:profileName];
}

- (void)rebuildBoxMenuPreservingSelection:(NSInteger)boxNumber
{
	const NSInteger boxCount = [self currentBoxCount];
	[_boxPopUpButton removeAllItems];

	for (NSInteger box = 1; box <= boxCount; ++box)
	{
		[_boxPopUpButton addItemWithTitle:[NSString stringWithFormat:@"Box %ld", (long)box]];
		[[_boxPopUpButton lastItem] setTag:box];
	}

	if (boxNumber < 1 || boxNumber > boxCount)
	{
		boxNumber = (_boxSnapshot.currentBox >= 1 && _boxSnapshot.currentBox <= boxCount) ? _boxSnapshot.currentBox : 1;
	}

	[_boxPopUpButton selectItemWithTag:boxNumber];
}

- (void)selectStatusRaw:(uint32_t)statusRaw
{
	for (NSInteger i = 0; i < [_statusPopUpButton numberOfItems]; ++i)
	{
		if ([[_statusPopUpButton itemAtIndex:i] tag] == (NSInteger)statusRaw)
		{
			[_statusPopUpButton selectItemAtIndex:i];
			return;
		}
	}
	[_statusPopUpButton selectItemAtIndex:0];
}

- (void)rebuildSelectionMenusPreservingPartySlot:(NSInteger)partySlot box:(NSInteger)boxNumber slot:(NSInteger)boxSlot
{
	[_slotPopUpButton removeAllItems];

	if ([self isBoxMode])
	{
		const NSInteger boxCount = [self currentBoxCount];
		if (boxNumber < 1 || boxNumber > boxCount)
		{
			boxNumber = (_boxSnapshot.currentBox >= 1 && _boxSnapshot.currentBox <= boxCount) ? _boxSnapshot.currentBox : 1;
		}
		[self rebuildBoxMenuPreservingSelection:boxNumber];

		for (size_t i = 0; i < _boxSnapshot.pokemon.size(); ++i)
		{
			const desmume_pokemon_http_export::BoxPokemonSummary &summary = _boxSnapshot.pokemon[i];
			if (summary.box != boxNumber)
			{
				continue;
			}

			NSString *title = [NSString stringWithFormat:@"Slot %d - %s Lv%d",
				summary.slot,
				summary.species.c_str(),
				summary.level];
			[_slotPopUpButton addItemWithTitle:title];
			[[_slotPopUpButton lastItem] setTag:summary.slot];
		}

		if ([_slotPopUpButton numberOfItems] > 0)
		{
			if (boxSlot >= 1 && [_slotPopUpButton indexOfItemWithTag:boxSlot] >= 0)
			{
				[_slotPopUpButton selectItemWithTag:boxSlot];
			}
			else
			{
				[_slotPopUpButton selectItemAtIndex:0];
			}
		}
	}
	else
	{
		for (size_t i = 0; i < _partySnapshot.party.size(); ++i)
		{
			const desmume_pokemon_http_export::PartyPokemonSummary &summary = _partySnapshot.party[i];
			NSString *title = [NSString stringWithFormat:@"Slot %d - %s Lv%d HP %d/%d",
				summary.slot,
				summary.species.c_str(),
				summary.level,
				summary.currentHp,
				summary.maxHp];
			[_slotPopUpButton addItemWithTitle:title];
			[[_slotPopUpButton lastItem] setTag:summary.slot];
		}

		if ([_slotPopUpButton numberOfItems] > 0)
		{
			if (partySlot >= 1 && [_slotPopUpButton indexOfItemWithTag:partySlot] >= 0)
			{
				[_slotPopUpButton selectItemWithTag:partySlot];
			}
			else
			{
				[_slotPopUpButton selectItemAtIndex:0];
			}
		}
	}
}

- (void)updateDetailFields
{
	const BOOL boxMode = [self isBoxMode];
	const NSInteger selectedIndex = boxMode ? [self selectedBoxIndex] : [self selectedPartyIndex];
	if (selectedIndex < 0)
	{
		[_summaryTextField setStringValue:@""];
		[_levelTextField setStringValue:@""];
		[_hpTextField setStringValue:boxMode ? @"N/A" : @""];
		[_expTextField setStringValue:@""];
		[_speciesComboBox setStringValue:@""];
		[_heldItemComboBox setStringValue:@""];
		[_abilityComboBox setStringValue:@""];
		[_friendshipTextField setStringValue:@""];
		[_abilitySlotTextField setStringValue:@""];
		[self selectStatusRaw:0];
		for (int i = 0; i < 6; ++i)
		{
			[_ivTextFields[i] setStringValue:@""];
			[_evTextFields[i] setStringValue:@""];
		}
		for (int i = 0; i < 4; ++i)
		{
			[_moveComboBoxes[i] setStringValue:@""];
		}
		[self updateHiddenPowerTypeField];
		[self refreshControlStates];
		return;
	}

	if (boxMode)
	{
		const desmume_pokemon_http_export::BoxPokemonSummary &summary = _boxSnapshot.pokemon[(size_t)selectedIndex];
		NSString *displayName = [NSString stringWithUTF8String:summary.species.c_str()];
		if (summary.hasNickname && !summary.nickname.empty())
		{
			displayName = [NSString stringWithFormat:@"%s (%s)", summary.nickname.c_str(), summary.species.c_str()];
		}
		[_summaryTextField setStringValue:[NSString stringWithFormat:@"Box %d Slot %d  Level %d  %@",
			summary.box,
			summary.slot,
			summary.level,
			displayName]];
		[_levelTextField setStringValue:[NSString stringWithFormat:@"%d", summary.level]];
		[_hpTextField setStringValue:@"N/A"];
		[_expTextField setStringValue:[NSString stringWithFormat:@"%u", summary.experience]];
		[_speciesComboBox setStringValue:[NSString stringWithUTF8String:summary.species.c_str()]];
		[_heldItemComboBox setStringValue:[NSString stringWithUTF8String:summary.heldItem.c_str()]];
		[_abilityComboBox setStringValue:[NSString stringWithUTF8String:summary.ability.c_str()]];
		[_friendshipTextField setStringValue:[NSString stringWithFormat:@"%u", summary.friendship]];
		[_abilitySlotTextField setStringValue:(summary.abilitySlot == 0u) ? @"" : [NSString stringWithFormat:@"%u", summary.abilitySlot]];
		[self selectStatusRaw:0];
		for (int i = 0; i < 6; ++i)
		{
			[_ivTextFields[i] setStringValue:[NSString stringWithFormat:@"%u", summary.stats.ivs[i]]];
			[_evTextFields[i] setStringValue:[NSString stringWithFormat:@"%u", summary.stats.evs[i]]];
		}
		for (int i = 0; i < 4; ++i)
		{
			[_moveComboBoxes[i] setStringValue:[NSString stringWithUTF8String:summary.moves[i].name.c_str()]];
		}
		[self updateHiddenPowerTypeField];
		[self refreshControlStates];
		return;
	}

	const desmume_pokemon_http_export::PartyPokemonSummary &summary = _partySnapshot.party[(size_t)selectedIndex];
	NSString *displayName = [NSString stringWithUTF8String:summary.species.c_str()];
	if (summary.hasNickname && !summary.nickname.empty())
	{
		displayName = [NSString stringWithFormat:@"%s (%s)", summary.nickname.c_str(), summary.species.c_str()];
	}
	[_summaryTextField setStringValue:[NSString stringWithFormat:@"Level %d  HP %d/%d  Status: %s  %@",
		summary.level,
		summary.currentHp,
		summary.maxHp,
		summary.statusLabel.c_str(),
		displayName]];
	[_levelTextField setStringValue:[NSString stringWithFormat:@"%d", summary.level]];
	[_hpTextField setStringValue:[NSString stringWithFormat:@"%d", summary.currentHp]];
	[_expTextField setStringValue:[NSString stringWithFormat:@"%u", summary.experience]];
	[_speciesComboBox setStringValue:[NSString stringWithUTF8String:summary.species.c_str()]];
	[_heldItemComboBox setStringValue:[NSString stringWithUTF8String:summary.heldItem.c_str()]];
	[_abilityComboBox setStringValue:[NSString stringWithUTF8String:summary.ability.c_str()]];
	[_friendshipTextField setStringValue:[NSString stringWithFormat:@"%u", summary.friendship]];
	[_abilitySlotTextField setStringValue:(summary.abilitySlot == 0u) ? @"" : [NSString stringWithFormat:@"%u", summary.abilitySlot]];
	[self selectStatusRaw:summary.statusRaw];
	for (int i = 0; i < 6; ++i)
	{
		[_ivTextFields[i] setStringValue:[NSString stringWithFormat:@"%u", summary.stats.ivs[i]]];
		[_evTextFields[i] setStringValue:[NSString stringWithFormat:@"%u", summary.stats.evs[i]]];
	}
	for (int i = 0; i < 4; ++i)
	{
		[_moveComboBoxes[i] setStringValue:[NSString stringWithUTF8String:summary.moves[i].name.c_str()]];
	}
	[self updateHiddenPowerTypeField];
	[self refreshControlStates];
}

- (BOOL)validatePartyApplySnapshotForSummary:(const desmume_pokemon_http_export::PartyPokemonSummary &)summary
{
	desmume_pokemon_http_export::PartyEditorSnapshot currentSnapshot;
	if (!desmume_pokemon_http_export::GetPartyEditorSnapshot(currentSnapshot))
	{
		return NO;
	}

	for (size_t i = 0; i < currentSnapshot.party.size(); ++i)
	{
		const desmume_pokemon_http_export::PartyPokemonSummary &currentSummary = currentSnapshot.party[i];
		if (currentSummary.slot == summary.slot)
		{
			return PokemonPartySummariesMatchForApplyPreflight(summary, currentSummary) ? YES : NO;
		}
	}

	return NO;
}

- (BOOL)visiblePartyPreservedFieldsMatchSummary:(const desmume_pokemon_http_export::PartyPokemonSummary &)summary
                                      speciesId:(uint16_t)speciesId
                                            ivs:(const uint8_t *)ivValues
                                            evs:(const uint8_t *)evValues
{
	if (speciesId != summary.speciesId)
	{
		return NO;
	}
	if ([_levelTextField integerValue] != summary.level)
	{
		return NO;
	}

	const NSInteger visibleExperience = [_expTextField integerValue];
	if (visibleExperience < 0 || static_cast<uint32_t>(visibleExperience) != summary.experience)
	{
		return NO;
	}

	for (int i = 0; i < 6; ++i)
	{
		if (ivValues[i] != summary.stats.ivs[i] || evValues[i] != summary.stats.evs[i])
		{
			return NO;
		}
	}

	return YES;
}

- (void)clearPendingPartyApply
{
	[_pendingPartyApplyTimer invalidate];
	[_pendingPartyApplyTimer release];
	_pendingPartyApplyTimer = nil;
	_pendingPartyApplyActive = NO;
	_pendingPartyApplyAttempts = 0;
	_pendingPartyApplySlot = 0;
	_pendingPartyWriteOperation = PokemonPendingPartyWriteNone;
	[self refreshControlStates];
}

- (void)queuePartyApplyRequest:(const desmume_pokemon_http_export::PartyPokemonEditRequest &)request
                          slot:(int)slot
{
	[self clearPendingPartyApply];
	_pendingPartyApplyRequest = request;
	_pendingPartyApplySlot = slot;
	_pendingPartyApplyAttempts = 0;
	_pendingPartyApplyActive = YES;
	_pendingPartyWriteOperation = PokemonPendingPartyWriteApply;
	[self refreshControlStates];
	[_statusTextField setStringValue:@"Apply queued, waiting for stable party data"];
	_pendingPartyApplyTimer = [[NSTimer scheduledTimerWithTimeInterval:PokemonPartyApplyQueueIntervalSeconds
	                                                            target:self
	                                                          selector:@selector(processPendingPartyApply:)
	                                                          userInfo:nil
	                                                           repeats:YES] retain];
	[self processPendingPartyApply:_pendingPartyApplyTimer];
}

- (void)queuePartyHpStatusRequest:(const desmume_pokemon_http_export::PartyPokemonHpStatusEditRequest &)request
                              slot:(int)slot
{
	[self clearPendingPartyApply];
	_pendingPartyHpStatusRequest = request;
	_pendingPartyApplySlot = slot;
	_pendingPartyApplyAttempts = 0;
	_pendingPartyApplyActive = YES;
	_pendingPartyWriteOperation = PokemonPendingPartyWriteHpStatus;
	[self refreshControlStates];
	[_statusTextField setStringValue:@"Apply queued, waiting for stable party data"];
	_pendingPartyApplyTimer = [[NSTimer scheduledTimerWithTimeInterval:PokemonPartyApplyQueueIntervalSeconds
	                                                            target:self
	                                                          selector:@selector(processPendingPartyApply:)
	                                                          userInfo:nil
	                                                           repeats:YES] retain];
	[self processPendingPartyApply:_pendingPartyApplyTimer];
}

- (void)queuePendingPartyWriteOperation:(PokemonPendingPartyWriteOperation)operation
                                   slot:(int)slot
                            statusText:(NSString *)statusText
{
	[self clearPendingPartyApply];
	_pendingPartyApplySlot = slot;
	_pendingPartyApplyAttempts = 0;
	_pendingPartyApplyActive = YES;
	_pendingPartyWriteOperation = operation;
	[self refreshControlStates];
	[_statusTextField setStringValue:(statusText != nil) ? statusText : @"Waiting for stable party data"];
	_pendingPartyApplyTimer = [[NSTimer scheduledTimerWithTimeInterval:PokemonPartyApplyQueueIntervalSeconds
	                                                            target:self
	                                                          selector:@selector(processPendingPartyApply:)
	                                                          userInfo:nil
	                                                           repeats:YES] retain];
	[self processPendingPartyApply:_pendingPartyApplyTimer];
}

- (void)processPendingPartyApply:(NSTimer *)timer
{
	(void)timer;
	if (!_pendingPartyApplyActive)
	{
		return;
	}

	std::string readinessError;
	const BOOL requiresWholePartyReadiness =
		(_pendingPartyWriteOperation == PokemonPendingPartyWriteBedtime) ||
		(_pendingPartyWriteOperation == PokemonPendingPartyWriteEdgeParty);
	const bool ready = requiresWholePartyReadiness
		? desmume_pokemon_http_export::IsEntirePartyReadyForWrite(readinessError)
		: desmume_pokemon_http_export::IsPartyPokemonReadyForWrite(_pendingPartyApplySlot, readinessError);
	if (!ready)
	{
		++_pendingPartyApplyAttempts;
		if (_pendingPartyApplyAttempts >= PokemonPartyApplyQueueMaxAttempts)
		{
			[self clearPendingPartyApply];
			[self reportError:PokemonPartyApplyFailedTryAgainMessage];
		}
		return;
	}

	std::string error;
	NSString *successStatus = @"Party Pokemon updated";
	bool operationSucceeded = false;
	switch (_pendingPartyWriteOperation)
	{
		case PokemonPendingPartyWriteApply:
			successStatus = @"Party Pokemon updated";
			operationSucceeded = desmume_pokemon_http_export::UpdatePartyPokemon(_pendingPartyApplyRequest, error);
			break;
		case PokemonPendingPartyWriteHpStatus:
			successStatus = @"Party Pokemon HP/status updated";
			operationSucceeded = desmume_pokemon_http_export::UpdatePartyPokemonHpStatus(_pendingPartyHpStatusRequest, error);
			break;
		case PokemonPendingPartyWriteEdgeSlot:
			successStatus = @"Party Pokemon edged to one XP before next level";
			operationSucceeded = desmume_pokemon_http_export::EdgePartyPokemon(_pendingPartyApplySlot, 1u, error);
			break;
		case PokemonPendingPartyWriteBedtime:
			successStatus = @"Entire party set to 1-turn sleep";
			operationSucceeded = desmume_pokemon_http_export::SetEntirePartyStatus(1u, error);
			break;
		case PokemonPendingPartyWriteEdgeParty:
			successStatus = @"Entire party edged to one XP before the next level";
			operationSucceeded = desmume_pokemon_http_export::EdgeEntireParty(1u, error);
			break;
		case PokemonPendingPartyWriteNone:
		default:
			error = "No pending party write";
			break;
	}
	if (!operationSucceeded)
	{
		++_pendingPartyApplyAttempts;
		if (_pendingPartyApplyAttempts < PokemonPartyApplyQueueMaxAttempts)
		{
			return;
		}

		[self clearPendingPartyApply];
		[self reportError:error];
		return;
	}

	const int appliedSlot = _pendingPartyApplySlot;
	const PokemonPendingPartyWriteOperation completedOperation = _pendingPartyWriteOperation;
	[self clearPendingPartyApply];
	[_statusTextField setStringValue:successStatus];
	[self reloadEditorData];
	if (!_partySnapshot.success)
	{
		return;
	}
	if (completedOperation == PokemonPendingPartyWriteEdgeSlot ||
	    completedOperation == PokemonPendingPartyWriteApply ||
	    completedOperation == PokemonPendingPartyWriteHpStatus)
	{
		[self rebuildSelectionMenusPreservingPartySlot:appliedSlot box:[self selectedBoxNumber] slot:[self selectedBoxSlotNumber]];
		[self updateDetailFields];
	}
}

- (void)reloadEditorData
{
	const NSInteger previousPartySlot = [self selectedPartySlotNumber];
	NSInteger previousBox = [self selectedBoxNumber];
	const NSInteger previousBoxSlot = [self selectedBoxSlotNumber];
	[self reloadAutocompleteOptions];

	if ([self isBoxMode])
	{
		desmume_pokemon_http_export::BoxEditorSnapshot nextSnapshot;
		if (!desmume_pokemon_http_export::GetBoxEditorSnapshot(nextSnapshot))
		{
			_boxSnapshot = nextSnapshot;
			[_slotPopUpButton removeAllItems];
			[self rebuildBoxMenuPreservingSelection:previousBox];
			[_statusTextField setStringValue:[NSString stringWithUTF8String:nextSnapshot.error.c_str()]];
			[self updateDetailFields];
			return;
		}

		_boxSnapshot = nextSnapshot;
		const NSInteger boxCount = [self currentBoxCount];
		if (previousBox < 1 || previousBox > boxCount)
		{
			previousBox = nextSnapshot.currentBox;
		}
		[_statusTextField setStringValue:[NSString stringWithFormat:@"%@ box ready", [self currentProfileDisplayName]]];
		[self rebuildSelectionMenusPreservingPartySlot:previousPartySlot box:previousBox slot:previousBoxSlot];
		if ([_slotPopUpButton numberOfItems] == 0)
		{
			[_statusTextField setStringValue:[NSString stringWithFormat:@"Box %ld is empty", (long)[self selectedBoxNumber]]];
		}
		[self updateDetailFields];
		return;
	}

	desmume_pokemon_http_export::PartyEditorSnapshot nextSnapshot;
	if (!desmume_pokemon_http_export::GetPartyEditorSnapshot(nextSnapshot))
	{
		desmume_pokemon_http_export::PartyEditorSnapshot retrySnapshot;
		if (desmume_pokemon_http_export::GetPartyEditorSnapshot(retrySnapshot))
		{
			nextSnapshot = retrySnapshot;
		}
		else
		{
		_partySnapshot = nextSnapshot;
		[_slotPopUpButton removeAllItems];
		[_statusTextField setStringValue:[NSString stringWithUTF8String:nextSnapshot.error.c_str()]];
		[self updateDetailFields];
		return;
		}
	}

	_partySnapshot = nextSnapshot;
	[_statusTextField setStringValue:[NSString stringWithFormat:@"%@ party ready", [self currentProfileDisplayName]]];
	[self rebuildSelectionMenusPreservingPartySlot:previousPartySlot box:previousBox slot:previousBoxSlot];
	if (_partySnapshot.party.empty())
	{
		[_statusTextField setStringValue:@"Party is empty"];
		[self updateDetailFields];
		return;
	}
	[self updateDetailFields];
}

- (void)reportError:(const std::string &)error
{
	[_statusTextField setStringValue:[NSString stringWithUTF8String:error.c_str()]];
	NSBeep();
}

- (void)slotSelectionChanged:(id)sender
{
	(void)sender;
	[self updateDetailFields];
}

- (void)sourceSelectionChanged:(id)sender
{
	(void)sender;
	[self reloadEditorData];
}

- (void)boxSelectionChanged:(id)sender
{
	(void)sender;
	if (![self isBoxMode] || !_boxSnapshot.success)
	{
		[self reloadEditorData];
		return;
	}

	[self rebuildSelectionMenusPreservingPartySlot:[self selectedPartySlotNumber] box:[self selectedBoxNumber] slot:[self selectedBoxSlotNumber]];
	[self updateDetailFields];
}

- (void)refreshButtonPressed:(id)sender
{
	(void)sender;
	[self reloadEditorData];
}

- (void)debugSnapshotButtonPressed:(id)sender
{
	(void)sender;
	const BOOL boxMode = [self isBoxMode];
	const NSInteger selectedIndex = boxMode ? [self selectedBoxIndex] : [self selectedPartyIndex];
	if (selectedIndex < 0)
	{
		return;
	}

	std::string fileName;
	std::string snapshotText;
	if (boxMode)
	{
		const desmume_pokemon_http_export::BoxPokemonSummary &summary = _boxSnapshot.pokemon[(size_t)selectedIndex];
		fileName = "debug_snapshot_box_" + std::to_string(summary.box) + "_slot_" + std::to_string(summary.slot);
		snapshotText = summary.debugSnapshot;
	}
	else
	{
		const desmume_pokemon_http_export::PartyPokemonSummary &summary = _partySnapshot.party[(size_t)selectedIndex];
		fileName = "debug_snapshot_party_slot_" + std::to_string(summary.slot);
		snapshotText = summary.debugSnapshot;
	}

	std::string pathOut;
	std::string error;
	if (!desmume_pokemon_http_export::WriteEditorDebugSnapshot(fileName, snapshotText, pathOut, error))
	{
		[self reportError:error];
		return;
	}

	[_statusTextField setStringValue:[NSString stringWithFormat:@"Debug snapshot written: %s", pathOut.c_str()]];
}

- (void)dumpFullPartyRawButtonPressed:(id)sender
{
	(void)sender;
	std::string pathOut;
	std::string error;
	if (!desmume_pokemon_http_export::WriteFullPartyRawBytesDebugSnapshot(pathOut, error))
	{
		[self reportError:error];
		return;
	}

	[_statusTextField setStringValue:[NSString stringWithFormat:@"Full party raw dump written: %s", pathOut.c_str()]];
}

- (void)levelFieldChanged:(id)sender
{
	(void)sender;
	const BOOL boxMode = [self isBoxMode];
	if (!boxMode)
	{
		return;
	}

	const NSInteger selectedIndex = boxMode ? [self selectedBoxIndex] : [self selectedPartyIndex];
	if (selectedIndex < 0)
	{
		return;
	}

	uint16_t speciesId = 0;
	if (boxMode)
	{
		speciesId = _boxSnapshot.pokemon[(size_t)selectedIndex].speciesId;
	}
	else
	{
		speciesId = _partySnapshot.party[(size_t)selectedIndex].speciesId;
	}

	int requestedLevel = [_levelTextField intValue];
	if (requestedLevel < 1)
	{
		requestedLevel = 1;
	}
	else if (requestedLevel > 100)
	{
		requestedLevel = 100;
	}

	uint32_t experience = 0;
	if (!desmume_pokemon_http_export::GetExperienceForSpeciesLevel(speciesId, requestedLevel, experience))
	{
		[self reportError:"Failed to resolve EXP for level"];
		return;
	}

	[_levelTextField setStringValue:[NSString stringWithFormat:@"%d", requestedLevel]];
	[_expTextField setStringValue:[NSString stringWithFormat:@"%u", experience]];
}

- (void)controlTextDidChange:(NSNotification *)notification
{
	id object = [notification object];
	for (int i = 0; i < 6; ++i)
	{
		if (object == _ivTextFields[i])
		{
			[self updateHiddenPowerTypeField];
			return;
		}
	}
}

- (void)applyButtonPressed:(id)sender
{
	(void)sender;
	const BOOL boxMode = [self isBoxMode];
	const NSInteger selectedIndex = boxMode ? [self selectedBoxIndex] : [self selectedPartyIndex];
	if (selectedIndex < 0)
	{
		return;
	}

	const NSInteger enteredLevel = [_levelTextField integerValue];
	int clampedLevel = (int)enteredLevel;
	if (clampedLevel < 1)
	{
		clampedLevel = 1;
	}
	else if (clampedLevel > 100)
	{
		clampedLevel = 100;
	}

	std::string canonicalName;
	uint16_t speciesId = 0;
	uint16_t heldItemId = 0;
	uint16_t abilityId = 0;
	const std::string speciesName = [[_speciesComboBox stringValue] UTF8String] ? [[_speciesComboBox stringValue] UTF8String] : "";
	const std::string heldItemName = [[_heldItemComboBox stringValue] UTF8String] ? [[_heldItemComboBox stringValue] UTF8String] : "";
	const std::string abilityName = [[_abilityComboBox stringValue] UTF8String] ? [[_abilityComboBox stringValue] UTF8String] : "";
	if (!desmume_pokemon_http_export::LookupSpeciesIdByName(speciesName, speciesId, canonicalName) || speciesId == 0u)
	{
		[self reportError:"Unknown species name"];
		return;
	}
	if (!desmume_pokemon_http_export::LookupItemIdByName(heldItemName, heldItemId, canonicalName))
	{
		[self reportError:"Unknown held item name"];
		return;
	}
	if (!desmume_pokemon_http_export::LookupAbilityIdByName(abilityName, abilityId, canonicalName) || abilityId == 0u)
	{
		[self reportError:"Unknown ability name"];
		return;
	}

	uint8_t friendship = static_cast<uint8_t>(std::max(0, std::min(255, (int)[_friendshipTextField integerValue])));
	uint8_t ivValues[6] = {};
	uint8_t evValues[6] = {};
	for (int i = 0; i < 6; ++i)
	{
		ivValues[i] = static_cast<uint8_t>(std::max(0, std::min(31, (int)[_ivTextFields[i] integerValue])));
		evValues[i] = static_cast<uint8_t>(std::max(0, std::min(255, (int)[_evTextFields[i] integerValue])));
	}

	uint16_t moveIds[4] = {};
	for (int i = 0; i < 4; ++i)
	{
		uint16_t moveId = 0;
		const std::string moveName = [[_moveComboBoxes[i] stringValue] UTF8String] ? [[_moveComboBoxes[i] stringValue] UTF8String] : "";
		if (!desmume_pokemon_http_export::LookupMoveIdByName(moveName, moveId, canonicalName))
		{
			[self reportError:("Unknown move name in slot " + std::to_string(i + 1))];
			return;
		}
		moveIds[i] = moveId;
	}

	std::string error;
	if (boxMode)
	{
		const desmume_pokemon_http_export::BoxPokemonSummary &summary = _boxSnapshot.pokemon[(size_t)selectedIndex];
		desmume_pokemon_http_export::BoxPokemonEditRequest request = {};
		request.box = summary.box;
		request.slot = summary.slot;
		request.speciesId = speciesId;
		request.heldItemId = heldItemId;
		request.abilityId = abilityId;
		request.friendship = friendship;
		for (int i = 0; i < 6; ++i)
		{
			request.ivs[i] = ivValues[i];
			request.evs[i] = evValues[i];
		}

		if (enteredLevel != summary.level || speciesId != summary.speciesId)
		{
			uint32_t adjustedExperience = 0;
			if (!desmume_pokemon_http_export::GetExperienceForSpeciesLevel(speciesId, clampedLevel, adjustedExperience))
			{
				[self reportError:"Failed to resolve EXP for requested level"];
				return;
			}

			request.experience = adjustedExperience;
			[_levelTextField setStringValue:[NSString stringWithFormat:@"%d", clampedLevel]];
			[_expTextField setStringValue:[NSString stringWithFormat:@"%u", adjustedExperience]];
		}
		else
		{
			request.experience = (uint32_t)[_expTextField integerValue];
		}

		for (int i = 0; i < 4; ++i)
		{
			request.moveIds[i] = moveIds[i];
		}

		if (!desmume_pokemon_http_export::UpdateBoxPokemon(request, error))
		{
			[self reportError:error];
			return;
		}

		[_statusTextField setStringValue:@"Box Pokemon updated"];
		[self reloadEditorData];
		[self rebuildSelectionMenusPreservingPartySlot:[self selectedPartySlotNumber] box:summary.box slot:summary.slot];
		[self updateDetailFields];
		return;
	}

	const desmume_pokemon_http_export::PartyPokemonSummary &summary = _partySnapshot.party[(size_t)selectedIndex];
	if (![self visiblePartyPreservedFieldsMatchSummary:summary speciesId:speciesId ivs:ivValues evs:evValues] ||
	    ![self validatePartyApplySnapshotForSummary:summary])
	{
		[self reportError:PokemonPartyApplyFailedTryAgainMessage];
		return;
	}

	desmume_pokemon_http_export::PartyPokemonEditRequest request = {};
	request.slot = summary.slot;
	request.currentHp = [_hpTextField intValue];
	request.statusRaw = (uint32_t)[[_statusPopUpButton selectedItem] tag];
	if (request.statusRaw == summary.statusRaw)
	{
		request.encodedStatusRaw = summary.encodedStatusRaw;
		request.preserveEncodedStatusRaw = true;
	}
	request.experience = summary.experience;
	request.speciesId = summary.speciesId;
	request.heldItemId = heldItemId;
	request.abilityId = abilityId;
	request.friendship = friendship;
	BOOL corePartyFieldsChanged =
		(heldItemId != summary.heldItemId) ||
		(abilityId != summary.abilityId) ||
		(friendship != summary.friendship);
	for (int i = 0; i < 6; ++i)
	{
		request.ivs[i] = summary.stats.ivs[i];
		request.evs[i] = summary.stats.evs[i];
	}
	[_levelTextField setStringValue:[NSString stringWithFormat:@"%d", summary.level]];
	[_expTextField setStringValue:[NSString stringWithFormat:@"%u", request.experience]];

	for (int i = 0; i < 4; ++i)
	{
		request.moveIds[i] = moveIds[i];
		if (moveIds[i] != summary.moves[i].id)
		{
			corePartyFieldsChanged = YES;
		}
	}

	if (!corePartyFieldsChanged)
	{
		desmume_pokemon_http_export::PartyPokemonHpStatusEditRequest hpStatusRequest = {};
		hpStatusRequest.slot = summary.slot;
		hpStatusRequest.currentHp = request.currentHp;
		hpStatusRequest.preserveCurrentHp = (request.currentHp == summary.currentHp) ? true : false;
		hpStatusRequest.statusRaw = request.statusRaw;
		if (request.preserveEncodedStatusRaw)
		{
			hpStatusRequest.encodedStatusRaw = request.encodedStatusRaw;
			hpStatusRequest.preserveEncodedStatusRaw = true;
		}
		[self queuePartyHpStatusRequest:hpStatusRequest slot:summary.slot];
		return;
	}

	[self queuePartyApplyRequest:request slot:summary.slot];
}

- (void)edgeButtonPressed:(id)sender
{
	(void)sender;
	const BOOL boxMode = [self isBoxMode];
	const NSInteger selectedIndex = boxMode ? [self selectedBoxIndex] : [self selectedPartyIndex];
	if (selectedIndex < 0)
	{
		return;
	}

	std::string error;
	if (boxMode)
	{
		const desmume_pokemon_http_export::BoxPokemonSummary &summary = _boxSnapshot.pokemon[(size_t)selectedIndex];
		if (!desmume_pokemon_http_export::EdgeBoxPokemon(summary.box, summary.slot, 1u, error))
		{
			[self reportError:error];
			return;
		}

		[_statusTextField setStringValue:@"Box Pokemon edged to one XP before next level"];
		[self reloadEditorData];
		[self rebuildSelectionMenusPreservingPartySlot:[self selectedPartySlotNumber] box:summary.box slot:summary.slot];
		[self updateDetailFields];
		return;
	}

	const int slot = _partySnapshot.party[(size_t)selectedIndex].slot;
	[self queuePendingPartyWriteOperation:PokemonPendingPartyWriteEdgeSlot
	                                 slot:slot
	                          statusText:@"Edge queued, waiting for stable party data"];
}

- (void)bedtimeButtonPressed:(id)sender
{
	(void)sender;
	if ([self isBoxMode])
	{
		return;
	}

	[self queuePendingPartyWriteOperation:PokemonPendingPartyWriteBedtime
	                                 slot:0
	                          statusText:@"Bedtime queued, waiting for stable party data"];
}

- (void)edgePartyButtonPressed:(id)sender
{
	(void)sender;
	if ([self isBoxMode])
	{
		return;
	}

	[self queuePendingPartyWriteOperation:PokemonPendingPartyWriteEdgeParty
	                                 slot:0
	                          statusText:@"Edge Party queued, waiting for stable party data"];
}

@end

@interface PokemonPartyStatusEditorWindowController : NSWindowController <NSWindowDelegate>
{
	NSTextField *_statusTextField;
	NSButton *_refreshButton;
	NSButton *_bedtimeButton;
	NSButton *_setAllButton;
	NSTextField *_nameTextFields[6];
	NSTextField *_hpLabelTextFields[6];
	NSTextField *_hpTextFields[6];
	NSButton *_statusButtons[6][7];
	desmume_pokemon_http_export::PartyEditorSnapshot _partySnapshot;
}

- (void)reloadEditorData;
@end

@implementation PokemonPartyStatusEditorWindowController

- (id)init
{
	NSWindow *window = [[[NSWindow alloc] initWithContentRect:NSMakeRect(120.0, 120.0, 1040.0, 376.0)
	                                                styleMask:(NSWindowStyleMaskTitled |
	                                                           NSWindowStyleMaskClosable |
	                                                           NSWindowStyleMaskMiniaturizable)
	                                                  backing:NSBackingStoreBuffered
	                                                    defer:NO] autorelease];
	self = [super initWithWindow:window];
	if (self == nil)
	{
		return nil;
	}

	[[self window] setTitle:@"Party Status/HP Editor"];
	[[self window] setDelegate:self];
	[[self window] setReleasedWhenClosed:NO];

	NSView *contentView = [window contentView];
	[contentView addSubview:CreateStaticLabel(NSMakeRect(20.0, 322.0, 880.0, 20.0), @"This editor works best while the player is in the in-game party menu.")];
	_statusTextField = [CreateStaticLabel(NSMakeRect(20.0, 342.0, 880.0, 20.0), @"No ROM loaded") retain];
	[contentView addSubview:_statusTextField];

	_refreshButton = [CreateActionButton(NSMakeRect(914.0, 338.0, 100.0, 26.0), @"Refresh", @selector(refreshButtonPressed:), self) retain];
	[contentView addSubview:_refreshButton];

	for (NSInteger row = 0; row < 6; ++row)
	{
		const CGFloat y = 270.0 - (row * 42.0);
		_nameTextFields[row] = [CreateStaticLabel(NSMakeRect(20.0, y + 4.0, 260.0, 20.0), @"") retain];
		[contentView addSubview:_nameTextFields[row]];

		[contentView addSubview:CreateStaticLabel(NSMakeRect(292.0, y + 4.0, 22.0, 20.0), @"HP")];
		_hpTextFields[row] = [CreateEditableField(NSMakeRect(318.0, y, 58.0, 24.0)) retain];
		[_hpTextFields[row] setTag:row];
		[_hpTextFields[row] setTarget:self];
		[_hpTextFields[row] setAction:@selector(hpFieldSubmitted:)];
		[contentView addSubview:_hpTextFields[row]];

		_hpLabelTextFields[row] = [CreateStaticLabel(NSMakeRect(382.0, y + 4.0, 54.0, 20.0), @"") retain];
		[contentView addSubview:_hpLabelTextFields[row]];

		for (NSInteger statusIndex = 0; statusIndex < 7; ++statusIndex)
		{
			_statusButtons[row][statusIndex] = [[NSButton alloc] initWithFrame:NSMakeRect(448.0 + (statusIndex * 82.0), y - 1.0, 78.0, 26.0)];
			[_statusButtons[row][statusIndex] setTitle:[NSString stringWithUTF8String:kPokemonPartyQuickStatusButtons[statusIndex].label]];
			[_statusButtons[row][statusIndex] setButtonType:NSButtonTypePushOnPushOff];
			[_statusButtons[row][statusIndex] setBezelStyle:NSBezelStyleRounded];
			[_statusButtons[row][statusIndex] setTarget:self];
			[_statusButtons[row][statusIndex] setAction:@selector(statusButtonPressed:)];
			[_statusButtons[row][statusIndex] setTag:(NSInteger)(row * 10 + statusIndex)];
			[contentView addSubview:_statusButtons[row][statusIndex]];
		}
	}

	_bedtimeButton = [CreateActionButton(NSMakeRect(20.0, 18.0, 120.0, 30.0), @"Bedtime", @selector(bedtimeButtonPressed:), self) retain];
	[contentView addSubview:_bedtimeButton];
	_setAllButton = [CreateActionButton(NSMakeRect(914.0, 18.0, 100.0, 30.0), @"Set", @selector(setAllButtonPressed:), self) retain];
	[contentView addSubview:_setAllButton];

	_partySnapshot.success = false;
	return self;
}

- (void)dealloc
{
	[_statusTextField release];
	[_refreshButton release];
	[_bedtimeButton release];
	[_setAllButton release];
	for (int row = 0; row < 6; ++row)
	{
		[_nameTextFields[row] release];
		[_hpLabelTextFields[row] release];
		[_hpTextFields[row] release];
		for (int statusIndex = 0; statusIndex < 7; ++statusIndex)
		{
			[_statusButtons[row][statusIndex] release];
		}
	}
	[super dealloc];
}

- (void)showWindow:(id)sender
{
	[super showWindow:sender];
	[[self window] makeKeyAndOrderFront:sender];
	[self reloadEditorData];
}

- (void)windowWillClose:(NSNotification *)notification
{
	(void)notification;
}

- (void)refreshControlStates
{
	const BOOL hasParty = !_partySnapshot.party.empty();
	[_bedtimeButton setEnabled:hasParty];
	[_setAllButton setEnabled:hasParty];

	for (NSInteger row = 0; row < 6; ++row)
	{
		const BOOL enabled = (row < (NSInteger)_partySnapshot.party.size());
		[_hpTextFields[row] setEnabled:enabled];
		for (NSInteger statusIndex = 0; statusIndex < 7; ++statusIndex)
		{
			[_statusButtons[row][statusIndex] setEnabled:enabled];
		}
	}
}

- (void)reloadEditorData
{
	desmume_pokemon_http_export::PartyEditorSnapshot nextSnapshot;
	if (!desmume_pokemon_http_export::GetPartyEditorSnapshot(nextSnapshot))
	{
		desmume_pokemon_http_export::PartyEditorSnapshot retrySnapshot;
		if (desmume_pokemon_http_export::GetPartyEditorSnapshot(retrySnapshot))
		{
			nextSnapshot = retrySnapshot;
		}
		else
		{
			_partySnapshot = nextSnapshot;
			[_statusTextField setStringValue:[NSString stringWithUTF8String:nextSnapshot.error.c_str()]];
			[self refreshControlStates];
			return;
		}
	}

	_partySnapshot = nextSnapshot;
	if (_partySnapshot.party.empty())
	{
		[_statusTextField setStringValue:@"Party is empty"];
	}
	else
	{
		const char *profileName = desmume_pokemon_http_export::GetProfileDisplayName(desmume_pokemon_http_export::GetSelectedProfile());
		[_statusTextField setStringValue:[NSString stringWithFormat:@"%s party ready", (profileName != NULL && profileName[0] != '\0') ? profileName : "Pokemon"]];
	}

	for (NSInteger row = 0; row < 6; ++row)
	{
		if (row >= (NSInteger)_partySnapshot.party.size())
		{
			[_nameTextFields[row] setStringValue:@""];
			[_hpTextFields[row] setStringValue:@""];
			[_hpLabelTextFields[row] setStringValue:@""];
			for (NSInteger statusIndex = 0; statusIndex < 7; ++statusIndex)
			{
				[_statusButtons[row][statusIndex] setState:NSControlStateValueOff];
			}
			continue;
		}

		const desmume_pokemon_http_export::PartyPokemonSummary &summary = _partySnapshot.party[(size_t)row];
		[_nameTextFields[row] setStringValue:[NSString stringWithFormat:@"%d. %@  HP %@",
			summary.slot,
			PokemonPartyDisplayNameForSummary(summary),
			HiddenPowerTypeForEditorIVs(summary.stats.ivs)]];
		[_hpTextFields[row] setStringValue:[NSString stringWithFormat:@"%d", summary.currentHp]];
		[_hpLabelTextFields[row] setStringValue:[NSString stringWithFormat:@"/ %d", summary.maxHp]];
		for (NSInteger statusIndex = 0; statusIndex < 7; ++statusIndex)
		{
			[_statusButtons[row][statusIndex] setState:(kPokemonPartyQuickStatusButtons[statusIndex].raw == summary.statusRaw)
				? NSControlStateValueOn
				: NSControlStateValueOff];
		}
	}
	[self refreshControlStates];
}

- (void)reportApplyFailedTryAgain
{
	[_statusTextField setStringValue:[NSString stringWithUTF8String:PokemonPartyApplyFailedTryAgainMessage]];
	NSBeep();
}

- (BOOL)validateCachedPartySnapshotStillMatchesLiveData
{
	desmume_pokemon_http_export::PartyEditorSnapshot currentSnapshot;
	if (!desmume_pokemon_http_export::GetPartyEditorSnapshot(currentSnapshot))
	{
		return NO;
	}
	if (currentSnapshot.party.size() != _partySnapshot.party.size())
	{
		return NO;
	}

	for (size_t cachedIndex = 0; cachedIndex < _partySnapshot.party.size(); ++cachedIndex)
	{
		const desmume_pokemon_http_export::PartyPokemonSummary &cachedSummary = _partySnapshot.party[cachedIndex];
		bool foundMatchingSlot = false;
		for (size_t currentIndex = 0; currentIndex < currentSnapshot.party.size(); ++currentIndex)
		{
			const desmume_pokemon_http_export::PartyPokemonSummary &currentSummary = currentSnapshot.party[currentIndex];
			if (currentSummary.slot != cachedSummary.slot)
			{
				continue;
			}

			foundMatchingSlot = true;
			if (!PokemonPartySummariesMatchForApplyPreflight(cachedSummary, currentSummary))
			{
				return NO;
			}
			break;
		}

		if (!foundMatchingSlot)
		{
			return NO;
		}
	}

	return YES;
}

- (void)applyEditForRow:(NSInteger)row currentHp:(NSInteger)currentHp statusRaw:(uint32_t)statusRaw
{
	if (row < 0 || row >= (NSInteger)_partySnapshot.party.size())
	{
		return;
	}

	const desmume_pokemon_http_export::PartyPokemonSummary &summary = _partySnapshot.party[(size_t)row];
	if (![self validateCachedPartySnapshotStillMatchesLiveData])
	{
		[self reportApplyFailedTryAgain];
		return;
	}

	desmume_pokemon_http_export::PartyPokemonHpStatusEditRequest request = {};
	request.slot = summary.slot;
	request.currentHp = (int)currentHp;
	request.preserveCurrentHp = YES;
	request.statusRaw = statusRaw;

	std::string error;
	if (!desmume_pokemon_http_export::UpdatePartyPokemonHpStatus(request, error))
	{
		[_statusTextField setStringValue:[NSString stringWithUTF8String:error.c_str()]];
		NSBeep();
		return;
	}

	NSString *displayName = PokemonPartyDisplayNameForSummary(summary);
	[self reloadEditorData];
	if (!_partySnapshot.success)
	{
		return;
	}
	[_statusTextField setStringValue:[NSString stringWithFormat:@"%@ updated", displayName]];
}

- (void)refreshButtonPressed:(id)sender
{
	(void)sender;
	[self reloadEditorData];
}

- (void)setAllButtonPressed:(id)sender
{
	(void)sender;

	BOOL shouldApply = NO;
	for (NSInteger row = 0; row < (NSInteger)_partySnapshot.party.size(); ++row)
	{
		const NSInteger requestedHp = [_hpTextFields[row] integerValue];
		const desmume_pokemon_http_export::PartyPokemonSummary &summary = _partySnapshot.party[(size_t)row];
		if (requestedHp != summary.currentHp)
		{
			shouldApply = YES;
			break;
		}
	}

	if (shouldApply && ![self validateCachedPartySnapshotStillMatchesLiveData])
	{
		[self reportApplyFailedTryAgain];
		return;
	}

	BOOL didChange = NO;
	for (NSInteger row = 0; row < (NSInteger)_partySnapshot.party.size(); ++row)
	{
		const NSInteger requestedHp = [_hpTextFields[row] integerValue];
		const desmume_pokemon_http_export::PartyPokemonSummary &summary = _partySnapshot.party[(size_t)row];
		if (requestedHp == summary.currentHp)
		{
			continue;
		}

		desmume_pokemon_http_export::PartyPokemonHpStatusEditRequest request = {};
		request.slot = summary.slot;
		request.currentHp = (int)requestedHp;
		request.preserveCurrentHp = NO;
		request.statusRaw = summary.statusRaw;
		request.encodedStatusRaw = summary.encodedStatusRaw;
		request.preserveEncodedStatusRaw = true;

		std::string error;
		if (!desmume_pokemon_http_export::UpdatePartyPokemonHpStatus(request, error))
		{
			[_statusTextField setStringValue:[NSString stringWithUTF8String:error.c_str()]];
			NSBeep();
			return;
		}

		didChange = YES;
	}

	[self reloadEditorData];
	if (!_partySnapshot.success)
	{
		return;
	}
	[_statusTextField setStringValue:didChange ? @"Party HP updated" : @"Party HP unchanged"];
}

- (void)hpFieldSubmitted:(id)sender
{
	[self setAllButtonPressed:sender];
}

- (void)statusButtonPressed:(id)sender
{
	const NSInteger tag = [sender tag];
	const NSInteger row = tag / 10;
	const NSInteger statusIndex = tag % 10;
	if (row < 0 || row >= (NSInteger)_partySnapshot.party.size() || statusIndex < 0 || statusIndex >= 7)
	{
		return;
	}

	[self applyEditForRow:row currentHp:_partySnapshot.party[(size_t)row].currentHp statusRaw:kPokemonPartyQuickStatusButtons[statusIndex].raw];
}

- (void)bedtimeButtonPressed:(id)sender
{
	(void)sender;

	std::string error;
	if (!desmume_pokemon_http_export::SetEntirePartyStatus(1u, error))
	{
		[_statusTextField setStringValue:[NSString stringWithUTF8String:error.c_str()]];
		NSBeep();
		return;
	}

	[self reloadEditorData];
	if (!_partySnapshot.success)
	{
		return;
	}
	const desmume_pokemon_http_export::ExportGameProfile selectedProfile = desmume_pokemon_http_export::GetSelectedProfile();
	if ((selectedProfile == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK_WHITE) ||
	    (selectedProfile == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK2_WHITE2) ||
	    (selectedProfile == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX))
	{
		[_statusTextField setStringValue:@"Entire party put to sleep"];
	}
	else
	{
		[_statusTextField setStringValue:@"Entire party set to 1-turn sleep"];
	}
}

@end

static NSMenuItem *FindMenuItemByTag(NSMenu *menu, NSInteger tag)
{
	for (NSMenuItem *item in [menu itemArray])
	{
		if ([item tag] == tag)
		{
			return item;
		}
		NSMenu *submenu = [item submenu];
		if (submenu != nil)
		{
			NSMenuItem *matchedItem = FindMenuItemByTag(submenu, tag);
			if (matchedItem != nil)
			{
				return matchedItem;
			}
		}
	}

	return nil;
}

@implementation EmuControllerDelegate

@synthesize inputManager;

@synthesize currentRom;
@dynamic cdsFirmware;
@synthesize audioController;

@synthesize cheatWindowDelegate;
@synthesize screenshotCaptureToolDelegate;
@synthesize avCaptureToolDelegate;
@synthesize prefWindowDelegate;
@synthesize firmwarePanelController;
@synthesize romInfoPanelController;
@synthesize cdsCoreController;
@synthesize cdsGraphicsController;
@synthesize cdsSoundController;
@synthesize cheatWindowController;
@synthesize slot2WindowController;
@synthesize inputDeviceListController;

@synthesize cheatDatabaseRecentsMenu;

@synthesize romInfoPanel;

@synthesize displayRotationPanel;
@synthesize displaySeparationPanel;
@synthesize displayVideoSettingsPanel;
@synthesize displayHUDSettingsPanel;

@synthesize executionControlWindow;
@synthesize slot1ManagerWindow;
@synthesize saveFileMigrationSheet;
@synthesize saveStatePrecloseSheet;
@synthesize ndsErrorSheet;
@synthesize ndsErrorStatusTextField;
@synthesize exportRomSavePanelAccessoryView;

@synthesize openglMSAAPopUpButton;

@synthesize iconExecute;
@synthesize iconPause;
@synthesize iconSpeedNormal;
@synthesize iconSpeedDouble;

@synthesize lastSetSpeedScalar;

@synthesize isRunningDarkMode;
@synthesize isWorking;
@synthesize isRomLoading;
@synthesize statusText;
@synthesize isHardwareMicAvailable;
@synthesize currentMicGainValue;
@synthesize micStatusTooltip;
@synthesize currentMicStatusIcon;
@synthesize ndsMicLevelIndicator;

@synthesize isShowingSaveStateDialog;
@synthesize isShowingFileMigrationDialog;
@synthesize isUserInterfaceBlockingExecution;

@synthesize currentSaveStateURL;
@synthesize selectedExportRomSaveID;
@synthesize selectedRomSaveTypeID;

@synthesize mainWindow;
@synthesize windowList;


- (id)init
{
	self = [super init];
	if(self == nil)
	{
		return nil;
	}
	
	_unfairlockFirmware = apple_unfairlock_create();
	
	mainWindow = nil;
	windowList = [[NSMutableArray alloc] initWithCapacity:32];
	pokemonPartyEditorWindowController = nil;
	pokemonPartyStatusEditorWindowController = nil;
	
	isRunningDarkMode = [CocoaDSUtil determineDarkModeAppearance];
	
	_displayRotationPanelTitle = nil;
	_displaySeparationPanelTitle = nil;
	_displayVideoSettingsPanelTitle = nil;
	_displayHUDSettingsPanelTitle = nil;
	
	currentRom = nil;
	cdsFirmware = nil;
	audioController = [[CocoaAudioController alloc] init];
	
	isSaveStateEdited = NO;
	isShowingSaveStateDialog = NO;
	isShowingFileMigrationDialog = NO;
	isUserInterfaceBlockingExecution = NO;
	_isNDSErrorSheetAlreadyShown = NO;
	
	currentSaveStateURL = nil;
	selectedRomSaveTypeID = ROMSAVETYPE_AUTOMATIC;
	selectedExportRomSaveID = 0;
	
	lastSetSpeedScalar = 1.0f;
	isHardwareMicAvailable = NO;
	currentMicGainValue = 0.0f;
	
	iconExecute           = [[NSImage imageNamed:@"Icon_Execute_420x420"] retain];
	iconPause             = [[NSImage imageNamed:@"Icon_Pause_420x420"] retain];
	iconSpeedNormal       = [[NSImage imageNamed:@"Icon_Speed1x_420x420"] retain];
	iconSpeedDouble       = [[NSImage imageNamed:@"Icon_Speed2x_420x420"] retain];
	pokemonClipboardSyncTimer = nil;
	pokemonClipboardLastExportText = nil;
	pokemonClipboardLastBattleEndUnixMs = 0u;
	pokemonClipboardLastRefreshCheckUnixMs = 0u;
	
	iconMicDisabled       = [[NSImage imageNamed:@"Icon_MicrophoneBlack_256x256"] retain];
	iconMicDisabledDM     = [[NSImage imageNamed:@"Icon_MicrophoneOff_DarkMode_256x256"] retain];
	iconMicIdle           = [[NSImage imageNamed:@"Icon_MicrophoneDarkGreen_256x256"] retain];
	iconMicIdleNoHardware = [[NSImage imageNamed:@"Icon_MicrophoneIdleNoHardware_256x256"] retain];
	iconMicActive         = [[NSImage imageNamed:@"Icon_MicrophoneGreen_256x256"] retain];
	iconMicInClip         = [[NSImage imageNamed:@"Icon_MicrophoneRed_256x256"] retain];
	iconMicManualOverride = [[NSImage imageNamed:@"Icon_MicrophoneGray_256x256"] retain];
	
	isWorking = NO;
	isRomLoading = NO;
	statusText = NSSTRING_STATUS_READY;
	micStatusTooltip = @"";
	currentMicStatusIcon = (isRunningDarkMode) ? [iconMicDisabledDM retain] : [iconMicDisabled retain];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(loadRomDidFinish:)
												 name:@"org.desmume.DeSmuME.loadRomDidFinish"
											   object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(handleNDSError:)
												 name:@"org.desmume.DeSmuME.handleNDSError"
											   object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(handleEmulatorExecutionState:)
												 name:@"org.desmume.DeSmuME.handleEmulatorExecutionState"
											   object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(updateCheatDatabaseRecentsMenu:)
												 name:@"org.desmume.DeSmuME.updateCheatDatabaseRecentsMenu"
											   object:nil];
	
	return self;
}

- (void)awakeFromNib
{
	[super awakeFromNib];
	[self installLoadStateBackupSaveMenuItem];
	[self installPokemonHttpApiMenuItems];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	CommonSettings.loadStateWritesBackupSave = ([defaults objectForKey:LoadStateWritesBackupSaveDefaultsKey] == nil) ? true : [defaults boolForKey:LoadStateWritesBackupSaveDefaultsKey];
	const NSInteger selectedProfile = [[NSUserDefaults standardUserDefaults] integerForKey:PokemonHttpExportProfileDefaultsKey];
	desmume_pokemon_http_export::SetSelectedProfile(
		(selectedProfile == 0)
			? desmume_pokemon_http_export::EXPORT_GAME_POKEMON_PLATINUM
			: (desmume_pokemon_http_export::ExportGameProfile)selectedProfile);
	const NSInteger platinumTimeOverride = [[NSUserDefaults standardUserDefaults] integerForKey:PokemonPlatinumTimeOverrideDefaultsKey];
	desmume_pokemon_http_export::SetPlatinumTimeOverride((desmume_pokemon_http_export::PlatinumTimeOverride)platinumTimeOverride);
	desmume_pokemon_http_export::SetHealthCheckEnabled(([defaults objectForKey:PokemonHttpExportHealthCheckEnabledDefaultsKey] == nil) ? true : [defaults boolForKey:PokemonHttpExportHealthCheckEnabledDefaultsKey]);
	desmume_pokemon_http_export::SetHealthCheckIntervalMs((uint32_t)[defaults integerForKey:PokemonHttpExportHealthCheckIntervalDefaultsKey]);
	desmume_pokemon_http_export::SetIdleRefreshEnabled(([defaults objectForKey:PokemonHttpExportIdleRefreshEnabledDefaultsKey] == nil) ? true : [defaults boolForKey:PokemonHttpExportIdleRefreshEnabledDefaultsKey]);
	desmume_pokemon_http_export::SetIdleRefreshIntervalMs((uint32_t)[defaults integerForKey:PokemonHttpExportIdleRefreshIntervalDefaultsKey]);
	pokemonClipboardAutoCopyEnabled = ([defaults objectForKey:PokemonClipboardAutoCopyEnabledDefaultsKey] == nil) ? YES : [defaults boolForKey:PokemonClipboardAutoCopyEnabledDefaultsKey];
	pokemonClipboardAutoCopyIntervalMs = (uint64_t)[defaults integerForKey:PokemonClipboardAutoCopyIntervalDefaultsKey];
	if (pokemonClipboardAutoCopyIntervalMs == 0u)
	{
		pokemonClipboardAutoCopyIntervalMs = PokemonClipboardOutOfBattleRefreshDefaultMs;
	}
	PokemonMoreScripts_SetDisplayPartyAbilitySlotsEnabled(([defaults objectForKey:PokemonDisplayPartyAbilitySlotsDefaultsKey] == nil) ? false : [defaults boolForKey:PokemonDisplayPartyAbilitySlotsDefaultsKey]);
	PokemonMoreScripts_SetSpinInPlaceEnabled(([defaults objectForKey:PokemonSpinInPlaceDefaultsKey] == nil) ? false : [defaults boolForKey:PokemonSpinInPlaceDefaultsKey]);
	NDS_setOneFrameTouchPressGuardEnabled(([defaults objectForKey:PokemonOneFrameTouchGuardDefaultsKey] == nil) ? true : [defaults boolForKey:PokemonOneFrameTouchGuardDefaultsKey]);
	const BOOL httpEnabled = [defaults boolForKey:PokemonHttpExportDefaultsKey];
	desmume_pokemon_http_export::SetEnabled(httpEnabled);
	desmume_pokemon_http_export::SetClipboardSyncEnabled(pokemonClipboardAutoCopyEnabled ? true : false);
	desmume_pokemon_http_export::SetBattleLoggingEnabled(([defaults objectForKey:PokemonBattleLoggingDefaultsKey] == nil) ? httpEnabled : [defaults boolForKey:PokemonBattleLoggingDefaultsKey]);
	pokemonClipboardSyncTimer = [[NSTimer scheduledTimerWithTimeInterval:1.0
	                                                              target:self
	                                                            selector:@selector(updatePokemonClipboardAutoSync:)
	                                                            userInfo:nil
	                                                             repeats:YES] retain];
	[self resetPokemonClipboardAutoSyncState];
	[self updatePokemonHttpApiMenuItems];
}

- (void)dealloc
{
	[pokemonClipboardSyncTimer invalidate];
	[pokemonClipboardSyncTimer release];
	pokemonClipboardSyncTimer = nil;
	[pokemonClipboardLastExportText release];
	pokemonClipboardLastExportText = nil;
	desmume_pokemon_http_export::Shutdown();
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[iconExecute release];
	[iconPause release];
	[iconSpeedNormal release];
	[iconSpeedDouble release];
	
	[iconMicDisabled release];
	[iconMicDisabledDM release];
	[iconMicIdle release];
	[iconMicIdleNoHardware release];
	[iconMicActive release];
	[iconMicInClip release];
	[iconMicManualOverride release];
	
	[[self currentRom] release];
	[self setCurrentRom:nil];
	
	[self setIsWorking:NO];
	[self setStatusText:@""];
	
	[romInfoPanelController setContent:[CocoaDSRom romNotLoadedBindings]];
	[cdsSoundController setContent:nil];
	[cdsGraphicsController setContent:nil];
	[firmwarePanelController setContent:nil];
	
	[self setMainWindow:nil];
	[windowList release];
	[pokemonPartyEditorWindowController release];
	[pokemonPartyStatusEditorWindowController release];
	
	[audioController release];
	
	apple_unfairlock_destroy(_unfairlockFirmware);
	
	[super dealloc];
}

#pragma mark Dynamic Property Methods

- (void) setCdsFirmware:(CocoaDSFirmware *)theFirmware
{
	apple_unfairlock_lock(_unfairlockFirmware);
	
	if (theFirmware == cdsFirmware)
	{
		apple_unfairlock_unlock(_unfairlockFirmware);
		return;
	}
	
	if (theFirmware != nil)
	{
		[theFirmware retain];
	}
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore setCdsFirmware:theFirmware];
	[cdsCore updateFirmwareMACAddressString];
	[firmwarePanelController setContent:theFirmware];
	
	[cdsFirmware release];
	cdsFirmware = theFirmware;
	
	apple_unfairlock_unlock(_unfairlockFirmware);
}

- (CocoaDSFirmware *) cdsFirmware
{
	apple_unfairlock_lock(_unfairlockFirmware);
	CocoaDSFirmware *theFirmware = cdsFirmware;
	apple_unfairlock_unlock(_unfairlockFirmware);
	
	return theFirmware;
}

- (void)installLoadStateBackupSaveMenuItem
{
	NSMenu *mainMenu = [NSApp mainMenu];
	if (mainMenu == nil || FindMenuItemByTag(mainMenu, LoadStateWritesBackupSaveMenuTag) != nil)
	{
		return;
	}

	NSMenuItem *fileRootItem = [mainMenu itemWithTitle:@"File"];
	NSMenu *fileMenu = [fileRootItem submenu];
	if (fileMenu == nil)
	{
		return;
	}

	NSInteger insertIndex = [fileMenu indexOfItemWithTitle:@"Save State Slot"];
	if (insertIndex < 0)
	{
		insertIndex = [fileMenu indexOfItemWithTitle:@"Load State Slot"];
	}
	if (insertIndex < 0)
	{
		insertIndex = [fileMenu numberOfItems];
	}
	else
	{
		insertIndex++;
	}

	NSMenuItem *item = [[[NSMenuItem alloc] initWithTitle:@"Load State Writes Battery Save"
	                                              action:@selector(toggleLoadStateWritesBackupSave:)
	                                       keyEquivalent:@""] autorelease];
	[item setTarget:self];
	[item setTag:LoadStateWritesBackupSaveMenuTag];
	[fileMenu insertItem:item atIndex:insertIndex];
}

- (void)installPokemonHttpApiMenuItems
{
	NSMenu *mainMenu = [NSApp mainMenu];
	if (mainMenu == nil || FindMenuItemByTag(mainMenu, PokemonMenuRootTag) != nil)
	{
		return;
	}

	NSMenu *pokemonMenu = [[[NSMenu alloc] initWithTitle:@"Poketools"] autorelease];
	NSMenuItem *pokemonMenuRootItem = [[[NSMenuItem alloc] initWithTitle:@"Poketools"
	                                                              action:nil
	                                                       keyEquivalent:@""] autorelease];
	[pokemonMenuRootItem setTag:PokemonMenuRootTag];
	[pokemonMenuRootItem setSubmenu:pokemonMenu];

	NSMenuItem *toggleItem = [[[NSMenuItem alloc] initWithTitle:@"Enable Pokemon HTTP API"
	                                                     action:@selector(togglePokemonHttpApi:)
	                                              keyEquivalent:@""] autorelease];
	[toggleItem setTarget:self];
	[toggleItem setTag:PokemonHttpExportToggleTag];

	NSMenuItem *battleLoggingToggleItem = [[[NSMenuItem alloc] initWithTitle:@"Enable Battle Logging"
	                                                                  action:@selector(togglePokemonBattleLogging:)
	                                                           keyEquivalent:@""] autorelease];
	[battleLoggingToggleItem setTarget:self];
	[battleLoggingToggleItem setTag:PokemonBattleLoggingToggleTag];

	NSMenuItem *oneFrameTouchGuardItem = [[[NSMenuItem alloc] initWithTitle:@"Touch Tap Guard"
	                                                                 action:@selector(togglePokemonOneFrameTouchGuard:)
	                                                          keyEquivalent:@""] autorelease];
	[oneFrameTouchGuardItem setTarget:self];
	[oneFrameTouchGuardItem setTag:PokemonOneFrameTouchGuardTag];

	NSMenuItem *copyExportItem = [[[NSMenuItem alloc] initWithTitle:@"Copy Pokemon Export to Clipboard"
	                                                         action:@selector(copyPokemonExportToClipboard:)
	                                                  keyEquivalent:@""] autorelease];
	[copyExportItem setTarget:self];
	[copyExportItem setTag:PokemonCopyExportMenuTag];

	NSMenuItem *partyEditorItem = [[[NSMenuItem alloc] initWithTitle:@"Party/Box Editor..."
	                                                         action:@selector(openPokemonPartyEditor:)
	                                                  keyEquivalent:@""] autorelease];
	[partyEditorItem setTarget:self];
	[partyEditorItem setTag:PokemonPartyEditorMenuTag];

	NSMenuItem *partyStatusEditorItem = [[[NSMenuItem alloc] initWithTitle:@"Party Status/HP Editor..."
	                                                               action:@selector(openPokemonPartyStatusEditor:)
	                                                        keyEquivalent:@""] autorelease];
	[partyStatusEditorItem setTarget:self];
	[partyStatusEditorItem setTag:PokemonPartyStatusEditorMenuTag];

	NSMenu *profileMenu = [[[NSMenu alloc] initWithTitle:@"Select Pokemon Game"] autorelease];
	NSMenuItem *profileMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Select Pokemon Game"
	                                                         action:nil
	                                                  keyEquivalent:@""] autorelease];
	[profileMenuItem setTag:PokemonHttpExportProfileMenuTag];
	[profileMenuItem setSubmenu:profileMenu];

	NSMenu *platinumTimeMenu = [[[NSMenu alloc] initWithTitle:@"Set Gen 4 Time of Day"] autorelease];
	NSMenuItem *platinumTimeMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Set Gen 4 Time of Day"
	                                                              action:nil
	                                                       keyEquivalent:@""] autorelease];
	[platinumTimeMenuItem setTag:PokemonPlatinumTimeMenuTag];
	[platinumTimeMenuItem setSubmenu:platinumTimeMenu];

	NSMenu *platinumTrainerDebugMenu = [[[NSMenu alloc] initWithTitle:@"Platinum Trainer Debug"] autorelease];
	NSMenuItem *platinumTrainerDebugMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Platinum Trainer Debug"
	                                                                   action:nil
	                                                            keyEquivalent:@""] autorelease];
	[platinumTrainerDebugMenuItem setSubmenu:platinumTrainerDebugMenu];

	NSMenu *maintenanceMenu = [[[NSMenu alloc] initWithTitle:@"Pokemon HTTP Maintenance"] autorelease];
	NSMenuItem *maintenanceMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Pokemon HTTP Maintenance"
	                                                             action:nil
	                                                      keyEquivalent:@""] autorelease];
	[maintenanceMenuItem setTag:PokemonHttpExportMaintenanceMenuTag];
	[maintenanceMenuItem setSubmenu:maintenanceMenu];

	NSMenu *moreScriptsMenu = [[[NSMenu alloc] initWithTitle:@"More Scripts"] autorelease];
	NSMenuItem *moreScriptsMenuItem = [[[NSMenuItem alloc] initWithTitle:@"More Scripts"
	                                                             action:nil
	                                                      keyEquivalent:@""] autorelease];
	[moreScriptsMenuItem setTag:PokemonMoreScriptsMenuTag];
	[moreScriptsMenuItem setSubmenu:moreScriptsMenu];

	NSMenuItem *platinumProfileItem = [[[NSMenuItem alloc] initWithTitle:@"Pokemon Platinum"
	                                                              action:@selector(selectPokemonHttpApiProfile:)
	                                                       keyEquivalent:@""] autorelease];
	[platinumProfileItem setTarget:self];
	[platinumProfileItem setTag:PokemonHttpExportProfilePlatinumTag];
	[profileMenu addItem:platinumProfileItem];

	NSMenuItem *platinumKaizoProfileItem = [[[NSMenuItem alloc] initWithTitle:@"Platinum Kaizo"
	                                                                   action:@selector(selectPokemonHttpApiProfile:)
	                                                            keyEquivalent:@""] autorelease];
	[platinumKaizoProfileItem setTarget:self];
	[platinumKaizoProfileItem setTag:PokemonHttpExportProfilePlatinumKaizoTag];
	[profileMenu addItem:platinumKaizoProfileItem];

	NSMenuItem *renegadePlatinumProfileItem = [[[NSMenuItem alloc] initWithTitle:@"Renegade Platinum"
	                                                                      action:@selector(selectPokemonHttpApiProfile:)
	                                                               keyEquivalent:@""] autorelease];
	[renegadePlatinumProfileItem setTarget:self];
	[renegadePlatinumProfileItem setTag:PokemonHttpExportProfileRenegadePlatinumTag];
	[profileMenu addItem:renegadePlatinumProfileItem];

	NSMenuItem *hgssProfileItem = [[[NSMenuItem alloc] initWithTitle:@"Pokemon HeartGold/SoulSilver"
	                                                          action:@selector(selectPokemonHttpApiProfile:)
	                                                   keyEquivalent:@""] autorelease];
	[hgssProfileItem setTarget:self];
	[hgssProfileItem setTag:PokemonHttpExportProfileHgssTag];
	[profileMenu addItem:hgssProfileItem];

	NSMenuItem *sterlingSilverProfileItem = [[[NSMenuItem alloc] initWithTitle:@"Sterling Silver"
	                                                                    action:@selector(selectPokemonHttpApiProfile:)
	                                                             keyEquivalent:@""] autorelease];
	[sterlingSilverProfileItem setTarget:self];
	[sterlingSilverProfileItem setTag:PokemonHttpExportProfileSterlingSilverTag];
	[profileMenu addItem:sterlingSilverProfileItem];

	NSMenuItem *blackWhiteProfileItem = [[[NSMenuItem alloc] initWithTitle:@"Pokemon Black/White"
	                                                                 action:@selector(selectPokemonHttpApiProfile:)
	                                                          keyEquivalent:@""] autorelease];
	[blackWhiteProfileItem setTarget:self];
	[blackWhiteProfileItem setTag:PokemonHttpExportProfileBlackWhiteTag];
	[profileMenu addItem:blackWhiteProfileItem];

	NSMenuItem *black2White2ProfileItem = [[[NSMenuItem alloc] initWithTitle:@"Pokemon Black 2/White 2"
	                                                                  action:@selector(selectPokemonHttpApiProfile:)
	                                                           keyEquivalent:@""] autorelease];
	[black2White2ProfileItem setTarget:self];
	[black2White2ProfileItem setTag:PokemonHttpExportProfileBlack2White2Tag];
	[profileMenu addItem:black2White2ProfileItem];

	NSMenuItem *bb2Vw2ReduxProfileItem = [[[NSMenuItem alloc] initWithTitle:@"Blaze Black 2 Redux/Volt White 2 Redux"
	                                                                 action:@selector(selectPokemonHttpApiProfile:)
	                                                          keyEquivalent:@""] autorelease];
	[bb2Vw2ReduxProfileItem setTarget:self];
	[bb2Vw2ReduxProfileItem setTag:PokemonHttpExportProfileBb2Vw2ReduxTag];
	[profileMenu addItem:bb2Vw2ReduxProfileItem];

	NSMenuItem *platinumTimeAutoItem = [[[NSMenuItem alloc] initWithTitle:@"Auto"
	                                                               action:@selector(selectPokemonPlatinumTimeOverride:)
	                                                        keyEquivalent:@""] autorelease];
	[platinumTimeAutoItem setTarget:self];
	[platinumTimeAutoItem setTag:PokemonPlatinumTimeAutoTag];
	[platinumTimeMenu addItem:platinumTimeAutoItem];

	NSMenuItem *platinumTimeMorningItem = [[[NSMenuItem alloc] initWithTitle:@"Morning"
	                                                                  action:@selector(selectPokemonPlatinumTimeOverride:)
	                                                           keyEquivalent:@""] autorelease];
	[platinumTimeMorningItem setTarget:self];
	[platinumTimeMorningItem setTag:PokemonPlatinumTimeMorningTag];
	[platinumTimeMenu addItem:platinumTimeMorningItem];

	NSMenuItem *platinumTimeDayItem = [[[NSMenuItem alloc] initWithTitle:@"Day"
	                                                              action:@selector(selectPokemonPlatinumTimeOverride:)
	                                                       keyEquivalent:@""] autorelease];
	[platinumTimeDayItem setTarget:self];
	[platinumTimeDayItem setTag:PokemonPlatinumTimeDayTag];
	[platinumTimeMenu addItem:platinumTimeDayItem];

	NSMenuItem *platinumTimeEveningItem = [[[NSMenuItem alloc] initWithTitle:@"Evening"
	                                                                  action:@selector(selectPokemonPlatinumTimeOverride:)
	                                                           keyEquivalent:@""] autorelease];
	[platinumTimeEveningItem setTarget:self];
	[platinumTimeEveningItem setTag:PokemonPlatinumTimeEveningTag];
	[platinumTimeMenu addItem:platinumTimeEveningItem];

	NSMenuItem *platinumTimeNightItem = [[[NSMenuItem alloc] initWithTitle:@"Night"
	                                                                action:@selector(selectPokemonPlatinumTimeOverride:)
	                                                         keyEquivalent:@""] autorelease];
	[platinumTimeNightItem setTarget:self];
	[platinumTimeNightItem setTag:PokemonPlatinumTimeNightTag];
	[platinumTimeMenu addItem:platinumTimeNightItem];

	NSMenuItem *platinumTimeLateNightItem = [[[NSMenuItem alloc] initWithTitle:@"Late Night"
	                                                                    action:@selector(selectPokemonPlatinumTimeOverride:)
	                                                             keyEquivalent:@""] autorelease];
	[platinumTimeLateNightItem setTarget:self];
	[platinumTimeLateNightItem setTag:PokemonPlatinumTimeLateNightTag];
	[platinumTimeMenu addItem:platinumTimeLateNightItem];

	NSMenuItem *platinumTrainerStatusItem = [[[NSMenuItem alloc] initWithTitle:@"Next trainer override: None"
	                                                                    action:nil
	                                                             keyEquivalent:@""] autorelease];
	[platinumTrainerStatusItem setTag:PokemonPlatinumTrainerBattleOverrideStatusTag];
	[platinumTrainerStatusItem setEnabled:NO];
	[platinumTrainerDebugMenu addItem:platinumTrainerStatusItem];

	NSMenuItem *platinumTrainerSetItem = [[[NSMenuItem alloc] initWithTitle:@"Set Next Trainer Battle..."
	                                                                 action:@selector(setPokemonPlatinumNextTrainerBattleOverride:)
	                                                          keyEquivalent:@""] autorelease];
	[platinumTrainerSetItem setTarget:self];
	[platinumTrainerSetItem setTag:PokemonPlatinumTrainerBattleOverrideSetTag];
	[platinumTrainerDebugMenu addItem:platinumTrainerSetItem];

	NSMenuItem *platinumTrainerClearItem = [[[NSMenuItem alloc] initWithTitle:@"Clear Next Trainer Override"
	                                                                   action:@selector(clearPokemonPlatinumNextTrainerBattleOverride:)
	                                                            keyEquivalent:@""] autorelease];
	[platinumTrainerClearItem setTarget:self];
	[platinumTrainerClearItem setTag:PokemonPlatinumTrainerBattleOverrideClearTag];
	[platinumTrainerDebugMenu addItem:platinumTrainerClearItem];

	NSMenuItem *forceRefreshItem = [[[NSMenuItem alloc] initWithTitle:@"Force Refresh Now"
	                                                           action:@selector(forcePokemonHttpApiRefresh:)
	                                                    keyEquivalent:@""] autorelease];
	[forceRefreshItem setTarget:self];
	[forceRefreshItem setTag:PokemonHttpExportForceRefreshTag];
	[maintenanceMenu addItem:forceRefreshItem];
	[maintenanceMenu addItem:[NSMenuItem separatorItem]];

	NSMenuItem *healthCheckToggleItem = [[[NSMenuItem alloc] initWithTitle:@"Enable Health Checks"
	                                                                action:@selector(togglePokemonHttpHealthCheck:)
	                                                         keyEquivalent:@""] autorelease];
	[healthCheckToggleItem setTarget:self];
	[healthCheckToggleItem setTag:PokemonHttpExportHealthCheckToggleTag];
	[maintenanceMenu addItem:healthCheckToggleItem];

	NSMenu *healthCheckIntervalMenu = [[[NSMenu alloc] initWithTitle:@"Health Check Interval"] autorelease];
	NSMenuItem *healthCheckIntervalMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Health Check Interval"
	                                                                     action:nil
	                                                              keyEquivalent:@""] autorelease];
	[healthCheckIntervalMenuItem setTag:PokemonHttpExportHealthCheckIntervalMenuTag];
	[healthCheckIntervalMenuItem setSubmenu:healthCheckIntervalMenu];
	[maintenanceMenu addItem:healthCheckIntervalMenuItem];

	NSMenuItem *healthCheck15sItem = [[[NSMenuItem alloc] initWithTitle:@"15 Seconds"
	                                                             action:@selector(selectPokemonHttpHealthCheckInterval:)
	                                                      keyEquivalent:@""] autorelease];
	[healthCheck15sItem setTarget:self];
	[healthCheck15sItem setTag:PokemonHttpExportHealthCheckInterval15sTag];
	[healthCheckIntervalMenu addItem:healthCheck15sItem];

	NSMenuItem *healthCheck30sItem = [[[NSMenuItem alloc] initWithTitle:@"30 Seconds"
	                                                             action:@selector(selectPokemonHttpHealthCheckInterval:)
	                                                      keyEquivalent:@""] autorelease];
	[healthCheck30sItem setTarget:self];
	[healthCheck30sItem setTag:PokemonHttpExportHealthCheckInterval30sTag];
	[healthCheckIntervalMenu addItem:healthCheck30sItem];

	NSMenuItem *healthCheck60sItem = [[[NSMenuItem alloc] initWithTitle:@"60 Seconds"
	                                                             action:@selector(selectPokemonHttpHealthCheckInterval:)
	                                                      keyEquivalent:@""] autorelease];
	[healthCheck60sItem setTarget:self];
	[healthCheck60sItem setTag:PokemonHttpExportHealthCheckInterval60sTag];
	[healthCheckIntervalMenu addItem:healthCheck60sItem];

	NSMenuItem *healthCheck120sItem = [[[NSMenuItem alloc] initWithTitle:@"120 Seconds"
	                                                              action:@selector(selectPokemonHttpHealthCheckInterval:)
	                                                       keyEquivalent:@""] autorelease];
	[healthCheck120sItem setTarget:self];
	[healthCheck120sItem setTag:PokemonHttpExportHealthCheckInterval120sTag];
	[healthCheckIntervalMenu addItem:healthCheck120sItem];

	NSMenuItem *healthCheck300sItem = [[[NSMenuItem alloc] initWithTitle:@"300 Seconds"
	                                                              action:@selector(selectPokemonHttpHealthCheckInterval:)
	                                                       keyEquivalent:@""] autorelease];
	[healthCheck300sItem setTarget:self];
	[healthCheck300sItem setTag:PokemonHttpExportHealthCheckInterval300sTag];
	[healthCheckIntervalMenu addItem:healthCheck300sItem];

	[maintenanceMenu addItem:[NSMenuItem separatorItem]];

	NSMenuItem *idleRefreshToggleItem = [[[NSMenuItem alloc] initWithTitle:@"Enable Idle Refresh"
	                                                                action:@selector(togglePokemonHttpIdleRefresh:)
	                                                         keyEquivalent:@""] autorelease];
	[idleRefreshToggleItem setTarget:self];
	[idleRefreshToggleItem setTag:PokemonHttpExportIdleRefreshToggleTag];
	[maintenanceMenu addItem:idleRefreshToggleItem];

	NSMenu *idleRefreshIntervalMenu = [[[NSMenu alloc] initWithTitle:@"Idle Refresh Interval"] autorelease];
	NSMenuItem *idleRefreshIntervalMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Idle Refresh Interval"
	                                                                     action:nil
	                                                              keyEquivalent:@""] autorelease];
	[idleRefreshIntervalMenuItem setTag:PokemonHttpExportIdleRefreshIntervalMenuTag];
	[idleRefreshIntervalMenuItem setSubmenu:idleRefreshIntervalMenu];
	[maintenanceMenu addItem:idleRefreshIntervalMenuItem];

	NSMenuItem *idleRefresh60sItem = [[[NSMenuItem alloc] initWithTitle:@"60 Seconds"
	                                                             action:@selector(selectPokemonHttpIdleRefreshInterval:)
	                                                      keyEquivalent:@""] autorelease];
	[idleRefresh60sItem setTarget:self];
	[idleRefresh60sItem setTag:PokemonHttpExportIdleRefreshInterval60sTag];
	[idleRefreshIntervalMenu addItem:idleRefresh60sItem];

	NSMenuItem *idleRefresh180sItem = [[[NSMenuItem alloc] initWithTitle:@"180 Seconds"
	                                                              action:@selector(selectPokemonHttpIdleRefreshInterval:)
	                                                       keyEquivalent:@""] autorelease];
	[idleRefresh180sItem setTarget:self];
	[idleRefresh180sItem setTag:PokemonHttpExportIdleRefreshInterval180sTag];
	[idleRefreshIntervalMenu addItem:idleRefresh180sItem];

	NSMenuItem *idleRefresh300sItem = [[[NSMenuItem alloc] initWithTitle:@"300 Seconds"
	                                                              action:@selector(selectPokemonHttpIdleRefreshInterval:)
	                                                       keyEquivalent:@""] autorelease];
	[idleRefresh300sItem setTarget:self];
	[idleRefresh300sItem setTag:PokemonHttpExportIdleRefreshInterval300sTag];
	[idleRefreshIntervalMenu addItem:idleRefresh300sItem];

	NSMenuItem *idleRefresh600sItem = [[[NSMenuItem alloc] initWithTitle:@"600 Seconds"
	                                                              action:@selector(selectPokemonHttpIdleRefreshInterval:)
	                                                       keyEquivalent:@""] autorelease];
	[idleRefresh600sItem setTarget:self];
	[idleRefresh600sItem setTag:PokemonHttpExportIdleRefreshInterval600sTag];
	[idleRefreshIntervalMenu addItem:idleRefresh600sItem];

	[maintenanceMenu addItem:[NSMenuItem separatorItem]];

	NSMenuItem *clipboardAutoCopyToggleItem = [[[NSMenuItem alloc] initWithTitle:@"Enable Clipboard Auto Copy"
	                                                                   action:@selector(togglePokemonClipboardAutoCopy:)
	                                                            keyEquivalent:@""] autorelease];
	[clipboardAutoCopyToggleItem setTarget:self];
	[clipboardAutoCopyToggleItem setTag:PokemonClipboardAutoCopyToggleTag];
	[maintenanceMenu addItem:clipboardAutoCopyToggleItem];

	NSMenu *clipboardAutoCopyIntervalMenu = [[[NSMenu alloc] initWithTitle:@"Clipboard Auto Copy Interval"] autorelease];
	NSMenuItem *clipboardAutoCopyIntervalMenuItem = [[[NSMenuItem alloc] initWithTitle:@"Clipboard Auto Copy Interval"
	                                                                        action:nil
	                                                                 keyEquivalent:@""] autorelease];
	[clipboardAutoCopyIntervalMenuItem setTag:PokemonClipboardAutoCopyIntervalMenuTag];
	[clipboardAutoCopyIntervalMenuItem setSubmenu:clipboardAutoCopyIntervalMenu];
	[maintenanceMenu addItem:clipboardAutoCopyIntervalMenuItem];

	NSMenuItem *clipboardAutoCopy5sItem = [[[NSMenuItem alloc] initWithTitle:@"5 Seconds"
	                                                              action:@selector(selectPokemonClipboardAutoCopyInterval:)
	                                                       keyEquivalent:@""] autorelease];
	[clipboardAutoCopy5sItem setTarget:self];
	[clipboardAutoCopy5sItem setTag:PokemonClipboardAutoCopyInterval5sTag];
	[clipboardAutoCopyIntervalMenu addItem:clipboardAutoCopy5sItem];

	NSMenuItem *clipboardAutoCopy10sItem = [[[NSMenuItem alloc] initWithTitle:@"10 Seconds"
	                                                               action:@selector(selectPokemonClipboardAutoCopyInterval:)
	                                                        keyEquivalent:@""] autorelease];
	[clipboardAutoCopy10sItem setTarget:self];
	[clipboardAutoCopy10sItem setTag:PokemonClipboardAutoCopyInterval10sTag];
	[clipboardAutoCopyIntervalMenu addItem:clipboardAutoCopy10sItem];

	NSMenuItem *clipboardAutoCopy30sItem = [[[NSMenuItem alloc] initWithTitle:@"30 Seconds"
	                                                               action:@selector(selectPokemonClipboardAutoCopyInterval:)
	                                                        keyEquivalent:@""] autorelease];
	[clipboardAutoCopy30sItem setTarget:self];
	[clipboardAutoCopy30sItem setTag:PokemonClipboardAutoCopyInterval30sTag];
	[clipboardAutoCopyIntervalMenu addItem:clipboardAutoCopy30sItem];

	NSMenuItem *clipboardAutoCopy60sItem = [[[NSMenuItem alloc] initWithTitle:@"60 Seconds"
	                                                               action:@selector(selectPokemonClipboardAutoCopyInterval:)
	                                                        keyEquivalent:@""] autorelease];
	[clipboardAutoCopy60sItem setTarget:self];
	[clipboardAutoCopy60sItem setTag:PokemonClipboardAutoCopyInterval60sTag];
	[clipboardAutoCopyIntervalMenu addItem:clipboardAutoCopy60sItem];

	NSMenuItem *displayPartyAbilitySlotsItem = [[[NSMenuItem alloc] initWithTitle:@"Display Party Ability Slots"
	                                                                    action:@selector(togglePokemonDisplayPartyAbilitySlots:)
	                                                             keyEquivalent:@""] autorelease];
	[displayPartyAbilitySlotsItem setTarget:self];
	[displayPartyAbilitySlotsItem setTag:PokemonDisplayPartyAbilitySlotsTag];
	[moreScriptsMenu addItem:displayPartyAbilitySlotsItem];

	NSMenuItem *spinInPlaceItem = [[[NSMenuItem alloc] initWithTitle:@"Spin in Place"
	                                                          action:@selector(togglePokemonSpinInPlace:)
	                                                   keyEquivalent:@""] autorelease];
	[spinInPlaceItem setTarget:self];
	[spinInPlaceItem setTag:PokemonSpinInPlaceTag];
	[moreScriptsMenu addItem:spinInPlaceItem];

	NSMenuItem *statusItem = [[[NSMenuItem alloc] initWithTitle:@"Pokemon HTTP API: Disabled"
	                                                     action:nil
	                                              keyEquivalent:@""] autorelease];
	[statusItem setTag:PokemonHttpExportStatusTag];
	[statusItem setEnabled:NO];

	[pokemonMenu addItem:copyExportItem];
	[pokemonMenu addItem:partyEditorItem];
	[pokemonMenu addItem:partyStatusEditorItem];
	[pokemonMenu addItem:[NSMenuItem separatorItem]];
	[pokemonMenu addItem:profileMenuItem];
	[pokemonMenu addItem:platinumTimeMenuItem];
	[pokemonMenu addItem:platinumTrainerDebugMenuItem];
	[pokemonMenu addItem:[NSMenuItem separatorItem]];
	[pokemonMenu addItem:toggleItem];
	[pokemonMenu addItem:battleLoggingToggleItem];
	[pokemonMenu addItem:oneFrameTouchGuardItem];
	[pokemonMenu addItem:statusItem];
	[pokemonMenu addItem:[NSMenuItem separatorItem]];
	[pokemonMenu addItem:moreScriptsMenuItem];
	[pokemonMenu addItem:[NSMenuItem separatorItem]];
	[pokemonMenu addItem:maintenanceMenuItem];

	NSMenuItem *insertAfterItem = nil;
	for (NSMenuItem *item in [mainMenu itemArray])
	{
		if ([[item title] isEqualToString:@"Tools"])
		{
			insertAfterItem = item;
			break;
		}
	}

	const NSInteger insertIndex = (insertAfterItem != nil)
		? ([mainMenu indexOfItem:insertAfterItem] + 1)
		: [mainMenu numberOfItems];
	[mainMenu insertItem:pokemonMenuRootItem atIndex:insertIndex];
}

- (void)updatePokemonHttpApiMenuItems
{
	RefreshPokemonPlatinumTrainerBattleOverrideHook();
	NSMenu *mainMenu = [NSApp mainMenu];
	NSMenuItem *toggleItem = FindMenuItemByTag(mainMenu, PokemonHttpExportToggleTag);
	NSMenuItem *battleLoggingToggleItem = FindMenuItemByTag(mainMenu, PokemonBattleLoggingToggleTag);
	NSMenuItem *oneFrameTouchGuardItem = FindMenuItemByTag(mainMenu, PokemonOneFrameTouchGuardTag);
	NSMenuItem *statusItem = FindMenuItemByTag(mainMenu, PokemonHttpExportStatusTag);
	NSMenuItem *platinumProfileItem = FindMenuItemByTag(mainMenu, PokemonHttpExportProfilePlatinumTag);
	NSMenuItem *platinumKaizoProfileItem = FindMenuItemByTag(mainMenu, PokemonHttpExportProfilePlatinumKaizoTag);
	NSMenuItem *renegadePlatinumProfileItem = FindMenuItemByTag(mainMenu, PokemonHttpExportProfileRenegadePlatinumTag);
	NSMenuItem *hgssProfileItem = FindMenuItemByTag(mainMenu, PokemonHttpExportProfileHgssTag);
	NSMenuItem *sterlingSilverProfileItem = FindMenuItemByTag(mainMenu, PokemonHttpExportProfileSterlingSilverTag);
	NSMenuItem *blackWhiteProfileItem = FindMenuItemByTag(mainMenu, PokemonHttpExportProfileBlackWhiteTag);
	NSMenuItem *black2White2ProfileItem = FindMenuItemByTag(mainMenu, PokemonHttpExportProfileBlack2White2Tag);
	NSMenuItem *bb2Vw2ReduxProfileItem = FindMenuItemByTag(mainMenu, PokemonHttpExportProfileBb2Vw2ReduxTag);
	NSMenuItem *platinumTimeAutoItem = FindMenuItemByTag(mainMenu, PokemonPlatinumTimeAutoTag);
	NSMenuItem *platinumTimeMorningItem = FindMenuItemByTag(mainMenu, PokemonPlatinumTimeMorningTag);
	NSMenuItem *platinumTimeDayItem = FindMenuItemByTag(mainMenu, PokemonPlatinumTimeDayTag);
	NSMenuItem *platinumTimeEveningItem = FindMenuItemByTag(mainMenu, PokemonPlatinumTimeEveningTag);
	NSMenuItem *platinumTimeNightItem = FindMenuItemByTag(mainMenu, PokemonPlatinumTimeNightTag);
	NSMenuItem *platinumTimeLateNightItem = FindMenuItemByTag(mainMenu, PokemonPlatinumTimeLateNightTag);
	NSMenuItem *forceRefreshItem = FindMenuItemByTag(mainMenu, PokemonHttpExportForceRefreshTag);
	NSMenuItem *healthCheckToggleItem = FindMenuItemByTag(mainMenu, PokemonHttpExportHealthCheckToggleTag);
	NSMenuItem *healthCheck15sItem = FindMenuItemByTag(mainMenu, PokemonHttpExportHealthCheckInterval15sTag);
	NSMenuItem *healthCheck30sItem = FindMenuItemByTag(mainMenu, PokemonHttpExportHealthCheckInterval30sTag);
	NSMenuItem *healthCheck60sItem = FindMenuItemByTag(mainMenu, PokemonHttpExportHealthCheckInterval60sTag);
	NSMenuItem *healthCheck120sItem = FindMenuItemByTag(mainMenu, PokemonHttpExportHealthCheckInterval120sTag);
	NSMenuItem *healthCheck300sItem = FindMenuItemByTag(mainMenu, PokemonHttpExportHealthCheckInterval300sTag);
	NSMenuItem *idleRefreshToggleItem = FindMenuItemByTag(mainMenu, PokemonHttpExportIdleRefreshToggleTag);
	NSMenuItem *idleRefresh60sItem = FindMenuItemByTag(mainMenu, PokemonHttpExportIdleRefreshInterval60sTag);
	NSMenuItem *idleRefresh180sItem = FindMenuItemByTag(mainMenu, PokemonHttpExportIdleRefreshInterval180sTag);
	NSMenuItem *idleRefresh300sItem = FindMenuItemByTag(mainMenu, PokemonHttpExportIdleRefreshInterval300sTag);
	NSMenuItem *idleRefresh600sItem = FindMenuItemByTag(mainMenu, PokemonHttpExportIdleRefreshInterval600sTag);
	NSMenuItem *clipboardAutoCopyToggleItem = FindMenuItemByTag(mainMenu, PokemonClipboardAutoCopyToggleTag);
	NSMenuItem *clipboardAutoCopy5sItem = FindMenuItemByTag(mainMenu, PokemonClipboardAutoCopyInterval5sTag);
	NSMenuItem *clipboardAutoCopy10sItem = FindMenuItemByTag(mainMenu, PokemonClipboardAutoCopyInterval10sTag);
	NSMenuItem *clipboardAutoCopy30sItem = FindMenuItemByTag(mainMenu, PokemonClipboardAutoCopyInterval30sTag);
	NSMenuItem *clipboardAutoCopy60sItem = FindMenuItemByTag(mainMenu, PokemonClipboardAutoCopyInterval60sTag);
	NSMenuItem *displayPartyAbilitySlotsItem = FindMenuItemByTag(mainMenu, PokemonDisplayPartyAbilitySlotsTag);
	NSMenuItem *spinInPlaceItem = FindMenuItemByTag(mainMenu, PokemonSpinInPlaceTag);
	NSMenuItem *platinumTrainerOverrideStatusItem = FindMenuItemByTag(mainMenu, PokemonPlatinumTrainerBattleOverrideStatusTag);
	NSMenuItem *platinumTrainerOverrideSetItem = FindMenuItemByTag(mainMenu, PokemonPlatinumTrainerBattleOverrideSetTag);
	NSMenuItem *platinumTrainerOverrideClearItem = FindMenuItemByTag(mainMenu, PokemonPlatinumTrainerBattleOverrideClearTag);
	if (toggleItem == nil || battleLoggingToggleItem == nil || oneFrameTouchGuardItem == nil || statusItem == nil || platinumProfileItem == nil || platinumKaizoProfileItem == nil || renegadePlatinumProfileItem == nil || hgssProfileItem == nil || sterlingSilverProfileItem == nil || blackWhiteProfileItem == nil || black2White2ProfileItem == nil || bb2Vw2ReduxProfileItem == nil || platinumTimeAutoItem == nil || platinumTimeMorningItem == nil || platinumTimeDayItem == nil || platinumTimeEveningItem == nil || platinumTimeNightItem == nil || platinumTimeLateNightItem == nil || forceRefreshItem == nil || healthCheckToggleItem == nil || healthCheck15sItem == nil || healthCheck30sItem == nil || healthCheck60sItem == nil || healthCheck120sItem == nil || healthCheck300sItem == nil || idleRefreshToggleItem == nil || idleRefresh60sItem == nil || idleRefresh180sItem == nil || idleRefresh300sItem == nil || idleRefresh600sItem == nil || clipboardAutoCopyToggleItem == nil || clipboardAutoCopy5sItem == nil || clipboardAutoCopy10sItem == nil || clipboardAutoCopy30sItem == nil || clipboardAutoCopy60sItem == nil || displayPartyAbilitySlotsItem == nil || spinInPlaceItem == nil || platinumTrainerOverrideStatusItem == nil || platinumTrainerOverrideSetItem == nil || platinumTrainerOverrideClearItem == nil)
	{
		return;
	}

	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	NSString *toggleTitle = status.enabled ? @"Disable Pokemon HTTP API" : @"Enable Pokemon HTTP API";
	NSString *battleLoggingToggleTitle = status.battleLoggingEnabled ? @"Disable Battle Logging" : @"Enable Battle Logging";
	NSString *statusTitle = @"Pokemon HTTP API: Disabled";

	if (status.enabled)
	{
		if (!status.running && !status.lastError.empty())
		{
			statusTitle = [NSString stringWithFormat:@"Pokemon HTTP API: Error - %s", status.lastError.c_str()];
		}
		else if (!status.running)
		{
			statusTitle = @"Pokemon HTTP API: Starting";
		}
		else if (!status.romLoaded)
		{
			statusTitle = @"Pokemon HTTP API: Running (no ROM loaded)";
		}
		else if (!status.supportedGame)
		{
			statusTitle = @"Pokemon HTTP API: Running (select export game)";
		}
		else if (!status.lastError.empty())
		{
			statusTitle = [NSString stringWithFormat:@"Pokemon HTTP API: Error - %s", status.lastError.c_str()];
		}
		else
		{
			statusTitle = [NSString stringWithFormat:@"Pokemon HTTP API: %s", status.url.c_str()];
		}
	}

	[toggleItem setTitle:toggleTitle];
	[toggleItem setState:status.enabled ? GUI_STATE_ON : GUI_STATE_OFF];
	[battleLoggingToggleItem setTitle:battleLoggingToggleTitle];
	[battleLoggingToggleItem setState:status.battleLoggingEnabled ? GUI_STATE_ON : GUI_STATE_OFF];
	[oneFrameTouchGuardItem setState:NDS_isOneFrameTouchPressGuardEnabled() ? GUI_STATE_ON : GUI_STATE_OFF];
	[statusItem setTitle:statusTitle];
	[statusItem setEnabled:NO];
	[platinumProfileItem setState:(desmume_pokemon_http_export::GetSelectedProfile() == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_PLATINUM) ? GUI_STATE_ON : GUI_STATE_OFF];
	[platinumKaizoProfileItem setState:(desmume_pokemon_http_export::GetSelectedProfile() == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_PLATINUM_KAIZO) ? GUI_STATE_ON : GUI_STATE_OFF];
	[renegadePlatinumProfileItem setState:(desmume_pokemon_http_export::GetSelectedProfile() == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_RENEGADE_PLATINUM) ? GUI_STATE_ON : GUI_STATE_OFF];
	[hgssProfileItem setState:(desmume_pokemon_http_export::GetSelectedProfile() == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER) ? GUI_STATE_ON : GUI_STATE_OFF];
	[sterlingSilverProfileItem setState:(desmume_pokemon_http_export::GetSelectedProfile() == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_STERLING_SILVER) ? GUI_STATE_ON : GUI_STATE_OFF];
	[blackWhiteProfileItem setState:(desmume_pokemon_http_export::GetSelectedProfile() == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK_WHITE) ? GUI_STATE_ON : GUI_STATE_OFF];
	[black2White2ProfileItem setState:(desmume_pokemon_http_export::GetSelectedProfile() == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK2_WHITE2) ? GUI_STATE_ON : GUI_STATE_OFF];
	[bb2Vw2ReduxProfileItem setState:(desmume_pokemon_http_export::GetSelectedProfile() == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX) ? GUI_STATE_ON : GUI_STATE_OFF];
	const desmume_pokemon_http_export::PlatinumTimeOverride timeOverride = desmume_pokemon_http_export::GetPlatinumTimeOverride();
	[platinumTimeAutoItem setState:(timeOverride == desmume_pokemon_http_export::PLATINUM_TIME_AUTO) ? GUI_STATE_ON : GUI_STATE_OFF];
	[platinumTimeMorningItem setState:(timeOverride == desmume_pokemon_http_export::PLATINUM_TIME_MORNING) ? GUI_STATE_ON : GUI_STATE_OFF];
	[platinumTimeDayItem setState:(timeOverride == desmume_pokemon_http_export::PLATINUM_TIME_DAY) ? GUI_STATE_ON : GUI_STATE_OFF];
	[platinumTimeEveningItem setState:(timeOverride == desmume_pokemon_http_export::PLATINUM_TIME_EVENING) ? GUI_STATE_ON : GUI_STATE_OFF];
	[platinumTimeNightItem setState:(timeOverride == desmume_pokemon_http_export::PLATINUM_TIME_NIGHT) ? GUI_STATE_ON : GUI_STATE_OFF];
	[platinumTimeLateNightItem setState:(timeOverride == desmume_pokemon_http_export::PLATINUM_TIME_LATE_NIGHT) ? GUI_STATE_ON : GUI_STATE_OFF];
	[forceRefreshItem setEnabled:status.enabled ? YES : NO];
	[healthCheckToggleItem setState:status.healthCheckEnabled ? GUI_STATE_ON : GUI_STATE_OFF];
	[healthCheck15sItem setState:(status.healthCheckIntervalMs == 15000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[healthCheck30sItem setState:(status.healthCheckIntervalMs == 30000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[healthCheck60sItem setState:(status.healthCheckIntervalMs == 60000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[healthCheck120sItem setState:(status.healthCheckIntervalMs == 120000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[healthCheck300sItem setState:(status.healthCheckIntervalMs == 300000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[idleRefreshToggleItem setState:status.idleRefreshEnabled ? GUI_STATE_ON : GUI_STATE_OFF];
	[idleRefresh60sItem setState:(status.idleRefreshIntervalMs == 60000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[idleRefresh180sItem setState:(status.idleRefreshIntervalMs == 180000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[idleRefresh300sItem setState:(status.idleRefreshIntervalMs == 300000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[idleRefresh600sItem setState:(status.idleRefreshIntervalMs == 600000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[clipboardAutoCopyToggleItem setState:pokemonClipboardAutoCopyEnabled ? GUI_STATE_ON : GUI_STATE_OFF];
	[clipboardAutoCopy5sItem setState:(pokemonClipboardAutoCopyIntervalMs == 5000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[clipboardAutoCopy10sItem setState:(pokemonClipboardAutoCopyIntervalMs == 10000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[clipboardAutoCopy30sItem setState:(pokemonClipboardAutoCopyIntervalMs == 30000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[clipboardAutoCopy60sItem setState:(pokemonClipboardAutoCopyIntervalMs == 60000u) ? GUI_STATE_ON : GUI_STATE_OFF];
	[clipboardAutoCopy5sItem setEnabled:pokemonClipboardAutoCopyEnabled ? YES : NO];
	[clipboardAutoCopy10sItem setEnabled:pokemonClipboardAutoCopyEnabled ? YES : NO];
	[clipboardAutoCopy30sItem setEnabled:pokemonClipboardAutoCopyEnabled ? YES : NO];
	[clipboardAutoCopy60sItem setEnabled:pokemonClipboardAutoCopyEnabled ? YES : NO];
	[displayPartyAbilitySlotsItem setState:PokemonMoreScripts_IsDisplayPartyAbilitySlotsEnabled() ? GUI_STATE_ON : GUI_STATE_OFF];
	[spinInPlaceItem setState:PokemonMoreScripts_IsSpinInPlaceEnabled() ? GUI_STATE_ON : GUI_STATE_OFF];
	NSString *platinumTrainerStatus = gPokemonPlatinumTrainerBattleOverride.hasPendingTrainerId
		? [NSString stringWithFormat:@"Next trainer override: %u", (unsigned int)gPokemonPlatinumTrainerBattleOverride.pendingTrainerId]
		: @"Next trainer override: None";
	[platinumTrainerOverrideStatusItem setTitle:platinumTrainerStatus];
	[platinumTrainerOverrideStatusItem setEnabled:NO];
	[platinumTrainerOverrideSetItem setEnabled:IsPokemonPlatinumTrainerOverrideAvailable() ? YES : NO];
	[platinumTrainerOverrideClearItem setEnabled:gPokemonPlatinumTrainerBattleOverride.hasPendingTrainerId ? YES : NO];
}

#pragma mark IBActions

- (IBAction) newDisplayWindow:(id)sender
{
	DisplayWindowController *newWindowController = [[DisplayWindowController alloc] initWithWindowNibName:@"DisplayWindow" emuControlDelegate:self];
	
	[newWindowController window]; // Just reference the window to force the nib to load.
	[[newWindowController view] setAllowViewUpdates:YES];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	const BOOL useVerticalSync = ([[newWindowController view] layer] != nil) || [cdsCore isFrameSkipEnabled] || (([cdsCore speedScalar] < 1.03f) && [cdsCore isSpeedLimitEnabled]);
	[[newWindowController view] setUseVerticalSync:useVerticalSync];
	
	MacDisplayViewOutput *dvo = [[newWindowController view] displayViewOutput];
	dvo->Dispatch(MESSAGE_RELOAD_REPROCESS_REDRAW);
	[[newWindowController window] makeKeyAndOrderFront:self];
	[[newWindowController window] makeMainWindow];
}

- (IBAction) openRom:(id)sender
{
	if ([self isRomLoading])
	{
		return;
	}
	
	NSURL *selectedFile = nil;
	NSInteger buttonClicked = 0;
	
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	[panel setCanChooseDirectories:NO];
	[panel setCanChooseFiles:YES];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setTitle:NSSTRING_TITLE_OPEN_ROM_PANEL];
	NSArray *fileTypes = [NSArray arrayWithObjects:@FILE_EXT_ROM_DS, @FILE_EXT_ROM_GBA, nil]; 
	
	// While [NSOpenPanel setAllowedFileTypes:] and [NSOpenPanel runModal]
	// are available in Leopard, the allowedFileTypes property is ignored on
	// that version of macOS. To maintain compatibility with Leopard, we need
	// to call the deprecated method [NSOpenPanel runModalForDirectory:file:types]
	// instead.
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
	if (IsOSXVersionSupported(10, 6, 0))
	{
		[panel setAllowedFileTypes:fileTypes];
		buttonClicked = [panel runModal];
	}
	else
#endif
	{
		SILENCE_DEPRECATION_MACOS_10_6( buttonClicked = [panel runModalForDirectory:nil file:nil types:fileTypes] );
	}
	
	if (buttonClicked == GUI_RESPONSE_OK)
	{
		selectedFile = [[panel URLs] lastObject];
		if(selectedFile == nil)
		{
			return;
		}
		
		[self handleLoadRomByURL:selectedFile];
	}
}

- (IBAction) loadRecentRom:(id)sender
{
	// Dummy selector, used for UI validation only.
}

- (IBAction) closeRom:(id)sender
{
	[self handleUnloadRom:REASONFORCLOSE_NORMAL romToLoad:nil];
}

- (IBAction) revealRomInFinder:(id)sender
{
	NSURL *romURL = [[self currentRom] fileURL];
	
	if (romURL != nil)
	{
		[[NSWorkspace sharedWorkspace] selectFile:[romURL path] inFileViewerRootedAtPath:@""];
	}
}

- (IBAction) revealGameDataFolderInFinder:(id)sender
{
	NSURL *folderURL = [CocoaDSFile userAppSupportURL:nil version:nil];
	[[NSWorkspace sharedWorkspace] selectFile:[folderURL path] inFileViewerRootedAtPath:@""];
}

- (IBAction) openEmuSaveState:(id)sender
{
	NSURL *selectedFile = nil;
	NSInteger buttonClicked = 0;
	
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	[panel setCanChooseDirectories:NO];
	[panel setCanChooseFiles:YES];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setTitle:NSSTRING_TITLE_OPEN_STATE_FILE_PANEL];
	NSArray *fileTypes = [NSArray arrayWithObjects:@FILE_EXT_SAVE_STATE, nil];
	
	// While [NSOpenPanel setAllowedFileTypes:] and [NSOpenPanel runModal]
	// are available in Leopard, the allowedFileTypes property is ignored on
	// that version of macOS. To maintain compatibility with Leopard, we need
	// to call the deprecated method [NSOpenPanel runModalForDirectory:file:types]
	// instead.
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
	if (IsOSXVersionSupported(10, 6, 0))
	{
		[panel setAllowedFileTypes:fileTypes];
		buttonClicked = [panel runModal];
	}
	else
#endif
	{
		SILENCE_DEPRECATION_MACOS_10_6( buttonClicked = [panel runModalForDirectory:nil file:nil types:fileTypes] );
	}
	
	if (buttonClicked == GUI_RESPONSE_OK)
	{
		selectedFile = [[panel URLs] lastObject];
		if(selectedFile == nil)
		{
			return;
		}
	}
	else
	{
		return;
	}
	
	[self pauseCore];
	
	const BOOL isStateLoaded = [CocoaDSFile loadState:selectedFile];
	if (!isStateLoaded)
	{
		[self setStatusText:NSSTRING_STATUS_SAVESTATE_LOADING_FAILED];
		[self restoreCoreState];
		return;
	}
	
	isSaveStateEdited = YES;
	for (DisplayWindowController *windowController in windowList)
	{
		[[windowController window] setDocumentEdited:isSaveStateEdited];
	}
	
	[self setStatusText:NSSTRING_STATUS_SAVESTATE_LOADED];
	[self restoreCoreState];
	
	[self setCurrentSaveStateURL:selectedFile];
}

- (IBAction) saveEmuSaveState:(id)sender
{
	if (isSaveStateEdited && [self currentSaveStateURL] != nil)
	{
		[self pauseCore];
		
		const BOOL isStateSaved = [CocoaDSFile saveState:[self currentSaveStateURL]];
		if (!isStateSaved)
		{
			[self setStatusText:NSSTRING_STATUS_SAVESTATE_SAVING_FAILED];
			return;
		}
		
		isSaveStateEdited = YES;
		for (DisplayWindowController *windowController in windowList)
		{
			[[windowController window] setDocumentEdited:isSaveStateEdited];
		}
		
		[self setStatusText:NSSTRING_STATUS_SAVESTATE_SAVED];
		[self restoreCoreState];
	}
	else
	{
		[self saveEmuSaveStateAs:sender];
	}
}

- (IBAction) saveEmuSaveStateAs:(id)sender
{
	NSSavePanel *panel = [NSSavePanel savePanel];
	[panel setCanCreateDirectories:YES];
	[panel setTitle:NSSTRING_TITLE_SAVE_STATE_FILE_PANEL];
	
	// While [NSSavePanel setAllowedFileTypes:] is available in Leopard, the
	// allowedFileTypes property is ignored on that version of macOS. To maintain
	// compatibility with Leopard, we need to call the deprecated method
	// [NSSavePanel setRequiredFileType:] instead.
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
	if (IsOSXVersionSupported(10, 6, 0))
	{
		NSArray *fileTypes = [NSArray arrayWithObjects:@FILE_EXT_SAVE_STATE, nil];
		[panel setAllowedFileTypes:fileTypes];
	}
	else
#endif
	{
		SILENCE_DEPRECATION_MACOS_10_6( [panel setRequiredFileType:@FILE_EXT_SAVE_STATE] );
	}
	
	const NSInteger buttonClicked = [panel runModal];
	if (buttonClicked == GUI_RESPONSE_OK)
	{
		NSURL *saveFileURL = [panel URL];
		
		[self pauseCore];
		
		const BOOL isStateSaved = [CocoaDSFile saveState:saveFileURL];
		if (!isStateSaved)
		{
			[self setStatusText:NSSTRING_STATUS_SAVESTATE_SAVING_FAILED];
			return;
		}
		
		isSaveStateEdited = YES;
		for (DisplayWindowController *windowController in windowList)
		{
			[[windowController window] setDocumentEdited:isSaveStateEdited];
		}
		
		[self setStatusText:NSSTRING_STATUS_SAVESTATE_SAVED];
		[self restoreCoreState];
		
		[self setCurrentSaveStateURL:saveFileURL];
	}
}

- (IBAction) revertEmuSaveState:(id)sender
{
	if (!isSaveStateEdited || [self currentSaveStateURL] == nil)
	{
		return;
	}
	
	[self pauseCore];
	
	const BOOL isStateLoaded = [CocoaDSFile loadState:[self currentSaveStateURL]];
	if (!isStateLoaded)
	{
		[self setStatusText:NSSTRING_STATUS_SAVESTATE_REVERTING_FAILED];
		return;
	}
	
	isSaveStateEdited = YES;
	for (DisplayWindowController *windowController in windowList)
	{
		[[windowController window] setDocumentEdited:isSaveStateEdited];
	}
	
	[self setStatusText:NSSTRING_STATUS_SAVESTATE_REVERTED];
	[self restoreCoreState];
}

- (IBAction) toggleLoadStateWritesBackupSave:(id)sender
{
	CommonSettings.loadStateWritesBackupSave = !CommonSettings.loadStateWritesBackupSave;
	[[NSUserDefaults standardUserDefaults] setBool:CommonSettings.loadStateWritesBackupSave forKey:LoadStateWritesBackupSaveDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction) loadEmuSaveStateSlot:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) saveEmuSaveStateSlot:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) openReplay:(id)sender
{
	NSURL *selectedFile = nil;
	NSInteger buttonClicked = 0;
	
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	[panel setCanChooseDirectories:NO];
	[panel setCanChooseFiles:YES];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setTitle:@"Load Replay"];
	NSArray *fileTypes = [NSArray arrayWithObjects:@"dsm", nil];
	
	// While [NSOpenPanel setAllowedFileTypes:] and [NSOpenPanel runModal]
	// are available in Leopard, the allowedFileTypes property is ignored on
	// that version of macOS. To maintain compatibility with Leopard, we need
	// to call the deprecated method [NSOpenPanel runModalForDirectory:file:types]
	// instead.
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
	if (IsOSXVersionSupported(10, 6, 0))
	{
		[panel setAllowedFileTypes:fileTypes];
		buttonClicked = [panel runModal];
	}
	else
#endif
	{
		SILENCE_DEPRECATION_MACOS_10_6( buttonClicked = [panel runModalForDirectory:nil file:nil types:fileTypes] );
	}
	
	if (buttonClicked == GUI_RESPONSE_OK)
	{
		selectedFile = [[panel URLs] lastObject];
		if(selectedFile == nil)
		{
			return;
		}
		
		[self pauseCore];
		const BOOL isMovieLoaded = [CocoaDSFile loadReplay:selectedFile];
		[self setStatusText:(isMovieLoaded) ? @"Replay loaded successfully." : @"Replay loading failed!"];
		[self restoreCoreState];
	}
}

- (IBAction) recordReplay:(id)sender
{
	NSSavePanel *panel = [NSSavePanel savePanel];
	[panel setCanCreateDirectories:YES];
	[panel setTitle:@"Record Replay"];
	
	// While [NSSavePanel setAllowedFileTypes:] is available in Leopard, the
	// allowedFileTypes property is ignored on that version of macOS. To maintain
	// compatibility with Leopard, we need to call the deprecated method
	// [NSSavePanel setRequiredFileType:] instead.
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
	if (IsOSXVersionSupported(10, 6, 0))
	{
		NSArray *fileTypes = [NSArray arrayWithObjects:@"dsm", nil];
		[panel setAllowedFileTypes:fileTypes];
	}
	else
#endif
	{
		SILENCE_DEPRECATION_MACOS_10_6( [panel setRequiredFileType:@"dsm"] );
	}
	
	const NSInteger buttonClicked = [panel runModal];
	if (buttonClicked == GUI_RESPONSE_OK)
	{
		NSURL *fileURL = [panel URL];
		if(fileURL == nil)
		{
			return;
		}
		
		[self pauseCore];
		CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
		NSURL *sramURL = [CocoaDSFile fileURLFromRomURL:[[self currentRom] fileURL] toKind:@"ROM Save"];
		
		//NSFileManager *fileManager = [[NSFileManager alloc] init];
		//const BOOL exists = [fileManager isReadableFileAtPath:[sramURL path]];
		//[fileManager release];
		
		const BOOL isMovieStarted = [cdsCore startReplayRecording:fileURL sramURL:sramURL];
		[self setStatusText:(isMovieStarted) ? @"Replay recording started." : @"Replay creation failed!"];
		[self restoreCoreState];
	}
}

- (IBAction) stopReplay:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	[self pauseCore];
	[cdsCore stopReplay];
	[self setStatusText:@"Replay stopped."];
	[self restoreCoreState];
}

- (IBAction) importRomSave:(id)sender
{
	NSURL *selectedFile = nil;
	NSInteger buttonClicked = 0;
	
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	[panel setCanChooseDirectories:NO];
	[panel setCanChooseFiles:YES];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setTitle:NSSTRING_TITLE_IMPORT_ROM_SAVE_PANEL];
	NSArray *fileTypes = [NSArray arrayWithObjects:
						  @FILE_EXT_ROM_SAVE,
						  @FILE_EXT_ROM_SAVE_RAW,
						  @FILE_EXT_ACTION_REPLAY_SAVE,
						  @FILE_EXT_ACTION_REPLAY_MAX_SAVE,
						  nil];
	
	[self pauseCore];
	
	// While [NSOpenPanel setAllowedFileTypes:] and [NSOpenPanel runModal]
	// are available in Leopard, the allowedFileTypes property is ignored on
	// that version of macOS. To maintain compatibility with Leopard, we need
	// to call the deprecated method [NSOpenPanel runModalForDirectory:file:types]
	// instead.
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
	if (IsOSXVersionSupported(10, 6, 0))
	{
		[panel setAllowedFileTypes:fileTypes];
		buttonClicked = [panel runModal];
	}
	else
#endif
	{
		SILENCE_DEPRECATION_MACOS_10_6( buttonClicked = [panel runModalForDirectory:nil file:nil types:fileTypes] );
	}
	
	if (buttonClicked == GUI_RESPONSE_OK)
	{
		selectedFile = [[panel URLs] lastObject];
		if(selectedFile == nil)
		{
			return;
		}
		
		const BOOL isRomSaveImported = [CocoaDSFile importRomSave:selectedFile];
		[self setStatusText:(isRomSaveImported) ? NSSTRING_STATUS_ROM_SAVE_IMPORTED : NSSTRING_STATUS_ROM_SAVE_IMPORT_FAILED];
	}
	
	[self restoreCoreState];
}

- (IBAction) exportRomSave:(id)sender
{
	NSSavePanel *panel = [NSSavePanel savePanel];
	[panel setTitle:NSSTRING_TITLE_EXPORT_ROM_SAVE_PANEL];
	[panel setCanCreateDirectories:YES];
	[panel setAccessoryView:exportRomSavePanelAccessoryView];
	
	[self pauseCore];
	
	const NSInteger buttonClicked = [panel runModal];
	if (buttonClicked == GUI_RESPONSE_OK)
	{
		NSURL *romSaveURL = [CocoaDSFile fileURLFromRomURL:[[self currentRom] fileURL] toKind:@"ROM Save"];
		if (romSaveURL != nil)
		{
			const BOOL isRomSaveExported = [CocoaDSFile exportRomSaveToURL:[panel URL] romSaveURL:romSaveURL fileType:[self selectedExportRomSaveID]];
			[self setStatusText:(isRomSaveExported) ? NSSTRING_STATUS_ROM_SAVE_EXPORTED : NSSTRING_STATUS_ROM_SAVE_EXPORT_FAILED];
		}
	}
	
	[self restoreCoreState];
}

- (IBAction) openCheatDatabaseFile:(id)sender
{
	CheatDatabaseWindowController *newWindowController = [[CheatDatabaseWindowController alloc] initWithWindowNibName:@"CheatDatabaseViewer" delegate:cheatWindowDelegate];
	[newWindowController window]; // Just reference the window to force the NSWindow object to load.
	
	[[newWindowController window] makeKeyAndOrderFront:sender];
	[[newWindowController window] makeMainWindow];
	[newWindowController openFile:sender];
}

- (IBAction) clearCheatDatabaseRecents:(id)sender
{
	NSArray *menuItemList = [cheatDatabaseRecentsMenu itemArray];
	
	for (NSMenuItem *menuItem in menuItemList)
	{
		if ( ([menuItem action] == @selector(openRecentCheatDatabase:)) || [menuItem isSeparatorItem] )
		{
			[cheatDatabaseRecentsMenu removeItem:menuItem];
		}
	}
	
	NSArray *emptyArray = [[[NSArray alloc] init] autorelease];
	[[NSUserDefaults standardUserDefaults] setObject:emptyArray forKey:@"CheatDatabase_RecentFilePath"];
	
	// Also remove the legacy setting.
	[[NSUserDefaults standardUserDefaults] removeObjectForKey:@"R4Cheat_DatabasePath"];
}

- (IBAction) togglePokemonHttpApi:(id)sender
{
	const BOOL enabled = !desmume_pokemon_http_export::IsEnabled();
	desmume_pokemon_http_export::SetEnabled(enabled);
	[[NSUserDefaults standardUserDefaults] setBool:enabled forKey:PokemonHttpExportDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];

	[self updatePokemonHttpApiMenuItems];

	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	NSString *statusString = [NSString stringWithUTF8String:status.statusText.c_str()];
	if (statusString != nil)
	{
		[self setStatusText:statusString];
	}
}

- (IBAction) togglePokemonBattleLogging:(id)sender
{
	const BOOL enabled = !desmume_pokemon_http_export::IsBattleLoggingEnabled();
	desmume_pokemon_http_export::SetBattleLoggingEnabled(enabled);
	[[NSUserDefaults standardUserDefaults] setBool:enabled forKey:PokemonBattleLoggingDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];

	[self updatePokemonHttpApiMenuItems];
	[self setStatusText:enabled ? @"Battle logging enabled" : @"Battle logging disabled"];
}

- (IBAction) togglePokemonOneFrameTouchGuard:(id)sender
{
	const BOOL enabled = !NDS_isOneFrameTouchPressGuardEnabled();
	NDS_setOneFrameTouchPressGuardEnabled(enabled ? true : false);
	[[NSUserDefaults standardUserDefaults] setBool:enabled forKey:PokemonOneFrameTouchGuardDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];

	[self updatePokemonHttpApiMenuItems];
	[self setStatusText:enabled ? @"Touch tap guard enabled" : @"Touch tap guard disabled"];
}

- (IBAction) togglePokemonDisplayPartyAbilitySlots:(id)sender
{
	const BOOL enabled = !PokemonMoreScripts_IsDisplayPartyAbilitySlotsEnabled();
	PokemonMoreScripts_SetDisplayPartyAbilitySlotsEnabled(enabled ? true : false);
	[[NSUserDefaults standardUserDefaults] setBool:enabled forKey:PokemonDisplayPartyAbilitySlotsDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];

	for (DisplayWindowController *windowController in windowList)
	{
		MacDisplayViewOutput *dvo = [[windowController view] displayViewOutput];
		dvo->UpdateHUD();
		dvo->SetViewNeedsFlush();
	}

	[self updatePokemonHttpApiMenuItems];
}

- (IBAction) togglePokemonSpinInPlace:(id)sender
{
	const BOOL enabled = !PokemonMoreScripts_IsSpinInPlaceEnabled();
	PokemonMoreScripts_SetSpinInPlaceEnabled(enabled ? true : false);
	[[NSUserDefaults standardUserDefaults] setBool:enabled forKey:PokemonSpinInPlaceDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self updatePokemonHttpApiMenuItems];
}

- (IBAction) forcePokemonHttpApiRefresh:(id)sender
{
	desmume_pokemon_http_export::ForceRefresh();
	[self updatePokemonHttpApiMenuItems];

	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	NSString *statusString = [NSString stringWithUTF8String:status.statusText.c_str()];
	if (statusString != nil)
	{
		[self setStatusText:statusString];
	}
}

- (IBAction) togglePokemonHttpHealthCheck:(id)sender
{
	const BOOL enabled = !desmume_pokemon_http_export::IsHealthCheckEnabled();
	desmume_pokemon_http_export::SetHealthCheckEnabled(enabled);
	[[NSUserDefaults standardUserDefaults] setBool:enabled forKey:PokemonHttpExportHealthCheckEnabledDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self updatePokemonHttpApiMenuItems];
}

- (IBAction) selectPokemonHttpHealthCheckInterval:(id)sender
{
	const NSInteger senderTag = [CocoaDSUtil getIBActionSenderTag:sender];
	const uint32_t intervalMs = PokemonHttpHealthCheckIntervalForTag(senderTag);
	desmume_pokemon_http_export::SetHealthCheckIntervalMs(intervalMs);
	[[NSUserDefaults standardUserDefaults] setInteger:(NSInteger)intervalMs forKey:PokemonHttpExportHealthCheckIntervalDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self updatePokemonHttpApiMenuItems];
}

- (IBAction) togglePokemonHttpIdleRefresh:(id)sender
{
	const BOOL enabled = !desmume_pokemon_http_export::IsIdleRefreshEnabled();
	desmume_pokemon_http_export::SetIdleRefreshEnabled(enabled);
	[[NSUserDefaults standardUserDefaults] setBool:enabled forKey:PokemonHttpExportIdleRefreshEnabledDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self updatePokemonHttpApiMenuItems];
}

- (IBAction) selectPokemonHttpIdleRefreshInterval:(id)sender
{
	const NSInteger senderTag = [CocoaDSUtil getIBActionSenderTag:sender];
	const uint32_t intervalMs = PokemonHttpIdleRefreshIntervalForTag(senderTag);
	desmume_pokemon_http_export::SetIdleRefreshIntervalMs(intervalMs);
	[[NSUserDefaults standardUserDefaults] setInteger:(NSInteger)intervalMs forKey:PokemonHttpExportIdleRefreshIntervalDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self updatePokemonHttpApiMenuItems];
}

- (IBAction) togglePokemonClipboardAutoCopy:(id)sender
{
	pokemonClipboardAutoCopyEnabled = !pokemonClipboardAutoCopyEnabled;
	desmume_pokemon_http_export::SetClipboardSyncEnabled(pokemonClipboardAutoCopyEnabled ? true : false);
	if (!pokemonClipboardAutoCopyEnabled)
	{
		[self resetPokemonClipboardAutoSyncState];
	}
	[[NSUserDefaults standardUserDefaults] setBool:pokemonClipboardAutoCopyEnabled forKey:PokemonClipboardAutoCopyEnabledDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self updatePokemonHttpApiMenuItems];
	[self setStatusText:pokemonClipboardAutoCopyEnabled ? @"Clipboard auto copy enabled" : @"Clipboard auto copy disabled"];
}

- (IBAction) selectPokemonClipboardAutoCopyInterval:(id)sender
{
	const NSInteger senderTag = [CocoaDSUtil getIBActionSenderTag:sender];
	pokemonClipboardAutoCopyIntervalMs = PokemonClipboardAutoCopyIntervalForTag(senderTag);
	[[NSUserDefaults standardUserDefaults] setInteger:(NSInteger)pokemonClipboardAutoCopyIntervalMs forKey:PokemonClipboardAutoCopyIntervalDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self updatePokemonHttpApiMenuItems];
}

- (void)resetPokemonClipboardAutoSyncState
{
	pokemonClipboardLastBattleEndUnixMs = 0u;
	pokemonClipboardLastRefreshCheckUnixMs = 0u;
	[pokemonClipboardLastExportText release];
	pokemonClipboardLastExportText = nil;
}

- (void)notePokemonClipboardCopyWithText:(NSString *)clipboardText status:(const desmume_pokemon_http_export::Status &)status nowMs:(uint64_t)nowMs
{
	pokemonClipboardLastBattleEndUnixMs = status.battleDetection.lastBattleEndUnixMs;
	pokemonClipboardLastRefreshCheckUnixMs = nowMs;
	if ((pokemonClipboardLastExportText == nil) || ![pokemonClipboardLastExportText isEqualToString:clipboardText])
	{
		[pokemonClipboardLastExportText release];
		pokemonClipboardLastExportText = [clipboardText copy];
	}
}

- (void)updatePokemonClipboardAutoSync:(NSTimer *)timer
{
	(void)timer;

	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	if (!pokemonClipboardAutoCopyEnabled || status.enabled || !status.romLoaded)
	{
		[self resetPokemonClipboardAutoSyncState];
		return;
	}

	const bool battleActive = status.battleDetection.battleActive;
	const bool battleEnded = !battleActive &&
		(status.battleDetection.lastBattleEndUnixMs != 0u) &&
		(status.battleDetection.lastBattleEndUnixMs != pokemonClipboardLastBattleEndUnixMs);
	if (battleActive)
	{
		return;
	}

	const uint64_t nowMs = PokemonClipboardCurrentUnixMs();
	if (!battleEnded &&
		(pokemonClipboardLastRefreshCheckUnixMs != 0u) &&
		((nowMs - pokemonClipboardLastRefreshCheckUnixMs) < pokemonClipboardAutoCopyIntervalMs))
	{
		return;
	}

	std::string showdownText;
	std::string error;
	if (!desmume_pokemon_http_export::GetShowdownExportText(showdownText, error))
	{
		return;
	}

	NSString *clipboardError = nil;
	NSString *clipboardText = PokemonClipboardStringFromUtf8(showdownText, &clipboardError);
	if (clipboardText == nil)
	{
		return;
	}

	if (!battleEnded && (pokemonClipboardLastExportText != nil) && [pokemonClipboardLastExportText isEqualToString:clipboardText])
	{
		pokemonClipboardLastRefreshCheckUnixMs = nowMs;
		return;
	}

	if (!WritePokemonClipboardString(clipboardText, &clipboardError))
	{
		return;
	}

	[self notePokemonClipboardCopyWithText:clipboardText status:status nowMs:nowMs];
}

- (IBAction) copyPokemonExportToClipboard:(id)sender
{
	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	std::string showdownText;
	std::string error;
	if (!desmume_pokemon_http_export::GetShowdownExportText(showdownText, error))
	{
		ShowPokemonClipboardCopyError([NSString stringWithUTF8String:error.c_str()]);
		[self setStatusText:@"Pokemon export copy failed."];
		return;
	}

	NSString *clipboardError = nil;
	NSString *clipboardText = PokemonClipboardStringFromUtf8(showdownText, &clipboardError);
	if (clipboardText == nil)
	{
		ShowPokemonClipboardCopyError(clipboardError);
		[self setStatusText:@"Pokemon export copy failed."];
		return;
	}

	if (!WritePokemonClipboardString(clipboardText, &clipboardError))
	{
		ShowPokemonClipboardCopyError(clipboardError);
		[self setStatusText:@"Pokemon export copy failed."];
		return;
	}

	[self notePokemonClipboardCopyWithText:clipboardText status:status nowMs:PokemonClipboardCurrentUnixMs()];
	[self setStatusText:@"Pokemon export copied to clipboard."];
}

- (IBAction) selectPokemonHttpApiProfile:(id)sender
{
	const NSInteger senderTag = [CocoaDSUtil getIBActionSenderTag:sender];
	desmume_pokemon_http_export::ExportGameProfile profile = desmume_pokemon_http_export::EXPORT_GAME_POKEMON_PLATINUM;

	if (senderTag == PokemonHttpExportProfilePlatinumTag)
	{
		profile = desmume_pokemon_http_export::EXPORT_GAME_POKEMON_PLATINUM;
	}
	else if (senderTag == PokemonHttpExportProfilePlatinumKaizoTag)
	{
		profile = desmume_pokemon_http_export::EXPORT_GAME_POKEMON_PLATINUM_KAIZO;
	}
	else if (senderTag == PokemonHttpExportProfileRenegadePlatinumTag)
	{
		profile = desmume_pokemon_http_export::EXPORT_GAME_POKEMON_RENEGADE_PLATINUM;
	}
	else if (senderTag == PokemonHttpExportProfileHgssTag)
	{
		profile = desmume_pokemon_http_export::EXPORT_GAME_POKEMON_HEARTGOLD_SOULSILVER;
	}
	else if (senderTag == PokemonHttpExportProfileSterlingSilverTag)
	{
		profile = desmume_pokemon_http_export::EXPORT_GAME_POKEMON_STERLING_SILVER;
	}
	else if (senderTag == PokemonHttpExportProfileBlackWhiteTag)
	{
		profile = desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK_WHITE;
	}
	else if (senderTag == PokemonHttpExportProfileBlack2White2Tag)
	{
		profile = desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK2_WHITE2;
	}
	else if (senderTag == PokemonHttpExportProfileBb2Vw2ReduxTag)
	{
		profile = desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX;
	}

	desmume_pokemon_http_export::SetSelectedProfile(profile);
	[[NSUserDefaults standardUserDefaults] setInteger:profile forKey:PokemonHttpExportProfileDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];

	[self updatePokemonHttpApiMenuItems];

	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	NSString *statusString = [NSString stringWithUTF8String:status.statusText.c_str()];
	if (statusString != nil)
	{
		[self setStatusText:statusString];
	}
}

- (IBAction) selectPokemonPlatinumTimeOverride:(id)sender
{
	const NSInteger senderTag = [CocoaDSUtil getIBActionSenderTag:sender];
	desmume_pokemon_http_export::PlatinumTimeOverride timeOverride = desmume_pokemon_http_export::PLATINUM_TIME_AUTO;

	if (senderTag == PokemonPlatinumTimeMorningTag)
	{
		timeOverride = desmume_pokemon_http_export::PLATINUM_TIME_MORNING;
	}
	else if (senderTag == PokemonPlatinumTimeDayTag)
	{
		timeOverride = desmume_pokemon_http_export::PLATINUM_TIME_DAY;
	}
	else if (senderTag == PokemonPlatinumTimeEveningTag)
	{
		timeOverride = desmume_pokemon_http_export::PLATINUM_TIME_EVENING;
	}
	else if (senderTag == PokemonPlatinumTimeNightTag)
	{
		timeOverride = desmume_pokemon_http_export::PLATINUM_TIME_NIGHT;
	}
	else if (senderTag == PokemonPlatinumTimeLateNightTag)
	{
		timeOverride = desmume_pokemon_http_export::PLATINUM_TIME_LATE_NIGHT;
	}

	desmume_pokemon_http_export::SetPlatinumTimeOverride(timeOverride);
	[[NSUserDefaults standardUserDefaults] setInteger:timeOverride forKey:PokemonPlatinumTimeOverrideDefaultsKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self updatePokemonHttpApiMenuItems];
}

- (IBAction) setPokemonPlatinumNextTrainerBattleOverride:(id)sender
{
	(void)sender;
	RefreshPokemonPlatinumTrainerBattleOverrideHook();
	if (!IsPokemonPlatinumRev1Loaded())
	{
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert setMessageText:@"Pokemon Platinum trainer override unavailable"];
		[alert setInformativeText:@"Load Pokemon Platinum CPUE/JAK7 rev1 to use this debug tool."];
		[alert runModal];
		return;
	}

	if (CommonSettings.use_jit)
	{
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert setMessageText:@"Pokemon Platinum trainer override requires interpreter mode"];
		[alert setInformativeText:@"Turn off the dynamic recompiler, then try again."];
		[alert runModal];
		return;
	}

	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert setMessageText:@"Set next trainer battle override"];
	[alert setInformativeText:@"Enter the trainer NARC ID to force into the next scripted trainer battle."];
	[alert addButtonWithTitle:@"Set"];
	[alert addButtonWithTitle:@"Cancel"];

	NSTextField *inputField = [[[NSTextField alloc] initWithFrame:NSMakeRect(0.0, 0.0, 240.0, 24.0)] autorelease];
	[inputField setStringValue:gPokemonPlatinumTrainerBattleOverride.hasPendingTrainerId
		? [NSString stringWithFormat:@"%u", (unsigned int)gPokemonPlatinumTrainerBattleOverride.pendingTrainerId]
		: @""];
	[alert setAccessoryView:inputField];
	[[alert window] makeFirstResponder:inputField];

	if ([alert runModal] != NSAlertFirstButtonReturn)
	{
		return;
	}

	NSString *input = [[inputField stringValue] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
	if ([input length] == 0)
	{
		return;
	}

	if ([input rangeOfCharacterFromSet:[[NSCharacterSet decimalDigitCharacterSet] invertedSet]].location != NSNotFound)
	{
		NSAlert *errorAlert = [[[NSAlert alloc] init] autorelease];
		[errorAlert setMessageText:@"Invalid trainer ID"];
		[errorAlert setInformativeText:@"Enter an unsigned integer between 0 and 65535."];
		[errorAlert runModal];
		return;
	}

	char *parseEnd = NULL;
	const unsigned long trainerId = strtoul([input UTF8String], &parseEnd, 10);
	if ((parseEnd == NULL) || (*parseEnd != '\0') || (trainerId > 0xFFFFu))
	{
		NSAlert *errorAlert = [[[NSAlert alloc] init] autorelease];
		[errorAlert setMessageText:@"Invalid trainer ID"];
		[errorAlert setInformativeText:@"Enter an unsigned integer between 0 and 65535."];
		[errorAlert runModal];
		return;
	}

	gPokemonPlatinumTrainerBattleOverride.hasPendingTrainerId = true;
	gPokemonPlatinumTrainerBattleOverride.pendingTrainerId = (uint16_t)trainerId;
	[self updatePokemonHttpApiMenuItems];
	[self setStatusText:[NSString stringWithFormat:@"Next Platinum trainer battle will use trainer ID %u.", (unsigned int)trainerId]];
}

- (IBAction) clearPokemonPlatinumNextTrainerBattleOverride:(id)sender
{
	(void)sender;
	ClearPokemonPlatinumTrainerBattleOverridePendingState();
	[self updatePokemonHttpApiMenuItems];
	[self setStatusText:@"Cleared pending Platinum trainer battle override."];
}

- (IBAction) openPokemonPartyEditor:(id)sender
{
	if (pokemonPartyEditorWindowController == nil)
	{
		pokemonPartyEditorWindowController = [[PokemonPartyEditorWindowController alloc] init];
	}

	[pokemonPartyEditorWindowController showWindow:sender];
}

- (IBAction) openPokemonPartyStatusEditor:(id)sender
{
	if (pokemonPartyStatusEditorWindowController == nil)
	{
		pokemonPartyStatusEditorWindowController = [[PokemonPartyStatusEditorWindowController alloc] init];
	}

	[pokemonPartyStatusEditorWindowController showWindow:sender];
}

- (IBAction) openRecentCheatDatabase:(id)sender
{
	CheatDatabaseWindowController *newWindowController = [[CheatDatabaseWindowController alloc] initWithWindowNibName:@"CheatDatabaseViewer" delegate:cheatWindowDelegate];
	[newWindowController window]; // Just reference the window to force the NSWindow object to load.
	
	[[newWindowController window] makeKeyAndOrderFront:self];
	[[newWindowController window] makeMainWindow];
	
	NSArray *recentDBFilePathsList = [[NSUserDefaults standardUserDefaults] arrayForKey:@"CheatDatabase_RecentFilePath"];
	NSInteger index = [CocoaDSUtil getIBActionSenderTag:sender];
	NSDictionary *recentItem = (NSDictionary *)[recentDBFilePathsList objectAtIndex:index];
	NSString *recentItemFilePath = nil;
	
	if (recentItem != nil)
	{
		NSNumber *compatibilityCheckNumber = (NSNumber *)[recentItem objectForKey:@"OptionIgnoreCompatibilityCheck"];
		if (compatibilityCheckNumber != nil)
		{
			[newWindowController setIsOptionWarningSilenced:YES];
			[newWindowController setIsCompatibilityCheckIgnored:[compatibilityCheckNumber boolValue]];
			[newWindowController setIsOptionWarningSilenced:NO];
		}
		
		// Set up the window properties.
		NSString *windowFrameString = (NSString *)[recentItem objectForKey:@"WindowFrame"];
		if (windowFrameString != nil)
		{
			[[newWindowController window] setFrameFromString:windowFrameString];
		}
		
		NSNumber *windowSplitViewDividerPositionNumber = (NSNumber *)[recentItem objectForKey:@"WindowSplitViewDividerPosition"];
		if (windowSplitViewDividerPositionNumber != nil)
		{
			CGFloat dividerPosition = [windowSplitViewDividerPositionNumber floatValue];
			[[newWindowController splitView] setPosition:dividerPosition ofDividerAtIndex:0];
		}
		
		// Check for the file's existence at its path, and then handle appropriately.
		recentItemFilePath = (NSString *)[recentItem objectForKey:@"FilePath"];
		NSFileManager *fileManager = [[NSFileManager alloc] init];
		BOOL doesFileExist = [fileManager fileExistsAtPath:recentItemFilePath];
		[fileManager release];
		
		if (!doesFileExist)
		{
			// If the file does not exist, then report the error to the user, and the remove the
			// nonexistent file from the recents menu.
			[newWindowController showErrorSheet:CheatSystemError_FileDoesNotExist];
			
			NSMutableArray *newRecentsList = [NSMutableArray arrayWithCapacity:[recentDBFilePathsList count]];
			
			for (NSDictionary *theItem in recentDBFilePathsList)
			{
				if (theItem != recentItem)
				{
					[newRecentsList addObject:theItem];
				}
			}
			
			[[NSUserDefaults standardUserDefaults] setObject:newRecentsList forKey:@"CheatDatabase_RecentFilePath"];
			[self updateCheatDatabaseRecentsMenu:nil];
			return;
		}
	}
	
	NSURL *dbFileURL = [NSURL fileURLWithPath:recentItemFilePath];
	[newWindowController loadFileStart:dbFileURL];
}

- (IBAction) toggleExecutePause:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) coreExecute:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) corePause:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) frameAdvance:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) frameJump:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) reset:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) toggleSpeedLimiter:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) toggleAutoFrameSkip:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) framesToSkipSetting:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore setFramesToSkipSetting:[CocoaDSUtil getIBActionSenderTag:sender]];
}

- (IBAction) toggleGPUState:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) toggleGDBStubActivate:(id)sender
{
	// Force end of editing of any text fields.
	[[(NSControl *)sender window] makeFirstResponder:nil];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	const BOOL willStart = ([cdsCore isGdbStubStarted]) ? NO : YES;
	[cdsCore setIsGdbStubStarted:willStart];
	[(NSButton *)sender setTitle:([cdsCore isGdbStubStarted]) ? @"Stop" : @"Start"];
}

- (IBAction) toggleDebugInstructionTrace:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	const BOOL enabled = [cdsCore debugInstructionTraceEnabled] ? NO : YES;
	[cdsCore setDebugInstructionTraceEnabled:enabled];
	[self setStatusText:(enabled ? @"Instruction trace enabled." : @"Instruction trace disabled.")];
}

- (IBAction) toggleDebugAutoCrashSnapshot:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	const BOOL enabled = [cdsCore debugAutoCrashSnapshotEnabled] ? NO : YES;
	[cdsCore setDebugAutoCrashSnapshotEnabled:enabled];
	[self setStatusText:(enabled ? @"Auto crash snapshots enabled." : @"Auto crash snapshots disabled.")];
}

- (IBAction) exportDebugSnapshot:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	NSString *path = [cdsCore exportDebugSnapshotIncludingFullMemory:NO reason:@"manual"];
	[self setStatusText:(path != nil) ? [NSString stringWithFormat:@"Debug snapshot exported: %@", path] : @"Debug snapshot export failed."];
}

- (IBAction) exportDebugFullMemorySnapshot:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	NSString *path = [cdsCore exportDebugSnapshotIncludingFullMemory:YES reason:@"manual-full-memory"];
	[self setStatusText:(path != nil) ? [NSString stringWithFormat:@"Full debug snapshot exported: %@", path] : @"Full debug snapshot export failed."];
}

- (IBAction) revealDebugSnapshotFolder:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	NSURL *snapshotURL = [cdsCore debugSnapshotDirectoryURL];
	[[NSWorkspace sharedWorkspace] selectFile:nil inFileViewerRootedAtPath:[snapshotURL path]];
}

- (IBAction) prepareARM9Debugging:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore setCpuEmulationEngine:0];
	[cdsCore setEnableGdbStubARM9:YES];
	[cdsCore setGdbStubPortARM9:20000];
	[cdsCore setDebugInstructionTraceEnabled:YES];
	[cdsCore setDebugAutoCrashSnapshotEnabled:YES];
	[self setStatusText:@"ARM9 debugging prepared: interpreter, ARM9 GDB on port 20000, trace, auto snapshots."];
}

- (IBAction) changeRomSaveType:(id)sender
{
	const NSInteger saveTypeID = [CocoaDSUtil getIBActionSenderTag:sender];
	[self setSelectedRomSaveTypeID:saveTypeID];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore changeRomSaveType:saveTypeID];
}

- (IBAction) changeHostMicrophonePermission:(id)sender
{
#if HAVE_OSAVAILABLE && defined(MAC_OS_X_VERSION_10_14) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_14)
	if (IsOSXVersionSupported(10, 14, 0))
	{
		if (@available(macOS 10.14, *))
		{
			[AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio completionHandler:^(BOOL isAuthorized) {
				if (isAuthorized)
				{
					puts("DeSmuME: User has just granted access to the microphone.");
				}
				
				[self performSelectorOnMainThread:@selector(updateHostMicrophonePermissionStatus) withObject:nil waitUntilDone:NO];
			}];
		}
	}
	else
#endif
	{
		puts("DeSmuME: User is not running macOS v10.14 Mojave or later -- microphone usage is automatically authorized.");
		[self updateHostMicrophonePermissionStatus];
	}
}

- (IBAction) toggleCheats:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) changeCoreSpeed:(id)sender
{
	if ([sender isKindOfClass:[NSSlider class]])
	{
		lastSetSpeedScalar = [(NSSlider *)sender floatValue];
	}
	else
	{
		const CGFloat newSpeedScalar = (CGFloat)[CocoaDSUtil getIBActionSenderTag:sender] / 100.0f;
		[self changeCoreSpeedWithDouble:newSpeedScalar];
	}
	
	[self setVerticalSyncForNonLayerBackedViews:sender];
}

- (IBAction) changeFirmwareSettings:(id)sender
{
	// Force end of editing of any text fields.
	[[(NSControl *)sender window] makeFirstResponder:nil];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[[cdsCore cdsFirmware] applySettings];
	[cdsCore updateFirmwareMACAddressString];
}

- (IBAction) changeHardwareMicGain:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[[cdsCore cdsController] setHardwareMicGain:([self currentMicGainValue]/100.0f)];
}

- (IBAction) changeHardwareMicMute:(id)sender
{
	const BOOL muteState = [CocoaDSUtil getIBActionSenderButtonStateBool:sender];
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[[cdsCore cdsController] setHardwareMicMute:muteState];
	[self updateMicStatusIcon];
}

- (IBAction) changeVolume:(id)sender
{
	NSNumber *volumeNumber = [audioController volume];
	[audioController setVolume:volumeNumber];
}

- (IBAction) changeAudioEngine:(id)sender
{
	[audioController setEngineIDValue:(int)[CocoaDSUtil getIBActionSenderTag:sender]];
}

- (IBAction) changeSpuAdvancedLogic:(id)sender
{
	[audioController setSpuAdvancedLogic:[CocoaDSUtil getIBActionSenderButtonStateBool:sender] ? YES : NO];
}

- (IBAction) changeSpuInterpolationMode:(id)sender
{
	[audioController setSpuInterpolationModeIDValue:(SPUInterpolationMode)[CocoaDSUtil getIBActionSenderTag:sender]];
}

- (IBAction) changeSpuSyncMode:(id)sender
{
	[audioController setSpuSyncModeIDValue:(ESynchMode)[CocoaDSUtil getIBActionSenderTag:sender]];
}

- (IBAction) changeSpuSyncMethod:(id)sender
{
	[audioController setSpuSyncMethodIDValue:(ESynchMethod)[CocoaDSUtil getIBActionSenderTag:sender]];
}

- (IBAction) toggleAllDisplays:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) autoholdSet:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) autoholdClear:(id)sender
{
	[inputManager dispatchCommandUsingIBAction:_cmd sender:sender];
}

- (IBAction) setVerticalSyncForNonLayerBackedViews:(id)sender
{
	if ( ([[(DisplayWindowController *)[windowList objectAtIndex:0] view] layer] == nil) && ([windowList count] > 0) )
	{
		CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
		const BOOL useVerticalSync = [cdsCore isFrameSkipEnabled] || (([cdsCore speedScalar] < 1.03f) && [cdsCore isSpeedLimitEnabled]);
		
		for (DisplayWindowController *windowController in windowList)
		{
			[[windowController view] setUseVerticalSync:useVerticalSync];
		}
	}
}

- (IBAction) chooseSlot1R4Directory:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	[panel setCanChooseDirectories:YES];
	[panel setCanChooseFiles:NO];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setTitle:@"Select R4 Directory"];
	
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
	if (IsOSXVersionSupported(10, 6, 0))
	{
		[panel beginSheetModalForWindow:slot1ManagerWindow
					  completionHandler:^(NSInteger result) {
						  [self didEndChooseSlot1R4Directory:panel returnCode:(int)result contextInfo:nil];
					  } ];
	}
	else
#endif
	{
		SILENCE_DEPRECATION_MACOS_10_6( [panel beginSheetForDirectory:nil
																 file:nil
																types:nil
													   modalForWindow:slot1ManagerWindow
														modalDelegate:self
													   didEndSelector:@selector(didEndChooseSlot1R4Directory:returnCode:contextInfo:)
														  contextInfo:nil] );
	}
}

- (IBAction) slot1Eject:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore slot1Eject];
}

- (IBAction) simulateEmulationCrash:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore setMasterExecute:NO];
}

- (IBAction) generateFirmwareMACAddress:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore generateFirmwareMACAddress];
}

- (IBAction) writeDefaults3DRenderingSettings:(id)sender
{
	CocoaGraphicsController *graphicsController = (CocoaGraphicsController *)[[self cdsGraphicsController] content];
	
	// Force end of editing of any text fields.
	[[(NSControl *)sender window] makeFirstResponder:nil];
	
	[[NSUserDefaults standardUserDefaults] setInteger:[graphicsController engine3DClientIndex] forKey:@"Render3D_RenderingEngine"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController highPrecisionColorInterpolationEnable] forKey:@"Render3D_HighPrecisionColorInterpolation"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController edgeMarkEnable] forKey:@"Render3D_EdgeMarking"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController fogEnable] forKey:@"Render3D_Fog"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController textureEnable] forKey:@"Render3D_Textures"];
	[[NSUserDefaults standardUserDefaults] setInteger:[graphicsController render3DThreads] forKey:@"Render3D_Threads"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController lineHackEnable] forKey:@"Render3D_LineHack"];
	[[NSUserDefaults standardUserDefaults] setInteger:[graphicsController multisampleSize] forKey:@"Render3D_MultisampleSize"];
	[[NSUserDefaults standardUserDefaults] setInteger:[graphicsController textureScalingFactor] forKey:@"Render3D_TextureScalingFactor"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController textureDeposterizeEnable] forKey:@"Render3D_TextureDeposterize"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController textureSmoothingEnable] forKey:@"Render3D_TextureSmoothing"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController fragmentSamplingHackEnable] forKey:@"Render3D_FragmentSamplingHack"];
	[[NSUserDefaults standardUserDefaults] setInteger:[graphicsController gpuScale] forKey:@"Render3D_ScalingFactor"];
	[[NSUserDefaults standardUserDefaults] setInteger:[graphicsController gpuColorFormat] forKey:@"Render3D_ColorFormat"];
	
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController shadowPolygonEnable] forKey:@"Render3D_OpenGL_EmulateShadowPolygon"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController specialZeroAlphaBlendingEnable] forKey:@"Render3D_OpenGL_EmulateSpecialZeroAlphaBlending"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController ndsDepthCalculationEnable] forKey:@"Render3D_OpenGL_EmulateNDSDepthCalculation"];
	[[NSUserDefaults standardUserDefaults] setBool:[graphicsController depthLEqualPolygonFacingEnable] forKey:@"Render3D_OpenGL_EmulateDepthLEqualPolygonFacing"];
	
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction) writeDefaultsEmulationSettings:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	CocoaDSFirmware *writeFirmware = (CocoaDSFirmware *)[firmwarePanelController content];
	
	// Force end of editing of any text fields.
	[[(NSControl *)sender window] makeFirstResponder:nil];
	
	// Saving the user defaults also applies the current firmware settings.
	[writeFirmware applySettings];
	
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagAdvancedBusLevelTiming] forKey:@"Emulation_AdvancedBusLevelTiming"];
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagRigorousTiming] forKey:@"Emulation_RigorousTiming"];
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagUseGameSpecificHacks] forKey:@"Emulation_UseGameSpecificHacks"];
	[[NSUserDefaults standardUserDefaults] setInteger:[cdsCore cpuEmulationEngine] forKey:@"Emulation_CPUEmulationEngine"];
	[[NSUserDefaults standardUserDefaults] setInteger:[cdsCore maxJITBlockSize] forKey:@"Emulation_MaxJITBlockSize"];
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagUseExternalBios] forKey:@"Emulation_UseExternalBIOSImages"];
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagEmulateBiosInterrupts] forKey:@"Emulation_BIOSEmulateSWI"];
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagPatchDelayLoop] forKey:@"Emulation_BIOSPatchDelayLoopSWI"];
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagUseExternalFirmware] forKey:@"Emulation_UseExternalFirmwareImage"];
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagFirmwareBoot] forKey:@"Emulation_FirmwareBoot"];
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagEmulateEnsata] forKey:@"Emulation_EmulateEnsata"];
	[[NSUserDefaults standardUserDefaults] setBool:[cdsCore emuFlagDebugConsole] forKey:@"Emulation_UseDebugConsole"];
	
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware firmwareMACAddressValue] forKey:@"FirmwareConfig_FirmwareMACAddress"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware customMACAddressValue] forKey:@"FirmwareConfig_CustomMACAddress"];
	
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP1_1] forKey:@"FirmwareConfig_IPv4Address_AP1_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP1_2] forKey:@"FirmwareConfig_IPv4Address_AP1_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP1_3] forKey:@"FirmwareConfig_IPv4Address_AP1_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP1_4] forKey:@"FirmwareConfig_IPv4Address_AP1_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP1_1] forKey:@"FirmwareConfig_IPv4Gateway_AP1_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP1_2] forKey:@"FirmwareConfig_IPv4Gateway_AP1_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP1_3] forKey:@"FirmwareConfig_IPv4Gateway_AP1_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP1_4] forKey:@"FirmwareConfig_IPv4Gateway_AP1_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP1_1] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP1_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP1_2] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP1_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP1_3] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP1_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP1_4] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP1_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP1_1] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP1_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP1_2] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP1_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP1_3] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP1_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP1_4] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP1_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware subnetMask_AP1] forKey:@"FirmwareConfig_SubnetMask_AP1"];
	
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP2_1] forKey:@"FirmwareConfig_IPv4Address_AP2_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP2_2] forKey:@"FirmwareConfig_IPv4Address_AP2_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP2_3] forKey:@"FirmwareConfig_IPv4Address_AP2_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP2_4] forKey:@"FirmwareConfig_IPv4Address_AP2_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP2_1] forKey:@"FirmwareConfig_IPv4Gateway_AP2_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP2_2] forKey:@"FirmwareConfig_IPv4Gateway_AP2_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP2_3] forKey:@"FirmwareConfig_IPv4Gateway_AP2_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP2_4] forKey:@"FirmwareConfig_IPv4Gateway_AP2_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP2_1] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP2_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP2_2] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP2_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP2_3] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP2_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP2_4] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP2_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP2_1] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP2_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP2_2] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP2_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP2_3] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP2_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP2_4] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP2_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware subnetMask_AP2] forKey:@"FirmwareConfig_SubnetMask_AP2"];
	
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP3_1] forKey:@"FirmwareConfig_IPv4Address_AP3_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP3_2] forKey:@"FirmwareConfig_IPv4Address_AP3_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP3_3] forKey:@"FirmwareConfig_IPv4Address_AP3_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Address_AP3_4] forKey:@"FirmwareConfig_IPv4Address_AP3_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP3_1] forKey:@"FirmwareConfig_IPv4Gateway_AP3_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP3_2] forKey:@"FirmwareConfig_IPv4Gateway_AP3_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP3_3] forKey:@"FirmwareConfig_IPv4Gateway_AP3_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4Gateway_AP3_4] forKey:@"FirmwareConfig_IPv4Gateway_AP3_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP3_1] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP3_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP3_2] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP3_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP3_3] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP3_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4PrimaryDNS_AP3_4] forKey:@"FirmwareConfig_IPv4PrimaryDNS_AP3_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP3_1] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP3_1"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP3_2] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP3_2"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP3_3] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP3_3"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware ipv4SecondaryDNS_AP3_4] forKey:@"FirmwareConfig_IPv4SecondaryDNS_AP3_4"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware subnetMask_AP3] forKey:@"FirmwareConfig_SubnetMask_AP3"];
	
	//[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware consoleType] forKey:@"FirmwareConfig_ConsoleType"];
	[[NSUserDefaults standardUserDefaults] setObject:[writeFirmware nickname] forKey:@"FirmwareConfig_Nickname"];
	[[NSUserDefaults standardUserDefaults] setObject:[writeFirmware message] forKey:@"FirmwareConfig_Message"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware favoriteColor] forKey:@"FirmwareConfig_FavoriteColor"];
	[[NSUserDefaults standardUserDefaults] setObject:[writeFirmware birthday] forKey:@"FirmwareConfig_Birthday"];
	[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware language] forKey:@"FirmwareConfig_Language"];
	//[[NSUserDefaults standardUserDefaults] setInteger:[writeFirmware backlightLevel] forKey:@"FirmwareConfig_BacklightLevel"];
	
	[prefWindowDelegate updateFirmwareMACAddressString:nil];
	[prefWindowDelegate updateSubnetMaskString_AP1:nil];
	[prefWindowDelegate updateSubnetMaskString_AP2:nil];
	[prefWindowDelegate updateSubnetMaskString_AP3:nil];
	
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction) writeDefaultsSlot1Settings:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	[[NSUserDefaults standardUserDefaults] setInteger:[cdsCore slot1DeviceType] forKey:@"EmulationSlot1_DeviceType"];
	[[NSUserDefaults standardUserDefaults] setObject:[[cdsCore slot1R4URL] path] forKey:@"EmulationSlot1_R4StoragePath"];
	
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction) writeDefaultsSoundSettings:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setFloat:[audioController volumeValue] forKey:@"Sound_Volume"];
	[[NSUserDefaults standardUserDefaults] setInteger:(NSInteger)[audioController engineIDValue] forKey:@"Sound_AudioOutputEngine"];
	[[NSUserDefaults standardUserDefaults] setBool:[audioController spuAdvancedLogic] forKey:@"SPU_AdvancedLogic"];
	[[NSUserDefaults standardUserDefaults] setInteger:(NSInteger)[audioController spuInterpolationModeIDValue] forKey:@"SPU_InterpolationMode"];
	[[NSUserDefaults standardUserDefaults] setInteger:(NSInteger)[audioController spuSyncModeIDValue] forKey:@"SPU_SyncMode"];
	[[NSUserDefaults standardUserDefaults] setInteger:(NSInteger)[audioController spuSyncMethodIDValue] forKey:@"SPU_SyncMethod"];
	
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction) writeDefaultsStylusSettings:(id)sender
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	CocoaDSController *cdsController = [cdsCore cdsController];
	
	[[NSUserDefaults standardUserDefaults] setInteger:[cdsController stylusPressure] forKey:@"Emulation_StylusPressure"];
	
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction) closeSheet:(id)sender
{
	NSWindow *sheet = [(NSControl *)sender window];
	const NSInteger code = [(NSControl *)sender tag];
	
	[CocoaDSUtil endSheet:sheet returnCode:code];
}

#pragma mark Class Methods

- (void) cmdUpdateDSController:(const ClientCommandAttributes &)cmdAttr
{
	const BOOL theState = (cmdAttr.input.state == ClientInputDeviceState_On) ? YES : NO;
	const NSUInteger controlID = cmdAttr.intValue[0];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[[cdsCore cdsController] setControllerState:theState controlID:controlID];
	
	for (DisplayWindowController *windowController in windowList)
	{
		MacDisplayViewOutput *dvo = [[windowController view] displayViewOutput];
		dvo->SetNDSFrameInfoWithInputHint([cdsCore execControl]->GetNDSFrameInfo(), true);
	}
}

- (void) cmdUpdateDSControllerWithTurbo:(const ClientCommandAttributes &)cmdAttr
{
	const BOOL theState = (cmdAttr.input.state == ClientInputDeviceState_On) ? YES : NO;
	const NSUInteger controlID = cmdAttr.intValue[0];
	const BOOL isTurboEnabled = (BOOL)cmdAttr.intValue[1];
	const uint32_t turboPattern = (uint32_t)cmdAttr.intValue[2];
	const uint32_t turboPatternLength = (uint32_t)cmdAttr.intValue[3];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[[cdsCore cdsController] setControllerState:theState controlID:controlID turbo:isTurboEnabled turboPattern:turboPattern turboPatternLength:turboPatternLength];
	
	for (DisplayWindowController *windowController in windowList)
	{
		MacDisplayViewOutput *dvo = [[windowController view] displayViewOutput];
		dvo->SetNDSFrameInfoWithInputHint([cdsCore execControl]->GetNDSFrameInfo(), true);
	}
}

- (void) cmdUpdateDSTouch:(const ClientCommandAttributes &)cmdAttr
{
	const BOOL theState = (cmdAttr.input.state == ClientInputDeviceState_On) ? YES : NO;
	
	const NSPoint touchLoc = (cmdAttr.useInputForIntCoord) ? NSMakePoint(cmdAttr.input.intCoordX, cmdAttr.input.intCoordY) : NSMakePoint(cmdAttr.intValue[1], cmdAttr.intValue[2]);
	if (touchLoc.x >= 0.0 && touchLoc.y >= 0.0)
	{
		CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
		[[cdsCore cdsController] setTouchState:theState location:touchLoc];
		
		for (DisplayWindowController *windowController in windowList)
		{
			MacDisplayViewOutput *dvo = [[windowController view] displayViewOutput];
			dvo->SetNDSFrameInfoWithInputHint([cdsCore execControl]->GetNDSFrameInfo(), true);
		}
	}
}

- (void) cmdUpdateDSMicrophone:(const ClientCommandAttributes &)cmdAttr
{
	const BOOL theState = (cmdAttr.input.state == ClientInputDeviceState_On) ? YES : NO;
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	CocoaDSController *cdsController = [cdsCore cdsController];
	
	const NSInteger micMode = cmdAttr.intValue[1];
	[cdsController setSoftwareMicState:theState mode:micMode];
	
	const float sineWaveFrequency = cmdAttr.floatValue[0];
	[cdsController setSineWaveGeneratorFrequency:sineWaveFrequency];
	
	NSString *audioFilePath = (NSString *)cmdAttr.object[0];
	[cdsController setSelectedAudioFileGenerator:[inputManager audioFileGeneratorFromFilePath:audioFilePath]];
}

- (void) cmdUpdateDSPaddle:(const ClientCommandAttributes &)cmdAttr
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	if (cmdAttr.input.isAnalog)
	{
		const float paddleSensitivity = cmdAttr.floatValue[0];
		const float paddleScalar = cmdAttr.input.scalar;
		float paddleAdjust = (paddleScalar * 2.0f) - 1.0f;
		
		// Clamp the paddle value.
		if (paddleAdjust < -1.0f)
		{
			paddleAdjust = -1.0f;
		}
		
		if (paddleAdjust > 1.0f)
		{
			paddleAdjust = 1.0f;
		}
		
		// Normalize the input value for the paddle.
		paddleAdjust *= paddleSensitivity;
		[[cdsCore cdsController] setPaddleAdjust:(NSInteger)(paddleAdjust + 0.5f)];
	}
	else
	{
		const BOOL theState = (cmdAttr.input.state == ClientInputDeviceState_On) ? YES : NO;
		const NSInteger paddleAdjust = (theState) ? cmdAttr.intValue[1] : 0;
		[[cdsCore cdsController] setPaddleAdjust:paddleAdjust];
	}
}

- (void) cmdAutoholdSet:(const ClientCommandAttributes &)cmdAttr
{
	const BOOL theState = (cmdAttr.input.state == ClientInputDeviceState_On) ? YES : NO;
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[[cdsCore cdsController] setAutohold:theState];
	[self setStatusText:(theState) ? NSSTRING_STATUS_AUTOHOLD_SET : NSSTRING_STATUS_AUTOHOLD_SET_RELEASE];
}

- (void) cmdAutoholdClear:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[[cdsCore cdsController] clearAutohold];
	[self setStatusText:NSSTRING_STATUS_AUTOHOLD_CLEAR];
	
}

- (void) cmdLoadEmuSaveStateSlot:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	NSString *saveStatePath = [[CocoaDSFile saveStateURL] path];
	if (saveStatePath == nil)
	{
		// Should throw an error message here...
		return;
	}
	
	const NSInteger slotNumber = (cmdAttr.useInputForObject) ? [CocoaDSUtil getIBActionSenderTag:(id)cmdAttr.input.object] : cmdAttr.intValue[0];
	if (slotNumber < 0 || slotNumber > MAX_SAVESTATE_SLOTS)
	{
		return;
	}
	
	NSURL *currentRomURL = [[self currentRom] fileURL];
	NSString *fileName = [CocoaDSFile saveSlotFileName:currentRomURL slotNumber:(NSUInteger)(slotNumber + 1)];
	if (fileName == nil)
	{
		return;
	}
	
	[self pauseCore];
	
	const BOOL isStateLoaded = [CocoaDSFile loadState:[NSURL fileURLWithPath:[saveStatePath stringByAppendingPathComponent:fileName]]];
	[self setStatusText:(isStateLoaded) ? NSSTRING_STATUS_SAVESTATE_LOADED : NSSTRING_STATUS_SAVESTATE_LOADING_FAILED];
	
	[self restoreCoreState];
}

- (void) cmdSaveEmuSaveStateSlot:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	NSString *saveStatePath = [[CocoaDSFile saveStateURL] path];
	if (saveStatePath == nil)
	{
		[self setStatusText:NSSTRING_STATUS_CANNOT_GENERATE_SAVE_PATH];
		return;
	}
	
	const BOOL isDirectoryCreated = [CocoaDSFile createUserAppSupportDirectory:@"States"];
	if (!isDirectoryCreated)
	{
		[self setStatusText:NSSTRING_STATUS_CANNOT_CREATE_SAVE_DIRECTORY];
		return;
	}
	
	const NSInteger slotNumber = (cmdAttr.useInputForObject) ? [CocoaDSUtil getIBActionSenderTag:(id)cmdAttr.input.object] : cmdAttr.intValue[0];
	if (slotNumber < 0 || slotNumber > MAX_SAVESTATE_SLOTS)
	{
		return;
	}
	
	NSURL *currentRomURL = [[self currentRom] fileURL];
	NSString *fileName = [CocoaDSFile saveSlotFileName:currentRomURL slotNumber:(NSUInteger)(slotNumber + 1)];
	if (fileName == nil)
	{
		return;
	}
	
	[self pauseCore];
	
	const BOOL isStateSaved = [CocoaDSFile saveState:[NSURL fileURLWithPath:[saveStatePath stringByAppendingPathComponent:fileName]]];
	[self setStatusText:(isStateSaved) ? NSSTRING_STATUS_SAVESTATE_SAVED : NSSTRING_STATUS_SAVESTATE_SAVING_FAILED];
	
	[self restoreCoreState];
}

- (void) cmdCopyScreen:(const ClientCommandAttributes &)cmdAttr
{
	[mainWindow copy:nil];
}

- (void) cmdRotateDisplayRelative:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	const double relativeDegrees = (cmdAttr.useInputForObject) ? (double)[CocoaDSUtil getIBActionSenderTag:(id)cmdAttr.input.object] : (double)cmdAttr.intValue[0];
	const double angleDegrees = [mainWindow displayRotation] + relativeDegrees;
	[mainWindow setDisplayRotation:angleDegrees];
}

- (void) cmdToggleAllDisplays:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	for (DisplayWindowController *theWindow in windowList)
	{
		const NSInteger displayMode = [theWindow displayMode];
		switch (displayMode)
		{
			case ClientDisplayMode_Main:
				[theWindow setDisplayMode:ClientDisplayMode_Touch];
				break;
				
			case ClientDisplayMode_Touch:
				[theWindow setDisplayMode:ClientDisplayMode_Main];
				break;
				
			case ClientDisplayMode_Dual:
			{
				const NSInteger displayOrder = [theWindow displayOrder];
				if (displayOrder == ClientDisplayOrder_MainFirst)
				{
					[theWindow setDisplayOrder:ClientDisplayOrder_TouchFirst];
				}
				else
				{
					[theWindow setDisplayOrder:ClientDisplayOrder_MainFirst];
				}
				break;
			}
				
			default:
				break;
		}
	}
}

- (void) cmdHoldToggleSpeedScalar:(const ClientCommandAttributes &)cmdAttr
{
	const float inputSpeedScalar = (cmdAttr.useInputForScalar) ? cmdAttr.input.scalar : cmdAttr.floatValue[0];
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	[cdsCore setSpeedScalar:(cmdAttr.input.state == ClientInputDeviceState_Off) ? lastSetSpeedScalar : inputSpeedScalar];
	[self setVerticalSyncForNonLayerBackedViews:nil];
}

- (void) cmdToggleSpeedLimiter:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	if ([cdsCore isSpeedLimitEnabled])
	{
		[cdsCore setIsSpeedLimitEnabled:NO];
		[self setStatusText:NSSTRING_STATUS_SPEED_LIMIT_DISABLED];
		[[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"CoreControl_EnableSpeedLimit"];
	}
	else
	{
		[cdsCore setIsSpeedLimitEnabled:YES];
		[self setStatusText:NSSTRING_STATUS_SPEED_LIMIT_ENABLED];
		[[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"CoreControl_EnableSpeedLimit"];
	}
	
	[self setVerticalSyncForNonLayerBackedViews:nil];
}

- (void) cmdToggleAutoFrameSkip:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	if ([cdsCore isFrameSkipEnabled])
	{
		[cdsCore setIsFrameSkipEnabled:NO];
		[self setStatusText:NSSTRING_STATUS_AUTO_FRAME_SKIP_DISABLED];
		[[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"CoreControl_EnableAutoFrameSkip"];
	}
	else
	{
		[cdsCore setIsFrameSkipEnabled:YES];
		[self setStatusText:NSSTRING_STATUS_AUTO_FRAME_SKIP_ENABLED];
		[[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"CoreControl_EnableAutoFrameSkip"];
	}
	
	[self setVerticalSyncForNonLayerBackedViews:nil];
}

- (void) cmdToggleCheats:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	if ([cdsCore isCheatingEnabled])
	{
		[cdsCore setIsCheatingEnabled:NO];
		[self setStatusText:NSSTRING_STATUS_CHEATS_DISABLED];
		[[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"CoreControl_EnableCheats"];
	}
	else
	{
		[cdsCore setIsCheatingEnabled:YES];
		[self setStatusText:NSSTRING_STATUS_CHEATS_ENABLED];
		[[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"CoreControl_EnableCheats"];
	}
}

- (void) cmdToggleExecutePause:(const ClientCommandAttributes &)cmdAttr
{
	if ( (cmdAttr.input.state == ClientInputDeviceState_Off) || ([self currentRom] == nil) )
	{
		return;
	}
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	if ([cdsCore coreState] == ExecutionBehavior_Pause)
	{
		[self executeCore];
	}
	else
	{
		[self pauseCore];
	}
}

- (void) cmdCoreExecute:(const ClientCommandAttributes &)cmdAttr
{
	if ( (cmdAttr.input.state == ClientInputDeviceState_Off) || ([self currentRom] == nil) )
	{
		return;
	}
	
	[self executeCore];
}

- (void) cmdCorePause:(const ClientCommandAttributes &)cmdAttr
{
	if ( (cmdAttr.input.state == ClientInputDeviceState_Off) || ([self currentRom] == nil) )
	{
		return;
	}
	
	[self pauseCore];
}

- (void) cmdFrameAdvance:(const ClientCommandAttributes &)cmdAttr
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	if ( (cmdAttr.input.state == ClientInputDeviceState_Off) ||
	     ([cdsCore coreState] != ExecutionBehavior_Pause) ||
	     ([self currentRom] == nil) )
	{
		return;
	}
	
	[cdsCore setCoreState:ExecutionBehavior_FrameAdvance];
}

- (void) cmdFrameJump:(const ClientCommandAttributes &)cmdAttr
{
	if ( (cmdAttr.input.state == ClientInputDeviceState_Off) || ([self currentRom] == nil) )
	{
		return;
	}
	
	[executionControlWindow makeFirstResponder:nil];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore setCoreState:ExecutionBehavior_FrameJump];
}

- (void) cmdReset:(const ClientCommandAttributes &)cmdAttr
{
	if ( (cmdAttr.input.state == ClientInputDeviceState_Off) || ([self currentRom] == nil) )
	{
		return;
	}
	
	[self setStatusText:NSSTRING_STATUS_EMULATOR_RESETTING];
	[self setIsWorking:YES];
	
	for (DisplayWindowController *windowController in windowList)
	{
		[[windowController window] displayIfNeeded];
	}
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore reset];
	
	for (DisplayWindowController *windowController in windowList)
	{
		MacDisplayViewOutput *dvo = [[windowController view] displayViewOutput];
		dvo->Dispatch(MESSAGE_RELOAD_REPROCESS_REDRAW);
	}
	
	[self setStatusText:NSSTRING_STATUS_EMULATOR_RESET];
	[self setIsWorking:NO];
	[self writeDefaultsSlot1Settings:nil];
	
	for (DisplayWindowController *windowController in windowList)
	{
		[[windowController window] displayIfNeeded];
	}
}

- (void) cmdOpenPokemonPartyStatusEditor:(const ClientCommandAttributes &)cmdAttr
{
	if ( (cmdAttr.input.state == ClientInputDeviceState_Off) || ([self currentRom] == nil) )
	{
		return;
	}

	[self openPokemonPartyStatusEditor:self];
}

- (void) cmdCopyPokemonExportToClipboard:(const ClientCommandAttributes &)cmdAttr
{
	if ( (cmdAttr.input.state == ClientInputDeviceState_Off) || ([self currentRom] == nil) )
	{
		return;
	}

	[self copyPokemonExportToClipboard:self];
}

- (void) cmdOpenPokemonPartyEditor:(const ClientCommandAttributes &)cmdAttr
{
	if ( (cmdAttr.input.state == ClientInputDeviceState_Off) || ([self currentRom] == nil) )
	{
		return;
	}

	[self openPokemonPartyEditor:self];
}

- (void) cmdToggleMute:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	if ([audioController isMuted])
	{
		[audioController setMute:NO];
		[self setStatusText:@"Sound unmuted."];
	}
	else
	{
		[audioController setMute:YES];
		[self setStatusText:@"Sound muted."];
	}
}

- (void) cmdToggleGPUState:(const ClientCommandAttributes &)cmdAttr
{
	if (cmdAttr.input.state == ClientInputDeviceState_Off)
	{
		return;
	}
	
	CocoaGraphicsController *graphicsController = (CocoaGraphicsController *)[[self cdsGraphicsController] content];
	const NSInteger bitNumber = (cmdAttr.useInputForObject) ? [CocoaDSUtil getIBActionSenderTag:(id)cmdAttr.input.object] : cmdAttr.intValue[0];
	const UInt32 flagBit = [graphicsController gpuStateFlags] ^ (1 << bitNumber);
	
	[graphicsController setGpuStateFlags:flagBit];
}

- (BOOL) handleLoadRomByURL:(NSURL *)fileURL
{
	BOOL result = NO;
	
	if (fileURL == nil || [self isRomLoading])
	{
		return result;
	}
	
	if ([self currentRom] != nil)
	{
		const BOOL closeResult = [self handleUnloadRom:REASONFORCLOSE_OPEN romToLoad:fileURL];
		if ([self isShowingSaveStateDialog])
		{
			return result;
		}
		
		if (![self isShowingFileMigrationDialog] && !closeResult)
		{
			return result;
		}
	}
	
	// Check for the v0.9.7 ROM Save File
	if ([CocoaDSFile romSaveExistsWithRom:fileURL] && ![CocoaDSFile romSaveExists:fileURL])
	{
		[fileURL retain];
		[self setIsUserInterfaceBlockingExecution:YES];
		[self setIsShowingFileMigrationDialog:YES];
		
		NSWindow *window = [[windowList objectAtIndex:0] window];
		
#if defined(MAC_OS_X_VERSION_10_9) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_9)
		if ([window respondsToSelector:@selector(beginSheet:completionHandler:)])
		{
			[window beginSheet:saveFileMigrationSheet
			 completionHandler:^(NSModalResponse response) {
				[self didEndFileMigrationSheet:nil returnCode:response contextInfo:fileURL];
			} ];
		}
		else
#endif
		{
			SILENCE_DEPRECATION_MACOS_10_10( [NSApp beginSheet:saveFileMigrationSheet
												modalForWindow:window
												 modalDelegate:self
												didEndSelector:@selector(didEndFileMigrationSheet:returnCode:contextInfo:)
												   contextInfo:fileURL] );
		}
	}
	else
	{
		result = [self loadRomByURL:fileURL asynchronous:YES];
	}
	
	return result;
}

- (BOOL) handleUnloadRom:(NSInteger)reasonID romToLoad:(NSURL *)romURL
{
	BOOL result = NO;
	
	if ([self isRomLoading] || [self currentRom] == nil)
	{
		return result;
	}
	
	[self pauseCore];
	
	if (isSaveStateEdited && [self currentSaveStateURL] != nil)
	{
		SEL endSheetSelector = @selector(didEndSaveStateSheet:returnCode:contextInfo:);
		
		switch (reasonID)
		{
			case REASONFORCLOSE_OPEN:
				[romURL retain];
				endSheetSelector = @selector(didEndSaveStateSheetOpen:returnCode:contextInfo:);
				break;	
				
			case REASONFORCLOSE_TERMINATE:
				endSheetSelector = @selector(didEndSaveStateSheetTerminate:returnCode:contextInfo:);
				break;
				
			default:
				break;
		}
		
		[self setIsUserInterfaceBlockingExecution:YES];
		[self setIsShowingSaveStateDialog:YES];
		
		NSWindow *window = [[windowList objectAtIndex:0] window];
		
#if defined(MAC_OS_X_VERSION_10_9) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_9)
		if ([window respondsToSelector:@selector(beginSheet:completionHandler:)])
		{
			[window beginSheet:saveStatePrecloseSheet
			 completionHandler:^(NSModalResponse response) {
				switch (reasonID)
				{
					case REASONFORCLOSE_NORMAL:
						[self didEndSaveStateSheet:nil returnCode:response contextInfo:romURL];
						break;
						
					case REASONFORCLOSE_OPEN:
						[self didEndSaveStateSheetOpen:nil returnCode:response contextInfo:romURL];
						break;
						
					case REASONFORCLOSE_TERMINATE:
						[self didEndSaveStateSheetTerminate:nil returnCode:response contextInfo:romURL];
						break;
						
					default:
						break;
				}
			} ];
		}
		else
#endif
		{
			SILENCE_DEPRECATION_MACOS_10_10( [NSApp beginSheet:saveStatePrecloseSheet
												modalForWindow:window
												 modalDelegate:self
												didEndSelector:endSheetSelector
												   contextInfo:romURL] );
		}
	}
	else
	{
		result = [self unloadRom];
	}
	
	return result;
}

- (BOOL) loadRomByURL:(NSURL *)romURL asynchronous:(BOOL)willLoadAsync
{
	BOOL result = NO;
	
	if (romURL == nil)
	{
		return result;
	}
	
	[self setStatusText:NSSTRING_STATUS_ROM_LOADING];
	[self setIsWorking:YES];
	
	for (DisplayWindowController *windowController in windowList)
	{
		[[windowController window] displayIfNeeded];
	}
	
	// Need to pause the core before loading the ROM.
	[self pauseCore];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	if (![cdsCore emuFlagUseExternalBios] || ![cdsCore emuFlagUseExternalFirmware])
	{
		[[cdsCore cdsFirmware] updateFirmwareConfigSessionValues];
	}
	
	[cdsCore execControl]->ApplySettingsOnReset();
	[cdsCore updateSlot1DeviceStatus];
	[self writeDefaultsSlot1Settings:nil];
	
	CocoaDSRom *newRom = [[CocoaDSRom alloc] init];
	if (newRom != nil)
	{
		[self setIsRomLoading:YES];
		[romURL retain];
		[newRom setSaveType:selectedRomSaveTypeID];
		[newRom setWillStreamLoadData:[[NSUserDefaults standardUserDefaults] boolForKey:@"General_StreamLoadRomData"]];
		
		if (willLoadAsync)
		{
			[NSThread detachNewThreadSelector:@selector(loadDataOnThread:) toTarget:newRom withObject:romURL];
		}
		else
		{
			[newRom loadData:romURL];
		}
		
		[romURL release];
	}
	
	result = YES;
	
	return result;
}

- (void) loadRomDidFinish:(NSNotification *)aNotification
{
	CocoaDSRom *theRom = [aNotification object];
	NSDictionary *userInfo = [aNotification userInfo];
	const BOOL didLoad = [(NSNumber *)[userInfo valueForKey:@"DidLoad"] boolValue];
	
	if ( theRom == nil || !didLoad || (![theRom willStreamLoadData] && ![theRom isDataLoaded]) )
	{
		// If ROM loading fails, restore the core state, but only if a ROM is already loaded.
		if([self currentRom] != nil)
		{
			[self restoreCoreState];
		}
		
		[self setStatusText:NSSTRING_STATUS_ROM_LOADING_FAILED];
		[self setIsWorking:NO];
		[self setIsRomLoading:NO];
		
		return;
	}
	
	// Set the core's ROM to our newly allocated ROM object.
	[self setCurrentRom:theRom];
	[romInfoPanelController setContent:[theRom bindings]];
	
	// If the ROM has an associated cheat file, load it now.
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cheatWindowDelegate cheatSystemStart:[cdsCore cdsCheatManager]];
	
	// Add the last loaded ROM to the Recent ROMs list.
	[[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:[theRom fileURL]];
	
	// Update the UI to indicate that a ROM has indeed been loaded.
	[self updateAllWindowTitles];
	[screenshotCaptureToolDelegate setRomName:[currentRom internalName]];
	[avCaptureToolDelegate setRomName:[currentRom internalName]];
	
	for (DisplayWindowController *windowController in windowList)
	{
		MacDisplayViewOutput *dvo = [[windowController view] displayViewOutput];
		dvo->Dispatch(MESSAGE_RELOAD_REPROCESS_REDRAW);
	}
	
	[self setStatusText:NSSTRING_STATUS_ROM_LOADED];
	[self setIsWorking:NO];
	[self setIsRomLoading:NO];
	
	Slot2WindowDelegate *slot2WindowDelegate = (Slot2WindowDelegate *)[slot2WindowController content];
	[slot2WindowDelegate setAutoSelectedDeviceText:[[slot2WindowDelegate deviceManager] autoSelectedDeviceName]];
	[[slot2WindowDelegate deviceManager] updateStatus];
		
	for (DisplayWindowController *windowController in windowList)
	{
		[[windowController window] displayIfNeeded];
	}
	
	[cdsCore updateCurrentSessionMACAddressString:YES];
	[cdsCore setMasterExecute:YES];
	
	// After the ROM loading is complete, send an execute message to the Cocoa DS per
	// user preferences.
	if ([[NSUserDefaults standardUserDefaults] boolForKey:@"General_ExecuteROMOnLoad"])
	{
		[self executeCore];
	}

	ClearPokemonPlatinumTrainerBattleOverridePendingState();
	[self updatePokemonHttpApiMenuItems];
}

- (BOOL) unloadRom
{
	BOOL result = NO;
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[self setCurrentSaveStateURL:nil];
	
	[self setIsWorking:YES];
	isSaveStateEdited = NO;
	for (DisplayWindowController *windowController in windowList)
	{
		[[windowController window] setDocumentEdited:isSaveStateEdited];
		[[windowController window] displayIfNeeded];
	}
	
	[cheatWindowDelegate cheatSystemEnd];
	
	// Update the UI to indicate that the ROM has started the process of unloading.
	[self setStatusText:NSSTRING_STATUS_ROM_UNLOADING];
	[romInfoPanelController setContent:[CocoaDSRom romNotLoadedBindings]];
	
	// Unload the ROM.
	if (![cdsCore emuFlagUseExternalBios] || ![cdsCore emuFlagUseExternalFirmware])
	{
		[[cdsCore cdsFirmware] writeUserDefaultWFCUserID];
	}
	
	[[self currentRom] release];
	[self setCurrentRom:nil];
	
	// Update the UI to indicate that the ROM has finished unloading.
	[self updateAllWindowTitles];
	[screenshotCaptureToolDelegate setRomName:[currentRom internalName]];
	[avCaptureToolDelegate setRomName:[currentRom internalName]];
	
	CocoaGraphicsController *graphicsController = (CocoaGraphicsController *)[[self cdsGraphicsController] content];
	[graphicsController clearWithColor:0x8000];
	
	for (DisplayWindowController *windowController in windowList)
	{
		MacDisplayViewOutput *dvo = [[windowController view] displayViewOutput];
		dvo->Dispatch(MESSAGE_RELOAD_REPROCESS_REDRAW);
	}
	
	[self setStatusText:NSSTRING_STATUS_ROM_UNLOADED];
	[self setIsWorking:NO];
	
	Slot2WindowDelegate *slot2WindowDelegate = (Slot2WindowDelegate *)[slot2WindowController content];
	[slot2WindowDelegate setAutoSelectedDeviceText:[[slot2WindowDelegate deviceManager] autoSelectedDeviceName]];
	[[slot2WindowDelegate deviceManager] updateStatus];
	
	[cdsCore setSlot1StatusText:NSSTRING_STATUS_EMULATION_NOT_RUNNING];
	[cdsCore updateCurrentSessionMACAddressString:NO];
	[[cdsCore cdsController] reset];

	ClearPokemonPlatinumTrainerBattleOverridePendingState();
	[self updatePokemonHttpApiMenuItems];
	
	result = YES;
	
	return result;
}

- (void) updateCheatDatabaseRecentsMenu:(NSNotification *)aNotification
{
	NSArray *dbRecentsList = (NSArray *)[aNotification object];
	BOOL needReleaseObject = (dbRecentsList != nil);
	
	if ( (dbRecentsList == nil) || ([dbRecentsList count] == 0) )
	{
		dbRecentsList = [[NSUserDefaults standardUserDefaults] arrayForKey:@"CheatDatabase_RecentFilePath"];
	}
	
	NSMutableArray *newRecentsList = [NSMutableArray arrayWithArray:dbRecentsList];
	
	// Note that we're relying on the notification object to retain this prior to
	// sending the notification.
	if (needReleaseObject)
	{
		[dbRecentsList release];
	}
	
	NSString *legacyFilePath = [[NSUserDefaults standardUserDefaults] stringForKey:@"R4Cheat_DatabasePath"];
	BOOL useLegacyFilePath = ( (legacyFilePath != nil) && ([legacyFilePath length] > 0) );
	
	if (useLegacyFilePath)
	{
		// We need to check if the legacy file path also exists in the recents list.
		// If it does, then the recents list version takes priority.
		for (NSDictionary *dbRecentItem in dbRecentsList)
		{
			NSString *dbRecentItemFilePath = (NSString *)[dbRecentItem valueForKey:@"FilePath"];
			if ([dbRecentItemFilePath isEqualToString:legacyFilePath])
			{
				useLegacyFilePath = NO;
				break;
			}
		}
	}
	
	if (useLegacyFilePath)
	{
		// The legacy file path must always be the first entry of the recents list.
		NSDictionary *legacyRecentItem = [NSDictionary dictionaryWithObjectsAndKeys:legacyFilePath, @"FilePath",
										  [legacyFilePath lastPathComponent], @"FileName",
										  nil];
		[newRecentsList insertObject:legacyRecentItem atIndex:0];
		
		if ([newRecentsList count] == 1)
		{
			// If the legacy file path is the only item in the recents list, then we can write it
			// back to user defaults right now.
			[[NSUserDefaults standardUserDefaults] setObject:newRecentsList forKey:@"CheatDatabase_RecentFilePath"];
		}
	}
	
	NSArray *recentsMenuItems = [cheatDatabaseRecentsMenu itemArray];
	for (NSMenuItem *menuItem in recentsMenuItems)
	{
		if ( [menuItem action] == @selector(openRecentCheatDatabase:) )
		{
			[cheatDatabaseRecentsMenu removeItem:menuItem];
		}
	}
	
	if ([newRecentsList count] > 0)
	{
		if ( ![[cheatDatabaseRecentsMenu itemAtIndex:0] isSeparatorItem] )
		{
			[cheatDatabaseRecentsMenu insertItem:[NSMenuItem separatorItem] atIndex:0];
		}
	}
	
	// Recent files are added in reverse order, in which least recent files appear below
	// more recent files in the menu. The most recent file should be at the top of the menu.
	for (NSDictionary *recentItem in newRecentsList)
	{
		NSString *menuNameString = [recentItem objectForKey:@"FileName"];
		if ( (menuNameString == nil) || ([menuNameString length] == 0) )
		{
			menuNameString = [recentItem objectForKey:@"FilePath"];
		}
		
		NSMenuItem *newMenuItem = [[[NSMenuItem alloc] initWithTitle:menuNameString action:@selector(openRecentCheatDatabase:) keyEquivalent:@""] autorelease];
		[newMenuItem setTag:[newRecentsList indexOfObject:recentItem]];
		[newMenuItem setTarget:self];
		[cheatDatabaseRecentsMenu insertItem:newMenuItem atIndex:0];
	}
}

- (void) handleNDSError:(NSNotification *)aNotification
{
	[self setIsUserInterfaceBlockingExecution:YES];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	NSString *ndsErrorString = [cdsCore errorStatus];
	const char *ndsErrorCString = [ndsErrorString cStringUsingEncoding:NSUTF8StringEncoding];
	size_t lineCount = 1;
	
	for (size_t i = 0; i < [ndsErrorString length]; i++)
	{
		if (ndsErrorCString[i] == '\n')
		{
			lineCount++;
		}
	}
	
	NSRect newSheetFrameRect = [ndsErrorSheet frame];
	newSheetFrameRect.size.height = 98.0f + (16.0f * lineCount);
	
	// For some reason, when the sheet is shown for the very first time,
	// the sheet doesn't drop down as low as it should. However, upon
	// subsequent showings, the sheet drops down to the intended distance.
	// To compensate for this difference, the first sheet showing will
	// have a little added height to it.
	if (!_isNDSErrorSheetAlreadyShown)
	{
		_isNDSErrorSheetAlreadyShown = YES;
		newSheetFrameRect.size.height += 22.0f;
	}
	
	[ndsErrorSheet setFrame:newSheetFrameRect display:NO];
	
	NSRect newTextFieldRect = [ndsErrorStatusTextField frame];
	newTextFieldRect.size.height = 16.0f * lineCount;
	[ndsErrorStatusTextField setFrame:newTextFieldRect];
	
	NSWindow *window = [[windowList objectAtIndex:0] window];
	
#if defined(MAC_OS_X_VERSION_10_9) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_9)
	if ([window respondsToSelector:@selector(beginSheet:completionHandler:)])
	{
		[window beginSheet:ndsErrorSheet
		 completionHandler:^(NSModalResponse response) {
			[self didEndErrorSheet:nil returnCode:response contextInfo:nil];
		} ];
	}
	else
#endif
	{
		SILENCE_DEPRECATION_MACOS_10_10( [NSApp beginSheet:ndsErrorSheet
											modalForWindow:window
											 modalDelegate:self
											didEndSelector:@selector(didEndErrorSheet:returnCode:contextInfo:)
											   contextInfo:nil] );
	}
}

- (void) handleEmulatorExecutionState:(NSNotification *)aNotification
{
	CocoaDSCore *cdsCore = [aNotification object];
	NSDictionary *userInfo = [aNotification userInfo];
	ExecutionBehavior execState = (ExecutionBehavior)[(NSNumber *)[userInfo valueForKey:@"ExecutionState"] integerValue];
	NSString *frameStatusString = (NSString *)[userInfo valueForKey:@"FrameStatusString"];
	
	[cdsCore setEmulationPaused:(execState == ExecutionBehavior_Pause)];
	[cdsCore setFrameStatus:frameStatusString];
}

- (void) changeCoreSpeedWithDouble:(double)newSpeedScalar
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore setSpeedScalar:newSpeedScalar];
	lastSetSpeedScalar = newSpeedScalar;
}

- (void) executeCore
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore setCoreState:ExecutionBehavior_Run];
	[self updateMicStatusIcon];
}

- (void) pauseCore
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore setCoreState:ExecutionBehavior_Pause];
	[self updateMicStatusIcon];
}

- (void) restoreCoreState
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore restoreCoreState];
	[self updateMicStatusIcon];
}

- (void) updateMicStatusIcon
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	CocoaDSController *cdsController = [cdsCore cdsController];
	NSImage *micIcon = ([self isRunningDarkMode]) ? iconMicDisabledDM : iconMicDisabled;
	
	if (![cdsCore emulationPaused])
	{
		if ([cdsController softwareMicState])
		{
			micIcon = iconMicManualOverride;
		}
		else
		{
			micIcon = iconMicIdleNoHardware;
			
			if (![cdsController hardwareMicMute])
			{
				if ([cdsController isHardwareMicAvailable])
				{
					if ([cdsController isHardwareMicInClip])
					{
						micIcon = iconMicInClip;
					}
					else if ([cdsController isHardwareMicIdle])
					{
						micIcon = iconMicIdle;
					}
					else
					{
						micIcon = iconMicActive;
					}
				}
			}
		}
	}
	
	if (micIcon != [self currentMicStatusIcon])
	{
		[self setCurrentMicStatusIcon:micIcon];
	}
	
	[ndsMicLevelIndicator setNeedsDisplay];
}

- (AudioSampleBlockGenerator *) selectedAudioFileGenerator
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	return [[cdsCore cdsController] selectedAudioFileGenerator];
}

- (void) setSelectedAudioFileGenerator:(AudioSampleBlockGenerator *)theGenerator
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[[cdsCore cdsController] setSelectedAudioFileGenerator:theGenerator];
}

- (void) didEndFileMigrationSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	NSURL *romURL = (NSURL *)contextInfo;
	NSURL *romSaveURL = [CocoaDSFile romSaveURLFromRomURL:romURL];
	
    [sheet orderOut:self];	
	
	switch (returnCode)
	{
		case GUI_RESPONSE_OK:
			[CocoaDSFile moveFileToCurrentDirectory:romSaveURL];
			break;
			
		default:
			break;
	}
	
	[self setIsUserInterfaceBlockingExecution:NO];
	[self setIsShowingFileMigrationDialog:NO];
	[self loadRomByURL:romURL asynchronous:YES];
	
	// We retained this when we initially put up the sheet, so we need to release it now.
	[romURL release];
}

- (void) didEndSaveStateSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[sheet orderOut:self];
	
	switch (returnCode)
	{
		case GUI_RESPONSE_CANCEL: // Cancel
			[self restoreCoreState];
			[self setIsUserInterfaceBlockingExecution:NO];
			[self setIsShowingSaveStateDialog:NO];
			return;
			break;
			
		case COCOA_DIALOG_DEFAULT: // Save
		{
			const BOOL isStateSaved = [CocoaDSFile saveState:[self currentSaveStateURL]];
			if (!isStateSaved)
			{
				// Throw an error here...
				[self setStatusText:NSSTRING_STATUS_SAVESTATE_SAVING_FAILED];
				return;
			}
			break;
		}
			
		case COCOA_DIALOG_OPTION: // Don't Save
			break;
			
		default:
			break;
	}
	
	[self unloadRom];
	[self setIsUserInterfaceBlockingExecution:NO];
	[self setIsShowingSaveStateDialog:NO];
}

- (void) didEndSaveStateSheetOpen:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[self didEndSaveStateSheet:sheet returnCode:returnCode contextInfo:contextInfo];
	
	NSURL *romURL = (NSURL *)contextInfo;
	[self handleLoadRomByURL:romURL];
	[romURL release];
}

- (void) didEndSaveStateSheetTerminate:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[self didEndSaveStateSheet:sheet returnCode:returnCode contextInfo:contextInfo];
	
	if (returnCode == GUI_RESPONSE_CANCEL)
	{
		[NSApp replyToApplicationShouldTerminate:NO];
	}
	else
	{
		if ([self currentSaveStateURL] == nil)
		{
			[NSApp replyToApplicationShouldTerminate:YES];
		}
	}
}

- (void) didEndChooseSlot1R4Directory:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	[sheet orderOut:self];
	
	if (returnCode == GUI_RESPONSE_CANCEL)
	{
		return;
	}
	
	NSURL *selectedDirURL = [[sheet URLs] lastObject]; //hopefully also the first object
	if(selectedDirURL == nil)
	{
		return;
	}
	
	[[NSUserDefaults standardUserDefaults] setObject:[selectedDirURL path] forKey:@"EmulationSlot1_R4StoragePath"];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore setSlot1R4URL:selectedDirURL];
}

- (void) didEndErrorSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[sheet orderOut:self];
	[self setIsUserInterfaceBlockingExecution:NO];
	
	switch (returnCode)
	{
		case COCOA_DIALOG_OPTION: // Reset
			[self reset:self];
			break;
			
		case GUI_RESPONSE_CANCEL: // Stop
		default:
			break;
	}
}

- (void) updateAllWindowTitles
{
	if ([windowList count] < 1)
	{
		return;
	}
	
	NSString *romName = nil;
	NSURL *repURL = nil;
	NSImage *titleIcon = nil;
	
	if ([self currentRom] == nil)
	{
		romName = (NSString *)[[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleName"];
	}
	else
	{
		romName = [currentRom internalName];
		repURL = [currentRom fileURL];
		titleIcon = [currentRom icon];
	}
	
	if ([windowList count] > 1)
	{
		for (DisplayWindowController *windowController in windowList)
		{
			NSString *newWindowTitle = [romName stringByAppendingFormat:@":%ld", (unsigned long)([windowList indexOfObject:windowController] + 1)];
			
			[[windowController masterWindow] setTitle:newWindowTitle];
			[[windowController masterWindow] setRepresentedURL:repURL];
			[[[windowController masterWindow] standardWindowButton:NSWindowDocumentIconButton] setImage:titleIcon];
		}
	}
	else
	{
		NSWindow *theWindow = [[windowList objectAtIndex:0] masterWindow];
		[theWindow setTitle:romName];
		[theWindow setRepresentedURL:repURL];
		[[theWindow standardWindowButton:NSWindowDocumentIconButton] setImage:titleIcon];
	}
}

- (void) updateDisplayPanelTitles
{
	// If the original panel titles haven't been saved yet, then save them now.
	if (_displayRotationPanelTitle == nil)
	{
		_displayRotationPanelTitle = [[displayRotationPanel title] copy];
		_displaySeparationPanelTitle = [[displaySeparationPanel title] copy];
		_displayVideoSettingsPanelTitle = [[displayVideoSettingsPanel title] copy];
		_displayHUDSettingsPanelTitle = [[displayHUDSettingsPanel title] copy];
	}
	
	// Set the panel titles to the window number.
	if ([windowList count] <= 1)
	{
		[displayRotationPanel setTitle:_displayRotationPanelTitle];
		[displaySeparationPanel setTitle:_displaySeparationPanelTitle];
		[displayVideoSettingsPanel setTitle:_displayVideoSettingsPanelTitle];
		[displayHUDSettingsPanel setTitle:_displayHUDSettingsPanelTitle];
	}
	else
	{
		unsigned long windowNumber = (unsigned long)[windowList indexOfObject:mainWindow] + 1;
		[displayRotationPanel setTitle:[_displayRotationPanelTitle stringByAppendingFormat:@":%ld", windowNumber]];
		[displaySeparationPanel setTitle:[_displaySeparationPanelTitle stringByAppendingFormat:@":%ld", windowNumber]];
		[displayVideoSettingsPanel setTitle:[_displayVideoSettingsPanelTitle stringByAppendingFormat:@":%ld", windowNumber]];
		[displayHUDSettingsPanel setTitle:[_displayHUDSettingsPanelTitle stringByAppendingFormat:@":%ld", windowNumber]];
	}
}

- (void) appInit
{
	[romInfoPanelController setContent:[CocoaDSRom romNotLoadedBindings]];
	[cheatWindowController setContent:[cheatWindowDelegate bindings]];
	
	// Update the SLOT-2 device list.
	Slot2WindowDelegate *slot2WindowDelegate = (Slot2WindowDelegate *)[slot2WindowController content];
	[slot2WindowDelegate update];
	[slot2WindowDelegate setHidManager:[inputManager hidManager]];
	[slot2WindowDelegate setAutoSelectedDeviceText:[[slot2WindowDelegate deviceManager] autoSelectedDeviceName]];
	
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	[cdsCore updateCurrentSessionMACAddressString:NO];
	
	CocoaGraphicsController *graphicsController = (CocoaGraphicsController *)[[self cdsGraphicsController] content];
	[screenshotCaptureToolDelegate setFetchObject:[graphicsController fetchObject]];
	[avCaptureToolDelegate setFetchObject:[graphicsController fetchObject]];
	[self fillOpenGLMSAAMenu];
}

- (void) fillOpenGLMSAAMenu
{
	CocoaGraphicsController *graphicsController = (CocoaGraphicsController *)[[self cdsGraphicsController] content];
	NSUInteger maxSamples = [graphicsController maxMultisampleSize];
	size_t itemCount = 0;
	
	while (maxSamples > 1)
	{
		itemCount++;
		maxSamples >>= 1;
	}
	
	if (itemCount <= 0)
	{
		return;
	}
	
	int itemValue = 2;
	
	for (size_t i = 0; i < itemCount; i++, itemValue<<=1)
	{
		NSString *itemTitle = [NSString stringWithFormat:@"%d", itemValue];
		[openglMSAAPopUpButton addItemWithTitle:itemTitle];
		
		NSMenuItem *menuItem = [openglMSAAPopUpButton itemAtIndex:i+1];
		[menuItem setTag:itemValue];
	}
	
	[openglMSAAPopUpButton selectItemAtIndex:0];
}

- (NSInteger) updateHostMicrophonePermissionStatus
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	CocoaDSController *cdsController = [cdsCore cdsController];
	
	NSInteger authStatus = 0;
	
#if HAVE_OSAVAILABLE && defined(MAC_OS_X_VERSION_10_14) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_14)
	if (IsOSXVersionSupported(10, 14, 0))
	{
		if (@available(macOS 10.14, *))
		{
			BOOL authFlag = NO;
			authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
			
			switch (authStatus)
			{
				case AVAuthorizationStatusNotDetermined:
					[self setMicStatusTooltip:@"Hardware microphone usage has yet to be authorized. If you need access, click \"OK\" when DeSmuME requests permission from the system."];
					break;
					
				case AVAuthorizationStatusRestricted:
					[self setMicStatusTooltip:@"Hardware microphone usage has been restricted. Please see your system administrator."];
					break;
					
				case AVAuthorizationStatusDenied:
					[self setMicStatusTooltip:@"Hardware microphone usage has been denied. If you need access, change your microphone privacy settings for DeSmuME in System Preferences."];
					break;
						
				case AVAuthorizationStatusAuthorized:
					[self setMicStatusTooltip:@""];
					authFlag = YES;
					break;
					
				default:
					[self setMicStatusTooltip:@"Hardware microphone usage is currently unknown. If you need access, change your microphone privacy settings for DeSmuME in System Preferences."];
					break;
			}
			
			[cdsController setHardwareMicAuthorization:authFlag];
		}
	}
	else
#endif
	{
		authStatus = 3; // AVAuthorizationStatusAuthorized
		[self setMicStatusTooltip:@""];
		[cdsController setHardwareMicAuthorization:YES];
	}
	
	[self updateMicStatusIcon];
	return authStatus;
}

- (void) readUserDefaults
{
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	Slot2WindowDelegate *slot2WindowDelegate = (Slot2WindowDelegate *)[slot2WindowController content];
	
	// Set up the user's default input settings.
	NSDictionary *userMappings = [[NSUserDefaults standardUserDefaults] dictionaryForKey:@"Input_ControllerMappings"];
	if (userMappings == nil)
	{
		NSDictionary *defaultKeyMappingsDict = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"DefaultKeyMappings" ofType:@"plist"]];
		NSArray *internalDefaultProfilesList = (NSArray *)[defaultKeyMappingsDict valueForKey:@"DefaultInputProfiles"];
		userMappings = [(NSDictionary *)[internalDefaultProfilesList objectAtIndex:0] valueForKey:@"Mappings"];
	}
	
	[inputManager setMappingsWithMappings:userMappings];
	[[inputManager hidManager] setDeviceListController:inputDeviceListController];
	[[inputManager hidManager] setRunLoop:[NSRunLoop currentRunLoop]];
	
	// Set the microphone settings per user preferences.
	[[cdsCore cdsController] setHardwareMicMute:[[NSUserDefaults standardUserDefaults] boolForKey:@"Microphone_HardwareMicMute"]];
	
	// Set the SPU settings per user preferences.
	[audioController setEngineIDValue:(int)[[NSUserDefaults standardUserDefaults] integerForKey:@"Sound_AudioOutputEngine"]];
	[audioController setVolumeValue:[[NSUserDefaults standardUserDefaults] floatForKey:@"Sound_Volume"]];
	[audioController setSpuAdvancedLogic:[[NSUserDefaults standardUserDefaults] boolForKey:@"SPU_AdvancedLogic"]];
	[audioController setSpuInterpolationModeIDValue:(SPUInterpolationMode)[[NSUserDefaults standardUserDefaults] integerForKey:@"SPU_InterpolationMode"]];
	[audioController setSpuSyncModeIDValue:(ESynchMode)[[NSUserDefaults standardUserDefaults] integerForKey:@"SPU_SyncMode"]];
	[audioController setSpuSyncMethodIDValue:(ESynchMethod)[[NSUserDefaults standardUserDefaults] integerForKey:@"SPU_SyncMethod"]];
	
	// Set the 3D rendering options per user preferences.
	CocoaGraphicsController *graphicsController = (CocoaGraphicsController *)[[self cdsGraphicsController] content];
	
	[graphicsController setRender3DThreads:(NSUInteger)[[NSUserDefaults standardUserDefaults] integerForKey:@"Render3D_Threads"]];
	[graphicsController setEngine3DClientIndex:[[NSUserDefaults standardUserDefaults] integerForKey:@"Render3D_RenderingEngine"]];
	[graphicsController setHighPrecisionColorInterpolationEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_HighPrecisionColorInterpolation"]];
	[graphicsController setEdgeMarkEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_EdgeMarking"]];
	[graphicsController setFogEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_Fog"]];
	[graphicsController setTextureEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_Textures"]];
	[graphicsController setLineHackEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_LineHack"]];
	[graphicsController setMultisampleSize:[[NSUserDefaults standardUserDefaults] integerForKey:@"Render3D_MultisampleSize"]];
	[graphicsController setTextureScalingFactor:(NSUInteger)[[NSUserDefaults standardUserDefaults] integerForKey:@"Render3D_TextureScalingFactor"]];
	[graphicsController setTextureDeposterizeEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_TextureDeposterize"]];
	[graphicsController setTextureSmoothingEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_TextureSmoothing"]];
	[graphicsController setFragmentSamplingHackEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_FragmentSamplingHack"]];
	[graphicsController setGpuScale:(NSUInteger)[[NSUserDefaults standardUserDefaults] integerForKey:@"Render3D_ScalingFactor"]];
	[graphicsController setGpuColorFormat:(NSUInteger)[[NSUserDefaults standardUserDefaults] integerForKey:@"Render3D_ColorFormat"]];
	
	[graphicsController setShadowPolygonEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_OpenGL_EmulateShadowPolygon"]];
	[graphicsController setSpecialZeroAlphaBlendingEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_OpenGL_EmulateSpecialZeroAlphaBlending"]];
	[graphicsController setNdsDepthCalculationEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_OpenGL_EmulateNDSDepthCalculation"]];
	[graphicsController setDepthLEqualPolygonFacingEnable:[[NSUserDefaults standardUserDefaults] boolForKey:@"Render3D_OpenGL_EmulateDepthLEqualPolygonFacing"]];
	
	((MacGPUFetchObjectAsync *)[graphicsController fetchObject])->FetchSynchronousAtIndex(0);
	
	// Set the stylus options per user preferences.
	[[cdsCore cdsController] setStylusPressure:[[NSUserDefaults standardUserDefaults] integerForKey:@"Emulation_StylusPressure"]];
	CommonSettings.loadStateWritesBackupSave = ([[NSUserDefaults standardUserDefaults] objectForKey:LoadStateWritesBackupSaveDefaultsKey] == nil) ? true : [[NSUserDefaults standardUserDefaults] boolForKey:LoadStateWritesBackupSaveDefaultsKey];
	
	// Set up the default SLOT-2 device.
	[slot2WindowDelegate setupUserDefaults];
	
	// Set up the ROM Info panel.
	[romInfoPanel setupUserDefaults];
	
	// Set up the capture tool defaults.
	[screenshotCaptureToolDelegate readUserDefaults];
	[avCaptureToolDelegate readUserDefaults];
}

- (void) writeUserDefaults
{
	[romInfoPanel writeDefaults];
	[screenshotCaptureToolDelegate writeUserDefaults];
	[avCaptureToolDelegate writeUserDefaults];
}

- (void) restoreDisplayWindowStates
{
	NSArray *windowPropertiesList = [[NSUserDefaults standardUserDefaults] arrayForKey:@"General_DisplayWindowRestorableStates"];
	const BOOL willRestoreWindows = [[NSUserDefaults standardUserDefaults] boolForKey:@"General_WillRestoreDisplayWindows"];
	
	if (!willRestoreWindows || windowPropertiesList == nil || [windowPropertiesList count] < 1)
	{
		// If no windows were saved for restoring (the user probably closed all windows before
		// app termination), then simply create a new display window per user defaults.
		[self newDisplayWindow:self];
	}
	else
	{
		for (NSDictionary *windowProperties in windowPropertiesList)
		{
			DisplayWindowController *windowController = [[DisplayWindowController alloc] initWithWindowNibName:@"DisplayWindow" emuControlDelegate:self];
			
			if (windowController == nil)
			{
				continue;
			}
			
			const NSInteger displayMode				= ([windowProperties objectForKey:@"displayMode"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"displayMode"] integerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"DisplayView_Mode"];
			const double displayScale				= ([windowProperties objectForKey:@"displayScale"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"displayScale"] doubleValue] : ([[NSUserDefaults standardUserDefaults] doubleForKey:@"DisplayView_Size"] / 100.0);
			const double displayRotation			= ([windowProperties objectForKey:@"displayRotation"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"displayRotation"] doubleValue] : [[NSUserDefaults standardUserDefaults] doubleForKey:@"DisplayView_Rotation"];
			const NSInteger displayOrientation		= ([windowProperties objectForKey:@"displayOrientation"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"displayOrientation"] integerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"DisplayViewCombo_Orientation"];
			const NSInteger displayOrder			= ([windowProperties objectForKey:@"displayOrder"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"displayOrder"] integerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"DisplayViewCombo_Order"];
			const double displayGap					= ([windowProperties objectForKey:@"displayGap"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"displayGap"] doubleValue] : ([[NSUserDefaults standardUserDefaults] doubleForKey:@"DisplayViewCombo_Gap"] / 100.0);
			
			const NSInteger displayMainSource		= ([windowProperties objectForKey:@"displayMainVideoSource"]  != nil) ? [(NSNumber *)[windowProperties valueForKey:@"displayMainVideoSource"] integerValue]  : [[NSUserDefaults standardUserDefaults] integerForKey:@"DisplayView_DisplayMainVideoSource"];
			const NSInteger displayTouchSource		= ([windowProperties objectForKey:@"displayTouchVideoSource"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"displayTouchVideoSource"] integerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"DisplayView_DisplayTouchVideoSource"];
			
			const BOOL videoFiltersPreferGPU		= ([windowProperties objectForKey:@"videoFiltersPreferGPU"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"videoFiltersPreferGPU"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"DisplayView_FiltersPreferGPU"];
			const BOOL videoSourceDeposterize		= ([windowProperties objectForKey:@"videoSourceDeposterize"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"videoSourceDeposterize"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"DisplayView_Deposterize"];
			const NSInteger videoPixelScaler		= ([windowProperties objectForKey:@"videoFilterType"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"videoFilterType"] integerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"DisplayView_VideoFilter"];
			const NSInteger videoOutputFilter		= ([windowProperties objectForKey:@"videoOutputFilter"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"videoOutputFilter"] integerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"DisplayView_OutputFilter"];
			
			const BOOL hudEnable					= ([windowProperties objectForKey:@"hudEnable"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudEnable"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"DisplayView_EnableHUD"];
			const BOOL hudShowExecutionSpeed		= ([windowProperties objectForKey:@"hudShowExecutionSpeed"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudShowExecutionSpeed"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"HUD_ShowExecutionSpeed"];
			const BOOL hudShowVideoFPS				= ([windowProperties objectForKey:@"hudShowVideoFPS"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudShowVideoFPS"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"HUD_ShowVideoFPS"];
			const BOOL hudShowRender3DFPS			= ([windowProperties objectForKey:@"hudShowRender3DFPS"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudShowRender3DFPS"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"HUD_ShowRender3DFPS"];
			const BOOL hudShowFrameIndex			= ([windowProperties objectForKey:@"hudShowFrameIndex"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudShowFrameIndex"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"HUD_ShowFrameIndex"];
			const BOOL hudShowLagFrameCount			= ([windowProperties objectForKey:@"hudShowLagFrameCount"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudShowLagFrameCount"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"HUD_ShowLagFrameCount"];
			const BOOL hudShowCPULoadAverage		= ([windowProperties objectForKey:@"hudShowCPULoadAverage"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudShowCPULoadAverage"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"HUD_ShowCPULoadAverage"];
			const BOOL hudShowRTC					= ([windowProperties objectForKey:@"hudShowRTC"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudShowRTC"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"HUD_ShowRTC"];
			const BOOL hudShowInput					= ([windowProperties objectForKey:@"hudShowInput"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudShowInput"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"HUD_ShowInput"];
			
			const NSUInteger hudColorExecutionSpeed	= ([windowProperties objectForKey:@"hudColorExecutionSpeed"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorExecutionSpeed"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_ExecutionSpeed"];
			const NSUInteger hudColorVideoFPS		= ([windowProperties objectForKey:@"hudColorVideoFPS"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorVideoFPS"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_VideoFPS"];
			const NSUInteger hudColorRender3DFPS	= ([windowProperties objectForKey:@"hudColorRender3DFPS"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorRender3DFPS"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_Render3DFPS"];
			const NSUInteger hudColorFrameIndex		= ([windowProperties objectForKey:@"hudColorFrameIndex"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorFrameIndex"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_FrameIndex"];
			const NSUInteger hudColorLagFrameCount	= ([windowProperties objectForKey:@"hudColorLagFrameCount"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorLagFrameCount"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_LagFrameCount"];
			const NSUInteger hudColorCPULoadAverage	= ([windowProperties objectForKey:@"hudColorCPULoadAverage"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorCPULoadAverage"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_CPULoadAverage"];
			const NSUInteger hudColorRTC			= ([windowProperties objectForKey:@"hudColorRTC"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorRTC"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_RTC"];
			const NSUInteger hudColorInputPendingAndApplied = ([windowProperties objectForKey:@"hudColorInputPendingAndApplied"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorInputPendingAndApplied"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_Input_PendingAndApplied"];
			const NSUInteger hudColorInputAppliedOnly = ([windowProperties objectForKey:@"hudColorInputAppliedOnly"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorInputAppliedOnly"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_Input_AppliedOnly"];
			const NSUInteger hudColorInputPendingOnly = ([windowProperties objectForKey:@"hudColorInputPendingOnly"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"hudColorInputPendingOnly"] unsignedIntegerValue] : [[NSUserDefaults standardUserDefaults] integerForKey:@"HUD_Color_Input_PendingOnly"];
			
			const BOOL useVerticalSync				= ([windowProperties objectForKey:@"useVerticalSync"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"useVerticalSync"] boolValue] : YES;
			const BOOL isMinSizeNormal				= ([windowProperties objectForKey:@"isMinSizeNormal"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"isMinSizeNormal"] boolValue] : YES;
			const BOOL isShowingStatusBar			= ([windowProperties objectForKey:@"isShowingStatusBar"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"isShowingStatusBar"] boolValue] : [[NSUserDefaults standardUserDefaults] boolForKey:@"DisplayView_ShowStatusBar"];
			const BOOL isInFullScreenMode			= ([windowProperties objectForKey:@"isInFullScreenMode"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"isInFullScreenMode"] boolValue] : NO;
			const NSUInteger screenIndex			= ([windowProperties objectForKey:@"screenIndex"] != nil) ? [(NSNumber *)[windowProperties valueForKey:@"screenIndex"] unsignedIntegerValue] : 0;
			NSString *windowFrameStr				= (NSString *)[windowProperties valueForKey:@"windowFrame"];
			
			int frameX = 0;
			int frameY = 0;
			int frameWidth = 256;
			int frameHeight = 192*2;
			const char *frameCStr = [windowFrameStr cStringUsingEncoding:NSUTF8StringEncoding];
			sscanf(frameCStr, "%i %i %i %i", &frameX, &frameY, &frameWidth, &frameHeight);
			
			// Force the window to load now so that we can overwrite its internal defaults with the user's defaults.
			[windowController window];
			
			[windowController setDisplayMode:(ClientDisplayMode)displayMode
								   viewScale:displayScale
									rotation:displayRotation
									  layout:(ClientDisplayLayout)displayOrientation
									   order:(ClientDisplayOrder)displayOrder
									gapScale:displayGap
							 isMinSizeNormal:isMinSizeNormal
						  isShowingStatusBar:isShowingStatusBar];
			
			[[windowController view] setDisplayMainVideoSource:displayMainSource];
			[[windowController view] setDisplayTouchVideoSource:displayTouchSource];
			
			[windowController setVideoPropertiesWithoutUpdateUsingPreferGPU:videoFiltersPreferGPU
														  sourceDeposterize:videoSourceDeposterize
															   outputFilter:videoOutputFilter
																pixelScaler:videoPixelScaler];
			
			[[windowController view] setUseVerticalSync:useVerticalSync];
			[[windowController view] setIsHUDVisible:hudEnable];
			[[windowController view] setIsHUDExecutionSpeedVisible:hudShowExecutionSpeed];
			[[windowController view] setIsHUDVideoFPSVisible:hudShowVideoFPS];
			[[windowController view] setIsHUDRender3DFPSVisible:hudShowRender3DFPS];
			[[windowController view] setIsHUDFrameIndexVisible:hudShowFrameIndex];
			[[windowController view] setIsHUDLagFrameCountVisible:hudShowLagFrameCount];
			[[windowController view] setIsHUDCPULoadAverageVisible:hudShowCPULoadAverage];
			[[windowController view] setIsHUDRealTimeClockVisible:hudShowRTC];
			[[windowController view] setIsHUDInputVisible:hudShowInput];
			
			[[windowController view] setHudColorExecutionSpeed:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorExecutionSpeed]];
			[[windowController view] setHudColorVideoFPS:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorVideoFPS]];
			[[windowController view] setHudColorRender3DFPS:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorRender3DFPS]];
			[[windowController view] setHudColorFrameIndex:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorFrameIndex]];
			[[windowController view] setHudColorLagFrameCount:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorLagFrameCount]];
			[[windowController view] setHudColorCPULoadAverage:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorCPULoadAverage]];
			[[windowController view] setHudColorRTC:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorRTC]];
			[[windowController view] setHudColorInputPendingAndApplied:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorInputPendingAndApplied]];
			[[windowController view] setHudColorInputAppliedOnly:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorInputAppliedOnly]];
			[[windowController view] setHudColorInputPendingOnly:[CocoaDSUtil NSColorFromRGBA8888:(uint32_t)hudColorInputPendingOnly]];
			
			[[windowController masterWindow] setFrameOrigin:NSMakePoint(frameX, frameY)];
			
			// If this is the last window in the list, make this window key and main.
			// Otherwise, just order the window to the front so that the windows will
			// stack in a deterministic order.
			[[windowController view] setAllowViewUpdates:YES];
			
			MacDisplayViewOutput *dvo = [[windowController view] displayViewOutput];
			dvo->Dispatch(MESSAGE_RELOAD_REPROCESS_REDRAW);
			
			if (windowProperties == [windowPropertiesList lastObject])
			{
				[[windowController window] makeKeyAndOrderFront:self];
				[[windowController window] makeMainWindow];
			}
			else
			{
				[[windowController window] orderFront:self];
				[[windowController window] makeMainWindow];
			}
			
			// If this window is set to full screen mode, its associated screen index must
			// exist. If not, this window will not enter full screen mode. This is necessary,
			// since the user's screen configuration could change in between app launches,
			// and since we don't want a window to go full screen on the wrong screen.
			if (isInFullScreenMode &&
				([[NSScreen screens] indexOfObject:[[windowController window] screen]] == screenIndex))
			{
				[windowController toggleFullScreenDisplay:self];
			}
		}
	}
}

- (void) saveDisplayWindowStates
{
	const BOOL willRestoreWindows = [[NSUserDefaults standardUserDefaults] boolForKey:@"General_WillRestoreDisplayWindows"];
	
	if (willRestoreWindows && [windowList count] > 0)
	{
		NSMutableArray *windowPropertiesList = [NSMutableArray arrayWithCapacity:[windowList count]];
		
		for (DisplayWindowController *windowController in windowList)
		{
			const NSUInteger screenIndex = [[NSScreen screens] indexOfObject:[[windowController masterWindow] screen]];
			
			const NSRect windowFrame = [windowController masterWindowFrame];
			NSString *windowFrameStr = [NSString stringWithFormat:@"%i %i %i %i",
										(int)windowFrame.origin.x, (int)windowFrame.origin.y, (int)windowFrame.size.width, (int)windowFrame.size.height];
			
			NSDictionary *windowProperties = [NSDictionary dictionaryWithObjectsAndKeys:
											  [NSNumber numberWithInteger:[windowController displayMode]], @"displayMode",
											  [NSNumber numberWithDouble:[windowController masterWindowScale]], @"displayScale",
											  [NSNumber numberWithDouble:[windowController displayRotation]], @"displayRotation",
											  [NSNumber numberWithInteger:[[windowController view] displayMainVideoSource]], @"displayMainVideoSource",
											  [NSNumber numberWithInteger:[[windowController view] displayTouchVideoSource]], @"displayTouchVideoSource",
											  [NSNumber numberWithInteger:[windowController displayOrientation]], @"displayOrientation",
											  [NSNumber numberWithInteger:[windowController displayOrder]], @"displayOrder",
											  [NSNumber numberWithDouble:[windowController displayGap]], @"displayGap",
											  [NSNumber numberWithBool:[[windowController view] videoFiltersPreferGPU]], @"videoFiltersPreferGPU",
											  [NSNumber numberWithInteger:[[windowController view] pixelScaler]], @"videoFilterType",
											  [NSNumber numberWithInteger:[[windowController view] outputFilter]], @"videoOutputFilter",
											  [NSNumber numberWithBool:[[windowController view] sourceDeposterize]], @"videoSourceDeposterize",
											  [NSNumber numberWithBool:[[windowController view] useVerticalSync]], @"useVerticalSync",
											  [NSNumber numberWithBool:[[windowController view] isHUDVisible]], @"hudEnable",
											  [NSNumber numberWithBool:[[windowController view] isHUDExecutionSpeedVisible]], @"hudShowExecutionSpeed",
											  [NSNumber numberWithBool:[[windowController view] isHUDVideoFPSVisible]], @"hudShowVideoFPS",
											  [NSNumber numberWithBool:[[windowController view] isHUDRender3DFPSVisible]], @"hudShowRender3DFPS",
											  [NSNumber numberWithBool:[[windowController view] isHUDFrameIndexVisible]], @"hudShowFrameIndex",
											  [NSNumber numberWithBool:[[windowController view] isHUDLagFrameCountVisible]], @"hudShowLagFrameCount",
											  [NSNumber numberWithBool:[[windowController view] isHUDCPULoadAverageVisible]], @"hudShowCPULoadAverage",
											  [NSNumber numberWithBool:[[windowController view] isHUDRealTimeClockVisible]], @"hudShowRTC",
											  [NSNumber numberWithBool:[[windowController view] isHUDInputVisible]], @"hudShowInput",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorExecutionSpeed]]], @"hudColorExecutionSpeed",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorVideoFPS]]], @"hudColorVideoFPS",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorRender3DFPS]]], @"hudColorRender3DFPS",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorFrameIndex]]], @"hudColorFrameIndex",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorLagFrameCount]]], @"hudColorLagFrameCount",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorCPULoadAverage]]], @"hudColorCPULoadAverage",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorRTC]]], @"hudColorRTC",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorInputPendingAndApplied]]], @"hudColorInputPendingAndApplied",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorInputAppliedOnly]]], @"hudColorInputAppliedOnly",
											  [NSNumber numberWithUnsignedInteger:[CocoaDSUtil RGBA8888FromNSColor:[[windowController view] hudColorInputPendingOnly]]], @"hudColorInputPendingOnly",
											  [NSNumber numberWithBool:[windowController isMinSizeNormal]], @"isMinSizeNormal",
											  [NSNumber numberWithBool:[windowController masterStatusBarState]], @"isShowingStatusBar",
											  [NSNumber numberWithBool:[windowController isFullScreen]], @"isInFullScreenMode",
											  [NSNumber numberWithUnsignedInteger:screenIndex], @"screenIndex",
											  windowFrameStr, @"windowFrame",
											  nil];
			
			[windowPropertiesList addObject:windowProperties];
		}
		
		[[NSUserDefaults standardUserDefaults] setObject:windowPropertiesList forKey:@"General_DisplayWindowRestorableStates"];
	}
	else
	{
		[[NSUserDefaults standardUserDefaults] removeObjectForKey:@"General_DisplayWindowRestorableStates"];
	}
}

- (void) handleAppearanceChange
{
	const BOOL newDarkModeState = [CocoaDSUtil determineDarkModeAppearance];
	if (newDarkModeState != [self isRunningDarkMode])
	{
		[audioController setDarkMode:newDarkModeState];
		[self setIsRunningDarkMode:newDarkModeState];
		[self updateMicStatusIcon];
	}
}

#pragma mark NSUserInterfaceValidations Protocol

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)theItem
{
	BOOL enable = YES;
    const SEL theAction = [theItem action];
	CocoaDSCore *cdsCore = (CocoaDSCore *)[cdsCoreController content];
	
	if (theAction == @selector(importRomSave:))
    {
		if ([self currentRom] == nil)
		{
			enable = NO;
		}
    }
	else if (theAction == @selector(exportRomSave:))
	{
		NSFileManager *fileManager = [[NSFileManager alloc] init];
		
		if ( ([self currentRom] == nil) ||
		     ![fileManager isReadableFileAtPath:[[CocoaDSFile fileURLFromRomURL:[[self currentRom] fileURL] toKind:@"ROM Save"] path]] )
		{
			enable = NO;
		}
		
		[fileManager release];
	}
	else if (theAction == @selector(openPokemonPartyEditor:))
	{
		if ([self currentRom] == nil)
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(openPokemonPartyStatusEditor:))
	{
		if ([self currentRom] == nil)
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(copyPokemonExportToClipboard:))
	{
		if ([self currentRom] == nil)
		{
			enable = NO;
		}
	}
    else if (theAction == @selector(toggleExecutePause:))
    {
		if (![cdsCore masterExecute] ||
			[self currentRom] == nil ||
			[self isUserInterfaceBlockingExecution])
		{
			enable = NO;
		}
		
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			if ([cdsCore coreState] == ExecutionBehavior_Pause)
			{
				[(NSMenuItem*)theItem setTitle:NSSTRING_TITLE_EXECUTE_CONTROL];
			}
			else if ([cdsCore coreState] == ExecutionBehavior_Run)
			{
				[(NSMenuItem*)theItem setTitle:NSSTRING_TITLE_PAUSE_CONTROL];
			}
		}
		else if ([(id)theItem isMemberOfClass:[NSToolbarItem class]])
		{
			if ([cdsCore coreState] == ExecutionBehavior_Pause)
			{
				[(NSToolbarItem*)theItem setLabel:NSSTRING_TITLE_EXECUTE_CONTROL];
				[(NSToolbarItem*)theItem setImage:iconExecute];
			}
			else if ([cdsCore coreState] == ExecutionBehavior_Run)
			{
				[(NSToolbarItem*)theItem setLabel:NSSTRING_TITLE_PAUSE_CONTROL];
				[(NSToolbarItem*)theItem setImage:iconPause];
			}
		}
    }
	else if (theAction == @selector(frameAdvance:))
	{
		if ([cdsCore coreState] != ExecutionBehavior_Pause ||
			![cdsCore masterExecute] ||
			[self currentRom] == nil ||
			[self isShowingSaveStateDialog])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(frameJump:))
	{
		if (![cdsCore masterExecute] ||
			[self currentRom] == nil ||
			[self isShowingSaveStateDialog])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(coreExecute:))
    {
		if ([cdsCore coreState] == ExecutionBehavior_Run ||
			[cdsCore coreState] == ExecutionBehavior_FrameAdvance ||
			![cdsCore masterExecute] ||
			[self currentRom] == nil ||
			[self isShowingSaveStateDialog])
		{
			enable = NO;
		}
    }
	else if (theAction == @selector(corePause:))
    {
		if ([cdsCore coreState] == ExecutionBehavior_Pause ||
			![cdsCore masterExecute] ||
			[self currentRom] == nil ||
			[self isShowingSaveStateDialog])
		{
			enable = NO;
		}
    }
	else if (theAction == @selector(reset:))
	{
		if ([self currentRom] == nil || [self isUserInterfaceBlockingExecution] || [cdsCore isInDebugTrap])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(loadRecentRom:))
	{
		if ([self isRomLoading] || [self isShowingSaveStateDialog] || [cdsCore isInDebugTrap])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(openRom:))
	{
		if ([self isRomLoading] || [self isShowingSaveStateDialog] || [cdsCore isInDebugTrap])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(closeRom:))
	{
		if ([self currentRom] == nil || [self isRomLoading] || [self isShowingSaveStateDialog] || [cdsCore isInDebugTrap])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(revealRomInFinder:))
	{
		if ([self currentRom] == nil || [self isRomLoading])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(loadEmuSaveStateSlot:))
	{
		if ([self currentRom] == nil ||
			[self isShowingSaveStateDialog] ||
			![CocoaDSFile saveStateExistsForSlot:[[self currentRom] fileURL] slotNumber:[theItem tag] + 1] ||
			[cdsCore isInDebugTrap])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(saveEmuSaveStateSlot:))
	{
		if ([self currentRom] == nil || [self isShowingSaveStateDialog])
		{
			enable = NO;
		}
		
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			if ([CocoaDSFile saveStateExistsForSlot:[[self currentRom] fileURL] slotNumber:[theItem tag] + 1])
			{
				[(NSMenuItem*)theItem setState:GUI_STATE_ON];
			}
			else
			{
				[(NSMenuItem*)theItem setState:GUI_STATE_OFF];
			}
		}
	}
	else if (theAction == @selector(openReplay:))
	{
		if ([self currentRom] == nil || [self isRomLoading] || [self isShowingSaveStateDialog])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(recordReplay:))
	{
		if ([self currentRom] == nil || [self isRomLoading] || [self isShowingSaveStateDialog])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(stopReplay:))
	{
		if ([self currentRom] == nil || [self isRomLoading] || [self isShowingSaveStateDialog])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(clearCheatDatabaseRecents:))
	{
		if ([cheatDatabaseRecentsMenu numberOfItems] < 2)
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(changeCoreSpeed:))
	{
		NSInteger speedScalar = (NSInteger)([cdsCore speedScalar] * 100.0);
		
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			if ([theItem tag] == -1)
			{
				if (speedScalar == (NSInteger)(SPEED_SCALAR_HALF * 100.0) ||
					speedScalar == (NSInteger)(SPEED_SCALAR_NORMAL * 100.0) ||
					speedScalar == (NSInteger)(SPEED_SCALAR_DOUBLE * 100.0))
				{
					[(NSMenuItem*)theItem setState:GUI_STATE_OFF];
				}
				else
				{
					[(NSMenuItem*)theItem setState:GUI_STATE_ON];
				}
			}
			else
			{
				[(NSMenuItem*)theItem setState:(speedScalar == [theItem tag]) ? GUI_STATE_ON : GUI_STATE_OFF];
			}
		}
		else if ([(id)theItem isMemberOfClass:[NSToolbarItem class]])
		{
			if (speedScalar == (NSInteger)(SPEED_SCALAR_DOUBLE * 100.0))
			{
				[(NSToolbarItem*)theItem setLabel:NSSTRING_TITLE_SPEED_1X];
				[(NSToolbarItem*)theItem setTag:100];
				[(NSToolbarItem*)theItem setImage:iconSpeedNormal];
			}
			else
			{
				[(NSToolbarItem*)theItem setLabel:NSSTRING_TITLE_SPEED_2X];
				[(NSToolbarItem*)theItem setTag:200];
				[(NSToolbarItem*)theItem setImage:iconSpeedDouble];
			}
		}
	}
	else if (theAction == @selector(toggleSpeedLimiter:))
	{
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			[(NSMenuItem*)theItem setTitle:([cdsCore isSpeedLimitEnabled]) ? NSSTRING_TITLE_DISABLE_SPEED_LIMIT : NSSTRING_TITLE_ENABLE_SPEED_LIMIT];
		}
	}
	else if (theAction == @selector(toggleAutoFrameSkip:))
	{
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			[(NSMenuItem*)theItem setTitle:([cdsCore isFrameSkipEnabled]) ? NSSTRING_TITLE_DISABLE_AUTO_FRAME_SKIP : NSSTRING_TITLE_ENABLE_AUTO_FRAME_SKIP];
		}
		
		if ([avCaptureToolDelegate isRecording])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(framesToSkipSetting:))
	{
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			[(NSMenuItem*)theItem setState:([cdsCore framesToSkipSetting] == [theItem tag]) ? GUI_STATE_ON : GUI_STATE_OFF];
		}
	}
	else if (theAction == @selector(toggleCheats:))
	{
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			[(NSMenuItem*)theItem setTitle:([cdsCore isCheatingEnabled]) ? NSSTRING_TITLE_DISABLE_CHEATS : NSSTRING_TITLE_ENABLE_CHEATS];
		}
	}
	else if (theAction == @selector(changeRomSaveType:))
	{
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			[(NSMenuItem*)theItem setState:([self selectedRomSaveTypeID] == [theItem tag]) ? GUI_STATE_ON : GUI_STATE_OFF];
		}
	}
	else if (theAction == @selector(toggleLoadStateWritesBackupSave:))
	{
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			[(NSMenuItem*)theItem setState:CommonSettings.loadStateWritesBackupSave ? GUI_STATE_ON : GUI_STATE_OFF];
		}
	}
	else if (theAction == @selector(openEmuSaveState:) ||
			 theAction == @selector(saveEmuSaveState:) ||
			 theAction == @selector(saveEmuSaveStateAs:))
	{
		if ([self currentRom] == nil || [self isShowingSaveStateDialog] || [cdsCore isInDebugTrap])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(revertEmuSaveState:))
	{
		if ([self currentRom] == nil ||
			[self isShowingSaveStateDialog] ||
			[self currentSaveStateURL] == nil ||
			[cdsCore isInDebugTrap])
		{
			enable = NO;
		}
	}
	else if (theAction == @selector(toggleGPUState:))
	{
		CocoaGraphicsController *graphicsController = (CocoaGraphicsController *)[[self cdsGraphicsController] content];
		
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			[(NSMenuItem*)theItem setState:([graphicsController gpuStateByBit:(UInt32)[theItem tag]]) ? GUI_STATE_ON : GUI_STATE_OFF];
		}
	}
	else if (theAction == @selector(toggleDebugInstructionTrace:))
	{
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			[(NSMenuItem*)theItem setState:([cdsCore debugInstructionTraceEnabled]) ? GUI_STATE_ON : GUI_STATE_OFF];
		}
		enable = YES;
	}
	else if (theAction == @selector(toggleDebugAutoCrashSnapshot:))
	{
		if ([(id)theItem isMemberOfClass:[NSMenuItem class]])
		{
			[(NSMenuItem*)theItem setState:([cdsCore debugAutoCrashSnapshotEnabled]) ? GUI_STATE_ON : GUI_STATE_OFF];
		}
		enable = YES;
	}
	else if (theAction == @selector(exportDebugSnapshot:) ||
			 theAction == @selector(exportDebugFullMemorySnapshot:) ||
			 theAction == @selector(revealDebugSnapshotFolder:) ||
			 theAction == @selector(prepareARM9Debugging:))
	{
		enable = YES;
	}
	else if (theAction == @selector(togglePokemonHttpApi:))
	{
		[self updatePokemonHttpApiMenuItems];
	}
	else if (theAction == @selector(togglePokemonBattleLogging:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(togglePokemonOneFrameTouchGuard:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(togglePokemonDisplayPartyAbilitySlots:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(togglePokemonSpinInPlace:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(forcePokemonHttpApiRefresh:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = desmume_pokemon_http_export::IsEnabled() ? YES : NO;
	}
	else if (theAction == @selector(selectPokemonHttpApiProfile:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(togglePokemonHttpHealthCheck:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(selectPokemonHttpHealthCheckInterval:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(togglePokemonHttpIdleRefresh:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(selectPokemonHttpIdleRefreshInterval:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(selectPokemonPlatinumTimeOverride:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = YES;
	}
	else if (theAction == @selector(setPokemonPlatinumNextTrainerBattleOverride:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = IsPokemonPlatinumTrainerOverrideAvailable() ? YES : NO;
	}
	else if (theAction == @selector(clearPokemonPlatinumNextTrainerBattleOverride:))
	{
		[self updatePokemonHttpApiMenuItems];
		enable = gPokemonPlatinumTrainerBattleOverride.hasPendingTrainerId ? YES : NO;
	}
	
	return enable;
}

#pragma mark CocoaDSControllerDelegate Protocol

- (void) doMicLevelUpdateFromController:(CocoaDSController *)cdsController
{
	[self performSelectorOnMainThread:@selector(updateMicStatusIcon) withObject:nil waitUntilDone:NO];
}

- (void) doMicHardwareStateChangedFromController:(CocoaDSController *)cdsController
									   isEnabled:(BOOL)isHardwareEnabled
										isLocked:(BOOL)isHardwareLocked
									isAuthorized:(BOOL)isAuthorized
{
	const BOOL hwMicAvailable = (isHardwareEnabled && !isHardwareLocked && isAuthorized);
	[self setIsHardwareMicAvailable:hwMicAvailable];
	[self performSelectorOnMainThread:@selector(updateMicStatusIcon) withObject:nil waitUntilDone:NO];
}

- (void) doMicHardwareGainChangedFromController:(CocoaDSController *)cdsController gain:(float)gainValue
{
	[self setCurrentMicGainValue:gainValue*100.0f];
}

@end
