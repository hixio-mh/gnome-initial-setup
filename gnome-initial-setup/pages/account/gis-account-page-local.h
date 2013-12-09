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

#ifndef __GIS_ACCOUNT_PAGE_LOCAL_H__
#define __GIS_ACCOUNT_PAGE_LOCAL_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GIS_TYPE_ACCOUNT_PAGE_LOCAL               (gis_account_page_local_get_type ())
#define GIS_ACCOUNT_PAGE_LOCAL(obj)                           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIS_TYPE_ACCOUNT_PAGE_LOCAL, GisAccountPageLocal))
#define GIS_ACCOUNT_PAGE_LOCAL_CLASS(klass)                   (G_TYPE_CHECK_CLASS_CAST ((klass),  GIS_TYPE_ACCOUNT_PAGE_LOCAL, GisAccountPageLocalClass))
#define GIS_IS_ACCOUNT_PAGE_LOCAL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIS_TYPE_ACCOUNT_PAGE_LOCAL))
#define GIS_IS_ACCOUNT_PAGE_LOCAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GIS_TYPE_ACCOUNT_PAGE_LOCAL))
#define GIS_ACCOUNT_PAGE_LOCAL_GET_CLASS(obj)                 (G_TYPE_INSTANCE_GET_CLASS ((obj),  GIS_TYPE_ACCOUNT_PAGE_LOCAL, GisAccountPageLocalClass))

typedef struct _GisAccountPageLocal        GisAccountPageLocal;
typedef struct _GisAccountPageLocalClass   GisAccountPageLocalClass;

struct _GisAccountPageLocal
{
    GtkBin parent;
};

struct _GisAccountPageLocalClass
{
    GtkBinClass parent_class;
};

GType gis_account_page_local_get_type (void);

gboolean gis_account_page_local_validate (GisAccountPageLocal *local);
gboolean gis_account_page_local_apply (GisAccountPageLocal *local, GisPage *page);
void gis_account_page_local_create_user (GisAccountPageLocal *local);

G_END_DECLS

#endif /* __GIS_ACCOUNT_PAGE_LOCAL_H__ */
