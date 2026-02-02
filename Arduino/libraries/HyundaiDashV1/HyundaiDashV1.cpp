#include "HyundaiDashV1.h"

HyundaiDashV1::HyundaiDashV1(
		HighSideRole HS1Role,
		HighSideRole HS2Role,
		LowSideRole LS1Role,
		LowSideRole LS2Role,
        
        uint16_t C_CANLightID,
		
        uint16_t tailLightID,
        uint8_t tailLightIndex0,
        uint8_t tailLightIndex0Value,
        uint8_t tailLightIndex1,
        uint8_t tailLightIndex1Value,
        
        uint16_t highBeamID,
        uint8_t highBeamIndex,
        uint8_t highBeamValue,
        
        uint16_t fogLightID,
        uint8_t fogLightIndex,
        uint8_t fogLightValue,
        
        uint16_t fuseSwID,
        uint8_t fuseSwIndex,
        uint8_t fuseSwValue,

        uint16_t extraDataID,
        uint8_t extraDataIndex,
        uint8_t extraDataValue,
        
        uint16_t turnSignalID,
        uint8_t turnSignalIndex,
        uint8_t turnSignalLeftValue,
        uint8_t turnSignalRightValue,
		
		bool hasBodyCAN,
		bool enableTurnSignalSound,
		
		uint16_t maxSpeed
    )
		: C_CAN(C_CANPin),
		  B_CAN(B_CANPin),
          
          HS1Role(HS1Role),
          HS2Role(HS2Role),
          LS1Role(LS1Role),
          LS2Role(LS2Role),
          
          C_CANLightID(C_CANLightID),
		  
		  tailLightID(tailLightID),
          tailLightIndex0(tailLightIndex0),
          tailLightIndex0Value(tailLightIndex0Value),
          tailLightIndex1(tailLightIndex1),
          tailLightIndex1Value(tailLightIndex1Value),
        
          highBeamID(highBeamID),
          highBeamIndex(highBeamIndex),
          highBeamValue(highBeamValue),
        
          fogLightID(fogLightID),
          fogLightIndex(fogLightIndex),
          fogLightValue(fogLightValue),
        
          fuseSwID(fuseSwID),
          fuseSwIndex(fuseSwIndex),
          fuseSwValue(fuseSwValue),

          extraDataID(extraDataID),
          extraDataIndex(extraDataIndex),
          extraDataValue(extraDataValue),
          
          turnSignalID(turnSignalID),
          turnSignalIndex(turnSignalIndex),
          turnSignalLeftValue(turnSignalLeftValue),
          turnSignalRightValue(turnSignalRightValue),
		  
		  hasBodyCAN(hasBodyCAN),
		  enableTurnSignalSound(enableTurnSignalSound),
		  
		  maxSpeed(maxSpeed)
	{		
		// Setup pin modes
		pinMode(dataPin, OUTPUT);
		pinMode(clockPin, OUTPUT);
		pinMode(latchPin, OUTPUT);
		pinMode(masterResetPin, OUTPUT);
		pinMode(buzzerPin, OUTPUT);
		pinMode(ignitionPin, OUTPUT);
		pinMode(HS1Pin, OUTPUT);
		pinMode(HS2Pin, OUTPUT);
		pinMode(LS1Pin, OUTPUT);
		pinMode(LS2Pin, OUTPUT);
		
		//Reset the shift register
		digitalWrite(masterResetPin, LOW);
        digitalWrite(masterResetPin, HIGH);
		digitalWrite(latchPin, HIGH);
		digitalWrite(latchPin, LOW);
	}

void HyundaiDashV1::run() {
    receiveSerialData();
    updateFuel();
    updateOutputs();
    updateSpeedAndRPM();
    gearSelector();
    updateCoolant();
    updateCAN();
}

void HyundaiDashV1::beginCAN() {
    C_CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);
    C_CAN.setMode(MCP_NORMAL);
    if(hasBodyCAN) {
        B_CAN.begin(MCP_ANY, CAN_100KBPS, MCP_8MHZ);
        B_CAN.setMode(MCP_NORMAL);
    }
}    

