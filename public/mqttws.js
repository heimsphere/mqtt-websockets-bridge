/*
 * Javascript API for MQTT websockets bridge
 */
var MQTT = (function() {
	var MESSAGE_FORMAT = /([^\n]+)\n(.*)/,
		MESSAGE_TOPIC = 1,
		MESSAGE_BODY = 2;

	var REQUEST_TOPIC = /(RPC\/[^/]+)\/(.*)/,
		REQUEST_SERVICE = 1,
		REQUEST_PATH = 2;
		
	var RPC_PREFIX = 'RPC/';

	function Connection(url) {
		EventEmitter.call(this);
		this.url = url;
		this.connect();
	}

	// see http://stackoverflow.com/questions/4152931/javascript-inheritance-call-super-constructor-or-use-prototype-chain

	function extend(parent, child) {
		// clone Channel without executing the constructor 
		var clone = function() {};
		clone.prototype = parent.prototype;
		child.prototype = new clone();
		child.prototype.constructor = child;
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
				var topic = match[MESSAGE_TOPIC];
				var body = match[MESSAGE_BODY];
				var topic_match = topic.match(REQUEST_TOPIC);
				if (topic_match) {
					var service = topic_match[REQUEST_SERVICE];
					var service_path = topic_match[REQUEST_PATH];
					that.emit(service, service_path, body);
				} else {
					that.emit(topic, topic, body);
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
		this.connection.on(topic, function(path, data) {
			that.emit(path, data);
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

	function Service(connection, topic) {
		// execute the super constructor
		Channel.call(this, connection, RPC_PREFIX + topic);
	}
	Service.prototype = Object.create(Channel.prototype);
	Service.prototype.constructor = Service;
	// extend(Channel, Service);

	Service.prototype.subscribe = function() {
		this.connection.send("subscribe", this.topic + "/#");
		this.subscribed = true;
	}

	Service.prototype.request = function(data, callback) {
		this.connection.send("request", this.topic, data);
	};

	Connection.prototype.service = function(name) {
		return new Service(this, name);
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
