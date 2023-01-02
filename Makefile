CFLAGS_GERANT_sql = -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../Administrateur -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -I/usr/include/mysql -m64 -L/usr/lib64/mysql
CFLAGS_GERANT = -Wno-unused-parameter -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../Administrateur -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++
LDFLAGS_GERANT = -Wno-unused-parameter /usr/lib64/libQt5Widgets.so /usr/lib64/libQt5Gui.so /usr/lib64/libQt5Core.so /usr/lib64/libGL.so -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl
OBJECTS_GERANT = maingerant.o moc_windowgerant.o windowgerant.o Semaphores.o

CFLAGS_CLIENT = -Wno-unused-parameter -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++
LDFLAGS_CLIENT = -Wno-unused-parameter /usr/lib64/libQt5Widgets.so /usr/lib64/libQt5Gui.so /usr/lib64/libQt5Core.so /usr/lib64/libGL.so -lpthread
OBJECTS_CLIENT = mainclient.o moc_windowclient.o windowclient.o

LDFLAGS_BD = -I/usr/include/mysql -m64 -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl

CFLAGS_FICHIER_CLIENT = -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_Exercice1 -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++

OBJECTS = $(OBJECTS_CLIENT) $(OBJECTS_GERANT) FichierClient.o

CC = g++

EXECS = Client Serveur Publicite Caddie CreationBD AccesBD Gerant

.SILENT:
all: $(EXECS)

Client: $(OBJECTS_CLIENT)
	$(CC) -o Client $(OBJECTS_CLIENT) $(LDFLAGS_CLIENT)
	echo Création Client

mainclient.o:
	$(CC) -c mainclient.cpp $(CFLAGS_CLIENT)
	echo Compilation mainclient.cpp

moc_windowclient.o:
	$(CC) -c moc_windowclient.cpp $(CFLAGS_CLIENT)
	echo Compilation moc_windowclient.cpp

windowclient.o:
	$(CC) -c windowclient.cpp $(CFLAGS_CLIENT)
	echo Compilation windowclient.cpp


Gerant: $(OBJECTS_GERANT) 
	$(CC) -o Gerant $(OBJECTS_GERANT) $(LDFLAGS_GERANT)
	echo Création Gerant

maingerant.o:
	$(CC) -c maingerant.cpp $(CFLAGS_GERANT)
	echo Compilation maingerant.cpp

windowgerant.o:
	$(CC) -c windowgerant.cpp $(CFLAGS_GERANT_sql)
	echo Compilation windowgerant.cpp

moc_windowgerant.o:
	$(CC) -c moc_windowgerant.cpp $(CFLAGS_GERANT)
	echo Compilation moc_windowgerant.cpp

FichierClient.o :
	$(CC) -c FichierClient.cpp $(CFLAGS_FICHIER_CLIENT)
	echo Compile FichierClient.cpp

Serveur: FichierClient.o Semaphores.o
	$(CC) -o Serveur Serveur.cpp FichierClient.o Semaphores.o $(LDFLAGS_BD)
	echo Création Serveur

CreationBD:
	$(CC) -o CreationBD CreationBD.cpp $(LDFLAGS_BD)
	echo Création CreationBD

Caddie:
	$(CC) -o Caddie Caddie.cpp $(LDFLAGS_BD)
	echo Création Caddie

AccesBD:
	$(CC) -o AccesBD AccesBD.cpp $(LDFLAGS_BD)
	echo Création AccesBD

Publicite:
	$(CC) -o Publicite Publicite.cpp
	echo Création Publicite

Semaphores.o:
	$(CC) -c Semaphores.cpp 
	echo Compilation Semaphores.cpp

Client_clean:
	rm -rf $(OBJECTS_CLIENT) Client

Gerant_clean:
	rm -rf $(OBJECTS_GERANT) Gerant

clean:
	rm -rf $(OBJECTS) $(EXECS)
