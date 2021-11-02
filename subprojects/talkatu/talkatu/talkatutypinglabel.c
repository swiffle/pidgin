/*
 * Talkatu - GTK widgets for chat applications
 * Copyright (C) 2017-2020 Gary Kramlich <grim@reaperworld.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include <glib/gi18n-lib.h>

#include <gtk/gtk.h>
#include <pango/pango.h>

#include <talkatu/talkatutypinglabel.h>

/**
 * SECTION:talkatutypinglabel
 * @Title: TypingLabel
 * @Short_description: A GtkLabel that displays who's typing
 *
 * This widget implements a simple interface for displaying who is currently
 * typing.  You can call talkatu_typing_label_start_typing() whenever you
 * receive a typing notification and #TalkatuTypingLabel will aggregate it for
 * you.
 *
 * If there are no new calls to talkatu_typing_label_start_typing() for a
 * specific user, then they will be removed after a timeout of 30 seconds.
 *
 * Usernames are passed in as strings and their default display can be
 * overridden by connecting to the #TalkatuTypingLabel::changed signal.
 */

/**
 * TALKATU_TYPE_TYPING_LABEL:
 *
 * The standard _get_type macro for #TalkatuTypingLabel.
 */

/**
 * TalkatuTypingLabel:
 *
 * A #GtkLabel subclass that keeps track of who's typing.
 */
typedef struct {
	GtkLabel parent;

	GHashTable *typers;
} TalkatuTypingLabelPrivate;

/**
 * TalkatuTypingLabelClass:
 * @changed: The changed vfunc is called to update the text in the widget.  The
 *           default handler can be overridden to customize the text.
 *
 * The backing class to a #TalkatuTypingLabel.
 */

typedef struct {
	TalkatuTypingLabel *label;
	gchar *who;
} TalkatuTypingLabelTimeoutData;

enum {
	SIG_CHANGED,
	LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = {0, };

G_DEFINE_TYPE_WITH_PRIVATE(TalkatuTypingLabel, talkatu_typing_label, GTK_TYPE_LABEL)

#define TALKATU_TYPING_LABEL_TIMEOUT (30)

/******************************************************************************
 * Callbacks
 *****************************************************************************/
static gboolean
talkatu_typing_label_timeout_cb(gpointer d) {
	TalkatuTypingLabelTimeoutData *data = (TalkatuTypingLabelTimeoutData *)d;

	talkatu_typing_label_finish_typing(data->label, data->who);

	/* we return G_SOURCE_CONTINUE because talkatu_typing_label_finish_typing
	 * will remove the source and we don't have a way to tell it that we
	 * removed it.  So it's best to let it remove it and we just won't get
	 * called again.
	 */
	return G_SOURCE_CONTINUE;
}

/******************************************************************************
 * Helpers
 *****************************************************************************/
static void
talkatu_typing_label_typer_value_free(gpointer data) {
	g_source_remove(GPOINTER_TO_UINT(data));
}

/******************************************************************************
 * Default Handlers
 *****************************************************************************/
static void
talkatu_typing_label_default_changed(TalkatuTypingLabel *label,
                                     GList *typers)
{
	gint n_typers = 0;

	if(typers == NULL) {
		gtk_label_set_text(GTK_LABEL(label), "");

		return;
	}

	n_typers = g_list_length(typers);
	if(n_typers > 3) {
		gtk_label_set_text(GTK_LABEL(label),
		                   _("Several people are typing..."));
	} else {
		gchar *text = NULL;

		if(n_typers == 1) {
			text = g_strdup_printf(_("%s is typing..."), (gchar *)typers->data);
		} else if(n_typers == 2) {
			text = g_strdup_printf(_("%s and %s are typing..."),
			                       (gchar *)typers->data,
			                       (gchar *)typers->next->data);
		} else {
			text = g_strdup_printf(_("%s, %s, and %s are typing..."),
			                       (gchar *)typers->data,
			                       (gchar *)typers->next->data,
			                       (gchar *)typers->next->next->data);
		}

		gtk_label_set_text(GTK_LABEL(label), text);
		g_free(text);
	}
}

/******************************************************************************
 * GObject Implementation
 *****************************************************************************/
static void
talkatu_typing_label_init(TalkatuTypingLabel *label) {
	TalkatuTypingLabelPrivate *priv = NULL;

	/* set our xalign to 0 to make it left justified... */
	gtk_label_set_xalign(GTK_LABEL(label), 0);

	priv = talkatu_typing_label_get_instance_private(TALKATU_TYPING_LABEL(label));

	priv->typers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
	                                     talkatu_typing_label_typer_value_free);
}

