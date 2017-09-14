
var http = require('http');
const server = http.createServer((request, response) => {
  response.end('hello world');
});

server.listen(8082);

/*
var app = require('express')();

app.get('*', (req, res)=> {
  res.end('hello world');
});

app.listen(8082);
*/
