DEPS = common.h pgma_io.h conv_image.h data_structs.h parallel_tasks.h
CFLAGS = -Wall -c
DFLAGS = -lm

.PHONY: all debug sanitize clean
all: server client

server: server.o common.o pgma_io.o conv_image.o data_structs.o parallel_tasks.o $(DEPS)
	gcc -o $@ server.o common.o pgma_io.o conv_image.o data_structs.o parallel_tasks.o $(DFLAGS)

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
	rm -rf server client createdata *.o download_*.txt *.gch images.txt ./images
