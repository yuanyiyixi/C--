CC=gcc
LFLAGS=-lcgi -lsqlite3 -lpthread

all:
	$(CC) login.c -o login.cgi $(LFLAGS)
	$(CC) user.c -o user.cgi $(LFLAGS)
	$(CC) video.c -o video.cgi $(LFLAGS)
	$(CC) status.c -o status.cgi $(LFLAGS)
	$(CC) clock.c -o clock.cgi $(LFLAGS)
	$(CC) picture1.c -o picture1.cgi $(LFLAGS)
	$(CC) picture2.c -o picture2.cgi $(LFLAGS)
	$(CC) picture3.c -o picture3.cgi $(LFLAGS)
	$(CC) text.c -o text.cgi $(LFLAGS)

clean:
	rm login.cgi
	rm user.cgi
	rm video.cgi
	rm status.cgi
	rm clock.cgi
	rm picture1.cgi
	rm picture2.cgi
	rm picture3.cgi
	rm text.cgi
