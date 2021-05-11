#include "NewPing.h"      
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoOTA.h>
#define BLYNK_PRINT Serial

int trigPin = 16;      // trigger pin
int echoPin = 5;      // echo pin

int swt = 1;


NewPing sonar(trigPin, echoPin);
WidgetLED led3(V6);
BlynkTimer timer;
WidgetTerminal terminal(V0);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "QEBSDNb7mt1J-Da5vZRClJkACoVFexTb";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Shushee";
char pass[] = "2444666668888888";

int count = 0; //to indicate that notification is ready to send

int flag = 0;

BLYNK_WRITE(V0)
{
  int pincode = 5646;
  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (pincode == param.asInt()) {
    terminal.clear();
    terminal.println("Pin Accepted") ;
    flag = 1;
    
  } else {

    // Send it back
    terminal.println("Pin Denied");
    //terminal.write(param.getBuffer(), param.getLength());
    //terminal.println();
    flag = 0;
  }

  // Ensure everything is sent
  terminal.flush();
}






BLYNK_WRITE(V4){ //switch button on blynk app
  int security_sensor = param.asInt(); //reading if it is 1 or 0 to turn on and off the switch 0 as off and 1 is on

  if(security_sensor == 1){
    Serial.print(security_sensor);
    if(flag == 1){
      swt = 0;
    }
    
  }
  else{
    Serial.print(security_sensor);
    swt = 1;
    flag = 0;
  }
}




void reconnectBlynk() { //connection checker
  if (!Blynk.connected()) {

    Serial.println("Lost connection");
    if(Blynk.connect()) {
      Serial.println("Reconnected");
    }
    else {
      Serial.println("Not reconnected");
    }
  }
}


void setup(){
  
  Serial.begin(9600);
  
  Blynk.begin(auth, ssid, pass);

  timer.setInterval(30*1000, reconnectBlynk); //run every 30s
  
}



void loop(){

  
  //** to indicate the button if is already on or off **//
  if(swt == 1){
    Blynk.virtualWrite(V4, 0); //to show that button is off if the switch is on
  }
  else{
    Blynk.virtualWrite(V4, 1); //to show that button is on if the switch is off
  }

  

  
  
  //*** run the program if blynk is connected to network ***//
  timer.run(); 
  if(Blynk.connected()) { Blynk.run(); }
  ArduinoOTA.handle();     
 
  
  float distance = sonar.ping_median(5); // reading the ultrasonic sensor
  
  Blynk.virtualWrite(V3, count); //insert counting in gauge of blynk app to know the counter if it is needed to notify
  
  if(swt ==1){ //when the security is on
      led3.on();
      if(distance>1000 || distance<2){
         
        Serial.println("Out of range");
        
        count+=1; //when it hits the certain count in gauge it will notify the phone
      }
      
      else
      {
        Serial.print("Distance: ");
        Serial.print(distance, 1); Serial.println(" cm"); //just printing the distance if the door is closed
      }
      
      delay(50);
    
  }
  else if (swt == 0) //when the security is off
  {
    led3.off();
    Serial.println("security off");  
    count = 0;
  }
  
}
