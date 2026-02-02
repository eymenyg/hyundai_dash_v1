#include <HyundaiDashV1.h>

#include "Sonata_YF.h"

HyundaiDashV1* activeCluster = &cluster;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  activeCluster->beginCAN();
}

void loop() {
  // put your main code here, to run repeatedly:
  activeCluster->run();
}
