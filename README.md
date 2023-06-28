# README.md

A basic sender and receiver communicating with ESP-NOW while being connected to a WiFi gateway. The WiFi gateway SSID and password only need to be saved on the receiver side. The sender connects to a local hotspot created by the receiver and tries all channels from 1-13 until it successfully sends a packet using ESP-NOW.
