/**
 * @file gtkprefs.c GTK+ Preferences
 * @ingroup gtkui
 *
 * gaim
 *
 * Copyright (C) 1998-2002, Mark Spencer <markster@marko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <string.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include "gtkimhtml.h"
#include "gaim.h"
#include "gtkblist.h"
#include "gtkdebug.h"
#include "gtkplugin.h"
#include "gtkprefs.h"
#include "prpl.h"
#include "prefs.h"
#include "proxy.h"
#include "sound.h"
#include "notify.h"

#ifdef _WIN32
#include "win32dep.h"
#endif

GtkWidget *tree_v = NULL;
GtkWidget *prefs_away_menu = NULL;
GtkWidget *fontseld = NULL;

GtkListStore *prefs_away_store = NULL;

static int sound_row_sel = 0;
static char *last_sound_dir = NULL;

static GtkWidget *sounddialog = NULL;
static GtkWidget *browser_entry = NULL;
static GtkWidget *sound_entry = NULL;
static GtkWidget *away_text = NULL;
static GtkListStore *smiley_theme_store = NULL;
GtkWidget *prefs_proxy_frame = NULL;
GtkWidget *gaim_button(const char *, guint *, int, GtkWidget *);
GtkWidget *gaim_labeled_spin_button(GtkWidget *, const gchar *, int*, int, int, GtkSizeGroup *);
static GtkWidget *gaim_dropdown(GtkWidget *, const gchar *, int *, int, ...);
static GtkWidget *gaim_dropdown_from_list(GtkWidget *, const gchar *, int *, int, GList *); 
static GtkWidget *show_color_pref(GtkWidget *, gboolean);
static void delete_prefs(GtkWidget *, void *);
void set_default_away(GtkWidget *, gpointer);
#ifndef _WIN32
static gboolean program_is_valid(const char *);
#endif

static GtkWidget *prefs = NULL;
GtkWidget *debugbutton = NULL;
static int notebook_page = 0;
static GtkTreeIter plugin_iter;

/*
 * PROTOTYPES
 */
GtkTreeIter *prefs_notebook_add_page(const char*, GdkPixbuf*, GtkWidget*, GtkTreeIter*, GtkTreeIter*, int);

static void update_plugin_list(void *data);

void delete_prefs(GtkWidget *asdf, void *gdsa) {
	GList *l;
	GaimPlugin *plug;

	gaim_plugins_unregister_probe_notify_cb(update_plugin_list);

	save_prefs();
	prefs = NULL;
	tree_v = NULL;
	sound_entry = NULL;
	browser_entry = NULL;
	debugbutton = NULL;
	prefs_away_menu = NULL;
	notebook_page = 0;
	smiley_theme_store = NULL;
	if(sounddialog)
		gtk_widget_destroy(sounddialog);
	g_object_unref(G_OBJECT(prefs_away_store));
	prefs_away_store = NULL;

	for (l = gaim_plugins_get_loaded(); l != NULL; l = l->next) {
		plug = l->data;

		if (GAIM_IS_GTK_PLUGIN(plug)) {
			GaimGtkPluginUiInfo *ui_info;

			ui_info = GAIM_GTK_PLUGIN_UI_INFO(plug);

			if (ui_info->iter != NULL) {
				g_free(ui_info->iter);
				ui_info->iter = NULL;
			}
		}
	}
}

GtkWidget *preflabel;
GtkWidget *prefsnotebook;
GtkTreeStore *prefstree;

static void set_misc_option();
static void set_logging_option();
static void set_blist_option();
static void set_convo_option();
static void set_im_option();
static void set_chat_option();
static void set_font_option();
static void set_sound_option();
static void set_away_option();

#define PROXYHOST 0
#define PROXYPORT 1
#define PROXYTYPE 2
#define PROXYUSER 3
#define PROXYPASS 4
static void proxy_print_option(GtkEntry *entry, int entrynum)
{
	if (entrynum == PROXYHOST)
		g_snprintf(global_proxy_info.proxyhost, sizeof(global_proxy_info.proxyhost), "%s", gtk_entry_get_text(entry));
	else if (entrynum == PROXYPORT)
		global_proxy_info.proxyport = atoi(gtk_entry_get_text(entry));
	else if (entrynum == PROXYUSER)
		g_snprintf(global_proxy_info.proxyuser, sizeof(global_proxy_info.proxyuser), "%s", gtk_entry_get_text(entry));
	else if (entrynum == PROXYPASS)
		g_snprintf(global_proxy_info.proxypass, sizeof(global_proxy_info.proxypass), "%s", gtk_entry_get_text(entry));
	proxy_info_is_from_gaimrc = 1; /* If the user specifies it, we want
					  to save it */
}

/* OK, Apply and Cancel */

static void pref_nb_select(GtkTreeSelection *sel, GtkNotebook *nb) {
	GtkTreeIter   iter;
	char text[128];
	GValue val = { 0, };
	GtkTreeModel *model = GTK_TREE_MODEL(prefstree);

	if (! gtk_tree_selection_get_selected (sel, &model, &iter))
		return;
	gtk_tree_model_get_value (model, &iter, 1, &val);
	g_snprintf(text, sizeof(text), "<span weight=\"bold\" size=\"larger\">%s</span>",
		   g_value_get_string(&val));
	gtk_label_set_markup (GTK_LABEL(preflabel), text);
	g_value_unset (&val);
	gtk_tree_model_get_value (model, &iter, 2, &val);
	gtk_notebook_set_current_page (GTK_NOTEBOOK (prefsnotebook), g_value_get_int (&val));

}

/* These are the pages in the preferences notebook */
GtkWidget *interface_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	vbox = gaim_gtk_make_frame(ret, _("Interface Options"));

	gaim_button(_("D_isplay remote nicknames if no alias is set"), &misc_options, OPT_MISC_USE_SERVER_ALIAS, vbox);


	gtk_widget_show_all(ret);
	return ret;
}

static void smiley_sel (GtkTreeSelection *sel, GtkTreeModel *model) {
	GtkTreeIter  iter;
	const char *filename;
	GValue val = { 0, };
	
	if (! gtk_tree_selection_get_selected (sel, &model, &iter))
		return;
	gtk_tree_model_get_value (model, &iter, 2, &val);
	filename = g_value_get_string(&val);
	load_smiley_theme(filename, TRUE);
	g_value_unset (&val);
	save_prefs();
}

GtkTreePath *theme_refresh_theme_list()
{
	GdkPixbuf *pixbuf;
	GSList *themes;
	GtkTreeIter iter;
	GtkTreePath *path = NULL;
	int ind = 0;


	smiley_theme_probe();

	if (!smiley_themes)
		return NULL;

	themes = smiley_themes;

	gtk_list_store_clear(smiley_theme_store);

	while (themes) {
		struct smiley_theme *theme = themes->data;
		char *description = g_strdup_printf("<span size='larger' weight='bold'>%s</span> - %s\n"
						    "<span size='smaller' foreground='dim grey'>%s</span>",
						    theme->name, theme->author, theme->desc);
		gtk_list_store_append (smiley_theme_store, &iter);
		pixbuf = gdk_pixbuf_new_from_file(theme->icon, NULL);

		gtk_list_store_set(smiley_theme_store, &iter,
				   0, pixbuf,
				   1, description,
				   2, theme->path,
				   -1);
		g_object_unref(G_OBJECT(pixbuf));
		g_free(description);
		themes = themes->next;
		if (current_smiley_theme && !strcmp(theme->path, current_smiley_theme->path)) {
			/* path = gtk_tree_path_new_from_indices(ind); */
			char *iwishihadgtk2_2 = g_strdup_printf("%d", ind);
			path = gtk_tree_path_new_from_string(iwishihadgtk2_2);
			g_free(iwishihadgtk2_2);
		}
		ind++;
	}

	return path;
}

void theme_install_theme(char *path, char *extn) {
#ifndef _WIN32
	gchar *command;
#endif
	gchar *destdir;
	gchar *tail;

	/* Just to be safe */
	g_strchomp(path);

	/* I dont know what you are, get out of here */
	if (extn != NULL)
		tail = extn;
	else if ((tail = strrchr(path, '.')) == NULL)
		return;

	destdir = g_strconcat(gaim_user_dir(), G_DIR_SEPARATOR_S "smileys", NULL);

	/* We'll check this just to make sure. This also lets us do something different on
	 * other platforms, if need be */
	if (!g_ascii_strcasecmp(tail, ".gz") || !g_ascii_strcasecmp(tail, ".tgz")) {
#ifndef _WIN32
		command = g_strdup_printf("tar > /dev/null xzf \"%s\" -C %s", path, destdir);
#else
		if(!wgaim_gz_untar(path, destdir)) {
			g_free(destdir);
			return;
		}
#endif
	}
	else {
		g_free(destdir);
		return;
	}

#ifndef _WIN32
	/* Fire! */
	system(command);

	g_free(command);
#endif
	g_free(destdir);

	theme_refresh_theme_list();
}

static void theme_got_url(gpointer data, char *themedata, unsigned long len) {
	FILE *f;
	gchar *path;

	f = gaim_mkstemp(&path);
	fwrite(themedata, len, 1, f);
	fclose(f);

	theme_install_theme(path, data);

	unlink(path);
	g_free(path);
}

void theme_dnd_recv(GtkWidget *widget, GdkDragContext *dc, guint x, guint y, GtkSelectionData *sd, 
				guint info, guint t, gpointer data) {
	gchar *name = sd->data;

	if ((sd->length >= 0) && (sd->format == 8)) {
		/* Well, it looks like the drag event was cool. 
		 * Let's do something with it */

		if (!g_ascii_strncasecmp(name, "file://", 7)) {
			GError *converr = NULL;
			gchar *tmp;
			/* It looks like we're dealing with a local file. Let's 
			 * just untar it in the right place */
			if(!(tmp = g_filename_from_uri(name, NULL, &converr))) {
				gaim_debug(GAIM_DEBUG_ERROR, "theme dnd", "%s\n",
						   (converr ? converr->message :
							"g_filename_from_uri error"));
				return;
			}
			theme_install_theme(tmp, NULL);
			g_free(tmp);
		} else if (!g_ascii_strncasecmp(name, "http://", 7)) {
			/* Oo, a web drag and drop. This is where things
			 * will start to get interesting */
			gchar *tail;

			if ((tail = strrchr(name, '.')) == NULL)
				return;

			/* We'll check this just to make sure. This also lets us do something different on
			 * other platforms, if need be */
			grab_url(name, TRUE, theme_got_url, ".tgz");
		}

		gtk_drag_finish(dc, TRUE, FALSE, t);
	}

	gtk_drag_finish(dc, FALSE, FALSE, t);
}

