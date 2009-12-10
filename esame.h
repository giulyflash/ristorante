/*
 * esame.h
 *
 *  Created on: Apr 17, 2009
 *      Author: 10011342
 */

typedef struct pacchetto{
	int protocollo;
	int nome_cameriere;
	int tavolo;
	int ordine[50];
	int pronti;
	int esauriti;
	char messaggio[20];
	int modificato;
	int servito;
	int sollecito;
	int error;
	int flag;
	int conto;
	int ttl;
	double tempi[2];
	struct pacchetto *next;
}pacchetto;

typedef struct dispensa {
	int piatto_1[2];
	int piatto_2[2];
	int piatto_3[2];
	int piatto_4[2];
	int piatto_5[2];
	int piatto_6[2];
	int piatto_7[2];
	int piatto_8[2];
	int piatto_9[2];
}dispensa;


pacchetto *head, *tail;
dispensa *d;
int sockfd;
int connsd;

pacchetto *newPacchetto(void);
void invia_menu(int connsd);
void leggi_menu(int sockfd);
void stampa_ordine(pacchetto p);
void menu_cameriere();
void my_send(int protocollo, pacchetto p);
void leggi_ordine(pacchetto p, int i);
void gestisci_protocollo_server(pacchetto p, int cameriere);
void gestisci_protocollo_client(pacchetto p);
void coda_tavoli(pacchetto p);
void modifica_ordine(pacchetto p);
pacchetto *cancella_pacchetto(pacchetto p);
void reset(pacchetto p);
void stampa_statistiche();
void init_dispensa();
