// broker
const aedes = require('aedes')();
const server = require('net').createServer(aedes.handle);
const port = 1883;

server.listen(port, () => {
    console.log('broker is ready');
});
