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
#include "../config.h"
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <gtk/gtk.h>
#include "gaim.h"
#include "gtkhtml.h"

#include "pixmaps/cancel.xpm"
#include "pixmaps/save.xpm"
#include "pixmaps/ok.xpm"
#include "pixmaps/add.xpm"
#include "pixmaps/warn.xpm"

#include "pixmaps/angel.xpm"
#include "pixmaps/bigsmile.xpm"
#include "pixmaps/burp.xpm"
#include "pixmaps/crossedlips.xpm"
#include "pixmaps/cry.xpm"
#include "pixmaps/embarrassed.xpm"
#include "pixmaps/kiss.xpm"
#include "pixmaps/moneymouth.xpm"
#include "pixmaps/sad.xpm"
#include "pixmaps/scream.xpm"
#include "pixmaps/smile.xpm"
#include "pixmaps/smile8.xpm"
#include "pixmaps/think.xpm"
#include "pixmaps/tongue.xpm"
#include "pixmaps/wink.xpm"
#include "pixmaps/yell.xpm"

#define DEFAULT_FONT_NAME "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1"

#define PATHSIZE 1024

int smiley_array[FACE_TOTAL];
GdkColor bgcolor;
GdkColor fgcolor;

static GtkWidget *imdialog = NULL; /*I only want ONE of these :) */
static GtkWidget *infodialog = NULL;
static GList *dialogwindows = NULL;
static GtkWidget *exportdialog, *importdialog;
static GtkWidget *aliasdlg = NULL;
static GtkWidget *aliasentry = NULL;
static GtkWidget *aliasname = NULL;

struct create_away {
        GtkWidget *window;
        GtkWidget *entry;
        GtkWidget *text;
        GtkWidget *checkbx;
	struct away_message *mess;
};


struct warning {
        GtkWidget *window;
        GtkWidget *anon;
        char *who;
	struct gaim_connection *gc;
};

struct addbuddy {
        GtkWidget *window;
        GtkWidget *combo;
        GtkWidget *entry;
        GtkWidget *entry_for_alias;
	struct gaim_connection *gc;
};

struct addperm {
        GtkWidget *window;
        GtkWidget *entry;
	struct gaim_connection *gc;
	gboolean permit;
};

struct addbp {
        GtkWidget *window;
        GtkWidget *nameentry;
        GtkWidget *messentry;
	GtkWidget *sendim;
	GtkWidget *openwindow;
	GtkWidget *p_signon;
	GtkWidget *p_unaway;
	GtkWidget *p_unidle;
	GtkWidget *menu;

	struct aim_user *user;
};

struct findbyemail {
	GtkWidget *window;
	GtkWidget *emailentry;
};

struct findbyinfo {
	GtkWidget *window;
	GtkWidget *firstentry;
	GtkWidget *middleentry;
	GtkWidget *lastentry;
	GtkWidget *maidenentry;
	GtkWidget *cityentry;
	GtkWidget *stateentry;
	GtkWidget *countryentry;
};

struct info_dlg {
	GtkWidget *window;
	GtkWidget *text;
	GtkWidget *close;
};


struct set_info_dlg {
	GtkWidget *window;
	GtkWidget *menu;
	struct aim_user *user;
	GtkWidget *text;
	GtkWidget *save;
	GtkWidget *cancel;
};

struct set_dir_dlg {
	GtkWidget *window;
	GtkWidget *first;
	GtkWidget *middle;
	GtkWidget *last;
	GtkWidget *maiden;
	GtkWidget *city;
	GtkWidget *state;
	GtkWidget *country;
	GtkWidget *web;
	GtkWidget *cancel;
	GtkWidget *save;
};

struct linkdlg {
	GtkWidget *ok;
	GtkWidget *cancel;
	GtkWidget *window;
	GtkWidget *url;
	GtkWidget *text;
	GtkWidget *toggle;
	GtkWidget *entry;
};

struct passwddlg {
	GtkWidget *window;
	GtkWidget *ok;
	GtkWidget *cancel;
	GtkWidget *original;
	GtkWidget *new1;
	GtkWidget *new2;
	struct gaim_connection *gc;
};

/*------------------------------------------------------------------------*/
/*  Destroys                                                              */
/*------------------------------------------------------------------------*/

static gint delete_event_dialog(GtkWidget *w, GdkEventAny *e, struct conversation *c)
{
	gchar *object_data;
	object_data = gtk_object_get_user_data(GTK_OBJECT(w));

	if (GTK_IS_COLOR_SELECTION_DIALOG(w))
	{
		set_state_lock(1);
		if (w == c->fg_color_dialog) {
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->fgcolorbtn), FALSE);
			c->fg_color_dialog = NULL;
		} else {
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->bgcolorbtn), FALSE);
			c->bg_color_dialog = NULL;
		}
		set_state_lock(0);
	}
	else if (GTK_IS_FONT_SELECTION_DIALOG(w))
	{
		set_state_lock(1);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->font), FALSE);
		set_state_lock(0);
		c->font_dialog = NULL;
	}
	else if (!g_strcasecmp(object_data, "smiley dialog"))
	{
		set_state_lock(1);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->smiley), FALSE);
		set_state_lock(0);
		c->smiley_dialog = NULL;
	}
	else if (!g_strcasecmp(object_data, "log dialog"))
	{
		set_state_lock(1);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->wood), FALSE);
		set_state_lock(0);
		c->log_dialog = NULL;
	}
	
	dialogwindows = g_list_remove(dialogwindows, w);
	gtk_widget_destroy(w);
	
	return FALSE;
}

static void destroy_dialog(GtkWidget *w, GtkWidget *w2)
{
        GtkWidget *dest;
        
        if (!GTK_IS_WIDGET(w2))
                dest = w;
        else
                dest = w2;

        if (dest == imdialog)
		imdialog = NULL;

	if (dest == infodialog)
		infodialog = NULL;

	if (dest == exportdialog)
		exportdialog = NULL;

	if (dest == importdialog)
		importdialog = NULL;

	if (dest == aliasdlg) {
		aliasdlg = NULL;
		aliasentry = NULL;
		aliasname = NULL;
	}

        dialogwindows = g_list_remove(dialogwindows, dest);
        gtk_widget_destroy(dest);

}


void destroy_all_dialogs()
{
        GList *d = dialogwindows;
        
        while(d) {
                destroy_dialog(NULL, d->data);
                d = d->next;
        }
        
        g_list_free(dialogwindows);
        dialogwindows = NULL;

	if (awaymessage)
		do_im_back(NULL, NULL);

        if (imdialog) {
                destroy_dialog(NULL, imdialog);
                imdialog = NULL;
        }

	if (infodialog) {
                destroy_dialog(NULL, infodialog);
                infodialog = NULL;
        }
        
        if (exportdialog) {
                destroy_dialog(NULL, exportdialog);
                exportdialog = NULL;
        }

        if (importdialog) {
                destroy_dialog(NULL, importdialog);
                importdialog = NULL;
        }
}

static void do_warn(GtkWidget *widget, struct warning *w)
{
        serv_warn(w->gc, w->who, (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->anon))) ?
                   1 : 0);
        
        destroy_dialog(NULL, w->window);
}


void show_warn_dialog(struct gaim_connection *gc, char *who)
{
	GtkWidget *cancel;
	GtkWidget *warn;
	GtkWidget *label;
	GtkWidget *vbox;
        GtkWidget *bbox;
	GtkWidget *frame;
	GtkWidget *fbox;

        struct warning *w = g_new0(struct warning, 1);
        
        char *buf = g_malloc(128);
        w->window = gtk_window_new(GTK_WINDOW_DIALOG);
        gtk_window_set_wmclass(GTK_WINDOW(w->window), "warning", "Gaim");
	gtk_window_set_policy(GTK_WINDOW(w->window), FALSE, FALSE, TRUE);
	gtk_widget_show(w->window);
	dialogwindows = g_list_prepend(dialogwindows, w->window);
        bbox = gtk_hbox_new(TRUE, 10);
        vbox = gtk_vbox_new(FALSE, 5);
	fbox = gtk_vbox_new(FALSE, 5);

	frame = gtk_frame_new(_("Warn"));
	
	warn = picture_button(w->window, _("Warn"), warn_xpm);

	cancel = picture_button(w->window, _("Cancel"), cancel_xpm);

	if (display_options & OPT_DISP_COOL_LOOK)
	{
		gtk_button_set_relief(GTK_BUTTON(cancel), GTK_RELIEF_NONE);
		gtk_button_set_relief(GTK_BUTTON(warn), GTK_RELIEF_NONE);
	}
	
	/* Put the buttons in the box */

	gtk_box_pack_start(GTK_BOX(bbox), warn, FALSE, FALSE, 5);
        gtk_box_pack_end(GTK_BOX(bbox), cancel, FALSE, FALSE, 5);

        g_snprintf(buf, 127, _("Do you really want to warn %s?"), who);
        label = gtk_label_new(buf);
        gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 5);
        gtk_widget_show(label);
        w->anon = gtk_check_button_new_with_label(_("Warn anonymously?"));
        gtk_box_pack_start(GTK_BOX(vbox), w->anon, TRUE, TRUE, 5);

        label = gtk_label_new(_("Anonymous warnings are less harsh."));
        gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 5);
        gtk_widget_show(label);

        w->who = who;
	w->gc = gc;
	
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gtk_box_pack_start(GTK_BOX(fbox), frame, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(fbox), bbox, FALSE, FALSE, 5);

        /* Handle closes right */
        gtk_signal_connect(GTK_OBJECT(w->window), "delete_event",
                           GTK_SIGNAL_FUNC(destroy_dialog), w->window);
        gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
                           GTK_SIGNAL_FUNC(destroy_dialog), w->window);
        gtk_signal_connect(GTK_OBJECT(warn), "clicked",
                           GTK_SIGNAL_FUNC(do_warn), w);
        /* Finish up */
        gtk_widget_show(w->anon);
        gtk_widget_show(bbox);
        gtk_widget_show(vbox);
	gtk_widget_show(frame);
	gtk_widget_show(fbox);

	gtk_window_set_title(GTK_WINDOW(w->window), _("Gaim - Warn user?"));
        gtk_container_add(GTK_CONTAINER(w->window), fbox);
	gtk_container_set_border_width(GTK_CONTAINER(w->window), 5);
	gtk_widget_realize(w->window);
        aol_icon(w->window->window);

        gtk_widget_show(w->window);
	g_free(buf);
}


/*------------------------------------------------------------------------*/
/*  The dialog for getting an error                                       */
/*------------------------------------------------------------------------*/

void
do_error_dialog(char *message, char *title)
{
        GtkWidget *d;
	GtkWidget *label;
	GtkWidget *close;


	d = gtk_dialog_new();
	gtk_window_set_policy(GTK_WINDOW(d), FALSE, FALSE, TRUE);
        gtk_widget_realize(d);
        label = gtk_label_new(message);
        gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(d)->vbox),
		label, FALSE, FALSE, 5);
		
	close = picture_button(d, _("Close"), cancel_xpm);

	if (display_options & OPT_DISP_COOL_LOOK)
		gtk_button_set_relief(GTK_BUTTON(close), GTK_RELIEF_NONE);
	
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(d)->action_area), 
		close, FALSE, FALSE, 5);

	gtk_container_set_border_width(GTK_CONTAINER(d), 5);
	gtk_window_set_title(GTK_WINDOW(d), title);
	gtk_signal_connect(GTK_OBJECT(close), "clicked", GTK_SIGNAL_FUNC(destroy_dialog), d);
	aol_icon(d->window);

	gtk_widget_show(d);
}



void show_error_dialog(char *d)
{

	int no = atoi(d);
	char *w;
	char buf[256];
	char buf2[32];

	plugin_event(event_error, (void *)no, 0, 0, 0);

	w = strtok(NULL, ":");
 	
	
        switch(no) {
        case 69:
                g_snprintf(buf, sizeof(buf), _("Unable to write file %s."), w);
                break;
        case 169:
                g_snprintf(buf, sizeof(buf), _("Unable to read file %s."), w);
                break;
        case 269:
                g_snprintf(buf, sizeof(buf), _("Message too long, last %s bytes truncated."), w);
                break;
        case 901:
                g_snprintf(buf, sizeof(buf), _("%s not currently logged in."), w);
                break;
        case 902:
                g_snprintf(buf, sizeof(buf), _("Warning of %s not allowed."), w);
                break;
        case 903:
                g_snprintf(buf, sizeof(buf), _("A message has been dropped, you are exceeding the server speed limit."));
                break;
        case 950:
                g_snprintf(buf, sizeof(buf), _("Chat in %s is not available."), w);
                break;
        case 960:
                g_snprintf(buf, sizeof(buf), _("You are sending messages too fast to %s."), w);
                break;
        case 961:
                g_snprintf(buf, sizeof(buf), _("You missed an IM from %s because it was too big."), w);
                break;
        case 962:
                g_snprintf(buf, sizeof(buf), _("You missed an IM from %s because it was sent too fast."), w);
                break;
        case 970:
                g_snprintf(buf, sizeof(buf), _("Failure."));
                break;
        case 971:
                g_snprintf(buf, sizeof(buf), _("Too many matches."));
                break;
        case 972:
                g_snprintf(buf, sizeof(buf), _("Need more qualifiers."));
                break;
        case 973:
                g_snprintf(buf, sizeof(buf), _("Dir service temporarily unavailable."));
                break;
        case 974:
                g_snprintf(buf, sizeof(buf), _("Email lookup restricted."));
                break;
        case 975:
                g_snprintf(buf, sizeof(buf), _("Keyword ignored."));
                break;
        case 976:
                g_snprintf(buf, sizeof(buf), _("No keywords."));
                break;
        case 977:
                g_snprintf(buf, sizeof(buf), _("User has no directory information."));
                /* g_snprintf(buf, sizeof(buf), "Language not supported."); */
                break;
        case 978:
                g_snprintf(buf, sizeof(buf), _("Country not supported."));
                break;
        case 979:
                g_snprintf(buf, sizeof(buf), _("Failure unknown: %s."), w);
                break;
        case 980:
                g_snprintf(buf, sizeof(buf), _("Incorrect nickname or password."));
                break;
        case 981:
                g_snprintf(buf, sizeof(buf), _("The service is temporarily unavailable."));
                break;
        case 982:
                g_snprintf(buf, sizeof(buf), _("Your warning level is currently too high to log in."));
                break;
        case 983:
                g_snprintf(buf, sizeof(buf), _("You have been connecting and disconnecting too frequently.  Wait ten minutes and try again.  If you continue to try, you will need to wait even longer."));
                break;
        case 989:
                g_snprintf(buf, sizeof(buf), _("An unknown signon error has occurred: %s."), w);
                break;
        default:
                g_snprintf(buf, sizeof(buf), _("An unknown error, %d, has occured.  Info: %s"), no, w);
	}
	
	g_snprintf(buf2, sizeof(buf2), _("Gaim - Error %d"), no);


        do_error_dialog(buf, buf2);
        return;
}

