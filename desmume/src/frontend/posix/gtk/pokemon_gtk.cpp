#include "pokemon_gtk.h"

#include <stdint.h>

#include <cstdlib>
#include <string.h>
#include <sstream>
#include <string>
#include <vector>

#include "NDSSystem.h"
#include "driver.h"
#include "path.h"
#include "../shared/pokemon_frontend_shared.h"

namespace {

using desmume::pokemon_frontend::BuildPartyStatusEditorRequest;
using desmume::pokemon_frontend::BuildPokemonStatusLabel;
using desmume::pokemon_frontend::GetClipboardIntervalOptions;
using desmume::pokemon_frontend::GetDefaultClipboardIntervalMs;
using desmume::pokemon_frontend::GetExportProfileOptions;
using desmume::pokemon_frontend::GetHealthCheckIntervalOptions;
using desmume::pokemon_frontend::GetIdleRefreshIntervalOptions;
using desmume::pokemon_frontend::GetPartyDisplayName;
using desmume::pokemon_frontend::GetPartyQuickStatusButtons;
using desmume::pokemon_frontend::GetPartyStatusMenuEntries;
using desmume::pokemon_frontend::GetPlatinumTimeOptions;
using desmume::pokemon_frontend::HiddenPowerTypeForEditorIVs;
using desmume::pokemon_frontend::IntervalFromTarget;
using desmume::pokemon_frontend::LoadPokemonBackendConfig;
using desmume::pokemon_frontend::NormalizeInterval;
using desmume::pokemon_frontend::PlatinumTimeFromTarget;
using desmume::pokemon_frontend::PrepareClipboardUtf8Text;
using desmume::pokemon_frontend::ProfileFromTarget;
using desmume::pokemon_frontend::SavePokemonBackendConfig;
using desmume::pokemon_frontend::TargetForInterval;
using desmume::pokemon_frontend::TargetForPlatinumTime;
using desmume::pokemon_frontend::TargetForProfile;

struct PokemonPartyEditorWidgets {
	GtkWidget *dialog;
	GtkBuilder *builder;
	GtkComboBoxText *sourceCombo;
	GtkComboBoxText *boxCombo;
	GtkComboBoxText *slotCombo;
	GtkEntry *summaryEntry;
	GtkSpinButton *levelSpin;
	GtkSpinButton *hpSpin;
	GtkSpinButton *expSpin;
	GtkComboBox *speciesCombo;
	GtkComboBox *itemCombo;
	GtkComboBox *abilityCombo;
	GtkSpinButton *friendshipSpin;
	GtkComboBoxText *statusCombo;
	GtkSpinButton *ivSpins[6];
	GtkSpinButton *evSpins[6];
	GtkComboBox *moveCombos[4];
	GtkWidget *applyButton;
	GtkWidget *edgeButton;
	GtkWidget *edgePartyButton;
	GtkWidget *bedtimeButton;
	GtkLabel *statusLabel;
	bool busy;
	desmume_pokemon_http_export::PartyEditorSnapshot partySnapshot;
	desmume_pokemon_http_export::BoxEditorSnapshot boxSnapshot;

	PokemonPartyEditorWidgets()
		: dialog(NULL)
		, builder(NULL)
		, sourceCombo(NULL)
		, boxCombo(NULL)
		, slotCombo(NULL)
		, summaryEntry(NULL)
		, levelSpin(NULL)
		, hpSpin(NULL)
		, expSpin(NULL)
		, speciesCombo(NULL)
		, itemCombo(NULL)
		, abilityCombo(NULL)
		, friendshipSpin(NULL)
		, statusCombo(NULL)
		, applyButton(NULL)
		, edgeButton(NULL)
		, edgePartyButton(NULL)
		, bedtimeButton(NULL)
		, statusLabel(NULL)
		, busy(false) {
		for (int i = 0; i < 6; ++i) {
			ivSpins[i] = NULL;
			evSpins[i] = NULL;
		}
		for (int i = 0; i < 4; ++i) {
			moveCombos[i] = NULL;
		}
		partySnapshot.success = false;
		boxSnapshot.success = false;
		boxSnapshot.currentBox = 1;
	}
};

struct PokemonPartyStatusEditorRow {
	GtkLabel *nameLabel;
	GtkSpinButton *hpSpin;
	GtkLabel *maxHpLabel;
	std::vector<GtkToggleButton *> statusButtons;

	PokemonPartyStatusEditorRow()
		: nameLabel(NULL)
		, hpSpin(NULL)
		, maxHpLabel(NULL) {
	}
};

struct PokemonPartyStatusEditorWidgets {
	GtkWidget *dialog;
	GtkBuilder *builder;
	GtkGrid *rowsGrid;
	GtkLabel *statusLabel;
	GtkWidget *bedtimeButton;
	GtkWidget *applyButton;
	std::vector<PokemonPartyStatusEditorRow> rows;
	bool busy;
	desmume_pokemon_http_export::PartyEditorSnapshot partySnapshot;

	PokemonPartyStatusEditorWidgets()
		: dialog(NULL)
		, builder(NULL)
		, rowsGrid(NULL)
		, statusLabel(NULL)
		, bedtimeButton(NULL)
		, applyButton(NULL)
		, busy(false) {
		partySnapshot.success = false;
	}
};

struct BattleLogPathDialogWidgets {
	GtkWidget *dialog;
	GtkCheckButton *customCheck;
	GtkEntry *pathEntry;
	GtkLabel *outputPathLabel;
	GtkLabel *statusLabel;
	GtkWidget *browseButton;
	GtkWidget *testButton;

	BattleLogPathDialogWidgets()
		: dialog(NULL)
		, customCheck(NULL)
		, pathEntry(NULL)
		, outputPathLabel(NULL)
		, statusLabel(NULL)
		, browseButton(NULL)
		, testButton(NULL) {
	}
};

struct PokemonGtkState {
	GtkApplication *app;
	GtkWindow *window;
	GtkWidget *statusBar;
	desmume::config::Config *config;
	GMenu *pokemonStatusSection;
	guint statusTimerId;
	bool initialized;
	bool clipboardAutoCopyEnabled;
	uint64_t clipboardIntervalMs;
	uint64_t lastBattleEndUnixMs;
	uint64_t lastRefreshCheckUnixMs;
	std::string lastExportText;
	PokemonPartyEditorWidgets *partyEditor;
	PokemonPartyStatusEditorWidgets *partyStatusEditor;

	PokemonGtkState()
		: app(NULL)
		, window(NULL)
		, statusBar(NULL)
		, config(NULL)
		, pokemonStatusSection(NULL)
		, statusTimerId(0)
		, initialized(false)
		, clipboardAutoCopyEnabled(true)
		, clipboardIntervalMs(GetDefaultClipboardIntervalMs())
		, lastBattleEndUnixMs(0u)
		, lastRefreshCheckUnixMs(0u)
		, partyEditor(NULL)
		, partyStatusEditor(NULL) {
	}
};

PokemonGtkState gPokemonState;

GtkEntry *GetComboEntry(GtkComboBox *combo) {
	GtkWidget *child = gtk_bin_get_child(GTK_BIN(combo));
	return GTK_IS_ENTRY(child) ? GTK_ENTRY(child) : NULL;
}

void SetStatusBarText(const std::string &text) {
	if (gPokemonState.statusBar == NULL) {
		return;
	}

	const guint contextId = gtk_statusbar_get_context_id(GTK_STATUSBAR(gPokemonState.statusBar), "Pokemon");
	gtk_statusbar_pop(GTK_STATUSBAR(gPokemonState.statusBar), contextId);
	gtk_statusbar_push(GTK_STATUSBAR(gPokemonState.statusBar), contextId, text.c_str());
}

void LogPokemonMessage(const std::string &text, bool updateStatusBar) {
	if (updateStatusBar) {
		SetStatusBarText(text);
	}
	if (driver != NULL) {
		driver->AddLine(text.c_str());
	}
}

void ShowPokemonErrorDialog(const std::string &message) {
	if (gPokemonState.window == NULL) {
		LogPokemonMessage(message, true);
		return;
	}

	GtkWidget *dialog = gtk_message_dialog_new(
		gPokemonState.window,
		static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_CLOSE,
		"%s",
		message.c_str());
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

GSimpleAction *LookupAction(const char *name) {
	if (gPokemonState.app == NULL) {
		return NULL;
	}

	return G_SIMPLE_ACTION(g_action_map_lookup_action(G_ACTION_MAP(gPokemonState.app), name));
}

void SetActionEnabled(const char *name, bool enabled) {
	GSimpleAction *action = LookupAction(name);
	if (action != NULL) {
		g_simple_action_set_enabled(action, enabled ? TRUE : FALSE);
	}
}

void SetBooleanActionState(const char *name, bool value) {
	GSimpleAction *action = LookupAction(name);
	if (action != NULL) {
		g_simple_action_set_state(action, g_variant_new_boolean(value ? TRUE : FALSE));
	}
}

void SetStringActionState(const char *name, const char *value) {
	GSimpleAction *action = LookupAction(name);
	if (action != NULL) {
		g_simple_action_set_state(action, g_variant_new_string(value));
	}
}

bool GetBooleanActionState(GSimpleAction *action) {
	if (action == NULL) {
		return false;
	}

	GVariant *state = g_action_get_state(G_ACTION(action));
	if (state == NULL) {
		return false;
	}

	const bool value = g_variant_get_boolean(state);
	g_variant_unref(state);
	return value;
}

void MirrorPokemonConfig() {
	if (gPokemonState.config == NULL) {
		return;
	}

	SavePokemonBackendConfig(
		*gPokemonState.config,
		gPokemonState.clipboardAutoCopyEnabled,
		gPokemonState.clipboardIntervalMs);
	gPokemonState.config->pokemon_touch_tap_guard_enabled = NDS_isOneFrameTouchPressGuardEnabled();
}

void SetBattleLogPathSetting(const std::string &configuredPath) {
	strncpy(path.pathToBattleLogs, configuredPath.c_str(), sizeof(path.pathToBattleLogs) - 1);
	path.pathToBattleLogs[sizeof(path.pathToBattleLogs) - 1] = '\0';
}

std::string GetBattleLogDialogConfiguredPath(BattleLogPathDialogWidgets *widgets) {
	if (widgets == NULL || !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->customCheck))) {
		return "";
	}

