var earth; 
var rain;
var thunderstorm;
var sun;
var snow;
var clouds;
var weatherData = {};
var weather = [];
let angle = 0;
function preload() {
    weatherData = loadJSON("https://api.mlab.com/api/1/databases/iframe/collections/weather?apiKey=QcMYUxzSPh1UFvwhGMNJHciyVqHemZmC");
}


function setup() {
    frameRate(30);
    angleMode(DEGREES)
    //console.log(weatherData[0].currentWeather);
    var canvas = createCanvas(800, 800);
    canvas.parent('canvas');
    earth = loadImage("images/earth.png");  // Load the image
    rain = loadImage("images/rain.png");
    thunderstorm = loadImage("images/thunderstorm.png");
    sun = loadImage("images/sun.png");
    snow = loadImage("images/snow.png");
    clouds = loadImage("images/clouds.png");
}

function drawWeather(){
    
    weatherDataLength = Object.keys(weatherData).length;
    
    for (var i = 0; i < weatherDataLength; i++) {
        rotate(angle);
        var currentWeather = weatherData[i].currentWeather;
        var temp = Math.trunc(parseInt(weatherData[i].temperature) - 273.15) + "C";
        if (currentWeather == "Rain" || currentWeather == "Drizzle"){
            image(rain, width/4, height/4, rain.width/4, rain.height/4);
            
            if (temp != "NaNC"){
                fill(255);
                text(temp, rain.width + 35, rain.height + 60);
            }

            
        }
        
        if (currentWeather == "Thunderstorm"){
            image(thunderstorm, width/6, height/6, thunderstorm.width/4, thunderstorm.height/4);
            if (temp != "NaNC"){
            fill(255);
            text(temp, thunderstorm.width + 35, thunderstorm.height + 60);
            }
        }
        
        if (currentWeather == "Clear"){
            image(sun, width/7.5, height/7.5, sun.width/4, sun.height/4);
            if (temp != "NaNC"){
            fill(255);
            text(temp, sun.width + 35, sun.height + 60);
            }
            
            
        }

        if (currentWeather == "Clouds"){
            image(clouds, width/5, height/5, clouds.width/4, clouds.height/4);
            if (temp != "NaNC"){
            fill(0);
           
            text(temp, clouds.width, clouds.height + 20);
            }
        }
       
        if (currentWeather == "Snow"){
            image(snow, width/3.5, height/3.5, clouds.width/4, clouds.height/4);
            if (temp != "NaNC"){
            fill(0);
           
            text(temp, clouds.width, clouds.height + 20);
            }
        }

           
       
        //console.log(currentWeather);
       
       
    
    
    }
    
    
}

function draw() {
    
    background(135, 206, 250);
    //fill(204, 101, 192, 127);
    image(earth, width/2.8, height/2.8, earth.width/2, earth.height/2);
    translate(width/2, height/2);
   

    
    drawWeather();
    angle = angle + 0.10;
}

