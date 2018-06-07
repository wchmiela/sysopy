#!/bin/bash

cmake . && make

T=(1 2 4 8)

SRC=feep.ascii.pgm
DEF=def.txt
OUT=outfeep.ascii.pgm

for i in 0 1 2 3
do
./zad1 -n ${T[i]} --input ${SRC} --def ${DEF} --output ${OUT}
done