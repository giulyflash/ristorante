#include "basic.h"
#include "esame.h"


pacchetto *newPacchetto(void) {
	return(pacchetto*)malloc(sizeof(pacchetto));
}

void invia_menu(int connsd) {
	FILE *fd;
	char buffer[MAXLINE],str[MAXLINE];
	printf("ti invio il menu\n");
	fd=fopen("menu","r");
	if (fd==NULL){
		perror ("Non e' possibile aprire il file");
		exit(1);
	}
	while(!feof(fd)){
		fgets(str,MAXLINE,fd);
		Write(connsd,str,strlen(str));
		memset(str,'\0',strlen(str));
	}
 	strcpy(buffer,"***fine del file***\n");
 		Write(connsd,buffer,strlen(buffer));
	fclose(fd);
}

void leggi_menu(int sockfd) {
	char echo[MAXLINE];
	int flag=0;
	while(flag!=1) {
		Read(sockfd,echo,MAXLINE);
		if(strstr(echo,"***fine del file***") != NULL)
			flag=1;
		if(flag!=1) {
			printf("%s",echo);
			memset(echo,'\0',MAXLINE);
		}
	}
}

void stampa_ordine(pacchetto p) {
	int i=0;
	printf("cameriere %d tavolo %d\n", p.nome_cameriere,p.tavolo);
	while(p.ordine[i]!= '\0') {
		printf("hai ordinato %d porizioni di %d\n",p.ordine[i+1],p.ordine[i]);
		i+=2;
	}
	printf("\n");
}


void my_send(int protocollo, pacchetto p) {
	char send[sizeof(pacchetto)];
	p.protocollo = protocollo;
	memcpy(send,&p,sizeof(pacchetto));
	Write(sockfd,send,sizeof(send));
}

/*
void prepara_piatti2(int connsd, pacchetto p) {
        char numPiatto[MAXLINE];
        int qt,tempo, piatto;
        int i=0;
        pid_t pid;
        double tempo_medio = 0;
        char send[sizeof(pacchetto)];
        struct timeval          starttime,endtime;
        double  tempo_di_servizio = 0, tempo_totale = 0;
        if((pid=fork()) == 0) {
//              sleep(20);
                if ( gettimeofday(&starttime, NULL) < 0 )
                        err_sys("gettime error");
                printf("\n *** Il cuoco [un processo figlio con pid %d] comincia a preparare il piatto ***\n", getpid());
                printf("%d\n", *l_piatti[i+1]);
                while(l_piatti[i]!= '\0') {
                        switch(*l_piatti[i]) {
                        case 1:
                                piatto = 1;
                                printf("\nsto preparando il piatto %d\n", *l_piatti[i]);
                                break;
                        case 2:
                                piatto = 2;
                                printf("\nsto preparando il piatto %d\n", *l_piatti[i]);
                                break;
                        case 3:
                                piatto = 3;
                                printf("\nsto preparando il piatto %d\n", *l_piatti[i]);                                break;
                        case 4:
                                piatto = 4;
                                printf("\nsto preparando il piatto %d\n", *l_piatti[i]);                                break;
                        case 5:
                                piatto = 5;
                                printf("\nsto preparando il piatto %d\n", *l_piatti[i]);                                break;
                        case 6:
                                piatto = 6;
                                printf("\nsto preparando il piatto %d\n", *l_piatti[i]);                                break;
                        case 7:
                                piatto = 7;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", temporaneo->ordine[i], temporaneo->tavolo);
                                break;
                        case 8:
                                piatto = 8;
                                printf("sto preparando\nsto preparando il piatto %d per il tavolo %d\n", temporaneo->ordine[i], temporaneo->tavolo);
                                break;
                        case 9:
                                piatto = 9;
                                printf("\nsto preparando il piatto %d per il tavolo %d\n", temporaneo->ordine[i], temporaneo->tavolo);
                                break;
                        }
                        memset(numPiatto,'\0',sizeof(numPiatto));
                        qt = 1;
                        tempo = qt*piatto;
                        sleep(tempo);
                        if ( gettimeofday(&endtime,NULL) < 0 )
                                err_sys("gettime error");
                        tempo_totale += (tempo_di_servizio = (endtime.tv_sec+(endtime.tv_usec/1000000.0)) - (starttime.tv_sec+(starttime.tv_usec/1000000.0)));
                        printf("\nIl tempo di preparazione del piatto e' di %fl secondi\n",tempo_di_servizio);
                        snprintf(numPiatto, sizeof(numPiatto),"%d", temporaneo->ordine[i]);
                        p.protocollo = 7;
                        strcpy(p.messaggio,numPiatto);
                        memcpy(send,&p,sizeof(pacchetto));
                        Write(connsd,send,sizeof(send));
                        printf("\ne' pronto il piatto %d\n", *l_piatti[i]);                     i+=2;
                }
                tempo_medio += tempo_totale;
                *medio += tempo_totale;
                printf("\n *** Il tempo di evasione dell' ordine e' di %f secondi ***\n",tempo_totale);
                printf("\n *** Il tempo medio di evasione degli ordini e' di %f secondi ***\n",(*medio / numero_ordine));
                printf("\n *** processo con pid [%d] terminato ***\n", getpid());
                exit(1);
        }
//      head = cancella_pacchetto(*temporaneo,connsd);
}
*/
