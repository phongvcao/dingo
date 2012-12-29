Summary:		An open-source software that helps Linux users manage and enjoy their video and audio collections
Name:			dingo
Version:		0.5.0
Release:		1
License:		GPLv3.0+
Group:			Applications/Multimedia
Source:			%{name}-%{version}.tar.gz
URL:			http://dingo.phongvcao.com/
BuildRequires:		gtkmm30 >= 3.4.0
BuildRequires:		gtkmm30-devel >= 3.4.0
BuildRequires:		gstreamermm >= 0.10.9
BuildRequires:		gstreamermm-devel >= 0.10.9
BuildRequires:		libmediainfo0 >= 0.7.61
BuildRequires:		libmediainfo-devel >= 0.7.61
BuildRequires:		sqlite >= 3.7.11
BuildRequires:		sqlite-devel >= 3.7.11
BuildRequires:		libxml++ >= 2.34.2
BuildRequires:		libxml++-devel >= 2.34.2
BuildRequires:		gettext >= 0.18.1.1
BuildRequires:		gettext-devel >= 0.18.1.1
BuildRequires:		intltool >= 0.50.2

%description
Dingo allows the user to manage and enjoy their video and audio collections.
It is able to manage and play both audio and video, in various file formats 
supported by GStreamer. Dingo can also organize tracks into playlists, repeat
only a particular section of the song, fetch lyrics and save subtitle for
future playbacks.

Install Dingo if you want to manage both videos and audios and enjoy them 
using a powerful, feature-rich media player.

%prep
%setup -q -n %{name}-%{version}

%build
%configure
make

%install
make DESTDIR=%{buildroot} install
%find_lang %{name}

%clean
%{__rm} -rf %{buildroot}

%files -f %{name}.lang
%defattr(-, root, root, 0755)
%doc AUTHORS ChangeLog COPYING INSTALL NEWS README TODO
%{_bindir}/*
%{_datadir}/%{name}/
%{_datadir}/applications/dingo.desktop
%{_datadir}/icons/hicolor/256x256/apps/dingo.png
%{_datadir}/icons/hicolor/128x128/apps/dingo.png
%{_datadir}/icons/hicolor/48x48/apps/dingo.png
%{_datadir}/icons/hicolor/32x32/apps/dingo.png
%{_datadir}/icons/hicolor/24x24/apps/dingo.png
%{_datadir}/icons/hicolor/16x16/apps/dingo.png

%changelog
