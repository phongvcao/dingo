/* 
 * dingo-mprisv2.cc
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA."
 *
 */

#include "dingo-mprisv2.h"
#include "dingo-mpriscommon.h"

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

//CREATORS
void Dingo::MPRISv2::createMPRISv2(Dingo::MPRISCommon* mpris_common) {
  Dingo::MPRISv2::d_mpris_common = mpris_common;

  Dingo::MPRISv2::createIntrospectionRoot();
  Dingo::MPRISv2::createIntrospectionPlayer();
  Dingo::MPRISv2::createIntrospectionPlaylists();
  
  const guint id = Gio::DBus::own_name(Gio::DBus::BUS_TYPE_SESSION, Dingo::MPRISv2::SERVICE, sigc::ptr_fun(&Dingo::MPRISv2::onBusAcquired), sigc::ptr_fun(&Dingo::MPRISv2::onNameAcquired), sigc::ptr_fun(&Dingo::MPRISv2::onNameLost));
}

const Gio::DBus::InterfaceVTable Dingo_MPRISv2_root_interface_vtable(sigc::ptr_fun(&Dingo::MPRISv2::onRootMethodCall), sigc::ptr_fun(&Dingo::MPRISv2::onRootGetProperty), sigc::ptr_fun(&Dingo::MPRISv2::onRootSetProperty));

void Dingo::MPRISv2::createIntrospectionRoot() {
  Dingo::MPRISv2::introspection_xml_root = 
    "<node>"
    "  <interface name='org.mpris.MediaPlayer2'>"
    "    <method name='Raise'/>"
    "    <method name='Quit'/>"
    "    <property name='CanQuit' type='b' access='read'/>"
    "    <property name='Fullscreen' type='b' access='readwrite'/>"
    "    <property name='CanSetFullscreen' type='b' access='read'/>"
    "    <property name='CanRaise' type='b' access='read'/>"
    "    <property name='HasTrackList' type='b' access='read'/>"
    "    <property name='Identity' type='s' access='read'/>"
    "    <property name='DesktopEntry' type='s' access='read'/>"
    "    <property name='SupportedUriSchemes' type='as' access='read'/>"
    "    <property name='SupportedMimeTypes' type='as' access='read'/>"
    "  </interface>"
    "</node>";
  
  try {
    Dingo::MPRISv2::introspection_data_root = Gio::DBus::NodeInfo::create_for_xml(Dingo::MPRISv2::introspection_xml_root);
  }
  
  catch (const Glib::Error& ex) {
    std::cerr << "Unable to create introspection root data!" << std::endl;
    
    return;
  }
}

const Gio::DBus::InterfaceVTable Dingo_MPRISv2_player_interface_vtable(sigc::ptr_fun(&Dingo::MPRISv2::onPlayerMethodCall), sigc::ptr_fun(&Dingo::MPRISv2::onPlayerGetProperty), sigc::ptr_fun(&Dingo::MPRISv2::onPlayerSetProperty));

