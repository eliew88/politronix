#!/bin/bash
# George Horrell's Politronix mySql Database Setup Script
# 07.19.16
# --------

EXPECTED_ARGS=2
E_BADARGS=65
MYSQL=`which mysql`
MYSQL_SERVICE="mysql"

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

if (( $(ps -ef | grep -v grep | grep $MYSQL_SERVICE | wc -l ) > 0 )) 
then 
    echo "MySQL server is running!"
else
    echo "Starting MySQL server."
    /etc/init.d/$MYSQL_SERVICE start
    echo "MySQL server started successfully."
fi

set -e
echo "Attempting to add new user, database and data table to your MySQL server."
echo "Enter your MySQL ROOT password in the ensuing prompt."
$MYSQL -uroot -p -e "$ALL"
echo "POLITRONIX db and table successfully created. Thanks for flying with GWPH airways."
