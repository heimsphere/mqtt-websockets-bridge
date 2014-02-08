# TODO

## Known Issues

### two clients connecting with the same id

	mosquitto_sub -i xxxx -t '/foo/bar'
	mosquitto_sub -i xxxx -t '/foo/bar'

Result is a subscription loop

```
1391853133: New connection from 127.0.0.1 on port 1883.
1391853133: Client xxxx already connected, closing old connection.
1391853133: New client connected from 127.0.0.1 as xxxx (c1, k60).
1391853133: Sending CONNACK to xxxx (0)
1391853133: Received SUBSCRIBE from xxxx
1391853133: 	/foo/bar (QoS 0)
1391853133: xxxx 0 /foo/bar
1391853133: Sending SUBACK to xxxx
1391853134: New connection from 127.0.0.1 on port 1883.
1391853134: Client xxxx already connected, closing old connection.
1391853134: New client connected from 127.0.0.1 as xxxx (c1, k60).
1391853134: Sending CONNACK to xxxx (0)
1391853134: Received SUBSCRIBE from xxxx
1391853134: 	/foo/bar (QoS 0)
1391853134: xxxx 0 /foo/bar
1391853134: Sending SUBACK to xxxx
^C1391853135: New connection from 127.0.0.1 on port 1883.
```

* Possible solution: Use Public-Key Authentication (every client using it's own client certificate)


