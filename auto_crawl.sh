#!/bin/sh

PIDFILE="./auto_crawl.pid"

if [ -f "${PIDFILE}" ] ; then
	echo "already running"; 
else
	touch ${PIDFILE}

AUTO_AUTH_LOGIN="ivanpetrov5" \
AUTO_AUTH_PASSWORD="DevoureR" \
AUTO_DB_FILE="./auto.sqlite" \
	./auto_crawl

	rm -f ${PIDFILE}
fi
