#!/bin/bash

curl -X POST "http://192.168.0.129/console/baud?rate=115200"
curl -X POST "http://192.168.0.129/console/fmt?fmt=8E1"
curl -X POST "http://192.168.0.129/pins?reset=0&isp=2&conn=-1&ser=-1&swap=0&rxpup=1"
