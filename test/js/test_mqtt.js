describe("message subscription", function() {	
	it("should receive mqtt message", function() {
	
		var received_message;
	
		var connection = MQTT.connect('ws://127.0.0.1:8080/', function() {
			var queue = connection.topic('/foo/bar');
			queue.subscribe();
			queue.on('/foo/bar', function(data) {
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
	
	it("should receive mqtt service message", function() {
		var received_message;
		var subtopic;
	
		var connection = MQTT.connect('ws://127.0.0.1:8080/', function() {
			var service = connection.service('service1');
			service.subscribe();
			service.on('xx/yy', function(data) {
				received_message = data;
			});
			connection.send('publish', 'RPC/service1/xx/yy', 'hello world');
			// queue.push('hello world');
		});
		
		waitsFor(function() {
			return received_message;
		}, "Message should have been received", 500);
			
		runs(function() {
			expect(received_message).toEqual("hello world");
		});
	});
});