	return gtk_entry_get_text(widgets->pathEntry);
}

std::string GetBattleLogDialogTestPath(BattleLogPathDialogWidgets *widgets) {
	const std::string configuredPath = GetBattleLogDialogConfiguredPath(widgets);
	return configuredPath.empty() ? GetDefaultBattleLogOutputPath() : configuredPath;
}

void UpdateBattleLogPathDialogState(BattleLogPathDialogWidgets *widgets) {
	if (widgets == NULL) {
		return;
	}

	const bool useCustomPath = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->customCheck));
	gtk_widget_set_sensitive(GTK_WIDGET(widgets->pathEntry), useCustomPath ? TRUE : FALSE);
	gtk_widget_set_sensitive(widgets->browseButton, useCustomPath ? TRUE : FALSE);

	const std::string outputPath = useCustomPath
		? GetBattleLogDialogTestPath(widgets)
		: GetDefaultBattleLogOutputPath();
	gtk_label_set_text(widgets->outputPathLabel, outputPath.c_str());
}

void BattleLogPathCustomToggled(GtkToggleButton *, gpointer userData) {
	UpdateBattleLogPathDialogState(static_cast<BattleLogPathDialogWidgets *>(userData));
}

void BattleLogPathBrowseClicked(GtkButton *, gpointer userData) {
	BattleLogPathDialogWidgets *widgets = static_cast<BattleLogPathDialogWidgets *>(userData);
	if (widgets == NULL) {
		return;
	}

	GtkFileChooserNative *chooser = gtk_file_chooser_native_new(
		"Select Battle Log Directory",
		GTK_WINDOW(widgets->dialog),
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
		"_Select",
		"_Cancel");

	const std::string currentPath = GetBattleLogDialogTestPath(widgets);
	if (!currentPath.empty()) {
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(chooser), currentPath.c_str());
	} else {
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(chooser), g_get_home_dir());
	}

	const int response = gtk_native_dialog_run(GTK_NATIVE_DIALOG(chooser));
	if (response == GTK_RESPONSE_ACCEPT) {
		GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(chooser));
		if (file != NULL) {
			gchar *selectedPath = g_file_get_path(file);
			if (selectedPath != NULL) {
				gtk_entry_set_text(widgets->pathEntry, selectedPath);
				g_free(selectedPath);
				UpdateBattleLogPathDialogState(widgets);
			}
			g_object_unref(file);
		}
	}

	g_object_unref(chooser);
}

void BattleLogPathTestClicked(GtkButton *, gpointer userData) {
	BattleLogPathDialogWidgets *widgets = static_cast<BattleLogPathDialogWidgets *>(userData);
	if (widgets == NULL) {
		return;
	}

	std::string resolvedPath;
	std::string error;
	if (TestBattleLogOutputPath(GetBattleLogDialogTestPath(widgets), resolvedPath, error)) {
		std::string message = "Write test succeeded: " + resolvedPath;
		gtk_label_set_text(widgets->statusLabel, message.c_str());
		return;
	}

	std::string message = "Write test failed: " + error;
	gtk_label_set_text(widgets->statusLabel, message.c_str());
}

bool SaveBattleLogPathDialog(BattleLogPathDialogWidgets *widgets) {
	const bool useCustomPath = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->customCheck));
	const std::string configuredPath = useCustomPath ? gtk_entry_get_text(widgets->pathEntry) : "";
	if (useCustomPath && configuredPath.empty()) {
		gtk_label_set_text(widgets->statusLabel, "Choose a battle log directory or disable the custom path.");
		return false;
	}

	SetBattleLogPathSetting(configuredPath);
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();

	std::string message = configuredPath.empty()
		? ("Battle log path reset to default: " + GetDefaultBattleLogOutputPath())
		: ("Battle log path set to: " + GetBattleLogOutputPath());
	LogPokemonMessage(message, true);
	return true;
}

void OpenBattleLogPathDialog() {
	BattleLogPathDialogWidgets widgets;
	widgets.dialog = gtk_dialog_new_with_buttons(
		"Battle Log Path",
		gPokemonState.window,
		static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		"_Save",
		GTK_RESPONSE_ACCEPT,
		NULL);

	GtkWidget *contentArea = gtk_dialog_get_content_area(GTK_DIALOG(widgets.dialog));
	GtkWidget *grid = gtk_grid_new();
	gtk_widget_set_margin_top(grid, 12);
	gtk_widget_set_margin_bottom(grid, 12);
	gtk_widget_set_margin_start(grid, 12);
	gtk_widget_set_margin_end(grid, 12);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
	gtk_container_add(GTK_CONTAINER(contentArea), grid);

	widgets.customCheck = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Use custom battle log directory"));
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.customCheck), 0, 0, 3, 1);

	widgets.pathEntry = GTK_ENTRY(gtk_entry_new());
	gtk_entry_set_width_chars(widgets.pathEntry, 48);
	gtk_entry_set_text(widgets.pathEntry, path.pathToBattleLogs);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.pathEntry), 0, 1, 2, 1);

	widgets.browseButton = gtk_button_new_with_label("Browse...");
	gtk_grid_attach(GTK_GRID(grid), widgets.browseButton, 2, 1, 1, 1);

	GtkWidget *outputLabel = gtk_label_new("Output path:");
	gtk_widget_set_halign(outputLabel, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), outputLabel, 0, 2, 1, 1);

	widgets.outputPathLabel = GTK_LABEL(gtk_label_new(""));
	gtk_widget_set_halign(GTK_WIDGET(widgets.outputPathLabel), GTK_ALIGN_START);
	gtk_label_set_selectable(widgets.outputPathLabel, TRUE);
	gtk_label_set_line_wrap(widgets.outputPathLabel, TRUE);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.outputPathLabel), 1, 2, 2, 1);

	widgets.testButton = gtk_button_new_with_label("Test Write");
	gtk_grid_attach(GTK_GRID(grid), widgets.testButton, 0, 3, 1, 1);

	widgets.statusLabel = GTK_LABEL(gtk_label_new(""));
	gtk_widget_set_halign(GTK_WIDGET(widgets.statusLabel), GTK_ALIGN_START);
	gtk_label_set_line_wrap(widgets.statusLabel, TRUE);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.statusLabel), 1, 3, 2, 1);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets.customCheck), path.pathToBattleLogs[0] != '\0');
	g_signal_connect(widgets.customCheck, "toggled", G_CALLBACK(BattleLogPathCustomToggled), &widgets);
	g_signal_connect(widgets.browseButton, "clicked", G_CALLBACK(BattleLogPathBrowseClicked), &widgets);
	g_signal_connect(widgets.testButton, "clicked", G_CALLBACK(BattleLogPathTestClicked), &widgets);
	UpdateBattleLogPathDialogState(&widgets);

	gtk_widget_show_all(widgets.dialog);
	for (;;) {
		const int response = gtk_dialog_run(GTK_DIALOG(widgets.dialog));
		if (response != GTK_RESPONSE_ACCEPT) {
			break;
		}
		if (SaveBattleLogPathDialog(&widgets)) {
			break;
		}
	}

	gtk_widget_destroy(widgets.dialog);
}

void ResetClipboardAutoSyncState() {
	gPokemonState.lastBattleEndUnixMs = 0u;
	gPokemonState.lastRefreshCheckUnixMs = 0u;
	gPokemonState.lastExportText.clear();
}

void SetClipboardAutoCopyEnabledInternal(bool enabled) {
	gPokemonState.clipboardAutoCopyEnabled = enabled;
	desmume_pokemon_http_export::SetClipboardSyncEnabled(enabled);
	if (!enabled) {
		ResetClipboardAutoSyncState();
	}
}

void SetClipboardIntervalInternal(uint64_t intervalMs) {
	gPokemonState.clipboardIntervalMs = (intervalMs == 0u)
		? GetDefaultClipboardIntervalMs()
		: intervalMs;
}

bool WritePokemonClipboardText(const std::string &utf8Text, std::string &errorOut) {
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == NULL) {
		errorOut = "Failed to access the clipboard";
		return false;
	}

	gtk_clipboard_set_text(clipboard, utf8Text.c_str(), static_cast<gint>(utf8Text.size()));
	gtk_clipboard_store(clipboard);
	errorOut.clear();
	return true;
}

bool BuildAndCopyPokemonExportText(std::string &showdownTextOut, std::string &errorOut) {
	if (!desmume_pokemon_http_export::GetShowdownExportText(showdownTextOut, errorOut)) {
		return false;
	}

	std::string normalizedText;
	if (!PrepareClipboardUtf8Text(showdownTextOut, normalizedText, errorOut)) {
		return false;
	}

	if (!WritePokemonClipboardText(normalizedText, errorOut)) {
		return false;
	}

	showdownTextOut.swap(normalizedText);
	return true;
}

void NotePokemonClipboardCopy(const desmume_pokemon_http_export::Status &status,
							  const std::string &showdownText,
							  uint64_t nowMs) {
	gPokemonState.lastBattleEndUnixMs = status.battleDetection.lastBattleEndUnixMs;
	gPokemonState.lastRefreshCheckUnixMs = nowMs;
	gPokemonState.lastExportText = showdownText;
}

uint64_t CurrentUnixMs() {
	return static_cast<uint64_t>(g_get_real_time() / 1000);
}

void SetPokemonStatusMenuLabel(const std::string &label) {
	if (gPokemonState.pokemonStatusSection == NULL) {
		return;
	}

	while (g_menu_model_get_n_items(G_MENU_MODEL(gPokemonState.pokemonStatusSection)) > 0) {
		g_menu_remove(gPokemonState.pokemonStatusSection, 0);
	}

	GMenuItem *item = g_menu_item_new(label.c_str(), "app.pokemon_status");
	g_menu_insert_item(gPokemonState.pokemonStatusSection, 0, item);
	g_object_unref(item);
}

bool PartyEditorBoxMode(PokemonPartyEditorWidgets *editor) {
	const gchar *activeId = gtk_combo_box_get_active_id(GTK_COMBO_BOX(editor->sourceCombo));
	return (activeId != NULL) && (strcmp(activeId, "box") == 0);
}

