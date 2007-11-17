/* gtkextra
 * Copyright 1999-2001 Adrian E. Feiguin <feiguin@ifir.edu.ar>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>
#include "gtkextrafeatures.h"

const guint gtkextra_major_version = GTKEXTRA_MAJOR_VERSION;
const guint gtkextra_minor_version = GTKEXTRA_MINOR_VERSION;
const guint gtkextra_micro_version = GTKEXTRA_MICRO_VERSION;
const guint gtkextra_binary_age = GTKEXTRA_BINARY_AGE;
const guint gtkextra_interface_age = GTKEXTRA_INTERFACE_AGE;

gchar * 
gtk_extra_check_version (guint required_major,
                         guint required_minor,
                         guint required_micro)
{
  if (required_major > GTKEXTRA_MAJOR_VERSION)
    return "GtkExtra version too old (major mismatch)";
  if (required_major < GTKEXTRA_MAJOR_VERSION)
    return "GtkExtra version too new (major mismatch)";
  if (required_minor > GTKEXTRA_MINOR_VERSION)
    return "GtkExtra version too old (minor mismatch)";
  if (required_minor < GTKEXTRA_MINOR_VERSION)
    return "GtkExtra version too new (minor mismatch)";
  if (required_micro < GTKEXTRA_MICRO_VERSION - GTKEXTRA_BINARY_AGE)
    return "GtkExtra version too new (micro mismatch)";
  if (required_micro > GTKEXTRA_MICRO_VERSION)
    return "GtkExtra version too old (micro mismatch)";
  return NULL;
}
