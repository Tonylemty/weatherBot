#!/bin/bash
g++ weather.cpp -o weather -I./ -lcurl -std=c++17
chmod +x weather