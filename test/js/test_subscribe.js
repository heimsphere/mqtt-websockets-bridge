describe("message subscription", function() {
	it("should receive published message for topic", function() {

		var socket = new WebSocket('ws://127.0.0.1:8080/', ['mqtt-bridge-protocol']);
		var received_message;

		socket.onmessage = function(event) {
			received_message = event.data;
		}

		socket.onopen = function() {
			socket.send('SUBSCRIBE /foo/bar');
			socket.send('PUBLISH /foo/bar\nhello world');
		};
		
		waitsFor(function() {
			return received_message;
		}, "Message should have been received", 500);

		runs(function() {
			expect(received_message).toEqual("/foo/bar\nhello world");
		});
	});
});
