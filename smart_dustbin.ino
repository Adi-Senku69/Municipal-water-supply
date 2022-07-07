/*Program to find the height of the garbage
 * and trigger a respons from the correspondent and open
 * the chute accordingly.
 */

#include <HttpClient.h>
#include <b64.h>

#include "ThingSpeak.h"
#include <ESP8266WiFi.h>


const int echoPin1 = 13;
const int trigPin1 = 15;
const int echoPin2 = 5;
const int trigPin2 = 2;

unsigned int duration1, duration2;
int distance1 = 0, distance2 = 0, distance = 0;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;

unsigned long ch_no = 1786665;//Replace with Thingspeak Channel number
const char * write_api = "EUNECMCHJLDDIJDC";//Replace with Thingspeak write API
const char kHostname[] = "maker.ifttt.com";
const char kPath[] = "/trigger/dustbin/json/with/key/6AyIkB075qT0xhoxZBcP-";

char ssid[] = "404UNAVAILABLE";
char pass[] = "";

WiFiClient  client;

void sound_write(void){//writing the data to think speak
      ThingSpeak.setField(1, distance);
      ThingSpeak.writeFields(ch_no, write_api);
      startMillis = currentMillis;
  
}

void sound_distance(void){//calculation of height and triggering the IFTTT protocol
  
  distance1 = duration1 * 0.034 / 2;
  distance2 = duration2 * 0.034 / 2;
  distance = (distance1 + distance2)/2;
  if(distance >= 50){
    HttpClient http(client);
    http.get(kHostname, kPath);
    Serial.println(distance);
    delay(60);
    http.stop();
  }
}

void sound_read(void){//reading the time taken for the sound to return to the sensor
  Serial.println("in sound_read\n");
  duration1 = pulseIn(echoPin1, HIGH);
  duration2 = pulseIn(echoPin2, HIGH);
  sound_distance();
  
}

void sound(void){//triggering an ultra sonic pulse
  digitalWrite(trigPin1, LOW);
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  digitalWrite(trigPin1, LOW);
  Serial.println("in sound\n");
  sound_read();
}

void setup()
{
  pinMode(echoPin1, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin2, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  //Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
  startMillis = millis();  //initial start time
}
void loop()
{
    Serial.println("in loop");
    sound();
    Serial.println(distance1);
    Serial.println(distance2);
    currentMillis = millis();
    if(currentMillis - startMillis >= period)
      sound_write();
}
