#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Controllino.h>

#define left_motor A0
#define right_motor A1



byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 188, 17);

EthernetUDP Udp;

struct MotorCommand {
  int16_t left;
  int16_t right;
  uint32_t seq;
};

MotorCommand cmd;

uint16_t timeout = 2000; //2 seconds after the last packet, motors shut off
unsigned long lastPacketTime = 0;

int16_t last_right = 0;
int16_t last_left = 0;

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  Udp.begin(8888);
  setupMotors();
}

void loop() {
  int packetSize = Udp.parsePacket();
  if(packetSize > 0){
    Serial.print("Recieved packet of size ");
    Serial.println(packetSize);
  }
  if(packetSize == sizeof(MotorCommand)) {
    Udp.read((char*)&cmd, sizeof(cmd));
    Serial.print("Packet contents: left: ");
    Serial.print(cmd.left);
    Serial.print(", right: ");
    Serial.println(cmd.right);
    lastPacketTime = millis();
    setMotorSpeeds(cmd.left, cmd.right);
  }
  else if(packetSize > 0){
    Serial.println("Packet had invalid size.");
  }

  // watchdog timeout
  if(millis() - lastPacketTime > timeout) {
    setMotorSpeeds(0, 0);
  }
}

void setupMotors() {
  pinMode(left_motor, OUTPUT);
  pinMode(right_motor, OUTPUT);
  //debug things
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
}

void setMotorSpeeds(int16_t left, int16_t right) {
  if(left == last_left && right == last_right){
    return;
  }
  left = constrain(left, -255, 255);
  right = constrain(right, -255, 255);
  Serial.print("Setting Motor Speeds to ");
  Serial.print(left);
  Serial.print(" and ");
  Serial.println(right);
  if(left < 0){
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
  }
  else if(left > 0){
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
  }
  else{
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
  }
  if(right < 0){
    digitalWrite(8, HIGH);
    digitalWrite(9, LOW);
  }
  else if(right > 0){
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
  }
  else{
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
  }
  analogWrite(left_motor, abs(left));
  analogWrite(right_motor, abs(right));
  last_left = left;
  last_right = right;
}