GtkWidget *theme_page() {
	GtkWidget *ret;
	GtkWidget *sw;
	GtkWidget *view;
	GtkCellRenderer *rend;
	GtkTreeViewColumn *col;
	GtkTreeSelection *sel;
	GtkTreePath *path = NULL;
	GtkWidget *label;
	GtkTargetEntry te[3] = {{"text/plain", 0, 0},{"text/uri-list", 0, 1},{"STRING", 0, 2}};

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	label = gtk_label_new(_("Select a smiley theme that you would like to use from the list below. New themes can be installed by dragging and dropping them onto the theme list."));

	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);

	gtk_box_pack_start(GTK_BOX(ret), label, FALSE, TRUE, 0);
	gtk_widget_show(label);

	sw = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_IN);

	gtk_box_pack_start(GTK_BOX(ret), sw, TRUE, TRUE, 0);
	smiley_theme_store = gtk_list_store_new (3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);

	path = theme_refresh_theme_list();
	
	view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(smiley_theme_store));

	gtk_drag_dest_set(view, GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP, te, 
					sizeof(te) / sizeof(GtkTargetEntry) , GDK_ACTION_COPY | GDK_ACTION_MOVE);

	g_signal_connect(G_OBJECT(view), "drag_data_received", G_CALLBACK(theme_dnd_recv), smiley_theme_store);

	rend = gtk_cell_renderer_pixbuf_new();
	sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));

	if(path) {
		gtk_tree_selection_select_path(sel, path);
		gtk_tree_path_free(path);
	}

	col = gtk_tree_view_column_new_with_attributes (_("Icon"),
							rend,
							"pixbuf", 0,
							NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);

	rend = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes (_("Description"),
							rend,
							"markup", 1,
							NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);
	g_object_unref(G_OBJECT(smiley_theme_store));
	gtk_container_add(GTK_CONTAINER(sw), view);

	g_signal_connect (G_OBJECT (sel), "changed",
			  G_CALLBACK (smiley_sel),
			  NULL);


	gtk_widget_show_all(ret);
	return ret;
}

static void update_color(GtkWidget *w, GtkWidget *pic)
{
	GdkColor c;
	GtkStyle *style;
	c.pixel = 0;

	if (pic == pref_fg_picture) {
		if (font_options & OPT_FONT_FGCOL) {
			c.red = fgcolor.red;
			c.blue = fgcolor.blue;
			c.green = fgcolor.green;
		} else {
			c.red = 0;
			c.blue = 0;
			c.green = 0;
		}
	} else {
		if (font_options & OPT_FONT_BGCOL) {
			c.red = bgcolor.red;
			c.blue = bgcolor.blue;
			c.green = bgcolor.green;
		} else {
			c.red = 0xffff;
			c.blue = 0xffff;
			c.green = 0xffff;
		}
	}

	style = gtk_style_new();
	style->bg[0] = c;
	gtk_widget_set_style(pic, style);
	g_object_unref(style);
}

GtkWidget *font_page() {
	GtkWidget *ret;
	GtkWidget *button;
	GtkWidget *vbox, *hbox;
	GtkWidget *select = NULL;
	GtkSizeGroup *sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	vbox = gaim_gtk_make_frame(ret, _("Style"));
	gaim_button(_("_Bold"), &font_options, OPT_FONT_BOLD, vbox);
	gaim_button(_("_Italics"), &font_options, OPT_FONT_ITALIC, vbox);
	gaim_button(_("_Underline"), &font_options, OPT_FONT_UNDERLINE, vbox);
	gaim_button(_("_Strikethrough"), &font_options, OPT_FONT_STRIKE, vbox);

	vbox = gaim_gtk_make_frame(ret, _("Face"));
	hbox = gtk_hbox_new(FALSE, 6);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	button = gaim_button(_("Use custo_m face"), &font_options, OPT_FONT_FACE, hbox);
	gtk_size_group_add_widget(sg, button);
	select = gtk_button_new_from_stock(GTK_STOCK_SELECT_FONT);

	if (!(font_options & OPT_FONT_FACE))
		gtk_widget_set_sensitive(GTK_WIDGET(select), FALSE);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(gaim_gtk_toggle_sensitive), select);
	g_signal_connect(G_OBJECT(select), "clicked",
					 G_CALLBACK(show_font_dialog), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), select, FALSE, FALSE, 0);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	button = gaim_button(_("Use custom si_ze"), &font_options, OPT_FONT_SIZE, hbox);
	gtk_size_group_add_widget(sg, button);
	select = gaim_labeled_spin_button(hbox, NULL, &fontsize, 1, 7, NULL);
	if (!(font_options & OPT_FONT_SIZE))
		gtk_widget_set_sensitive(GTK_WIDGET(select), FALSE);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(gaim_gtk_toggle_sensitive), select);

	vbox = gaim_gtk_make_frame(ret, _("Color"));
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);


	button = gaim_button(_("_Text color"), &font_options, OPT_FONT_FGCOL, hbox);
	gtk_size_group_add_widget(sg, button);

	select = gtk_button_new_from_stock(GTK_STOCK_SELECT_COLOR);
	gtk_box_pack_start(GTK_BOX(hbox), select, FALSE, FALSE, 0);
	pref_fg_picture = show_color_pref(hbox, TRUE);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(update_color),
			   pref_fg_picture);

	if (!(font_options & OPT_FONT_FGCOL))
		gtk_widget_set_sensitive(GTK_WIDGET(select), FALSE);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(gaim_gtk_toggle_sensitive), select);
	g_signal_connect(G_OBJECT(select), "clicked", G_CALLBACK(show_fgcolor_dialog), NULL);
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);

	button = gaim_button(_("Bac_kground color"), &font_options, OPT_FONT_BGCOL, hbox);
	gtk_size_group_add_widget(sg, button);
	select = gtk_button_new_from_stock(GTK_STOCK_SELECT_COLOR);
	gtk_box_pack_start(GTK_BOX(hbox), select, FALSE, FALSE, 0);
	pref_bg_picture = show_color_pref(hbox, FALSE);
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(update_color), pref_bg_picture);

	if (!(font_options & OPT_FONT_BGCOL))
		gtk_widget_set_sensitive(GTK_WIDGET(select), FALSE);
	g_signal_connect(G_OBJECT(select), "clicked",
					 G_CALLBACK(show_bgcolor_dialog), NULL);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(gaim_gtk_toggle_sensitive), select);

	gtk_widget_show_all(ret);
	return ret;
}


GtkWidget *messages_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	vbox = gaim_gtk_make_frame (ret, _("Display"));
	gaim_button(_("Show graphical _smileys"), &convo_options, OPT_CONVO_SHOW_SMILEY, vbox);
	gaim_button(_("Show _timestamp on messages"), &convo_options, OPT_CONVO_SHOW_TIME, vbox);
	gaim_button(_("Show _URLs as links"), &convo_options, OPT_CONVO_SEND_LINKS, vbox);
#ifdef USE_GTKSPELL
	gaim_button(_("_Highlight misspelled words"), &convo_options, OPT_CONVO_CHECK_SPELLING, vbox);
#endif
	vbox = gaim_gtk_make_frame (ret, _("Ignore"));
	gaim_button(_("Ignore c_olors"), &convo_options, OPT_CONVO_IGNORE_COLOUR, vbox);
	gaim_button(_("Ignore font _faces"), &convo_options, OPT_CONVO_IGNORE_FONTS, vbox);
	gaim_button(_("Ignore font si_zes"), &convo_options, OPT_CONVO_IGNORE_SIZES, vbox);

	gtk_widget_show_all(ret);
	return ret;
}

GtkWidget *hotkeys_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	vbox = gaim_gtk_make_frame(ret, _("Send Message"));
	gaim_button(_("_Enter sends message"), &convo_options, OPT_CONVO_ENTER_SENDS, vbox);
	gaim_button(_("C_ontrol-Enter sends message"), &convo_options, OPT_CONVO_CTL_ENTER, vbox);

	vbox = gaim_gtk_make_frame (ret, _("Window Closing"));
	gaim_button(_("E_scape closes window"), &convo_options, OPT_CONVO_ESC_CAN_CLOSE, vbox);
	gaim_button(_("Control-_W closes window"), &convo_options, OPT_CONVO_CTL_W_CLOSES, vbox);

	vbox = gaim_gtk_make_frame(ret, _("Insertions"));
	gaim_button(_("Control-{B/I/U/S} inserts _HTML tags"), &convo_options, OPT_CONVO_CTL_CHARS, vbox);
	gaim_button(_("Control-(number) inserts _smileys"), &convo_options, OPT_CONVO_CTL_SMILEYS, vbox);

	gtk_widget_show_all(ret);
	return ret;
}

GtkWidget *list_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkWidget *button, *b2;
	int r = 0;
	gboolean fnd = FALSE;
	GList *l= NULL;
	GSList *sl = gaim_gtk_blist_sort_methods;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);
	
	
	vbox = gaim_gtk_make_frame (ret, _("Buddy List Sorting"));
	while (sl) {
		l = g_list_append(l, ((struct gaim_gtk_blist_sort_method*)sl->data)->name);
		l = g_list_append(l, ((struct gaim_gtk_blist_sort_method*)sl->data)->name);
		if (!fnd && !gaim_utf8_strcasecmp(((struct gaim_gtk_blist_sort_method*)sl->data)->name, sort_method))
			fnd = TRUE;
			sl = sl->next;
		if (!fnd) r++;
	}
	gaim_dropdown_from_list(vbox, _("Sorting:"),
				(int*)&sort_method, r, l);
	
	g_list_free(l);

	vbox = gaim_gtk_make_frame (ret, _("Buddy List Toolbar"));
	gaim_dropdown(vbox, _("Show _buttons as:"), &blist_options, OPT_BLIST_SHOW_BUTTON_XPM | OPT_BLIST_NO_BUTTON_TEXT,
		      _("Pictures"), OPT_BLIST_SHOW_BUTTON_XPM | OPT_BLIST_NO_BUTTON_TEXT, 
		      _("Text"), 0,
		      _("Pictures and text"), OPT_BLIST_SHOW_BUTTON_XPM,
		      _("None"), OPT_BLIST_NO_BUTTON_TEXT, NULL);

	vbox = gaim_gtk_make_frame (ret, _("Buddy List Window"));
	gaim_button(_("_Raise window on events"), &blist_options, OPT_BLIST_POPUP, vbox);

	vbox = gaim_gtk_make_frame (ret, _("Group Display"));
	/* gaim_button(_("Hide _groups with no online buddies"), &blist_options, OPT_BLIST_NO_MT_GRP, vbox); */
	gaim_button(_("Show _numbers in groups"), &blist_options, OPT_BLIST_SHOW_GRPNUM, vbox);

	vbox = gaim_gtk_make_frame (ret, _("Buddy Display"));
	button = gaim_button(_("Show buddy _icons"), &blist_options, OPT_BLIST_SHOW_ICONS, vbox);
	b2 = gaim_button(_("Show _warning levels"), &blist_options, OPT_BLIST_SHOW_WARN, vbox);
	if (blist_options & OPT_BLIST_SHOW_ICONS)
		gtk_widget_set_sensitive(GTK_WIDGET(b2), FALSE);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(gaim_gtk_toggle_sensitive), b2);
	b2 = gaim_button(_("Show idle _times"), &blist_options, OPT_BLIST_SHOW_IDLETIME, vbox);
	if (blist_options & OPT_BLIST_SHOW_ICONS)
		gtk_widget_set_sensitive(GTK_WIDGET(b2), FALSE);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(gaim_gtk_toggle_sensitive), b2);
	gaim_button(_("Dim i_dle buddies"), &blist_options, OPT_BLIST_GREY_IDLERS, vbox);

	gtk_widget_show_all(ret);
	return ret;
}

