# main.cc
# dingo-dbmanagertest.cc
# dingo-dbmanager.cc
# dingo-tagreader.cc
# dingo-sqlite3.cc
# dingo-dbmanobserver.cc
# dingo-converter.cc

main: main.o dingo-dbmanagertest.o dingo-dbmanager.o dingo-tagreader.o dingo-sqlite3.o dingo-dbmanobserver.o dingo-converter.o libsqlite3_d.a
	g++ -Wall -g main.o dingo-dbmanagertest.o dingo-dbmanager.o dingo-tagreader.o dingo-sqlite3.o dingo-dbmanobserver.o dingo-converter.o libsqlite3_d.a -ldl -o main `pkg-config --cflags --libs gtkmm-3.0 gstreamermm-0.10`

main.o: main.cc dingo-dbmanagertest.h dingo-dbmanager.h dingo-tagreader.h dingo-sqlite3.h dingo-dbmanobserver.h dingo-converter.h
	g++ -Wall -g -c main.cc `pkg-config --cflags --libs gtkmm-3.0 gstreamermm-0.10`

dingo-dbmanagertest.o: dingo-dbmanagertest.cc dingo-dbmanagertest.h dingo-dbmanager.h dingo-tagreader.h dingo-sqlite3.h dingo-dbmanobserver.h dingo-converter.h
	g++ -Wall -g -c dingo-dbmanagertest.cc `pkg-config --cflags --libs gtkmm-3.0 gstreamermm-0.10`

dingo-dbmanager.o: dingo-dbmanager.cc dingo-dbmanager.h dingo-tagreader.h dingo-sqlite3.h dingo-dbmanobserver.h dingo-converter.h
	g++ -Wall -g -c dingo-dbmanager.cc `pkg-config --cflags --libs gtkmm-3.0 gstreamermm-0.10`

dingo-tagreader.o: dingo-tagreader.cc dingo-tagreader.h
	g++ -Wall -g -c dingo-tagreader.cc `pkg-config --cflags --libs gtkmm-3.0 gstreamermm-0.10`

dingo-sqlite3.o: dingo-sqlite3.cc dingo-sqlite3.h
	g++ -Wall -g -c dingo-sqlite3.cc `pkg-config --cflags --libs gtkmm-3.0 gstreamermm-0.10`

dingo-dbmanobserver.o: dingo-dbmanobserver.cc dingo-dbmanobserver.h
	g++ -Wall -g -c dingo-dbmanobserver.cc `pkg-config --cflags --libs gtkmm-3.0 gstreamermm-0.10`

dingo-converter.o: dingo-converter.cc dingo-converter.h
	g++ -Wall -g -c dingo-converter.cc `pkg-config --cflags --libs gtkmm-3.0 gstreamermm-0.10`
	
libsqlite3_d.a: sqlite3.c sqlite3.h
	gcc -Wall -g -c sqlite3.c
	ar cr libsqlite3_d.a sqlite3.o