int GetComboActiveInt(GtkComboBox *combo, int fallbackValue) {
	const gchar *activeId = gtk_combo_box_get_active_id(combo);
	if (activeId == NULL || activeId[0] == '\0') {
		return fallbackValue;
	}
	return atoi(activeId);
}

void SetComboEntryText(GtkComboBox *combo, const std::string &text) {
	GtkEntry *entry = GetComboEntry(combo);
	if (entry != NULL) {
		gtk_entry_set_text(entry, text.c_str());
	}
}

std::string GetComboEntryText(GtkComboBox *combo) {
	GtkEntry *entry = GetComboEntry(combo);
	if (entry == NULL) {
		return "";
	}
	return gtk_entry_get_text(entry);
}

void SetComboActiveInt(GtkComboBox *combo, int value) {
	std::ostringstream id;
	id << value;
	gtk_combo_box_set_active_id(combo, id.str().c_str());
}

void PopulateComboEntryOptions(GtkComboBoxText *combo, const std::vector<std::string> &options) {
	const std::string preservedText = GetComboEntryText(GTK_COMBO_BOX(combo));
	gtk_combo_box_text_remove_all(combo);
	for (size_t i = 0; i < options.size(); ++i) {
		gtk_combo_box_text_append_text(combo, options[i].c_str());
	}
	SetComboEntryText(GTK_COMBO_BOX(combo), preservedText);
}

void SetPartyEditorStatus(PokemonPartyEditorWidgets *editor, const std::string &text) {
	gtk_label_set_text(editor->statusLabel, text.c_str());
}

void SetPartyStatusEditorStatus(PokemonPartyStatusEditorWidgets *editor, const std::string &text) {
	gtk_label_set_text(editor->statusLabel, text.c_str());
}

void PopulateStatusCombo(PokemonPartyEditorWidgets *editor) {
	const std::vector<desmume::pokemon_frontend::PartyStatusMenuEntry> &options = GetPartyStatusMenuEntries();
	gtk_combo_box_text_remove_all(editor->statusCombo);
	for (size_t i = 0; i < options.size(); ++i) {
		std::ostringstream id;
		id << options[i].raw;
		gtk_combo_box_text_append(editor->statusCombo, id.str().c_str(), options[i].label);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(editor->statusCombo), 0);
}

void SetStatusComboRaw(PokemonPartyEditorWidgets *editor, uint32_t statusRaw) {
	std::ostringstream id;
	id << statusRaw;
	if (!gtk_combo_box_set_active_id(GTK_COMBO_BOX(editor->statusCombo), id.str().c_str())) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(editor->statusCombo), 0);
	}
}

void PopulatePartyEditorAutocomplete(PokemonPartyEditorWidgets *editor) {
	PopulateComboEntryOptions(GTK_COMBO_BOX_TEXT(editor->speciesCombo), desmume_pokemon_http_export::GetSpeciesNameOptions());
	PopulateComboEntryOptions(GTK_COMBO_BOX_TEXT(editor->itemCombo), desmume_pokemon_http_export::GetItemNameOptions());
	PopulateComboEntryOptions(GTK_COMBO_BOX_TEXT(editor->abilityCombo), desmume_pokemon_http_export::GetAbilityNameOptions());
	const std::vector<std::string> moveOptions = desmume_pokemon_http_export::GetMoveNameOptions();
	for (int i = 0; i < 4; ++i) {
		PopulateComboEntryOptions(GTK_COMBO_BOX_TEXT(editor->moveCombos[i]), moveOptions);
	}
}

void ClearPartyEditorFields(PokemonPartyEditorWidgets *editor) {
	gtk_entry_set_text(editor->summaryEntry, "");
	gtk_spin_button_set_value(editor->levelSpin, 1.0);
	gtk_spin_button_set_value(editor->hpSpin, 0.0);
	gtk_spin_button_set_value(editor->expSpin, 0.0);
	SetComboEntryText(editor->speciesCombo, "");
	SetComboEntryText(editor->itemCombo, "");
	SetComboEntryText(editor->abilityCombo, "");
	gtk_spin_button_set_value(editor->friendshipSpin, 0.0);
	SetStatusComboRaw(editor, 0u);
	for (int i = 0; i < 6; ++i) {
		gtk_spin_button_set_value(editor->ivSpins[i], 0.0);
		gtk_spin_button_set_value(editor->evSpins[i], 0.0);
	}
	for (int i = 0; i < 4; ++i) {
		SetComboEntryText(editor->moveCombos[i], "");
	}
}

int GetSelectedPartyIndex(PokemonPartyEditorWidgets *editor) {
	const int selectedSlot = GetComboActiveInt(GTK_COMBO_BOX(editor->slotCombo), -1);
	if (selectedSlot < 1) {
		return -1;
	}

	for (size_t i = 0; i < editor->partySnapshot.party.size(); ++i) {
		if (editor->partySnapshot.party[i].slot == selectedSlot) {
			return static_cast<int>(i);
		}
	}

	return -1;
}

int GetSelectedBoxIndex(PokemonPartyEditorWidgets *editor) {
	const int selectedBox = GetComboActiveInt(GTK_COMBO_BOX(editor->boxCombo), -1);
	const int selectedSlot = GetComboActiveInt(GTK_COMBO_BOX(editor->slotCombo), -1);
	if (selectedBox < 1 || selectedSlot < 1) {
		return -1;
	}

	for (size_t i = 0; i < editor->boxSnapshot.pokemon.size(); ++i) {
		const desmume_pokemon_http_export::BoxPokemonSummary &summary = editor->boxSnapshot.pokemon[i];
		if (summary.box == selectedBox && summary.slot == selectedSlot) {
			return static_cast<int>(i);
		}
	}

	return -1;
}

void UpdatePartyEditorControls(PokemonPartyEditorWidgets *editor) {
	const bool boxMode = PartyEditorBoxMode(editor);
	const bool hasSelection = boxMode ? (GetSelectedBoxIndex(editor) >= 0) : (GetSelectedPartyIndex(editor) >= 0);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->boxCombo), boxMode ? TRUE : FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->slotCombo), hasSelection ? TRUE : FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->levelSpin), (hasSelection && boxMode) ? TRUE : FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->expSpin), (hasSelection && boxMode) ? TRUE : FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->hpSpin), (hasSelection && !boxMode) ? TRUE : FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->statusCombo), (hasSelection && !boxMode) ? TRUE : FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->speciesCombo), (hasSelection && boxMode) ? TRUE : FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->itemCombo), hasSelection ? TRUE : FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->abilityCombo), hasSelection ? TRUE : FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(editor->friendshipSpin), hasSelection ? TRUE : FALSE);
	for (int i = 0; i < 6; ++i) {
		gtk_widget_set_sensitive(GTK_WIDGET(editor->ivSpins[i]), (hasSelection && boxMode) ? TRUE : FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(editor->evSpins[i]), (hasSelection && boxMode) ? TRUE : FALSE);
	}
	for (int i = 0; i < 4; ++i) {
		gtk_widget_set_sensitive(GTK_WIDGET(editor->moveCombos[i]), hasSelection ? TRUE : FALSE);
	}
	gtk_widget_set_sensitive(editor->applyButton, hasSelection ? TRUE : FALSE);
	gtk_widget_set_sensitive(editor->edgeButton, hasSelection ? TRUE : FALSE);
	gtk_widget_set_sensitive(editor->edgePartyButton, (!boxMode && !editor->partySnapshot.party.empty()) ? TRUE : FALSE);
	gtk_widget_set_sensitive(editor->bedtimeButton, (!boxMode && !editor->partySnapshot.party.empty()) ? TRUE : FALSE);
}

void RebuildPartyEditorBoxMenu(PokemonPartyEditorWidgets *editor, int selectedBox) {
	gtk_combo_box_text_remove_all(editor->boxCombo);
	const int boxCount = (editor->boxSnapshot.boxCount > 0) ? editor->boxSnapshot.boxCount : 14;
	for (int box = 1; box <= boxCount; ++box) {
		std::ostringstream id;
		std::ostringstream label;
		id << box;
		label << "Box " << box;
		gtk_combo_box_text_append(editor->boxCombo, id.str().c_str(), label.str().c_str());
	}

	if (selectedBox < 1 || selectedBox > boxCount) {
		selectedBox = (editor->boxSnapshot.currentBox >= 1 && editor->boxSnapshot.currentBox <= boxCount)
			? editor->boxSnapshot.currentBox
			: 1;
	}
	SetComboActiveInt(GTK_COMBO_BOX(editor->boxCombo), selectedBox);
}

void RebuildPartyEditorSlotMenu(PokemonPartyEditorWidgets *editor, int selectedSlot, int selectedBox, bool rebuildBoxMenu = true) {
	gtk_combo_box_text_remove_all(editor->slotCombo);
	if (PartyEditorBoxMode(editor)) {
		if (rebuildBoxMenu) {
			RebuildPartyEditorBoxMenu(editor, selectedBox);
		}
		selectedBox = GetComboActiveInt(GTK_COMBO_BOX(editor->boxCombo), selectedBox);
		for (size_t i = 0; i < editor->boxSnapshot.pokemon.size(); ++i) {
			const desmume_pokemon_http_export::BoxPokemonSummary &summary = editor->boxSnapshot.pokemon[i];
			if (summary.box != selectedBox) {
				continue;
			}
			std::ostringstream id;
			std::ostringstream label;
			id << summary.slot;
			label << "Slot " << summary.slot << " - " << summary.species << " Lv" << summary.level;
			gtk_combo_box_text_append(editor->slotCombo, id.str().c_str(), label.str().c_str());
		}
	} else {
		for (size_t i = 0; i < editor->partySnapshot.party.size(); ++i) {
			const desmume_pokemon_http_export::PartyPokemonSummary &summary = editor->partySnapshot.party[i];
			std::ostringstream id;
			std::ostringstream label;
			id << summary.slot;
			label << "Slot " << summary.slot << " - " << summary.species << " Lv" << summary.level
			      << " HP " << summary.currentHp << "/" << summary.maxHp;
			gtk_combo_box_text_append(editor->slotCombo, id.str().c_str(), label.str().c_str());
		}
	}

	std::ostringstream selectedSlotId;
	selectedSlotId << selectedSlot;
	if (!gtk_combo_box_set_active_id(GTK_COMBO_BOX(editor->slotCombo), selectedSlotId.str().c_str())) {
		GtkTreeModel *slotModel = gtk_combo_box_get_model(GTK_COMBO_BOX(editor->slotCombo));
		if (slotModel != NULL && gtk_tree_model_iter_n_children(slotModel, NULL) > 0) {
			gtk_combo_box_set_active(GTK_COMBO_BOX(editor->slotCombo), 0);
		}
	}
}