GtkWidget *conv_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkWidget *label;
	GtkSizeGroup *sg;
	GList *names = NULL;
	int i;

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width(GTK_CONTAINER(ret), 12);

	sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	vbox = gaim_gtk_make_frame(ret, _("Conversations"));

	/* Build a list of names. */
	for (i = 0; i < gaim_conv_placement_get_fnc_count(); i++) {
		names = g_list_append(names, (char *)gaim_conv_placement_get_name(i));
		names = g_list_append(names, GINT_TO_POINTER(i));
	}

	label = gaim_dropdown_from_list(vbox, _("_Placement:"),
									&conv_placement_option, -1, names);

	g_list_free(names);

	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_size_group_add_widget(sg, label);

	gaim_button(_("Show IMs and chats in _same tabbed window."),
				&convo_options, OPT_CONVO_COMBINE, vbox);

	gtk_widget_show_all(ret);

	return ret;
}

GtkWidget *im_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkWidget *typingbutton, *widge;
	GtkSizeGroup *sg;

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

	vbox = gaim_gtk_make_frame (ret, _("Window"));
	widge = gaim_dropdown(vbox, _("Show _buttons as:"), &im_options, OPT_IM_BUTTON_TEXT | OPT_IM_BUTTON_XPM,
		      _("Pictures"), OPT_IM_BUTTON_XPM,
		      _("Text"), OPT_IM_BUTTON_TEXT,
		      _("Pictures and text"), OPT_IM_BUTTON_XPM | OPT_IM_BUTTON_TEXT, NULL);
	gtk_size_group_add_widget(sg, widge);
	gtk_misc_set_alignment(GTK_MISC(widge), 0, 0);
	gaim_labeled_spin_button(vbox, _("New window _width:"), &conv_size.width, 25, 9999, sg);
	gaim_labeled_spin_button(vbox, _("New window _height:"), &conv_size.height, 25, 9999, sg);
	gaim_labeled_spin_button(vbox, _("_Entry field height:"), &conv_size.entry_height, 25, 9999, sg);
	gaim_button(_("_Raise windows on events"), &im_options, OPT_IM_POPUP, vbox);
	gaim_button(_("Hide window on _send"), &im_options, OPT_IM_POPDOWN, vbox);
	gtk_widget_show (vbox);

	vbox = gaim_gtk_make_frame (ret, _("Buddy Icons"));
	gaim_button(_("Hide buddy _icons"), &im_options, OPT_IM_HIDE_ICONS, vbox);
	gaim_button(_("Disable buddy icon a_nimation"), &im_options, OPT_IM_NO_ANIMATION, vbox);

	vbox = gaim_gtk_make_frame (ret, _("Display"));
	gaim_button(_("Show _logins in window"), &im_options, OPT_IM_LOGON, vbox);
	gaim_button(_("Show a_liases in tabs/titles"), &im_options, OPT_IM_ALIAS_TAB, vbox);

	vbox = gaim_gtk_make_frame (ret, _("Typing Notification"));
	typingbutton = gaim_button(_("Notify buddies that you are _typing to them"), &misc_options,
				   OPT_MISC_STEALTH_TYPING, vbox);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(typingbutton), !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(typingbutton)));
	misc_options ^= OPT_MISC_STEALTH_TYPING;

	gtk_widget_show_all(ret);
	return ret;
}

GtkWidget *chat_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkWidget *dd;
	GtkSizeGroup *sg;

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	sg = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

	vbox = gaim_gtk_make_frame (ret, _("Window"));
	dd = gaim_dropdown(vbox, _("Show _buttons as:"), &chat_options, OPT_CHAT_BUTTON_TEXT | OPT_CHAT_BUTTON_XPM,
			   _("Pictures"), OPT_CHAT_BUTTON_XPM,
			   _("Text"), OPT_CHAT_BUTTON_TEXT,
			   _("Pictures and text"), OPT_CHAT_BUTTON_XPM | OPT_CHAT_BUTTON_TEXT, NULL);
	gtk_size_group_add_widget(sg, dd);
	gtk_misc_set_alignment(GTK_MISC(dd), 0, 0);
	gaim_labeled_spin_button(vbox, _("New window _width:"), &buddy_chat_size.width, 25, 9999, sg);
	gaim_labeled_spin_button(vbox, _("New window _height:"), &buddy_chat_size.height, 25, 9999, sg);
	gaim_labeled_spin_button(vbox, _("_Entry field height:"), &buddy_chat_size.entry_height, 25, 9999, sg);
	gaim_button(_("_Raise windows on events"), &chat_options, OPT_CHAT_POPUP, vbox);

	vbox = gaim_gtk_make_frame (ret, _("Tab Completion"));
	gaim_button(_("_Tab-complete nicks"), &chat_options, OPT_CHAT_TAB_COMPLETE, vbox);
	gaim_button(_("_Old-style tab completion"), &chat_options, OPT_CHAT_OLD_STYLE_TAB, vbox);

	vbox = gaim_gtk_make_frame (ret, _("Display"));
	gaim_button(_("_Show people joining/leaving in window"), &chat_options, OPT_CHAT_LOGON, vbox);
	gaim_button(_("Co_lorize screennames"), &chat_options, OPT_CHAT_COLORIZE, vbox);

	gtk_widget_show_all(ret);
	return ret;
}

GtkWidget *tab_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkWidget *dd;
	GtkWidget *button;
	GtkSizeGroup *sg;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

	vbox = gaim_gtk_make_frame (ret, _("IM Tabs"));
	dd = gaim_dropdown(vbox, _("Tab _placement:"), &im_options, OPT_IM_SIDE_TAB | OPT_IM_BR_TAB,
		      _("Top"), 0,
		      _("Bottom"), OPT_IM_BR_TAB,
		      _("Left"), OPT_IM_SIDE_TAB,
		      _("Right"), OPT_IM_BR_TAB | OPT_IM_SIDE_TAB, NULL);
	gtk_size_group_add_widget(sg, dd);
	gaim_button(_("Show all _instant messages in one tabbed\nwindow"), &im_options, OPT_IM_ONE_WINDOW, vbox);


	vbox = gaim_gtk_make_frame (ret, _("Chat Tabs"));
	dd = gaim_dropdown(vbox, _("Tab _placement:"), &chat_options, OPT_CHAT_SIDE_TAB | OPT_CHAT_BR_TAB,
			   _("Top"), 0,
			   _("Bottom"), OPT_CHAT_BR_TAB,
			   _("Left"), OPT_CHAT_SIDE_TAB,
			   _("Right"), OPT_CHAT_SIDE_TAB | OPT_CHAT_BR_TAB, NULL);
	gtk_size_group_add_widget(sg, dd);
	gaim_button(_("Show all c_hats in one tabbed window"), &chat_options, OPT_CHAT_ONE_WINDOW,
		    vbox);

	vbox = gaim_gtk_make_frame (ret, _("Tab Options"));
	button = gaim_button(_("Show _close button on tabs."), &convo_options, OPT_CONVO_NO_X_ON_TAB, vbox);
	convo_options ^= OPT_CONVO_NO_X_ON_TAB;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(button), !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)));


	gtk_widget_show_all(ret);
	return ret;
}

GtkWidget *proxy_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *hbox;
	GtkWidget *table;

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	vbox = gaim_gtk_make_frame (ret, _("Proxy Type"));
	gaim_dropdown(vbox, _("Proxy _type:"), (int*)&global_proxy_info.proxytype, -1,
		      _("No proxy"), PROXY_NONE,
		      "SOCKS 4", PROXY_SOCKS4,
		      "SOCKS 5", PROXY_SOCKS5,
		      "HTTP", PROXY_HTTP, NULL);

	vbox = gaim_gtk_make_frame(ret, _("Proxy Server"));
	prefs_proxy_frame = vbox;

	if (global_proxy_info.proxytype == PROXY_NONE)
		gtk_widget_set_sensitive(GTK_WIDGET(vbox), FALSE);

	table = gtk_table_new(2, 4, FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(table), 5);
	gtk_table_set_col_spacings(GTK_TABLE(table), 5);
	gtk_table_set_row_spacings(GTK_TABLE(table), 10);
	gtk_container_add(GTK_CONTAINER(vbox), table);


	label = gtk_label_new_with_mnemonic(_("_Host"));
	gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0);

	entry = gtk_entry_new();
	gtk_label_set_mnemonic_widget(GTK_LABEL(label), entry);
	gtk_table_attach(GTK_TABLE(table), entry, 1, 2, 0, 1, GTK_FILL, 0, 0, 0);
	g_signal_connect(G_OBJECT(entry), "changed",
					 G_CALLBACK(proxy_print_option), (void *)PROXYHOST);
	gtk_entry_set_text(GTK_ENTRY(entry), global_proxy_info.proxyhost);

	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	label = gtk_label_new_with_mnemonic(_("Port"));
	gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0);

	entry = gtk_entry_new();
	gtk_label_set_mnemonic_widget(GTK_LABEL(label), entry);
	gtk_table_attach(GTK_TABLE(table), entry, 1, 2, 1, 2, GTK_FILL, 0, 0, 0);
	g_signal_connect(G_OBJECT(entry), "changed",
					 G_CALLBACK(proxy_print_option), (void *)PROXYPORT);

	if (global_proxy_info.proxyport) {
		char buf[128];
		g_snprintf(buf, sizeof(buf), "%d", global_proxy_info.proxyport);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
	}

	label = gtk_label_new_with_mnemonic(_("_User"));
	gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, 0, 0, 0);

	entry = gtk_entry_new();
	gtk_label_set_mnemonic_widget(GTK_LABEL(label), entry);
	gtk_table_attach(GTK_TABLE(table), entry, 1, 2, 2, 3, GTK_FILL, 0, 0, 0);
	g_signal_connect(G_OBJECT(entry), "changed",
					 G_CALLBACK(proxy_print_option), (void *)PROXYUSER);
	gtk_entry_set_text(GTK_ENTRY(entry), global_proxy_info.proxyuser);

	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	label = gtk_label_new_with_mnemonic(_("Pa_ssword"));
	gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, 0, 0, 0);

	entry = gtk_entry_new();
	gtk_label_set_mnemonic_widget(GTK_LABEL(label), entry);
	gtk_table_attach(GTK_TABLE(table), entry, 1, 2, 3, 4, GTK_FILL , 0, 0, 0);
	gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
	g_signal_connect(G_OBJECT(entry), "changed",
					 G_CALLBACK(proxy_print_option), (void *)PROXYPASS);
	gtk_entry_set_text(GTK_ENTRY(entry), global_proxy_info.proxypass);

	gtk_widget_show_all(ret);
	return ret;
}

