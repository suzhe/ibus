/* vim:set et sts=4: */
/* ibus - The Input Bus
 * Copyright (C) 2008-2009 Huang Peng <shawn.p.huang@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "ibusproxy.h"
#include "ibusinternal.h"

#define IBUS_PROXY_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), IBUS_TYPE_PROXY, IBusProxyPrivate))

enum {
    DBUS_SIGNAL,
    LAST_SIGNAL,
};


/* IBusProxyPriv */
struct _IBusProxyPrivate {
    gchar *name;
    gchar *path;
    IBusConnection *connection;
};
typedef struct _IBusProxyPrivate IBusProxyPrivate;

static guint            _signals[LAST_SIGNAL] = { 0 };

/* functions prototype */
static void     ibus_proxy_class_init  (IBusProxyClass  *klass);
static void     ibus_proxy_init        (IBusProxy       *proxy);
static void     ibus_proxy_destroy     (IBusProxy       *proxy);

static gboolean ibus_proxy_dbus_signal (IBusProxy       *proxy,
                                        DBusMessage     *message);

static IBusObjectClass  *_parent_class = NULL;

GType
ibus_proxy_get_type (void)
{
    static GType type = 0;

    static const GTypeInfo type_info = {
        sizeof (IBusProxyClass),
        (GBaseInitFunc)     NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc)    ibus_proxy_class_init,
        NULL,               /* class finalize */
        NULL,               /* class data */
        sizeof (IBusProxy),
        0,
        (GInstanceInitFunc) ibus_proxy_init,
    };

    if (type == 0) {
        type = g_type_register_static (IBUS_TYPE_OBJECT,
                    "IBusProxy",
                    &type_info,
                    (GTypeFlags)0);
    }
    return type;
}

IBusProxy *
ibus_proxy_new (const gchar     *name,
                const gchar     *path,
                IBusConnection  *connection)
{
    g_assert (name != NULL);
    g_assert (path != NULL);
    g_assert (IBUS_IS_CONNECTION (connection));

    IBusProxy *proxy;
    IBusProxyPrivate *priv;

    proxy = IBUS_PROXY (g_object_new (IBUS_TYPE_PROXY, NULL));

    priv = IBUS_PROXY_GET_PRIVATE (proxy);
    priv->name = g_strdup (name);
    priv->path = g_strdup (path);
    priv->connection = g_object_ref (connection);

    return proxy;
}

static void
ibus_proxy_class_init (IBusProxyClass *klass)
{
    IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);

    _parent_class = (IBusObjectClass *) g_type_class_peek_parent (klass);

    g_type_class_add_private (klass, sizeof (IBusProxyPrivate));

    ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_proxy_destroy;

    klass->dbus_signal = ibus_proxy_dbus_signal;

    _signals[DBUS_SIGNAL] =
        g_signal_new (I_("dbus-signal"),
            G_TYPE_FROM_CLASS (klass),
            G_SIGNAL_RUN_FIRST,
            G_STRUCT_OFFSET (IBusProxyClass, dbus_signal),
            NULL, NULL,
            ibus_marshal_BOOLEAN__POINTER,
            G_TYPE_BOOLEAN,
            1, G_TYPE_POINTER);

}

static void
ibus_proxy_init (IBusProxy *proxy)
{
    IBusProxyPrivate *priv;
    priv = IBUS_PROXY_GET_PRIVATE (proxy);

    priv->name = NULL;
    priv->path = NULL;
    priv->connection = NULL;
}

static void
ibus_proxy_destroy (IBusProxy *proxy)
{
    IBusProxyPrivate *priv;
    priv = IBUS_PROXY_GET_PRIVATE (proxy);
    
    g_free (priv->name);
    g_free (priv->path);
    g_object_unref (priv->connection);
    
    priv->name = NULL;
    priv->path = NULL;
    priv->connection = NULL;
    
    IBUS_OBJECT_CLASS(_parent_class)->destroy (IBUS_OBJECT (proxy));
}

gboolean
ibus_proxy_handle_signal (IBusProxy *proxy, DBusMessage *message)
{
    gboolean retval = FALSE;
    g_return_val_if_fail (message != NULL, FALSE);
    
    g_signal_emit (proxy, _signals[DBUS_SIGNAL], 0, message, &retval);
    return retval ? DBUS_HANDLER_RESULT_HANDLED : DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static gboolean
ibus_proxy_dbus_signal (IBusProxy *proxy, DBusMessage *message)
{
    return FALSE;
}