void UpdatePartyEditorLevelFromSelection(PokemonPartyEditorWidgets *editor) {
	if (!PartyEditorBoxMode(editor)) {
		return;
	}
	const int selectedIndex = GetSelectedBoxIndex(editor);
	if (selectedIndex < 0) {
		return;
	}

	const desmume_pokemon_http_export::BoxPokemonSummary &summary = editor->boxSnapshot.pokemon[static_cast<size_t>(selectedIndex)];
	int level = gtk_spin_button_get_value_as_int(editor->levelSpin);
	if (level < 1) {
		level = 1;
	} else if (level > 100) {
		level = 100;
	}

	uint16_t speciesId = summary.speciesId;
	std::string canonicalName;
	if (!desmume_pokemon_http_export::LookupSpeciesIdByName(GetComboEntryText(editor->speciesCombo), speciesId, canonicalName) || speciesId == 0u) {
		speciesId = summary.speciesId;
	}

	uint32_t experience = 0;
	if (desmume_pokemon_http_export::GetExperienceForSpeciesLevel(speciesId, level, experience)) {
		gtk_spin_button_set_value(editor->levelSpin, level);
		gtk_spin_button_set_value(editor->expSpin, static_cast<double>(experience));
	}
}

void UpdatePartyEditorDetailFields(PokemonPartyEditorWidgets *editor) {
	if (editor->busy) {
		return;
	}
	editor->busy = true;

	const bool boxMode = PartyEditorBoxMode(editor);
	const int selectedIndex = boxMode ? GetSelectedBoxIndex(editor) : GetSelectedPartyIndex(editor);
	if (selectedIndex < 0) {
		ClearPartyEditorFields(editor);
		UpdatePartyEditorControls(editor);
		editor->busy = false;
		return;
	}

	if (boxMode) {
		const desmume_pokemon_http_export::BoxPokemonSummary &summary = editor->boxSnapshot.pokemon[static_cast<size_t>(selectedIndex)];
		std::ostringstream summaryText;
		summaryText << "Box " << summary.box << " Slot " << summary.slot << "  Level " << summary.level << "  ";
		if (summary.hasNickname && !summary.nickname.empty()) {
			summaryText << summary.nickname << " (" << summary.species << ")";
		} else {
			summaryText << summary.species;
		}
		gtk_entry_set_text(editor->summaryEntry, summaryText.str().c_str());
		gtk_spin_button_set_value(editor->levelSpin, summary.level);
		gtk_spin_button_set_value(editor->hpSpin, 0.0);
		gtk_spin_button_set_value(editor->expSpin, summary.experience);
		SetComboEntryText(editor->speciesCombo, summary.species);
		SetComboEntryText(editor->itemCombo, summary.heldItem);
		SetComboEntryText(editor->abilityCombo, summary.ability);
		gtk_spin_button_set_value(editor->friendshipSpin, summary.friendship);
		SetStatusComboRaw(editor, 0u);
		for (int i = 0; i < 6; ++i) {
			gtk_spin_button_set_value(editor->ivSpins[i], summary.stats.ivs[i]);
			gtk_spin_button_set_value(editor->evSpins[i], summary.stats.evs[i]);
		}
		for (int i = 0; i < 4; ++i) {
			SetComboEntryText(editor->moveCombos[i], summary.moves[i].name);
		}
	} else {
		const desmume_pokemon_http_export::PartyPokemonSummary &summary = editor->partySnapshot.party[static_cast<size_t>(selectedIndex)];
		std::ostringstream summaryText;
		summaryText << "Level " << summary.level
		            << "  HP " << summary.currentHp << "/" << summary.maxHp
		            << "  Status: " << summary.statusLabel << "  ";
		if (summary.hasNickname && !summary.nickname.empty()) {
			summaryText << summary.nickname << " (" << summary.species << ")";
		} else {
			summaryText << summary.species;
		}
		gtk_entry_set_text(editor->summaryEntry, summaryText.str().c_str());
		gtk_spin_button_set_value(editor->levelSpin, summary.level);
		gtk_spin_button_set_value(editor->hpSpin, summary.currentHp);
		gtk_spin_button_set_value(editor->expSpin, summary.experience);
		SetComboEntryText(editor->speciesCombo, summary.species);
		SetComboEntryText(editor->itemCombo, summary.heldItem);
		SetComboEntryText(editor->abilityCombo, summary.ability);
		gtk_spin_button_set_value(editor->friendshipSpin, summary.friendship);
		SetStatusComboRaw(editor, summary.statusRaw);
		for (int i = 0; i < 6; ++i) {
			gtk_spin_button_set_value(editor->ivSpins[i], summary.stats.ivs[i]);
			gtk_spin_button_set_value(editor->evSpins[i], summary.stats.evs[i]);
		}
		for (int i = 0; i < 4; ++i) {
			SetComboEntryText(editor->moveCombos[i], summary.moves[i].name);
		}
	}

	UpdatePartyEditorControls(editor);
	editor->busy = false;
}

void ReloadPartyEditorData(PokemonPartyEditorWidgets *editor) {
	if (editor == NULL || editor->busy) {
		return;
	}

	editor->busy = true;
	const bool boxMode = PartyEditorBoxMode(editor);
	const int previousSlot = GetComboActiveInt(GTK_COMBO_BOX(editor->slotCombo), -1);
	int previousBox = GetComboActiveInt(GTK_COMBO_BOX(editor->boxCombo), -1);
	PopulatePartyEditorAutocomplete(editor);

	if (boxMode) {
		desmume_pokemon_http_export::BoxEditorSnapshot snapshot;
		if (!desmume_pokemon_http_export::GetBoxEditorSnapshot(snapshot)) {
			editor->boxSnapshot = snapshot;
			RebuildPartyEditorBoxMenu(editor, previousBox);
			gtk_combo_box_text_remove_all(editor->slotCombo);
			SetPartyEditorStatus(editor, snapshot.error);
			ClearPartyEditorFields(editor);
			UpdatePartyEditorControls(editor);
			editor->busy = false;
			return;
		}

		editor->boxSnapshot = snapshot;
		if (previousBox < 1 || previousBox > snapshot.boxCount) {
			previousBox = snapshot.currentBox;
		}
		RebuildPartyEditorSlotMenu(editor, previousSlot, previousBox);
		const std::string statusText = snapshot.pokemon.empty()
			? std::string("Box is empty")
			: (std::string(desmume_pokemon_http_export::GetProfileDisplayName(desmume_pokemon_http_export::GetSelectedProfile())) + " box ready");
		SetPartyEditorStatus(editor, statusText);
	} else {
		desmume_pokemon_http_export::PartyEditorSnapshot snapshot;
		if (!desmume_pokemon_http_export::GetPartyEditorSnapshot(snapshot)) {
			desmume_pokemon_http_export::PartyEditorSnapshot retrySnapshot;
			if (desmume_pokemon_http_export::GetPartyEditorSnapshot(retrySnapshot)) {
				snapshot = retrySnapshot;
			}
		}

		editor->partySnapshot = snapshot;
		if (!snapshot.success) {
			gtk_combo_box_text_remove_all(editor->slotCombo);
			SetPartyEditorStatus(editor, snapshot.error);
			ClearPartyEditorFields(editor);
			UpdatePartyEditorControls(editor);
			editor->busy = false;
			return;
		}

		RebuildPartyEditorSlotMenu(editor, previousSlot, previousBox);
		const std::string statusText = snapshot.party.empty()
			? std::string("Party is empty")
			: (std::string(desmume_pokemon_http_export::GetProfileDisplayName(desmume_pokemon_http_export::GetSelectedProfile())) + " party ready");
		SetPartyEditorStatus(editor, statusText);
	}

	editor->busy = false;
	UpdatePartyEditorDetailFields(editor);
}

void ReportPartyEditorError(PokemonPartyEditorWidgets *editor, const std::string &message) {
	SetPartyEditorStatus(editor, message);
	ShowPokemonErrorDialog(message);
}

