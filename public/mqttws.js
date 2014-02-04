/*
 * Javascript API for MQTT websockets bridge
 */
var MQTT = (function() {
	var MESSAGE_FORMAT = /(\w+) ([^\n]+)\n(.*)/,
		MESSAGE_COMMAND = 1,
		MESSAGE_TOPIC = 2,
		MESSAGE_BODY = 3;

	function Connection(url) {
		EventEmitter.call(this);
		this.url = url;
		this.connect();
	}
	Connection.prototype = new EventEmitter();

	Connection.prototype.connect = function() {
		var that = this;
		this.socket = new WebSocket(this.url, ['mqtt-bridge-protocol']);
		this.socket.onopen = function() {
			that.emit('connect');
		};

		this.socket.onmessage = function(event) {
			var match = event.data.match(MESSAGE_FORMAT);

			if (match) {
				switch (match[MESSAGE_COMMAND]) {
					case 'PUBLISH':
						that.emit(match[MESSAGE_TOPIC], match[MESSAGE_BODY]);
						break;
					default:
						console.log("Got unkown command: " + event.data);
						break;
				}
			} else {
				console.error("Got unknown message: " + event.data);
			}
		};

		// reconnect on connection loss
		this.socket.onclose = function(event) {
			that.emit('close', event);
			setTimeout(function() {
				that.connect();
			}, 100)
		};
	}

	Connection.prototype.topic = function(topic) {
		return new Channel(this, topic);
	};

	Connection.prototype.send = function(name, topic, body) {
		var header = name.toUpperCase() + " " + topic;

		if (body) {
			this.socket.send(header + "\n" + body);
		} else {
			this.socket.send(header);
		}
	}

	function Channel(connection, topic) {
		EventEmitter.call(this);
		var that = this;
		this.connection = connection;
		this.topic = topic;
		this.subscribed = false;
		this.connection.on(this.topic, function(data) {
			that.emit('data', data);
		});
		this.connection.on('connect', function() {
			// resubscribe if connection was lost
			if (that.subscribed) {
				that.subscribe();
			}
		});
	}
	Channel.prototype = new EventEmitter();

	Channel.prototype.push = function(data) {
		if (data && (data + "").length > 0) {
			this.connection.send("publish", this.topic, data);
		} else {
			throw new Error("Can't push empty message to queue " + this.topic);
		}

	};

	Channel.prototype.subscribe = function() {
		this.connection.send("subscribe", this.topic);
		this.subscribed = true;
	};

	Channel.prototype.unsubscribe = function() {
		this.connection.send("unsubscribe", this.topic);
		this.subscribed = false;
	};

	function connect(url, callback) {
		var connection = new Connection(url);
		connection.once('connect', callback);
		return connection;
	}

	return {
		'connect': connect
	};
})();
