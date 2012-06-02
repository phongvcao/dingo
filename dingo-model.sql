/* default artist: artistid = 1, artistname = 'Unknown Artist' 
 * default album: albumid = 1, artistid = 1, albumname = 'Unknown Album'
 * default playlistsortcolidx: 2 (track_title)
 * default playlistsorttype: 1 (Gtk::SORT_ASCENDING)
 */

/* create coreartists table */

create table coreartists
(
  artistid integer primary key,
  artistname text check(artistname != '') unique not null
);

insert into coreartists (artistname) values ('Unknown Artist'); 

/* create corealbums table */

create table corealbums
(
  albumid integer primary key,
  albumname text check(albumname != '') unique not null
);

insert into corealbums (albumname) values ('Unknown Album');

/* create coretracks table */
create table coretracks
(
  trackid integer primary key,
  artistid integer default 1 not null references coreartists(artistid) on delete set default,
  albumid integer default 1 not null references corealbums(albumid) on delete set default,
  tracknumber integer,
  tracktitle text check(tracktitle != '') not null,
  trackgenre text,
  tracktime integer not null,
  tracksize integer not null,
  trackfilename text not null,
  trackuri text unique not null,
  trackyear integer,
  trackbitrate integer not null,
  trackdateadded text not null
);

/* create coreplaylists table */

create table coreplaylists
(
  playlistID integer primary key,
  playlistname text check(playlistname != '') not null,
  playlistsortcolidx integer default 2 not null,
  playlistsorttype integer default 1 not null,
  playlistautostart numeric default 0 not null
);

/* create coresmartplaylists table */
create table coresmartplaylists 
(
  smartplaylistid integer primary key,
  smartplaylistname text check(smartplaylistname != '') not null,
  smartplaylistsortcolidx integer default 2 not null,
  smartplaylistsorttype integer default 1 not null,
  smartplaylistautostart numeric default 0 not null,
  smartplaylistquery text unique not null
);

/* create coresettings table */
create table coresettings 
(
  settingid integer primary key,
  settingname text check(settingname != '') not null,
  settingvalue integer not null
);

/* create queuetracks table */
create table queuetracks 
(
  trackid integer primary key,
  artistname text check(artistname != '') default 'Unknown Artist' not null,
  albumname text check(albumname != '') default 'Unknown Album' not null,
  tracknumber integer,
  tracktitle text check(tracktitle != '') not null,
  trackgenre text,
  tracktime integer not null,
  tracksize integer not null,
  trackfilename text not null,
  trackuri text unique not null,
  trackyear integer,
  trackbitrate integer not null,
  trackdateadded text not null
);

/* create coretracksplaylists table */
create table coretracksplaylists 
(
  trackID integer not null references coretracks(trackid) on delete cascade,
  playlistID integer not null references coreplaylists(playlistid) on delete cascade,
  primary key (trackid, playlistid)
);

/* create coretrackssmartplaylists table */
create table coretrackssmartplaylists
(
  trackid integer not null references coretracks(trackid) on delete cascade,
  smartplaylistid integer not null references coresmartplaylists(smartplaylistid) on delete cascade,
  primary key (trackid, smartplaylistid)
);
