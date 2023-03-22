#!/bin/bash
clear
gcc sock_server.c -o server
gcc sock_client.c -o client
./server