void Dingo::MPRISv2::createIntrospectionPlayer() {
  Dingo::MPRISv2::introspection_xml_player = 
    "<node>"
    "  <interface name='org.mpris.MediaPlayer2.Player'>"
    "    <method name='Next'/>"
    "    <method name='Previous'/>"
    "    <method name='Pause'/>"
    "    <method name='PlayPause'/>"
    "    <method name='Stop'/>"
    "    <method name='Play'/>"
    "    <method name='Seek'>"
    "      <arg direction='in' name='Offset' type='x'/>"
    "    </method>"
    "    <method name='SetPosition'>"
    "      <arg direction='in' name='TrackId' type='o'/>"
    "      <arg direction='in' name='Position' type='x'/>"
    "    </method>"
    "    <method name='OpenUri'>"
    "      <arg direction='in' name='Uri' type='s'/>"
    "    </method>"
    "    <signal name='Seeked'>"
    "      <arg name='Position' type='x'/>"
    "    </signal>"
    "    <property name='PlaybackStatus' type='s' access='read'/>"
    "    <property name='LoopStatus' type='s' access='readwrite'/>"
    "    <property name='Rate' type='d' access='readwrite'/>"
    "    <property name='Shuffle' type='b' access='readwrite'/>"
    "    <property name='Metadata' type='a{sv}' access='read'/>"
    "    <property name='Volume' type='d' access='readwrite'/>"
    "    <property name='Position' type='x' access='read'/>"
    "    <property name='MinimumRate' type='d' access='read'/>"
    "    <property name='MaximumRate' type='d' access='read'/>"
    "    <property name='CanGoNext' type='b' access='read'/>"
    "    <property name='CanGoPrevious' type='b' access='read'/>"
    "    <property name='CanPlay' type='b' access='read'/>"
    "    <property name='CanPause' type='b' access='read'/>"
    "    <property name='CanSeek' type='b' access='read'/>"
    "    <property name='CanControl' type='b' access='read'/>"
    "  </interface>"
    "</node>";
    
  try {
    Dingo::MPRISv2::introspection_data_player = Gio::DBus::NodeInfo::create_for_xml(Dingo::MPRISv2::introspection_xml_player);
  }
  
  catch (const Glib::Error& ex) {
    std::cerr << "Unable to create introspection player data!" << std::endl;
    
    return;
  }
}

const Gio::DBus::InterfaceVTable Dingo_MPRISv2_playlists_interface_vtable(sigc::ptr_fun(&Dingo::MPRISv2::onPlaylistsMethodCall), sigc::ptr_fun(&Dingo::MPRISv2::onPlaylistsGetProperty), sigc::ptr_fun(&Dingo::MPRISv2::onPlaylistsSetProperty));

void Dingo::MPRISv2::createIntrospectionPlaylists() {
  Dingo::MPRISv2::introspection_xml_playlists = 
    "<node>"
    "  <interface name='org.mpris.MediaPlayer2.Playlists'>"
    "    <method name='ActivatePlaylist'>"
    "      <arg direction='in' name='PlaylistId' type='o'/>"
    "    </method>"
    "    <method name='GetPlaylists'>"
    "      <arg direction='in' name='Index' type='u'/>"
    "      <arg direction='in' name='MaxCount' type='u'/>"
    "      <arg direction='in' name='Order' type='s'/>"
    "      <arg direction='in' name='ReverseOrder' type='b'/>"
    "      <arg direction='out' name='Playlists' type='a(oss)'/>"
    "    </method>"
    "    <signal name='PlaylistChanged'>"
    "      <arg name='Playlist' type='(oss)'/>"
    "    </signal>"
    "    <property name='PlaylistCount' type='u' access='read'/>"
    "    <property name='Orderings' type='as' access='read'/>"
    "    <property name='ActivePlaylist' type='(b(oss))' access='read'/>"
    "  </interface>"
    "</node>";
    
  try {
    Dingo::MPRISv2::introspection_data_playlists = Gio::DBus::NodeInfo::create_for_xml(Dingo::MPRISv2::introspection_xml_playlists);
  }
  
  catch (const Glib::Error& ex) {
    std::cerr << "Unable to create introspection playlists data!" << std::endl;
    
    return;
  }
}

//PUBLIC CONNECTION SIGNALS
void Dingo::MPRISv2::onBusAcquired(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& name) {
  //Registration for root object
  try {
    Dingo::MPRISv2::registered_id_root = connection->register_object(Dingo::MPRISv2::PATH, Dingo::MPRISv2::introspection_data_root->lookup_interface(), Dingo_MPRISv2_root_interface_vtable);
  }
  
  catch (const Glib::Error& ex) {
    std::cerr << "Registration of root object failed!" << std::endl;
    
    return;
  }
  
  //Registration for player object
  try {
    Dingo::MPRISv2::registered_id_player = connection->register_object(Dingo::MPRISv2::PATH, Dingo::MPRISv2::introspection_data_player->lookup_interface(), Dingo_MPRISv2_player_interface_vtable);
  }
  
  catch (const Glib::Error& ex) {
    std::cerr << "Registration of player object failed!" << std::endl;
    
    return;
  }
  
  //Registration for playlists object
  try {
    Dingo::MPRISv2::registered_id_playlists = connection->register_object(Dingo::MPRISv2::PATH, Dingo::MPRISv2::introspection_data_playlists->lookup_interface(), Dingo_MPRISv2_playlists_interface_vtable);
  }
  
  catch (const Glib::Error& ex) {
    std::cerr << "Registration of playlists object failed!" << std::endl;
    
    return;
  }
  
  //save the connection
  Dingo::MPRISv2::d_connection = connection;
}