#ifndef _WIN32
static gboolean manual_browser_set(GtkWidget *entry, GdkEventFocus *event, gpointer data) {
	const char *program = gtk_entry_get_text(GTK_ENTRY(entry));

	if (!program_is_valid(program)) {
		char *error = g_strdup_printf(_("The entered manual browser "
						"'%s' is not valid. Hyperlinks will "
						"not work."), program); 
		gaim_notify_warning(NULL, NULL, error, NULL);
	}

	g_strlcpy(web_command, program, sizeof(web_command));

	/* carry on normally */
	return FALSE;
}

static GList *get_available_browsers() 
{
	struct browser {
		char *name;
		char *command;
		int id; 
	};

	static struct browser possible_browsers[] = {
		{N_("Konqueror"), "kfmclient", BROWSER_KONQ},
		{N_("Opera"), "opera", BROWSER_OPERA}, 
		{N_("Galeon"), "galeon", BROWSER_GALEON},
		{N_("Netscape"), "netscape", BROWSER_NETSCAPE},
		{N_("Mozilla"), "mozilla", BROWSER_MOZILLA},
	};
	static const int num_possible_browsers = 5;

	GList *browsers = NULL;
	int i = 0;

	browsers = g_list_prepend(browsers, GINT_TO_POINTER(BROWSER_MANUAL));
	browsers = g_list_prepend(browsers, _("Manual"));
	for (i = 0; i < num_possible_browsers; i++) {
		if (program_is_valid(possible_browsers[i].command)) {
			browsers = g_list_prepend(browsers, 
						  GINT_TO_POINTER(possible_browsers[i].id));
			browsers = g_list_prepend(browsers, possible_browsers[i].name);
		}
	}

	return browsers;
}

GtkWidget *browser_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkSizeGroup *sg;
	GList *browsers = NULL;

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	vbox = gaim_gtk_make_frame (ret, _("Browser Selection"));

	browsers = get_available_browsers();
	if (browsers != NULL) {
		label = gaim_dropdown_from_list(vbox,_("_Browser"), &web_browser, -1, 
						browsers);
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
		gtk_size_group_add_widget(sg, label);
	}

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new_with_mnemonic(_("_Manual: "));
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_size_group_add_widget(sg, label);

	browser_entry = gtk_entry_new();
	gtk_label_set_mnemonic_widget(GTK_LABEL(label), browser_entry);
	if (web_browser != BROWSER_MANUAL)
		gtk_widget_set_sensitive(hbox, FALSE);
	gtk_box_pack_start (GTK_BOX (hbox), browser_entry, FALSE, FALSE, 0);

	gtk_entry_set_text(GTK_ENTRY(browser_entry), web_command);
	g_signal_connect(G_OBJECT(browser_entry), "focus-out-event", G_CALLBACK(manual_browser_set), NULL);

	if (browsers != NULL) {
		vbox = gaim_gtk_make_frame (ret, _("Browser Options"));
		label = gaim_button(_("Open new _window by default"), &misc_options, OPT_MISC_BROWSER_POPUP, vbox);
	}

	gtk_widget_show_all(ret);
	return ret;
}
#endif /*_WIN32*/

GtkWidget *logging_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	vbox = gaim_gtk_make_frame (ret, _("Message Logs"));
	gaim_button(_("_Log all instant messages"), &logging_options, OPT_LOG_CONVOS, vbox);
	gaim_button(_("Log all c_hats"), &logging_options, OPT_LOG_CHATS, vbox);
	gaim_button(_("Strip _HTML from logs"), &logging_options, OPT_LOG_STRIP_HTML, vbox);

	vbox = gaim_gtk_make_frame (ret, _("System Logs"));
	gaim_button(_("Log when buddies _sign on/sign off"), &logging_options, OPT_LOG_BUDDY_SIGNON,
		    vbox);
	gaim_button(_("Log when buddies become _idle/un-idle"), &logging_options, OPT_LOG_BUDDY_IDLE,
		    vbox);
	gaim_button(_("Log when buddies go away/come _back"), &logging_options, OPT_LOG_BUDDY_AWAY, vbox);
	gaim_button(_("Log your _own signons/idleness/awayness"), &logging_options, OPT_LOG_MY_SIGNON,
		    vbox);
	gaim_button(_("I_ndividual log file for each buddy's signons"), &logging_options,
		    OPT_LOG_INDIVIDUAL, vbox);

	gtk_widget_show_all(ret);
	return ret;
}

static GtkWidget *sndcmd = NULL;

#ifndef _WIN32
static gint sound_cmd_yeah(GtkEntry *entry, gpointer d)
{
	gaim_sound_set_command(gtk_entry_get_text(GTK_ENTRY(sndcmd)));
	return TRUE;
}
#endif

GtkWidget *sound_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkSizeGroup *sg;
#ifndef _WIN32
	GtkWidget *dd;
	GtkWidget *hbox;
	GtkWidget *label;
	char *cmd;
#endif

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

	vbox = gaim_gtk_make_frame (ret, _("Sound Options"));
	gaim_button(_("_No sounds when you log in"), &sound_options, OPT_SOUND_SILENT_SIGNON, vbox);
	gaim_button(_("_Sounds while away"), &sound_options, OPT_SOUND_WHEN_AWAY, vbox);

#ifndef _WIN32
	vbox = gaim_gtk_make_frame (ret, _("Sound Method"));
	dd = gaim_dropdown(vbox, _("_Method"), &sound_options, OPT_SOUND_BEEP |
		      OPT_SOUND_ESD | OPT_SOUND_ARTS | OPT_SOUND_NAS |
			  OPT_SOUND_NORMAL | OPT_SOUND_CMD,
		      _("Console beep"), OPT_SOUND_BEEP,
#ifdef USE_AO
		      _("Automatic"), OPT_SOUND_NORMAL,
			  "ESD", OPT_SOUND_ESD,
			  "Arts", OPT_SOUND_ARTS,
#endif
#ifdef USE_NAS_AUDIO
			  "NAS", OPT_SOUND_NAS,
#endif
		      _("Command"), OPT_SOUND_CMD, NULL);
	gtk_size_group_add_widget(sg, dd);
	gtk_misc_set_alignment(GTK_MISC(dd), 0, 0);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	label = gtk_label_new_with_mnemonic(_("Sound c_ommand\n(%s for filename)"));
	gtk_size_group_add_widget(sg, label);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);

	sndcmd = gtk_entry_new();
	gtk_label_set_mnemonic_widget(GTK_LABEL(label), sndcmd);

	gtk_editable_set_editable(GTK_EDITABLE(sndcmd), TRUE);
	cmd = gaim_sound_get_command();
	if(cmd)
		gtk_entry_set_text(GTK_ENTRY(sndcmd), cmd);
	gtk_widget_set_size_request(sndcmd, 75, -1);

	gtk_widget_set_sensitive(sndcmd, (sound_options & OPT_SOUND_CMD));
	gtk_box_pack_start(GTK_BOX(hbox), sndcmd, TRUE, TRUE, 5);
	g_signal_connect(G_OBJECT(sndcmd), "changed",
					 G_CALLBACK(sound_cmd_yeah), NULL);
#endif /* _WIN32 */
	gtk_widget_show_all(ret);
	return ret;
}

GtkWidget *away_page() {
	GtkWidget *ret;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *button;
	GtkWidget *select;
	GtkWidget *dd;
	GtkSizeGroup *sg;

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

	vbox = gaim_gtk_make_frame (ret, _("Away"));
	gaim_button(_("_Sending messages removes away status"), &away_options, OPT_AWAY_BACK_ON_IM, vbox);
	gaim_button(_("_Queue new messages when away"), &away_options, OPT_AWAY_QUEUE, vbox);

	vbox = gaim_gtk_make_frame (ret, _("Auto-response"));
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	gaim_labeled_spin_button(hbox, _("Seconds before _resending:"),
				 &away_resend, 1, 24 * 60 * 60, sg);
	gaim_button(_("_Don't send auto-response"), &away_options, OPT_AWAY_NO_AUTO_RESP, vbox);
	gaim_button(_("_Only send auto-response when idle"), &away_options, OPT_AWAY_IDLE_RESP, vbox);
	gaim_button(_("Do_n't send auto-response in active conversations"), &away_options, OPT_AWAY_DELAY_IN_USE, vbox);

	if (away_options & OPT_AWAY_NO_AUTO_RESP)
		gtk_widget_set_sensitive(hbox, FALSE);

	vbox = gaim_gtk_make_frame (ret, _("Idle"));
	dd = gaim_dropdown(vbox, _("Idle _time reporting:"), &report_idle, -1,
			   _("None"), IDLE_NONE,
			   _("Gaim usage"), IDLE_GAIM,
#ifdef USE_SCREENSAVER
#ifndef _WIN32
			   _("X usage"), IDLE_SCREENSAVER,
#else
			   _("Windows usage"), IDLE_SCREENSAVER,
#endif
#endif
			   NULL);
	gtk_size_group_add_widget(sg, dd);
	gtk_misc_set_alignment(GTK_MISC(dd), 0, 0);

	vbox = gaim_gtk_make_frame (ret, _("Auto-away"));
	button = gaim_button(_("Set away _when idle"), &away_options, OPT_AWAY_AUTO, vbox);
	select = gaim_labeled_spin_button(vbox, _("_Minutes before setting away:"), &auto_away, 1, 24 * 60, sg);
	if (!(away_options & OPT_AWAY_AUTO))
		gtk_widget_set_sensitive(GTK_WIDGET(select), FALSE);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(gaim_gtk_toggle_sensitive), select);

	label = gtk_label_new_with_mnemonic(_("Away m_essage:"));
	gtk_size_group_add_widget(sg, label);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	prefs_away_menu = gtk_option_menu_new();
	gtk_label_set_mnemonic_widget(GTK_LABEL(label), prefs_away_menu);
	if (!(away_options & OPT_AWAY_AUTO))
		gtk_widget_set_sensitive(GTK_WIDGET(prefs_away_menu), FALSE);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(gaim_gtk_toggle_sensitive), prefs_away_menu);
	default_away_menu_init(prefs_away_menu);
	gtk_widget_show(prefs_away_menu);
	gtk_box_pack_start(GTK_BOX(hbox), prefs_away_menu, FALSE, FALSE, 0);

	gtk_widget_show_all(ret);
	return ret;
}

static GtkWidget *plugin_description=NULL, *plugin_details=NULL;

