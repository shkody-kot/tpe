const express = require("express");
var path = require('path');
var fs = require('fs');
var app = express();

//includes all (static) css, js, wasm files
app.use(express.static(__dirname + '/static', {
	setHeaders: function(response, path) 
	{
		response.set("Cross-Origin-Embedder-Policy", "require-corp");
		response.set("Cross-Origin-Opener-Policy", "same-origin");
		response.set("Cross-Origin-Resource-Policy", "same-site");
	}
}));

var port = process.env.PORT || 3000;

app.get('/', function (request, response)
{
	response.setHeader("Cross-Origin-Embedder-Policy", "require-corp");
	response.setHeader("Cross-Origin-Opener-Policy", "same-origin");
	response.sendFile(path.join(__dirname, '/index.html'));
});

app.listen(port, function () {
	console.log("Server is listening on port", port);
});