void Dingo::MPRISv2::onNameAcquired(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& name) {
  //no idea what to implement here...
}

void Dingo::MPRISv2::onNameLost(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& name) {
  connection->unregister_object(registered_id_root);
  connection->unregister_object(registered_id_player);
  connection->unregister_object(registered_id_playlists);
}

//PUBLIC ROOT SIGNALS
void Dingo::MPRISv2::onRootMethodCall(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& sender, const Glib::ustring& object_path, const Glib::ustring& interface_name, const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters, const Glib::RefPtr<Gio::DBus::MethodInvocation>& invocation) {
  Dingo::MPRISv2::d_mpris_common->invokeMethodRoot(method_name, parameters);
}
    
void Dingo::MPRISv2::onRootGetProperty(Glib::VariantBase& property, const
Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring&
sender, const Glib::ustring& object_path, const Glib::ustring&
interface_name, const Glib::ustring& property_name) {
  property = Dingo::MPRISv2::d_mpris_common->readPropertyValueRoot(property_name);
}

bool Dingo::MPRISv2::onRootSetProperty(const Glib::RefPtr<Gio::DBus::Connection>&
connection, const Glib::ustring& sender, const Glib::ustring&
object_path, const Glib::ustring& interface_name, const Glib::ustring&
property_name, const Glib::VariantBase& value) {
  Dingo::MPRISv2::d_mpris_common->writePropertyValueRoot(property_name, value);

  //emit the org.freedesktop.DBus.Properties.PropertiesChanged signal
  Dingo::MPRISv2::emitPropertiesChangedSignal(interface_name, property_name);
    
  return true;
}
    
//PUBLIC PLAYER SIGNALS
void Dingo::MPRISv2::onPlayerMethodCall(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& sender, const Glib::ustring& object_path, const Glib::ustring& interface_name, const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters, const Glib::RefPtr<Gio::DBus::MethodInvocation>& invocation) {
  Dingo::MPRISv2::d_mpris_common->invokeMethodPlayer(method_name, parameters);
}
    
void Dingo::MPRISv2::onPlayerGetProperty(Glib::VariantBase& property, const
Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring&
sender, const Glib::ustring& object_path, const Glib::ustring&
interface_name, const Glib::ustring& property_name) {
  property = Dingo::MPRISv2::d_mpris_common->readPropertyValuePlayer(property_name);
}

bool Dingo::MPRISv2::onPlayerSetProperty(const Glib::RefPtr<Gio::DBus::Connection>&
connection, const Glib::ustring& sender, const Glib::ustring&
object_path, const Glib::ustring& interface_name, const Glib::ustring&
property_name, const Glib::VariantBase& value) {
  Dingo::MPRISv2::d_mpris_common->writePropertyValuePlayer(property_name, value);
  
  //emit the org.freedesktop.DBus.Properties.PropertiesChanged signal
  Dingo::MPRISv2::emitPropertiesChangedSignal(interface_name, property_name);
  
  return true;
}

//PUBLIC PLAYLISTS SIGNALS
void Dingo::MPRISv2::onPlaylistsMethodCall(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& sender, const Glib::ustring& object_path, const Glib::ustring& interface_name, const Glib::ustring& method_name, const Glib::VariantContainerBase& parameters, const Glib::RefPtr<Gio::DBus::MethodInvocation>& invocation) {
  invocation->return_value(Dingo::MPRISv2::d_mpris_common->invokeMethodPlaylists(method_name, parameters));
}
    
