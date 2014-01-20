#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

int level = 0;
int dataPin = 2;
int latchPin = 3;
int clockPin = 4;
int buttonPin = 5;
int firehoseLedPin = 6;

int firehose = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50; 
int firehoseLedState = LOW;
int buttonState;
int lastButtonState = LOW;  // the previous reading from the input pin

int last_output = 1;
int led_map[] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

byte macAddress[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 10, 0, 1, 6 };

EthernetClient ethClient;
PubSubClient client(server, 1883, mqttCallback, ethClient);
int checkFirehoseMode();
int levelToOutput(int level);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(firehoseLedPin, OUTPUT);
  digitalWrite(firehoseLedPin, firehoseLedState);

  Serial.begin(38400);
  Ethernet.begin(macAddress); // IP via DHCP
  client.connect("arduinoClient");
}

void loop() {
  firehose = checkFirehoseMode();
  
  level = analogRead(A0);
  int output = levelToOutput(level);
  
  char message[5];
  
  if (firehose == HIGH) {
    sprintf(message, "%d", level); 
    client.publish("readings", message);
  }
    
  if (output != last_output) {
    if (firehose == LOW) {
      sprintf(message, "%d", output); 
      client.publish("readings", message);
    }
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, output);
    digitalWrite(latchPin, HIGH);
  }
  last_output = output;

  delay(50);
}

int levelToOutput(int level) {
  int index = (int)level/128;
  return (int)led_map[index];
}

int checkFirehoseMode() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        firehoseLedState = !firehoseLedState;
      }
    }
  }
  
  digitalWrite(firehoseLedPin, firehoseLedState);
  lastButtonState = reading;

  return firehoseLedState;
}