static void do_im(GtkWidget *widget, GtkWidget *imentry)
{
        char *who;
        struct conversation *c;
	
        who = g_strdup(normalize(gtk_entry_get_text(GTK_ENTRY(imentry))));
	destroy_dialog(NULL, imdialog);
        imdialog = NULL;
        
        if (!strcasecmp(who, "")) {
                g_free(who);
		return;
	}

        c = find_conversation(who);

        if (c == NULL) {
                c = new_conversation(who);
        } else {
                gdk_window_raise(c->window->window);
	}
        g_free(who);
}

static void do_info(GtkWidget *widget, GtkWidget *infoentry)
{
	char *who;
	
        who = g_strdup(normalize(gtk_entry_get_text(GTK_ENTRY(infoentry))));
	destroy_dialog(NULL, infodialog);
        infodialog = NULL;
        
        if (!strcasecmp(who, "")) {
                g_free(who);
		return;
	}
	
	/* what do we want to do about this case? */
	if (connections)
		serv_get_info(connections->data, who);

	g_free(who);
}

void show_ee_dialog(int ee)
{
	GtkWidget *ok;
	GtkWidget *label;
	GtkWidget *box;
	GtkWidget *eedialog;

	eedialog = gtk_window_new(GTK_WINDOW_DIALOG);
        ok = gtk_button_new_with_label(_("OK"));
        box = gtk_vbox_new(FALSE, 10);


	if (display_options & OPT_DISP_COOL_LOOK)
		gtk_button_set_relief(GTK_BUTTON(ok), GTK_RELIEF_NONE);
	
	if (ee == 0)
		label = gtk_label_new("Amazing!  Simply Amazing!");
	else if (ee == 1)
		label = gtk_label_new("Pimpin\' Penguin Style! *Waddle Waddle*");
	else if (ee == 2)
		label = gtk_label_new("You should be me.  I'm so cute!");
	else
		label = gtk_label_new("Now that's what I like!");

	gtk_widget_show(label);
	gtk_widget_show(ok);

	gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(box), ok, FALSE, FALSE, 10);

	gtk_widget_show(box);

	gtk_container_add(GTK_CONTAINER(eedialog), box);
	gtk_window_set_title(GTK_WINDOW(eedialog), "Gaim - SUPRISE!");

	gtk_signal_connect(GTK_OBJECT(ok), "clicked",
			   GTK_SIGNAL_FUNC(destroy_dialog), eedialog);
	gtk_widget_realize(eedialog);
        aol_icon(eedialog->window);

	gtk_widget_show(eedialog);
}

void show_im_dialog()
{
	GtkWidget *button;
	GtkWidget *imentry;
        GtkWidget *vbox;
        GtkWidget *ebox;
        GtkWidget *bbox;
        GtkWidget *label;
	GtkWidget *frame;
	GtkWidget *fbox;

        if (!imdialog) {

                imdialog = gtk_window_new(GTK_WINDOW_DIALOG);
		gtk_window_set_wmclass(GTK_WINDOW(imdialog), "imdialog",
                                       "Gaim");
		gtk_widget_set_usize(imdialog, 255, 105);
		gtk_container_border_width(GTK_CONTAINER(imdialog), 5);
		gtk_window_set_policy(GTK_WINDOW(imdialog), FALSE, FALSE, TRUE);
		gtk_widget_show(imdialog);

		bbox = gtk_hbox_new(TRUE, 10);
                vbox = gtk_vbox_new(FALSE, 5);
                ebox = gtk_hbox_new(FALSE, 2);
		fbox = gtk_vbox_new(TRUE, 10);

		frame = gtk_frame_new(_("Send Instant Message"));
	
		imentry = gtk_entry_new();

		button = picture_button(imdialog, _("OK"), ok_xpm);
		gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);	
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				   GTK_SIGNAL_FUNC(do_im), imentry);

		button = picture_button(imdialog, _("Cancel"), cancel_xpm);
		gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);	
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				   GTK_SIGNAL_FUNC(destroy_dialog), imdialog);

                label = gtk_label_new(_("IM who:"));
                gtk_box_pack_start(GTK_BOX(ebox), label, TRUE, TRUE, 10);
                gtk_widget_show(label);

                gtk_box_pack_start(GTK_BOX(ebox), imentry, TRUE, TRUE, 10);

                gtk_box_pack_start(GTK_BOX(vbox), ebox, FALSE, FALSE, 5);
                gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 5);

                /* Handle closes right */
		gtk_signal_connect(GTK_OBJECT(imentry), "activate",
				   GTK_SIGNAL_FUNC(do_im), imentry);
                gtk_signal_connect(GTK_OBJECT(imdialog), "destroy",
                                   GTK_SIGNAL_FUNC(destroy_dialog), imdialog);

		/* Finish up */
                gtk_widget_show(ebox);
                gtk_widget_show(imentry);
                gtk_widget_show(bbox);
                gtk_widget_show(vbox);
		gtk_widget_show(fbox);
		gtk_widget_show(frame);
		gtk_container_add(GTK_CONTAINER(frame), vbox);
		gtk_box_pack_start(GTK_BOX(fbox), frame, FALSE, FALSE, 5);
		gtk_window_set_title(GTK_WINDOW(imdialog), _("Gaim - IM user"));
                gtk_container_add(GTK_CONTAINER(imdialog), fbox);
                gtk_widget_grab_focus(imentry);
                gtk_widget_realize(imdialog);
		
		aol_icon(imdialog->window);

        }
        gtk_widget_show(imdialog);
}

void show_info_dialog()
{
	GtkWidget *button;
	GtkWidget *infoentry;
        GtkWidget *vbox;
        GtkWidget *ebox;
        GtkWidget *bbox;
        GtkWidget *label;
	GtkWidget *frame;
	GtkWidget *fbox;

        if (!infodialog) {

                infodialog = gtk_window_new(GTK_WINDOW_DIALOG);
		gtk_window_set_wmclass(GTK_WINDOW(infodialog), "infodialog",
                                       "Gaim");
		gtk_widget_set_usize(infodialog, 255, 105);
		gtk_container_border_width(GTK_CONTAINER(infodialog), 5);
		gtk_window_set_policy(GTK_WINDOW(infodialog), FALSE, FALSE, TRUE);
		gtk_widget_show(infodialog);

		bbox = gtk_hbox_new(TRUE, 10);
                vbox = gtk_vbox_new(FALSE, 5);
                ebox = gtk_hbox_new(FALSE, 2);
		fbox = gtk_vbox_new(TRUE, 10);

		frame = gtk_frame_new(_("Get User Info"));
	
		infoentry = gtk_entry_new();

		button = picture_button(infodialog, _("OK"), ok_xpm);
		gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);	
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				   GTK_SIGNAL_FUNC(do_info), infoentry);

		button = picture_button(infodialog, _("Cancel"), cancel_xpm);
		gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);	
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				   GTK_SIGNAL_FUNC(destroy_dialog), infodialog);

                label = gtk_label_new(_("User:"));
                gtk_box_pack_start(GTK_BOX(ebox), label, TRUE, TRUE, 10);
                gtk_widget_show(label);

                gtk_box_pack_start(GTK_BOX(ebox), infoentry, TRUE, TRUE, 10);

                gtk_box_pack_start(GTK_BOX(vbox), ebox, FALSE, FALSE, 5);
                gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 5);

                /* Handle closes right */
		gtk_signal_connect(GTK_OBJECT(infoentry), "activate",
				   GTK_SIGNAL_FUNC(do_info), infoentry);
                gtk_signal_connect(GTK_OBJECT(infodialog), "destroy",
                                   GTK_SIGNAL_FUNC(destroy_dialog), infodialog);

		/* Finish up */
                gtk_widget_show(ebox);
                gtk_widget_show(infoentry);
                gtk_widget_show(bbox);
                gtk_widget_show(vbox);
		gtk_widget_show(fbox);
		gtk_widget_show(frame);
		gtk_container_add(GTK_CONTAINER(frame), vbox);
		gtk_box_pack_start(GTK_BOX(fbox), frame, FALSE, FALSE, 5);
		gtk_window_set_title(GTK_WINDOW(infodialog), _("Gaim - Get User Info"));
                gtk_container_add(GTK_CONTAINER(infodialog), fbox);
                gtk_widget_grab_focus(infoentry);
                gtk_widget_realize(infodialog);
		
		aol_icon(infodialog->window);

        }
        gtk_widget_show(infodialog);
}


/*------------------------------------------------------------------------*/
/*  The dialog for adding buddies                                         */
/*------------------------------------------------------------------------*/

extern void add_callback(GtkWidget *, struct conversation *);

void do_add_buddy(GtkWidget *w, struct addbuddy *a)
{
	char *grp, *who, *whoalias;
        struct conversation *c;
	GSList *n = connections;
	struct gaim_connection *g;
        
	who = gtk_entry_get_text(GTK_ENTRY(a->entry));
        grp = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(a->combo)->entry));
        whoalias = gtk_entry_get_text(GTK_ENTRY(a->entry_for_alias));

        c = find_conversation(who);

	if (a->gc) add_buddy(a->gc, grp, who, whoalias);
	else if (connections) add_buddy(connections->data, grp, who, whoalias);

        if (c != NULL) {
		update_convo_add_button(c);
	}
        
        build_edit_tree();

	while (n) {
		g = (struct gaim_connection *)n->data;
		serv_add_buddy(g, who);
		n = n->next;
	}

	do_export( (GtkWidget *) NULL, 0 );

        destroy_dialog(NULL, a->window);
}

void do_add_group(GtkWidget *w, struct addbuddy *a)
{
	char *grp;

	grp = gtk_entry_get_text(GTK_ENTRY(a->entry));

	if (a->gc) add_group(a->gc, grp);
	else if (connections) add_group(connections->data, grp);

	build_edit_tree();

	do_export( (GtkWidget *) NULL, 0 );

	destroy_dialog(NULL, a->window);
}


static GList *groups_tree(struct gaim_connection *gc)
{
	GList *tmp=NULL;
        char *tmp2;
	struct group *g;
        GSList *grp = gc->groups;
        
	if (!grp) {
                tmp2 = g_strdup(_("Buddies"));
                tmp = g_list_append(tmp, tmp2);
	} else {
		while(grp) {
			g = (struct group *)grp->data;
                        tmp2 = g->name;
                        tmp=g_list_append(tmp, tmp2);
			grp = g_slist_next(grp);
		}
	}
	return tmp;
}

static void free_dialog(GtkWidget *w, struct addbuddy *a)
{
	g_free(a);
}


void show_add_group(struct gaim_connection *gc)
{
	GtkWidget *cancel;
	GtkWidget *add;
	GtkWidget *label;
	GtkWidget *bbox;
	GtkWidget *vbox;
        GtkWidget *topbox;
	GtkWidget *frame;

        struct addbuddy *a = g_new0(struct addbuddy, 1);
	a->gc = gc;
        
        a->window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_wmclass(GTK_WINDOW(a->window), "add_group", "Gaim");
	gtk_window_set_policy(GTK_WINDOW(a->window), FALSE, FALSE, TRUE);
	gtk_widget_realize(a->window);
	dialogwindows = g_list_prepend(dialogwindows, a->window);

	bbox = gtk_hbox_new(TRUE, 10);
        topbox = gtk_hbox_new(FALSE, 5);
        vbox = gtk_vbox_new(FALSE, 5);

        a->entry = gtk_entry_new();
        /* Put the buttons in the box */

	add = picture_button(a->window, _("Add"), add_xpm);

	cancel = picture_button(a->window, _("Cancel"), cancel_xpm);

        gtk_box_pack_start(GTK_BOX(bbox), add, FALSE, FALSE, 5);
        gtk_box_pack_end(GTK_BOX(bbox), cancel, FALSE, FALSE, 5);

	frame = gtk_frame_new(NULL);
	gtk_frame_set_label(GTK_FRAME(frame), _("Add Group"));

        label = gtk_label_new(_("Group"));
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(topbox), label, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(topbox), a->entry, FALSE, FALSE, 5);

        /* And the boxes in the box */
        gtk_box_pack_start(GTK_BOX(vbox), topbox, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), bbox, TRUE, TRUE, 5);

        /* Handle closes right */
        gtk_signal_connect(GTK_OBJECT(a->window), "destroy",
                           GTK_SIGNAL_FUNC(destroy_dialog), a->window);
	gtk_signal_connect(GTK_OBJECT(a->window), "destroy",
			   GTK_SIGNAL_FUNC(free_dialog), a);
        gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
                           GTK_SIGNAL_FUNC(destroy_dialog), a->window);
        gtk_signal_connect(GTK_OBJECT(add), "clicked",
                           GTK_SIGNAL_FUNC(do_add_group), a);
        gtk_signal_connect(GTK_OBJECT(a->entry), "activate",
                           GTK_SIGNAL_FUNC(do_add_group), a);
        /* Finish up */
        gtk_widget_show(add);
        gtk_widget_show(cancel);
        gtk_widget_show(a->entry);
        gtk_widget_show(topbox);
        gtk_widget_show(bbox);
        gtk_widget_show(vbox);
	gtk_widget_show(frame);
        gtk_window_set_title(GTK_WINDOW(a->window), _("Gaim - Add Group"));
        gtk_window_set_focus(GTK_WINDOW(a->window), a->entry);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
        gtk_container_add(GTK_CONTAINER(a->window), frame);
	gtk_container_set_border_width(GTK_CONTAINER(a->window), 5);
        gtk_widget_realize(a->window);
        aol_icon(a->window->window);

	gtk_widget_show(a->window);
}

