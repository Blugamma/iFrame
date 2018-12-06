
const express = require('express');
const bodyParser = require(`body-parser`);
const app = express();
const port = 3000;
const request = require('request');
var path = require('path');
var mqtt = require('mqtt');
var MQTT_ADDR = "mqtt://test.mosquitto.org:1883"; //Use the broker address here
var MQTT_PORT = 80; //And broker's port
var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://admin:password123@ds249503.mlab.com:49503/iframe";
var session = require('express-session');
var sess;
var weatherLocationSet = false;
var tempSet = false;
var weatherSet = false;
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.static('www/public'));
app.use(session({secret: 'iFrameSecretv2'}));

app.listen(port, () => {
    console.log(`App is listening to ${port}`);
});


app.post('/signUp', function (req, res){
    //console.log(req.body);
    MongoClient.connect(url, function(err, db) {
        if (err) throw err;
        var dbo = db.db("iframe");
        var myobj = req.body;
        dbo.collection('users').findOne({ email: req.body.email}, function(err, user) {
            if (err) throw err;
            if (user == null){
                dbo.collection("users").insertOne(myobj, function(err, res) {
                    if (err) throw err;
                    console.log("1 user inserted");
                    db.close();
                });
                res.sendFile(__dirname + '/www/confirmed.html');
            }
            else if (user.email == req.body.email){
                console.log("User already exists");
                res.sendFile(__dirname + '/www/incorrectInfo.html');
            }
        });
      });
      
});


function convertTimestamp(timestamp) {
    var d = new Date(timestamp * 1000),	// Convert the passed timestamp to milliseconds
          yyyy = d.getFullYear(),
          mm = ('0' + (d.getMonth() + 1)).slice(-2),	// Months are zero based. Add leading 0.
          dd = ('0' + d.getDate()).slice(-2),			// Add leading 0.
          hh = d.getHours(),
          h = hh,
          min = ('0' + d.getMinutes()).slice(-2),		// Add leading 0.
          ampm = 'AM',
          time;
              
      if (hh > 12) {
          h = hh - 12;
          ampm = 'PM';
      } else if (hh === 12) {
          h = 12;
          ampm = 'PM';
      } else if (hh == 0) {
          h = 12;
      }
      
      // ie: 2013-02-18, 8:35 AM	
      time = yyyy + '-' + mm + '-' + dd + ', ' + h + ':' + min + ' ' + ampm;
          
      return time;
  }

  
    
 
app.get("/", (req, res) => {
    res.sendFile(__dirname + '/www/index.html');
});

app.post('/weatherCheck', function (req, res) {
    sess = req.session;
    var location = req.body.location;
    sess.location = location;
    console.log(sess.location);
    weatherLocationSet = true;
    weatherSet = true;
    res.sendFile(__dirname + '/www/weatherSending.html');
});

