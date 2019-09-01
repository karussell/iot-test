var fs = require("fs");
// make it available in the LAN to your ESP
var host = "0.0.0.0";
var port = 8000;
var express = require("express");
var app = express();

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
    res.send("Hello!!");
});

app.post("/", function(req, res) {
    console.log(new Date().toISOString() + " " + req.rawBody)
    var stream = fs.createWriteStream("out.log", {flags:'a'});
    stream.write(req.rawBody + "\n")
    stream.end();
    res.send('ok');
});

app.listen(port, host);