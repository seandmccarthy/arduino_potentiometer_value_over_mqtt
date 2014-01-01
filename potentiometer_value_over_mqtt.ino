#include <PubSubClient.h>



int level = 0;
int data_pin = 2;
int latch_pin = 3;
int clock_pin = 4;

boolean debug = true;

int last_output = 1;

int led_map[] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

int levelToOutput(int level) {
  int index = (int)level/128;
  return (int)led_map[index];
}

void setup() {
  pinMode(data_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  Serial.begin(38400);
}

void log(String message) {
  if (debug) {
    Serial.println(message);
  }
}

void loop() {
  level = analogRead(A0);
  int output = levelToOutput(level);

  if (output != last_output) {
    log(String("Level changed from ") + last_output + " to " + output);

    digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, MSBFIRST, output);
    digitalWrite(latch_pin, HIGH);
    last_output = output;
  }
  delay(200);
}
