/*
 * conexiones.c
 *
 *  Created on: 2 mar. 2019
 *      Author: utnso
 */

#include "utils.h"

void* serializar_paquete(t_paquete* paquete, int *bytes)
{
	*bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = malloc(*bytes);
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	return a_enviar;
}

char* deserializar_paquete(t_buffer* buffer) {
	char* mensaje;
	int largo = 0;

	void* stream = buffer->stream;

	memcpy(&largo, stream, sizeof(int));
	stream += sizeof(int); //Se desplaza un int
	memcpy(&mensaje, stream, largo + 1);

	return mensaje;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		socket_cliente = 0;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	int tamanioMensaje = strlen(mensaje);
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size =  sizeof(int) + strlen(mensaje) + 1;; //se guarda el tamaño del stream del buffer en size
	paquete->buffer->stream = malloc(paquete->buffer->size); //se reserva espacio del tamaño del stream del buffer

	void* stream = malloc(paquete->buffer->size);

	memcpy(stream, &tamanioMensaje, sizeof(int)); //se guarda primer el int que contiene tamaño del stream del buffer
	memcpy(stream + sizeof(int), &mensaje, tamanioMensaje + 1);
	//se guarda en mensaje con el centinela, desplazando el stream un int por el int tamanioMensaje


	paquete->buffer->stream = stream;

	int bytes = 0;
	void* mensaje_serializado = serializar_paquete(paquete, &bytes);


	if(send(socket_cliente, mensaje_serializado, bytes, 0) == -1)
		printf("Error al enviar el mensaje.");

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	free(mensaje_serializado);
}

char* recibir_mensaje(int socket_cliente)
{
	t_log* logger = log_create("recibir.log", "RECIBIENDO", true, LOG_LEVEL_TRACE);

	t_paquete* paquete = malloc(sizeof(paquete));
	char* mensaje = "";
	int largo = 0;
	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(int), 0);
	log_trace(logger, "Codigo_opeacion: %d", paquete->codigo_operacion);
	switch(paquete->codigo_operacion) {
	    case MENSAJE:
	    	recv(socket_cliente, &(paquete->buffer->size), sizeof(int), 0);
	    	//log_trace(logger, "Tamanio del stream buffer: %d", paquete->buffer->size);
	    	//paquete->buffer->stream = malloc(paquete->buffer->size);
	    	recv(socket_cliente, &(paquete->buffer->stream), paquete->buffer->size, 0);
	    	void* stream = paquete->buffer->stream;

	    	memcpy(&largo, stream, sizeof(int));
	    	log_trace(logger, "Tamanio del string: %d", largo);
	    	stream += sizeof(int); //Se desplaza un int en el stream
	    	memcpy(&mensaje, stream, paquete->buffer->size);

	        //mensaje = deserializar_paquete(buffer);
	        break;
	    default:
	    	printf("Código no reconocido :(\n");
	    	break;
	}

	return mensaje;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
