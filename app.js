
const express = require('express');
const bodyParser = require(`body-parser`);
const app = express();
const port = process.env.PORT || 5000;
const request = require('request');
var path = require('path');
var mqtt = require('mqtt');
var MQTT_TOPIC = "iFrame";
var MQTT_ADDR = "mqtt://broker.i-dat.org:80"; //Use the broker address here
var MQTT_PORT = 80; //And broker's port
var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://admin:password123@ds249503.mlab.com:49503/iframe";
var session = require('express-session');
var sess;


app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.static('www/public'));
app.use(session({secret: 'iFrameSecret'}));

app.listen(port, () => {
    console.log(`App is listening to ${port}`);
});


app.post('/signUp', function (req, res){
    console.log(req.body1);
    MongoClient.connect(url, function(err, db) {
        if (err) throw err;
        var dbo = db.db("iframe");
        var myobj = req.body;
        dbo.collection("users").insertOne(myobj, function(err, res) {
          if (err) throw err;
          console.log("1 user inserted");
          db.close();
        });
      });
      res.sendFile(__dirname + '/www/confirmed.html');
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
    var location = req.body.location;
    request('http://api.openweathermap.org/data/2.5/weather?q='+ location +'&appid=35d0cd20cdfd920305d90e2eb8dc5a93', { json: true }, (err, res, body) => {
        if (err) { return console.log(err); }
        var weather = body.weather[0].main;
        var weatherPlace = body.name;
        var dateTime = body.dt;
        var convertedDateTime = convertTimestamp(dateTime);
        MongoClient.connect(url, function(err, db) {
            if (err) throw err;
            var dbo = db.db("iframe");
            var myobj = {
                dateTime: convertedDateTime,
                currentWeather: weather,
                weatherLocation: weatherPlace    
            };
            dbo.collection("weather").insertOne(myobj, function(err, res) {
            if (err) throw err;
            console.log("1 user inserted");
            db.close();
            });
        });
    });
});

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

      app.get("/account-dashboard", (req, res) => {
        sess = req.session;
        if (sess.email){
            res.sendFile(__dirname + '/www/account-dashboard.html');
        }
        else{
            res.redirect('/');
        }
      });
    //var client  = mqtt.connect(MQTT_ADDR); //Create a new connection (use the MQTT adress)
    
    
    //var timer = req.query.timer;
    //console.log(username);
    //console.log(password);
    //client.on('connect', function() { //connect the MQTT client
       // client.subscribe(MQTT_TOPIC, { qos: 1 }); //Subscribe to the topic
        //var message = timer;
       // client.publish(MQTT_TOPIC, message); //Puish the message of the client
       // console.log(message); //Print the results on the console (i.e. Terminal)
   // });
   // res.sendFile(__dirname + '/www/confirmed.html');
});
