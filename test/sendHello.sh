#!/bin/sh

echo -n "{ \"v\" : 1, \"cmd\":\"hello\",\"sequence\":3,\"user\":\"Lorenz\"}" | nc -w 1 -u -4 127.0.0.1 30001
