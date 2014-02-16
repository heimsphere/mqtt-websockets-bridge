// describe("message subscription", function() {
// 	it("should receive published message", function() {
// 		var connection = MQTT.connect('ws://127.0.0.1:8080/', function() {
// 
// 			/*
// 				- a separate queue for events 
// 				- a separate queue for RPC calls ?
// 			
// 				Do not distinguish between RPC and PUBLISH events on the
// 				receiver side !!!
// 			
// 			*/
// 			var mpd = connection.service('mpd/ruben'); /* SUBSCRIBE RPC/mpd/ruben/# */
// 
// 			mpd.on('status', function(data) {
// 				console.log("status changed");
// 				console.log(data);
// 			});
// 
// 			mpd.play(); /* mpd.send("CALL", "play"); */
// 
// 			mpd.on('search', function(data) {
// 
// 			});
// 
// 			mpd.publish('search', function(data) {
// 
// 			});
// 
// 			// error through timeout or published by MPD
// 			mpd.on('error', function(data)) {
// 
// 			}
// 
// 			mpd.request('search', 'any Burton', 500);
// 
// 			// create a timer that emits 
// 
// 
// 			/* through 
// 			mpd.search('any Burton', function(data) {
// 				
// 			} , function(error) {
// 			}, 500);
// 			
// 			// connection.send('REQUEST', "/mpd/search", "search any Burton")
// 			
// 			service.request('search', function(data) {
// 				
// 			});
// 			
// 			
// 			
// 
// 			var message = 'hello world'
// 			var received_message;
// 
// 			/* client is already subscribed to RPC backchannel */
// 			var echo_service = connection.service('echo');
// 
// 			/* the service is a channel 
// 				we have to distinguish between unicast and multicast publishes
// 				use the verb !!!!
// 			*/
// 
// 
// 			echo_service.subscribe(); // 'REQUEST /echo/#'
// 
// 
// 			echo_service.on('data', function(data) {
// 				received_message = data;
// 			});
// 			echo_service.push('hello world');
// 			/* SUBSCRIBE to service method */
// 			// mpd.on('status', function(data) {
// 			// 	console.log("MPD status\n" + data);
// 			// });
// 
// 
// 			// all messages should be send to the service
// 
// 			// echo_service.push('echo', message, function(data) {
// 			// 				received_message = data;
// 			// 			});
// 
// 			waitsFor(function() {
// 				return received_message;
// 			}, "Should receive echo request response", 500);
// 
// 			runs(function() {
// 				expect(received_message).toEqual(message);
// 			});
// 		});
// 	});
// });
