# RPC (Draft)

The bridge subscribes the client to an RPC back-channel on connect:

	/clients/{client_id}/rpc/+/result

The back-channel subscription is managed by the bridge.
Clients are not allowed to subscribe to any client's back-channel manually
(including their own channel).

## API browsing / discovery

* The Client can get all available RPC APIs by subscribing to ```/rpc/+/api```
* Every RPC service publishes its API to ```/rpc/{service_id}/api```
	
TODO Define API format

## CALL / RESULT

* The client subscribes to ```/clients/{client_id}/rpc/+/result``` to get results from RPC services
* The client publishes an RPC call to ```/clients/{client_id}/rpc/{service_id}```
* The service responds to the RPC call at ```/clients/{client_id}/rpc/{service_id}/result```

### Example

Service [MPD](http://www.musicpd.org/doc/protocol/), using a UUID as client identifier):

	CALL /clients/EA81D912-2BB7-40F2-AE0F-1F0CD5F81469/rpc/mpd
	search any Burton


	RESULT /clients/EA81D912-2BB7-40F2-AE0F-1F0CD5F81469/rpc/mpd/result
	file: L01_play_along.mp3
	Last-Modified: 2014-02-04T20:24:38Z
	Time: 156
	Artist: Gary Burton
	Title: What Is Thing Called Love Trio
	Date: 2012

### Resources

* [WAMP - The WebSocket Application Messaging Protocol](http://wamp.ws)