void show_add_buddy(struct gaim_connection *gc, char *buddy, char *group)
{
	GtkWidget *cancel;
	GtkWidget *add;
	GtkWidget *label;
	GtkWidget *bbox;
	GtkWidget *vbox;
        GtkWidget *topbox;
        GtkWidget *topbox1;
        GtkWidget *topbox2;
	GtkWidget *frame;

        struct addbuddy *a = g_new0(struct addbuddy, 1);
	a->gc = gc;
        
        a->window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_wmclass(GTK_WINDOW(a->window), "add_buddy", "Gaim");
	gtk_window_set_policy(GTK_WINDOW(a->window), FALSE, FALSE, TRUE);
        gtk_window_set_title(GTK_WINDOW(a->window), _("Gaim - Add Buddy"));
	gtk_container_set_border_width(GTK_CONTAINER(a->window), 5);
	gtk_widget_realize(a->window);
        aol_icon(a->window->window);
        gtk_signal_connect(GTK_OBJECT(a->window), "destroy",
                           GTK_SIGNAL_FUNC(destroy_dialog), a->window);
	gtk_signal_connect(GTK_OBJECT(a->window), "destroy",
			   GTK_SIGNAL_FUNC(free_dialog), a);
	dialogwindows = g_list_prepend(dialogwindows, a->window);

	frame = gtk_frame_new(_("Add Buddy"));
        gtk_container_add(GTK_CONTAINER(a->window), frame);
	gtk_widget_show(frame);

        vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
        gtk_widget_show(vbox);

        topbox = gtk_hbox_new(FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), topbox, TRUE, TRUE, 5);
        gtk_widget_show(topbox);

        label = gtk_label_new(_("Buddy"));
        gtk_box_pack_start(GTK_BOX(topbox), label, FALSE, FALSE, 5);
        gtk_widget_show(label);

        a->entry = gtk_entry_new();
        gtk_box_pack_end(GTK_BOX(topbox), a->entry, FALSE, FALSE, 5);
        gtk_window_set_focus(GTK_WINDOW(a->window), a->entry);
        if (buddy != NULL)
                gtk_entry_set_text(GTK_ENTRY(a->entry), buddy);
        gtk_signal_connect(GTK_OBJECT(a->entry), "activate",
                           GTK_SIGNAL_FUNC(do_add_buddy), a);
        gtk_widget_show(a->entry);

        topbox1 = gtk_hbox_new(FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), topbox1, TRUE, TRUE, 5);
        gtk_widget_show(topbox1);

        label = gtk_label_new(_("Alias"));
        gtk_box_pack_start(GTK_BOX(topbox1), label, FALSE, FALSE, 5);
        gtk_widget_show(label);

        a->entry_for_alias = gtk_entry_new();
        gtk_box_pack_end(GTK_BOX(topbox1), a->entry_for_alias, FALSE, FALSE, 5);
        gtk_widget_show(a->entry_for_alias);

        topbox2 = gtk_hbox_new(FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), topbox2, TRUE, TRUE, 5);
        gtk_widget_show(topbox2);

        label = gtk_label_new(_("Group"));
        gtk_box_pack_start(GTK_BOX(topbox2), label, FALSE, FALSE, 5);
        gtk_widget_show(label);

        a->combo = gtk_combo_new();
        gtk_combo_set_popdown_strings(GTK_COMBO(a->combo), groups_tree(gc ? gc : connections->data));
        if (group != NULL)
                gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(a->combo)->entry), group);
        gtk_box_pack_end(GTK_BOX(topbox2), a->combo, FALSE, FALSE, 5);
        gtk_widget_show(a->combo);

	bbox = gtk_hbox_new(TRUE, 10);
        gtk_box_pack_start(GTK_BOX(vbox), bbox, TRUE, TRUE, 5);
        gtk_widget_show(bbox);

	add = picture_button(a->window, _("Add"), add_xpm);
        gtk_box_pack_start(GTK_BOX(bbox), add, FALSE, FALSE, 5);
        gtk_signal_connect(GTK_OBJECT(add), "clicked",
                           GTK_SIGNAL_FUNC(do_add_buddy), a);
        gtk_widget_show(add);

	cancel = picture_button(a->window, _("Cancel"), cancel_xpm);
        gtk_box_pack_end(GTK_BOX(bbox), cancel, FALSE, FALSE, 5);
        gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
                           GTK_SIGNAL_FUNC(destroy_dialog), a->window);
        gtk_widget_show(cancel);
	
	gtk_widget_show(a->window);
}


/*------------------------------------------------------------------------*/
/*  The dialog for new buddy pounces                                      */
/*------------------------------------------------------------------------*/


void do_new_bp(GtkWidget *w, struct addbp *b)
{
        struct buddy_pounce *bp = g_new0(struct buddy_pounce, 1);
	
	g_snprintf(bp->name, 80, "%s", gtk_entry_get_text(GTK_ENTRY(b->nameentry)));
	g_snprintf(bp->message, 2048, "%s", gtk_entry_get_text(GTK_ENTRY(b->messentry)));
	g_snprintf(bp->pouncer, 80, "%s", b->user->username);

	bp->protocol = b->user->protocol;

	if (GTK_TOGGLE_BUTTON(b->openwindow)->active)
		bp->popup = 1;
	else
		bp->popup = 0;

	if (GTK_TOGGLE_BUTTON(b->sendim)->active)
		bp->sendim = 1;
	else
		bp->sendim = 0;
	
	if (GTK_TOGGLE_BUTTON(b->p_signon)->active)
		bp->signon = 1;
	else
		bp->signon = 0;
	
	if (GTK_TOGGLE_BUTTON(b->p_unaway)->active)
		bp->unaway = 1;
	else
		bp->unaway = 0;
	
	if (GTK_TOGGLE_BUTTON(b->p_unidle)->active)
		bp->unidle = 1;
	else
		bp->unidle = 0;

        buddy_pounces = g_list_append(buddy_pounces, bp);
	
        do_bp_menu();
        
        destroy_dialog(NULL, b->window);

	save_prefs();
        g_free(b);
}

static void pounce_choose(GtkWidget *opt, struct addbp *b)
{
	struct aim_user *u = gtk_object_get_user_data(GTK_OBJECT(opt));
	b->user = u;
}

static void pounce_user_menu(struct addbp *b, GtkWidget *box)
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *optmenu;
	GtkWidget *menu;
	GtkWidget *opt;
	GList *u = aim_users;
	struct aim_user *a;

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new(_("Pounce buddy as:"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	optmenu = gtk_option_menu_new();
	gtk_box_pack_start(GTK_BOX(hbox), optmenu, FALSE, FALSE, 0);
	gtk_widget_show(optmenu);

	menu = gtk_menu_new();

	/* We should go ahead and set this in order to prevent problems */
	if (u) {
		a = (struct aim_user *)u->data;
		b->user = a;
	}

	while (u) {
		a = (struct aim_user *)u->data;
		opt = gtk_menu_item_new_with_label(a->username);
		gtk_object_set_user_data(GTK_OBJECT(opt), a);
		gtk_signal_connect(GTK_OBJECT(opt), "activate", GTK_SIGNAL_FUNC(pounce_choose), b);
		gtk_menu_append(GTK_MENU(menu), opt);
		gtk_widget_show(opt);
		u = u->next;
	}

	gtk_option_menu_set_menu(GTK_OPTION_MENU(optmenu), menu);
	gtk_option_menu_set_history(GTK_OPTION_MENU(optmenu),
			g_list_index(aim_users, ((struct gaim_connection *)connections->data)->user));

	b->menu = optmenu;
}


void show_new_bp(char *name)
{
	GtkWidget *label;
	GtkWidget *bbox;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *button;

        struct addbp *b = g_new0(struct addbp, 1);
        
        b->window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_policy(GTK_WINDOW(b->window), FALSE, FALSE, TRUE);
	gtk_window_set_wmclass(GTK_WINDOW(b->window), "new_bp", "Gaim");
	gtk_widget_realize(b->window);
        dialogwindows = g_list_prepend(dialogwindows, b->window);
        bbox = gtk_hbox_new(TRUE, 10);
        vbox = gtk_vbox_new(FALSE, 5);
        b->nameentry = gtk_entry_new();
        b->messentry = gtk_entry_new();

	/* Build OK Button */

	button = picture_button(b->window, _("OK"), ok_xpm);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
                           GTK_SIGNAL_FUNC(do_new_bp), b);
	gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);	

	button = picture_button(b->window, _("Cancel"), cancel_xpm);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
                           GTK_SIGNAL_FUNC(destroy_dialog), b->window);
	gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);	

	/* Pounce as menu */
	pounce_user_menu(b, vbox);
	

	hbox = gtk_hbox_new(FALSE, 5);
	
        label = gtk_label_new(_("Buddy To Pounce:"));
        gtk_widget_show(label);
	gtk_widget_show(hbox);
        gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), b->nameentry, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
	

	label = gtk_label_new(_("Events:"));
	
	/* Set up the different options */
	b->p_signon = gtk_check_button_new_with_label(_("Sign On"));
	gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b->p_signon), TRUE);
	b->p_unaway = gtk_check_button_new_with_label(_("Return from away"));
	b->p_unidle = gtk_check_button_new_with_label(_("Return from idle"));

	/* Show them */
	gtk_widget_show(label);
	gtk_widget_show(b->p_signon);
	gtk_widget_show(b->p_unaway);
	gtk_widget_show(b->p_unidle);

	/* And pack'em */
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), b->p_signon, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), b->p_unaway, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), b->p_unidle, FALSE, FALSE, 0);


	label = gtk_label_new(_("Actions:"));

	/* And now the other stuff */	
	b->openwindow = gtk_check_button_new_with_label(_("Open IM window on pounce"));
	gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b->openwindow), FALSE);
	
	b->sendim = gtk_check_button_new_with_label(_("Send IM on pounce")); 
	gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b->sendim), TRUE);

	gtk_widget_show(label);
	gtk_widget_show(b->openwindow);
	gtk_widget_show(b->sendim);

	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), b->openwindow, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), b->sendim, FALSE, FALSE, 0);

        label = gtk_label_new(_("Message to send:"));
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), b->messentry, FALSE, FALSE, 0);


        /* And the boxes in the box */
        gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 0);

        /* Handle closes right */
        gtk_signal_connect(GTK_OBJECT(b->window), "destroy",
                           GTK_SIGNAL_FUNC(destroy_dialog), b->window);
        gtk_signal_connect(GTK_OBJECT(b->messentry), "activate",
                           GTK_SIGNAL_FUNC(do_new_bp), b);

        
        /* Finish up */
        gtk_widget_show(b->nameentry);
        gtk_widget_show(b->messentry);
        gtk_widget_show(bbox);
        gtk_widget_show(vbox);
        gtk_window_set_title(GTK_WINDOW(b->window), _("Gaim - New Buddy Pounce"));
        if (name != NULL) {
                gtk_entry_set_text(GTK_ENTRY(b->nameentry), name);
                gtk_window_set_focus(GTK_WINDOW(b->window), b->messentry);
        } else
                gtk_window_set_focus(GTK_WINDOW(b->window), b->nameentry);
        gtk_container_add(GTK_CONTAINER(b->window), vbox);
        gtk_container_border_width(GTK_CONTAINER(b->window), 10);
        aol_icon(b->window->window);
        gtk_widget_show(b->window);
}



/*------------------------------------------------------------------------*/
/*  The dialog for SET INFO / SET DIR INFO                                */
/*------------------------------------------------------------------------*/

void do_save_info(GtkWidget *widget, struct set_info_dlg *b)
{
	gchar *junk;
	char *buf;
	struct gaim_connection *gc;

	junk = gtk_editable_get_chars(GTK_EDITABLE(b->text), 0, -1);

	if (b->user) {
		g_snprintf(b->user->user_info, sizeof(b->user->user_info), "%s", junk);
		gc = b->user->gc;
			
		save_prefs();

		if (gc) {
			buf = g_malloc(strlen(junk) * 4);
			if (!buf) {
				buf = g_malloc(1);
				buf[0] = 0;
			}
			g_snprintf(buf, MIN(strlen(junk) * 2, 4096), "%s", junk);
			serv_set_info(gc, buf);
			g_free(buf);
		}
	}
	g_free(junk);
	destroy_dialog(NULL, b->window);
	g_free(b);
}

void do_set_dir(GtkWidget *widget, struct set_dir_dlg *b)
{
        char *first = gtk_entry_get_text(GTK_ENTRY(b->first));
        int web = GTK_TOGGLE_BUTTON(b->web)->active;
	char *middle = gtk_entry_get_text(GTK_ENTRY(b->middle));
	char *last = gtk_entry_get_text(GTK_ENTRY(b->last));
	char *maiden = gtk_entry_get_text(GTK_ENTRY(b->maiden));
	char *city = gtk_entry_get_text(GTK_ENTRY(b->city));
	char *state = gtk_entry_get_text(GTK_ENTRY(b->state));
	char *country = gtk_entry_get_text(GTK_ENTRY(b->country));


	/* FIXME : set dir. not important */
        if (connections)
		serv_set_dir(connections->data, first, middle, last, maiden, city, state, country, web);

        destroy_dialog(NULL, b->window);
	g_free(b);
}

