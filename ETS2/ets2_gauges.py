import time
import serial
import requests
import struct

RATE = 15
PERIOD = 1.0 / RATE

API_URL = "http://127.0.0.1:25555/api/ets2/telemetry"
COM_PORT = "COM3"
BAUD_RATE = 115200

START_BYTE = 0xAA
END_BYTE   = 0x55


def remap(x, in_min, in_max, out_min, out_max):
    if in_max == in_min:
        return out_min
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min))


def api_to_com():
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=0.5)

    try:
        while True:
            start = time.time()
            t = requests.get(API_URL).json()["truck"]

            ign = 2 if t["electricOn"] else 0
            speed = abs(int(t["speed"]))
            rpm = remap(t["engineRpm"], 0, t["engineRpmMax"], 0, 6500)
            idle_rpm = remap(500, 0, t["engineRpmMax"], 0, 6500)
            water = abs(int(t["waterTemperature"]))

            # Gear handling
            dg = t["displayedGear"]
            is_auto = t["shifterType"] in ("arcade", "automatic")

            if dg < 0:
                gear, gear_idx = 82, 0      # R
            elif dg == 0:
                gear, gear_idx = 78, 0      # N
            else:
                gear_idx = min((dg - 1) % 6 + 1, 6)
                gear = 68 if is_auto else gear_idx  # D or 1–6

            # Lights bitmask
            lights = 0
            lights |= 1    if t["lightsParkingOn"] else 0
            lights |= 2    if t["lightsBeamHighOn"] else 0
            lights |= 4    if t["wearEngine"] > .3 or t["wearTransmission"] > .3 else 0
            lights |= 8    if t["oilPressureWarningOn"] else 0
            lights |= 16   if t["parkBrakeOn"] or t["airPressureWarningOn"] or t["airPressureEmergencyOn"] else 0
            lights |= 32   if dg > 6 and not is_auto else 0
            lights |= 64   if t["blinkerLeftOn"] else 0
            lights |= 128  if t["blinkerRightOn"] else 0
            lights |= 256  if t["lightsAuxFrontOn"] or t["lightsAuxRoofOn"] else 0
            lights |= 512  if t["cruiseControlOn"] else 0
            lights |= 1024 if t["motorBrakeOn"] or t["retarderBrake"] > 0 else 0

            fuel_cap = int(t["fuelCapacity"])
            fuel_vol = int(t["fuel"])
            throttle = int(t["gameThrottle"] * 100)
            clutch = int(t["gameClutch"] * 100)

            packet = struct.pack(
                "<BBHHbBBHHHHBBB",
                START_BYTE,            # start
                ign,
                speed,
                rpm,
                gear,
                gear_idx,
                water,
                lights,
                idle_rpm,
                fuel_cap,
                fuel_vol,
                throttle,
                clutch,
                END_BYTE             # end
            )

            ser.write(packet)

            sleep = PERIOD - (time.time() - start)
            if sleep > 0:
                time.sleep(sleep)

    except KeyboardInterrupt:
        print("Stopping...")
    finally:
        ser.close()


if __name__ == "__main__":
    api_to_com()