const uint8_t HyundaiDashV1::ectValues[12] = {
    0x00,0x8F,0x94,0x99,0x9E,0xD4,
    0xD6,0xD8,0xDB,0xDE,0xE1,0xE6
};

uint8_t HyundaiDashV1::getEctValueAtIndex(uint8_t index) const {
    return ectValues[index];
}

void HyundaiDashV1::receiveSerialData() {
    static uint8_t index = 0;

    while (Serial.available()) {
        uint8_t b = Serial.read();

        if (index == 0 && b != 0xAA) continue;

        ((uint8_t*)&rxPacket)[index++] = b;

        if (index == sizeof(Telemetry)) {
            index = 0;

            if (rxPacket.start == 0xAA && rxPacket.end == 0x55) {
                applyTelemetry(rxPacket);
                newData = true;
            }
        }
    }
}

void HyundaiDashV1::updateFuel() {
	uint16_t fuel = (fuelCapacity == 0)
			? 1000
			: uint16_t((float)fuelVolume / fuelCapacity * 1000);

	uint8_t shiftVal, shiftTimes;
	if(fuel > 917) {shiftVal = 3; shiftTimes = 6;} //12 bars - full
	else if(fuel > 834 && fuel <= 917) {shiftVal = 1; shiftTimes = 7;} //11 bars
	else if(fuel > 750 && fuel <= 834) {shiftVal = 1; shiftTimes = 6;} //10 bars
	else if(fuel > 667 && fuel <= 750) {shiftVal = 34; shiftTimes = 0;} //9 bars
	else if(fuel > 584 && fuel <= 667) {shiftVal = 1; shiftTimes = 5;} //8 bars
	else if(fuel > 500 && fuel <= 584) {shiftVal = 1; shiftTimes = 4;} //7 bars
	else if(fuel > 417 && fuel <= 500) {shiftVal = 3; shiftTimes = 1;} //6 bars - half
	else if(fuel > 334 && fuel <= 417) {shiftVal = 1; shiftTimes = 3;} //5 bars
	else if(fuel > 250 && fuel <= 334) {shiftVal = 3; shiftTimes = 0;} //4 bars
	else if(fuel > 167 && fuel <= 250) {shiftVal = 1; shiftTimes = 2;} //3 bars
	else if(fuel > 84 && fuel <= 167) {shiftVal = 1; shiftTimes = 1;} //1 bar
	else if(fuel <= 84) {shiftVal = 1; shiftTimes = 0;} //1 bar flash
	else {shiftVal = 3; shiftTimes = 6;}

	uint8_t val = shiftVal << shiftTimes;
	shiftOut(dataPin, clockPin, LSBFIRST, val);
	digitalWrite(latchPin, HIGH);
	digitalWrite(latchPin, LOW);
}

void HyundaiDashV1::turnSignalSound(bool signalOn) {
	if(!enableTurnSignalSound) return;
	if(signalOn == previousSignal) return;
	if(signalOn && !previousSignal) tone(buzzerPin, 1000, 5);
	else if(!signalOn && previousSignal) tone(buzzerPin, 500, 5);
	previousSignal = signalOn;
}

