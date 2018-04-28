#!/bin/bash

make

# pour avoir un seul des deux tests : -gene ou -local
# pour changer le repertoire de sortie : -out <dossier>
./steiner.exe -print -verbose -time 10 -file ../Instances/test/example.stp