static void prefs_plugin_sel (GtkTreeSelection *sel, GtkTreeModel *model) 
{
	gchar *buf, *pname, *perr, *pdesc, *pauth, *pweb;
	GtkTreeIter  iter;
	GValue val = { 0, };
	GaimPlugin *plug;

	if (! gtk_tree_selection_get_selected (sel, &model, &iter))
		return;
	gtk_tree_model_get_value (model, &iter, 2, &val);
	plug = g_value_get_pointer(&val);
	
	pname = g_markup_escape_text(_(plug->info->name), -1);
	pdesc = g_markup_escape_text(_(plug->info->description), -1);
	pauth = g_markup_escape_text(_(plug->info->author), -1);
	pweb = g_markup_escape_text(_(plug->info->homepage), -1);
	if (plug->error != NULL) {
		perr = g_markup_escape_text(_(plug->error), -1);
		buf = g_strdup_printf(
				"<span size=\"larger\">%s %s</span>\n\n"
				"<span weight=\"bold\" color=\"red\">%s</span>\n\n"
				"%s",
				pname, plug->info->version, perr, pdesc);
		g_free(perr);
	}
	else {
		buf = g_strdup_printf(
				"<span size=\"larger\">%s %s</span>\n\n%s",
				pname, plug->info->version, pdesc);
	}
	gtk_label_set_markup(GTK_LABEL(plugin_description), buf);
	g_free(buf);

	buf = g_strdup_printf(
#ifndef _WIN32
		   _("<span size=\"larger\">%s %s</span>\n\n"
		     "<span weight=\"bold\">Written by:</span>\t%s\n"
		     "<span weight=\"bold\">Web site:</span>\t\t%s\n"
		     "<span weight=\"bold\">File name:</span>\t%s"),
#else
		   _("<span size=\"larger\">%s %s</span>\n\n"
		     "<span weight=\"bold\">Written by:</span>  %s\n"
		     "<span weight=\"bold\">URL:</span>  %s\n"
		     "<span weight=\"bold\">File name:</span>  %s"),
#endif
		   pname, plug->info->version, pauth, pweb, plug->path);

	gtk_label_set_markup(GTK_LABEL(plugin_details), buf);
	g_value_unset(&val);
	g_free(buf);
	g_free(pname);
	g_free(pdesc);
	g_free(pauth);
	g_free(pweb);
}

static void plugin_load (GtkCellRendererToggle *cell, gchar *pth, gpointer data)
{
	GtkTreeModel *model = (GtkTreeModel *)data;
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_from_string(pth);
	GaimPlugin *plug;
	gchar buf[1024];
	
	GdkCursor *wait = gdk_cursor_new (GDK_WATCH);
	gdk_window_set_cursor(prefs->window, wait);
	gdk_cursor_unref(wait);
  
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter, 2, &plug, -1);
	
	if (!gaim_plugin_is_loaded(plug)) {
		gaim_plugin_load(plug);

		/*
		 * NOTE: This is basically the same check as before
		 *       (plug->type == plugin), but now there aren't plugin types.
		 *       Not yet, anyway. I want to do a V2 of the plugin API.
		 *       The thing is, we should have a flag specifying the UI type,
		 *       or just whether it's a general plugin or a UI-specific
		 *       plugin. We should only load this if it's UI-specific.
		 *
		 *         -- ChipX86
		 */
		if (GAIM_IS_GTK_PLUGIN(plug))
		{
			GtkWidget *config_frame;
			GaimGtkPluginUiInfo *ui_info;

			ui_info = GAIM_GTK_PLUGIN_UI_INFO(plug);
			config_frame = gaim_gtk_plugin_get_config_frame(plug);

			if (config_frame != NULL) {
				ui_info->iter = g_new0(GtkTreeIter, 1);
				prefs_notebook_add_page(_(plug->info->name), NULL,
										config_frame, ui_info->iter,
										&plugin_iter, notebook_page++);

				if (gtk_tree_model_iter_n_children(GTK_TREE_MODEL(prefstree),
												   &plugin_iter) == 1) {

					/* Expand the tree for the first plugin added */
					GtkTreePath *path2;
					
					path2 = gtk_tree_model_get_path(GTK_TREE_MODEL(prefstree),
													&plugin_iter);
					gtk_tree_view_expand_row(GTK_TREE_VIEW(tree_v),
											 path2, TRUE);
					gtk_tree_path_free(path2);
				}
			}
		}
	}
	else {
		if (GAIM_IS_GTK_PLUGIN(plug)) {
			GaimGtkPluginUiInfo *ui_info;

			ui_info = GAIM_GTK_PLUGIN_UI_INFO(plug);

			if (ui_info != NULL && ui_info->iter != NULL) {
				gtk_tree_store_remove(GTK_TREE_STORE(prefstree), ui_info->iter);
				g_free(ui_info->iter);
				ui_info->iter = NULL;
			}
		}

		gaim_plugin_unload(plug);
	}

	gdk_window_set_cursor(prefs->window, NULL);

	if (plug->error != NULL) {
		g_snprintf(buf, sizeof(buf),
				   "<span size=\"larger\">%s %s</span>\n\n"
				   "<span weight=\"bold\" color=\"red\">%s</span>\n\n"
				   "%s",
				   g_markup_escape_text(_(plug->info->name), -1),
				   plug->info->version,
				   g_markup_escape_text(plug->error, -1),
				   g_markup_escape_text(_(plug->info->description), -1));
	}
	else {
		g_snprintf(buf, sizeof(buf),
				   "<span size=\"larger\">%s %s</span>\n\n%s",
				   g_markup_escape_text(_(plug->info->name), -1),
				   plug->info->version,
				   g_markup_escape_text(_(plug->info->description), -1));
	}

	gtk_label_set_markup(GTK_LABEL(plugin_description), buf);
	gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0,
						gaim_plugin_is_loaded(plug), -1);
	
	gtk_label_set_markup(GTK_LABEL(plugin_description), buf);
	gtk_tree_path_free(path);
}

static void
update_plugin_list(void *data)
{
	GtkListStore *ls = GTK_LIST_STORE(data);
	GtkTreeIter iter;
	GList *probes;
	GaimPlugin *plug;

	gtk_list_store_clear(ls);

	for (probes = gaim_plugins_get_all();
		 probes != NULL;
		 probes = probes->next) {

		plug = probes->data;

		if (plug->info->type != GAIM_PLUGIN_STANDARD)
			continue;

		gtk_list_store_append (ls, &iter);
		gtk_list_store_set(ls, &iter,
				   0, gaim_plugin_is_loaded(plug),
				   1, plug->info->name ? _(plug->info->name) : plug->path, 
				   2, plug, -1);
	}
}

static GtkWidget *plugin_page ()
{
	GtkWidget *ret;
	GtkWidget *sw, *vp;
	GtkWidget *event_view;
	GtkListStore *ls;
	GtkCellRenderer *rend, *rendt;
	GtkTreeViewColumn *col;
	GtkTreeSelection *sel;
	GtkTreePath *path;
	GtkWidget *nb;

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	sw = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_IN);

	gtk_box_pack_start(GTK_BOX(ret), sw, TRUE, TRUE, 0);

	ls = gtk_list_store_new (3, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_POINTER);

	update_plugin_list(ls);

	event_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(ls));

	rend = gtk_cell_renderer_toggle_new();
	sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (event_view));

	
	col = gtk_tree_view_column_new_with_attributes (_("Load"),
							rend,
							"active", 0,
							NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(event_view), col);

	rendt = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes (_("Name"),
							rendt,
							"text", 1,
							NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(event_view), col);
	g_object_unref(G_OBJECT(ls));
	gtk_container_add(GTK_CONTAINER(sw), event_view);
	

	nb = gtk_notebook_new();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK(nb), GTK_POS_BOTTOM);
	gtk_notebook_popup_disable(GTK_NOTEBOOK(nb));
	
	/* Description */
	sw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	plugin_description = gtk_label_new(NULL);
	
	vp = gtk_viewport_new(NULL, NULL);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(vp), GTK_SHADOW_NONE);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_NONE);

	gtk_container_add(GTK_CONTAINER(vp), plugin_description);
	gtk_container_add(GTK_CONTAINER(sw), vp);

	gtk_label_set_selectable(GTK_LABEL(plugin_description), TRUE);  
	gtk_label_set_line_wrap(GTK_LABEL(plugin_description), TRUE);
	gtk_misc_set_alignment(GTK_MISC(plugin_description), 0, 0);
	gtk_misc_set_padding(GTK_MISC(plugin_description), 6, 6);
	gtk_notebook_append_page(GTK_NOTEBOOK(nb), sw, gtk_label_new(_("Description")));

	/* Details */
	sw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	plugin_details = gtk_label_new(NULL);
	
	vp = gtk_viewport_new(NULL, NULL);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(vp), GTK_SHADOW_NONE);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_NONE);

	gtk_container_add(GTK_CONTAINER(vp), plugin_details);
	gtk_container_add(GTK_CONTAINER(sw), vp);

	gtk_label_set_selectable(GTK_LABEL(plugin_details), TRUE);  
	gtk_label_set_line_wrap(GTK_LABEL(plugin_details), TRUE);
	gtk_misc_set_alignment(GTK_MISC(plugin_details), 0, 0);
	gtk_misc_set_padding(GTK_MISC(plugin_details), 6, 6);	
	gtk_notebook_append_page(GTK_NOTEBOOK(nb), sw, gtk_label_new(_("Details")));
	gtk_box_pack_start(GTK_BOX(ret), nb, TRUE, TRUE, 0);

	g_signal_connect (G_OBJECT (sel), "changed",
			  G_CALLBACK (prefs_plugin_sel),
			  NULL); 
	g_signal_connect (G_OBJECT(rend), "toggled",
			  G_CALLBACK(plugin_load), ls);

	path = gtk_tree_path_new_first();
	gtk_tree_selection_select_path(sel, path);
	gtk_tree_path_free(path);

	gaim_plugins_register_probe_notify_cb(update_plugin_list, ls);

	gtk_widget_show_all(ret);
	return ret;
}

static void event_toggled (GtkCellRendererToggle *cell, gchar *pth, gpointer data)
{
	GtkTreeModel *model = (GtkTreeModel *)data;
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_from_string(pth);
	gint soundnum;

	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter, 2, &soundnum, -1);

	sound_options ^= gaim_sound_get_event_option(soundnum);
	gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, sound_options & gaim_sound_get_event_option(soundnum), -1);

	gtk_tree_path_free(path);
}

static void test_sound(GtkWidget *button, gpointer i_am_NULL)
{
	guint32 tmp_sound = sound_options;
	if (!(sound_options & OPT_SOUND_WHEN_AWAY))
		sound_options ^= OPT_SOUND_WHEN_AWAY;
	if (!(sound_options & gaim_sound_get_event_option(sound_row_sel)))
		sound_options ^= gaim_sound_get_event_option(sound_row_sel);
	gaim_sound_play_event(sound_row_sel);

	sound_options = tmp_sound;
}

static void reset_sound(GtkWidget *button, gpointer i_am_also_NULL)
{
	/* This just resets a sound file back to default */
	gaim_sound_set_event_file(sound_row_sel, NULL);

	gtk_entry_set_text(GTK_ENTRY(sound_entry), "(default)");
}

void close_sounddialog(GtkWidget *w, GtkWidget *w2)
{

	GtkWidget *dest;

	if (!GTK_IS_WIDGET(w2))
		dest = w;
	else
		dest = w2;

	sounddialog = NULL;

	gtk_widget_destroy(dest);
}