app.post('/tempCheck', function (req, res) {
    sess = req.session;
    var location = req.body.location;
    sess.location = location;
    console.log(sess.location);
    weatherLocationSet = true;
    tempSet = true;
    res.sendFile(__dirname + '/www/weatherSending.html');
});


 setInterval(function(){
    if (weatherLocationSet === true){
    request('http://api.openweathermap.org/data/2.5/weather?q='+ sess.location + '&appid=35d0cd20cdfd920305d90e2eb8dc5a93', { json: true }, (err, res, body) => {
        if (err) { return console.log(err); }
        var weather = body.weather[0].main;
        var weatherPlace = body.name;
        var dateTime = body.dt;
        var convertedDateTime = convertTimestamp(dateTime);
        var temp = body.main.temp.toString();
        MongoClient.connect(url, function(err, db) {
            if (err) throw err;
            var dbo = db.db("iframe");
            var weatherObj = {
                dateTime: convertedDateTime,
                currentWeather: weather,
                weatherLocation: weatherPlace,
                temperature: temp  
            };
            
            var col = dbo.collection("weather");
            col.findOne(weatherObj, function(err, result) {
                if (err) throw err;
                if (result == null){
                    col.createIndex({ dateTime: 1, weatherLocation: 1  }, { unique: true });
                    col.insertOne(weatherObj, function(err, result) {
                                if (err) throw err;
                                console.log("1 weather object inserted");
                                if (tempSet === true){
                                    var message = result.ops[0].temperature;
                                    var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)
                                    client.on('connect', function() { //connect the MQTT client
                                        client.subscribe('weatherFrame', { qos: 1 }); //Subscribe to the topic
                                        client.publish('weatherFrame', message); //Puish the message of the client
                                        console.log(message); //Print the results on the console (i.e. Terminal)
                                    });
                                }
                                else if (weatherSet === true){
                                    var message = result.ops[0].currentWeather;
                                    var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)
                                    client.on('connect', function() { //connect the MQTT client
                                        client.subscribe('weatherFrame', { qos: 1 }); //Subscribe to the topic
                                        client.publish('weatherFrame', message); //Puish the message of the client
                                        console.log(message); //Print the results on the console (i.e. Terminal)
                                    });
                                }
                                db.close();
                    });
                }
                else if (result.dateTime != convertedDateTime && result.weatherLocation != weatherPlace){
                    col.createIndex({ dateTime: 1, weatherLocation: 1  }, { unique: true });
                    col.insertOne(weatherObj, function(err, result) {
                                if (err) throw err;
                                console.log("1 weather object inserted");
                                if (tempSet === true){
                                    var message = result.ops[0].temperature;
                                    var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)
                                    client.on('connect', function() { //connect the MQTT client
                                        client.subscribe('weatherFrame', { qos: 1 }); //Subscribe to the topic
                                        client.publish('weatherFrame', message); //Puish the message of the client
                                        console.log(message); //Print the results on the console (i.e. Terminal)
                                    });
                                }
                                else if (weatherSet === true){
                                    var message = result.ops[0].currentWeather;
                                    var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)
                                    client.on('connect', function() { //connect the MQTT client
                                        client.subscribe('weatherFrame', { qos: 1 }); //Subscribe to the topic
                                        client.publish('weatherFrame', message); //Puish the message of the client
                                        console.log(message); //Print the results on the console (i.e. Terminal)
                                    });
                                }
                                db.close();  
                    });
                }
                else{
                    console.log("dubplicated");
                    //console.log(result);
                    if (tempSet === true){
                        var message = result.temperature;
                        var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)
                        client.on('connect', function() { //connect the MQTT client
                            client.subscribe('weatherFrame', { qos: 1 }); //Subscribe to the topic
                            client.publish('weatherFrame', message); //Puish the message of the client
                            console.log(message); //Print the results on the console (i.e. Terminal)
                        });
                    }
                    else if (weatherSet === true){
                        var message = result.currentWeather;
                        var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)
                        client.on('connect', function() { //connect the MQTT client
                            client.subscribe('weatherFrame', { qos: 1 }); //Subscribe to the topic
                            client.publish('weatherFrame', message); //Puish the message of the client
                            console.log(message); //Print the results on the console (i.e. Terminal)
                        });
                    }
                }
            });
        });
    });
}},10000);

app.post('/login', function (req, res) {
    sess = req.session;
    var email = req.body.email;
    var pass = req.body.password;
    
    MongoClient.connect(url, function(err, db) {
        if (err) throw err;
        var dbo = db.db("iframe");
        dbo.collection("users").findOne({ email: email}, function(err, result) {
            if (err) throw err;
            //console.log(result.email);
           // console.log(result.password);
            if (result.email === email && result.password === pass){
                console.log("success");
                sess.email = result.email;
                res.redirect('/account-dashboard');
            } 
            else {
                console.log("Credentials wrong");
            }
        db.close();
        });
      });
    
    });
    app.get("/account-dashboard", (req, res) => {
        sess = req.session;
        if (sess.email){
            res.sendFile(__dirname + '/www/account-dashboard.html');
        }
        else{
            res.redirect('/');
        }
      });


