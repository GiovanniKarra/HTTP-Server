client: src/client.c
	@gcc -o client src/client.c

server: src/server.c src/process.c src/parse.c
	@gcc -o server src/server.c src/process.c src/parse.c

clean:
	@rm -f server client