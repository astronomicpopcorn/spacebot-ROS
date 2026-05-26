#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Controllino.h>

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

unsigned long lastPacketTime = 0;

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  Udp.begin(8888);
  setupMotors();
}

void loop() {
  int packetSize = Udp.parsePacket();
  if(packetSize == sizeof(MotorCommand)) {
    Udp.read((char*)&cmd, sizeof(cmd));
    lastPacketTime = millis();
    setMotorSpeeds(cmd.left, cmd.right);
  }

  // watchdog timeout
  if(millis() - lastPacketTime > 200) {
    setMotorSpeeds(0, 0);
  }
}

void setupMotors() {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

void setMotorSpeeds(int16_t left, int16_t right) {
  left = constrain(left, -255, 255);
  right = constrain(right, -255, 255);

  analogWrite(5, abs(left));
  analogWrite(6, abs(right));
}