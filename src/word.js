Pebble.addEventListener('ready', 
	function(e){
		console.log("PebbleKit JS ready!");
		
		getWord();
	}
);

Pebble.addEventListener('appmessage', 
	function(e){
		console.log("AppMessage received!");
		getWord();
	}											
);

var xhrRequest = function(url, type, callback){
	var xhr = new XMLHttpRequest();
	xhr.onload = function(){
		callback(this.responseText);	
	};
	xhr.open(type, url);
	xhr.send();
};

function getWord(){
	var url = "http://api.wordnik.com/v4/words.json/wordOfTheDay?api_key=cfe89c88f8b00fae210080171580e7b0571b39847b56f7d39";
	
	xhrRequest(url, 'GET', 
		function(responseText){
			var json = JSON.parse(responseText);
			
			var word = json.word.toUpperCase();
			console.log("Word of the Day: " + word);
							 
			var definition = json.definitions[0].text.toLowerCase();
			console.log("Definition " + definition);	
			
			var dictionary = {
				"KEY_WORD": word,
				"KEY_DEFINITION": definition
			};
			Pebble.sendAppMessage(dictionary, 
				function(e){
					console.log("Info send to Pebble successfully!");
				},
				function(e){
					console.log("Error sending Info to Pebble!");
				}										
			);
		}
	);
}