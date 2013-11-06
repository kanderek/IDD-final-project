var noble = require('noble');

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
		this.updateRssi();
	});
	
	peripheral.on('disconnect', function() {
		console.log('disconnect');
	});
	
	peripheral.on('rssiUpdate', function(rssi) {
		console.log('RSSI update ' + rssi);
		this.discoverServices();
	});
	
	// console.log('\tcan I interest you in any of the following advertised services:');
	// console.log('\t\t' + JSON.stringify(peripheral.advertisement.serviceUuids));
	// console.log();
	
	// peripheral.discoverServices(["195ae58a437a489bb0cdb7c9c394bae4"], function(error, services) {
	// 	console.log(services)
	// });
	
	// service.discoverCharacteristics([], callback(error, characteristics)]);
	
	peripheral.on('servicesDiscover', function(services) {
		var arduino = services[2];
		// console.log('on -> peripheral services discovered ' + services);
		var buffer = new Buffer("I'm a string!", "utf-8")
		arduino.discoverCharacteristics();
		
		arduino.on('characteristicsDiscover', function(characteristics) {
			var writer = characteristics[0],
					reader = characteristics[1];
					
			// console.log('on -> service characteristics discovered ' + characteristics);
			// console.log(read)
			// writer.write(buffer);
			// reader.read(function(error, data){
			// 	console.log(data.toString('hex'))
			// });
			
			reader.notify(true, function(error) {
				console.log('notification on');
			});
			
			reader.on('read', function(data, isNotification){
				console.log(data.toString('hex'));
			});
			
			reader.read();
		});
		
		
		
	});
	
	peripheral.connect();
	
});
