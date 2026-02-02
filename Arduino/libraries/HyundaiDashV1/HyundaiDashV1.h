#pragma once
#include <mcp_can.h>

struct Telemetry {
    uint8_t  start;
    uint8_t  ignitionState;
    uint16_t speedKMH;
    uint16_t rpm;
    int8_t   gear;
    uint8_t  gearIndex;
    uint8_t  waterTemp;
    uint16_t showLights;
    uint16_t idleRPM;
    uint16_t fuelCapacity;
    uint16_t fuelVolume;
    uint8_t  throttle;
    uint8_t  clutch;
    uint8_t  end;
};

class HyundaiDashV1 {
public:
	enum class HighSideRole : uint8_t {
		TSG,
		ILL,
		SRS,
        NON
	};

	enum class LowSideRole : uint8_t {
		OIL,
		CEL,
		BAT,
        NON
	};

    HyundaiDashV1(
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
    );
    
    void run();
    void beginCAN();

private:
    // --- Functions ---
    void receiveSerialData();
    void updateFuel();
    void turnSignalSound(bool);
    void updateOutputs();
    void updateSpeedAndRPM();
    void gearSelector();
    void updateCoolant();
    void updateBrakes();
    void updateLights();
    void updateCAN();
    void applyTelemetry(const Telemetry&);
    uint8_t getEctValueAtIndex(uint8_t) const;
    void resetCANLightBuffers();

    // --- CAN controllers ---
    MCP_CAN C_CAN;
    MCP_CAN B_CAN;
	const bool hasBodyCAN;

    // --- CAN buffers ---
    uint8_t speedData[8]  = {0};
    uint8_t rpmData[8]    = {0};
    uint8_t gearData[8]   = {0};
    uint8_t ectData[8]    = {0};
    uint8_t brakeData[8]  = {0};
    uint8_t cruiseData[8] = {0};

    uint8_t celData[8]  = {0};
    uint8_t epsData[8]  = {0};
    uint8_t tpmsData[8] = {0};

    uint8_t B_CANLight0Data[8] = {0};
    uint8_t B_CANLight1Data[8] = {0};
    uint8_t C_CANLightData[8] = {0};
    uint8_t extraData[8]  = {0};

    // --- Fixed indices ---
    static constexpr uint8_t speedDataIndex = 2;
    static constexpr uint8_t rpmDataIndex   = 3;
    static constexpr uint8_t gearDataIndexA = 0;
    static constexpr uint8_t gearDataIndexB = 1;
    static constexpr uint8_t gearDataIndexBValueMT = 0x08;
    static constexpr uint8_t ectDataIndex   = 1;
    static constexpr uint8_t brakeDataIndex = 0;
    static constexpr uint8_t cruiseDataIndex = 3;
    
    static constexpr uint8_t brakesABSLightValue = 0x01;
    static constexpr uint8_t brakesBrakeLightValue = 0x02;
    static constexpr uint8_t brakesESCLightValue = 0x08;
    static constexpr uint8_t brakesESCOffLightValue = 0x04;
    
    static constexpr uint8_t cruiseDataValue = 0x02;

    static const uint8_t ectValues[12];

    // --- Pins ---
    static constexpr uint8_t dataPin = 3;
    static constexpr uint8_t latchPin = 4;
    static constexpr uint8_t clockPin = 5;
    static constexpr uint8_t masterResetPin = 6;
	static constexpr uint8_t B_CANPin = 7;
	static constexpr uint8_t C_CANPin = 8;
    static constexpr uint8_t ignitionPin = A4;
    static constexpr uint8_t HS1Pin = A3;
    static constexpr uint8_t HS2Pin = A2;
    static constexpr uint8_t LS1Pin = A1;
    static constexpr uint8_t LS2Pin = A0;
    static constexpr uint8_t buzzerPin = 9;

    // --- CAN IDs (fixed) ---
    static constexpr uint16_t speedID  = 0x440;
    static constexpr uint16_t rpmID    = 0x316;
    static constexpr uint16_t gearID   = 0x43F;
    static constexpr uint16_t ectID    = 0x329;
    static constexpr uint16_t brakeID  = 0x1F1;
    static constexpr uint16_t epsID    = 0x5E4;
    static constexpr uint16_t tpmsID   = 0x5F0;
    static constexpr uint16_t celID    = 0x545;
    static constexpr uint16_t cruiseID = 0x260;
    static constexpr uint16_t B_CANLight0ID = 0x100;
    static constexpr uint16_t B_CANLight1ID = 0x104;

    // --- Variant-dependent ---
    const uint16_t C_CANLightID;
    
    const uint16_t tailLightID;
    const uint8_t tailLightIndex0;
    const uint8_t tailLightIndex0Value;
    const uint8_t tailLightIndex1;
    const uint8_t tailLightIndex1Value;
    
    const uint16_t highBeamID;
    const uint8_t highBeamIndex;
    const uint8_t highBeamValue;
    
    const uint16_t fogLightID;
    const uint8_t fogLightIndex;
    const uint8_t fogLightValue;
    
    const uint16_t fuseSwID;
    const uint8_t fuseSwIndex;
    const uint8_t fuseSwValue;

    const uint16_t extraDataID;
    const uint8_t extraDataIndex;
    const uint8_t extraDataValue;
    
    const uint16_t turnSignalID;
    const uint8_t turnSignalIndex;
    const uint8_t turnSignalLeftValue;
    const uint8_t turnSignalRightValue;

	const HighSideRole HS1Role;
	const HighSideRole HS2Role;
	const LowSideRole  LS1Role;
	const LowSideRole  LS2Role;
	
	const bool enableTurnSignalSound;
	int previousSignal = 0;
	
	// --- Incoming active light bits ---
	enum DashLight : uint16_t {
        DL_LOHIBEAM  = 1 << 0,
        DL_HIGHBEAM  = 1 << 1,
        DL_CHECKENG  = 1 << 2,
        DL_OILWARN   = 1 << 3,
        DL_HANDBRAKE = 1 << 4,
        DL_TC        = 1 << 5,
        DL_SIGNAL_L  = 1 << 6,
        DL_SIGNAL_R  = 1 << 7,
        DL_FOG       = 1 << 8,
        DL_CRUISE    = 1 << 9,
        DL_ABS       = 1 << 10,
        DL_SRS       = 1 << 11
    };
	
	// --- Variables ---
	uint8_t ignitionState = 0;
	uint16_t speedKMH = 0;
	uint16_t rpm = 0;
	uint8_t gear = 0;
	uint8_t gearIndex = 0;
	uint8_t waterTemp = 0;
	uint16_t showLights = 0;
	uint16_t idleRPM = 0;
	uint16_t fuelCapacity = 0;
	uint16_t fuelVolume = 0;
	uint8_t throttle = 0;
	uint8_t clutch = 0;
    
    Telemetry rxPacket;
	
	// CAN send interval
	unsigned long previousMillis = 0;
	static constexpr unsigned long canInterval = 25;
	
	const uint16_t maxSpeed;
    static constexpr uint16_t maxRPM = 8000;
    static constexpr uint8_t maxTemp = 130;
	bool sendCANOnceAfterShutdown = true; // helps keep the taillights off after shutdown
	bool celRequestedByEngine = false;
	bool celRequestedByShifter = false; // use the CEL to indicate gear > 6
	bool isICE = true;
    
    unsigned long tcLightActiveTime = 0;
    bool previousTCLightStatus = false;
    static constexpr unsigned long tcIntervalMs = 500;

	// Serial buffer
    bool newData = false;
};
