#!/bin/bash

make

# pour avoir un seul des deux tests : -gene ou -local
#pop : 1 = aléatoire, 2 = PCC, 3 = ACPM

./steiner.exe -print -verbose -pop 2 -time 10 -file ../Instances/B/b01.stp


# pour changer le repertoire de sortie : -out <dossier>
#output avec un seul fichier:
# ./steiner.exe -print -verbose -time 10 -file ../Instances/D/d14.stp -out ../Output/test/ -gene


# output avec un dossier:
# ./steiner.exe -print -verbose -time 10 -dir ../Instances/B/ -out ../Output/B/ -local -pop 1 -verbose