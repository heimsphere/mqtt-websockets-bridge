describe("message subscription", function() {
	it("should receive published message", function() {

		var received_message;

		var connection = MQTT.connect('ws://127.0.0.1:8080/', function() {
			var queue = connection.topic('/foo/bar');
			queue.subscribe();
			queue.on('data', function(data) {
				received_message = data;
			});
			queue.push('hello world');
		});
		
		waitsFor(function() {
			return received_message;
		}, "Message should have been received", 500);

		runs(function() {
			expect(received_message).toEqual("hello world");
		});
	});
});
