#!/bin/bash
function gen_asm {
	for i in $(seq 0 31); do
		echo "global exc_handler_$i"
	done

	echo

	for i in $(seq 0 31); do
		echo "exc_handler_$i:"
		echo -e "\tjmp exc_handler_$i"
		echo 
	done
}

function gen_header {
	echo '#include "kerndefs.h"'
	echo
	for i in $(seq 0 31); do
		echo "extern void exc_handler_$i();"
	done

	echo 

	echo "uint32_t tab[32]={"
	for i in $(seq 0 31); do
		echo "(uint32_t)exc_handler_$i,"
	done
	echo "};"
}
gen_asm > exc.asm
gen_header > exc.h
