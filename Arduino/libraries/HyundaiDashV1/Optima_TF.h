// 2014-2015 Kia Optima (TF F/L)

#include <HyundaiDashV1.h>

HyundaiDashV1 cluster(
    // HS1Role, HS2Role
    HyundaiDashV1::HighSideRole::ILL, HyundaiDashV1::HighSideRole::NON,
    // LS1Role, LS2Role
    HyundaiDashV1::LowSideRole::OIL, HyundaiDashV1::LowSideRole::BAT,
    // C_CANLightID, tailLightID, tailLightIndex0, tailLightIndex0Value, tailLightIndex1, tailLightIndex1Value
    0x0, 0x100, 4, 64, 0, 0,
    // highBeamID, highBeamIndex, highBeamValue
    0x104, 2, 32,
    // fogLightID, fogLightIndex, fogLightValue
    0x104, 2, 4,
    // fuseSwID, fuseSwIndex, fuseSwValue
    0x104, 5, 64,
    // extraDataID, extraDataIndex, extraDataValue
    0x44, 3, 132, //25*C
    // turnSignalID, turnSignalIndex, turnSignalLeftValue, turnSignalRightValue
    0x104, 3, 1, 2,
    //hasBodyCAN, enableTurnSignalSound, maxSpeed
    true, false, 260
);