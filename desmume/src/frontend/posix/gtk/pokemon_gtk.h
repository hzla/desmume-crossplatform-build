#ifndef DESMUME_GTK_POKEMON_GTK_H_
#define DESMUME_GTK_POKEMON_GTK_H_

#include <gtk/gtk.h>

#include "config.h"

void PokemonGtkLoadConfig(desmume::config::Config *config);
void PokemonGtkInitialize(GtkApplication *app,
						  GtkWindow *window,
						  GtkWidget *statusBar,
						  GMenu *pokemonStatusSection,
						  desmume::config::Config *config);
void PokemonGtkShutdown();
void PokemonGtkUpdateMenuState();
void PokemonGtkOnRomChanged();

void PokemonGtkStatusAction(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkToggleApi(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkToggleBattleLogging(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkForceRefresh(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkSetProfile(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkSetPlatinumTime(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkToggleHealthChecks(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkSetHealthInterval(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkToggleIdleRefresh(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkSetIdleInterval(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkCopyExport(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkToggleClipboardAutoCopy(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkSetClipboardInterval(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkToggleTouchTapGuard(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkConfigureBattleLogPath(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkOpenPartyBoxEditor(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void PokemonGtkOpenPartyStatusEditor(GSimpleAction *action, GVariant *parameter, gpointer user_data);

#endif  // DESMUME_GTK_POKEMON_GTK_H_
