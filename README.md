# Proyecto final CONVOLUCION CONCURRENTE SINCRONIZADA
Proyecto final; procesamiento concurrente sincronizado de imagenes pgm.

## Servidor

El programa servidor *./server* tiene el siguiente modo de uso:
### HELP
Si ejecutamos el servidor con la ocpión -h (help), se muestra el siguiente mensaje de ayuda:
```
$ ./server -h
Procesador (sharpen) de imagenes multiples
uso:
 ./server <puerto> <ruta/nombre listado de imagenes> <cantidad_hilos> <cantidad_bloques>
 ./server -h
Opciones:
 -h			Ayuda, muestra este mensaje
```

### Ejecucion
El servidor necesita de 4 argumentos para ejecutarse, si un argumento no es proporcionado se imprime un mensaje de error.
El servidor recibe los argumentos en el siguiente orden:
./server <puerto> <ruta/nombre del listado de imagenes> <cantidad_hilos> <cantidad_bloques>
Ejemplo:
```
$ ./server 8080 lista.txt 4 5
```

Una vez ejecutado, el servidor ajusta los parametros y empieza los hilos que procesan las imagenes de forma concurrente mientras el hilo principal toma la tarea de escuchar instrucciones remotas en el <puerto> proporcionado como argumento.

Mientras se procesan las imagenes, los hilos proporcionan informacion que permite reconocer el estado del proceso, como se muesra en el siguiente ejemplo.
```
$ ./server 8080 lista.txt 4 2
MAIN: Leyendo el archivo "lista.txt"
MAIN: Hilo Lector/Planificador inciado.
MAIN: Hilo Almacenador inciado.
WORKER_THREAD 1: Sin imagenes que procesar. Suspendiendo...
WORKER_THREAD 2: Sin imagenes que procesar. Suspendiendo...
WORKER_THREAD 3: Sin imagenes que procesar. Suspendiendo...
MAIN: Cantidad inicial de hilos de procesamiento seteada a 4 threads.
MAIN: Cantidad fija de bloques seteada en 2.
WORKER_THREAD 4: Sin imagenes que procesar. Suspendiendo...
MAIN: Escuchando instrucciones remotas en el puerto 8080...
Lector/Planificador: Imagen images/1demoIn.pgm cargada en memoria y divida en 2 bloques.
WORKER_THREAD 1: Procesando imagen "images/1demoIn.pgm" bloque numero: 1 de 2
WORKER_THREAD 2: Procesando imagen "images/1demoIn.pgm" bloque numero: 2 de 2
WORKER_THREAD 3: Sin imagenes que procesar. Suspendiendo...
WORKER_THREAD 4: Sin imagenes que procesar. Suspendiendo...
WORKER_THREAD 1: Sin imagenes que procesar. Suspendiendo...
WORKER_THREAD 2: Sin imagenes que procesar. Suspendiendo...
Lector/Planificador: Imagen images/2demoIn.pgm cargada en memoria y divida en 2 bloques.
Lector/Planificador: Sin imagenes que cargar. Suspendiendo...
WORKER_THREAD 2: Procesando imagen "images/2demoIn.pgm" bloque numero: 1 de 2
WORKER_THREAD 1: Procesando imagen "images/2demoIn.pgm" bloque numero: 2 de 2
WORKER_THREAD 3: Sin imagenes que procesar. Suspendiendo...
WORKER_THREAD 4: Sin imagenes que procesar. Suspendiendo...
ALMACENADOR: Imagen "images/1demoIn.pgm" procesada con exito. La imagen resultante ha sido guardado en disco como: "imagesOut/sharpen_1_1demoIn.pgm"
WORKER_THREAD 1: Sin imagenes que procesar. Suspendiendo...
WORKER_THREAD 2: Sin imagenes que procesar. Suspendiendo...
ALMACENADOR: Imagen "images/2demoIn.pgm" procesada con exito. La imagen resultante ha sido guardado en disco como: "imagesOut/sharpen_2_2demoIn.pgm"

```

## Cliente

El programa *./servidor* 
### *HELP
Si ejecutamos el cliente con la opción -h (help), la salida del programa es la siguiente:
```
$ ./client -h
Cliente para enviar instrucciones al server de procesamiento de imagenes pgm.
uso:
 ./client <puerto>
 ./client -h
Opciones:
 -h			Ayuda, muestra este mensaje


```
### Ejecucion
Para ejecutar el cliente es necesario proporcionar como argumento el puerto en el que el server escucha instrucciones remotas.
./client <puerto>
Ejemplo:
```
$ ./client 8080
```

Si el servidor no ha sido iniciado previamente, el cliente mostrara un error de conexion:
```
$ ./client 8080
Error de conexión: Connection refused
```

Si el cliente se conecta de forma exitosa, el prompt ">>" indica al usuario que puede ingresar uno de los 3 comandos disponibles [add] [threads] [exit]
```
$ ./client 8080
Conectado exitosamente a localhost en el puerto 8080.
sharpen_client>>
```

#### comando add
El comando "add" permite al usuario indicarle al servidor que debe cargar y procesar una imagen adicional a las que ha cargado previamente.
Se usa de la siguiente forma:
```
sharpen_client>>add demoIn.pgm
EL archivo demoIn.pgm ha sido ingresado a la cola de procesamiento
```
Si el servidor no encuentra el archivo, se muestra el siguiente mensaje:
```
sharpen_client>>add demoOn.pgm
ERROR: El archivo ingresado no existe
```
#### comando threads
Si el usuario desea actualizar la cantidad de hilos de procesamiento, lo puede hacer meduante el comando "threads" de la siguiente manera:
```
sharpen_client>>threads 12
Cantidad de hilos de procesamientos cambiada con exito de 1 a 12
```
Toda instruccion enviada desde el cliente, tambien se ve reflejada mediante mensajes en consola del servidor:
```
MAIN: Comando remoto recibido>> add demoInn.pgm
	ERROR:El archivo demoInn.pgm no exite
MAIN: Comando remoto recibido>> threads 12 
MAIN: Cambiando a 12 hilos de procesamiento disponibles
> 
```
#### comando exit
Si el usuario desea terminar la ejecucion del cliente, lo puede hacer mediante el comando "exit" el cual no recibe argumento:
```
sharpen_client>>exit
Desconectando...
```
Cuando el cliente se desconecta, el servidor muestra un mensaje indicandolo:
```
MAIN: desconectando de localhost (127.0.0.1)
```

## COMPILACION

Se proporciona una herramienta llamada *./data* que genera dataset del tamano que queramos para facilitar el testeo del programa en diferentes escenarios.
Para compilar la herramienta:
```
$ make data
```
Para usar la herramienta ejecutamos *./data* seguido de la cantidad de imagenes que queremos en el dataset generado.
Por ejemplo, si queremos generar un dataset de 100 imagenes, ejecutamos:
```
$./data 100
```
Esto crea un directorio con 100 imagenes y un archivo txt con la lista de las rutas de las 100 imagenes.

Para compilar cliente y servidor:
```
$ make
```
Para compilar solo el servidor:
```
$ make server
```
Para compilar solo el cliente:
```
$ make client
```
Para eliminar archivos temporales .o .a y directorios/archivos generados durante el testeo del programa.
```
$ make clean
```
