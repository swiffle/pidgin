/*
 * gaim
 *
 * Copyright (C) 1998-1999, Mark Spencer <markster@marko.net>
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
#ifdef GAIM_PLUGINS
#ifndef _WIN32
#include <dlfcn.h>
#endif
#endif /* GAIM_PLUGINS */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#include <gdk/gdkwin32.h>
#else
#include <unistd.h>
#include <gdk/gdkx.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "prpl.h"
#include "sound.h"
#include "gaim.h"
#include "gtklist.h"
#include "gtkft.h"

#ifdef _WIN32
#include "win32dep.h"
#endif

static struct gaim_gtk_buddy_list *gtkblist = NULL;

/* part of the best damn Docklet code this side of Tahiti */
static gboolean gaim_gtk_blist_obscured = FALSE;

static void gaim_gtk_blist_selection_changed(GtkTreeSelection *selection, gpointer data);
static void gaim_gtk_blist_update(struct gaim_buddy_list *list, GaimBlistNode *node);
static char *gaim_get_tooltip_text(struct buddy *b);
static GdkPixbuf *gaim_gtk_blist_get_status_icon(struct buddy *b, GaimStatusIconSize size);
static char *item_factory_translate_func (const char *path, gpointer func_data);

/***************************************************
 *              Callbacks                          *
 ***************************************************/

static gboolean gtk_blist_delete_cb(GtkWidget *w, GdkEventAny *event, gpointer data)
{
	if (docklet_count)
		gaim_blist_set_visible(FALSE);
	else
		do_quit();

	/* we handle everything, event should not propogate further */
	return TRUE;
}

static gboolean gtk_blist_save_prefs_cb(gpointer data)
{
	save_prefs();

	/* only run once */
	return FALSE;
}

static gboolean gtk_blist_configure_cb(GtkWidget *w, GdkEventConfigure *event, gpointer data)
{
	/* unfortunately GdkEventConfigure ignores the window gravity, but  *
	 * the only way we have of setting the position doesn't. we have to *
	 * call get_position and get_size because they do pay attention to  *
	 * the gravity. this is inefficient and I agree it sucks, but it's  *
	 * more likely to work correctly.                        - Robot101 */
	gint x, y;

	/* check for visibility because when we aren't visible, this will   *
	 * give us bogus (0,0) coordinates.                      - xOr      */
	if (GTK_WIDGET_VISIBLE(w)) {
		gtk_window_get_position(GTK_WINDOW(w), &x, &y);

		if (x != blist_pos.x ||
		    y != blist_pos.y ||
		    event->width != blist_pos.width ||
		    event->height != blist_pos.height) {
			blist_pos.x = x;
			blist_pos.y = y;
			blist_pos.width = event->width;
			blist_pos.height = event->height;

			if (!g_main_context_find_source_by_user_data(NULL, &gtk_blist_save_prefs_cb)) {
				g_timeout_add(5000, gtk_blist_save_prefs_cb, &gtk_blist_save_prefs_cb);
			}
		}
	}

	/* continue to handle event normally */
	return FALSE;
}

static gboolean gtk_blist_visibility_cb(GtkWidget *w, GdkEventVisibility *event, gpointer data)
{
	if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
		gaim_gtk_blist_obscured = TRUE;
	else
		gaim_gtk_blist_obscured = FALSE;

	/* continue to handle event normally */
	return FALSE;
}

static void gtk_blist_menu_info_cb(GtkWidget *w, struct buddy *b)
{
	serv_get_info(b->account->gc, b->name);
}

static void gtk_blist_menu_im_cb(GtkWidget *w, struct buddy *b)
{
       gaim_conversation_new(GAIM_CONV_IM, b->account, b->name);
}

static void gtk_blist_menu_alias_cb(GtkWidget *w, struct buddy *b)
{
       alias_dialog_bud(b);
}

static void gtk_blist_menu_bp_cb(GtkWidget *w, struct buddy *b)
{
       show_new_bp(b->name, b->account->gc, b->idle,
                               b->uc & UC_UNAVAILABLE, NULL);
}

static void gtk_blist_menu_showlog_cb(GtkWidget *w, struct buddy *b)
{
       show_log(b->name);
}

static void gtk_blist_show_systemlog_cb()
{
       show_log(NULL);
}

static void gtk_blist_show_onlinehelp_cb()
{
       open_url(NULL, WEBSITE "documentation.php");
}

static void gtk_blist_button_im_cb(GtkWidget *w, GtkTreeView *tv)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(tv);
	GtkTreeSelection *sel = gtk_tree_view_get_selection(tv);

	if(gtk_tree_selection_get_selected(sel, &model, &iter)){
		GaimBlistNode *node;

		gtk_tree_model_get(GTK_TREE_MODEL(gtkblist->treemodel), &iter, NODE_COLUMN, &node, -1);
		if (GAIM_BLIST_NODE_IS_BUDDY(node)) 
			gaim_conversation_new(GAIM_CONV_IM, ((struct buddy*)node)->account, ((struct buddy*)node)->name);
	}
}

static void gtk_blist_button_info_cb(GtkWidget *w, GtkTreeView *tv)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(tv);
	GtkTreeSelection *sel = gtk_tree_view_get_selection(tv);

	if(gtk_tree_selection_get_selected(sel, &model, &iter)){
		GaimBlistNode *node;

		gtk_tree_model_get(GTK_TREE_MODEL(gtkblist->treemodel), &iter, NODE_COLUMN, &node, -1);
		if (GAIM_BLIST_NODE_IS_BUDDY(node)) {
			serv_get_info(((struct buddy*)node)->account->gc, ((struct buddy*)node)->name);
			return;
		}
	}
	show_info_dialog();
}

static void gtk_blist_button_chat_cb(GtkWidget *w, gpointer data)
{
	/* FIXME: someday, we can check to see if we've selected a chat node */
	join_chat();
}

static void gtk_blist_button_away_cb(GtkWidget *w, gpointer data)
{
	gtk_menu_popup(GTK_MENU(awaymenu), NULL, NULL, NULL, NULL, 1, GDK_CURRENT_TIME);
}

static void gtk_blist_row_activated_cb(GtkTreeView *tv, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data) {
	GaimBlistNode *node;
	GtkTreeIter iter;
	GValue val = { 0, };

	gtk_tree_model_get_iter(GTK_TREE_MODEL(gtkblist->treemodel), &iter, path);

	gtk_tree_model_get_value (GTK_TREE_MODEL(gtkblist->treemodel), &iter, NODE_COLUMN, &val);
	node = g_value_get_pointer(&val);

	if (GAIM_BLIST_NODE_IS_BUDDY(node)) {
		struct gaim_conversation *conv =
			gaim_conversation_new(GAIM_CONV_IM, ((struct buddy*)node)->account, ((struct buddy*)node)->name);
		if(conv) {
			gaim_window_raise(gaim_conversation_get_window(conv));
			gaim_window_switch_conversation(
				gaim_conversation_get_window(conv),
				gaim_conversation_get_index(conv));
		}
	} else if (GAIM_BLIST_NODE_IS_GROUP(node)) {
		if (gtk_tree_view_row_expanded(tv, path))
			gtk_tree_view_collapse_row(tv, path);
		else
			gtk_tree_view_expand_row(tv,path,FALSE);
	}
}

static void gaim_gtk_blist_add_buddy_cb()
{
	GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(gtkblist->treeview));
	GtkTreeIter iter;
	GaimBlistNode *node;

	if(gtk_tree_selection_get_selected(sel, NULL, &iter)){
		gtk_tree_model_get(GTK_TREE_MODEL(gtkblist->treemodel), &iter, NODE_COLUMN, &node, -1);
		if (GAIM_BLIST_NODE_IS_BUDDY(node)) 
			show_add_buddy(NULL, NULL, ((struct group*)node->parent)->name, NULL);
		else if (GAIM_BLIST_NODE_IS_GROUP(node))
			show_add_buddy(NULL, NULL, ((struct group*)node)->name, NULL);
	}
	else {
		show_add_buddy(NULL, NULL, NULL, NULL);
	}
}
static void
gaim_gtk_blist_remove_cb (GtkWidget *w, GaimBlistNode *node) 
{
	if (GAIM_BLIST_NODE_IS_BUDDY(node)) {
		struct buddy *b = (struct buddy*)node;
		show_confirm_del(b->account->gc, b->name);
	} else if (GAIM_BLIST_NODE_IS_GROUP(node)) {
		struct group *g = (struct group*)node;
		show_confirm_del_group(g);
	}
}