void HyundaiDashV1::updateOutputs() {
	bool oilState = (showLights & DL_OILWARN) || (ignitionState >= 2 && rpm < idleRPM / 2);
	bool batState = ignitionState >= 2 && rpm < idleRPM / 3;
	bool srsState = ignitionState >= 2 && (showLights & DL_SRS || rpm < idleRPM / 3);
	bool celState = (showLights & DL_CHECKENG) || (ignitionState >= 2 && rpm < idleRPM / 5) || celRequestedByShifter;
	if(!isICE) oilState = batState = celState = false;
	celRequestedByEngine = celState;

	bool lSignal = showLights & DL_SIGNAL_L;
	bool rSignal = showLights & DL_SIGNAL_R;

	digitalWrite(ignitionPin, ignitionState >= 2 ? HIGH : LOW);
	if(ignitionState >= 2) sendCANOnceAfterShutdown = true;
    
	if(HS1Role == HyundaiDashV1::HighSideRole::TSG) digitalWrite(HS1Pin, lSignal ? HIGH : LOW);

    switch(HS2Role) {
        case HyundaiDashV1::HighSideRole::TSG:
            digitalWrite(HS2Pin, rSignal ? HIGH : LOW);
            break;
        case HyundaiDashV1::HighSideRole::SRS:
            digitalWrite(HS2Pin, srsState ? HIGH : LOW);
            break;
    }
    
	if(LS1Role == HyundaiDashV1::LowSideRole::OIL) digitalWrite(LS1Pin, oilState ? HIGH : LOW);
    
    switch(LS2Role) {
        case HyundaiDashV1::LowSideRole::BAT:
            digitalWrite(LS2Pin, batState ? HIGH : LOW);
            break;
        case HyundaiDashV1::LowSideRole::CEL:
            digitalWrite(LS2Pin, celState ? HIGH : LOW);
            break;
    }
	turnSignalSound(lSignal || rSignal);
}

void HyundaiDashV1::updateSpeedAndRPM() {  
    speedData[speedDataIndex] = map(speedKMH, 0, maxSpeed, 0, 254);
    rpmData[rpmDataIndex]     = isICE 
                            ? map(rpm, 0, maxRPM, 0, 127)
                            : map(throttle, 0, 100, 0, 127);
}

void HyundaiDashV1::gearSelector() {
    celRequestedByShifter = false;
    switch(gear) {
        case 'P': gearData[gearDataIndexA]=0x00; gearData[gearDataIndexB]=0x10; break; // P
        case '-': // ASCII for minus
        case 'R': gearData[gearDataIndexA]=0x00; gearData[gearDataIndexB]=0x07; break; // -/R 
        case 'N': gearData[gearDataIndexA]=0x00; gearData[gearDataIndexB]=0x06; break; // N
        case 'D': gearData[gearDataIndexA]=0x00; gearData[gearDataIndexB]=0x05; break; // D
        default: // manual or S
        switch(gearIndex) {
            case 0: gearData[0]=0x00; gearData[gearDataIndexB]=0x06; break; // N
            case 1: gearData[0]=0x01; gearData[gearDataIndexB]=gearDataIndexBValueMT; break;
            case 2: gearData[0]=0x02; gearData[gearDataIndexB]=gearDataIndexBValueMT; break;
            case 3: gearData[0]=0x03; gearData[gearDataIndexB]=gearDataIndexBValueMT; break;
            case 4: gearData[0]=0x04; gearData[gearDataIndexB]=gearDataIndexBValueMT; break;
            case 5: gearData[0]=0x05; gearData[gearDataIndexB]=gearDataIndexBValueMT; break;
            case 6: gearData[0]=0x06; gearData[gearDataIndexB]=gearDataIndexBValueMT; break;
            default: gearData[0]=0x06; gearData[gearDataIndexB]=gearDataIndexBValueMT; celRequestedByShifter = true;
        }
    }
}

void HyundaiDashV1::updateCoolant() {
    uint8_t idx;
    if(waterTemp <= 50) idx = 0;
    else if(waterTemp > 50 && waterTemp <= 60) idx = 1;
    else if(waterTemp > 60 && waterTemp <= 70) idx = 2;
    else if(waterTemp > 70 && waterTemp <= 80) idx = 3;
    else if(waterTemp > 80 && waterTemp < 85) idx = 4;
    else if(waterTemp >= 85 && waterTemp <= 95) idx = 5; // middle
    else if(waterTemp > 95 && waterTemp <= 100) idx = 6;
    else if(waterTemp > 100 && waterTemp <= 105) idx = 7;
    else if(waterTemp > 105 && waterTemp <= 110) idx = 8;
    else if(waterTemp > 110 && waterTemp <= 115) idx = 9;
    else if(waterTemp > 115 && waterTemp <= 120) idx = 10;
    else if(waterTemp > 120) idx = 11;
    else idx = 0;

    ectData[ectDataIndex] = getEctValueAtIndex(idx);
}