static void
talkatu_typing_label_finalize(GObject *obj) {
	TalkatuTypingLabelPrivate *priv = NULL;

	priv = talkatu_typing_label_get_instance_private(TALKATU_TYPING_LABEL(obj));

	g_hash_table_destroy(priv->typers);

	G_OBJECT_CLASS(talkatu_typing_label_parent_class)->finalize(obj);
}

static void
talkatu_typing_label_class_init(TalkatuTypingLabelClass *klass) {
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);

	obj_class->finalize = talkatu_typing_label_finalize;

	klass->changed = talkatu_typing_label_default_changed;

	/**
	 * TalkatuTypingLabel::changed:
	 * @talkatutypinglabel: The #TalkatuTypingLabel instance.
	 * @arg1: A #GList of who's typing.
	 * @user_data: User supplied data.
	 *
	 * Emitted when the typing state of an individual has changed.
	 */
	signals[SIG_CHANGED] = g_signal_new(
		"changed",
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		G_STRUCT_OFFSET(TalkatuTypingLabelClass, changed),
		NULL,
		NULL,
		NULL,
		G_TYPE_NONE,
		1,
		G_TYPE_POINTER
	);
}

/******************************************************************************
 * Public API
 *****************************************************************************/

/**
 * talkatu_typing_label_new:
 *
 * Creates a new #TalkatuTypingLabel that displays who is typing.
 *
 * Returns: (transfer full): The new #TalkatuTypingLabel instance.
 */
GtkWidget *talkatu_typing_label_new(void) {
	return GTK_WIDGET(g_object_new(
		TALKATU_TYPE_TYPING_LABEL,
		NULL
	));
}

/**
 * talkatu_typing_label_start_typing:
 * @label: The #TalkatuTypingLabel instance.
 * @who: The caller defined user that started typing.
 *
 * Adds @who to the list of users in @label that are currently typing.
 */
void
talkatu_typing_label_start_typing(TalkatuTypingLabel *label, const gchar *who) {
	TalkatuTypingLabelPrivate *priv = NULL;
	TalkatuTypingLabelTimeoutData *typing_data = NULL;
	guint timeout_id = 0;
	gpointer value = NULL;

	g_return_if_fail(TALKATU_IS_TYPING_LABEL(label));
	g_return_if_fail(who != NULL);

	priv = talkatu_typing_label_get_instance_private(label);

	/* create a timeout to remove this person from the list */
	typing_data = g_new(TalkatuTypingLabelTimeoutData, 1);
	typing_data->label = label;

	/* this gets cleaned up by the hash table's key destroy function */
	typing_data->who = g_strdup(who);

	timeout_id = g_timeout_add_seconds_full(G_PRIORITY_DEFAULT,
	                                        TALKATU_TYPING_LABEL_TIMEOUT,
	                                        talkatu_typing_label_timeout_cb,
	                                        typing_data,
	                                        g_free);

	value = GUINT_TO_POINTER(timeout_id);

	if(g_hash_table_replace(priv->typers, typing_data->who, value)) {
		/* the user wasn't in the list so we need to emit our changed signal */
		GList *typers = g_hash_table_get_keys(priv->typers);

		g_signal_emit(label, signals[SIG_CHANGED], 0, typers);

		g_list_free(typers);
	}
}

/**
 * talkatu_typing_label_finish_typing:
 * @label: The #TalkatuTypingLabel instance.
 * @who: The caller defined user that has finished typing.
 *
 * Removes @who from the list of users in @label that are currently typing.
 */
void
talkatu_typing_label_finish_typing(TalkatuTypingLabel *label,
                                   const gchar* who)
{
	TalkatuTypingLabelPrivate *priv = NULL;

	g_return_if_fail(TALKATU_IS_TYPING_LABEL(label));
	g_return_if_fail(who != NULL);

	priv = talkatu_typing_label_get_instance_private(label);

	if(g_hash_table_remove(priv->typers, who)) {
		GList *typers = g_hash_table_get_keys(priv->typers);

		/* emit our changed signal */
		g_signal_emit(label, signals[SIG_CHANGED], 0, typers);

		g_list_free(typers);
	}
}