static void gaim_proto_menu_cb(GtkMenuItem *item, struct buddy *b)
{
	struct proto_buddy_menu *pbm = g_object_get_data(G_OBJECT(item), "gaimcallback");
	if (pbm->callback)
		pbm->callback(pbm->gc, b->name);
}

static gboolean gtk_blist_button_press_cb(GtkWidget *tv, GdkEventButton *event, gpointer null)
{
	GtkTreePath *path;
	GaimBlistNode *node;
	GValue val = { 0, };
	GtkTreeIter iter;
	GtkWidget *menu, *menuitem;
	GtkWidget *image;
	GtkTreeSelection *sel;
	GList *list;
	struct prpl *prpl;

	if (event->button != 3)
		return FALSE;

	/* Here we figure out which node was clicked */
	if (!gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(tv), event->x, event->y, &path, NULL, NULL, NULL))
		return FALSE;
	gtk_tree_model_get_iter(GTK_TREE_MODEL(gtkblist->treemodel), &iter, path);
	gtk_tree_model_get_value (GTK_TREE_MODEL(gtkblist->treemodel), &iter, NODE_COLUMN, &val);
	node = g_value_get_pointer(&val);
	menu = gtk_menu_new();

	if (GAIM_BLIST_NODE_IS_GROUP(node)) {
		menuitem = gtk_image_menu_item_new_with_mnemonic(_("_Add a Buddy"));
		g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(gaim_gtk_blist_add_buddy_cb), node);
		image = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_MENU);
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
		
		menuitem = gtk_image_menu_item_new_with_mnemonic(_("_Delete Group"));
		g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(gaim_gtk_blist_remove_cb), node);
		image = gtk_image_new_from_stock(GTK_STOCK_REMOVE, GTK_ICON_SIZE_MENU);
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

		menuitem = gtk_image_menu_item_new_with_mnemonic(_("_Rename"));
		g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(show_rename_group), node);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
	} else if (GAIM_BLIST_NODE_IS_BUDDY(node)) {
		/* Protocol specific options */
		prpl = find_prpl(((struct buddy*)node)->account->protocol);

		if(prpl && prpl->get_info) {
			menuitem = gtk_image_menu_item_new_with_mnemonic(_("_Get Info"));
			g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(gtk_blist_menu_info_cb), node);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
		}

		menuitem = gtk_image_menu_item_new_with_mnemonic(_("_IM"));
		g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(gtk_blist_menu_im_cb), node);
		image = gtk_image_new_from_stock(GAIM_STOCK_IM, GTK_ICON_SIZE_MENU);
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

		menuitem = gtk_image_menu_item_new_with_mnemonic(_("Add Buddy _Pounce"));
		g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(gtk_blist_menu_bp_cb), node);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

		menuitem = gtk_image_menu_item_new_with_mnemonic(_("View _Log"));
		g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(gtk_blist_menu_showlog_cb), node);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

		if (prpl) {
			list = prpl->buddy_menu(((struct buddy*)node)->account->gc, ((struct buddy*)node)->name);
			while (list) {
				struct proto_buddy_menu *pbm = list->data;
				menuitem = gtk_menu_item_new_with_mnemonic(pbm->label);
				g_object_set_data(G_OBJECT(menuitem), "gaimcallback", pbm);
				g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(gaim_proto_menu_cb), node);
				gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
				list = list->next;
			}
		}
	
		gaim_separator(menu);
	
		menuitem = gtk_image_menu_item_new_with_mnemonic(_("_Alias"));
		g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(gtk_blist_menu_alias_cb), node);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

		menuitem = gtk_image_menu_item_new_with_mnemonic(_("_Remove"));
		g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(gaim_gtk_blist_remove_cb), node);
		image = gtk_image_new_from_stock(GTK_STOCK_REMOVE, GTK_ICON_SIZE_MENU);
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuitem), image);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
	}
	
	gtk_widget_show_all(menu);
	
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 3, event->time);

#if (1)  /* This code only exists because GTK doesn't work.  If we return FALSE here, as would be normal
	  * the event propoagates down and somehow gets interpreted as the start of a drag event. */
	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tv));
	gtk_tree_selection_select_path(sel, path);
	gtk_tree_path_free(path);
	return TRUE;
#endif
}

static void gaim_gtk_blist_edit_mode_cb(gpointer callback_data, guint callback_action,
		GtkWidget *checkitem) {
	GdkCursor *cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(gtkblist->window->window, cursor);
	while (gtk_events_pending())
		gtk_main_iteration();
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(checkitem)))
		blist_options |= OPT_BLIST_SHOW_OFFLINE;
	else
		blist_options &= ~OPT_BLIST_SHOW_OFFLINE;
	save_prefs();
	gdk_cursor_unref(cursor);
	cursor = gdk_cursor_new(GDK_LEFT_PTR);
	gdk_window_set_cursor(gtkblist->window->window, cursor);
	gdk_cursor_unref(cursor);
	gaim_gtk_blist_refresh(gaim_get_blist());
}

/* This is called 10 seconds after the buddy logs in.  It removes the "logged in" icon and replaces it with
 * the normal status icon. Make sure they didn't sign off in the mean-time though. */

static gboolean gaim_reset_present_icon (GaimBlistNode *b)
{
	if (((struct buddy *)b)->present == 2) {
		((struct buddy *)b)->present = 1;
		gaim_gtk_blist_update(NULL, b);
	}

	return FALSE;
}

static void gaim_gtk_blist_drag_data_get_cb (GtkWidget *widget,
					     GdkDragContext *dc,
					     GtkSelectionData *data,
					     guint info,
					     guint time,
					     gpointer *null)
{
	if (data->target == gdk_atom_intern("GAIM_BLIST_NODE", FALSE)) {
		GtkTreeRowReference *ref = g_object_get_data(G_OBJECT(dc), "gtk-tree-view-source-row");
		GtkTreePath *sourcerow = gtk_tree_row_reference_get_path(ref);
		GtkTreeIter iter;
		GaimBlistNode *node = NULL;
		GValue val = {0};
		gtk_tree_model_get_iter(GTK_TREE_MODEL(gtkblist->treemodel), &iter, sourcerow);
		gtk_tree_model_get_value (GTK_TREE_MODEL(gtkblist->treemodel), &iter, NODE_COLUMN, &val);
		node = g_value_get_pointer(&val);
		gtk_selection_data_set (data,
					gdk_atom_intern ("GAIM_BLIST_NODE", FALSE),
					8, /* bits */
					(void*)&node,
					sizeof (node));
		
		gtk_tree_path_free(sourcerow);
	}
	
}

static void gaim_gtk_blist_drag_data_rcv_cb(GtkWidget *widget, GdkDragContext *dc, guint x, guint y,
			  GtkSelectionData *sd, guint info, guint t)
{	
	if (sd->target == gdk_atom_intern("GAIM_BLIST_NODE", FALSE) && sd->data) {
		GaimBlistNode *n = NULL;
		GtkTreePath *path = NULL;
		GtkTreeViewDropPosition position;
		memcpy(&n, sd->data, sizeof(n));
		if(gtk_tree_view_get_dest_row_at_pos(GTK_TREE_VIEW(widget), x, y, &path, &position)) {
			/* if we're here, I think it means the drop is ok */
			GtkTreeIter iter;
			GaimBlistNode *node;
			GValue val = {0};
			gtk_tree_model_get_iter(GTK_TREE_MODEL(gtkblist->treemodel), &iter, path);
			gtk_tree_model_get_value (GTK_TREE_MODEL(gtkblist->treemodel), &iter, NODE_COLUMN, &val);
			node = g_value_get_pointer(&val);

			if (GAIM_BLIST_NODE_IS_BUDDY(n)) {
				struct buddy *b = (struct buddy*)n;
				if (GAIM_BLIST_NODE_IS_BUDDY(node)) {
					switch(position) {
						case GTK_TREE_VIEW_DROP_AFTER:
						case GTK_TREE_VIEW_DROP_INTO_OR_AFTER:
							gaim_blist_add_buddy(b, (struct group*)node->parent, node);
							break;
						case GTK_TREE_VIEW_DROP_BEFORE:
						case GTK_TREE_VIEW_DROP_INTO_OR_BEFORE:
							gaim_blist_add_buddy(b, (struct group*)node->parent, node->prev);
							break;
					}
				} else if (GAIM_BLIST_NODE_IS_GROUP(node)) {
					gaim_blist_add_buddy(b, (struct group*)node, NULL);
				}
			} else if (GAIM_BLIST_NODE_IS_GROUP(n)) {
				struct group *g = (struct group*)n;
				if (GAIM_BLIST_NODE_IS_GROUP(node)) {
					switch (position) {
					case GTK_TREE_VIEW_DROP_INTO_OR_AFTER:
					case GTK_TREE_VIEW_DROP_AFTER:
						gaim_blist_add_group(g, node);
						break;
					case GTK_TREE_VIEW_DROP_INTO_OR_BEFORE:
					case GTK_TREE_VIEW_DROP_BEFORE:
						gaim_blist_add_group(g, node->prev);
						break;
					}

				}

			}

			gtk_tree_path_free(path);
			gaim_blist_save();
		}
	}
}