void Dingo::MPRISv2::onPlaylistsGetProperty(Glib::VariantBase& property, const
Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring&
sender, const Glib::ustring& object_path, const Glib::ustring&
interface_name, const Glib::ustring& property_name) {
  property = Dingo::MPRISv2::d_mpris_common->readPropertyValuePlaylists(property_name);
}

bool Dingo::MPRISv2::onPlaylistsSetProperty(const Glib::RefPtr<Gio::DBus::Connection>&
connection, const Glib::ustring& sender, const Glib::ustring&
object_path, const Glib::ustring& interface_name, const Glib::ustring&
property_name, const Glib::VariantBase& value) {
  //no writable properties

  return false;
}

//PUBLIC OBSERVER METHODS
void Dingo::MPRISv2::emitPropertiesChangedSignal(const Glib::ustring& interface_name, const Glib::ustring& property_name) {
  std::map<Glib::ustring, Glib::VariantBase> properties_map;
  std::vector<Glib::ustring> invalidated_vector;
  
  if (interface_name == Dingo::MPRISv2::INTERFACE_ROOT) {
    properties_map.insert(std::pair<Glib::ustring, Glib::VariantBase>(property_name, Dingo::MPRISv2::d_mpris_common->readPropertyValueRoot(property_name)));
  }
  
  else if (interface_name == Dingo::MPRISv2::INTERFACE_PLAYER) {
    properties_map.insert(std::pair<Glib::ustring, Glib::VariantBase>(property_name, Dingo::MPRISv2::d_mpris_common->readPropertyValuePlayer(property_name)));
  }
  
  else if (interface_name == Dingo::MPRISv2::INTERFACE_PLAYLISTS) {
    properties_map.insert(std::pair<Glib::ustring, Glib::VariantBase>(property_name, Dingo::MPRISv2::d_mpris_common->readPropertyValuePlaylists(property_name)));
  }
  
  else {
    std::cout << "No support for interface: " << interface_name << std::endl;
  }
      
  Glib::Variant<std::map<Glib::ustring, Glib::VariantBase> > properties_variant = Glib::Variant<std::map<Glib::ustring, Glib::VariantBase> >::create(properties_map);
  Glib::Variant<std::vector<Glib::ustring> > invalidated_variant = Glib::Variant<std::vector<Glib::ustring> >::create(invalidated_vector);
      
  std::vector<Glib::VariantBase> parameters_tuple_vector;
  parameters_tuple_vector.push_back(Glib::Variant<Glib::ustring>::create(interface_name));
  parameters_tuple_vector.push_back(properties_variant);
  parameters_tuple_vector.push_back(invalidated_variant);
      
  Glib::VariantContainerBase parameters_tuple = Glib::VariantContainerBase::create_tuple(parameters_tuple_vector);
      
  Dingo::MPRISv2::d_connection->emit_signal(Dingo::MPRISv2::PATH, "org.freedesktop.DBus.Properties", "PropertiesChanged", "", parameters_tuple);
}

void Dingo::MPRISv2::emitInterfacesSignal(const Glib::ustring& object_path, const Glib::ustring& interface_name, const Glib::ustring& signal_name) {
  if (interface_name == Dingo::MPRISv2::INTERFACE_PLAYER) {
    Dingo::MPRISv2::d_connection->emit_signal(Dingo::MPRISv2::PATH, interface_name, signal_name, "", Dingo::MPRISv2::d_mpris_common->getParametersPlayerSignal(signal_name));
  }
  
  else if (interface_name == Dingo::MPRISv2::INTERFACE_PLAYLISTS) {
    Dingo::MPRISv2::d_connection->emit_signal(Dingo::MPRISv2::PATH, interface_name, signal_name, "", Dingo::MPRISv2::d_mpris_common->getParametersPlaylistsSignal(signal_name));
  }
  
  else {
    std::cout << "No support for interface: " << interface_name << std::endl;
  }
}
