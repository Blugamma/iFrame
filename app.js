
const express = require('express');
const bodyParser = require(`body-parser`);
const app = express();
const port = 2323;

var mqtt = require('mqtt');
var MQTT_TOPIC = "iFrame";
var MQTT_ADDR = "mqtt://broker.i-dat.org:80"; //Use the broker address here
var MQTT_PORT = 80; //And broker's port

app.use(bodyParser.urlencoded({ extended: true }));

app.listen(port, () => {
    console.log(`App is listening to ${port}`);
});

app.get("/", (req, res) => {
    res.sendFile(__dirname + '/www/index.html');
});

app.get('/new_name', function (req, res) {
    var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)
    var username = req.query.username;
    var password = req.query.password;
    var timer = req.query.timer;
    console.log(username);
    console.log(password);
    client.on('connect', function() { //connect the MQTT client
        client.subscribe(MQTT_TOPIC, { qos: 1 }); //Subscribe to the topic
        var message = timer;
        client.publish(MQTT_TOPIC, message); //Puish the message of the client
        console.log(message); //Print the results on the console (i.e. Terminal)
    });
    res.sendFile(__dirname + '/www/confirmed.html');
});
