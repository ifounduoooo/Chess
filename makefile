CC=gcc
CFLAGS=-I. -I./include -std=c99
BUILDDIR=./build
OBUILDDIR=./lib
SRCBUILDDIR=./src
INCLUDEDIR=./include

$(shell mkdir -p $(BUILDDIR) $(OBUILDDIR))

server: $(OBUILDDIR)/chess_server.o $(OBUILDDIR)/server_opengl.o
	$(CC) -o $(BUILDDIR)/server $(OBUILDDIR)/chess_server.o $(OBUILDDIR)/server_opengl.o -lpthread -lglut -lGLU -lGL -lm

client: $(OBUILDDIR)/chess_client.o $(OBUILDDIR)/client_opengl.o
	$(CC) -o $(BUILDDIR)/client $(OBUILDDIR)/chess_client.o $(OBUILDDIR)/client_opengl.o -lpthread -lglut -lGLU -lGL -lm

all: $(OBUILDDIR)/chess_server.o  $(OBUILDDIR)/chess_client.o $(OBUILDDIR)/client_opengl.o $(OBUILDDIR)/server_opengl.o
	$(CC) -o $(BUILDDIR)/server $(OBUILDDIR)/chess_server.o $(OBUILDDIR)/server_opengl.o -lpthread -lglut -lGLU -lGL -lm
	$(CC) -o $(BUILDDIR)/client $(OBUILDDIR)/chess_client.o $(OBUILDDIR)/client_opengl.o -lpthread -lglut -lGLU -lGL -lm

$(OBUILDDIR)/chess_server.o:  $(SRCBUILDDIR)/chess_server.c $(INCLUDEDIR)/server_opengl.h
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $(SRCBUILDDIR)/chess_server.c -o $(OBUILDDIR)/chess_server.o -lpthread

$(OBUILDDIR)/chess_client.o:  $(SRCBUILDDIR)/chess_client.c $(INCLUDEDIR)/client_opengl.h
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c  $(SRCBUILDDIR)/chess_client.c -o $(OBUILDDIR)/chess_client.o -lpthread

$(OBUILDDIR)/client_opengl.o:  $(SRCBUILDDIR)/client_opengl.c $(INCLUDEDIR)/client_opengl.h
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c  $(SRCBUILDDIR)/client_opengl.c -o $(OBUILDDIR)/client_opengl.o -lglut -lGLU -lGL -lm

$(OBUILDDIR)/server_opengl.o:  $(SRCBUILDDIR)/server_opengl.c $(INCLUDEDIR)/server_opengl.h
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c  $(SRCBUILDDIR)/server_opengl.c -o $(OBUILDDIR)/server_opengl.o -lglut -lGLU -lGL -lm

clean:
	rm -rf $(BUILDDIR) $(OBUILDDIR)