void do_select_sound(GtkWidget *w, int snd)
{
	const char *file;

	file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(sounddialog));

	/* If they type in a directory, change there */
	if (file_is_dir(file, sounddialog))
		return;

	/* Set it -- and forget it */
	gaim_sound_set_event_file(snd, file);

	/* Set our text entry */
	gtk_entry_set_text(GTK_ENTRY(sound_entry), file);

	/* Close the window! It's getting cold in here! */
	close_sounddialog(NULL, sounddialog);

	if (last_sound_dir)
		g_free(last_sound_dir);
	last_sound_dir = g_path_get_dirname(file);
}

static void sel_sound(GtkWidget *button, gpointer being_NULL_is_fun)
{
	char *buf = g_malloc(BUF_LEN);

	if (!sounddialog) {
		sounddialog = gtk_file_selection_new(_("Sound Selection"));

		gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(sounddialog));

		g_snprintf(buf, BUF_LEN - 1, "%s" G_DIR_SEPARATOR_S, last_sound_dir ? last_sound_dir : gaim_home_dir());

		gtk_file_selection_set_filename(GTK_FILE_SELECTION(sounddialog), buf);

		g_signal_connect(G_OBJECT(sounddialog), "destroy",
						 G_CALLBACK(close_sounddialog), sounddialog);

		g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(sounddialog)->ok_button),
						 "clicked",
						 G_CALLBACK(do_select_sound), (int *)sound_row_sel);

		g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(sounddialog)->cancel_button),
						 "clicked",
						 G_CALLBACK(close_sounddialog), sounddialog);
	}

	g_free(buf);
	gtk_widget_show(sounddialog);
	gdk_window_raise(sounddialog->window);
}


static void prefs_sound_sel (GtkTreeSelection *sel, GtkTreeModel *model) {
	GtkTreeIter  iter;
	GValue val = { 0, };
	char *file;

	if (! gtk_tree_selection_get_selected (sel, &model, &iter))
		return;
	gtk_tree_model_get_value (model, &iter, 2, &val);
	sound_row_sel = g_value_get_uint(&val);
	file = gaim_sound_get_event_file(sound_row_sel);
	if (sound_entry)
		gtk_entry_set_text(GTK_ENTRY(sound_entry), file ? file : "(default)");
	g_value_unset (&val);
	if (sounddialog)
		gtk_widget_destroy(sounddialog);
}

GtkWidget *sound_events_page() {

	GtkWidget *ret;
	GtkWidget *sw;
	GtkWidget *button, *hbox;
	GtkTreeIter iter;
	GtkWidget *event_view;
	GtkListStore *event_store;
	GtkCellRenderer *rend;
	GtkTreeViewColumn *col;
	GtkTreeSelection *sel;
	GtkTreePath *path;
	int j;
	char *file;

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	sw = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_IN);

	gtk_box_pack_start(GTK_BOX(ret), sw, TRUE, TRUE, 0);
	event_store = gtk_list_store_new (3, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_UINT);

	for (j=0; j < GAIM_NUM_SOUNDS; j++) {
		guint opt = gaim_sound_get_event_option(j);
		if (opt == 0)
			continue;

		gtk_list_store_append (event_store, &iter);
		gtk_list_store_set(event_store, &iter,
				   0, sound_options & opt,
				   1, gettext(gaim_sound_get_event_label(j)),
				   2, j, -1);
	}

	event_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(event_store));

	rend = gtk_cell_renderer_toggle_new();
	sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (event_view));
	g_signal_connect (G_OBJECT (sel), "changed",
			  G_CALLBACK (prefs_sound_sel),
			  NULL);
	g_signal_connect (G_OBJECT(rend), "toggled",
			  G_CALLBACK(event_toggled), event_store);
	path = gtk_tree_path_new_first();
	gtk_tree_selection_select_path(sel, path);
	gtk_tree_path_free(path);

	col = gtk_tree_view_column_new_with_attributes (_("Play"),
							rend,
							"active", 0,
							NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(event_view), col);

	rend = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes (_("Event"),
							rend,
							"text", 1,
							NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(event_view), col);
	g_object_unref(G_OBJECT(event_store));
	gtk_container_add(GTK_CONTAINER(sw), event_view);

	hbox = gtk_hbox_new(FALSE, 6);
	gtk_box_pack_start(GTK_BOX(ret), hbox, FALSE, FALSE, 0);
	sound_entry = gtk_entry_new();
	file = gaim_sound_get_event_file(0);
	gtk_entry_set_text(GTK_ENTRY(sound_entry), file ? file : "(default)");
	gtk_editable_set_editable(GTK_EDITABLE(sound_entry), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), sound_entry, FALSE, FALSE, 5);

	button = gtk_button_new_with_label(_("Test"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(test_sound), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 1);

	button = gtk_button_new_with_label(_("Reset"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(reset_sound), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 1);

	button = gtk_button_new_with_label(_("Choose..."));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(sel_sound), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 1);

	gtk_widget_show_all (ret);

	return ret;
}

void away_message_sel(GtkTreeSelection *sel, GtkTreeModel *model)
{
	GtkTreeIter  iter;
	GValue val = { 0, };
	gchar buffer[BUF_LONG];
	char *tmp;
	struct away_message *am;

	if (! gtk_tree_selection_get_selected (sel, &model, &iter))
		return;
	gtk_tree_model_get_value (model, &iter, 1, &val);
	am = g_value_get_pointer(&val);
	gtk_imhtml_clear(GTK_IMHTML(away_text));
	strncpy(buffer, am->message, BUF_LONG);
	tmp = stylize(buffer, BUF_LONG);
	gtk_imhtml_append_text(GTK_IMHTML(away_text), tmp, -1, GTK_IMHTML_NO_TITLE |
			       GTK_IMHTML_NO_COMMENTS | GTK_IMHTML_NO_SCROLL);
	gtk_imhtml_append_text(GTK_IMHTML(away_text), "<BR>", -1, GTK_IMHTML_NO_TITLE |
			       GTK_IMHTML_NO_COMMENTS | GTK_IMHTML_NO_SCROLL);
	g_free(tmp);
	g_value_unset (&val);

}

void remove_away_message(GtkWidget *widget, GtkTreeView *tv) {
        struct away_message *am;
	GtkTreeIter iter;
	GtkTreeSelection *sel = gtk_tree_view_get_selection(tv);
	GtkTreeModel *model = GTK_TREE_MODEL(prefs_away_store);
	GValue val = { 0, };

	if (! gtk_tree_selection_get_selected (sel, &model, &iter))
		return;
	gtk_tree_model_get_value (GTK_TREE_MODEL(prefs_away_store), &iter, 1, &val);
	am = g_value_get_pointer (&val);
	gtk_imhtml_clear(GTK_IMHTML(away_text));
	rem_away_mess(NULL, am);
}

GtkWidget *away_message_page() {
	GtkWidget *ret;
	GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *sw;
	GtkTreeIter iter;
	GtkWidget *event_view;
	GtkCellRenderer *rend;
	GtkTreeViewColumn *col;
	GtkTreeSelection *sel;
	GSList *awy = away_messages;
	struct away_message *a;
	GtkWidget *sw2;
	GtkSizeGroup *sg;

	ret = gtk_vbox_new(FALSE, 18);
	gtk_container_set_border_width (GTK_CONTAINER (ret), 12);

	sg = gtk_size_group_new(GTK_SIZE_GROUP_BOTH);

	sw = gtk_scrolled_window_new(NULL,NULL);
	away_text = gtk_imhtml_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	/*
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_IN);
	*/
	gtk_box_pack_start(GTK_BOX(ret), sw, TRUE, TRUE, 0);

	prefs_away_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
	while (awy) {
		a = (struct away_message *)awy->data;
		gtk_list_store_append (prefs_away_store, &iter);
		gtk_list_store_set(prefs_away_store, &iter,
				   0, a->name,
				   1, a, -1);
		awy = awy->next;
	}
	event_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(prefs_away_store));


	rend = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes ("NULL",
							rend,
							"text", 0,
							NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(event_view), col);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(event_view), FALSE);
	gtk_widget_show(event_view);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), event_view);

	sw2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw2),
				       GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(ret), sw2, TRUE, TRUE, 0);

	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw2), away_text);
	gaim_setup_imhtml(away_text);
	sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (event_view));
	g_signal_connect (G_OBJECT (sel), "changed",
			  G_CALLBACK (away_message_sel),
			  NULL);
	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(ret), hbox, FALSE, FALSE, 0);
	button = gtk_button_new_from_stock (GTK_STOCK_ADD);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	gtk_size_group_add_widget(sg, button);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(create_away_mess), NULL);

	button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
	gtk_size_group_add_widget(sg, button);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(remove_away_message), event_view);

	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);

	button = gaim_pixbuf_button_from_stock(_("_Edit"), GAIM_STOCK_EDIT, GAIM_BUTTON_HORIZONTAL);
	gtk_size_group_add_widget(sg, button);
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(create_away_mess), event_view);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);

	gtk_widget_show_all(ret);
	return ret;
}

GtkTreeIter *prefs_notebook_add_page(const char *text,
				     GdkPixbuf *pixbuf,
				     GtkWidget *page,
				     GtkTreeIter *iter,
				     GtkTreeIter *parent,
				     int ind) {
	GdkPixbuf *icon = NULL;

	if (pixbuf)
		icon = gdk_pixbuf_scale_simple (pixbuf, 18, 18, GDK_INTERP_BILINEAR);

	gtk_tree_store_append (prefstree, iter, parent);
	gtk_tree_store_set (prefstree, iter, 0, icon, 1, text, 2, ind, -1);

	if (pixbuf)
		g_object_unref(pixbuf);
	if (icon)
		g_object_unref(icon);
	gtk_notebook_append_page(GTK_NOTEBOOK(prefsnotebook), page, gtk_label_new(text));
	return iter;
}