void show_set_dir()
{
	GtkWidget *label;
	GtkWidget *bot;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *frame;
	GtkWidget *fbox;

	struct set_dir_dlg *b = g_new0(struct set_dir_dlg, 1);

	b->window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_widget_set_usize(b->window, 300, 320);
	gtk_window_set_policy(GTK_WINDOW(b->window), FALSE, FALSE, TRUE);
	gtk_window_set_wmclass(GTK_WINDOW(b->window), "set_dir", "Gaim");
	gtk_widget_show(b->window);

	dialogwindows = g_list_prepend(dialogwindows, b->window);

	vbox = gtk_vbox_new(FALSE, 5);
	fbox = gtk_vbox_new(FALSE, 5);

	frame = gtk_frame_new(_("Directory Info"));	

	/* Build Save Button */

	b->save = picture_button(b->window, _("Save"), save_xpm);

	b->cancel = picture_button(b->window, _("Cancel"), cancel_xpm);
	
	bot = gtk_hbox_new(TRUE, 10);

	gtk_box_pack_start(GTK_BOX(bot), b->save, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(bot), b->cancel, FALSE, FALSE, 5);

	gtk_widget_show(bot);

	b->first = gtk_entry_new();
	b->middle = gtk_entry_new();
	b->last = gtk_entry_new();
	b->maiden = gtk_entry_new();
	b->city = gtk_entry_new();
	b->state = gtk_entry_new();
	b->country = gtk_entry_new();
	b->web = gtk_check_button_new_with_label(_("Allow Web Searches To Find Your Info"));

	/* Line 1 */	
	label = gtk_label_new(_("First Name"));
	gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 2);
	gtk_box_pack_end(GTK_BOX(hbox), b->first, FALSE, FALSE, 2);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
	gtk_widget_show(hbox);

	/* Line 2 */
        label = gtk_label_new(_("Middle Name"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 2);
	gtk_box_pack_end(GTK_BOX(hbox), b->middle, FALSE, FALSE, 2);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
	gtk_widget_show(hbox);


	/* Line 3 */
        label = gtk_label_new(_("Last Name"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 2);
	gtk_box_pack_end(GTK_BOX(hbox), b->last, FALSE, FALSE, 2);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
	gtk_widget_show(hbox);

	/* Line 4 */
        label = gtk_label_new(_("Maiden Name"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 2);
	gtk_box_pack_end(GTK_BOX(hbox), b->maiden, FALSE, FALSE, 2);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
	gtk_widget_show(hbox);

	/* Line 5 */
        label = gtk_label_new(_("City"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 2);
	gtk_box_pack_end(GTK_BOX(hbox), b->city, FALSE, FALSE, 2);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
	gtk_widget_show(hbox);

	/* Line 6 */
	label = gtk_label_new(_("State"));        
	gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 2);
	gtk_box_pack_end(GTK_BOX(hbox), b->state, FALSE, FALSE, 2);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
	gtk_widget_show(hbox);

	/* Line 7 */
        label = gtk_label_new(_("Country"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 2);
	gtk_box_pack_end(GTK_BOX(hbox), b->country, FALSE, FALSE, 2);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
	gtk_widget_show(hbox);

	/* Line 8 */

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), b->web, TRUE, TRUE, 2);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);

	/* And add the buttons */
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	gtk_box_pack_start(GTK_BOX(fbox), frame, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(fbox), bot, FALSE, FALSE, 2);


	gtk_widget_show(vbox);
	gtk_widget_show(fbox);

	gtk_widget_show(frame);
        gtk_widget_show(b->first); 
        gtk_widget_show(b->middle);
        gtk_widget_show(b->last); 
        gtk_widget_show(b->maiden);
        gtk_widget_show(b->city);
	gtk_widget_show(b->state);
	gtk_widget_show(b->country);
	gtk_widget_show(b->web);

        gtk_signal_connect(GTK_OBJECT(b->window), "destroy",
                           GTK_SIGNAL_FUNC(destroy_dialog), b->window);
        gtk_signal_connect(GTK_OBJECT(b->cancel), "clicked",
                           GTK_SIGNAL_FUNC(destroy_dialog), b->window);
        gtk_signal_connect(GTK_OBJECT(b->save), "clicked",                                                      GTK_SIGNAL_FUNC(do_set_dir), b);   	
	
	gtk_container_add(GTK_CONTAINER(b->window), fbox);
	gtk_container_border_width(GTK_CONTAINER(b->window), 5);

	gtk_window_set_title(GTK_WINDOW(b->window), _("Gaim - Set Dir Info"));
        gtk_window_set_focus(GTK_WINDOW(b->window), b->first);
        gtk_widget_realize(b->window);
	aol_icon(b->window->window);

	gtk_widget_show(b->window);	
}

void do_change_password(GtkWidget *widget, struct passwddlg *b)
{
	gchar *orig, *new1, *new2;

	orig = gtk_entry_get_text(GTK_ENTRY(b->original));
	new1 = gtk_entry_get_text(GTK_ENTRY(b->new1));
	new2 = gtk_entry_get_text(GTK_ENTRY(b->new2));

	if (strcasecmp(new1, new2)) {
		do_error_dialog(_("New Passwords Do Not Match"), _("Gaim - Change Password Error"));
		return ;
	}

	if ((strlen(orig) < 1) || (strlen(new1) < 1) || (strlen(new2) < 1)) {
		do_error_dialog(_("Fill out all fields completely"), _("Gaim - Change Password Error"));
		return;
	}

	serv_change_passwd(b->gc, orig, new1);
	
	destroy_dialog(NULL, b->window);
	g_free(b);
}

static void pwd_choose(GtkObject *obj, struct passwddlg *pwd)
{
	pwd->gc = (struct gaim_connection *)gtk_object_get_user_data(obj);
}

static void passwd_multi_menu(GtkWidget *box, struct passwddlg *pwd)
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *optmenu;
	GtkWidget *menu;
	GtkWidget *opt;
	GSList *c = connections;
	struct gaim_connection *g;

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new(_("Change password for:"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	optmenu = gtk_option_menu_new();
	gtk_box_pack_end(GTK_BOX(hbox), optmenu, FALSE, FALSE, 5);
	gtk_widget_show(optmenu);

	menu = gtk_menu_new();

	while (c) {
		g = (struct gaim_connection *)c->data;
		opt = gtk_menu_item_new_with_label(g->username);
		gtk_object_set_user_data(GTK_OBJECT(opt), g);
		gtk_signal_connect(GTK_OBJECT(opt), "activate", GTK_SIGNAL_FUNC(pwd_choose), pwd);
		gtk_menu_append(GTK_MENU(menu), opt);
		gtk_widget_show(opt);
		c = c->next;
	}

	gtk_option_menu_set_menu(GTK_OPTION_MENU(optmenu), menu);
	gtk_option_menu_set_history(GTK_OPTION_MENU(optmenu), 0);

	pwd->gc = (struct gaim_connection *)connections->data;
}

void show_change_passwd()
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *vbox;
	GtkWidget *fbox;
	GtkWidget *frame;

	struct passwddlg *b = g_new0(struct passwddlg, 1);

	b->window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_widget_set_usize(b->window, 325, -1);
	gtk_window_set_policy(GTK_WINDOW(b->window), FALSE, FALSE, TRUE);
	gtk_window_set_wmclass(GTK_WINDOW(b->window), "change_passwd", "Gaim");
	gtk_container_border_width(GTK_CONTAINER(b->window), 5);
	gtk_window_set_title(GTK_WINDOW(b->window), _("Gaim - Password Change"));
        gtk_signal_connect(GTK_OBJECT(b->window), "destroy",
                           GTK_SIGNAL_FUNC(destroy_dialog), b->window);
	gtk_widget_realize(b->window);
	aol_icon(b->window->window);
	dialogwindows = g_list_prepend(dialogwindows, b->window);

	fbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(b->window), fbox);
	gtk_widget_show(fbox);

	frame = gtk_frame_new(_("Change Password"));
	gtk_box_pack_start(GTK_BOX(fbox), frame, FALSE, FALSE, 5);
	gtk_widget_show(frame);

	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	gtk_widget_show(vbox);

#ifndef NO_MULTI
	passwd_multi_menu(vbox, b);
#else
	b->gc = connections->data;
#endif

	/* First Line */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new(_("Original Password"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	b->original = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(b->original), FALSE);
	gtk_box_pack_end(GTK_BOX(hbox), b->original, FALSE, FALSE, 5);
	gtk_widget_show(b->original);

	/* Next Line */	
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
	gtk_widget_show(hbox);

        label = gtk_label_new(_("New Password"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
        gtk_widget_show(label);

        b->new1 = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(b->new1), FALSE);
	gtk_box_pack_end(GTK_BOX(hbox), b->new1, FALSE, FALSE, 5);
        gtk_widget_show(b->new1);

	/* Next Line */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
	gtk_widget_show(hbox);
	
	label = gtk_label_new(_("New Password (again)"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
        gtk_widget_show(label);

        b->new2 = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(b->new2), FALSE);
	gtk_box_pack_end(GTK_BOX(hbox), b->new2, FALSE, FALSE, 5);
        gtk_widget_show(b->new2);

	/* Now do our row of buttons */	
	hbox = gtk_hbox_new(TRUE, TRUE);
	gtk_box_pack_start(GTK_BOX(fbox), hbox, FALSE, FALSE, 5);
	gtk_widget_show(hbox);

	b->ok = picture_button(b->window, _("OK"), ok_xpm);
	gtk_box_pack_start(GTK_BOX(hbox), b->ok, FALSE, FALSE, 5);
        gtk_signal_connect(GTK_OBJECT(b->ok), "clicked",
                           GTK_SIGNAL_FUNC(do_change_password), b);

	b->cancel = picture_button(b->window, _("Cancel"), cancel_xpm);
	gtk_box_pack_start(GTK_BOX(hbox), b->cancel, FALSE, FALSE, 5);
        gtk_signal_connect(GTK_OBJECT(b->cancel), "clicked",
                           GTK_SIGNAL_FUNC(destroy_dialog), b->window);


	gtk_widget_show(b->window);
}

static void info_choose(GtkWidget *opt, struct set_info_dlg *b)
{
	int text_len;
	struct aim_user *u = gtk_object_get_user_data(GTK_OBJECT(opt));
	b->user = u;
	text_len = gtk_text_get_length(GTK_TEXT(b->text));
	gtk_text_set_point(GTK_TEXT(b->text), 0);
	gtk_text_forward_delete(GTK_TEXT(b->text), text_len);
	gtk_text_insert(GTK_TEXT(b->text), NULL, NULL, NULL, u->user_info, -1);
}

static void info_user_menu(struct set_info_dlg *b, GtkWidget *box)
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *optmenu;
	GtkWidget *menu;
	GtkWidget *opt;
	GList *u = aim_users;
	struct aim_user *a;

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new(_("Set info for:"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	optmenu = gtk_option_menu_new();
	gtk_box_pack_start(GTK_BOX(hbox), optmenu, FALSE, FALSE, 5);
	gtk_widget_show(optmenu);

	menu = gtk_menu_new();

	while (u) {
		a = (struct aim_user *)u->data;
		opt = gtk_menu_item_new_with_label(a->username);
		gtk_object_set_user_data(GTK_OBJECT(opt), a);
		gtk_signal_connect(GTK_OBJECT(opt), "activate", GTK_SIGNAL_FUNC(info_choose), b);
		gtk_menu_append(GTK_MENU(menu), opt);
		gtk_widget_show(opt);
		u = u->next;
	}

	gtk_option_menu_set_menu(GTK_OPTION_MENU(optmenu), menu);
	gtk_option_menu_set_history(GTK_OPTION_MENU(optmenu), 0);

	b->menu = optmenu;
}

void show_set_info()
{
	GtkWidget *bot;
	GtkWidget *top;
	
	struct set_info_dlg *b = g_new0(struct set_info_dlg, 1);

	b->window = gtk_window_new(GTK_WINDOW_DIALOG);
        gtk_window_set_wmclass(GTK_WINDOW(b->window), "set_info", "Gaim");
	dialogwindows = g_list_prepend(dialogwindows, b->window);
	gtk_widget_realize(b->window);

	bot = gtk_hbox_new(TRUE, 10);
	top = gtk_vbox_new(FALSE, 10);

	/* Build OK Button */

	b->save = picture_button(b->window, _("Save"), save_xpm);
	b->cancel = picture_button(b->window, _("Cancel"), cancel_xpm);

	gtk_box_pack_start(GTK_BOX(bot), b->save, FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(bot), b->cancel, FALSE, FALSE, 10);

	gtk_signal_connect(GTK_OBJECT(b->window), "destroy",
			   GTK_SIGNAL_FUNC(destroy_dialog), b->window);
	gtk_signal_connect(GTK_OBJECT(b->cancel), "clicked",
			   GTK_SIGNAL_FUNC(destroy_dialog), b->window);
	gtk_signal_connect(GTK_OBJECT(b->save), "clicked",
			   GTK_SIGNAL_FUNC(do_save_info), b);

	gtk_widget_show(bot);

	info_user_menu(b, top);

	b->text = gtk_text_new(NULL, NULL);
	gtk_text_set_word_wrap(GTK_TEXT(b->text), TRUE);
	gtk_text_set_editable(GTK_TEXT(b->text), TRUE);
	gtk_widget_set_usize(b->text, 350, 100);
	if (aim_users) {
		gtk_text_insert(GTK_TEXT(b->text), NULL, NULL, NULL,
				((struct aim_user *)aim_users->data)->user_info, -1);
		b->user = (struct aim_user *)aim_users->data;
	}

	gtk_widget_show(b->text);

	gtk_box_pack_start(GTK_BOX(top), b->text, TRUE, TRUE, 10);
	gtk_widget_show(top);

	gtk_box_pack_start(GTK_BOX(top), bot, FALSE, FALSE, 10);

	gtk_container_add(GTK_CONTAINER(b->window), top);
        gtk_container_border_width(GTK_CONTAINER(b->window), 10);
        gtk_widget_realize(b->window);
	aol_icon(b->window->window);
	
	gtk_window_set_title(GTK_WINDOW(b->window), _("Gaim - Set User Info"));
	gtk_widget_show(b->window);

}

/*------------------------------------------------------------------------*/
/*  The dialog for the info requests                                      */
/*------------------------------------------------------------------------*/

static void info_dlg_free(GtkWidget *b, struct info_dlg *d) {
	g_free(d);
}

void g_show_info_text(char *info)
{
        GtkWidget *ok;
        GtkWidget *label;
	GtkWidget *text;
        GtkWidget *bbox;
        GtkWidget *sw;

        struct info_dlg *b = g_new0(struct info_dlg, 1);

        b->window = gtk_window_new(GTK_WINDOW_DIALOG);
        dialogwindows = g_list_prepend(dialogwindows, b->window);
	gtk_window_set_title(GTK_WINDOW(b->window), "Gaim");
        gtk_container_border_width(GTK_CONTAINER(b->window), 5);
        bbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(b->window), bbox);
		gtk_widget_realize(GTK_WIDGET(b->window));
        ok = picture_button(b->window, _("OK"), ok_xpm);
	gtk_signal_connect(GTK_OBJECT(b->window), "destroy",
			   GTK_SIGNAL_FUNC(destroy_dialog), b->window);
	gtk_signal_connect(GTK_OBJECT(b->window), "destroy",
			   GTK_SIGNAL_FUNC(info_dlg_free), b);
        gtk_signal_connect(GTK_OBJECT(ok), "clicked",
			   GTK_SIGNAL_FUNC(destroy_dialog), b->window);

	if (display_options & OPT_DISP_COOL_LOOK)
		gtk_button_set_relief(GTK_BUTTON(ok), GTK_RELIEF_NONE);

        label = gtk_label_new(_("Below are the results of your search: "));

	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
					GTK_POLICY_NEVER,
					GTK_POLICY_ALWAYS);
	text = gtk_html_new(NULL, NULL);
	b->text = text;
	gtk_container_add(GTK_CONTAINER(sw), text);

	GTK_HTML (text)->hadj->step_increment = 10.0;
	GTK_HTML (text)->vadj->step_increment = 10.0;
	gtk_widget_set_usize(sw, 300, 250);

	gtk_box_pack_start(GTK_BOX(bbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(bbox), sw, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(bbox), ok, FALSE, FALSE, 0);

	aol_icon(b->window->window);
	gtk_widget_show_all(b->window);

	if (display_options & OPT_DISP_SHOW_SMILEY)
		write_html_with_smileys(b->window, b->text, info);
	else
		gtk_html_append_text(GTK_HTML(b->text), info, (display_options & OPT_DISP_IGNORE_COLOUR) ? HTML_OPTION_NO_COLOURS : 0);
	gtk_html_append_text(GTK_HTML(b->text), "</BODY>", 0);

	gtk_adjustment_set_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(sw)), 0);
}

