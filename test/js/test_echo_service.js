describe("message subscription", function() {
	it("should receive response for request", function() {

		var socket = new WebSocket('ws://127.0.0.1:8080/', ['mqtt-bridge-protocol']);
		var received_message;

		socket.onmessage = function(event) {
			received_message = event.data;
		}

		socket.onopen = function() {			
			socket.send('PUBLISH /echo/echo\n\n' + 'hello world');
		};
		
		waitsFor(function() {
			return received_message;
		}, "Message should have been received", 500);

		runs(function() {
			expect(received_message).toEqual("hello world");
		});
	});
});
