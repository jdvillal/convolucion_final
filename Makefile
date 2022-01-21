DEPS = common.h pgma_io.h conv_image.h data_structs.h parallel_tasks.h
CFLAGS = -c -g
DFLAGS = -lm -lpthread

.PHONY: all debug sanitize clean
all: server client

server: pgma_io.o data_structs.o conv_image.o parallel_tasks.o common.o  server.o $(DEPS)
	gcc -g -o $@  pgma_io.o data_structs.o conv_image.o parallel_tasks.o common.o server.o $(DFLAGS)

client: client.o common.o $(DEPS)
	gcc -o $@ client.o common.o $(DFLAGS)

data: createdata.o
	gcc -o $@ createdata.o

%.o: %.c $(DEPS)
	gcc $(CFLAGS) $< $(DFLAGS)

# Compila usando la opción -g para facilitar la depuración con gdb.
debug: DFLAGS = -g
debug: clean all

# Compila habilitando la herramienta AddressSanitizer para
# facilitar la depuración en tiempo de ejecución.
sanitize: DFLAGS = -fsanitize=address,undefined
sanitize: clean all

clean:
	rm -rf server client createdata *.o download_*.txt *.gch ./imagesOut ./data ./images lista.txt
