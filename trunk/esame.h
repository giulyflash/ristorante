/*
 * esame.h
 *
 *  Created on: Apr 17, 2009
 *      Author: 10011342
 */

/*definizione della struttura pacchetto*/
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

/*	definizione della struttura dispensa
 * 	ogni piatto e' un array di interi formato da due campi:
 *	d->piatto_i[0] = quantita' disponibile in dispensa
 *  d->piatto_i[1] = prezzo unitario del piatto
 */
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

/********** prototipi funzioni lato client ******************/
void gestisci_protocollo_client(pacchetto p);
void gestisci_input(char *input_cameriere);
void nuovo_ordine();
pacchetto scegli_piatti(pacchetto tmp);
void modifica_ordine_client(pacchetto p);
void servi(pacchetto p);
void invia_sollecito();
void richiedi_conto();
void esci() ;
/************************************************************/



/********** prototipi funzioni lato server ******************/
void gestisci_protocollo_server(pacchetto p, int cameriere);
void modifica_ordine_server(pacchetto p);
void handler(int signum);
void inserisci_pacchetto(pacchetto *p);
pacchetto *cancella_pacchetto(pacchetto p);
void stampa_lista(pacchetto *p);
void lista_piatti_attesa(pacchetto p);
void servi_piatto(pacchetto p, int cameriere, int i, int tot);
void prepara_piatti(pacchetto p);
void leggi_ordine(pacchetto p, int i);
void evadi_ordine(pacchetto p);
void stampa_statistiche();
void memorizza_info(pacchetto p);
void gestisci_sollecito(pacchetto p);
void invia_conto(pacchetto p);
void shared();
/************************************************************/


/********** prototipi funzioni condivise ******************/
pacchetto *newPacchetto(void);
void invia_menu(int connsd);
void leggi_menu(int sockfd);
void stampa_ordine(pacchetto p);
void menu_cameriere();
void my_send(int protocollo, pacchetto p);
void coda_tavoli(pacchetto p);
void init_dispensa();
/************************************************************/
