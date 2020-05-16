#!/bin/bash
for i in o0/*
do
  PROM=`basename "$i"`
  echo $PROM
  readelf -s $i |less > "${i}_sym"
  objdump -D $i |less > "${i}_asm"
done
for i in o1/*
do
  PROM=`basename "$i"`
  echo $PROM
  readelf -s $i |less > "${i}_sym"
  objdump -D $i |less > "${i}_asm"
done
for i in o2/*
do
  PROM=`basename "$i"`
  echo $PROM
  readelf -s $i |less > "${i}_sym"
  objdump -D $i |less > "${i}_asm"
done