static void gaim_gtk_blist_paint_tip(GtkWidget *widget, GdkEventExpose *event, struct buddy *b)
{
	GtkStyle *style;
	GdkPixbuf *pixbuf = gaim_gtk_blist_get_status_icon(b, GAIM_STATUS_ICON_LARGE);
	PangoLayout *layout;
	char *tooltiptext = gaim_get_tooltip_text(b);

	layout = gtk_widget_create_pango_layout (gtkblist->tipwindow, NULL);
	pango_layout_set_markup(layout, tooltiptext, strlen(tooltiptext));
	pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
	pango_layout_set_width(layout, 300000);
	style = gtkblist->tipwindow->style;

	gtk_paint_flat_box (style, gtkblist->tipwindow->window, GTK_STATE_NORMAL, GTK_SHADOW_OUT,
			    NULL, gtkblist->tipwindow, "tooltip", 0, 0, -1, -1);

#if GTK_CHECK_VERSION(2,2,0)
	gdk_draw_pixbuf(GDK_DRAWABLE(gtkblist->tipwindow->window), NULL, pixbuf,
			0, 0, 4, 4, -1 , -1, GDK_RGB_DITHER_NONE, 0, 0);
#else
	gdk_pixbuf_render_to_drawable(pixbuf, GDK_DRAWABLE(gtkblist->tipwindow->window), NULL, 0, 0, 4, 4, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
#endif

	gtk_paint_layout (style, gtkblist->tipwindow->window, GTK_STATE_NORMAL, TRUE,
			  NULL, gtkblist->tipwindow, "tooltip", 38, 4, layout);

	g_object_unref (pixbuf);
	g_object_unref (layout);
	g_free(tooltiptext);
	return;
}

static gboolean gaim_gtk_blist_tooltip_timeout(GtkWidget *tv)
{
	GtkTreePath *path;
	GtkTreeIter iter;
	GaimBlistNode *node;
	GValue val = {0};

	if (!gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(tv), gtkblist->rect.x, gtkblist->rect.y, &path, NULL, NULL, NULL))
		return FALSE;
	gtk_tree_model_get_iter(GTK_TREE_MODEL(gtkblist->treemodel), &iter, path);
	gtk_tree_model_get_value (GTK_TREE_MODEL(gtkblist->treemodel), &iter, NODE_COLUMN, &val);
	node = g_value_get_pointer(&val);
	
	if (GAIM_BLIST_NODE_IS_BUDDY(node)) {
		int scr_w,scr_h, w, h, x, y;
		PangoLayout *layout;
		struct buddy *buddy = (struct buddy*)node;
		char *tooltiptext = gaim_get_tooltip_text(buddy);
		gtkblist->tipwindow = gtk_window_new(GTK_WINDOW_POPUP);
		gtkblist->tipwindow->parent = tv;
		gtk_widget_set_app_paintable(gtkblist->tipwindow, TRUE);
		gtk_window_set_resizable(GTK_WINDOW(gtkblist->tipwindow), FALSE);
		gtk_widget_set_name(gtkblist->tipwindow, "gtk-tooltips");
		g_signal_connect(G_OBJECT(gtkblist->tipwindow), "expose_event", 
				 G_CALLBACK(gaim_gtk_blist_paint_tip), buddy);
		gtk_widget_ensure_style (gtkblist->tipwindow);

		layout = gtk_widget_create_pango_layout (gtkblist->tipwindow, NULL);
		pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
		pango_layout_set_width(layout, 300000);
		pango_layout_set_markup(layout, tooltiptext, strlen(tooltiptext));
		scr_w = gdk_screen_width();
		scr_h = gdk_screen_height();
		pango_layout_get_size (layout, &w, &h);
		w = PANGO_PIXELS(w) + 8;
		h = PANGO_PIXELS(h) + 8;

		/* 38 is the size of a large status icon plus 4 pixels padding on each side.
		   I should #define this or something */
		w = w + 38;
		h = MAX(h, 38);

		gdk_window_get_pointer(NULL, &x, &y, NULL);
		if (GTK_WIDGET_NO_WINDOW(gtkblist->window))
			y+=gtkblist->window->allocation.y;
	
		x -= ((w >> 1) + 4);
		
		if ((x + w) > scr_w)
			x -= (x + w) - scr_w;
		else if (x < 0)
			x = 0;

		if ((y + h + 4) > scr_h)
			y = y - h;
		else
			y = y + 6;
		g_object_unref (layout);
		g_free(tooltiptext);
		gtk_widget_set_size_request(gtkblist->tipwindow, w, h);
		gtk_window_move(GTK_WINDOW(gtkblist->tipwindow), x, y);
		gtk_widget_show(gtkblist->tipwindow);
	}

	gtk_tree_path_free(path);
	return FALSE;
}

static gboolean gaim_gtk_blist_motion_cb (GtkWidget *tv, GdkEventMotion *event, gpointer null)
{
	GtkTreePath *path;
	if (gtkblist->timeout) {
		if ((event->y > gtkblist->rect.y) && ((event->y - gtkblist->rect.height) < gtkblist->rect.y))
			return FALSE;
		/* We've left the cell.  Remove the timeout and create a new one below */
		if (gtkblist->tipwindow) {
			gtk_widget_destroy(gtkblist->tipwindow);
			gtkblist->tipwindow = NULL;
		}
		
		g_source_remove(gtkblist->timeout);
	}
	
	gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(tv), event->x, event->y, &path, NULL, NULL, NULL);
	gtk_tree_view_get_cell_area(GTK_TREE_VIEW(tv), path, NULL, &gtkblist->rect);
	if (path)
		gtk_tree_path_free(path);
	gtkblist->timeout = g_timeout_add(500, (GSourceFunc)gaim_gtk_blist_tooltip_timeout, tv);
	return FALSE;
}

static void gaim_gtk_blist_leave_cb (GtkWidget *w, GdkEventCrossing *e, gpointer n)
{
	if (gtkblist->timeout) {
		g_source_remove(gtkblist->timeout);
		gtkblist->timeout = 0;
	}
	if (gtkblist->tipwindow) {
		gtk_widget_destroy(gtkblist->tipwindow);
		gtkblist->tipwindow = NULL;
	}
}

/***************************************************
 *            Crap                                 *
 ***************************************************/
static GtkItemFactoryEntry blist_menu[] =
{
	/* Buddies menu */
	{ N_("/_Buddies"), NULL, NULL, 0, "<Branch>" },
		{ N_("/Buddies/New _Instant Message..."), "<CTL>I", show_im_dialog, 0,
	  "<StockItem>", GAIM_STOCK_IM },
	{ N_("/Buddies/Join a _Chat..."), "<CTL>C", join_chat, 0, 
	  "<StockItem>", GAIM_STOCK_CHAT },
	{ "/Buddies/sep1", NULL, NULL, 0, "<Separator>" },
	{ N_("/Buddies/Get _User Info..."), "<CTL>J", show_info_dialog, 0,
	  "<StockItem>", GAIM_STOCK_INFO },
	{ "/Buddies/sep2", NULL, NULL, 0, "<Separator>" },
	{ N_("/Buddies/_Show Offline Buddies"), NULL, gaim_gtk_blist_edit_mode_cb, 1, "<CheckItem>"},
	{ N_("/Buddies/_Add a Buddy..."), NULL, gaim_gtk_blist_add_buddy_cb, 0, "<StockItem>", GTK_STOCK_ADD }, 
	{ N_("/Buddies/Add a _Group..."), NULL, show_add_group, 0, NULL},
	{ "/Buddies/sep3", NULL, NULL, 0, "<Separator>" },
	{ N_("/Buddies/_Signoff"), "<CTL>D", signoff_all, 0, NULL },
	{ N_("/Buddies/_Quit"), "<CTL>Q", do_quit, 0,
	  "<StockItem>", GTK_STOCK_QUIT },

	/* Tools */ 
	{ N_("/_Tools"), NULL, NULL, 0, "<Branch>" },
	{ N_("/Tools/_Away"), NULL, NULL, 0, "<Branch>" },
	{ N_("/Tools/Buddy _Pounce"), NULL, NULL, 0, "<Branch>" },
	{ N_("/Tools/sep1"), NULL, NULL, 0, "<Separator>" },
	{ N_("/Tools/_File Transfers"), NULL, gaim_show_xfer_dialog, 0,
	  "<StockItem>", GTK_STOCK_REVERT_TO_SAVED },
	{ "/Tools/sep2", NULL, NULL, 0, "<Separator>" },
	{ N_("/Tools/P_rotocol Actions"), NULL, NULL, 0, "<Branch>" },
	{ N_("/Tools/View System _Log"), NULL, gtk_blist_show_systemlog_cb, 0, NULL },
	{ "/Tools/sep3", NULL, NULL, 0, "<Separator>" },
	{ N_("/Tools/A_ccounts"), "<CTL>A", account_editor, 0, NULL },
	{ N_("/Tools/Preferences"), "<CTL>P", show_prefs, 0,
	  "<StockItem>", GTK_STOCK_PREFERENCES },
	{ N_("/Tools/Pr_ivacy"), NULL, show_privacy_options, 0, NULL },

	/* Help */
	{ N_("/_Help"), NULL, NULL, 0, "<Branch>" },
	{ N_("/Help/Online _Help"), "F1", gtk_blist_show_onlinehelp_cb, 0,
	  "<StockItem>", GTK_STOCK_HELP },
	{ N_("/Help/_Debug Window"), NULL, toggle_debug, 0, NULL },
	{ N_("/Help/_About"), NULL, show_about, 0, NULL },

};

