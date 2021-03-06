#include"discordiador.h"

config_struct configuracion;

int main(int argc, char* argv[]) {

	t_log* logger;
	int id_patota=0;
	int id_tripulante=0;

	//Reinicio el anterior y arranco uno nuevo
	FILE* archivo = fopen("discordiador.log","w");
	fclose(archivo);
	logger = log_create("discordiador.log","discordiador",1,LOG_LEVEL_INFO);



	leer_config();
	leer_tareas("tareas.txt");
	int conexionMiRam = crear_conexion(configuracion.ip_miram,configuracion.puerto_miram);
	int conexionMongoStore = crear_conexion(configuracion.ip_mongostore, configuracion.puerto_mongostore);

	menu_discordiador(conexionMiRam, conexionMongoStore,logger,id_tripulante,id_patota);

	terminar_discordiador(conexionMiRam, conexionMongoStore, logger);


	return 0;


}

int menu_discordiador(int conexionMiRam, int conexionMongoStore,  t_log* logger, int id_patota,int id_tripulante) {
	int tipoMensaje = -1;

	while(1){
		//nuevoTripulante* tripulante = crearNuevoTripulante(id_tripulante,5,6,id_patota);

		t_paquete* paquete;
		char* leido = readline("");
		switch (codigoOperacion(leido)){
			case INICIAR_PATOTA:
				paquete = crear_paquete(INICIAR_PATOTA);
				char** parametros = string_split(leido, " ");
				log_info(logger, (char*)parametros[1]);

				//pcbPatota patota=crearPCB(id_patota);
				//agregar_a_paquete(paquete,patota,sizeof(pcbPatota));
				//tcbTripulante tripulante=crearTCB(id_tripulante,0,0);

				for(int i = 0; i < atoi(parametros[1]); i++){
					//Creacion de tripulantes
					//agregar_a_paquete(paquete, tripulante, sizeof(tcbTripulante));

					nuevoTripulante* tripulante = crearNuevoTripulante(id_tripulante,5,6,id_patota);

					agregar_a_paquete(paquete, tripulante, tamanioTripulante(tripulante));
					printf("\nSe han enviado  %d tripulantes",id_tripulante);
					enviar_paquete(paquete, conexionMiRam);
					id_tripulante++;
					free(tripulante);
				}
				//agregar_a_paquete(paquete, tripulante, tamanioTripulante(tripulante));
				//enviar_paquete(paquete, conexionMiRam);
				id_patota++;
				eliminar_paquete(paquete);
				break;

			case LISTAR_TRIPULANTES:
				enviar_header(LISTAR_TRIPULANTES, conexionMiRam);
				tipoMensaje = recibir_operacion(conexionMiRam);
				recibir_lista_tripulantes(tipoMensaje, conexionMiRam, logger);
				break;

			case OBTENER_BITACORA:
				enviar_header(OBTENER_BITACORA, conexionMongoStore);
				tipoMensaje = recibir_operacion(conexionMongoStore);
				t_list* lista = recibir_paquete(conexionMongoStore);
				char* mensaje = (char*)list_get(lista, 0);
				log_info(logger, mensaje);
				list_destroy(lista);
				break;

			case FIN:
				paquete = crear_paquete(FIN);
				enviar_paquete(paquete, conexionMiRam);
				enviar_paquete(paquete, conexionMongoStore);
				eliminar_paquete(paquete);
				return EXIT_FAILURE;

			default:
				mensajeError(logger);
				break;
		}
		free(leido);
		//free(tripulante);
	}
}

void leer_config(void){
	t_config * archConfig = config_create("discordiador.config");

	    configuracion.ip_miram = config_get_string_value(archConfig, "IP_MI_RAM_HQ");
	    configuracion.puerto_miram = config_get_string_value(archConfig, "PUERTO_MI_RAM_HQ");
	    configuracion.ip_mongostore = config_get_string_value(archConfig, "IP_I_MONGO_STORE");
	    configuracion.puerto_mongostore = config_get_string_value(archConfig, "PUERTO_I_MONGO_STORE");
	    configuracion.grado_multitarea= config_get_int_value(archConfig, "GRADO_MULTITAREA");
	    configuracion.algoritmo= config_get_string_value(archConfig, "ALGORITMO");
	    configuracion.quantum = config_get_int_value(archConfig, "QUANTUM");
	    configuracion.duracion_sabotaje = config_get_int_value(archConfig, "DURACION_SABOTAJE");
	    configuracion.retardo_cpu = config_get_int_value(archConfig, "RETARDO_CICLO_CPU");
}

void terminar_discordiador (int conexionMiRam, int conexionMongoStore, t_log* logger){

	log_destroy(logger);
	liberar_conexion(conexionMiRam);
	liberar_conexion(conexionMongoStore);
}


