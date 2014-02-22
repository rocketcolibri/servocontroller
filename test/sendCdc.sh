#!/bin/sh

echo -n "{ \"v\" : 1, \"cmd\":\"cdc\",\"sequence\":3,\"user\":\"gugus\",\"channels\":[ 100,101,102,103,104,105,106,107]}" | nc -w 1 -u -4 127.0.0.1 30001
