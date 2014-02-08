# RPC over MQTT (Draft)

## About

The purpose of this document is to describe an adressing scheme
for unicast communication between MQTT clients.

An MQTT client sends a request to an MQTT service.
The service processes the requests and sends the response 
back to the client which send the request.

Client and service are both MQTT clients and connected through an MQTT broker.

	MQTT Client (Client) --> [REQUEST] --> Broker --> MQTT Client (Service)
	MQTT Client (Client) <-- Broker <-- [RESPONSE] <-- MQTT Client (Service)

## Addressing

The source and destination address can be transparently encoded 
in the message topic. When can then simply use PUBLISH/SUBSCRIBE 
to handle the unicast communication.

### Security

Clients must not be allowed to subscribe to other client's back-channels.
A bridge should transparently handle the back-channels of it's clients.

### Addressing Scheme

	_RPC/{message_direction}/{source_path}/{service_id}/{service_path}

* ```message_direction``` : in (request) | out (response)
* ```client_id``` : A unique id for the client (in the context of the bridge)
* ```bridge_id``` :	The MQTT client of the registered bridge
* ```source_path``` := ```bridge_id```/```client_id```
* ```service_id``` : The name/id of the service
* ```service_path``` : The service method path

A service can simply replace the topic prefix  ```_RPC/in``` with ```_RPC/out``` to respond to a request:

	Request  --> _RPC/in/{source_path}/{service_id}/{service_path}
	Response <-- _RPC/out/{source_path}/{service_id}/{service_path}

### Request

#### SUBSCRIBE

A service must subscribe to request messages:

	_RPC/in/+/+/{service_id}/#

#### PUBLISH

A client must publish request messages to:

	_RPC/in/{source_path}/{service_id}/{service_path}

### Response

#### SUBSCRIBE

A client can subscribe to the back-channel of a single service:

	_RPC/out/{source_path}/{service_id}/#

or to the back-channel of all services:

	_RPC/out/{source_path}/#

##### Bridge

A bridge must subscribe to the back-channel of all it's clients:

	_RPC/out/{bridge_id}/#

#### PUBLISH

A service must publish the response message to:

	_RPC/out/{source_path}/{service_id}/{service_path}


### Example

Service [MPD](http://www.musicpd.org/doc/protocol/), using a UUID as client identifier)

* bridge_id : **mybridge**
* client_id : **myclient**
* service : **mpd**
* service_path : **search**

#### Request

**_RPC/in/mybridge/myclient/mpd/search**

	search any Burton

#### Response

**_RPC/out/mybridge/myclient/mpd/search**

	file: L01_play_along.mp3
	Last-Modified: 2014-02-04T20:24:38Z
	Time: 156
	Artist: Gary Burton
	Title: What Is Thing Called Love Trio
	Date: 2012

## Service API Discovery

### PUBLISH

A service publishes retained API messages to:

	_RPC/api/{service_id}/{service_path}

for all of it's service  methods.

### SUBSCRIBE

When client subscribes to

	_RPC/api/+/{service_path}

it will receive API messages from all services providing the given service path

When a client subscibes to 

	_RPC/api/{service_id}/#

it will receive API messages for all service paths provided by the given service

### Other Resources

* [WAMP - The WebSocket Application Messaging Protocol](http://wamp.ws)