/*********************************************************
 * Private Utility functions                             *
 *********************************************************/

static char *gaim_get_tooltip_text(struct buddy *b)
{
	char *text = NULL;
	struct prpl* prpl = find_prpl(b->account->protocol);
	char *statustext = NULL;
	char *aliastext = NULL, *nicktext = NULL;
	char *warning = NULL, *idletime = NULL;

	if (prpl->tooltip_text) {
		const char *end;
		statustext = prpl->tooltip_text(b);

		if(statustext && !g_utf8_validate(statustext, -1, &end)) {
			char *new = g_strndup(statustext,
					g_utf8_pointer_to_offset(statustext, end));
			g_free(statustext);
			statustext = new;
		}
	}

	if (b->idle) {
		int ihrs, imin;
		time_t t;
		time(&t);
		ihrs = (t - b->idle) / 3600;
		imin = ((t - b->idle) / 60) % 60;
		if (ihrs)
			idletime = g_strdup_printf(_("%dh%02dm"), ihrs, imin);
		else
			idletime = g_strdup_printf(_("%dm"), imin);
	}

	if(b->alias && b->alias[0])
		aliastext = g_markup_escape_text(b->alias, -1);

	if(b->server_alias)
		nicktext = g_markup_escape_text(b->server_alias, -1);

	if (b->evil > 0)
		warning = g_strdup_printf(_("%d%%"), b->evil);

	text = g_strdup_printf("<span size='larger' weight='bold'>%s</span>"
			       "%s %s"  /* Alias */
			       "%s %s"  /* Nickname */
			       "%s %s"     /* Idle */
			       "%s %s"     /* Warning */
			       "%s"        /* Offline */
			       "%s%s"     /* Status */
				   "%s",
			       b->name,
			       aliastext ? _("\n<b>Alias:</b>") : "", aliastext ? aliastext : "",
			       nicktext ? _("\n<b>Nickname:</b>") : "", nicktext ? nicktext : "",
			       b->idle ? _("\n<b>Idle:</b>") : "", b->idle ? idletime : "",
			       b->evil ? _("\n<b>Warned:</b>") : "", b->evil ? warning : "",
			       !b->present ? _("\n<b>Status:</b> Offline") : "",
			       statustext ? "\n" : "", statustext ? statustext : "",
				   !g_ascii_strcasecmp(b->name, "robflynn") ? "\n<b>Description:</b> Spooky" : "");
	
	if(warning)
		g_free(warning);
	if(idletime)
		g_free(idletime);
	if(statustext)
		g_free(statustext);
	if(nicktext)
		g_free(nicktext);
	if(aliastext)
		g_free(aliastext);

	return text;

}

static GdkPixbuf *gaim_gtk_blist_get_status_icon(struct buddy *b, GaimStatusIconSize size) 
{
	GdkPixbuf *status = NULL;
	GdkPixbuf *scale = NULL;
	GdkPixbuf *emblem = NULL;
	gchar *filename = NULL;
	const char *protoname = NULL;

	char *se = NULL, *sw = NULL ,*nw = NULL ,*ne = NULL;

	int scalesize = 30;

	struct prpl* prpl = find_prpl(b->account->protocol);

	if (!prpl) 
		return NULL;

	if (prpl->list_icon)
		protoname = prpl->list_icon(b->account, b);
	if (prpl->list_emblems)
		prpl->list_emblems(b, &se, &sw, &nw, &ne);
	
	if (size == GAIM_STATUS_ICON_SMALL) {
		scalesize = 15;
		sw = nw = ne = NULL; /* So that only the se icon will composite */
	}


	if (b->present == 2) {
		struct gaim_gtk_blist_node *gtknode;
		/* If b->present is 2, that means this buddy has just signed on.  We use the "login" icon for the
		 * status, and we set a timeout to change it to a normal icon after 10 seconds. */
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", "login.png", NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);

		gtknode = GAIM_GTK_BLIST_NODE((GaimBlistNode*)b);
		if (gtknode->timer > 0)
			g_source_remove(gtknode->timer);
		gtknode->timer = g_timeout_add(10000, (GSourceFunc)gaim_reset_present_icon, b);

		/* "Hey, what's all this crap?" you ask.  Status icons will be themeable too, and
		   then it will look up protoname from the theme */
	} else {
		char *image = g_strdup_printf("%s.png", protoname);
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", image, NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(image);
		g_free(filename);

	}

	if (!status)
		return NULL;

	scale =  gdk_pixbuf_scale_simple(status, scalesize, scalesize, GDK_INTERP_BILINEAR);

	g_object_unref(G_OBJECT(status));

	/* Emblems */

	/* Each protocol can specify up to four "emblems" to composite over the base icon.  "away", "busy", "mobile user"
	 * are all examples of states represented by emblems.  I'm not even really sure I like this yet. */

	/* XXX Clean this crap up, yo. */
	if (se) {
		char *image = g_strdup_printf("%s.png", se);
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", image, NULL);
		g_free(image);
		emblem = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
		if (emblem) {
			if (size == GAIM_STATUS_ICON_LARGE)
				gdk_pixbuf_composite (emblem,
						      scale, 15, 15,
						      15, 15,
						      15, 15,
						      1, 1,
						      GDK_INTERP_BILINEAR,
						      255);
			else
					gdk_pixbuf_composite (emblem,
						      scale, 0, 0,
						      15, 15,
						      0, 0,
						      1, 1,
						      GDK_INTERP_BILINEAR,
						      255);
			g_object_unref(G_OBJECT(emblem));
		}
	}
	if (sw) {
		char *image = g_strdup_printf("%s.png", sw);
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", image, NULL);
		g_free(image);
		emblem = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
		if (emblem) {
			gdk_pixbuf_composite (emblem,
					scale, 0, 15,
					15, 15,
					0, 15,
					1, 1,
					GDK_INTERP_BILINEAR,
					255);
			g_object_unref(G_OBJECT(emblem));
		}
	}
	if (nw) {
		char *image = g_strdup_printf("%s.png", nw);
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", image, NULL);
		g_free(image);
		emblem = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
		if (emblem) {
			gdk_pixbuf_composite (emblem,
					      scale, 0, 0,
					      15, 15,
					      0, 0,
					      1, 1,
					      GDK_INTERP_BILINEAR,
					      255);
			g_object_unref(G_OBJECT(emblem));
		}
	}
	if (ne) {
		char *image = g_strdup_printf("%s.png", ne);
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", image, NULL);
		g_free(image);
		emblem = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
		if (emblem) {
			gdk_pixbuf_composite (emblem,
					      scale, 15, 0,
					      15, 15,
					      15, 0,
					      1, 1,
					      GDK_INTERP_BILINEAR,
					      255);
			g_object_unref(G_OBJECT(emblem));
		}
	}


	/* Idle grey buddies affects the whole row.  This converts the status icon to greyscale. */
	if (!b->present)
		gdk_pixbuf_saturate_and_pixelate(scale, scale, 0.0, FALSE);
	else if (b->idle && blist_options & OPT_BLIST_GREY_IDLERS)
		gdk_pixbuf_saturate_and_pixelate(scale, scale, 0.25, FALSE);
	return scale;
}

