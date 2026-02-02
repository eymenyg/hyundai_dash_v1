// 2014-2016 Kia Soul (PS)

#include <HyundaiDashV1.h>

HyundaiDashV1 cluster(
    // HS1Role, HS2Role
    HyundaiDashV1::HighSideRole::ILL, HyundaiDashV1::HighSideRole::NON,
    // LS1Role, LS2Role
    HyundaiDashV1::LowSideRole::OIL, HyundaiDashV1::LowSideRole::BAT,
    // C_CANLightID, tailLightID, tailLightIndex0, tailLightIndex0Value, tailLightIndex1, tailLightIndex1Value
    0x18, 0x18, 0, 32, 4, 1,
    // highBeamID, highBeamIndex, highBeamValue
    0x18, 2, 2,
    // fogLightID, fogLightIndex, fogLightValue
    0x18, 1, 2,
    // fuseSwID, fuseSwIndex, fuseSwValue
    0x18, 6, 32,
    // extraDataID, extraDataIndex, extraDataValue
    0x350, 3, 131, //77*F
    // turnSignalID, turnSignalIndex, turnSignalLeftValue, turnSignalRightValue
    0x18, 5, 64, 32,
    //hasBodyCAN, enableTurnSignalSound, maxSpeed
    false, false, 256
);