void HandlePartyEditorApply(PokemonPartyEditorWidgets *editor) {
	if (editor == NULL) {
		return;
	}

	const bool boxMode = PartyEditorBoxMode(editor);
	const int selectedIndex = boxMode ? GetSelectedBoxIndex(editor) : GetSelectedPartyIndex(editor);
	if (selectedIndex < 0) {
		return;
	}

	uint16_t moveIds[4] = {};
	uint16_t speciesId = 0;
	uint16_t heldItemId = 0;
	uint16_t abilityId = 0;
	std::string canonicalName;
	if (!desmume_pokemon_http_export::LookupSpeciesIdByName(GetComboEntryText(editor->speciesCombo), speciesId, canonicalName) || speciesId == 0u) {
		ReportPartyEditorError(editor, "Unknown species name");
		return;
	}
	if (!desmume_pokemon_http_export::LookupItemIdByName(GetComboEntryText(editor->itemCombo), heldItemId, canonicalName)) {
		ReportPartyEditorError(editor, "Unknown held item name");
		return;
	}
	if (!desmume_pokemon_http_export::LookupAbilityIdByName(GetComboEntryText(editor->abilityCombo), abilityId, canonicalName) || abilityId == 0u) {
		ReportPartyEditorError(editor, "Unknown ability name");
		return;
	}
	for (int i = 0; i < 4; ++i) {
		if (!desmume_pokemon_http_export::LookupMoveIdByName(GetComboEntryText(editor->moveCombos[i]), moveIds[i], canonicalName)) {
			std::ostringstream error;
			error << "Unknown move name in slot " << (i + 1);
			ReportPartyEditorError(editor, error.str());
			return;
		}
	}

	const uint8_t friendship = static_cast<uint8_t>(gtk_spin_button_get_value_as_int(editor->friendshipSpin));
	std::string error;
	if (boxMode) {
		const desmume_pokemon_http_export::BoxPokemonSummary &summary = editor->boxSnapshot.pokemon[static_cast<size_t>(selectedIndex)];
		desmume_pokemon_http_export::BoxPokemonEditRequest request = {};
		request.box = summary.box;
		request.slot = summary.slot;
		request.speciesId = speciesId;
		request.heldItemId = heldItemId;
		request.abilityId = abilityId;
		request.friendship = friendship;
		for (int i = 0; i < 6; ++i) {
			request.ivs[i] = static_cast<uint8_t>(gtk_spin_button_get_value_as_int(editor->ivSpins[i]));
			request.evs[i] = static_cast<uint8_t>(gtk_spin_button_get_value_as_int(editor->evSpins[i]));
		}
		int level = gtk_spin_button_get_value_as_int(editor->levelSpin);
		if (level < 1) {
			level = 1;
		} else if (level > 100) {
			level = 100;
		}
		if (level != summary.level || speciesId != summary.speciesId) {
			uint32_t experience = 0;
			if (!desmume_pokemon_http_export::GetExperienceForSpeciesLevel(speciesId, level, experience)) {
				ReportPartyEditorError(editor, "Failed to resolve EXP for requested level");
				return;
			}
			request.experience = experience;
			gtk_spin_button_set_value(editor->expSpin, experience);
			gtk_spin_button_set_value(editor->levelSpin, level);
		} else {
			request.experience = static_cast<uint32_t>(gtk_spin_button_get_value_as_int(editor->expSpin));
		}
		for (int i = 0; i < 4; ++i) {
			request.moveIds[i] = moveIds[i];
		}
		if (!desmume_pokemon_http_export::UpdateBoxPokemon(request, error)) {
			ReportPartyEditorError(editor, error);
			return;
		}
		ReloadPartyEditorData(editor);
		SetPartyEditorStatus(editor, "Box Pokemon updated");
		return;
	}

	const desmume_pokemon_http_export::PartyPokemonSummary &summary = editor->partySnapshot.party[static_cast<size_t>(selectedIndex)];
	desmume_pokemon_http_export::PartyPokemonEditRequest request = {};
	request.slot = summary.slot;
	request.currentHp = gtk_spin_button_get_value_as_int(editor->hpSpin);
	request.statusRaw = static_cast<uint32_t>(GetComboActiveInt(GTK_COMBO_BOX(editor->statusCombo), 0));
	if (request.statusRaw == summary.statusRaw) {
		request.encodedStatusRaw = summary.encodedStatusRaw;
		request.preserveEncodedStatusRaw = true;
	}
	request.experience = summary.experience;
	request.speciesId = summary.speciesId;
	request.heldItemId = heldItemId;
	request.abilityId = abilityId;
	request.friendship = friendship;
	for (int i = 0; i < 6; ++i) {
		request.ivs[i] = summary.stats.ivs[i];
		request.evs[i] = summary.stats.evs[i];
	}
	for (int i = 0; i < 4; ++i) {
		request.moveIds[i] = moveIds[i];
	}
	if (!desmume_pokemon_http_export::UpdatePartyPokemon(request, error)) {
		ReportPartyEditorError(editor, error);
		return;
	}
	ReloadPartyEditorData(editor);
	SetPartyEditorStatus(editor, "Party Pokemon updated");
}

void HandlePartyEditorEdge(PokemonPartyEditorWidgets *editor) {
	if (editor == NULL) {
		return;
	}
	const bool boxMode = PartyEditorBoxMode(editor);
	const int selectedIndex = boxMode ? GetSelectedBoxIndex(editor) : GetSelectedPartyIndex(editor);
	if (selectedIndex < 0) {
		return;
	}

	std::string error;
	if (boxMode) {
		const desmume_pokemon_http_export::BoxPokemonSummary &summary = editor->boxSnapshot.pokemon[static_cast<size_t>(selectedIndex)];
		if (!desmume_pokemon_http_export::EdgeBoxPokemon(summary.box, summary.slot, 1u, error)) {
			ReportPartyEditorError(editor, error);
			return;
		}
		ReloadPartyEditorData(editor);
		SetPartyEditorStatus(editor, "Box Pokemon edged to one XP before next level");
		return;
	}

	const int slot = editor->partySnapshot.party[static_cast<size_t>(selectedIndex)].slot;
	if (!desmume_pokemon_http_export::EdgePartyPokemon(slot, 1u, error)) {
		ReportPartyEditorError(editor, error);
		return;
	}
	ReloadPartyEditorData(editor);
	SetPartyEditorStatus(editor, "Party Pokemon edged to one XP before next level");
}

void HandlePartyEditorEdgeParty(PokemonPartyEditorWidgets *editor) {
	if (editor == NULL || PartyEditorBoxMode(editor)) {
		return;
	}

	std::string error;
	if (!desmume_pokemon_http_export::EdgeEntireParty(1u, error)) {
		ReportPartyEditorError(editor, error);
		return;
	}
	ReloadPartyEditorData(editor);
	SetPartyEditorStatus(editor, "Entire party edged to one XP before the next level");
}

void HandlePartyEditorBedtime(PokemonPartyEditorWidgets *editor) {
	if (editor == NULL || PartyEditorBoxMode(editor)) {
		return;
	}

	std::string error;
	if (!desmume_pokemon_http_export::SetEntirePartyStatus(1u, error)) {
		ReportPartyEditorError(editor, error);
		return;
	}
	ReloadPartyEditorData(editor);
	SetPartyEditorStatus(editor, "Entire party set to 1-turn sleep");
}

void ClearPartyStatusEditorRow(PokemonPartyStatusEditorWidgets *editor, size_t rowIndex) {
	PokemonPartyStatusEditorRow &row = editor->rows[rowIndex];
	gtk_label_set_text(row.nameLabel, "");
	gtk_spin_button_set_value(row.hpSpin, 0.0);
	gtk_label_set_text(row.maxHpLabel, "");
	gtk_widget_set_sensitive(GTK_WIDGET(row.hpSpin), FALSE);
	for (size_t i = 0; i < row.statusButtons.size(); ++i) {
		gtk_toggle_button_set_active(row.statusButtons[i], FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(row.statusButtons[i]), FALSE);
	}
}

void UpdatePartyStatusEditorRows(PokemonPartyStatusEditorWidgets *editor) {
	gtk_widget_set_sensitive(editor->bedtimeButton, editor->partySnapshot.party.empty() ? FALSE : TRUE);
	gtk_widget_set_sensitive(editor->applyButton, editor->partySnapshot.party.empty() ? FALSE : TRUE);
	for (size_t rowIndex = 0; rowIndex < editor->rows.size(); ++rowIndex) {
		if (rowIndex >= editor->partySnapshot.party.size()) {
			ClearPartyStatusEditorRow(editor, rowIndex);
			continue;
		}

		const desmume_pokemon_http_export::PartyPokemonSummary &summary = editor->partySnapshot.party[rowIndex];
		PokemonPartyStatusEditorRow &row = editor->rows[rowIndex];
		std::ostringstream label;
		label << summary.slot << ". " << GetPartyDisplayName(summary)
		      << "  HP " << HiddenPowerTypeForEditorIVs(summary.stats.ivs);
		gtk_label_set_text(row.nameLabel, label.str().c_str());
		gtk_spin_button_set_value(row.hpSpin, summary.currentHp);
		gtk_widget_set_sensitive(GTK_WIDGET(row.hpSpin), TRUE);
		std::ostringstream maxHp;
		maxHp << "/ " << summary.maxHp;
		gtk_label_set_text(row.maxHpLabel, maxHp.str().c_str());

		const std::vector<desmume::pokemon_frontend::PartyQuickStatusButtonEntry> &buttons = GetPartyQuickStatusButtons();
		for (size_t i = 0; i < row.statusButtons.size(); ++i) {
			gtk_widget_set_sensitive(GTK_WIDGET(row.statusButtons[i]), TRUE);
			gtk_toggle_button_set_active(row.statusButtons[i], buttons[i].raw == summary.statusRaw);
		}
	}
}

void ReloadPartyStatusEditorData(PokemonPartyStatusEditorWidgets *editor) {
	if (editor == NULL || editor->busy) {
		return;
	}

	editor->busy = true;
	desmume_pokemon_http_export::PartyEditorSnapshot snapshot;
	if (!desmume_pokemon_http_export::GetPartyEditorSnapshot(snapshot)) {
		desmume_pokemon_http_export::PartyEditorSnapshot retrySnapshot;
		if (desmume_pokemon_http_export::GetPartyEditorSnapshot(retrySnapshot)) {
			snapshot = retrySnapshot;
		}
	}

	editor->partySnapshot = snapshot;
	UpdatePartyStatusEditorRows(editor);
	if (!snapshot.success) {
		SetPartyStatusEditorStatus(editor, snapshot.error);
		editor->busy = false;
		return;
	}

	const std::string statusText = snapshot.party.empty()
		? std::string("Party is empty")
		: (std::string(desmume_pokemon_http_export::GetProfileDisplayName(desmume_pokemon_http_export::GetSelectedProfile())) + " party ready");
	SetPartyStatusEditorStatus(editor, statusText);
	editor->busy = false;
}

void ReportPartyStatusEditorError(PokemonPartyStatusEditorWidgets *editor, const std::string &message) {
	SetPartyStatusEditorStatus(editor, message);
	ShowPokemonErrorDialog(message);
}

void ApplyPartyStatusEditorUpdate(PokemonPartyStatusEditorWidgets *editor, size_t rowIndex, int currentHp, uint32_t statusRaw) {
	if (editor == NULL || rowIndex >= editor->partySnapshot.party.size()) {
		return;
	}

	const desmume_pokemon_http_export::PartyPokemonSummary &summary = editor->partySnapshot.party[rowIndex];
	const desmume_pokemon_http_export::PartyPokemonEditRequest request =
		BuildPartyStatusEditorRequest(summary, currentHp, statusRaw, false);
	std::string error;
	if (!desmume_pokemon_http_export::UpdatePartyPokemon(request, error)) {
		ReportPartyStatusEditorError(editor, error);
		return;
	}

	ReloadPartyStatusEditorData(editor);
	SetPartyStatusEditorStatus(editor, GetPartyDisplayName(summary) + " updated");
}

