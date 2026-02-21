#include <dbus/dbus.h>

#include "notify.hh"

namespace {
  DBusError error;
  DBusConnection* connection;
}

namespace clamshell::notify {
  void init() noexcept {
    dbus_error_init(&error);

    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (dbus_error_is_set(&error)) {
      dbus_error_free(&error);
    }
  }

  void deinit() noexcept {
    if (connection) {
      dbus_connection_unref(connection);
    }

    if (dbus_error_is_set(&error)) {
      dbus_error_free(&error);
    }
  }

  void send(
    const char* name,
    const char* icon,
    const char* summary,
    const char* body,
    int timeout
  ) noexcept {
    if (!connection) {
      return;
    }

    DBusMessage* message = dbus_message_new_method_call(
      "org.freedesktop.Notifications",
      "/org/freedesktop/Notifications",
      "org.freedesktop.Notifications",
      "Notify"
    );

    if (!message) {
      return;
    }

    DBusMessageIter args;
    dbus_message_iter_init_append(message, &args);

    dbus_uint32_t replaces_id = 0;

    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &name);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &replaces_id);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &icon);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &summary);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &body);

    DBusMessageIter actions_iter;
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING, &actions_iter);
    dbus_message_iter_close_container(&args, &actions_iter);

    DBusMessageIter hints_iter;
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &hints_iter);
    dbus_message_iter_close_container(&args, &hints_iter);

    dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &timeout);

    DBusPendingCall* pending = nullptr;
    if (!dbus_connection_send_with_reply(connection, message, &pending, -1)) {
      dbus_message_unref(message);
      return;
    }

    dbus_connection_flush(connection);
    dbus_message_unref(message);

    dbus_pending_call_block(pending);
    DBusMessage* reply = dbus_pending_call_steal_reply(pending);
    dbus_pending_call_unref(pending);

    if (reply) {
      dbus_uint32_t notif_id = 0;
      DBusMessageIter reply_iter;
      dbus_message_iter_init(reply, &reply_iter);
      dbus_message_iter_get_basic(&reply_iter, &notif_id);
      dbus_message_unref(reply);
    }
  }
}
