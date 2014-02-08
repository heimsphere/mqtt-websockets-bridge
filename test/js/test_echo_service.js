describe("message subscription", function() {
	it("should receive published message for topic", function() {

		var socket = new WebSocket('ws://127.0.0.1:8080/', ['mqtt-bridge-protocol']);
		var received_message;

		socket.onmessage = function(event) {
			console.log(event.data);
			received_message = event.data;
		}

		socket.onopen = function() {
			socket.send('SUBSCRIBE _RPC/out/xxx/xxx/echo/echo');
			socket.send('PUBLISH _RPC/in/xxx/xxx/echo/echo\n' + 'hello world');
		};
		
		waitsFor(function() {
			return received_message;
		}, "Message should have been received", 2000);

		runs(function() {
			expect(received_message).toEqual("PUBLISH _RPC/out/xxx/xxx/echo/echo\n" + "hello world");
		});
	});
});
