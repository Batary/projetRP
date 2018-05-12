#!/bin/bash

make

# pour avoir un seul des deux tests : -gene ou -local
# pour changer le repertoire de sortie : -out <dossier>
./steiner.exe -print -verbose -time 10 -file ../Instances/test/example.stp

#output avec un seul fichier:
./steiner.exe -print -verbose -time 10 -file ../Instances/D/d14.stp -out ../Output/test/ -gene


# output avec un dossier:
# ./steiner.exe -print -verbose -time 10 -dir ../Instances/B/ -out ../Output/B/ -local -pop 1 -verbose