void g_show_info(struct aim_user *user, char *url) {
	char *url_text = grab_url(user, url);
	if (connections)
		g_show_info_text(away_subs(url_text,
					((struct gaim_connection *)connections->data)->username));
	else
		g_show_info_text(url_text);
	g_free(url_text);
}

/*------------------------------------------------------------------------*/
/*  The dialog for adding to permit/deny                                  */
/*------------------------------------------------------------------------*/


static void do_add_perm(GtkWidget *w, struct addperm *p)
{

        char *who;
        char *name;
        int d = 0;


        who = gtk_entry_get_text(GTK_ENTRY(p->entry));

        name = g_malloc(strlen(who) + 2);
        g_snprintf(name, strlen(who) + 2, "%s", who);
        
        if (!p->permit) {
		GSList *d = p->gc->deny;
		char *n;
		n = g_strdup(normalize(name));
		while (d) {
			if (!strcasecmp(n, normalize(d->data)))
				break;
			d = d->next;
		}
		g_free(n);
		if (!d) {
			p->gc->deny = g_slist_append(p->gc->deny, name);
			build_block_list();
			serv_add_deny(p->gc, name);
			do_export(0, 0);
		} else
			g_free(name);
        } else {
		GSList *d = p->gc->permit;
		char *n;
		n = g_strdup(normalize(name));
		while (d) {
			if (!strcasecmp(n, normalize(d->data)))
				break;
			d = d->next;
		}
		g_free(n);
		if (!d) {
			p->gc->permit = g_slist_append(p->gc->permit, name);
			build_allow_list();
			serv_add_permit(p->gc, name);
			do_export(0, 0);
		} else
			g_free(name);
        }

        destroy_dialog(NULL, p->window);
}



void show_add_perm(struct gaim_connection *gc, char *who, gboolean permit)
{
	GtkWidget *cancel;
	GtkWidget *add;
	GtkWidget *label;
        GtkWidget *bbox;
        GtkWidget *vbox;
        GtkWidget *topbox;
	GtkWidget *frame;
	
	struct addperm *p = g_new0(struct addperm, 1);
	p->gc = gc;
	p->permit = permit;

        p->window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_container_set_border_width(GTK_CONTAINER(p->window), 5);
	gtk_window_set_policy(GTK_WINDOW(p->window), FALSE, FALSE, TRUE);
	gtk_widget_realize(p->window);

	dialogwindows = g_list_prepend(dialogwindows, p->window);

	bbox = gtk_hbox_new(TRUE, 10);
        topbox = gtk_hbox_new(FALSE, 5);
        vbox = gtk_vbox_new(FALSE, 5);
        p->entry = gtk_entry_new();

	if (permit)
		frame = gtk_frame_new(_("Permit"));
	else
		frame = gtk_frame_new(_("Deny"));

	/* Build Add Button */

	if (permit)
		add = picture_button(p->window, _("Permit"), add_xpm);
	else
		add = picture_button(p->window, _("Deny"), add_xpm);
	cancel = picture_button(p->window, _("Cancel"), cancel_xpm);
	
	/* End of Cancel Button */
        if (who != NULL)
                gtk_entry_set_text(GTK_ENTRY(p->entry), who);

        /* Put the buttons in the box */
	
	gtk_box_pack_start(GTK_BOX(bbox), add, FALSE, FALSE, 5);
        gtk_box_pack_end(GTK_BOX(bbox), cancel, FALSE, FALSE, 5);
		
        label = gtk_label_new(_("Add"));
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(topbox), label, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(topbox), p->entry, FALSE, FALSE, 5);
        /* And the boxes in the box */
        gtk_box_pack_start(GTK_BOX(vbox), topbox, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);


        /* Handle closes right */
        gtk_signal_connect(GTK_OBJECT(p->window), "destroy",
                           GTK_SIGNAL_FUNC(destroy_dialog), p->window);
        gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
                           GTK_SIGNAL_FUNC(destroy_dialog), p->window);
        gtk_signal_connect(GTK_OBJECT(add), "clicked",
                           GTK_SIGNAL_FUNC(do_add_perm), p);
        gtk_signal_connect(GTK_OBJECT(p->entry), "activate",
                           GTK_SIGNAL_FUNC(do_add_perm), p);

        /* Finish up */
        gtk_widget_show(add);
        gtk_widget_show(cancel);
        gtk_widget_show(p->entry);
        gtk_widget_show(topbox);
        gtk_widget_show(bbox);
        gtk_widget_show(vbox);
	gtk_widget_show(frame);
	if (permit)
		gtk_window_set_title(GTK_WINDOW(p->window), _("Gaim - Add Permit"));
	else
		gtk_window_set_title(GTK_WINDOW(p->window), _("Gaim - Add Deny"));
        gtk_window_set_focus(GTK_WINDOW(p->window), p->entry);
        gtk_container_add(GTK_CONTAINER(p->window), frame);
        gtk_widget_realize(p->window);
        aol_icon(p->window->window);

        gtk_widget_show(p->window);
}


/*------------------------------------------------------------------------*/
/*  Functions Called To Add A Log                                          */
/*------------------------------------------------------------------------*/

void cancel_log(GtkWidget *widget, struct conversation *c)
{	
	if (c->wood)
	{
		set_state_lock(1);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->wood), FALSE);
		set_state_lock(0);
	}
	dialogwindows = g_list_remove(dialogwindows, c->log_dialog);
	gtk_widget_destroy(c->log_dialog);
	c->log_dialog = NULL;	
}

void do_log(GtkWidget *w, struct conversation *c)
{
        struct log_conversation *l;
        char buf[128];
	char *file;
	char path[PATHSIZE];

        if (!find_log_info(c->name)) {
		file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(c->log_dialog));
		strncpy( path, file, PATHSIZE - 1 );
		if (file_is_dir(path, c->log_dialog)) {
			return;
		}

                l = (struct log_conversation *)g_new0(struct log_conversation, 1);
                strcpy(l->name, c->name);
                strcpy(l->filename, file);
                log_conversations = g_list_append(log_conversations, l);

                if (c != NULL)
                {
                        g_snprintf(buf, sizeof(buf), LOG_CONVERSATION_TITLE, c->name);
                        gtk_window_set_title(GTK_WINDOW(c->window), buf);
                }
        }

        save_prefs();
        cancel_log(NULL, c);
} 

void show_log_dialog(struct conversation *c)
{
	char *buf = g_malloc(BUF_LEN);
	
	if (!c->log_dialog) {
		c->log_dialog = gtk_file_selection_new(_("Gaim - Log Conversation"));
		
		gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(c->log_dialog));	
		
		g_snprintf(buf, BUF_LEN - 1, "%s/%s.log", getenv("HOME"), c->name);
		gtk_object_set_user_data(GTK_OBJECT(c->log_dialog), "log dialog");
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(c->log_dialog), buf);
		gtk_signal_connect(GTK_OBJECT(c->log_dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_dialog), c);
		gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(c->log_dialog)->ok_button), "clicked", GTK_SIGNAL_FUNC(do_log), c);
		gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(c->log_dialog)->cancel_button), "clicked", GTK_SIGNAL_FUNC(cancel_log), c);
	}

	g_free(buf);

	gtk_widget_show(c->log_dialog);
	gdk_window_raise(c->log_dialog->window);
}

/*------------------------------------------------------*/
/* Find Buddy By Email                                  */
/*------------------------------------------------------*/

void do_find_info(GtkWidget *w, struct findbyinfo *b)
{
        char *first;
	char *middle;
	char *last;
	char *maiden;
	char *city;
	char *state;
	char *country;

        first = gtk_entry_get_text(GTK_ENTRY(b->firstentry));
	middle = gtk_entry_get_text(GTK_ENTRY(b->middleentry));
	last = gtk_entry_get_text(GTK_ENTRY(b->lastentry));
	maiden = gtk_entry_get_text(GTK_ENTRY(b->maidenentry));
	city = gtk_entry_get_text(GTK_ENTRY(b->cityentry)); 
	state = gtk_entry_get_text(GTK_ENTRY(b->stateentry)); 
	country = gtk_entry_get_text(GTK_ENTRY(b->countryentry)); 

	/* FIXME : dir search. not sure if even works; not important */
	if (connections)
		serv_dir_search(connections->data, first, middle, last, maiden, city, state, country, "");
        destroy_dialog(NULL, b->window);
} 

void do_find_email(GtkWidget *w, struct findbyemail *b)
{
	char *email;

	email = gtk_entry_get_text(GTK_ENTRY(b->emailentry));
	
	/* FIXME : email search. not sure if even works; not important */
	if (connections)
		serv_dir_search(connections->data, "","","","","","","", email);
 
	destroy_dialog(NULL, b->window);
}

void show_find_info()
{
        GtkWidget *cancel;
        GtkWidget *ok;
        GtkWidget *label;
        GtkWidget *bbox;
        GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *fbox;
	GtkWidget *frame;

	struct findbyinfo *b = g_new0(struct findbyinfo, 1);
        b->window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_widget_set_usize(b->window, 350, 320);
	gtk_window_set_policy(GTK_WINDOW(b->window), FALSE, FALSE, TRUE);
	gtk_window_set_wmclass(GTK_WINDOW(b->window), "find_info", "Gaim");
	gtk_widget_show(b->window);

	dialogwindows = g_list_prepend(dialogwindows, b->window);


	frame = gtk_frame_new(_("Search for Buddy"));
	fbox = gtk_vbox_new(FALSE, 5);

	/* Build OK Button */

	ok = picture_button(b->window, _("OK"), ok_xpm);
	cancel = picture_button(b->window, _("Cancel"), cancel_xpm);

        bbox = gtk_hbox_new(TRUE, 10);
        vbox = gtk_vbox_new(FALSE, 2);

        b->firstentry = gtk_entry_new();
	b->middleentry = gtk_entry_new();
	b->lastentry = gtk_entry_new();
	b->maidenentry = gtk_entry_new();
	b->cityentry = gtk_entry_new();
	b->stateentry = gtk_entry_new();
	b->countryentry = gtk_entry_new();

        gtk_box_pack_start(GTK_BOX(bbox), ok, FALSE, FALSE, 10);
        gtk_box_pack_end(GTK_BOX(bbox), cancel, FALSE, FALSE, 10);

	/* Line 1 */
        label = gtk_label_new(_("First Name"));
	gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(hbox), b->firstentry, FALSE, FALSE, 5);

	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	/* Line 2 */

        label = gtk_label_new(_("Middle Name"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(hbox), b->middleentry, FALSE, FALSE, 5);

	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	/* Line 3 */

	label = gtk_label_new(_("Last Name"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(hbox), b->lastentry, FALSE, FALSE, 5);

	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	/* Line 4 */

	label = gtk_label_new(_("Maiden Name"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(hbox), b->maidenentry, FALSE, FALSE, 5);

	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	/* Line 5 */
	
	label = gtk_label_new(_("City"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(hbox), b->cityentry, FALSE, FALSE, 5);

	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	/* Line 6 */
        label = gtk_label_new(_("State"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(hbox), b->stateentry, FALSE, FALSE, 5);

	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	/* Line 7 */
        label = gtk_label_new(_("Country"));
        gtk_widget_show(label);
	
	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(hbox), b->countryentry, FALSE, FALSE, 5);

	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	/* Merge The Boxes */	

	gtk_container_add(GTK_CONTAINER(frame), vbox);
	gtk_box_pack_start(GTK_BOX(fbox), frame, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(fbox), bbox, FALSE, FALSE, 5);

        gtk_signal_connect(GTK_OBJECT(b->window), "destroy",
                           GTK_SIGNAL_FUNC(destroy_dialog), b->window);
        gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
                         GTK_SIGNAL_FUNC(destroy_dialog), b->window);
        gtk_signal_connect(GTK_OBJECT(ok), "clicked",
                           GTK_SIGNAL_FUNC(do_find_info), b);

        gtk_widget_show(ok);
        gtk_widget_show(cancel);
        gtk_widget_show(b->firstentry);
	gtk_widget_show(b->middleentry);
	gtk_widget_show(b->lastentry);
	gtk_widget_show(b->maidenentry);
	gtk_widget_show(b->cityentry);
	gtk_widget_show(b->stateentry);
	gtk_widget_show(b->countryentry);
        gtk_widget_show(bbox);                      
        gtk_widget_show(vbox);
	gtk_widget_show(frame);
	gtk_widget_show(fbox);

        gtk_window_set_title(GTK_WINDOW(b->window), _("Gaim - Find Buddy By Info"));
        gtk_window_set_focus(GTK_WINDOW(b->window), b->firstentry);
        gtk_container_add(GTK_CONTAINER(b->window), fbox);
        gtk_container_border_width(GTK_CONTAINER(b->window), 5);
        gtk_widget_realize(b->window);
        aol_icon(b->window->window);

        gtk_widget_show(b->window);
}        

void show_find_email()
{
        GtkWidget *label;
        GtkWidget *bbox;
        GtkWidget *vbox;
        GtkWidget *topbox;
	GtkWidget *frame;
	GtkWidget *button;

        struct findbyemail *b = g_new0(struct findbyemail, 1);
        b->window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_widget_set_usize(b->window, 240, 110);
	gtk_window_set_policy(GTK_WINDOW(b->window), FALSE, FALSE, TRUE);
	gtk_window_set_wmclass(GTK_WINDOW(b->window), "find_email", "Gaim");
	gtk_widget_show(b->window);
        dialogwindows = g_list_prepend(dialogwindows, b->window); 

	frame = gtk_frame_new(_("Search for Buddy"));

	bbox = gtk_hbox_new(TRUE, 10);
        topbox = gtk_hbox_new(FALSE, 5);
        vbox = gtk_vbox_new(FALSE, 5);

        b->emailentry = gtk_entry_new();

	/* Build OK Button */

	button = picture_button(b->window, _("OK"), ok_xpm);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
                           GTK_SIGNAL_FUNC(do_find_email), b);
	gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);	

	button = picture_button(b->window, _("Cancel"), cancel_xpm);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
                           GTK_SIGNAL_FUNC(destroy_dialog), b->window);
	gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);	

        label = gtk_label_new(_("Email"));
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(topbox), label, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(topbox), b->emailentry, FALSE, FALSE, 5);

        gtk_box_pack_start(GTK_BOX(vbox), topbox, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 5);

        gtk_signal_connect(GTK_OBJECT(b->window), "destroy",
                           GTK_SIGNAL_FUNC(destroy_dialog), b->window);
        gtk_signal_connect(GTK_OBJECT(b->emailentry), "activate",
                           GTK_SIGNAL_FUNC(do_find_email), b);

	gtk_container_add(GTK_CONTAINER(frame), vbox);
        gtk_widget_show(b->emailentry);
	gtk_widget_show(frame);
	gtk_widget_show(topbox);
        gtk_widget_show(bbox);
        gtk_widget_show(vbox);
        gtk_window_set_title(GTK_WINDOW(b->window), _("Gaim - Find Buddy By Email"));
        gtk_window_set_focus(GTK_WINDOW(b->window), b->emailentry);
        gtk_container_add(GTK_CONTAINER(b->window), frame);
        gtk_container_border_width(GTK_CONTAINER(b->window), 10);
        gtk_widget_realize(b->window);
        aol_icon(b->window->window);

        gtk_widget_show(b->window);
}

