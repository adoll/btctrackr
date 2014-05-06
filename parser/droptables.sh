#!/bin/bash
 
EXPECTED_ARGS=0
E_BADARGS=65
MYSQL=`which mysql`
 
Q1="USE test;"
Q2="DROP TABLE test;"
SQL="${Q1}${Q2};"
 
if [ $# -ne $EXPECTED_ARGS ]
then
  echo "Usage: $0"
  exit $E_BADARGS
fi
 
$MYSQL -uroot -e "$SQL"
