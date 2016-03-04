La cucina deve poter gestire più ordinazioni contemporaneamente (ogni tavolo una ordinazione). Ogni tavolo e' identificato univocamente da un ID.

Il cameriere prende le ordinazioni (tramite palmare, ad esempio) e le comunica alla cucina.

Le applicazioni relative ai vari attori sono mandati in esecuzione su diversi apparati di cui si conosce solo l'hostname (non l'indirizzo IP).

In ogni momento, un ordine può essere modificato (nuovi "piatti" per ordine, eliminazione di vecchi "piatti" per ordine) sia dal cameriere (ripensamento del client) sia dalla cucina (esaurimento di qualche ingrediente).

La cucina avvisa il cameriere quando è pronta un'ordinazione. Solleciti devo essere inviati tra gli attori in caso di ritardo (ordine troppo lento ad essere evaso, cameriere disperso).

A fine giornata occorre fornire il tempo di ogni ordine evaso ed il tempo medio.

La definizione del protocollo di gioco e la sua implementazione sono lasciati allo studente che dovrà motiviare e commentare ogni scelta implementativa nella relazione legata al progetto.

L'implementazione deve essere tollerante ai guasti del sistema ed ad eventuali situazioni inconsistenti (input errato dell'utente, ordini diversi per lo stesso tavolo, scadenza di timeout...)