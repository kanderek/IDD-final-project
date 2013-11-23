$( document ).ready(function(){
	
	var $cubes = $("#cubes");
	var $button = $("#start");
	var speaker = document.getElementById("speaker");
	
	var socket = io.connect('http://localhost');
	
	var word = "";
	var prev = false;
	
	socket.on('connected', function(){
		$button.show();
	});

	socket.on('serial', function (data) {
		var $charecter = $('<div>');
		
		// if(data.charecter == prev) return;
		prev = data.charecter;
		console.log(data);
		
		googleSpeak(data.charecter);
		
		$charecter.html(data.charecter);
		$cubes.append($charecter);
		
		word += data.charecter;
		
		if(word == 'CAT') {
			console.log(word)
			setTimeout(function(){
					googleSpeak("Cat. Good job!");
			}, 1600);
			
			word = "";
			$button.show();
		}
	});
	
	$button.on('click', function(e){
		word = "";
		prev = false;
		googleSpeak("Can you spell Cat?");
		e.preventDefault();
	});
	
	function speak(str) {
		// var speech = new SpeechSynthesisUtterance();
		// speech.volume = 1; // 0 to 1
		// speech.rate = 1; // 0.1 to 9
		// speech.pitch = 1; // 0 to 2, 1=normal
		// speech.lang = "en-US";
		// speech.text = str;
		// speechSynthesis.speak(speech);
		var u = new SpeechSynthesisUtterance('h');
		speechSynthesis.speak(u);
	}
	
	function googleSpeak(str) {
		//http://translate.google.com/translate_tts?tl=en&q=B
		console.log(encodeURI(str));
		// speaker.src = "http://translate.google.com/translate_tts?tl=en&q=B";
		var url = "http://translate.google.com/translate_tts?tl=en&q=";
		var speaker = new Audio(url + encodeURI(str));
		speaker.load();
		speaker.play();
		// speaker.play();
	}
	
	
});