/*------------------------------------------------------*/
/* Link Dialog                                          */
/*------------------------------------------------------*/

void cancel_link(GtkWidget *widget, struct conversation *c)
{
	if (c->link) {
		set_state_lock(1);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->link), FALSE);
		set_state_lock(0);
	}	
	destroy_dialog(NULL, c->link_dialog);
	c->link_dialog = NULL;
}

void do_add_link(GtkWidget *widget, struct linkdlg *b)
{
	char *open_tag;
        char *urltext, *showtext;
	open_tag = g_malloc(2048);


	urltext = gtk_entry_get_text(GTK_ENTRY(b->url));
        showtext = gtk_entry_get_text(GTK_ENTRY(b->text));
	
	g_snprintf(open_tag, 2048, "<A HREF=\"%s\">%s", urltext, showtext);
	surround(b->entry, open_tag, "</A>");

	g_free(open_tag);
	destroy_dialog(NULL, b->window);
}


void show_add_link(GtkWidget *linky, struct conversation *c)
{
	GtkWidget *vbox;
	GtkWidget *bbox;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *frame;
	GtkWidget *fbox;

	if (!c->link_dialog) {
		struct linkdlg *b = g_new0(struct linkdlg, 1);
		c->link_dialog = gtk_window_new(GTK_WINDOW_DIALOG);
                gtk_window_set_wmclass(GTK_WINDOW(c->link_dialog),
                                       "add_link", "Gaim");
		dialogwindows = g_list_prepend(dialogwindows, c->link_dialog);

		gtk_widget_set_usize(c->link_dialog, 270, 165);
		gtk_window_set_policy(GTK_WINDOW(c->link_dialog), FALSE, FALSE, TRUE);
		gtk_widget_show(c->link_dialog);

		vbox = gtk_vbox_new(FALSE, 10);
		bbox = gtk_hbox_new(TRUE, 10);
		frame = gtk_frame_new(_("Insert Link"));
		fbox = gtk_vbox_new(FALSE, 5);

		/* Build OK Button */

		b->ok = picture_button(c->link_dialog, _("OK"), ok_xpm);
		b->cancel = picture_button(c->link_dialog, _("Cancel"), cancel_xpm);

		gtk_widget_set_usize(b->ok, 75, 30);
		gtk_widget_set_usize(b->cancel, 75, 30);

		gtk_box_pack_start(GTK_BOX(bbox), b->ok, FALSE, FALSE, 10);
		gtk_box_pack_end(GTK_BOX(bbox), b->cancel, FALSE, FALSE, 10);
		gtk_widget_show(bbox);

		b->url = gtk_entry_new();
		b->text = gtk_entry_new();

		hbox = gtk_hbox_new(FALSE, 5);
		label = gtk_label_new(_("URL"));
		gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
		gtk_box_pack_end(GTK_BOX(hbox), b->url, FALSE, FALSE, 5);
		gtk_widget_show(label);
		gtk_widget_show(hbox);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
		
		hbox = gtk_hbox_new(FALSE, 5);
		label = gtk_label_new(_("Description"));
		gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
		gtk_box_pack_end(GTK_BOX(hbox), b->text, FALSE, FALSE, 5);
		gtk_widget_show(label);
		gtk_widget_show(hbox);
		gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
		
		gtk_widget_show(b->url);
		gtk_widget_show(b->text);
		gtk_widget_show(frame);
		gtk_widget_show(fbox);

		gtk_container_add(GTK_CONTAINER(frame), vbox);
		gtk_box_pack_start(GTK_BOX(fbox), frame, FALSE, FALSE, 5);
		gtk_box_pack_start(GTK_BOX(fbox), bbox, TRUE, TRUE, 5);
		gtk_widget_show(vbox);

		gtk_signal_connect(GTK_OBJECT(c->link_dialog), "destroy",
				   GTK_SIGNAL_FUNC(cancel_link), c);
		gtk_signal_connect(GTK_OBJECT(b->cancel), "clicked",
				   GTK_SIGNAL_FUNC(cancel_link), c);
		gtk_signal_connect(GTK_OBJECT(b->ok), "clicked",
				   GTK_SIGNAL_FUNC(do_add_link), b);

		gtk_container_add(GTK_CONTAINER(c->link_dialog  ), fbox);
		gtk_container_border_width(GTK_CONTAINER(c->link_dialog  ), 10);
		gtk_window_set_title(GTK_WINDOW(c->link_dialog  ), _("GAIM - Add URL"));
		gtk_window_set_focus(GTK_WINDOW(c->link_dialog  ), b->url);
		b->window = c->link_dialog;
		b->toggle = linky;
		b->entry = c->entry;
                gtk_widget_realize(c->link_dialog);
		aol_icon(c->link_dialog->window);

	}

	gtk_widget_show(c->link_dialog);
	gdk_window_raise(c->link_dialog->window);
}


/*------------------------------------------------------*/
/* Color Selection Dialog                               */
/*------------------------------------------------------*/

static GtkWidget *fgcseld = NULL;
static GtkWidget *bgcseld = NULL;

void cancel_fgcolor(GtkWidget *widget, struct conversation *c)
{
 	if (c->fgcolorbtn && widget)
	{
		set_state_lock(1);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->fgcolorbtn), FALSE);
		set_state_lock(0);
	}
	dialogwindows = g_list_remove(dialogwindows, c->fg_color_dialog);
	gtk_widget_destroy(c->fg_color_dialog);
	c->fg_color_dialog = NULL;
}

void cancel_bgcolor(GtkWidget *widget, struct conversation *c)
{
 	if (c->bgcolorbtn && widget)
	{
		set_state_lock(1);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->bgcolorbtn), FALSE);
		set_state_lock(0);
	}
	dialogwindows = g_list_remove(dialogwindows, c->bg_color_dialog);
	gtk_widget_destroy(c->bg_color_dialog);
	c->bg_color_dialog = NULL;
}

void do_fgcolor(GtkWidget *widget, GtkColorSelection *colorsel)
{
	gdouble color[3];
	GdkColor text_color;
	struct conversation *c;
	char *open_tag;

	open_tag = g_malloc(30);

	gtk_color_selection_get_color (colorsel, color);

	c = gtk_object_get_user_data(GTK_OBJECT(colorsel));
	/* GTK_IS_EDITABLE(c->entry); huh? */

	text_color.red = ((guint16)(color[0]*65535))>>8;
	text_color.green = ((guint16)(color[1]*65535))>>8;
	text_color.blue = ((guint16)(color[2]*65535))>>8;
	
	c->fgcol = text_color;
	c->hasfg = 1;
	g_snprintf(open_tag, 23, "<FONT COLOR=\"#%02X%02X%02X\">", text_color.red, text_color.green, text_color.blue);
	surround(c->entry, open_tag, "</FONT>");
	sprintf(debug_buff,"#%02X%02X%02X\n", text_color.red, text_color.green, text_color.blue);
	debug_print(debug_buff);
	g_free(open_tag);
	cancel_fgcolor(NULL, c);
}

void do_bgcolor(GtkWidget *widget, GtkColorSelection *colorsel)
{
	gdouble color[3];
	GdkColor text_color;
	struct conversation *c;
	char *open_tag;

	open_tag = g_malloc(30);

	gtk_color_selection_get_color (colorsel, color);

	c = gtk_object_get_user_data(GTK_OBJECT(colorsel));
	/* GTK_IS_EDITABLE(c->entry); huh? */

	text_color.red = ((guint16)(color[0]*65535))>>8;
	text_color.green = ((guint16)(color[1]*65535))>>8;
	text_color.blue = ((guint16)(color[2]*65535))>>8;
	
	c->bgcol = text_color;
	c->hasbg = 1;
	g_snprintf(open_tag, 25, "<BODY BGCOLOR=\"#%02X%02X%02X\">", text_color.red, text_color.green, text_color.blue);
	surround(c->entry, open_tag, "</BODY>");
	sprintf(debug_buff,"#%02X%02X%02X\n", text_color.red, text_color.green, text_color.blue);
	debug_print(debug_buff);
	g_free(open_tag);
	cancel_bgcolor(NULL, c);
}

static void destroy_colorsel(GtkWidget *w, gpointer d)
{
	if (d) {
		gtk_widget_destroy(fgcseld);
		fgcseld = NULL;
	} else {
		gtk_widget_destroy(bgcseld);
		bgcseld = NULL;
	}
}

static void apply_color_dlg(GtkWidget *w, gpointer d)
{
	gdouble color[3];
	if ((int)d == 1) {
		gtk_color_selection_get_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(fgcseld)->colorsel), color);
		destroy_colorsel(NULL, (void *)1);

		fgcolor.red = ((guint16)(color[0]*65535))>>8;
		fgcolor.green = ((guint16)(color[1]*65535))>>8;
		fgcolor.blue = ((guint16)(color[2]*65535))>>8;
		update_color(NULL, pref_fg_picture);
	} else {
		gtk_color_selection_get_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(bgcseld)->colorsel), color);
		destroy_colorsel(NULL, (void *)0);

		bgcolor.red = ((guint16)(color[0]*65535))>>8;
		bgcolor.green = ((guint16)(color[1]*65535))>>8;
		bgcolor.blue = ((guint16)(color[2]*65535))>>8;
		update_color(NULL, pref_bg_picture);
	}
}

void show_fgcolor_dialog(struct conversation *c, GtkWidget *color)
{
	GtkWidget *colorsel;

	if (color == NULL) { /* we came from the prefs */
		if (fgcseld) return;
		fgcseld = gtk_color_selection_dialog_new(_("Select Text Color"));
		
		gtk_signal_connect(GTK_OBJECT(fgcseld), "delete_event", GTK_SIGNAL_FUNC(destroy_colorsel), (void *)1);
		gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(fgcseld)->cancel_button), "clicked", GTK_SIGNAL_FUNC(destroy_colorsel), (void *)1);
		gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(fgcseld)->ok_button), "clicked", GTK_SIGNAL_FUNC(apply_color_dlg), (void *)1);
		gtk_widget_realize(fgcseld);
		aol_icon(fgcseld->window);
		gtk_widget_show(fgcseld);
		gdk_window_raise(fgcseld->window);
		return;
	}

	if (!c->fg_color_dialog) {
		c->fg_color_dialog = gtk_color_selection_dialog_new(_("Select Text Color"));
		
		colorsel = GTK_COLOR_SELECTION_DIALOG(c->fg_color_dialog)->colorsel;

		gtk_object_set_user_data(GTK_OBJECT(colorsel), c);
		
		gtk_signal_connect(GTK_OBJECT(c->fg_color_dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_dialog), c);
		gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(c->fg_color_dialog)->ok_button), "clicked", GTK_SIGNAL_FUNC(do_fgcolor), colorsel);
		gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(c->fg_color_dialog)->cancel_button), "clicked", GTK_SIGNAL_FUNC(cancel_fgcolor), c);

		gtk_widget_realize(c->fg_color_dialog);
		aol_icon(c->fg_color_dialog->window);
	}

	gtk_widget_show(c->fg_color_dialog);
	gdk_window_raise(c->fg_color_dialog->window);
}

void show_bgcolor_dialog(struct conversation *c, GtkWidget *color)
{
	GtkWidget *colorsel;

	if (color == NULL) { /* we came from the prefs */
		if (bgcseld) return;
		bgcseld = gtk_color_selection_dialog_new(_("Select Background Color"));

		gtk_signal_connect(GTK_OBJECT(bgcseld), "delete_event", GTK_SIGNAL_FUNC(destroy_colorsel), NULL);
		gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(bgcseld)->cancel_button), "clicked", GTK_SIGNAL_FUNC(destroy_colorsel), NULL);
		gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(bgcseld)->ok_button), "clicked", GTK_SIGNAL_FUNC(apply_color_dlg), (void *)2);
		gtk_widget_realize(bgcseld);
		aol_icon(bgcseld->window);
		gtk_widget_show(bgcseld);
		gdk_window_raise(bgcseld->window);
		return;
	}

	if (!c->bg_color_dialog) {
		c->bg_color_dialog = gtk_color_selection_dialog_new(_("Select Text Color"));
		
		colorsel = GTK_COLOR_SELECTION_DIALOG(c->bg_color_dialog)->colorsel;

		gtk_object_set_user_data(GTK_OBJECT(colorsel), c);
		
		gtk_signal_connect(GTK_OBJECT(c->bg_color_dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_dialog), c);
		gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(c->bg_color_dialog)->ok_button), "clicked", GTK_SIGNAL_FUNC(do_bgcolor), colorsel);
		gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(c->bg_color_dialog)->cancel_button), "clicked", GTK_SIGNAL_FUNC(cancel_bgcolor), c);

		gtk_widget_realize(c->bg_color_dialog);
		aol_icon(c->bg_color_dialog->window);
	}

	gtk_widget_show(c->bg_color_dialog);
	gdk_window_raise(c->bg_color_dialog->window);
}

/*------------------------------------------------------------------------*/
/*  Font Selection Dialog                                                 */
/*------------------------------------------------------------------------*/

void cancel_font(GtkWidget *widget, struct conversation *c)
{	
	if (c->font && widget)
	{
		set_state_lock(1);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->font), FALSE);
		set_state_lock(0);
	}
	dialogwindows = g_list_remove(dialogwindows, c->font_dialog);
	gtk_widget_destroy(c->font_dialog);
	c->font_dialog = NULL;	
}

void apply_font(GtkWidget *widget, GtkFontSelection *fontsel)
{
	/* this could be expanded to include font size, weight, etc.
	   but for now only works with font face */
	int i, j = 0, k = 0;
	char *fontname;
	char font[64];
	struct conversation *c = gtk_object_get_user_data(GTK_OBJECT(fontsel));
	
	if (c)
	{
		fontname = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(fontsel));

		for (i = 0; i < strlen(fontname); i++)
		{
			if (fontname[i] == '-')
			{
				if (++j > 2)
					break;	
			}		
			else if (j == 2)
				font[k++] = fontname[i];
		}
		font[k] = '\0';

		sprintf(debug_buff, "Setting font face %s\n", font);
		debug_print(debug_buff);

		set_font_face(font, c);
	}
	else
	{
		fontname = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(fontsel));
	
		for (i = 0; i < strlen(fontname); i++)
		{
			if (fontname[i] == '-')
			{
				if (++j > 2)
					break;	
			}		
			else if (j == 2)
				fontface[k++] = fontname[i];
		}
		fontface[k] = '\0';
	}
	
	cancel_font(NULL, c);
}

