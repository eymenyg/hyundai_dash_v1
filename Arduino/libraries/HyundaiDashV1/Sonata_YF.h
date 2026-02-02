// 2011-2014 Hyundai Sonata (YF)

#include <HyundaiDashV1.h>

HyundaiDashV1 cluster(
    // HS1Role, HS2Role
    HyundaiDashV1::HighSideRole::TSG, HyundaiDashV1::HighSideRole::TSG,
    // LS1Role, LS2Role
    HyundaiDashV1::LowSideRole::OIL, HyundaiDashV1::LowSideRole::CEL,
    // C_CANLightID, tailLightID, tailLightIndex0, tailLightIndex0Value, tailLightIndex1, tailLightIndex1Value
    0x0, 0x104, 2, 8, 0, 0,
    // highBeamID, highBeamIndex, highBeamValue
    0x100, 4, 2,
    // fogLightID, fogLightIndex, fogLightValue
    0x100, 4, 1,
    // fuseSwID, fuseSwIndex, fuseSwValue
    0x0, 0, 0,
    // extraDataID, extraDataIndex, extraDataValue
    0x0, 0, 0,
    // turnSignalID, turnSignalIndex, turnSignalLeftValue, turnSignalRightValue
    0x0, 0, 0, 0,
    //hasBodyCAN, enableTurnSignalSound, maxSpeed
    true, false, 260
);