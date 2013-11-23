/**
 * Module dependencies.
 */

var express = require('express'),
		// routes = require('./routes'),
		http = require('http');

var app = express();
var server = app.listen(3030);
var io = require('socket.io').listen(server);
var SerialPort = require("serialport").SerialPort;
var serialPort;
var sock;

var alphabet = {
	"0x4A 0x00 0x42 0xD9" : 'A'
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
	sock.emit('serial', { data: "0x4A 0x00 0x42 0xD9", charecter: "A" })
});

serialPort = new SerialPort("/dev/tty-usbserial1", {
	baudrate: 9600
});

serialPort.on("open", function () {
	console.log('open');
	serialPort.on('data', function(data) {
		var charecter;
		if(data in alpahbet) charecter = alphabet[data];

		console.log('data received: ', data, charecter);
		if (data == 'Ready') {
			if (sock) sock.emit('ready');
		} else {

			if (sock) sock.emit('serial', { data: data, charecter: charecter });
		}

	});  
	// serialPort.write("ls\n", function(err, results) {
	// 	console.log('err ' + err);
	// 	console.log('results ' + results);
	// }); 
});

console.log("Express server listening on port 3030");

