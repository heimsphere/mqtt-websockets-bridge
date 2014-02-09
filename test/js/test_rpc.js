describe("message subscription", function() {
	it("should receive published message", function() {
		var connection = MQTT.connect('ws://127.0.0.1:8080/', function() {

			var message = 'hello world'
			var received_message;

			/* client is already subscribed to RPC backchannel */
			var echo_service = connection.service('echo');

			echo_service.subscribe();

			echo_service.on('data', function(data) {
				received_message = data;
			});
			echo_service.push('hello world');
			/* SUBSCRIBE to service method */
			// mpd.on('status', function(data) {
			// 	console.log("MPD status\n" + data);
			// });



			// echo_service.push('echo', message, function(data) {
			// 				received_message = data;
			// 			});

			waitsFor(function() {
				return received_message;
			}, "Should receive echo request response", 500);

			runs(function() {
				expect(received_message).toEqual(message);
			});
		});
	});
});
