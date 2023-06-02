#include <Servo.h>
#include <Arduino_APDS9960.h>
#include <String.h>
#include <Arduino_OV767X.h>
#include <stdint.h>
#include <stdlib.h>

// setup UART and Servo
UART mySerial(A6,A7,NC,NC); 
Servo myServo;
// constants
const int buttonPin = 13;

// variables
int buttonState = 0;
String push_button_status = "Not Pressed";
String led_status = "Red";
String conveyor_status = "Not Moving";
String proximity_status = "Not Detected";
String object_detection_status = "Not Detected";
String detected_object_name = "None";
float highest_detected_confidence = 0;
String servo_status = "Not Sorted";
String send_to_cloud_status = "Not Sent";
int loop_count = 0;



// Interrupt service routine for button
void buttonInterrupt() {
  buttonState = digitalRead(buttonPin);
}


// all the setups
void setup() {
  // Serial and myserial Setup
  mySerial.begin(9600);
  Serial.begin(9600);
  randomSeed(analogRead(0)); // Seed the random number generator

  // pin setup
  pinMode(buttonPin, INPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDR, OUTPUT);


  // enable the internal pull-up resistor for the button pin
  digitalWrite(buttonPin, HIGH);

  // initialize status for LED
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDR, LOW);

  // Attach the interrupt to the button pin
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, CHANGE);
  // APDS
  if (!APDS.begin())
  {
    Serial.println("Error initializing APDS9960 sensor!");
    while (1);
  }
  
  // attach Servo
  myServo.attach(11);
}


// main loop
void loop() {
  // reset status
  proximity_status = "Not Detected";
  object_detection_status = "Not Detected";
  detected_object_name = "None";
  highest_detected_confidence = 0;
  servo_status = "Not Sorted";
  // if button was pressed
  if (buttonState) 
  {
    push_button_status = "Pressed";
    // Green LED should turn on, Red LED should turn off 
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDR, HIGH);
    led_status = "Green";
    // start moving the conveyor
    conveyor_status = "Moving";
    // start proximity detection
    if (APDS.proximityAvailable()) 
    {
      int prox = APDS.readProximity();
      if (prox < 10) 
      {
        // led becomes red
        digitalWrite(LEDG, HIGH);
        digitalWrite(LEDR, LOW);
        // led status
        led_status = "Red";
        // stop conveyor
        conveyor_status = "Not Moving";
        // Proximity status
        proximity_status = "Detected";
        // start object detection
        float random_val = random(10000) / 100.0;
        highest_detected_confidence = random_val;
        // object detection status
        object_detection_status = "Detected";
      }
    }

    // // check model confidence 
    if (object_detection_status == "Detected" && highest_detected_confidence > 50)
    {
      // turn servo motor
      myServo.write(180);
      delay(5000);
      
      // Move servo back to 0 degrees
      myServo.write(0);
      servo_status = "Sorted";
      // Transmit detection score to SBC by UART.
      mySerial.println(highest_detected_confidence);
      loop_count++;
      Serial.println(loop_count);
    }
  }
}