void prefs_notebook_init() {
	GtkTreeIter p, p2, c;
	GList *l;
	GaimPlugin *plug;
	prefs_notebook_add_page(_("Interface"), NULL, interface_page(), &p, NULL, notebook_page++);
	prefs_notebook_add_page(_("Smiley Themes"), NULL, theme_page(), &c, &p, notebook_page++);
	prefs_notebook_add_page(_("Fonts"), NULL, font_page(), &c, &p, notebook_page++);
	prefs_notebook_add_page(_("Message Text"), NULL, messages_page(), &c, &p, notebook_page++);
	prefs_notebook_add_page(_("Shortcuts"), NULL, hotkeys_page(), &c, &p, notebook_page++);
	prefs_notebook_add_page(_("Buddy List"), NULL, list_page(), &c, &p, notebook_page++);
	prefs_notebook_add_page(_("Conversations"), NULL, conv_page(), &p2, NULL, notebook_page++);
	prefs_notebook_add_page(_("IMs"), NULL, im_page(), &c, &p2, notebook_page++);
	prefs_notebook_add_page(_("Chats"), NULL, chat_page(), &c, &p2, notebook_page++);
	prefs_notebook_add_page(_("Tabs"), NULL, tab_page(), &c, &p2, notebook_page++);
	prefs_notebook_add_page(_("Proxy"), NULL, proxy_page(), &p, NULL, notebook_page++);
#ifndef _WIN32
	/* We use the registered default browser in windows */
	prefs_notebook_add_page(_("Browser"), NULL, browser_page(), &p, NULL, notebook_page++);
#endif
	prefs_notebook_add_page(_("Logging"), NULL, logging_page(), &p, NULL, notebook_page++);
	prefs_notebook_add_page(_("Sounds"), NULL, sound_page(), &p, NULL, notebook_page++);
	prefs_notebook_add_page(_("Sound Events"), NULL, sound_events_page(), &c, &p, notebook_page++);
	prefs_notebook_add_page(_("Away / Idle"), NULL, away_page(), &p, NULL, notebook_page++);
	prefs_notebook_add_page(_("Away Messages"), NULL, away_message_page(), &c, &p, notebook_page++);

	if (gaim_plugins_enabled()) {
		prefs_notebook_add_page(_("Plugins"), NULL, plugin_page(), &plugin_iter, NULL, notebook_page++);

		for (l = gaim_plugins_get_loaded(); l != NULL; l = l->next) {
			plug = l->data;

			if (GAIM_IS_GTK_PLUGIN(plug)) {
				GtkWidget *config_frame;
				GaimGtkPluginUiInfo *ui_info;

				ui_info = GAIM_GTK_PLUGIN_UI_INFO(plug);
				config_frame = gaim_gtk_plugin_get_config_frame(plug);

				if (config_frame != NULL) {
					ui_info->iter = g_new0(GtkTreeIter, 1);
					prefs_notebook_add_page(_(plug->info->name), NULL,
											config_frame, ui_info->iter,
											&plugin_iter, notebook_page++);
				}
			}
		}
	}
}

void gaim_gtk_prefs_show(void)
{
	GtkWidget *vbox, *vbox2;
	GtkWidget *hbox;
	GtkWidget *frame;
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell;
	GtkTreeSelection *sel;
	GtkWidget *notebook;
	GtkWidget *sep;
	GtkWidget *button;
	GtkSizeGroup *sg = gtk_size_group_new(GTK_SIZE_GROUP_BOTH);

	if (prefs) {
		gtk_window_present(GTK_WINDOW(prefs));
		return;
	}

	/* copy the preferences to tmp values...
	 * I liked "take affect immediately" Oh well :-( */
	
	/* Back to instant-apply! I win!  BU-HAHAHA! */

	/* Create the window */
	prefs = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_role(GTK_WINDOW(prefs), "preferences");
	gtk_widget_realize(prefs);
	gtk_window_set_title(GTK_WINDOW(prefs), _("Preferences"));
	gtk_window_set_resizable (GTK_WINDOW(prefs), FALSE);
	g_signal_connect(G_OBJECT(prefs), "destroy",
					 G_CALLBACK(delete_prefs), NULL);

	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
	gtk_container_add(GTK_CONTAINER(prefs), vbox);
	gtk_widget_show(vbox);

	hbox = gtk_hbox_new (FALSE, 6);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 6);
	gtk_container_add (GTK_CONTAINER(vbox), hbox);
	gtk_widget_show (hbox);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
	gtk_box_pack_start (GTK_BOX (hbox), frame, FALSE, FALSE, 0);
	gtk_widget_show (frame);

	/* The tree -- much inspired by the Gimp */
	prefstree = gtk_tree_store_new (3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	tree_v = gtk_tree_view_new_with_model (GTK_TREE_MODEL (prefstree));
	gtk_container_add (GTK_CONTAINER (frame), tree_v);

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (tree_v), FALSE);
	gtk_widget_show(tree_v);
	/* icons */
	/* XXX: to be used at a later date
	cell = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes ("icons", cell, "pixbuf", 0, NULL);
	*/

	/* text */
	cell = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("text", cell, "text", 1, NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_v), column);

	/* The right side */
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
	gtk_box_pack_start (GTK_BOX (hbox), frame, TRUE, TRUE, 0);
	gtk_widget_show (frame);

	vbox2 = gtk_vbox_new (FALSE, 4);
	gtk_container_add (GTK_CONTAINER (frame), vbox2);
	gtk_widget_show (vbox2);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	gtk_box_pack_start (GTK_BOX (vbox2), frame, FALSE, TRUE, 0);
	gtk_widget_show (frame);

	hbox = gtk_hbox_new (FALSE, 4);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 4);
	gtk_container_add (GTK_CONTAINER (frame), hbox);
	gtk_widget_show (hbox);

	preflabel = gtk_label_new(NULL);
	gtk_box_pack_end (GTK_BOX (hbox), preflabel, FALSE, FALSE, 0);
	gtk_widget_show (preflabel);

	/* The notebook */
	prefsnotebook = notebook = gtk_notebook_new ();
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook), FALSE);
	gtk_box_pack_start (GTK_BOX (vbox2), notebook, FALSE, FALSE, 0);

	sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_v));
	g_signal_connect (G_OBJECT (sel), "changed",
			   G_CALLBACK (pref_nb_select),
			   notebook);
	gtk_widget_show(notebook);
	sep = gtk_hseparator_new();
	gtk_widget_show(sep);
	gtk_box_pack_start (GTK_BOX (vbox), sep, FALSE, FALSE, 0);

	/* The buttons^H to press! */
	hbox = gtk_hbox_new (FALSE, 6);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 6);
	gtk_container_add (GTK_CONTAINER(vbox), hbox);
	gtk_widget_show (hbox);

	button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	gtk_size_group_add_widget(sg, button);
	g_signal_connect_swapped(G_OBJECT(button), "clicked",
							 G_CALLBACK(gtk_widget_destroy), prefs);
	gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	prefs_notebook_init();

	gtk_tree_view_expand_all (GTK_TREE_VIEW(tree_v));
	gtk_widget_show(prefs);
}

static void set_misc_option(GtkWidget *w, int option)
{
	misc_options ^= option;

	if(option == OPT_MISC_USE_SERVER_ALIAS) {
		/* XXX blist reset the aliases here */
		gaim_conversation_foreach(gaim_conversation_autoset_title);
	}
}

static void set_logging_option(GtkWidget *w, int option)
{
	logging_options ^= option;

	if (option == OPT_LOG_CONVOS || option == OPT_LOG_CHATS)
		update_log_convs();
}

static void set_blist_option(GtkWidget *w, int option)
{
	struct gaim_gtk_buddy_list *gtkblist;

	gtkblist = GAIM_GTK_BLIST(gaim_get_blist());

	blist_options ^= option;

	if (!gtkblist)
		return;

	if (option == OPT_BLIST_SHOW_WARN)
		gaim_gtk_blist_update_columns();
	else if (option == OPT_BLIST_SHOW_IDLETIME) {
		gaim_gtk_blist_update_refresh_timeout();
		gaim_gtk_blist_update_columns();
	}
	else if (option == OPT_BLIST_SHOW_ICONS) {
		gaim_gtk_blist_update_refresh_timeout();
		gaim_gtk_blist_refresh(gaim_get_blist());
		gaim_gtk_blist_update_columns();
	} else
		gaim_gtk_blist_refresh(gaim_get_blist());

}

static void set_convo_option(GtkWidget *w, int option)
{
	convo_options ^= option;

	if (option == OPT_CONVO_SHOW_SMILEY)
		gaim_gtkconv_toggle_smileys();

	if (option == OPT_CONVO_SHOW_TIME)
		gaim_gtkconv_toggle_timestamps();

	if (option == OPT_CONVO_CHECK_SPELLING)
		gaim_gtkconv_toggle_spellchk();

	if (option == OPT_CONVO_NO_X_ON_TAB)
		gaim_gtkconv_toggle_close_buttons();
}

static void set_im_option(GtkWidget *w, int option)
{
	im_options ^= option;

#if 0
	if (option == OPT_IM_ONE_WINDOW)
		im_tabize();
#endif

	if (option == OPT_IM_HIDE_ICONS)
		gaim_gtkconv_hide_buddy_icons();

	if (option == OPT_IM_ALIAS_TAB)
		gaim_conversation_foreach(gaim_conversation_autoset_title);

	if (option == OPT_IM_NO_ANIMATION)
		gaim_gtkconv_set_anim();
}

static void set_chat_option(GtkWidget *w, int option)
{
	chat_options ^= option;

#if 0
	if (option == OPT_CHAT_ONE_WINDOW)
		chat_tabize();
#endif
}

void set_sound_option(GtkWidget *w, int option)
{
	sound_options ^= option;
}

static void set_font_option(GtkWidget *w, int option)
{
	font_options ^= option;

	gaim_gtkconv_update_font_buttons();
}

static void set_away_option(GtkWidget *w, int option)
{
	away_options ^= option;

	if (option == OPT_AWAY_QUEUE)
		toggle_away_queue();
}

GtkWidget *gaim_button(const char *text, guint *options, int option, GtkWidget *page)
{
	GtkWidget *button;
	button = gtk_check_button_new_with_mnemonic(text);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), (*options & option));
	gtk_box_pack_start(GTK_BOX(page), button, FALSE, FALSE, 0);
	g_object_set_data(G_OBJECT(button), "options", options);

	if (options == &misc_options) {
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(set_misc_option), (int *)option);
	} else if (options == &logging_options) {
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(set_logging_option), (int *)option);
	} else if (options == &blist_options) {
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(set_blist_option), (int *)option);
	} else if (options == &convo_options) {
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(set_convo_option), (int *)option);
	} else if (options == &im_options) {
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(set_im_option), (int *)option);
	} else if (options == &chat_options) {
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(set_chat_option), (int *)option);
	} else if (options == &font_options) {
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(set_font_option), (int *)option);
	} else if (options == &sound_options) {
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(set_sound_option), (int *)option);
	} else if (options == &away_options) {
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(set_away_option), (int *)option);
	} else {
		gaim_debug(GAIM_DEBUG_WARNING, "gaim_button",
				   "\"%s\" has no signal handler attached to it!\n", text);
	}
	gtk_widget_show(button);

	return button;
}

void default_away_menu_init(GtkWidget *omenu)
{
	GtkWidget *menu, *opt;
	int index = 0;
	GSList *awy = away_messages;
	struct away_message *a;

	menu = gtk_menu_new();

	while (awy) {
		a = (struct away_message *)awy->data;
		opt = gtk_menu_item_new_with_label(a->name);
		g_signal_connect(G_OBJECT(opt), "activate",
						 G_CALLBACK(set_default_away), (gpointer)index);
		gtk_widget_show(opt);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), opt);

		awy = awy->next;
		index++;
	}

	gtk_option_menu_remove_menu(GTK_OPTION_MENU(omenu));
	gtk_option_menu_set_menu(GTK_OPTION_MENU(omenu), menu);
	gtk_option_menu_set_history(GTK_OPTION_MENU(omenu), g_slist_index(away_messages, default_away));
}

