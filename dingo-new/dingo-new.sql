/* default artist: artistid = 1, artistname = 'Unknown Artist' 
 * default album: albumid = 1, albumname = 'Unknown Album'
 * default playlistsortcolidx: 2 (track_title)
 * default playlistsorttype: 1 (Gtk::SORT_ASCENDING)
 */

/* CREATE 'CoreArtists' TABLE */
CREATE TABLE CoreArtists
(
  ArtistID INTEGER PRIMARY KEY,
  ArtistName TEXT CHECK(ArtistName != '') UNIQUE NOT NULL
);

INSERT INTO CoreArtists (ArtistName) VALUES ('Unknown Artist');


/* CREATE 'CoreAlbums' TABLE */
CREATE TABLE CoreAlbums
(
  AlbumID INTEGER PRIMARY KEY,
  AlbumName TEXT CHECK(AlbumName != '') UNIQUE NOT NULL
);

INSERT INTO CoreAlbums (AlbumName) VALUES ('Unknown Album');


/* CREATE 'CoreTracks' TABLE */
CREATE TABLE CoreTracks
(
  TrackID INTEGER PRIMARY KEY,
  ArtistID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreArtists(ArtistID) ON DELETE SET DEFAULT,
  AlbumID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreAlbums(AlbumID) ON DELETE SET DEFAULT,
  TrackNumber TEXT,
  TrackTitle TEXT CHECK(TrackTitle != '') NOT NULL,
  TrackGenre TEXT,
  TrackTime INTEGER NOT NULL,
  TrackSize INTEGER NOT NULL,
  TrackFileName TEXT NOT NULL,
  TrackURI TEXT UNIQUE NOT NULL,
  TrackYear INTEGER,
  TrackBitrate INTEGER NOT NULL,
  TrackDateAdded TEXT NOT NULL
);


/* CREATE 'CorePlaylists' TABLE */
CREATE TABLE CorePlaylists
(
  PlaylistID INTEGER PRIMARY KEY,
  PlaylistName TEXT CHECK(PlaylistName != '') NOT NULL,
  PlaylistSortColIdx INTEGER DEFAULT 2 NOT NULL,
  PlaylistSortType INTEGER DEFAULT 1 NOT NULL,
  PlaylistAutoStart INTEGER DEFAULT 0 NOT NULL
);

INSERT INTO CorePlaylists(PlaylistName) VALUES ('File Queue');


/* CREATE 'CoreTracksPlaylists' TABLE */
CREATE TABLE CoreTracksPlaylists
(
  TrackID INTEGER NOT NULL REFERENCES CoreTracks(TrackID) ON DELETE CASCADE,
  PlaylistID INTEGER NOT NULL REFERENCES CorePlaylists(PlaylistID) ON DELETE CASCADE,
  PRIMARY KEY (TrackID, PlaylistID)
);
