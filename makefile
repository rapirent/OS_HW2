all: server client
server:server.o api.o
	gcc -o server server.o api.o -lrt
client:client.o api.o
	gcc -o client client.o api.o -lrt
server.o:server.c api.h
	gcc -c server.c -lrt
client.o:client.c api.h
	gcc -c client.c -lrt
api.o:api.c api.h
	gcc -c api.c -lrt