void ApplyPartyStatusEditorAllHp(PokemonPartyStatusEditorWidgets *editor) {
	if (editor == NULL) {
		return;
	}

	bool didChange = false;
	for (size_t rowIndex = 0; rowIndex < editor->partySnapshot.party.size(); ++rowIndex) {
		const desmume_pokemon_http_export::PartyPokemonSummary &summary = editor->partySnapshot.party[rowIndex];
		const int currentHp = gtk_spin_button_get_value_as_int(editor->rows[rowIndex].hpSpin);
		if (currentHp == summary.currentHp) {
			continue;
		}

		const desmume_pokemon_http_export::PartyPokemonEditRequest request =
			BuildPartyStatusEditorRequest(summary, currentHp, summary.statusRaw, true);
		std::string error;
		if (!desmume_pokemon_http_export::UpdatePartyPokemon(request, error)) {
			ReportPartyStatusEditorError(editor, error);
			return;
		}

		didChange = true;
	}

	ReloadPartyStatusEditorData(editor);
	SetPartyStatusEditorStatus(editor, didChange ? "Party HP updated" : "Party HP unchanged");
}

void ApplyPartyStatusEditorBedtime(PokemonPartyStatusEditorWidgets *editor) {
	if (editor == NULL) {
		return;
	}

	std::string error;
	if (!desmume_pokemon_http_export::SetEntirePartyStatus(1u, error)) {
		ReportPartyStatusEditorError(editor, error);
		return;
	}

	ReloadPartyStatusEditorData(editor);
	const desmume_pokemon_http_export::ExportGameProfile selectedProfile = desmume_pokemon_http_export::GetSelectedProfile();
	if ((selectedProfile == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK_WHITE) ||
	    (selectedProfile == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLACK2_WHITE2) ||
	    (selectedProfile == desmume_pokemon_http_export::EXPORT_GAME_POKEMON_BLAZE_BLACK2_VOLT_WHITE2_REDUX)) {
		SetPartyStatusEditorStatus(editor, "Entire party put to sleep");
	} else {
		SetPartyStatusEditorStatus(editor, "Entire party set to 1-turn sleep");
	}
}

gboolean PokemonTimerTick(gpointer) {
	PokemonGtkUpdateMenuState();

	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	if (!gPokemonState.clipboardAutoCopyEnabled || !status.romLoaded || !status.supportedGame) {
		ResetClipboardAutoSyncState();
		return TRUE;
	}

	const bool battleActive = status.battleDetection.battleActive;
	const bool battleEnded = !battleActive &&
		(status.battleDetection.lastBattleEndUnixMs != 0u) &&
		(status.battleDetection.lastBattleEndUnixMs != gPokemonState.lastBattleEndUnixMs);
	if (battleActive) {
		return TRUE;
	}

	const uint64_t nowMs = CurrentUnixMs();
	if (!battleEnded &&
		(gPokemonState.lastRefreshCheckUnixMs != 0u) &&
		((nowMs - gPokemonState.lastRefreshCheckUnixMs) < gPokemonState.clipboardIntervalMs)) {
		return TRUE;
	}

	std::string showdownText;
	std::string error;
	if (!desmume_pokemon_http_export::GetShowdownExportText(showdownText, error)) {
		return TRUE;
	}

	std::string normalizedText;
	if (!PrepareClipboardUtf8Text(showdownText, normalizedText, error)) {
		return TRUE;
	}

	if (!battleEnded && normalizedText == gPokemonState.lastExportText) {
		gPokemonState.lastRefreshCheckUnixMs = nowMs;
		return TRUE;
	}

	if (!WritePokemonClipboardText(normalizedText, error)) {
		return TRUE;
	}

	NotePokemonClipboardCopy(status, normalizedText, nowMs);
	return TRUE;
}

void PartyEditorSourceChanged(GtkComboBox *, gpointer userData) {
	ReloadPartyEditorData(static_cast<PokemonPartyEditorWidgets *>(userData));
}

void PartyEditorBoxChanged(GtkComboBox *, gpointer userData) {
	PokemonPartyEditorWidgets *editor = static_cast<PokemonPartyEditorWidgets *>(userData);
	if (editor == NULL || editor->busy) {
		return;
	}
	if (!PartyEditorBoxMode(editor) || !editor->boxSnapshot.success) {
		return;
	}

	editor->busy = true;
	RebuildPartyEditorSlotMenu(
		editor,
		GetComboActiveInt(GTK_COMBO_BOX(editor->slotCombo), -1),
		GetComboActiveInt(GTK_COMBO_BOX(editor->boxCombo), editor->boxSnapshot.currentBox),
		false);
	editor->busy = false;
	UpdatePartyEditorDetailFields(editor);
}

void PartyEditorSlotChanged(GtkComboBox *, gpointer userData) {
	PokemonPartyEditorWidgets *editor = static_cast<PokemonPartyEditorWidgets *>(userData);
	if (editor == NULL || editor->busy) {
		return;
	}
	UpdatePartyEditorDetailFields(editor);
}

void PartyEditorLevelChanged(GtkSpinButton *, gpointer userData) {
	PokemonPartyEditorWidgets *editor = static_cast<PokemonPartyEditorWidgets *>(userData);
	if (editor == NULL || editor->busy) {
		return;
	}
	UpdatePartyEditorLevelFromSelection(editor);
}

void PartyEditorRefreshClicked(GtkButton *, gpointer userData) {
	ReloadPartyEditorData(static_cast<PokemonPartyEditorWidgets *>(userData));
}

void PartyEditorApplyClicked(GtkButton *, gpointer userData) {
	HandlePartyEditorApply(static_cast<PokemonPartyEditorWidgets *>(userData));
}

void PartyEditorEdgeClicked(GtkButton *, gpointer userData) {
	HandlePartyEditorEdge(static_cast<PokemonPartyEditorWidgets *>(userData));
}

void PartyEditorEdgePartyClicked(GtkButton *, gpointer userData) {
	HandlePartyEditorEdgeParty(static_cast<PokemonPartyEditorWidgets *>(userData));
}

void PartyEditorBedtimeClicked(GtkButton *, gpointer userData) {
	HandlePartyEditorBedtime(static_cast<PokemonPartyEditorWidgets *>(userData));
}

void PartyEditorCloseClicked(GtkButton *, gpointer userData) {
	gtk_widget_destroy(static_cast<PokemonPartyEditorWidgets *>(userData)->dialog);
}

void PartyEditorDestroyed(GtkWidget *, gpointer userData) {
	PokemonPartyEditorWidgets *editor = static_cast<PokemonPartyEditorWidgets *>(userData);
	if (editor == NULL) {
		return;
	}
	if (editor->builder != NULL) {
		g_object_unref(editor->builder);
	}
	delete editor;
	gPokemonState.partyEditor = NULL;
}

void PartyStatusEditorRefreshClicked(GtkButton *, gpointer userData) {
	ReloadPartyStatusEditorData(static_cast<PokemonPartyStatusEditorWidgets *>(userData));
}

void PartyStatusEditorBedtimeClicked(GtkButton *, gpointer userData) {
	ApplyPartyStatusEditorBedtime(static_cast<PokemonPartyStatusEditorWidgets *>(userData));
}

void PartyStatusEditorApplyClicked(GtkButton *, gpointer userData) {
	ApplyPartyStatusEditorAllHp(static_cast<PokemonPartyStatusEditorWidgets *>(userData));
}

void PartyStatusEditorCloseClicked(GtkButton *, gpointer userData) {
	gtk_widget_destroy(static_cast<PokemonPartyStatusEditorWidgets *>(userData)->dialog);
}

void PartyStatusEditorDestroyed(GtkWidget *, gpointer userData) {
	PokemonPartyStatusEditorWidgets *editor = static_cast<PokemonPartyStatusEditorWidgets *>(userData);
	if (editor == NULL) {
		return;
	}
	if (editor->builder != NULL) {
		g_object_unref(editor->builder);
	}
	delete editor;
	gPokemonState.partyStatusEditor = NULL;
}

void PartyStatusButtonToggled(GtkToggleButton *button, gpointer userData) {
	PokemonPartyStatusEditorWidgets *editor = static_cast<PokemonPartyStatusEditorWidgets *>(userData);
	if (editor == NULL || editor->busy || !gtk_toggle_button_get_active(button)) {
		return;
	}

	const size_t rowIndex = static_cast<size_t>(GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "row-index")));
	const uint32_t statusRaw = static_cast<uint32_t>(GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "status-raw")));
	if (rowIndex >= editor->rows.size() || rowIndex >= editor->partySnapshot.party.size()) {
		return;
	}
	const int currentHp = gtk_spin_button_get_value_as_int(editor->rows[rowIndex].hpSpin);
	ApplyPartyStatusEditorUpdate(editor, rowIndex, currentHp, statusRaw);
}

void CreatePartyStatusEditorRows(PokemonPartyStatusEditorWidgets *editor) {
	const std::vector<desmume::pokemon_frontend::PartyQuickStatusButtonEntry> &buttons = GetPartyQuickStatusButtons();
	for (int rowIndex = 0; rowIndex < 6; ++rowIndex) {
		PokemonPartyStatusEditorRow row;
		row.nameLabel = GTK_LABEL(gtk_label_new(""));
		gtk_widget_set_halign(GTK_WIDGET(row.nameLabel), GTK_ALIGN_START);
		gtk_grid_attach(editor->rowsGrid, GTK_WIDGET(row.nameLabel), 0, rowIndex + 1, 1, 1);

		row.hpSpin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.0, 999.0, 1.0));
		gtk_grid_attach(editor->rowsGrid, GTK_WIDGET(row.hpSpin), 1, rowIndex + 1, 1, 1);

		row.maxHpLabel = GTK_LABEL(gtk_label_new(""));
		gtk_widget_set_halign(GTK_WIDGET(row.maxHpLabel), GTK_ALIGN_START);
		gtk_grid_attach(editor->rowsGrid, GTK_WIDGET(row.maxHpLabel), 2, rowIndex + 1, 1, 1);

		GSList *group = NULL;
		for (size_t statusIndex = 0; statusIndex < buttons.size(); ++statusIndex) {
			GtkWidget *button = gtk_radio_button_new_with_label(group, buttons[statusIndex].label);
			group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
			g_object_set_data(G_OBJECT(button), "row-index", GUINT_TO_POINTER(rowIndex));
			g_object_set_data(G_OBJECT(button), "status-raw", GUINT_TO_POINTER(buttons[statusIndex].raw));
			g_signal_connect(button, "toggled", G_CALLBACK(PartyStatusButtonToggled), editor);
			gtk_grid_attach(editor->rowsGrid, button, static_cast<gint>(3 + statusIndex), rowIndex + 1, 1, 1);
			row.statusButtons.push_back(GTK_TOGGLE_BUTTON(button));
		}

		editor->rows.push_back(row);
	}
}

