/* 
 * dingo-mprisv2.h
 *
 *  Copyright (C) 2011-2013 Phong V. Cao <phongvcao@phongvcao.com>
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 3 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA."
 *
 */

#ifndef INCLUDED_DINGO_MPRISV2_H
#define INCLUDED_DINGO_MPRISV2_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif //INCLUDED_MEMORY

namespace Dingo {
  class MPRISCommon;

  namespace MPRISv2 {
    //CREATORS
    void createMPRISv2(MPRISCommon* mpris_common);
    void createIntrospectionRoot();
    void createIntrospectionPlayer();
    void createIntrospectionPlaylists();
  
    //PUBLIC CONNECTION SIGNALS
    void onBusAcquired(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& name);
    void onNameAcquired(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& name);
    void onNameLost(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& name);
    
    //PUBLIC ROOT SIGNALS
    void onRootMethodCall(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& sender, const Glib::ustring& object_path, const Glib::ustring& interface_name, const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters, const Glib::RefPtr<Gio::DBus::MethodInvocation>& invocation);
    
    void onRootGetProperty(Glib::VariantBase& property, const
Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring&
sender, const Glib::ustring& object_path, const Glib::ustring&
interface_name, const Glib::ustring& property_name);

    bool onRootSetProperty(const Glib::RefPtr<Gio::DBus::Connection>&
connection, const Glib::ustring& sender, const Glib::ustring&
object_path, const Glib::ustring& interface_name, const Glib::ustring&
property_name, const Glib::VariantBase& value);
    
    //PUBLIC PLAYER SIGNALS
    void onPlayerMethodCall(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& sender, const Glib::ustring& object_path, const Glib::ustring& interface_name, const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters, const Glib::RefPtr<Gio::DBus::MethodInvocation>& invocation);
    
    void onPlayerGetProperty(Glib::VariantBase& property, const
Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring&
sender, const Glib::ustring& object_path, const Glib::ustring&
interface_name, const Glib::ustring& property_name);

    bool onPlayerSetProperty(const Glib::RefPtr<Gio::DBus::Connection>&
connection, const Glib::ustring& sender, const Glib::ustring&
object_path, const Glib::ustring& interface_name, const Glib::ustring&
property_name, const Glib::VariantBase& value);

    //PUBLIC PLAYLISTS SIGNALS
    void onPlaylistsMethodCall(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& sender, const Glib::ustring& object_path, const Glib::ustring& interface_name, const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters, const Glib::RefPtr<Gio::DBus::MethodInvocation>& invocation);
    
    void onPlaylistsGetProperty(Glib::VariantBase& property, const
Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring&
sender, const Glib::ustring& object_path, const Glib::ustring&
interface_name, const Glib::ustring& property_name);

    bool onPlaylistsSetProperty(const Glib::RefPtr<Gio::DBus::Connection>&
connection, const Glib::ustring& sender, const Glib::ustring&
object_path, const Glib::ustring& interface_name, const Glib::ustring&
property_name, const Glib::VariantBase& value);

    //PUBLIC OBSERVER METHODS
    void emitPropertiesChangedSignal(const Glib::ustring& interface_name, const Glib::ustring& property_name);
    void emitInterfacesSignal(const Glib::ustring& object_path, const Glib::ustring& interface_name, const Glib::ustring& signal_name);
    
    //PUBLIC CONST VARIABLES
    static const Glib::ustring INTERFACE_ROOT = "org.mpris.MediaPlayer2";
    static const Glib::ustring INTERFACE_PLAYER = "org.mpris.MediaPlayer2.Player";
    static const Glib::ustring INTERFACE_PLAYLISTS = "org.mpris.MediaPlayer2.Playlists";
    static const Glib::ustring SERVICE = "org.mpris.MediaPlayer2.dingo";
    static const Glib::ustring PATH = "/org/mpris/MediaPlayer2";
    
    //PUBLIC VARIABLES
    static Glib::ustring introspection_xml_root;
    static Glib::ustring introspection_xml_player;
    static Glib::ustring introspection_xml_playlists;
    
    static Glib::RefPtr<Gio::DBus::NodeInfo> introspection_data_root;
    static Glib::RefPtr<Gio::DBus::NodeInfo> introspection_data_player;
    static Glib::RefPtr<Gio::DBus::NodeInfo> introspection_data_playlists;
    
    static guint registered_id_root;
    static guint registered_id_player;
    static guint registered_id_playlists;
    
    static MPRISCommon* d_mpris_common;
    static Glib::RefPtr<Gio::DBus::Connection> d_connection;
  }
}

#endif //INCLUDED_DINGO_MPRISV2_H
