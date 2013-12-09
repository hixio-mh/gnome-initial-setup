/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * Copyright (C) 2013 Red Hat
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by:
 *     Jasper St. Pierre <jstpierre@mecheye.net>
 */

#include "config.h"

#include "gis-page.h"
#include "gis-account-page-local.h"
#include "gnome-initial-setup.h"

#include <glib/gi18n.h>
#include <gio/gio.h>

#include <string.h>
#include <act/act-user-manager.h>
#include "um-utils.h"

struct _GisAccountPageLocalPrivate
{
  GtkWidget *fullname_entry;
  GtkWidget *username_combo;

  ActUser *act_user;
  ActUserManager *act_client;

  gboolean valid_name;
  gboolean valid_username;
  ActUserAccountType account_type;
};
typedef struct _GisAccountPageLocalPrivate GisAccountPageLocalPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GisAccountPageLocal, gis_account_page_local, GTK_TYPE_BIN);

enum {
  VALIDATION_CHANGED,
  USER_CREATED,
  LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
validation_changed (GisAccountPageLocal *page)
{
  g_signal_emit (page, signals[VALIDATION_CHANGED], 0);
}

static void
clear_account_page (GisAccountPageLocal *page)
{
  GisAccountPageLocalPrivate *priv = gis_account_page_local_get_instance_private (page);

  priv->valid_name = FALSE;
  priv->valid_username = FALSE;

  /* FIXME: change this for a large deployment scenario; maybe through a GSetting? */
  priv->account_type = ACT_USER_ACCOUNT_TYPE_ADMINISTRATOR;

  gtk_entry_set_text (GTK_ENTRY (priv->fullname_entry), "");
  gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX (priv->username_combo))));
}

static void
fullname_changed (GtkWidget      *w,
                  GParamSpec     *pspec,
                  GisAccountPageLocal *page)
{
  GisAccountPageLocalPrivate *priv = gis_account_page_local_get_instance_private (page);
  GtkWidget *entry;
  GtkTreeModel *model;
  const char *name;

  name = gtk_entry_get_text (GTK_ENTRY (w));

  entry = gtk_bin_get_child (GTK_BIN (priv->username_combo));
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (priv->username_combo));

  gtk_list_store_clear (GTK_LIST_STORE (model));

  priv->valid_name = is_valid_name (name);

  if (!priv->valid_name) {
    gtk_entry_set_text (GTK_ENTRY (entry), "");
    return;
  }

  generate_username_choices (name, GTK_LIST_STORE (model));

  gtk_combo_box_set_active (GTK_COMBO_BOX (priv->username_combo), 0);

  validation_changed (page);
}

static void
username_changed (GtkComboBoxText     *combo,
                  GisAccountPageLocal *page)
{
  GisAccountPageLocalPrivate *priv = gis_account_page_local_get_instance_private (page);
  const gchar *username;
  gchar *tip;
  GtkWidget *entry;

  username = gtk_combo_box_text_get_active_text (combo);

  priv->valid_username = is_valid_username (username, &tip);

  entry = gtk_bin_get_child (GTK_BIN (combo));

  if (tip) {
    set_entry_validation_error (GTK_ENTRY (entry), tip);
    g_free (tip);
  }
  else {
    clear_entry_validation_error (GTK_ENTRY (entry));
  }

  validation_changed (page);
}

static void
gis_account_page_local_constructed (GObject *object)
{
  GisAccountPageLocal *page = GIS_ACCOUNT_PAGE_LOCAL (object);
  GisAccountPageLocalPrivate *priv = gis_account_page_local_get_instance_private (page);

  G_OBJECT_CLASS (gis_account_page_local_parent_class)->constructed (object);

  priv->act_client = act_user_manager_get_default ();

  g_signal_connect (priv->fullname_entry, "notify::text",
                    G_CALLBACK (fullname_changed), page);
  g_signal_connect (priv->username_combo, "changed",
                    G_CALLBACK (username_changed), page);

  clear_account_page (page);
}

static void
local_create_user (GisAccountPageLocal *page)
{
  GisAccountPageLocalPrivate *priv = gis_account_page_local_get_instance_private (page);
  const gchar *username;
  const gchar *fullname;
  GError *error = NULL;

  username = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (priv->username_combo));
  fullname = gtk_entry_get_text (GTK_ENTRY (priv->fullname_entry));

  priv->act_user = act_user_manager_create_user (priv->act_client, username, fullname, priv->account_type, &error);
  if (error != NULL) {
    g_warning ("Failed to create user: %s", error->message);
    g_error_free (error);
    return;
  }

  act_user_set_user_name (priv->act_user, username);
  act_user_set_account_type (priv->act_user, priv->account_type);

  g_signal_emit (page, signals[USER_CREATED], 0, priv->act_user, "");
}

static void
gis_account_page_local_class_init (GisAccountPageLocalClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/org/gnome/initial-setup/gis-account-page-local.ui");

  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GisAccountPageLocal, fullname_entry);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GisAccountPageLocal, username_combo);

  object_class->constructed = gis_account_page_local_constructed;

  signals[VALIDATION_CHANGED] = g_signal_new ("validation-changed", GIS_TYPE_ACCOUNT_PAGE_LOCAL,
                                              G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL,
                                              G_TYPE_NONE, 0);

  signals[USER_CREATED] = g_signal_new ("user-created", GIS_TYPE_ACCOUNT_PAGE_LOCAL,
                                        G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL,
                                        G_TYPE_NONE, 2, ACT_TYPE_USER, G_TYPE_STRING);
}

static void
gis_account_page_local_init (GisAccountPageLocal *page)
{
  gtk_widget_init_template (GTK_WIDGET (page));
}

gboolean
gis_account_page_local_validate (GisAccountPageLocal *page)
{
  GisAccountPageLocalPrivate *priv = gis_account_page_local_get_instance_private (page);

  return priv->valid_name && priv->valid_username;
}

void
gis_account_page_local_create_user (GisAccountPageLocal *page)
{
  local_create_user (page);
}

gboolean
gis_account_page_local_apply (GisAccountPageLocal *local, GisPage *page)
{
  GisAccountPageLocalPrivate *priv = gis_account_page_local_get_instance_private (local);
  const gchar *username;

  username = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (priv->username_combo));
  gis_driver_set_username (GIS_PAGE (page)->driver, username);

  return FALSE;
}
