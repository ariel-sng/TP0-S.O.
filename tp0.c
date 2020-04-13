/*
 * main.c
 *
 *  Created on: 28 feb. 2019
 *      Author: utnso
 */

#include "tp0.h"

//TODO
t_log* iniciar_logger(void)
{
	return log_create("tp0.log", "tp0", true, LOG_LEVEL_TRACE);
}

//TODO
t_config* leer_config(void)
{
	return config_create("tp0.config");
}

//TODO
void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	//Y por ultimo, para cerrar, hay que liberar lo que utilizamos (conexion, log y config) con las funciones de las commons y del TP mencionadas en el enunciado
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/
	int conexion;
	char* ip;
	char* puerto;

	t_log* logger;
	t_config* config;

	logger = iniciar_logger();

	//Loggear "soy un log"

	config = leer_config();
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");


	log_trace(logger, puerto);
	log_trace(logger, ip);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	//antes de continuar, tenemos que asegurarnos que el servidor esté corriendo porque lo necesitaremos para lo que sigue.

	//crear conexion
	conexion = crear_conexion(ip, puerto);
	//enviar mensaje
	enviar_mensaje("123456789", conexion);
	//recibir mensajte
	//log_trace(logger, "empieza a recibir mensaje");
	char* mensajeRecibido = recibir_mensaje(conexion);
	//loguear mensaje recibido
	log_trace(logger, mensajeRecibido);

	log_trace(logger, "Fin");
	terminar_programa(conexion, logger, config);

	return 0;
}