PokemonPartyEditorWidgets *BuildPartyEditorWindow() {
	GtkBuilder *builder = gtk_builder_new_from_resource("/org/desmume/DeSmuME/pokemon_party_editor.ui");
	PokemonPartyEditorWidgets *editor = new PokemonPartyEditorWidgets();
	editor->builder = builder;
	editor->dialog = GTK_WIDGET(gtk_builder_get_object(builder, "dialog"));
	editor->sourceCombo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "source_combo"));
	editor->boxCombo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "box_combo"));
	editor->slotCombo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "slot_combo"));
	editor->summaryEntry = GTK_ENTRY(gtk_builder_get_object(builder, "summary_entry"));
	editor->levelSpin = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "level_spin"));
	editor->hpSpin = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "hp_spin"));
	editor->expSpin = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "exp_spin"));
	editor->speciesCombo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "species_combo"));
	editor->itemCombo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "item_combo"));
	editor->abilityCombo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "ability_combo"));
	editor->friendshipSpin = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "friendship_spin"));
	editor->statusCombo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "status_combo"));
	editor->ivSpins[0] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "iv_hp_spin"));
	editor->ivSpins[1] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "iv_atk_spin"));
	editor->ivSpins[2] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "iv_def_spin"));
	editor->ivSpins[3] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "iv_spa_spin"));
	editor->ivSpins[4] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "iv_spd_spin"));
	editor->ivSpins[5] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "iv_spe_spin"));
	editor->evSpins[0] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "ev_hp_spin"));
	editor->evSpins[1] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "ev_atk_spin"));
	editor->evSpins[2] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "ev_def_spin"));
	editor->evSpins[3] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "ev_spa_spin"));
	editor->evSpins[4] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "ev_spd_spin"));
	editor->evSpins[5] = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "ev_spe_spin"));
	editor->moveCombos[0] = GTK_COMBO_BOX(gtk_builder_get_object(builder, "move1_combo"));
	editor->moveCombos[1] = GTK_COMBO_BOX(gtk_builder_get_object(builder, "move2_combo"));
	editor->moveCombos[2] = GTK_COMBO_BOX(gtk_builder_get_object(builder, "move3_combo"));
	editor->moveCombos[3] = GTK_COMBO_BOX(gtk_builder_get_object(builder, "move4_combo"));
	editor->applyButton = GTK_WIDGET(gtk_builder_get_object(builder, "apply_button"));
	editor->edgeButton = GTK_WIDGET(gtk_builder_get_object(builder, "edge_button"));
	editor->edgePartyButton = GTK_WIDGET(gtk_builder_get_object(builder, "edge_party_button"));
	editor->bedtimeButton = GTK_WIDGET(gtk_builder_get_object(builder, "bedtime_button"));
	editor->statusLabel = GTK_LABEL(gtk_builder_get_object(builder, "status_label"));

	gtk_window_set_transient_for(GTK_WINDOW(editor->dialog), gPokemonState.window);
	gtk_combo_box_text_append(editor->sourceCombo, "party", "Party");
	gtk_combo_box_text_append(editor->sourceCombo, "box", "Box");
	gtk_combo_box_set_active_id(GTK_COMBO_BOX(editor->sourceCombo), "party");
	PopulateStatusCombo(editor);
	gtk_editable_set_editable(GTK_EDITABLE(editor->summaryEntry), FALSE);
	g_signal_connect(editor->dialog, "destroy", G_CALLBACK(PartyEditorDestroyed), editor);
	g_signal_connect(editor->sourceCombo, "changed", G_CALLBACK(PartyEditorSourceChanged), editor);
	g_signal_connect(editor->boxCombo, "changed", G_CALLBACK(PartyEditorBoxChanged), editor);
	g_signal_connect(editor->slotCombo, "changed", G_CALLBACK(PartyEditorSlotChanged), editor);
	g_signal_connect(editor->levelSpin, "value-changed", G_CALLBACK(PartyEditorLevelChanged), editor);
	g_signal_connect(gtk_builder_get_object(builder, "refresh_button"), "clicked", G_CALLBACK(PartyEditorRefreshClicked), editor);
	g_signal_connect(gtk_builder_get_object(builder, "apply_button"), "clicked", G_CALLBACK(PartyEditorApplyClicked), editor);
	g_signal_connect(gtk_builder_get_object(builder, "edge_button"), "clicked", G_CALLBACK(PartyEditorEdgeClicked), editor);
	g_signal_connect(gtk_builder_get_object(builder, "edge_party_button"), "clicked", G_CALLBACK(PartyEditorEdgePartyClicked), editor);
	g_signal_connect(gtk_builder_get_object(builder, "bedtime_button"), "clicked", G_CALLBACK(PartyEditorBedtimeClicked), editor);
	g_signal_connect(gtk_builder_get_object(builder, "close_button"), "clicked", G_CALLBACK(PartyEditorCloseClicked), editor);
	PopulatePartyEditorAutocomplete(editor);
	ReloadPartyEditorData(editor);
	return editor;
}

PokemonPartyStatusEditorWidgets *BuildPartyStatusEditorWindow() {
	GtkBuilder *builder = gtk_builder_new_from_resource("/org/desmume/DeSmuME/pokemon_party_status_editor.ui");
	PokemonPartyStatusEditorWidgets *editor = new PokemonPartyStatusEditorWidgets();
	editor->builder = builder;
	editor->dialog = GTK_WIDGET(gtk_builder_get_object(builder, "dialog"));
	editor->rowsGrid = GTK_GRID(gtk_builder_get_object(builder, "rows_grid"));
	editor->statusLabel = GTK_LABEL(gtk_builder_get_object(builder, "status_label"));
	editor->bedtimeButton = GTK_WIDGET(gtk_builder_get_object(builder, "bedtime_button"));
	editor->applyButton = GTK_WIDGET(gtk_builder_get_object(builder, "apply_button"));
	gtk_window_set_transient_for(GTK_WINDOW(editor->dialog), gPokemonState.window);
	CreatePartyStatusEditorRows(editor);
	g_signal_connect(editor->dialog, "destroy", G_CALLBACK(PartyStatusEditorDestroyed), editor);
	g_signal_connect(gtk_builder_get_object(builder, "refresh_button"), "clicked", G_CALLBACK(PartyStatusEditorRefreshClicked), editor);
	g_signal_connect(gtk_builder_get_object(builder, "bedtime_button"), "clicked", G_CALLBACK(PartyStatusEditorBedtimeClicked), editor);
	g_signal_connect(gtk_builder_get_object(builder, "apply_button"), "clicked", G_CALLBACK(PartyStatusEditorApplyClicked), editor);
	g_signal_connect(gtk_builder_get_object(builder, "close_button"), "clicked", G_CALLBACK(PartyStatusEditorCloseClicked), editor);
	ReloadPartyStatusEditorData(editor);
	return editor;
}

void UpdatePokemonActionsFromStatus(const desmume_pokemon_http_export::Status &status) {
	SetBooleanActionState("pokemon_toggle_api", status.enabled);
	SetBooleanActionState("pokemon_toggle_battle_logging", status.battleLoggingEnabled);
	SetBooleanActionState("pokemon_toggle_health_checks", status.healthCheckEnabled);
	SetBooleanActionState("pokemon_toggle_idle_refresh", status.idleRefreshEnabled);
	SetBooleanActionState("pokemon_toggle_clipboard_auto_copy", gPokemonState.clipboardAutoCopyEnabled);
	SetBooleanActionState("pokemon_toggle_touch_tap_guard", NDS_isOneFrameTouchPressGuardEnabled());
	SetStringActionState("pokemon_set_profile", TargetForProfile(desmume_pokemon_http_export::GetSelectedProfile()));
	SetStringActionState("pokemon_set_platinum_time", TargetForPlatinumTime(desmume_pokemon_http_export::GetPlatinumTimeOverride()));
	SetStringActionState("pokemon_set_health_interval", TargetForInterval(status.healthCheckIntervalMs, GetHealthCheckIntervalOptions()));
	SetStringActionState("pokemon_set_idle_interval", TargetForInterval(status.idleRefreshIntervalMs, GetIdleRefreshIntervalOptions()));
	SetStringActionState("pokemon_set_clipboard_interval", TargetForInterval(static_cast<uint32_t>(gPokemonState.clipboardIntervalMs), GetClipboardIntervalOptions()));

	SetActionEnabled("pokemon_status", false);
	SetActionEnabled("pokemon_force_refresh", status.enabled);
	SetActionEnabled("pokemon_set_health_interval", status.healthCheckEnabled);
	SetActionEnabled("pokemon_set_idle_interval", status.idleRefreshEnabled);
	SetActionEnabled("pokemon_copy_export", status.romLoaded);
	SetActionEnabled("pokemon_toggle_clipboard_auto_copy", status.romLoaded);
	SetActionEnabled("pokemon_set_clipboard_interval", gPokemonState.clipboardAutoCopyEnabled && status.romLoaded);
	SetActionEnabled("pokemon_open_party_box_editor", status.romLoaded && status.supportedGame);
	SetActionEnabled("pokemon_open_party_status_editor", status.romLoaded && status.supportedGame);
	SetActionEnabled("pokemon_configure_battle_log_path", true);
}

std::string CurrentStatusTextOrFallback(const std::string &fallback) {
	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	if (!status.statusText.empty()) {
		return status.statusText;
	}
	return fallback;
}

}  // namespace

void PokemonGtkLoadConfig(desmume::config::Config *config) {
	gPokemonState.config = config;
	if (config == NULL) {
		return;
	}

	LoadPokemonBackendConfig(*config, gPokemonState.clipboardAutoCopyEnabled, gPokemonState.clipboardIntervalMs);
	NDS_setOneFrameTouchPressGuardEnabled(config->pokemon_touch_tap_guard_enabled.get());
}