static GdkPixbuf *gaim_gtk_blist_get_buddy_icon(struct buddy *b)
{
	/* This just opens a file from ~/.gaim/icons/screenname.  This needs to change to be more gooder. */
	char *file;
	GdkPixbuf *buf, *ret;

	if (!(blist_options & OPT_BLIST_SHOW_ICONS))
		return NULL;

	if ((file = gaim_buddy_get_setting(b, "buddy_icon")) == NULL)
		return NULL;

	buf = gdk_pixbuf_new_from_file(file, NULL);
	g_free(file);


	if (buf) {
		if (!b->present)
			gdk_pixbuf_saturate_and_pixelate(buf, buf, 0.0, FALSE);
		if (b->idle && blist_options & OPT_BLIST_GREY_IDLERS)
			gdk_pixbuf_saturate_and_pixelate(buf, buf, 0.25, FALSE);

		ret = gdk_pixbuf_scale_simple(buf,30,30, GDK_INTERP_BILINEAR);
		g_object_unref(G_OBJECT(buf));
		return ret;
	}
	return NULL;
}

static gchar *gaim_gtk_blist_get_name_markup(struct buddy *b, gboolean selected)
{
	char *name = gaim_get_buddy_alias(b);
	char *esc = g_markup_escape_text(name, strlen(name)), *text = NULL;
	struct prpl* prpl = find_prpl(b->account->protocol);

	/* XXX Clean up this crap */

	int ihrs, imin;
	char *idletime = NULL, *warning = NULL, *statustext = NULL;
	time_t t;

	if (!(blist_options & OPT_BLIST_SHOW_ICONS)) {
		if ((b->idle > 0 && blist_options & OPT_BLIST_GREY_IDLERS && !selected) || b->present == 0) {
			text =  g_strdup_printf("<span color='dim grey'>%s</span>",
						esc);
			g_free(esc);
			return text;
		} else {
			return esc;
		}
	}

	time(&t);
	ihrs = (t - b->idle) / 3600;
	imin = ((t - b->idle) / 60) % 60;

	if (prpl && prpl->status_text) {
		char *tmp = prpl->status_text(b);
		const char *end;

		if(tmp && !g_utf8_validate(tmp, -1, &end)) {
			char *new = g_strndup(tmp,
					g_utf8_pointer_to_offset(tmp, end));
			g_free(tmp);
			tmp = new;
		}

		if(tmp) {
			char buf[32];
			char *c = tmp;
			int length = 0, vis=0;
			gboolean inside = FALSE;
			g_strdelimit(tmp, "\n", ' ');

			while(*c && vis < 20) {
				if(*c == '&')
					inside = TRUE;
				else if(*c == ';')
					inside = FALSE;
				if(!inside)
					vis++;
				length++;
				c++; /* this is fun */
			}

			if(vis == 20)
				g_snprintf(buf, sizeof(buf), "%%.%ds...", length);
			else
				g_snprintf(buf, sizeof(buf), "%%s ");

			statustext = g_strdup_printf(buf, tmp);

			g_free(tmp);
		}
	}

	if (b->idle) {
		if (ihrs)
			idletime = g_strdup_printf(_("Idle (%dh%02dm) "), ihrs, imin);
		else
			idletime = g_strdup_printf(_("Idle (%dm) "), imin);
	}

	if (b->evil > 0)
		warning = g_strdup_printf(_("Warned (%d%%) "), b->evil);

	if (b->idle && blist_options & OPT_BLIST_GREY_IDLERS && !selected) {
		text =  g_strdup_printf("<span color='dim grey'>%s</span>\n"
					"<span color='dim grey' size='smaller'>%s%s%s%s</span>",
					esc,
					statustext != NULL ? statustext : "",
					idletime != NULL ? idletime : "", 
					warning != NULL ? warning : "",
					!b->present ? _("Offline ") : "");
	} else if (statustext == NULL && idletime == NULL && warning == NULL && b->present) {
		text = g_strdup(esc);
	} else {
		text = g_strdup_printf("%s\n"
				       "<span %s size='smaller'>%s%s%s%s</span>", esc,
				       selected ? "" : "color='dim grey'",
				       statustext != NULL ? statustext :  "",
				       idletime != NULL ? idletime : "", 
				       warning != NULL ? warning : "",
				       !b->present ? _("Offline ") : "");
	}
	if (idletime)
		g_free(idletime);
	if (warning)
		g_free(warning);
	if (statustext)
		g_free(statustext);
	if (esc)
		g_free(esc);

	return text;
}

static void gaim_gtk_blist_restore_position()
{
	/* if the window exists, is hidden, we're saving positions, and the position is sane... */
	if(gtkblist && gtkblist->window &&
	   !GTK_WIDGET_VISIBLE(gtkblist->window) &&
	   blist_options & OPT_BLIST_SAVED_WINDOWS &&
	   blist_pos.width != 0) {
		/* ...check position is on screen... */
		if (blist_pos.x >= gdk_screen_width())
			blist_pos.x = gdk_screen_width() - 100;
		else if (blist_pos.x < 0)
			blist_pos.x = 100;
		
		if (blist_pos.y >= gdk_screen_height())
			blist_pos.y = gdk_screen_height() - 100;
		else if (blist_pos.y < 0)
			blist_pos.y = 100;
		
		/* ...and move it back. */
		gtk_window_move(GTK_WINDOW(gtkblist->window), blist_pos.x, blist_pos.y);
		gtk_window_resize(GTK_WINDOW(gtkblist->window), blist_pos.width, blist_pos.height);
	}
}

static gboolean gaim_gtk_blist_refresh_timer(struct gaim_buddy_list *list)
{
	GaimBlistNode *group = list->root;
	GaimBlistNode *buddy;

	while (group) {
		buddy = group->child;
		while (buddy) {
			if (((struct buddy *)buddy)->idle)
				gaim_gtk_blist_update(list, buddy);
			buddy = buddy->next;
		}
		group = group->next;
	}

	/* keep on going */
	return TRUE;
}

/**********************************************************************************
 * Public API Functions                                                           *
 **********************************************************************************/
static void gaim_gtk_blist_new_list(struct gaim_buddy_list *blist)
{
	blist->ui_data = g_new0(struct gaim_gtk_buddy_list, 1);
}

static void gaim_gtk_blist_new_node(GaimBlistNode *node)
{
	node->ui_data = g_new0(struct gaim_gtk_blist_node, 1);
}

void gaim_gtk_blist_update_columns()
{
	if (blist_options & OPT_BLIST_SHOW_ICONS) {
		gtk_tree_view_column_set_visible(gtkblist->buddy_icon_column, TRUE);
		gtk_tree_view_column_set_visible(gtkblist->idle_column, FALSE);
		gtk_tree_view_column_set_visible(gtkblist->warning_column, FALSE);
	} else {
		gtk_tree_view_column_set_visible(gtkblist->idle_column, blist_options & OPT_BLIST_SHOW_IDLETIME);
		gtk_tree_view_column_set_visible(gtkblist->warning_column, blist_options & OPT_BLIST_SHOW_WARN);
		gtk_tree_view_column_set_visible(gtkblist->buddy_icon_column, FALSE);
	}
}

enum {DRAG_BUDDY, DRAG_ROW};

static char *
item_factory_translate_func (const char *path, gpointer func_data)
{
	return _(path);
}

