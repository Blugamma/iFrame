//Use the library MQTT (this exists in the node_modules folder)
var mqtt = require('mqtt')
var MQTT_TOPIC = "iFrame"; //MQTT topis is test
var MQTT_ADDR = "mqtt://broker.i-dat.org:80"; //Use the broker address here
var MQTT_PORT = 80; //And broker's port

var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)
var numToGo = 0; //Initialise a number variable to 0

//MQTT
client.on('connect', function() { //connect the MQTT client
    client.subscribe(MQTT_TOPIC, { qos: 1 }); //Subscribe to the topic

   
      numToGo = getRandom(100); //Create a random number from 0 to 100

      //Create a new variable that will store information as text (String)
      //Concatenate the name with the random number (as string)
      var message = "Name: "+numToGo.toString();
      client.publish(MQTT_TOPIC, message); //Publish the message of the client
      console.log(message); //Print the results on the console (i.e. Terminal)
});

//Function that helps us to create a random number
function getRandom(max) {
  return Math.floor(Math.random() * Math.floor(max));
}

/*
client.on('error', function(){
    console.log("ERROR")
    client.end()
})
client.on('offline', function() {
    console.log("offline");
});

client.on('reconnect', function() {
    console.log("reconnect");
});
*/
