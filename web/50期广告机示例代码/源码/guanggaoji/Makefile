SUBDIRS=./board-src/guanggaoji/ ./web-server/cgi-src/
CC=gcc

all:subdirs

subdirs:
	    for n in $(SUBDIRS);do $(MAKE) -C $$n || exit 1 ;done
		$(CC) ./board-src/main_server.c -o ./board-src/main_server	
clean:
	    for n in $(SUBDIRS);do $(MAKE) -C $$n clean;done
		rm ./board-src/main_server
	    rm board.tar.gz pcapp.tar.gz

boarddirs:
	mkdir -p ./project_ggj
	mkdir -p ./project_ggj/board
	mkdir -p ./project_ggj/board/guanggaoji

pcdirs:
	mkdir -p ./project_ggj/web-server
	mkdir -p ./project_ggj/web-server/cgi-bin
	

cpboardapp:
	cp ./board-src/main_server ./project_ggj/board/
	cp ./board-src/guanggaoji/background.jpg ./project_ggj/board/guanggaoji/
	cp ./board-src/guanggaoji/mplayer ./project_ggj/board/guanggaoji/
	cp ./board-src/guanggaoji/guanggaoji ./project_ggj/board/guanggaoji/
	cp ./board-src/guanggaoji/picture1 ./project_ggj/board/guanggaoji/ -R
	cp ./board-src/guanggaoji/picture2 ./project_ggj/board/guanggaoji/ -R
	cp ./board-src/guanggaoji/picture3 ./project_ggj/board/guanggaoji/ -R
	cp ./board-src/guanggaoji/video ./project_ggj/board/guanggaoji/ -R
	cp ./board-src/guanggaoji/list ./project_ggj/board/guanggaoji/ -R


pcapps:
	cp ./web-server/htdocs ./project_ggj/web-server -R
	cp ./web-server/cgi-src/*.cgi ./project_ggj/web-server/cgi-bin -R
	cp ./web-server/cgi-src/data.db ./project_ggj/web-server/cgi-bin/


install:boarddirs cpboardapp pcdirs pcapps
	tar -zcvf board.tar.gz  ./project_ggj/board/
	tar -zcvf pcapp.tar.gz ./project_ggj/web-server/cgi-bin/ ./project_ggj/web-server/htdocs/
	rm -R ./project_ggj 

	
	





	
