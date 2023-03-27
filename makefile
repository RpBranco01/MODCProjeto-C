OBJ_dir = obj
CLIENTE = client.o
SERVER = server.o
client.o = 
server.o =  
CC = gcc -Wall -g

vpath %.o $(OBJ_dir)
all: client server 

server: $(SERVER)
		$(CC) $(addprefix $(OBJ_dir)/,$(SERVER)) -o bin/server -lrt -pthread -g

client: $(CLIENTE)
		$(CC) $(addprefix $(OBJ_dir)/,$(CLIENTE)) -o  bin/client -lrt -pthread -g 

%.o: src/%.c $($@)
		$(CC) -I include -o $(OBJ_dir)/$@ -c $<

CLEAN_OBJS = client.o server.o
CLEAN_EXECS = bin/client bin/server		

clean:
	rm -f $(addprefix $(OBJ_DIR)/,$(CLEAN_OBJS)) lib/client-lib.o $(CLEAN_EXECS) *pb-c.[ch]