GtkWidget *pref_fg_picture = NULL;
GtkWidget *pref_bg_picture = NULL;

void destroy_colorsel(GtkWidget *w, gpointer d)
{
	if (d) {
		gtk_widget_destroy(fgcseld);
		fgcseld = NULL;
	} else {
		gtk_widget_destroy(bgcseld);
		bgcseld = NULL;
	}
}

void apply_color_dlg(GtkWidget *w, gpointer d)
{
	if ((int)d == 1) {
		gtk_color_selection_get_current_color(GTK_COLOR_SELECTION
						      (GTK_COLOR_SELECTION_DIALOG(fgcseld)->colorsel),
						      &fgcolor);
		destroy_colorsel(NULL, (void *)1);
		update_color(NULL, pref_fg_picture);
	} else {
		gtk_color_selection_get_current_color(GTK_COLOR_SELECTION
						      (GTK_COLOR_SELECTION_DIALOG(bgcseld)->colorsel),
						      &bgcolor);
		destroy_colorsel(NULL, (void *)0);
		update_color(NULL, pref_bg_picture);
	}
	gaim_conversation_foreach(gaim_gtkconv_update_font_colors);
}

void set_default_away(GtkWidget *w, gpointer i)
{

	int length = g_slist_length(away_messages);

	if (away_messages == NULL)
		default_away = NULL;
	else if ((int)i >= length)
		default_away = g_slist_nth_data(away_messages, length - 1);
	else
		default_away = g_slist_nth_data(away_messages, (int)i);
}

#ifndef _WIN32
static gboolean program_is_valid(const char *program) 
{
	GError *error = NULL;
	char **argv; 
	gchar *progname;
	gboolean is_valid = FALSE;

	if (program == NULL || *program == '\0') {
		return FALSE;
	}

	if (!g_shell_parse_argv(program, NULL, &argv, &error)) {
		gaim_debug(GAIM_DEBUG_ERROR, "program_is_valid",
				   "Could not parse program '%s': %s\n",
				   program, error->message);
		g_error_free(error);
		return FALSE;
	}

	if (argv == NULL) {
		return FALSE;
	}

	progname = g_find_program_in_path(argv[0]);
	is_valid = (progname != NULL);

	g_strfreev(argv);
	g_free(progname);
	
	return is_valid;
}
#endif

static void update_spin_value(GtkWidget *w, GtkWidget *spin)
{
	int *value = g_object_get_data(G_OBJECT(spin), "val");
	*value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
}

GtkWidget *gaim_labeled_spin_button(GtkWidget *box, const gchar *title, int *val, int min, int max, GtkSizeGroup *sg)
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *spin;
	GtkObject *adjust;

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new_with_mnemonic(title);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	adjust = gtk_adjustment_new(*val, min, max, 1, 1, 1);
	spin = gtk_spin_button_new(GTK_ADJUSTMENT(adjust), 1, 0);
	g_object_set_data(G_OBJECT(spin), "val", val);
	gtk_widget_set_size_request(spin, 50, -1);
	gtk_box_pack_start(GTK_BOX(hbox), spin, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(adjust), "value-changed",
					 G_CALLBACK(update_spin_value), GTK_WIDGET(spin));
	gtk_widget_show(spin);

	gtk_label_set_mnemonic_widget(GTK_LABEL(label), spin);

	if (sg) {
		gtk_size_group_add_widget(sg, label);
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0);

	}
	return label;
}

void dropdown_set(GObject *w, int *option)
{
	int opt = GPOINTER_TO_INT(g_object_get_data(w, "value"));
	int clear = GPOINTER_TO_INT(g_object_get_data(w, "clear"));

	if (option == (int*)&sort_method) {
		/* Hack city -- Population: Sean Egan */
		char *name = (char*)opt;
		 gaim_gtk_blist_sort_method_set(name);
		 return;
	}		
	if (clear != -1) {
		*option = *option & ~clear;
		*option = *option | opt;
	} else {
		gaim_debug(GAIM_DEBUG_MISC, "dropdown_set", "HELLO %d\n", opt);
		*option = opt;
	}

	if (option == (int*)&global_proxy_info.proxytype) {
		if (opt == PROXY_NONE)
			gtk_widget_set_sensitive(prefs_proxy_frame, FALSE);
		else
			gtk_widget_set_sensitive(prefs_proxy_frame, TRUE);
	} else if (option == &web_browser) {
		if (opt == BROWSER_MANUAL)
			gtk_widget_set_sensitive(gtk_widget_get_parent(browser_entry), TRUE);
		else
			gtk_widget_set_sensitive(gtk_widget_get_parent(browser_entry), FALSE);
	} else if (option == (int*)&sound_options) {
		if (opt == OPT_SOUND_CMD)
			gtk_widget_set_sensitive(sndcmd, TRUE);
		else
			gtk_widget_set_sensitive(sndcmd, FALSE);
		gaim_sound_change_output_method();
	} else if (option == (int*)&blist_options) {
		gaim_gtk_blist_update_toolbar();
	} else if (option == (int*)&im_options) { 
		if (clear == (OPT_IM_SIDE_TAB | OPT_IM_BR_TAB))
			gaim_gtkconv_update_tabs();
		else if (clear == (OPT_IM_BUTTON_TEXT | OPT_IM_BUTTON_XPM))
			gaim_gtkconv_update_im_button_style();
	} else if (option == (int*)&chat_options) {
		if (clear == (OPT_CHAT_SIDE_TAB | OPT_CHAT_BR_TAB))
			gaim_gtkconv_update_tabs();
		else if (clear == (OPT_CHAT_BUTTON_TEXT | OPT_CHAT_BUTTON_XPM))
			gaim_gtkconv_update_chat_button_style();
		//	} else if (option == (int*)&blist_options) {
		//    set_blist_tab();
	} else if (option == (int *)&conv_placement_option) {
		gaim_conv_placement_set_active(conv_placement_option);
	} 
}

static GtkWidget *gaim_dropdown(GtkWidget *box, const gchar *title, int *option, int clear, ...)
{
	va_list ap;
	GList *menuitems = NULL;
	GtkWidget *dropdown = NULL;
	char *name;
	int id;

	va_start(ap, clear);
	while ((name = va_arg(ap, char *)) != NULL) {
		id = va_arg(ap, int);

		menuitems = g_list_prepend(menuitems, name);
		menuitems = g_list_prepend(menuitems, GINT_TO_POINTER(id));
	}
	va_end(ap);

	g_return_val_if_fail(menuitems != NULL, NULL);

	menuitems = g_list_reverse(menuitems);

	dropdown = gaim_dropdown_from_list(box, title, option, clear, menuitems);

	g_list_free(menuitems);

	return dropdown;
}

static GtkWidget *gaim_dropdown_from_list(GtkWidget *box, const gchar *title, int *option, int clear, GList *menuitems)
{
	GtkWidget *dropdown, *opt, *menu;
	GtkWidget *label;
	gchar     *text;
	int       value;
	int       o = 0;
	GtkWidget *hbox;

	g_return_val_if_fail(menuitems != NULL, NULL);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add (GTK_CONTAINER (box), hbox);
	gtk_widget_show(hbox);

	label = gtk_label_new_with_mnemonic(title);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	dropdown = gtk_option_menu_new();
	menu = gtk_menu_new();

	gtk_label_set_mnemonic_widget(GTK_LABEL(label), dropdown);

	while (menuitems != NULL && (text = (char *) menuitems->data) != NULL) {
		menuitems = g_list_next(menuitems);
		g_return_val_if_fail(menuitems != NULL, NULL);
		value = GPOINTER_TO_INT(menuitems->data);
		menuitems = g_list_next(menuitems);

		opt = gtk_menu_item_new_with_label(text);
		g_object_set_data(G_OBJECT(opt), "value", GINT_TO_POINTER(value));
		g_object_set_data(G_OBJECT(opt), "clear", GINT_TO_POINTER(clear));
		g_signal_connect(G_OBJECT(opt), "activate",
						 G_CALLBACK(dropdown_set), (void *)option);
		gtk_widget_show(opt);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), opt);

		if (option == (int*)sort_method) {
			/* Now Entering Hacksville, Estd. May 17, 2003 */
			gtk_menu_set_active(GTK_MENU(menu), clear);
		} else if (((clear > -1) && ((*option & clear) == value)) || *option == value) {
			gtk_menu_set_active(GTK_MENU(menu), o);
		}
		o++;

	}

	gtk_option_menu_set_menu(GTK_OPTION_MENU(dropdown), menu);
	gtk_box_pack_start(GTK_BOX(hbox), dropdown, FALSE, FALSE, 0);
	gtk_widget_show(dropdown);
	return label;
}

static GtkWidget *show_color_pref(GtkWidget *box, gboolean fgc)
{
	/* more stuff stolen from X-Chat */
	GtkWidget *swid;
	GdkColor c;
	GtkStyle *style;
	c.pixel = 0;
	if (fgc) {
		if (font_options & OPT_FONT_FGCOL) {
			c.red = fgcolor.red;
			c.blue = fgcolor.blue;
			c.green = fgcolor.green;
		} else {
			c.red = 0;
			c.blue = 0;
			c.green = 0;
		}
	} else {
		if (font_options & OPT_FONT_BGCOL) {
			c.red = bgcolor.red;
			c.blue = bgcolor.blue;
			c.green = bgcolor.green;
		} else {
			c.red = 0xffff;
			c.blue = 0xffff;
			c.green = 0xffff;
		}
	}

	style = gtk_style_new();
	style->bg[0] = c;

	swid = gtk_event_box_new();
	gtk_widget_set_style(GTK_WIDGET(swid), style);
	g_object_unref(style);
	gtk_widget_set_size_request(GTK_WIDGET(swid), 40, -1);
	gtk_box_pack_start(GTK_BOX(box), swid, FALSE, FALSE, 5);
	gtk_widget_show(swid);
	return swid;
}

void apply_font_dlg(GtkWidget *w, GtkWidget *f)
{
	int i = 0;
	char *fontname;

	fontname = g_strdup(gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(f)));
	destroy_fontsel(0, 0);

	while(fontname[i] && !isdigit(fontname[i]) && i < sizeof(fontface)) {
		fontface[i] = fontname[i];
		i++;
	}

	fontface[i] = 0;
	g_free(fontname);

	gaim_conversation_foreach(gaim_gtkconv_update_font_face);
}

void
gaim_gtk_prefs_init(void)
{
	gaim_prefs_add_none("/gaim");
	gaim_prefs_add_none("/gaim/gtk");

	/* Debug window preferences. */
	gaim_prefs_add_none("/gaim/gtk/debug");
	gaim_prefs_add_bool("/gaim/gtk/debug/toolbar", TRUE);
	gaim_prefs_add_bool("/gaim/gtk/debug/timestamps", FALSE);
	gaim_prefs_add_bool("/gaim/gtk/debug/enabled", FALSE);
	gaim_prefs_add_int("/gaim/gtk/debug/width",  400);
	gaim_prefs_add_int("/gaim/gtk/debug/height", 150);
}