static void gaim_gtk_blist_show(struct gaim_buddy_list *list)
{
	GtkItemFactory *ift;
	GtkCellRenderer *rend;
	GtkTreeViewColumn *column;
	GtkWidget *sw;
	GtkWidget *button;
	GtkTooltips *tooltips;
	GtkSizeGroup *sg;
	GtkTreeSelection *selection;
	GtkTargetEntry gte[] = {{"GAIM_BLIST_NODE", GTK_TARGET_SAME_APP, DRAG_ROW},
				{"application/x-im-contact", 0, DRAG_BUDDY}};

	if (gtkblist && gtkblist->window) {
		gtk_widget_show(gtkblist->window);
		return;
	}

	gtkblist = GAIM_GTK_BLIST(list);

	gtkblist->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_role(GTK_WINDOW(gtkblist->window), "buddy_list");
	gtk_window_set_title(GTK_WINDOW(gtkblist->window), _("Buddy List"));

	gtkblist->vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(gtkblist->window), gtkblist->vbox);

	g_signal_connect(G_OBJECT(gtkblist->window), "delete_event", G_CALLBACK(gtk_blist_delete_cb), NULL);
	g_signal_connect(G_OBJECT(gtkblist->window), "configure_event", G_CALLBACK(gtk_blist_configure_cb), NULL);
	g_signal_connect(G_OBJECT(gtkblist->window), "visibility_notify_event", G_CALLBACK(gtk_blist_visibility_cb), NULL);
	gtk_widget_add_events(gtkblist->window, GDK_VISIBILITY_NOTIFY_MASK);

	/******************************* Menu bar *************************************/
	ift = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<GaimMain>", NULL);
	gtk_item_factory_set_translate_func (ift,
					     item_factory_translate_func,
					     NULL, NULL);
	gtk_item_factory_create_items(ift, sizeof(blist_menu) / sizeof(*blist_menu),
				      blist_menu, NULL);
	gtk_box_pack_start(GTK_BOX(gtkblist->vbox), gtk_item_factory_get_widget(ift, "<GaimMain>"), FALSE, FALSE, 0);

	awaymenu = gtk_item_factory_get_widget(ift, N_("/Tools/Away"));
	do_away_menu();

	bpmenu = gtk_item_factory_get_widget(ift, N_("/Tools/Buddy Pounce"));
	do_bp_menu();

	protomenu = gtk_item_factory_get_widget(ift, N_("/Tools/Protocol Actions"));
	do_proto_menu();

	/****************************** GtkTreeView **********************************/
	sw = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	gtkblist->treemodel = gtk_tree_store_new(BLIST_COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING,
						 G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_POINTER);

	gtkblist->treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(gtkblist->treemodel));
	gtk_widget_set_size_request(gtkblist->treeview, -1, 200);

	/* Set up selection stuff */

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gtkblist->treeview));
	g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(gaim_gtk_blist_selection_changed), NULL);


	/* Set up dnd */
	gtk_tree_view_enable_model_drag_source(GTK_TREE_VIEW(gtkblist->treeview), GDK_BUTTON1_MASK, gte,
					       2, GDK_ACTION_COPY);
	gtk_tree_view_enable_model_drag_dest(GTK_TREE_VIEW(gtkblist->treeview), gte, 2,
					     GDK_ACTION_COPY | GDK_ACTION_MOVE);
	g_signal_connect(G_OBJECT(gtkblist->treeview), "drag-data-received", G_CALLBACK(gaim_gtk_blist_drag_data_rcv_cb), NULL);
	g_signal_connect(G_OBJECT(gtkblist->treeview), "drag-data-get", G_CALLBACK(gaim_gtk_blist_drag_data_get_cb), NULL);

	/* Tooltips */
	g_signal_connect(G_OBJECT(gtkblist->treeview), "motion-notify-event", G_CALLBACK(gaim_gtk_blist_motion_cb), NULL);
	g_signal_connect(G_OBJECT(gtkblist->treeview), "leave-notify-event", G_CALLBACK(gaim_gtk_blist_leave_cb), NULL);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(gtkblist->treeview), FALSE);

	rend = gtk_cell_renderer_pixbuf_new();
	column = gtk_tree_view_column_new_with_attributes("Status", rend, "pixbuf", STATUS_ICON_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(gtkblist->treeview), column);
	g_object_set(rend, "xalign", 0.0, "ypad", 0, NULL);

	rend = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Name", rend, "markup", NAME_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(gtkblist->treeview), column);
	g_object_set(rend, "ypad", 0, "yalign", 0.5, NULL);

	rend = gtk_cell_renderer_text_new();
	gtkblist->warning_column = gtk_tree_view_column_new_with_attributes("Warning", rend, "markup", WARNING_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(gtkblist->treeview), gtkblist->warning_column);
	g_object_set(rend, "xalign", 1.0, "ypad", 0, NULL);

	rend = gtk_cell_renderer_text_new();
	gtkblist->idle_column = gtk_tree_view_column_new_with_attributes("Idle", rend, "markup", IDLE_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(gtkblist->treeview), gtkblist->idle_column);
	g_object_set(rend, "xalign", 1.0, "ypad", 0, NULL);

	rend = gtk_cell_renderer_pixbuf_new();
	gtkblist->buddy_icon_column = gtk_tree_view_column_new_with_attributes("Buddy Icon", rend, "pixbuf", BUDDY_ICON_COLUMN, NULL);
	g_object_set(rend, "xalign", 1.0, "ypad", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(gtkblist->treeview), gtkblist->buddy_icon_column);

	g_signal_connect(G_OBJECT(gtkblist->treeview), "row-activated", G_CALLBACK(gtk_blist_row_activated_cb), NULL);
	g_signal_connect(G_OBJECT(gtkblist->treeview), "button-press-event", G_CALLBACK(gtk_blist_button_press_cb), NULL);

	gtk_box_pack_start(GTK_BOX(gtkblist->vbox), sw, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(sw), gtkblist->treeview);
	gaim_gtk_blist_update_columns();

	/* set the Show Offline Buddies option. must be done
	 * after the treeview or faceprint gets mad. -Robot101
	 */
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item (ift, N_("/Edit/Show Offline Buddies"))),
			blist_options & OPT_BLIST_SHOW_OFFLINE);

	/* OK... let's show this bad boy. */
	gaim_gtk_blist_refresh(list);
	gaim_gtk_blist_restore_position();
	gtk_widget_show_all(gtkblist->window);

	/**************************** Button Box **************************************/
	/* add this afterwards so it doesn't force up the width of the window         */

	tooltips = gtk_tooltips_new();

	sg = gtk_size_group_new(GTK_SIZE_GROUP_BOTH);
	gtkblist->bbox = gtk_hbox_new(TRUE, 0);
	gtk_box_pack_start(GTK_BOX(gtkblist->vbox), gtkblist->bbox, FALSE, FALSE, 0);
	gtk_widget_show(gtkblist->bbox);

	button = gaim_pixbuf_button_from_stock(_("IM"), GAIM_STOCK_IM, GAIM_BUTTON_VERTICAL);
	gtk_box_pack_start(GTK_BOX(gtkblist->bbox), button, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_size_group_add_widget(sg, button);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(gtk_blist_button_im_cb),
			 gtkblist->treeview);
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), button, _("Send an Instant Message to the selected buddy"), NULL);
	gtk_widget_show(button);

	button = gaim_pixbuf_button_from_stock(_("Get Info"), GAIM_STOCK_INFO, GAIM_BUTTON_VERTICAL);
	gtk_box_pack_start(GTK_BOX(gtkblist->bbox), button, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_size_group_add_widget(sg, button);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(gtk_blist_button_info_cb),
			 gtkblist->treeview);
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), button, _("Get information on the selected buddy"), NULL);
	gtk_widget_show(button);

	button = gaim_pixbuf_button_from_stock(_("Chat"), GAIM_STOCK_CHAT, GAIM_BUTTON_VERTICAL);
	gtk_box_pack_start(GTK_BOX(gtkblist->bbox), button, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_size_group_add_widget(sg, button);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(gtk_blist_button_chat_cb), NULL);
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), button, _("Join a chat room"), NULL);
	gtk_widget_show(button);

	button = gaim_pixbuf_button_from_stock(_("Away"), GAIM_STOCK_AWAY, GAIM_BUTTON_VERTICAL);
	gtk_box_pack_start(GTK_BOX(gtkblist->bbox), button, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_size_group_add_widget(sg, button);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(gtk_blist_button_away_cb), NULL);
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), button, _("Set an away message"), NULL);
	gtk_widget_show(button);

	/* we're done setting tooltips */
	gtk_object_sink(GTK_OBJECT(tooltips));

	/* this will show the right image/label widgets for us */
	gaim_gtk_blist_update_toolbar();

	/* start the refresh timer */
	gtkblist->refresh_timer = g_timeout_add(30000, (GSourceFunc)gaim_gtk_blist_refresh_timer, list);
}

void gaim_gtk_blist_refresh(struct gaim_buddy_list *list)
{
	GaimBlistNode *group = list->root;
	GaimBlistNode *buddy;

	while (group) {
		buddy = group->child;
		gaim_gtk_blist_update(list, group);
		while (buddy) {
			gaim_gtk_blist_update(list, buddy);
			buddy = buddy->next;
		}
		group = group->next;
	}
}

