// IPC via TCP
const net = require('net');
const sockets = [];
const server = net.createServer((socket) => {
    socket.name = socket.remoteAddress + ":" + socket.remotePort;
    sockets.push(socket);
    console.log(`connected: ${socket.remoteAddress}:${socket.remotePort}`);
    socket.on('end', () => {
        console.log(`disconnected: ${socket.name}`);
        sockets.splice(sockets.indexOf(socket), 1);
    });
});
server.listen(8000);


// REST API
const express = require('express');

const app = express();

const PORT = process.env.PORT || 8080;

app.use(express.json());

app.post('/', (req, res) => {
    try {
        console.log(req.body);
        const action = req.body.action;
        let start = 0;
        if (action === 'start') {
            start = 1;
        } else if (action === 'stop') {
            start = 0;
        }
        sockets.forEach( (socket) => socket.write(Buffer.from([start])) );
        
        res.sendStatus(200);
    } catch (error) {
        console.log(`error: ${error}`);
    }
});

app.listen(PORT);
