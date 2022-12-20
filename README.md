# Progetto-APSE-Attention-Mechanism
Progetto Architetture e Programmazione dei Sistemi di Elaborazione 2022/2023

Per eseguire la parte sequenziale descrivo diverse fasi.
1) Esecuzione dell'algoritmo.
    Entriamo nella cartella "APSE-sequenziale" ed eseguiamo i seguenti comandi:
  - gcc att32c.c -lm
  - ./a.out -ds test_2048_48_32.ds -wq test_48_32_32.wq -wk test_48_32_32.wk -wv test_48_32_32.wv -bq test_32_32.bq -bk test_32_32.bk -bv test_32_32.bv -si 8 -n 64
          
2) Spostiamo il file che si è appena generato nella stessa cartella "out32_2048_8_64_48.ds2" in Programmi\ Ausiliari/leggiFile/input. Se già presente sovrascrivilo.

3) Nella cartella leggiFile eseguire i seguenti comandi, (serve per convertire il file in output ed i file dati in input in formato txt):
  - gcc leggiFile.c -lm
  - ./a.out
        
4) Apri, nella cartella Programmi\ Ausiliari/leggiFile/output, i file "out32_2048_8_64_48ds2.txt" e "test_2048_48_32os.txt" e copia, TRANNE LE PRIME DUE RIGHE DI ENTRAMBI I FILE, il loro contenuto nei file "MatrixA.txt" e "MatrixB.txt" che trovi nella cartella "Programmi\ Ausiliari/".

5) Eseguire due comandi (ciò serve per avere un controllo automatico tra la matrice in output e la matrice dei risultati). Gli assert che escono fuori si riferiscono comunque a piccole approssimazioni:
  - gcc compareMatrix.c
  - ./a.out
  
  TEMPO DI ESECUZIONE DELLA PARTE SEQUENZIALE: 0.088 ~ 0.141 secs
