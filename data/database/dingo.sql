/* default artist: artistid = 1, artistname = 'Unknown Artist' 
 * default album: albumid = 1, albumname = 'Unknown Album'
 * default playlistsortcolidx: 2 (track_title)
 * default playlistsorttype: 1 (Gtk::SORT_ASCENDING)
 */

/* CREATE 'CoreArtists' TABLE */
CREATE TABLE CoreArtists 
(
  ArtistID INTEGER PRIMARY KEY,
  ArtistName TEXT CHECK(ArtistName <> '') UNIQUE NOT NULL
);

INSERT INTO CoreArtists (ArtistName) VALUES ('Unknown Artist');


/* CREATE 'CoreAlbums' TABLE */
CREATE TABLE CoreAlbums
(
  AlbumID INTEGER PRIMARY KEY,
  AlbumName TEXT CHECK(AlbumName <> '') UNIQUE NOT NULL
);

INSERT INTO CoreAlbums (AlbumName) VALUES ('Unknown Album');


/* CREATE 'CoreGenres' TABLE */
CREATE TABLE CoreGenres 
(
  GenreID INTEGER PRIMARY KEY,
  GenreName TEXT CHECK(GenreName <> '') UNIQUE NOT NULL
);

INSERT INTO CoreGenres (GenreName) VALUES ('Unknown Genre');


/* CREATE 'CoreYears' TABLE */
CREATE TABLE CoreYears 
(
  YearID INTEGER PRIMARY KEY,
  YearName TEXT CHECK(YearName <> '') UNIQUE NOT NULL
);

INSERT INTO CoreYears (YearName) VALUES ('Unknown Year');


/* CREATE 'CoreTracks' TABLE */
CREATE TABLE CoreTracks
(
  TrackID INTEGER PRIMARY KEY,
  ArtistID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreArtists(ArtistID) ON DELETE SET DEFAULT,
  AlbumID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreAlbums(AlbumID) ON DELETE SET DEFAULT,
  GenreID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreGenres(GenreID) ON DELETE SET DEFAULT,
  YearID INTEGER DEFAULT 1 NOT NULL REFERENCES CoreYears(YearID) ON DELETE SET DEFAULT,
  TrackNumber INTEGER,
  TrackTitle TEXT CHECK(TrackTitle <> '') NOT NULL,
  TrackComment TEXT DEFAULT '',
  TrackLyrics TEXT DEFAULT '',
  TrackTime INTEGER NOT NULL,
  TrackSize INTEGER NOT NULL,
  TrackFileName TEXT CHECK(TrackFileName <> '') NOT NULL,
  TrackFileExtension TEXT CHECK(TrackFileExtension <> '') NOT NULL,
  TrackURI TEXT CHECK(TrackURI <> '') UNIQUE NOT NULL,
  TrackSubtitleURI TEXT DEFAULT '',
  TrackBitrate INTEGER NOT NULL,
  TrackMIMEType TEXT CHECK(TrackMIMEType <> '') NOT NULL,
  TrackDateAdded TEXT CHECK(TrackDateAdded <> '') NOT NULL
);


/* CREATE 'CorePlaylists' TABLE */
CREATE TABLE CorePlaylists
(
  PlaylistID INTEGER PRIMARY KEY,
  PlaylistName TEXT CHECK(PlaylistName <> '') NOT NULL,
);

INSERT INTO CorePlaylists (PlaylistName) VALUES ('Play Queue');


/* CREATE 'CoreTracksPlaylists' TABLE */
CREATE TABLE CoreTracksPlaylists
(
  TrackID INTEGER NOT NULL REFERENCES CoreTracks(TrackID) ON DELETE CASCADE,
  PlaylistID INTEGER NOT NULL REFERENCES CorePlaylists(PlaylistID) ON DELETE CASCADE,
  PRIMARY KEY (TrackID, PlaylistID)
);