void HyundaiDashV1::updateBrakes() {
    bool tcLightActive = showLights & DL_TC;

    uint8_t lightState = 0;
    if(showLights & DL_ABS) lightState += brakesABSLightValue; // ABS
    if(showLights & DL_HANDBRAKE) lightState += brakesBrakeLightValue; // handbrake
    if(tcLightActive){
        unsigned long currentMillis = millis();
        lightState += brakesESCLightValue; //ESC
        if(!previousTCLightStatus) tcLightActiveTime = currentMillis; //TC Light has just turned on, start timer
        if(currentMillis - tcLightActiveTime >= tcIntervalMs) lightState += brakesESCOffLightValue; //TC Light has been on for >= tcIntervalMs -> ESC Off
    }
    else tcLightActiveTime = 0;
    brakeData[brakeDataIndex] = lightState;

    previousTCLightStatus = tcLightActive;
}

void HyundaiDashV1::updateLights() {
    bool tailLights = showLights & DL_LOHIBEAM;
    bool highBeam = showLights & DL_HIGHBEAM;
    bool fogLights = showLights & DL_FOG;
    bool cruiseActive = showLights & DL_CRUISE;

    bool leftTurn = showLights & DL_SIGNAL_L;
    bool rightTurn = showLights & DL_SIGNAL_R;
    
    // Reset values and reinit the buffers
    resetCANLightBuffers();
    
    // Tail lights
    if(HS1Role == HyundaiDashV1::HighSideRole::ILL) digitalWrite(HS1Pin, tailLights ? HIGH : LOW);
    if(hasBodyCAN) {
        if(tailLightID == B_CANLight0ID) {
            B_CANLight0Data[tailLightIndex0] += tailLights ? tailLightIndex0Value : 0;
            B_CANLight0Data[tailLightIndex1] += tailLights ? tailLightIndex1Value : 0;
        }
        else if(tailLightID == B_CANLight1ID) {
            B_CANLight1Data[tailLightIndex0] += tailLights ? tailLightIndex0Value : 0;
            B_CANLight1Data[tailLightIndex1] += tailLights ? tailLightIndex1Value : 0;
        }
    }
    else {
        C_CANLightData[tailLightIndex0] += tailLights ? tailLightIndex0Value : 0;
        C_CANLightData[tailLightIndex1] += tailLights ? tailLightIndex1Value : 0;
    }
    
    // High beam
    if(hasBodyCAN) {
        if(highBeamID == B_CANLight0ID) B_CANLight0Data[highBeamIndex] += highBeam ? highBeamValue : 0;
        else if(highBeamID == B_CANLight1ID) B_CANLight1Data[highBeamIndex] += highBeam ? highBeamValue : 0;
    }
    if(highBeamID == C_CANLightID) C_CANLightData[highBeamIndex] += highBeam ? highBeamValue : 0;
    
    // Fog lights
    if(hasBodyCAN) {
        if(fogLightID == B_CANLight0ID) B_CANLight0Data[fogLightIndex] += fogLights ? fogLightValue : 0;
        else if(fogLightID == B_CANLight1ID) B_CANLight1Data[fogLightIndex] += fogLights ? fogLightValue : 0;
    }
    if(fogLightID == C_CANLightID) C_CANLightData[fogLightIndex] += fogLights ? fogLightValue : 0;
    
    // Turn signals
    if(hasBodyCAN) {
        if(turnSignalID == B_CANLight0ID) {
            B_CANLight0Data[turnSignalIndex] += leftTurn ? turnSignalLeftValue : 0;
            B_CANLight0Data[turnSignalIndex] += rightTurn ? turnSignalRightValue : 0;
        }
        else if(turnSignalID == B_CANLight1ID) {
            B_CANLight1Data[turnSignalIndex] += leftTurn ? turnSignalLeftValue : 0;
            B_CANLight1Data[turnSignalIndex] += rightTurn ? turnSignalRightValue : 0;
        }
    }
    if(turnSignalID == C_CANLightID) {
        C_CANLightData[turnSignalIndex] += leftTurn ? turnSignalLeftValue : 0;
        C_CANLightData[turnSignalIndex] += rightTurn ? turnSignalRightValue : 0;
    }
    
    // Cruise Control
    cruiseData[cruiseDataIndex] = cruiseActive ? cruiseDataValue : 0;
    
    // Extra
    extraData[extraDataIndex] = extraDataValue;
}

