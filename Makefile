default: shm_server.so shm_client.so
LDFLAGS= -lsporth -lsoundpipe -lsndfile -lm -ldl -lpthread -ljack

shm_server.so: shm_server.c
	gcc -g -fPIC -shared $^ -o $@ $(LDFLAGS)

shm_server: shm_server.c
	gcc -g $^ -o $@ $(LDFLAGS)

shm_client.so: shm_client.c
	gcc -g -fPIC -shared $^ -o $@ $(LDFLAGS)

install: shm_client.so shm_server.so
	install shm_client.so /usr/local/share/sporth/plugins/
	install shm_server.so /usr/local/share/sporth/plugins/

clean:
	rm -rf shm_client.so shm_server.so shm_server
