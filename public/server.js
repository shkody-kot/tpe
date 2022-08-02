const express = require("express");
var path = require('path');
var fs = require('fs');
var app = express();

//includes all our funky little css and js files
app.use(express.static(__dirname + '/static'));

var port = process.env.PORT || 3000;

app.get('/', function (request, response)
{
	response.sendFile(path.join(__dirname, '/index.html'));
});

app.listen(port, function () {
	console.log("Server is listening on port", port);
});
