#!/bin/bash
#
# PRL projekt 2: Enumeration sort
# Autor: Jakub Pastuszek (xpastu00)
#

#pocet cisel bud zadam nebo 10 :)
if [ $# -lt 1 ];then 
    numbers=10;
elif [ $# -gt 1 ];then 
    echo "error: Too many arguments" >&2; exit 1
else
	re='^[0-9]+$'
	if ! [[ $1 =~ $re ]] ; then
		echo "error: Not a number" >&2; exit 1
	fi;
    numbers=$1;
fi;

#vyrobeni souboru s random cisly
dd if=/dev/random bs=1 count=$numbers of=numbers 2> /dev/null

procs=`expr $numbers + 1`

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o es es.cpp

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -np $procs es

#uklid
rm -f es numbers