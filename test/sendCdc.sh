#!/bin/sh

echo -n "{ \"v\" : 1, \"cmd\":\"cdc\",\"sequence\":1,\"user\":\"gugus\",\"channels\":[ 100,101,102,103,104,105,106,107]}" | nc -w 1 -u -4 192.168.10.112 30001