static GtkWidget *fontseld;

void destroy_fontsel(GtkWidget *w, gpointer d) {
	gtk_widget_destroy(fontseld);
	fontseld = NULL;
}

void apply_font_dlg(GtkWidget *w, GtkWidget *f) {
	int i, j = 0, k = 0;
	char *fontname;

	fontname = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(fontseld));
	destroy_fontsel(0, 0);
	for (i = 0; i < strlen(fontname); i++) {
		if (fontname[i] == '-') {
			if (++j > 2) break;
		} else if (j == 2)
			fontface[k++] = fontname[i];
	}
	fontface[k] = '\0';
}

void show_font_dialog(struct conversation *c, GtkWidget *font)
{

	if (!font) { /* we came from the prefs dialog */
		if (fontseld) return;
		fontseld = gtk_font_selection_dialog_new(_("Select Font"));
		gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(fontseld), DEFAULT_FONT_NAME);

		gtk_object_set_user_data(GTK_OBJECT(fontseld), NULL);
		gtk_signal_connect(GTK_OBJECT(fontseld), "delete_event", GTK_SIGNAL_FUNC(destroy_fontsel), NULL);
		gtk_signal_connect(GTK_OBJECT(GTK_FONT_SELECTION_DIALOG(fontseld)->cancel_button), "clicked", GTK_SIGNAL_FUNC(destroy_fontsel), NULL);
		gtk_signal_connect(GTK_OBJECT(GTK_FONT_SELECTION_DIALOG(fontseld)->ok_button), "clicked", GTK_SIGNAL_FUNC(apply_font_dlg), NULL);
		gtk_widget_realize(fontseld);
		aol_icon(fontseld->window);
		gtk_widget_show(fontseld);
		gdk_window_raise(fontseld->window);
		return;
	}
	
	if (!c->font_dialog)
	{
		c->font_dialog = gtk_font_selection_dialog_new(_("Select Font"));

		if (font)
			gtk_object_set_user_data(GTK_OBJECT(c->font_dialog), c);
		else
			gtk_object_set_user_data(GTK_OBJECT(c->font_dialog), NULL);
			
		gtk_font_selection_dialog_set_font_name((GtkFontSelectionDialog *)c->font_dialog, DEFAULT_FONT_NAME);	
		
		gtk_signal_connect(GTK_OBJECT(c->font_dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_dialog), c);
		gtk_signal_connect(GTK_OBJECT(GTK_FONT_SELECTION_DIALOG(c->font_dialog)->ok_button), "clicked", GTK_SIGNAL_FUNC(apply_font), c->font_dialog);
		gtk_signal_connect(GTK_OBJECT(GTK_FONT_SELECTION_DIALOG(c->font_dialog)->cancel_button), "clicked", GTK_SIGNAL_FUNC(cancel_font), c);
	
		gtk_font_selection_dialog_set_font_name((GtkFontSelectionDialog *)c->font_dialog, DEFAULT_FONT_NAME);		
		
		gtk_widget_realize(c->font_dialog);
	
		aol_icon(c->font_dialog->window);
	}	
	gtk_widget_show(c->font_dialog);
	gdk_window_raise(c->font_dialog->window);
}

/*------------------------------------------------------------------------*/
/*  The dialog for import/export                                          */
/*------------------------------------------------------------------------*/

/* see if a buddy list cache file for this user exists */

gboolean
bud_list_cache_exists(struct gaim_connection *gc)
{
	gboolean ret = FALSE;
	char path[PATHSIZE];
	char *file;
	struct stat sbuf;
	char g_screenname[64];
	int i;

	for (i = 0; i < strlen(gc->username); i++)
		g_screenname[i] = toupper(gc->username[i]);
	g_screenname[i] = '\0';

	file = getenv( "HOME" );
	if ( file != (char *) NULL ) {
	       	sprintf( path, "%s/.gaim/%s.blist", file, g_screenname);
		if ( !stat(path, &sbuf) ) {
			sprintf(debug_buff, "%s exists.\n", path);
			debug_print(debug_buff);
			ret = TRUE;
		} else {
			sprintf(debug_buff, "%s does not exist.\n", path);
			debug_print(debug_buff);
		}
	}
	return ret;
}

/* if dummy is 0, save to ~/.gaim/screenname.blist, where screenname is each
 * signed in user. Else, let user choose */

void do_export(GtkWidget *w, void *dummy)
{
        FILE *f;
	gint show_dialog = (int) dummy;
        char *buf = g_malloc(BUF_LONG);
        char *file;
	char path[PATHSIZE];

	if ( show_dialog == 1 ) {
		file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(exportdialog));
		strncpy( path, file, PATHSIZE - 1 );
		if (file_is_dir(path, exportdialog)) {
			g_free (buf);
			return;
		}
		if ((f = fopen(path,"w"))) {
			toc_build_config(connections->data, buf, 8192 - 1, TRUE);
			fprintf(f, "%s\n", buf);
			fclose(f);
			chmod(buf, S_IRUSR | S_IWUSR);
		} else {
			g_snprintf(buf, BUF_LONG / 2, _("Error writing file %s"), file);
			do_error_dialog(buf, _("Error"));
		}
        	destroy_dialog(NULL, exportdialog);
        	exportdialog = NULL;
	} else {
		GSList *c = connections;
		struct gaim_connection *g;
		char g_screenname[64];
		int i;

		file = getenv( "HOME" );
		if ( file != (char *) NULL ) {
			FILE *dir;
			sprintf(buf, "%s/.gaim/", file);
			dir = fopen(buf, "r");
			if (!dir)
				mkdir(buf, S_IRUSR | S_IWUSR | S_IXUSR);
			else
				fclose(dir);

			while (c) {
				g = (struct gaim_connection *)c->data;

				for (i = 0; i < strlen(g->username); i++)
					g_screenname[i] = toupper(g->username[i]);
				g_screenname[i] = '\0';
				sprintf( path, "%s/.gaim/%s.blist", file, g_screenname);
				if ((f = fopen(path,"w"))) {
					sprintf(debug_buff, "writing %s\n", path);
					debug_print(debug_buff);
					toc_build_config(g, buf, 8192 - 1, TRUE);
					fprintf(f, "%s\n", buf);
					fclose(f);
					chmod(buf, S_IRUSR | S_IWUSR);
				} else {
					sprintf(debug_buff, "unable to write %s\n", path);
					debug_print(debug_buff);
				}

				c = c->next;
			}
		} else return;
	}

        g_free(buf);
        
}

	
void show_export_dialog()
{
        char *buf = g_malloc(BUF_LEN);
        if (!exportdialog) {
                exportdialog = gtk_file_selection_new(_("Gaim - Export Buddy List"));

                gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(exportdialog));

                g_snprintf(buf, BUF_LEN - 1, "%s/gaim.buddy", getenv("HOME"));
                
                gtk_file_selection_set_filename(GTK_FILE_SELECTION(exportdialog), buf);
                gtk_signal_connect(GTK_OBJECT(exportdialog), "destroy",
                                   GTK_SIGNAL_FUNC(destroy_dialog), exportdialog);
                
                gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(exportdialog)->ok_button),
                                   "clicked", GTK_SIGNAL_FUNC(do_export), (void*)1);
                gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(exportdialog)->cancel_button),
                                   "clicked", GTK_SIGNAL_FUNC(destroy_dialog), exportdialog);
                

	}

	g_free(buf);

        gtk_widget_show(exportdialog);
        gdk_window_raise(exportdialog->window);

}

/* if gc is non-NULL, then import from ~/.gaim/gc->username.blist, else let user
   choose */

void do_import(GtkWidget *w, struct gaim_connection *gc)
{
        char *buf = g_malloc(BUF_LONG);
        char *buf2;
        char *first = g_malloc(64);
	char *file;
	char path[PATHSIZE];
	char g_screenname[64];
	int i;
        FILE *f;

        if ( !gc ) {
        	file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(importdialog));
                strncpy( path, file, PATHSIZE - 1 );
		if (file_is_dir(path, importdialog)) {
			g_free (buf);
			g_free (first);
			return;
		}
		/* FIXME : import buddy list file. moderately important */
		gc = connections->data;
        }
        else {
		for (i = 0; i < strlen(gc->username); i++)
			g_screenname[i] = toupper(gc->username[i]);
		g_screenname[i] = '\0';

                file = getenv( "HOME" );
                if ( file != (char *) NULL )
                        sprintf( path, "%s/.gaim/%s.blist", file, g_screenname);
                else
			return;
        }

        if (!(f = fopen(path,"r"))) {
		if ( !gc ) {
                	g_snprintf(buf, BUF_LONG / 2, _("Error reading file %s"), file);
                	do_error_dialog(buf, _("Error"));
                	destroy_dialog(NULL, importdialog);
                	importdialog = NULL;
		}
		sprintf(debug_buff, "Unable to open %s.\n", path);
		debug_print(debug_buff);
                g_free(buf);
		g_free(first);
                return;
        }
                
        fgets(first, 64, f);

	/* AIM 4 buddy list */
        if (!strcasecmp(first, "Config {\n")) {
		debug_print("aim 4\n");
		rewind(f);
		translate_blt (f, buf);
		sprintf(debug_buff, "%s\n", buf);
		debug_print(debug_buff);
		buf2 = buf;
		buf = g_malloc(8193);
                g_snprintf(buf, 8192, "toc_set_config {%s}\n", buf2);
                g_free(buf2);
	/* AIM 3 buddy list */
	} else if (strstr(first, "group") != NULL) {
		debug_print("aim 3\n");
		rewind(f);
		translate_lst (f, buf);
		sprintf(debug_buff, "%s\n", buf);
		debug_print(debug_buff);
		buf2 = buf;
		buf = g_malloc(8193);
                g_snprintf(buf, 8192, "toc_set_config {%s}\n", buf2);
                g_free(buf2);
	/* GAIM buddy list - no translation */
        } else if (first[0] == 'm') {
		rewind(f);
		fread(buf, BUF_LONG, 1, f);
		buf2 = buf;
		buf = g_malloc(8193);
                g_snprintf(buf, 8192, "toc_set_config {%s}\n", buf2);
                g_free(buf2);
	/* Something else */
        } else {
		if ( !gc ) {
                	destroy_dialog(NULL, importdialog);
                	importdialog = NULL;
		}
		g_free(buf);
		g_free(first);
		fclose( f );
                return;
	}

        parse_toc_buddy_list(gc, buf, 1);

        build_edit_tree();

	fclose( f );

	if ( !gc ) {
		/* save what we just did to cache */

		do_export( (GtkWidget *) NULL, 0 );
               	destroy_dialog(NULL, importdialog);
               	importdialog = NULL;
	} 

        g_free(buf);
        g_free(first);
}

void show_import_dialog()
{
        char *buf = g_malloc(BUF_LEN);
        if (!importdialog) {
                importdialog = gtk_file_selection_new(_("Gaim - Import Buddy List"));

                gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(importdialog));

                g_snprintf(buf, BUF_LEN - 1, "%s/", getenv("HOME"));
                
                gtk_file_selection_set_filename(GTK_FILE_SELECTION(importdialog), buf);
                gtk_signal_connect(GTK_OBJECT(importdialog), "destroy",
                                   GTK_SIGNAL_FUNC(destroy_dialog), importdialog);
                
                gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(importdialog)->ok_button),
                                   "clicked", GTK_SIGNAL_FUNC(do_import), NULL);
                gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(importdialog)->cancel_button),
                                   "clicked", GTK_SIGNAL_FUNC(destroy_dialog), importdialog);
                

        }

	g_free(buf);
        gtk_widget_show(importdialog);
        gdk_window_raise(importdialog->window);
}


/*------------------------------------------------------------------------*/
/*  The dialog for new away messages                                      */
/*------------------------------------------------------------------------*/

void save_away_mess(GtkWidget *widget, struct create_away *ca)
{
	struct away_message *am;
	guint text_len;
	gchar *away_message;
	
	if (!ca->mess)
		am = g_new0(struct away_message, 1);
	else
		am = ca->mess;
	g_snprintf(am->name, sizeof(am->name), "%s", gtk_entry_get_text(GTK_ENTRY(ca->entry)));
	text_len = gtk_text_get_length(GTK_TEXT(ca->text));
	away_message = gtk_editable_get_chars(GTK_EDITABLE(ca->text), 0, text_len);

	g_snprintf(am->message, sizeof(am->message), "%s", away_message);
	g_free(away_message);

	if (!ca->mess) {
		away_messages = g_slist_append(away_messages, am);
		if (GTK_TOGGLE_BUTTON(ca->checkbx)->active)
			do_away_message(NULL, am);
	}

	save_prefs();
	do_away_menu();	
	if (prefs_away_list != NULL)
		gtk_list_select_item(GTK_LIST(prefs_away_list), g_slist_index(away_messages, am));
	
	destroy_dialog(NULL, ca->window);
	
	return;
}
	
