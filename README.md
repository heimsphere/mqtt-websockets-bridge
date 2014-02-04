About
=========================

Proof-of-concept [MQTT](http://mqtt.org/) to Websockets bridge.

The bridge forwards messages from websocket 
clients to the MQTT broker and vice versa.

## Getting Started

1. Install the dependencies and start the mosquitto server.

		mosquitto
		
2. Build and start the bridge.

		cd mqtt-websockets-bridge/src
		make
		./bridge
		
3. Open [ws.html](ws.html) in multiple browser instances.

4. Publish a message using the mosquitto client.
		
		mosquitto_pub -t /foo/bar -m "Hello World"

## Protocol

### PUB/SUB

see [PUBSUB documentation](doc/PUBSUB.md)

### RPC

see [RPC documentation](doc/RPC.md) 

#### Dependencies

* [libwebsockets](http://libwebsockets.org/trac/libwebsockets) 1.23
* [mosquitto](http://mosquitto.org/) 1.2.3
* A websockets capable browser
* A posix compliant system (Only tested on OS X 10.9.1)

### Clients

see [MQTT Clients](https://github.com/heimsphere/mqtt-clients)

### Limitations

The event loop integration of mosquitto and websockets is very simple.
Both loops simply run alternating for 20 milliseconds.

Currently only simple subscription is possble by sending the topic path 
through the websockets connection. This is subject to change.

### TODO

* Benchmark 
* Tests