void HyundaiDashV1::updateCAN() {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= canInterval) {
        previousMillis = currentMillis;

        updateBrakes();
        updateLights();

        if(ignitionState < 2) { // reset taillights to prevent keeping cluster alive after ignition off
            if(sendCANOnceAfterShutdown) {
                sendCANOnceAfterShutdown = false;
                resetCANLightBuffers();
            }
            else return;
        }

        C_CAN.sendMsgBuf(speedID, 0, 8, speedData);
        C_CAN.sendMsgBuf(rpmID, 0, 8, rpmData);
        C_CAN.sendMsgBuf(gearID, 0, 8, gearData);
        C_CAN.sendMsgBuf(ectID, 0, 8, ectData);
        C_CAN.sendMsgBuf(brakeID, 0, 8, brakeData);
        C_CAN.sendMsgBuf(epsID, 0, 8, epsData);
        C_CAN.sendMsgBuf(tpmsID, 0, 8, tpmsData);
        if(!(celRequestedByEngine || celRequestedByShifter)) C_CAN.sendMsgBuf(celID, 0, 8, celData);
        C_CAN.sendMsgBuf(cruiseID, 0, 8, cruiseData);
        
        if(extraDataID) C_CAN.sendMsgBuf(extraDataID, 0, 8, extraData);
        
        if(hasBodyCAN) {
            B_CAN.sendMsgBuf(B_CANLight0ID, 0, 8, B_CANLight0Data);
            B_CAN.sendMsgBuf(B_CANLight1ID, 0, 8, B_CANLight1Data);
        }
        else C_CAN.sendMsgBuf(C_CANLightID, 0, 8, C_CANLightData);
        
    }
}

void HyundaiDashV1::resetCANLightBuffers() {
    memset(B_CANLight0Data, 0, sizeof(B_CANLight0Data));
    memset(B_CANLight1Data, 0, sizeof(B_CANLight1Data));
    memset(C_CANLightData, 0, sizeof(C_CANLightData));
    if(hasBodyCAN) {
        if(fuseSwID == B_CANLight0ID) B_CANLight0Data[fuseSwIndex] = fuseSwValue;
        else if(fuseSwID == B_CANLight1ID) B_CANLight1Data[fuseSwIndex] = fuseSwValue;
    }
    if(fuseSwID == C_CANLightID) C_CANLightData[fuseSwIndex] = fuseSwValue;
}

void HyundaiDashV1::applyTelemetry(const Telemetry& t) {
    ignitionState = t.ignitionState;
    speedKMH      = t.speedKMH; if(speedKMH > maxSpeed) speedKMH = maxSpeed;
    rpm           = t.rpm; if(rpm < 0) rpm = 0; if(rpm > maxRPM) rpm = maxRPM;
    gear          = t.gear;
    gearIndex     = t.gearIndex; if(gearIndex == 255) gearIndex = 0;
    waterTemp     = t.waterTemp; if(waterTemp < 0) waterTemp = 0; if(waterTemp > maxTemp) waterTemp = maxTemp;
    showLights    = t.showLights;
    idleRPM       = t.idleRPM; isICE = idleRPM > 0;
    fuelCapacity  = t.fuelCapacity;
    fuelVolume    = t.fuelVolume;
    throttle      = t.throttle;
    clutch        = t.clutch;
}

