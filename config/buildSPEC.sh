#!/bin/bash
DIR="/home/s0416306/LLVM/SPEC_CPU2006/benchspec/CPU2006/"
DESDIR="/home/s0416306/LLVM/final/"
mkdir ${DESDIR}o0
mkdir ${DESDIR}o1
mkdir ${DESDIR}o2
for i in $DIR*
do
  if [[ "$i" == $DIR[0-9][0-9][0-9].* ]]; then
    PROM=`basename "$i"`
    NAME=${PROM:4}
    runspec --config=megaca_o0.cfg --action=build --tune=base $NAME
    cp $i/exe/* ${DESDIR}o0/${NAME}_o0
    echo $NAME
  fi
done

for i in $DIR*
do
  if [[ "$i" == $DIR[0-9][0-9][0-9].* ]]; then
    PROM=`basename "$i"`
    NAME=${PROM:4}
    runspec --config=megaca_o1.cfg --action=build --tune=base $NAME
    cp $i/exe/* ${DESDIR}o1/${NAME}_o1
    echo $NAME
  fi
done

for i in $DIR*
do
  if [[ "$i" == $DIR[0-9][0-9][0-9].* ]]; then
    PROM=`basename "$i"`
    NAME=${PROM:4}
    runspec --config=megaca_o2.cfg --action=build --tune=base $NAME
    cp $i/exe/* ${DESDIR}o2/${NAME}_o2
    echo $NAME
  fi
done