static gboolean get_iter_from_node_helper(GaimBlistNode *node, GtkTreeIter *iter, GtkTreeIter *root) {

	do {
		GaimBlistNode *n;
		GtkTreeIter child;

		gtk_tree_model_get(GTK_TREE_MODEL(gtkblist->treemodel), root, NODE_COLUMN, &n, -1);
		if(n == node) {
			*iter = *root;
			return TRUE;
		}

		if(gtk_tree_model_iter_children(GTK_TREE_MODEL(gtkblist->treemodel), &child, root)) {
			if(get_iter_from_node_helper(node,iter,&child))
				return TRUE;
		}
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(gtkblist->treemodel), root));

	return FALSE;
}

static gboolean get_iter_from_node(GaimBlistNode *node, GtkTreeIter *iter) {
	GtkTreeIter root;

	if (!gtkblist)
		return FALSE;

	if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(gtkblist->treemodel), &root))
		return FALSE;

	return get_iter_from_node_helper(node, iter, &root);
}

/*
 * These state assignments suck. I'm sorry. They're for historical reasons.
 * Roll on new prefs. -Robot101
 * 
 * NO_BUTTON_TEXT && SHOW_BUTTON_XPM - image
 * !NO_BUTTON_TEXT && !SHOW_BUTTON_XPM - text
 * !NO_BUTTON_TEXT && SHOW_BUTTON_XPM - text & images
 * NO_BUTTON_TEXT && !SHOW_BUTTON_XPM - none
 */

static void gaim_gtk_blist_update_toolbar_icons (GtkWidget *widget, gpointer data) {
	if (GTK_IS_IMAGE(widget)) {
		if (blist_options & OPT_BLIST_SHOW_BUTTON_XPM)
			gtk_widget_show(widget);
		else
			gtk_widget_hide(widget);
	} else if (GTK_IS_LABEL(widget)) {
		if (blist_options & OPT_BLIST_NO_BUTTON_TEXT)
			gtk_widget_hide(widget);
		else
			gtk_widget_show(widget);
	} else if (GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach(GTK_CONTAINER(widget), gaim_gtk_blist_update_toolbar_icons, NULL);
	}
}

void gaim_gtk_blist_update_toolbar() {
	if (!gtkblist)
		return;

	if (blist_options & OPT_BLIST_NO_BUTTON_TEXT && !(blist_options & OPT_BLIST_SHOW_BUTTON_XPM))
		gtk_widget_hide(gtkblist->bbox);
	else {
		gtk_container_foreach(GTK_CONTAINER(gtkblist->bbox), gaim_gtk_blist_update_toolbar_icons, NULL);
		gtk_widget_show(gtkblist->bbox);
	}
}

static void gaim_gtk_blist_remove(struct gaim_buddy_list *list, GaimBlistNode *node)
{
	struct gaim_gtk_blist_node *gtknode;
	GtkTreeIter iter;

	if (!node->ui_data)
		return;

	gtknode = (struct gaim_gtk_blist_node *)node->ui_data;

	if (gtknode->timer > 0) {
		g_source_remove(gtknode->timer);
		gtknode->timer = 0;
	}

	/* For some reason, we're called before we have a buddy list sometimes */
	if(!gtkblist)
		return;

	if(gtkblist->selected_node == node)
		gtkblist->selected_node = NULL;

	if (get_iter_from_node(node, &iter)) {
		gtk_tree_store_remove(gtkblist->treemodel, &iter);
		if(GAIM_BLIST_NODE_IS_BUDDY(node) &&
		   !(blist_options & OPT_BLIST_SHOW_OFFLINE) &&
		   gaim_blist_get_group_online_count((struct group *)node->parent) == 0) {
			GtkTreeIter groupiter;
			if(get_iter_from_node(node->parent, &groupiter))
				gtk_tree_store_remove(gtkblist->treemodel, &groupiter);
		}
	}
}

static gboolean do_selection_changed(GaimBlistNode *new_selection)
{
	GaimBlistNode *old_selection = gtkblist->selected_node;

	if(new_selection != gtkblist->selected_node) {
		gtkblist->selected_node = new_selection;
		if(new_selection)
			gaim_gtk_blist_update(NULL, new_selection);
		if(old_selection)
			gaim_gtk_blist_update(NULL, old_selection);
	}

	return FALSE;
}

static void gaim_gtk_blist_selection_changed(GtkTreeSelection *selection, gpointer data)
{
	GaimBlistNode *new_selection = NULL;
	GtkTreeIter iter;

	if(gtk_tree_selection_get_selected(selection, NULL, &iter)){
		gtk_tree_model_get(GTK_TREE_MODEL(gtkblist->treemodel), &iter,
				NODE_COLUMN, &new_selection, -1);
	}
	/* we set this up as a timeout, otherwise the blist flickers */
	g_timeout_add(0, (GSourceFunc)do_selection_changed, new_selection);
}

static void make_a_group(GaimBlistNode *node, GtkTreeIter *iter) {
	GaimBlistNode *sibling;
	GtkTreeIter siblingiter;
	GdkPixbuf *groupicon = gtk_widget_render_icon(gtkblist->treeview,
			GTK_STOCK_OPEN, GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
	struct group *group = (struct group *)node;
	char *esc = g_markup_escape_text(group->name, -1);
	char *mark;

	if(blist_options & OPT_BLIST_SHOW_GRPNUM)
		mark = g_strdup_printf("<span weight='bold'>%s</span> (%d/%d)", esc, gaim_blist_get_group_online_count(group), gaim_blist_get_group_size(group, FALSE));
	else
		mark = g_strdup_printf("<span weight='bold'>%s</span>", esc);

	g_free(esc);

	sibling = node->prev;
	while (sibling && !get_iter_from_node(sibling, &siblingiter)) {
		sibling = sibling->prev;
	}

	gtk_tree_store_insert_after(gtkblist->treemodel, iter, NULL,
			sibling ? &siblingiter : NULL);
	gtk_tree_store_set(gtkblist->treemodel, iter,
			STATUS_ICON_COLUMN, groupicon,
			NAME_COLUMN, mark,
			NODE_COLUMN, node,
			-1);
	g_free(mark);
	g_object_unref(groupicon);
}


static void gaim_gtk_blist_update(struct gaim_buddy_list *list, GaimBlistNode *node)
{
	struct gaim_gtk_blist_node *gtknode;
	GtkTreeIter iter;
	GtkTreePath *expand = NULL;
	gboolean new_entry = FALSE;

	if (!gtkblist)
		return;

	gtknode = GAIM_GTK_BLIST_NODE(node);


	if (!get_iter_from_node(node, &iter)) { /* This is a newly added node */
		new_entry = TRUE;
		if (GAIM_BLIST_NODE_IS_BUDDY(node)) {
			if (((struct buddy*)node)->present || (blist_options & OPT_BLIST_SHOW_OFFLINE && ((struct buddy*)node)->account->gc)) {
				GtkTreeIter groupiter;
				GaimBlistNode *oldersibling;
				GtkTreeIter oldersiblingiter;

				if(node->parent &&
						!get_iter_from_node(node->parent, &groupiter)) {
					/* This buddy's group has not yet been added.
					 * We do that here */
					make_a_group(node->parent, &groupiter);
					expand = gtk_tree_model_get_path(GTK_TREE_MODEL(gtkblist->treemodel), &groupiter);
				}

				oldersibling = node->prev;
				while (oldersibling && !get_iter_from_node(oldersibling, &oldersiblingiter)) {
					oldersibling = oldersibling->prev;
				}

				gtk_tree_store_insert_after(gtkblist->treemodel, &iter, &groupiter, oldersibling ? &oldersiblingiter : NULL);

				if (blist_options & OPT_BLIST_POPUP)
					gtk_window_present(GTK_WINDOW(gtkblist->window));

			}
		}
		else if (GAIM_BLIST_NODE_IS_GROUP(node) && (blist_options & OPT_BLIST_SHOW_OFFLINE)) {
			make_a_group(node, &iter);
			expand = gtk_tree_model_get_path(GTK_TREE_MODEL(gtkblist->treemodel), &iter);
		}
	} else if (GAIM_BLIST_NODE_IS_GROUP(node)) {
		if ((gaim_blist_get_group_online_count((struct group *)node) == 0) ||
				(!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(gtkblist->treemodel), &iter) && !(blist_options & OPT_BLIST_SHOW_OFFLINE))) {
			gtk_tree_store_remove(gtkblist->treemodel, &iter);
		} else {
			struct group *group = (struct group *)node;
			char *esc = g_markup_escape_text(group->name, -1);
			char *mark;

			if(blist_options & OPT_BLIST_SHOW_GRPNUM)
				mark = g_strdup_printf("<span weight='bold'>%s</span> (%d/%d)", esc, gaim_blist_get_group_online_count(group), gaim_blist_get_group_size(group, FALSE));
			else
				mark = g_strdup_printf("<span weight='bold'>%s</span>", esc);

			g_free(esc);
			gtk_tree_store_set(gtkblist->treemodel, &iter,
					NAME_COLUMN, mark,
					-1);
			g_free(mark);
		}
	}

	if (GAIM_BLIST_NODE_IS_BUDDY(node) && (((struct buddy*)node)->present || (blist_options & OPT_BLIST_SHOW_OFFLINE && ((struct buddy*)node)->account->gc))) {
		GdkPixbuf *status, *avatar;
		char *mark;
		char *warning = NULL, *idle = NULL;

		gboolean selected = (gtkblist->selected_node == node);

		status = gaim_gtk_blist_get_status_icon((struct buddy*)node,
							blist_options & OPT_BLIST_SHOW_ICONS ? GAIM_STATUS_ICON_LARGE : GAIM_STATUS_ICON_SMALL);
		avatar = gaim_gtk_blist_get_buddy_icon((struct buddy*)node);
		mark   = gaim_gtk_blist_get_name_markup((struct buddy*)node, selected);

		if (((struct buddy*)node)->idle > 0) {
			time_t t;
			int ihrs, imin;
			time(&t);
			ihrs = (t - ((struct buddy *)node)->idle) / 3600;
			imin = ((t - ((struct buddy*)node)->idle) / 60) % 60;
			if(ihrs > 0)
				idle = g_strdup_printf("(%d:%02d)", ihrs, imin);
			else
				idle = g_strdup_printf("(%d)", imin);
		}

		if (((struct buddy*)node)->evil > 0)
			warning = g_strdup_printf("%d%%", ((struct buddy*)node)->evil);


		if((blist_options & OPT_BLIST_GREY_IDLERS)
				&& ((struct buddy *)node)->idle) {
			if(warning && !selected) {
				char *w2 = g_strdup_printf("<span color='dim grey'>%s</span>",
						warning);
				g_free(warning);
				warning = w2;
			}

			if(idle && !selected) {
				char *i2 = g_strdup_printf("<span color='dim grey'>%s</span>",
						idle);
				g_free(idle);
				idle = i2;
			}
		}


		gtk_tree_store_set(gtkblist->treemodel, &iter,
				   STATUS_ICON_COLUMN, status,
				   NAME_COLUMN, mark,
				   WARNING_COLUMN, warning,
				   IDLE_COLUMN, idle,
				   BUDDY_ICON_COLUMN, avatar,
				   NODE_COLUMN, node,
				   -1);

		g_free(mark);
		if (idle)
			g_free(idle);
		if (warning)
			g_free(warning);

		if (status != NULL)
			g_object_unref(status);

		if (avatar != NULL)
			g_object_unref(avatar);

	} else if (GAIM_BLIST_NODE_IS_BUDDY(node) && !new_entry) {
		gaim_gtk_blist_remove(list, node);
		if (blist_options & OPT_BLIST_POPUP)
			gtk_window_present(GTK_WINDOW(gtkblist->window));
	}
	gtk_tree_view_columns_autosize(GTK_TREE_VIEW(gtkblist->treeview));


	if(expand) {
		gtk_tree_view_expand_row(GTK_TREE_VIEW(gtkblist->treeview), expand, TRUE);
		gtk_tree_path_free(expand);
	}

	if(GAIM_BLIST_NODE_IS_BUDDY(node))
		gaim_gtk_blist_update(list, node->parent);
}

