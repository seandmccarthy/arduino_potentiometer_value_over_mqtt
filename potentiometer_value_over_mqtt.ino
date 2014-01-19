#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

int level = 0;
int data_pin = 2;
int latch_pin = 3;
int clock_pin = 4;
int diffMode = 0;

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 10, 0, 1, 6 };
byte ip[]     = { 10, 0, 1, 8 };

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

boolean debug = true;

int last_output = 1;

int led_map[] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

int levelToOutput(int level) {
  int index = (int)level/128;
  return (int)led_map[index];
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void setup() {
  pinMode(data_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  Serial.begin(38400);
  Ethernet.begin(mac, ip);
  client.connect("arduinoClient");
}

void log(String message) {
  if (debug) {
    Serial.println(message);
  }
}

void loop() {
  
  level = analogRead(A0);
  int output = levelToOutput(level);
  char message[4];
  
  if diffMode == 0) {
    sprintf(message, "%d", level); 
    client.publish("readings", message);
  end
    
  if (output != last_output) {
    //log(String("Level changed from ") + last_output + " to " + output);
    if (diffMode == 1)
      sprintf(message, "%d", output); 
      client.publish("readings", message);
    }
    digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, MSBFIRST, output);
    digitalWrite(latch_pin, HIGH);
  }
  last_output = output;

  delay(100);
}
