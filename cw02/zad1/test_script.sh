#!/bin/bash

RESULTS=wyniki.txt
cmake . && make

M=(250000 1812 244 288)
P=(350000 2812 312 388)
N=(4 512 4096 8192)

SRC=file
SRC_CPY=filecopy
TAR=target

for i in 0 1 2 3
do
echo ">>>Generowanie pliku. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${M[i]}" >> ${RESULTS}
./zad1 --generate --name ${SRC} -m ${N[i]} -n ${M[i]}

cp ${SRC} ${SRC_CPY}
echo ">>>Sortowanie - funkcje systemowe. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${M[i]}" >> ${RESULTS}
./zad1 --sort --name ${SRC} -n ${N[i]} -m ${M[i]} --sys --test >> ${RESULTS}
rm ${SRC}
mv ${SRC_CPY} ${SRC}

cp ${SRC} ${SRC_CPY}
echo ">>>Sortowanie - funkcje biblioteczne. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${M[i]}" >> ${RESULTS}
./zad1 --sort --name ${SRC} -n ${N[i]} -m ${M[i]} --lib --test >> ${RESULTS}
rm ${SRC}
mv ${SRC_CPY} ${SRC}

echo ">>>Kopiowanie - funkcje systemowe. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${M[i]}" >> ${RESULTS}
./zad1 --copy --name ${SRC} --target ${TAR} -n ${N[i]} -m ${M[i]} --sys --test >> ${RESULTS}
rm ${TAR}

echo ">>>Kopiowanie - funkcje biblioteczne. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${M[i]}" >> ${RESULTS}
./zad1 --copy --name ${SRC} --target ${TAR} -n ${N[i]} -m ${M[i]} --lib --test >> ${RESULTS}
rm ${TAR}
done

rm ${SRC}

for i in 0 1 2 3
do
echo ">>>Generowanie pliku. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${P[i]}" >> ${RESULTS}
./zad1 --generate --name ${SRC} -m ${N[i]} -n ${P[i]}

cp ${SRC} ${SRC_CPY}
echo ">>>Sortowanie - funkcje systemowe. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${P[i]}" >> ${RESULTS}
./zad1 --sort --name ${SRC} -n ${N[i]} -m ${P[i]} --sys --test >> ${RESULTS}
rm ${SRC}
mv ${SRC_CPY} ${SRC}

cp ${SRC} ${SRC_CPY}
echo ">>>Sortowanie - funkcje biblioteczne. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${P[i]}" >> ${RESULTS}
./zad1 --sort --name ${SRC} -n ${N[i]} -m ${P[i]} --lib --test >> ${RESULTS}
rm ${SRC}
mv ${SRC_CPY} ${SRC}

echo ">>>Kopiowanie - funkcje systemowe. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${P[i]}" >> ${RESULTS}
./zad1 --copy --name ${SRC} --target ${TAR} -n ${N[i]} -m ${P[i]} --sys --test >> ${RESULTS}
rm ${TAR}

echo ">>>Kopiowanie - funkcje biblioteczne. Rozmiar rekordu:${N[i]}. Ilosc rekordow:${P[i]}" >> ${RESULTS}
./zad1 --copy --name ${SRC} --target ${TAR} -n ${N[i]} -m ${P[i]} --lib --test >> ${RESULTS}
rm ${TAR}
done

rm ${SRC}
