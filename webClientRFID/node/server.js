var express = require('express');
var app = express();
//var lookupAudio = new Function("rfid", "if(rfid==='6A003E4B243B') {return 'It\'s a C!';} else {return 'Not found :(';}");

function lookupAudio(rfid) {
	if(rfid === "6A003E4B243B") {
		return "C_SOUND.AFM";
	} 
	else if(rfid === "6A003E1E3C76") {
		return "A_SOUND.AFM";
	}
	else if(rfid === "6A003E503C38") {
		return "T_SOUND.AFM";
	}
	else {
		return "NOT FOUND!";
	}
}

app.get('/', function(req, res){
	res.set('Content-Type', 'text/plain');
	res.send('Welcome!');
});

app.get('/hello', function(req, res){
	var query;
	res.set('Content-Type', 'text/plain');
	query = req.query.rfid;
	//res.send('Hello, CC3000! I found ' + query + 'as a query string for rfid/n');
	res.send(/*'Sent query for rfid = ' + query + ' : ' + */lookupAudio(query));
});
app.get('/json', function(req,res){
	res.json({msg:'Hello CC3000 from JSON'});
});

/*
char tag11[13] = "6A003E4B243B"; //our tags C
char tag12[13] = "6A003E1E3C76"; //our tags A
char tag13[13] = "6A003E503C38"; //our tags T
*/

app.listen(8080);
console.log('Listening on port 8080');