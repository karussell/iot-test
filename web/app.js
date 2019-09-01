var fs = require("fs");
// make it available in the LAN to your ESP
var host = "0.0.0.0";
var port = 8000;
var express = require("express");
var app = express();

var lastEntry = "not initialized";

app.use('/public', express.static(__dirname + '/public'));

function rawBody(req, res, next) {
  req.setEncoding('utf8');
  req.rawBody = '';
  req.on('data', function(chunk) {
    req.rawBody += chunk;
  });
  req.on('end', function(){
    next();
  });
}
app.use(rawBody);

app.get("/", function(req, res) {
    res.send(lastEntry);
});

app.post("/", function(req, res) {
    var s = new Date().toISOString() + "," + req.rawBody
    lastEntry = s
    console.log(s)
    var stream = fs.createWriteStream("out.log", {flags:'a'})
    stream.write(s + "\n")
    stream.end()
    res.send('ok')
});

app.listen(port, host);