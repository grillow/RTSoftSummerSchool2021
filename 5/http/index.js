const express = require('express');

const app = express();

const PORT = process.env.PORT || 8080;

app.use(express.json());

app.post('/', (req, res) => {
    try {
        console.log(req.body.action);
        res.sendStatus(200);
    } catch (error) {
        console.log(`error: ${error}`);
    }
});

app.listen(PORT);
