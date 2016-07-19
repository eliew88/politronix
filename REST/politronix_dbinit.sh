#!/bin/bash
# George Horrell's Politronix mySql Database Setup Script
# 07.19.16
# --------

EXPECTED_ARGS=2
E_BADARGS=65
MYSQL=`which mysql`

Q1="CREATE DATABASE IF NOT EXISTS POLITRONIX;"
Q2="GRANT USAGE ON POLITRONIX.* TO $1@localhost IDENTIFIED BY '$2';"
Q3="GRANT ALL PRIVILEGES ON POLITRONIX.* TO $1@localhost;"
Q4="FLUSH PRIVILEGES;"
DB_CREATE="${Q1}${Q2}${Q3}${Q4}${Q5}"
SELECT_DB="use POLITRONIX;";
TABLE_CREATE="CREATE TABLE data (id INT UNSIGNED AUTO_INCREMENT, topic VARCHAR(20), score FLOAT, datetime DATETIME, PRIMARY KEY (id));"

ALL="${DB_CREATE}${SELECT_DB}${TABLE_CREATE}"

if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $0 dbuser dbpass"
    exit $E_BADARGS
fi

set -e
$MYSQL -uroot -p -e "$ALL"
echo "POLITRONIX db and data table successfully created"