static void gaim_gtk_blist_destroy(struct gaim_buddy_list *list)
{
	if (!gtkblist)
		return;

	gtk_widget_destroy(gtkblist->window);

	if (gtkblist->refresh_timer)
		g_source_remove(gtkblist->refresh_timer);
	if (gtkblist->timeout)
		g_source_remove(gtkblist->timeout);

	gtkblist->refresh_timer = 0;
	gtkblist->timeout = 0;
	gtkblist->window = gtkblist->vbox = gtkblist->treeview = NULL;
	gtkblist->treemodel = NULL;
	gtkblist->idle_column = NULL;
	gtkblist->warning_column = gtkblist->buddy_icon_column = NULL;
	gtkblist->bbox = gtkblist->tipwindow = NULL;
	protomenu = NULL;
	awaymenu = NULL;
	bpmenu = NULL;
}

static void gaim_gtk_blist_set_visible(struct gaim_buddy_list *list, gboolean show)
{
	if (!(gtkblist && gtkblist->window))
		return;

	if (show) {
		gaim_gtk_blist_restore_position();
		gtk_window_present(GTK_WINDOW(gtkblist->window));
	} else {
		if (!connections || docklet_count) {
#ifdef _WIN32
			wgaim_systray_minimize(gtkblist->window);
#endif
			gtk_widget_hide(gtkblist->window);
		} else {
			gtk_window_iconify(GTK_WINDOW(gtkblist->window));
		}
	}
}

void gaim_gtk_blist_docklet_toggle() {
	/* Useful for the docklet plugin and also for the win32 tray icon*/
	/* This is called when one of those is clicked--it will show/hide the 
	   buddy list/login window--depending on which is active */
	if (connections) {
		if (gtkblist && gtkblist->window) {
			if (GTK_WIDGET_VISIBLE(gtkblist->window)) {
				gaim_blist_set_visible(GAIM_WINDOW_ICONIFIED(gtkblist->window) || gaim_gtk_blist_obscured);
			} else {
#if _WIN32
				wgaim_systray_maximize(gtkblist->window);
#endif
				gaim_blist_set_visible(TRUE);
			}
		} else {
			/* we're logging in or something... do nothing */
			/* or should I make the blist? */
			debug_printf("docklet_toggle called with connections but no blist!\n");
		}
	} else if (mainwindow) {
		if (GTK_WIDGET_VISIBLE(mainwindow)) {
			if (GAIM_WINDOW_ICONIFIED(mainwindow)) {
				gtk_window_present(GTK_WINDOW(mainwindow));
			} else {
#if _WIN32
				wgaim_systray_minimize(mainwindow);
#endif
				gtk_widget_hide(mainwindow);
			}
		} else {
#if _WIN32
			wgaim_systray_maximize(mainwindow);
#endif
			show_login();
		}
	} else {
		show_login();
	}
}

void gaim_gtk_blist_docklet_add()
{
	docklet_count++;
}

void gaim_gtk_blist_docklet_remove()
{
	docklet_count--;
	if (!docklet_count) {
		if (connections)
			gaim_blist_set_visible(TRUE);
		else if (mainwindow)
			gtk_window_present(GTK_WINDOW(mainwindow));
		else
			show_login();
	}
}

static struct gaim_blist_ui_ops blist_ui_ops =
{
	gaim_gtk_blist_new_list,
	gaim_gtk_blist_new_node,
	gaim_gtk_blist_show,
	gaim_gtk_blist_update,
	gaim_gtk_blist_remove,
	gaim_gtk_blist_destroy,
	gaim_gtk_blist_set_visible
};


struct gaim_blist_ui_ops *gaim_get_gtk_blist_ui_ops()
{
	return &blist_ui_ops;
}



/*********************************************************************
 * Public utility functions                                          *
 *********************************************************************/

GdkPixbuf *
create_prpl_icon(struct gaim_account *account)
{
	struct prpl *prpl = find_prpl(account->protocol);
	GdkPixbuf *status = NULL;
	char *filename = NULL;
	const char *protoname = prpl->list_icon(account, NULL);
	/* "Hey, what's all this crap?" you ask.  Status icons will be themeable too, and 
	   then it will look up protoname from the theme */
	if (!strcmp(protoname, "aim")) {
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", "aim.png", NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
	} else if (!strcmp(protoname, "yahoo")) {
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", "yahoo.png", NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
	} else if (!strcmp(protoname, "msn")) {
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", "msn.png", NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
	} else if (!strcmp(protoname, "jabber")) {
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", "jabber.png", NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
	} else if (!strcmp(protoname, "icq")) {
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", "icq.png", NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
	} else if (!strcmp(protoname, "gadu-gadu")) {
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", "gadugadu.png", NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
	} else if (!strcmp(protoname, "napster")) {
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", "napster.png", NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
	} else if (!strcmp(protoname, "irc")) {
		filename = g_build_filename(DATADIR, "pixmaps", "gaim", "status", "default", "irc.png", NULL);
		status = gdk_pixbuf_new_from_file(filename,NULL);
		g_free(filename);
	}
	return status;
}

