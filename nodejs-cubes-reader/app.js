/**
 * Module dependencies.
 */

var express = require('express'),
		// routes = require('./routes'),
		http = require('http');

var app = express();
var server = app.listen(3030);
var io = require('socket.io').listen(server, { log: false });

var noble = require('noble');
var sock;
var connected = false;

var alphabet = {
	"4a0042d9" : 'A',
	"0aed3fd9" : 'C',
	"dab041d9" : 'T'
}

app.configure(function(){
	app.set('views', __dirname + '/views');
	app.set('view engine', 'jade');
	app.use(express.favicon());
	app.use(express.logger('dev'));
	app.use(express.static(__dirname + '/public'));
	app.use(express.bodyParser());
	app.use(express.methodOverride());
	app.use(app.router);
});

app.configure('development', function(){
	app.use(express.errorHandler());
});

app.get('/', function(){
	
});

io.sockets.on('connection', function (socket) {
	sock = socket; 
	// socket.emit('news', { hello: 'world' });
	// socket.on('my other event', function (data) {
	// 	console.log(data);
	// });
	if(connected) sock.emit('connected');
});


noble.on('stateChange', function(state) {
	if (state === 'poweredOn') {
		console.log("Starting scan...")
		noble.startScanning(["195ae58a437a489bb0cdb7c9c394bae4"]);
	} else {
		noble.stopScanning();
	}
});

noble.on('discover', function(peripheral) {
	console.log('peripheral discovered:', peripheral.advertisement.localName);
	noble.stopScanning();

	peripheral.on('connect', function() {
		console.log('connected');
		if (sock) sock.emit('connected');
		connected = true;
		this.updateRssi();
	});

	peripheral.on('disconnect', function() {
		console.log('disconnect');
		connected = false;
		if (sock) sock.emit('disconnect');
	});

	peripheral.on('rssiUpdate', function(rssi) {
		console.log('RSSI update ' + rssi);
		this.discoverServices();
	});

	peripheral.on('servicesDiscover', function(services) {
		var arduino = services[2];
		// console.log('on -> peripheral services discovered ' + services);
		var buffer = new Buffer("I'm a string!", "utf-8")
		arduino.discoverCharacteristics();

		arduino.on('characteristicsDiscover', function(characteristics) {
			var writer = characteristics[0],
					reader = characteristics[1];

			reader.notify(true, function(error) {
				console.log('notification on');
			});

			reader.on('read', function(data, isNotification){
				var charecter;
				var hex = data.toString('hex');
				if(hex in alphabet) {
					charecter = alphabet[hex];
					if (sock) sock.emit('serial', { data: hex, charecter: charecter });
					console.log("registed", hex, charecter);
				} else {
					console.log("tagged", hex);
				}
				
				
				
			});

			// reader.read();
		});



	});

	peripheral.connect();

});

console.log("Express server listening on port 3030");