void create_away_mess(GtkWidget *widget, void *dummy)
{
	GtkWidget *bbox;
	GtkWidget *hbox;
	GtkWidget *titlebox;
	GtkWidget *tbox;
	GtkWidget *sw;
	GtkWidget *label;
	GtkWidget *frame;
	GtkWidget *fbox;
	GtkWidget *button;

        struct create_away *ca = g_new0(struct create_away, 1);
        
	/* Set up window */
	ca->window = gtk_window_new(GTK_WINDOW_DIALOG);
 	gtk_widget_set_usize(ca->window, 275, 200); 
	gtk_container_border_width(GTK_CONTAINER(ca->window), 5);
	gtk_window_set_policy(GTK_WINDOW(ca->window), FALSE, FALSE, TRUE);
	gtk_window_set_wmclass(GTK_WINDOW(ca->window), "away_mess", "Gaim");
	gtk_window_set_title(GTK_WINDOW(ca->window), _("Gaim - New away message"));
	gtk_widget_show(ca->window);
	gtk_signal_connect(GTK_OBJECT(ca->window),"delete_event",
		           GTK_SIGNAL_FUNC(destroy_dialog), ca->window);

	/* Set up our frame */

	frame = gtk_frame_new(_("New away message"));

	/* set up container boxes */
	bbox = gtk_hbox_new(FALSE, 0);
	fbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	titlebox = gtk_hbox_new(FALSE, 0);
	tbox = gtk_vbox_new(FALSE, 0);

	/* Make a label for away entry */
	label = gtk_label_new(_("Away title: "));
	gtk_box_pack_start(GTK_BOX(titlebox), label, TRUE, TRUE, 5);
	gtk_widget_show(label);

	/* make away title entry */
	ca->entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(titlebox), ca->entry, TRUE, TRUE, 5);

	sw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_widget_show(sw);

	/* create and format text box */
	ca->text = gtk_text_new(NULL, NULL);
	gtk_text_set_word_wrap(GTK_TEXT(ca->text), TRUE);
	gtk_text_set_editable(GTK_TEXT(ca->text), TRUE );
	gtk_container_add(GTK_CONTAINER(sw), ca->text);
	gtk_widget_show(ca->text);
	gtk_box_pack_start(GTK_BOX(bbox), sw, TRUE, TRUE, 5);   

	button = picture_button(ca->window, _("Save"), save_xpm);
	gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(save_away_mess), ca);

	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 5);
	
	button = picture_button(ca->window, _("Cancel"), cancel_xpm);
	gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(destroy_dialog), ca->window);
	gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 5);
	
	/* pack boxes where they belong */
	gtk_box_pack_start(GTK_BOX(fbox), titlebox, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(fbox), bbox, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(frame), fbox);
	gtk_container_set_border_width(GTK_CONTAINER(frame), 5);
	gtk_box_pack_start(GTK_BOX(tbox), frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(tbox), hbox, TRUE, FALSE, 5);
	
	gtk_container_add(GTK_CONTAINER(ca->window), tbox);

	if (dummy && GTK_LIST(prefs_away_list)->selection) {
		GtkWidget *item = GTK_LIST(prefs_away_list)->selection->data;
		struct away_message *amt = gtk_object_get_user_data(GTK_OBJECT(item));
		int pos = 0;
		gtk_entry_set_text(GTK_ENTRY(ca->entry), amt->name);
		gtk_editable_insert_text(GTK_EDITABLE(ca->text), amt->message, strlen(amt->message), &pos);
		ca->mess = amt;
	} else {
		/* Checkbox for showing away msg */
		ca->checkbx = gtk_check_button_new_with_label(_("Make Away Now"));
		gtk_box_pack_start(GTK_BOX(fbox), ca->checkbx, TRUE, TRUE, 5);
		gtk_widget_show(ca->checkbx);
	}

	/* let the world see what we have done. */
	gtk_widget_show(label);
	gtk_widget_show(ca->entry);
	gtk_widget_show(titlebox);
	gtk_widget_show(hbox);
	gtk_widget_show(tbox);
	gtk_widget_show(bbox);
	gtk_widget_show(fbox);
	gtk_widget_show(frame);

        gtk_widget_realize(ca->window);
        aol_icon(ca->window->window);
	gtk_widget_grab_focus(ca->entry);
}

/* smiley dialog */

void close_smiley_dialog(GtkWidget *widget, struct conversation *c)
{	
	if (c->smiley)
	{
		set_state_lock(1);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(c->smiley), FALSE);
		set_state_lock(0);
	}
	dialogwindows = g_list_remove(dialogwindows, c->smiley_dialog);
	gtk_widget_destroy(c->smiley_dialog);
	c->smiley_dialog = NULL;	
}

void set_smiley_array(GtkWidget *widget, int smiley_type)
{
	int i;

	for (i = 0; i < FACE_TOTAL; i++)
		smiley_array[i] = 0;
		
	smiley_array[smiley_type] = 1;
	
	return;
}

void insert_smiley_text(GtkWidget *widget, struct conversation *c)
{
	char *smiley_text;
	int i;
	
	for (i = 0; i < FACE_TOTAL; i++)
		if (smiley_array[i] == 1)
			break;

	switch (i)
	{
		case (FACE_ANGEL): smiley_text = g_strndup("O:-)", strlen("O:-)")); break;
		case (FACE_BIGSMILE): smiley_text = g_strndup(":-D", strlen(":-D")); break;
		case (FACE_BURP): smiley_text = g_strndup(":-!", strlen(":-!")); break;
		case (FACE_CROSSEDLIPS): smiley_text = g_strndup(":-X", strlen(":-X")); break;
		case (FACE_CRY): smiley_text = g_strndup(":'(", strlen(":'(")); break;
		case (FACE_EMBARRASSED): smiley_text = g_strndup(":-[", strlen(":-[")); break;
		case (FACE_KISS): smiley_text = g_strndup(":-*", strlen(":-*")); break;
		case (FACE_MONEYMOUTH): smiley_text = g_strndup(":-$", strlen(":-$")); break;
		case (FACE_SAD): smiley_text = g_strndup(":-(", strlen(":-(")); break;
		case (FACE_SCREAM): smiley_text = g_strndup("=-O", strlen("=-O")); break;
		case (FACE_SMILE): smiley_text = g_strndup(":-)", strlen(":-)")); break;
		case (FACE_SMILE8): smiley_text = g_strndup("8-)", strlen("8-)")); break;
		case (FACE_THINK): smiley_text = g_strndup(":-/", strlen(":-/")); break;
		case (FACE_TONGUE): smiley_text = g_strndup(":-P", strlen(":-p")); break;
		case (FACE_WINK): smiley_text = g_strndup(";-)", strlen(";-)")); break;
		case (FACE_YELL): smiley_text = g_strndup(">:o", strlen(">:o")); break;
		default: smiley_text = g_strndup(":-)", strlen(":-)")); break;
	}
	
	/* surround(c->entry, smiley_text, ""); */

	if (GTK_EDITABLE(c->entry)->has_selection) {
		int finish = GTK_EDITABLE(c->entry)->selection_end_pos;
		gtk_editable_insert_text(GTK_EDITABLE(c->entry),
					 smiley_text, strlen(smiley_text), &finish);
	} else {
		int pos = GTK_EDITABLE(c->entry)->current_pos;
		gtk_editable_insert_text(GTK_EDITABLE(c->entry),
					 smiley_text, strlen(smiley_text), &pos);
	}

	g_free(smiley_text);
	
	close_smiley_dialog(NULL, c);
		
	return;
}

static void toolbar_add_smiley(struct conversation *c, GtkWidget *bar, char **xpm, GtkWidget *win, int face)
{
	GtkWidget *tpm;
	GdkBitmap *mask;
	GdkPixmap *dpm;
	GtkWidget *button;

	dpm = gdk_pixmap_create_from_xpm_d(win->window, &mask, &win->style->white, xpm);
	tpm = gtk_pixmap_new(dpm, mask);
	gtk_widget_show(tpm);
	gdk_pixmap_unref(dpm);
	gdk_bitmap_unref(mask);
	button = gtk_toolbar_append_element(GTK_TOOLBAR(bar), GTK_TOOLBAR_CHILD_BUTTON, NULL, NULL, NULL, NULL, tpm, GTK_SIGNAL_FUNC(set_smiley_array), (int *)face);
	gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(insert_smiley_text), c);

	/* these look really weird with borders */
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
}
	
void show_smiley_dialog(struct conversation *c, GtkWidget *widget)
{
	GtkWidget *dialog;
	GtkWidget *vbox, *smiley_box_1, *smiley_box_2, *smiley_box_3, *smiley_box_4;
	GtkWidget *win;
	GtkWidget *bbox;

	if (c->smiley_dialog)
		return;

	win = c->window;
		
	dialog = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_policy(GTK_WINDOW(dialog), FALSE, FALSE, TRUE);
	gtk_window_set_wmclass(GTK_WINDOW(dialog), "smiley_dialog", "Gaim");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);

	/* setup boxes */
	vbox = gtk_vbox_new(TRUE, 5);
	bbox = gtk_hbox_new(FALSE, 5);

	smiley_box_1 = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	smiley_box_2 = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	smiley_box_3 = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	smiley_box_4 = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);

	/* setup buttons */

	/* pack buttons */
	toolbar_add_smiley(c, smiley_box_1, angel_xpm, win, FACE_ANGEL);
	toolbar_add_smiley(c, smiley_box_1, bigsmile_xpm, win, FACE_BIGSMILE);
	toolbar_add_smiley(c, smiley_box_1, burp_xpm, win, FACE_BURP);
	toolbar_add_smiley(c, smiley_box_1, crossedlips_xpm, win, FACE_CROSSEDLIPS);

	toolbar_add_smiley(c, smiley_box_2, cry_xpm, win, FACE_CRY);
	toolbar_add_smiley(c, smiley_box_2, embarrassed_xpm, win, FACE_EMBARRASSED);
	toolbar_add_smiley(c, smiley_box_2, kiss_xpm, win, FACE_KISS);
	toolbar_add_smiley(c, smiley_box_2, moneymouth_xpm, win, FACE_MONEYMOUTH);

	toolbar_add_smiley(c, smiley_box_3, sad_xpm, win, FACE_SAD);
	toolbar_add_smiley(c, smiley_box_3, scream_xpm, win, FACE_SCREAM);
	toolbar_add_smiley(c, smiley_box_3, smile_xpm, win, FACE_SMILE);
	toolbar_add_smiley(c, smiley_box_3, smile8_xpm, win, FACE_SMILE8);

	toolbar_add_smiley(c, smiley_box_4, think_xpm, win, FACE_THINK);
	toolbar_add_smiley(c, smiley_box_4, tongue_xpm, win, FACE_TONGUE);
	toolbar_add_smiley(c, smiley_box_4, wink_xpm, win, FACE_WINK);
	toolbar_add_smiley(c, smiley_box_4, yell_xpm, win, FACE_YELL);

	/* pack containers */
	gtk_box_pack_start(GTK_BOX(vbox), smiley_box_1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), smiley_box_2, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), smiley_box_3, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), smiley_box_4, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(dialog), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 5);

	/* connect signals */
	gtk_object_set_user_data(GTK_OBJECT(dialog), "smiley dialog");
	gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_dialog), c);
	
	/* show everything */
	gtk_window_set_title(GTK_WINDOW(dialog), _("Smile!"));
	gtk_widget_show_all(dialog);

	c->smiley_dialog = dialog;
		
	return;
}

static void do_alias(GtkWidget *w, struct buddy_show *b)
{
	GSList *c = b->connlist;
	struct gaim_connection *g;
	struct buddy *n;
	g_free(b->show);
	b->show = g_strdup(gtk_entry_get_text(GTK_ENTRY(aliasname)));
	gtk_label_set(GTK_LABEL(b->label), b->show);
	while (c) {
		g = (struct gaim_connection *)c->data;
		n = find_buddy(g, b->name);
		if (n) g_snprintf(n->show, sizeof(n->show), "%s", b->show);
		c = c->next;
	}
	do_export(0, 0);
	destroy_dialog(aliasdlg, aliasdlg);
}

void alias_dialog(struct buddy_show *b)
{
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *bbox;
	GtkWidget *cancel;
	static GtkWidget *add = NULL;
	GtkWidget *label;
        GtkWidget *topbox;
	static int a, c;

	if (aliasdlg) {
		gtk_entry_set_text(GTK_ENTRY(aliasentry), b->name);
		gtk_widget_show(aliasdlg);
		gtk_signal_disconnect(GTK_OBJECT(add), a);
		gtk_signal_disconnect(GTK_OBJECT(aliasname), c);
		a = gtk_signal_connect(GTK_OBJECT(add), "clicked",
				   GTK_SIGNAL_FUNC(do_alias), b);
		c = gtk_signal_connect(GTK_OBJECT(aliasname), "activate",
				   GTK_SIGNAL_FUNC(do_alias), b);
		return;
	}

        aliasdlg = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_policy(GTK_WINDOW(aliasdlg), FALSE, FALSE, TRUE);
	gtk_window_set_wmclass(GTK_WINDOW(aliasdlg), "alias_dialog", "Gaim");
	gtk_widget_realize(aliasdlg);
	dialogwindows = g_list_prepend(dialogwindows, aliasdlg);

	bbox = gtk_hbox_new(TRUE, 10);
        topbox = gtk_hbox_new(FALSE, 5);
        vbox = gtk_vbox_new(FALSE, 5);

        aliasentry = gtk_entry_new();
	aliasname = gtk_entry_new();
        /* Put the buttons in the box */

	add = picture_button(aliasdlg, _("Alias"), add_xpm);

	cancel = picture_button(aliasdlg, _("Cancel"), cancel_xpm);

        gtk_box_pack_start(GTK_BOX(bbox), add, FALSE, FALSE, 5);
        gtk_box_pack_end(GTK_BOX(bbox), cancel, FALSE, FALSE, 5);

	frame = gtk_frame_new(NULL);
	gtk_frame_set_label(GTK_FRAME(frame), _("Alias Buddy"));

        label = gtk_label_new(_("Buddy"));
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(topbox), label, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(topbox), aliasentry, FALSE, FALSE, 5);
	gtk_entry_set_editable(GTK_ENTRY(aliasentry), FALSE);
	label = gtk_label_new(_("Alias"));
	gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(topbox), label, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(topbox), aliasname, FALSE, FALSE, 5);

	gtk_entry_set_text(GTK_ENTRY(aliasentry), b->name);
	gtk_entry_set_text(GTK_ENTRY(aliasname), b->show);

        /* And the boxes in the box */
        gtk_box_pack_start(GTK_BOX(vbox), topbox, TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), bbox, TRUE, TRUE, 5);

        /* Handle closes right */
        gtk_signal_connect(GTK_OBJECT(aliasdlg), "destroy",
                           GTK_SIGNAL_FUNC(destroy_dialog), aliasdlg);
        gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
                           GTK_SIGNAL_FUNC(destroy_dialog), aliasdlg);
        a = gtk_signal_connect(GTK_OBJECT(add), "clicked",
                           GTK_SIGNAL_FUNC(do_alias), b);
	c = gtk_signal_connect(GTK_OBJECT(aliasname), "activate",
			   GTK_SIGNAL_FUNC(do_alias), b);
        /* Finish up */
        gtk_widget_show(add);
        gtk_widget_show(cancel);
        gtk_widget_show(aliasentry);
        gtk_widget_show(aliasname);
        gtk_widget_show(topbox);
        gtk_widget_show(bbox);
        gtk_widget_show(vbox);
	gtk_widget_show(frame);
        gtk_window_set_title(GTK_WINDOW(aliasdlg), _("Gaim - Alias Buddy"));
        gtk_window_set_focus(GTK_WINDOW(aliasdlg), aliasentry);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
        gtk_container_add(GTK_CONTAINER(aliasdlg), frame);
	gtk_container_set_border_width(GTK_CONTAINER(aliasdlg), 5);
        gtk_widget_realize(aliasdlg);
        aol_icon(aliasdlg->window);

	gtk_widget_show(aliasdlg);
}
