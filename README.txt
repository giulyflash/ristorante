
esercizio realizzato dallo studente 
Sergio Urbano
matricola: 10011342

COMPILAZIONE

Per compilare il sorgente è sufficiente [all'interno della directory
corrente] lanciare il comando:

	make

tale comando si occupera' di compilare l'esercizio e fornira' due
eseguibili: 

	cameriere
	cucina

e' inoltre possibile compilare solo il client o solo il server con
i comandi
	
	make cameriere
	make cucina

ESECUZIONE

Lanciando 

	./cucina <Port> 

sara' possibile avviare il server, mentre con 

	./cameriere <indirizzo/hostname server> <porta server> 

sara' possibile avviare il client e cominciare a testare la soluzione 
proposta.

NOTE

Il client non si avvia se prima non e' stato avviato il server [l'esecuzione
generera' un errore "connect error"].

E' consigliabile lanciare almeno 2-3 client [al massimo n ma bisogna modificare leggermente il codice]
in modo da poter notare la gestione delle notifiche.

Per ripulire la directory lanciare il comando

	make clean

Ogni volta che viene lanciato il make i vecchi file compilati vengono sostituiti
da quelli nuovi.
