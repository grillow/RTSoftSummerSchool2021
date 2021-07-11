const kafka = require('kafka');
const {InfluxDB, Point, HttpError} = require('@influxdata/influxdb-client');


const consumer = new kafka.Consumer({
    host: 'localhost',
    port: 9092,
    pollInterval: 2000,
    maxSize: 1048576
});

const influxDB = new InfluxDB({proxy: '', });