void PokemonGtkInitialize(GtkApplication *app,
						  GtkWindow *window,
						  GtkWidget *statusBar,
						  GMenu *pokemonStatusSection,
						  desmume::config::Config *config) {
	gPokemonState.app = app;
	gPokemonState.window = window;
	gPokemonState.statusBar = statusBar;
	gPokemonState.config = config;
	if (gPokemonState.pokemonStatusSection != NULL) {
		g_object_unref(gPokemonState.pokemonStatusSection);
	}
	gPokemonState.pokemonStatusSection = (pokemonStatusSection != NULL)
		? G_MENU(g_object_ref(pokemonStatusSection))
		: NULL;
	gPokemonState.initialized = true;

	if (gPokemonState.statusTimerId == 0) {
		gPokemonState.statusTimerId = g_timeout_add_seconds(1, PokemonTimerTick, NULL);
	}

	PokemonGtkUpdateMenuState();
}

void PokemonGtkShutdown() {
	if (gPokemonState.statusTimerId != 0) {
		g_source_remove(gPokemonState.statusTimerId);
		gPokemonState.statusTimerId = 0;
	}

	if (gPokemonState.partyEditor != NULL && GTK_IS_WIDGET(gPokemonState.partyEditor->dialog)) {
		gtk_widget_destroy(gPokemonState.partyEditor->dialog);
	}
	if (gPokemonState.partyStatusEditor != NULL && GTK_IS_WIDGET(gPokemonState.partyStatusEditor->dialog)) {
		gtk_widget_destroy(gPokemonState.partyStatusEditor->dialog);
	}
	if (gPokemonState.pokemonStatusSection != NULL) {
		g_object_unref(gPokemonState.pokemonStatusSection);
		gPokemonState.pokemonStatusSection = NULL;
	}

	desmume_pokemon_http_export::Shutdown();
	gPokemonState.initialized = false;
}

void PokemonGtkUpdateMenuState() {
	if (!gPokemonState.initialized) {
		return;
	}

	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	SetPokemonStatusMenuLabel(BuildPokemonStatusLabel(status));
	UpdatePokemonActionsFromStatus(status);
}

void PokemonGtkOnRomChanged() {
	PokemonGtkUpdateMenuState();
	if (gPokemonState.partyEditor != NULL) {
		ReloadPartyEditorData(gPokemonState.partyEditor);
	}
	if (gPokemonState.partyStatusEditor != NULL) {
		ReloadPartyStatusEditorData(gPokemonState.partyStatusEditor);
	}
}

void PokemonGtkStatusAction(GSimpleAction *, GVariant *, gpointer) {
}

void PokemonGtkToggleApi(GSimpleAction *action, GVariant *, gpointer) {
	const bool enabled = !GetBooleanActionState(action);
	desmume_pokemon_http_export::SetEnabled(enabled);
	SetBooleanActionState("pokemon_toggle_api", enabled);
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage(CurrentStatusTextOrFallback(enabled ? "Pokemon HTTP API enabled" : "Pokemon HTTP API disabled"), true);
}

void PokemonGtkToggleBattleLogging(GSimpleAction *action, GVariant *, gpointer) {
	const bool enabled = !GetBooleanActionState(action);
	desmume_pokemon_http_export::SetBattleLoggingEnabled(enabled);
	SetBooleanActionState("pokemon_toggle_battle_logging", enabled);
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage(enabled ? "Battle logging enabled" : "Battle logging disabled", true);
}

void PokemonGtkForceRefresh(GSimpleAction *, GVariant *, gpointer) {
	desmume_pokemon_http_export::ForceRefresh();
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage(CurrentStatusTextOrFallback("Pokemon HTTP API forced refresh requested"), true);
}

void PokemonGtkSetProfile(GSimpleAction *, GVariant *parameter, gpointer) {
	desmume_pokemon_http_export::ExportGameProfile profile;
	if (!ProfileFromTarget(g_variant_get_string(parameter, NULL), profile)) {
		return;
	}
	desmume_pokemon_http_export::SetSelectedProfile(profile);
	SetStringActionState("pokemon_set_profile", TargetForProfile(profile));
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	std::ostringstream message;
	message << "Pokemon profile set to " << desmume_pokemon_http_export::GetProfileDisplayName(profile);
	LogPokemonMessage(message.str(), true);
}

void PokemonGtkSetPlatinumTime(GSimpleAction *, GVariant *parameter, gpointer) {
	desmume_pokemon_http_export::PlatinumTimeOverride mode;
	if (!PlatinumTimeFromTarget(g_variant_get_string(parameter, NULL), mode)) {
		return;
	}
	desmume_pokemon_http_export::SetPlatinumTimeOverride(mode);
	SetStringActionState("pokemon_set_platinum_time", TargetForPlatinumTime(mode));
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	std::ostringstream message;
	message << "Pokemon time override set to " << desmume_pokemon_http_export::GetPlatinumTimeOverrideDisplayName(mode);
	LogPokemonMessage(message.str(), true);
}

void PokemonGtkToggleHealthChecks(GSimpleAction *action, GVariant *, gpointer) {
	const bool enabled = !GetBooleanActionState(action);
	desmume_pokemon_http_export::SetHealthCheckEnabled(enabled);
	SetBooleanActionState("pokemon_toggle_health_checks", enabled);
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage(enabled ? "Pokemon health checks enabled" : "Pokemon health checks disabled", true);
}

void PokemonGtkSetHealthInterval(GSimpleAction *, GVariant *parameter, gpointer) {
	uint32_t intervalMs = 0u;
	if (!IntervalFromTarget(g_variant_get_string(parameter, NULL), GetHealthCheckIntervalOptions(), intervalMs)) {
		return;
	}
	desmume_pokemon_http_export::SetHealthCheckIntervalMs(intervalMs);
	SetStringActionState("pokemon_set_health_interval", TargetForInterval(intervalMs, GetHealthCheckIntervalOptions()));
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage("Pokemon health check interval updated", true);
}

void PokemonGtkToggleIdleRefresh(GSimpleAction *action, GVariant *, gpointer) {
	const bool enabled = !GetBooleanActionState(action);
	desmume_pokemon_http_export::SetIdleRefreshEnabled(enabled);
	SetBooleanActionState("pokemon_toggle_idle_refresh", enabled);
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage(enabled ? "Pokemon idle refresh enabled" : "Pokemon idle refresh disabled", true);
}

void PokemonGtkSetIdleInterval(GSimpleAction *, GVariant *parameter, gpointer) {
	uint32_t intervalMs = 0u;
	if (!IntervalFromTarget(g_variant_get_string(parameter, NULL), GetIdleRefreshIntervalOptions(), intervalMs)) {
		return;
	}
	desmume_pokemon_http_export::SetIdleRefreshIntervalMs(intervalMs);
	SetStringActionState("pokemon_set_idle_interval", TargetForInterval(intervalMs, GetIdleRefreshIntervalOptions()));
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage("Pokemon idle refresh interval updated", true);
}

void PokemonGtkCopyExport(GSimpleAction *, GVariant *, gpointer) {
	const desmume_pokemon_http_export::Status status = desmume_pokemon_http_export::GetStatus();
	std::string showdownText;
	std::string error;
	if (!BuildAndCopyPokemonExportText(showdownText, error)) {
		ShowPokemonErrorDialog(error);
		LogPokemonMessage("Pokemon export copy failed", true);
		return;
	}

	NotePokemonClipboardCopy(status, showdownText, CurrentUnixMs());
	LogPokemonMessage("Pokemon export copied to clipboard", true);
}

void PokemonGtkToggleClipboardAutoCopy(GSimpleAction *action, GVariant *, gpointer) {
	const bool enabled = !GetBooleanActionState(action);
	SetClipboardAutoCopyEnabledInternal(enabled);
	SetBooleanActionState("pokemon_toggle_clipboard_auto_copy", enabled);
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage(enabled ? "Clipboard auto copy enabled" : "Clipboard auto copy disabled", true);
}

void PokemonGtkSetClipboardInterval(GSimpleAction *, GVariant *parameter, gpointer) {
	uint32_t intervalMs = 0u;
	if (!IntervalFromTarget(g_variant_get_string(parameter, NULL), GetClipboardIntervalOptions(), intervalMs)) {
		return;
	}
	SetClipboardIntervalInternal(intervalMs);
	SetStringActionState("pokemon_set_clipboard_interval", TargetForInterval(intervalMs, GetClipboardIntervalOptions()));
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage("Pokemon clipboard auto copy interval updated", true);
}

void PokemonGtkToggleTouchTapGuard(GSimpleAction *action, GVariant *, gpointer) {
	const bool enabled = !GetBooleanActionState(action);
	NDS_setOneFrameTouchPressGuardEnabled(enabled);
	SetBooleanActionState("pokemon_toggle_touch_tap_guard", enabled);
	MirrorPokemonConfig();
	PokemonGtkUpdateMenuState();
	LogPokemonMessage(enabled ? "Touch tap guard enabled" : "Touch tap guard disabled", true);
}

void PokemonGtkConfigureBattleLogPath(GSimpleAction *, GVariant *, gpointer) {
	OpenBattleLogPathDialog();
}

void PokemonGtkOpenPartyBoxEditor(GSimpleAction *, GVariant *, gpointer) {
	if (gPokemonState.partyEditor == NULL) {
		gPokemonState.partyEditor = BuildPartyEditorWindow();
	}
	gtk_widget_show_all(gPokemonState.partyEditor->dialog);
	gtk_window_present(GTK_WINDOW(gPokemonState.partyEditor->dialog));
	ReloadPartyEditorData(gPokemonState.partyEditor);
}

void PokemonGtkOpenPartyStatusEditor(GSimpleAction *, GVariant *, gpointer) {
	if (gPokemonState.partyStatusEditor == NULL) {
		gPokemonState.partyStatusEditor = BuildPartyStatusEditorWindow();
	}
	gtk_widget_show_all(gPokemonState.partyStatusEditor->dialog);
	gtk_window_present(GTK_WINDOW(gPokemonState.partyStatusEditor->dialog));
	ReloadPartyStatusEditorData(gPokemonState.partyStatusEditor);
}
