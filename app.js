
const express = require('express');
const bodyParser = require(`body-parser`);
const app = express();
const port = 2323;

var mqtt = require('mqtt');
var MQTT_TOPIC = "iFrame";
var MQTT_ADDR = "mqtt://broker.i-dat.org:80"; //Use the broker address here
var MQTT_PORT = 80; //And broker's port

var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)


//MQTT
client.on('connect', function() { //connect the MQTT client
    client.subscribe(MQTT_TOPIC, { qos: 1 }); //Subscribe to the topic

   
      

      //Create a new variable that will store information as text (String)
      //Concatenate the name with the random number (as string)
      var message = "Name";
      client.publish(MQTT_TOPIC, message); //Publish the message of the client
      console.log(message); //Print the results on the console (i.e. Terminal)
});

app.use(bodyParser.urlencoded({ extended: true }));

app.listen(port, () => {
    console.log(`App is listening to ${port}`);
});

app.get("/", (req, res) => {
    res.sendFile(__dirname + '/www/index.html');
});
