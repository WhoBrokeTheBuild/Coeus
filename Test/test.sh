#!/bin/sh

SCRIPTDIR=$(dirname $0)

cd $SCRIPTDIR

pkill HTTPServer

cd ../HTTP
../Build/HTTPServer -f conf/main.cfg &
cd ..

cd $SCRIPTDIR
python http.py > http.log

pkill HTTPServer

echo "== Test Results =="
cat http.log
