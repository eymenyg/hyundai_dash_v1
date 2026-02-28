import time
import serial
import requests
import struct

RATE = 15
PERIOD = 1.0 / RATE
API_URL = "http://127.0.0.1:25555/api/ets2/telemetry"
BAUD_RATE = 115200
START_BYTE = 0xAA
END_BYTE = 0x55

set_idle_rpm = 500
cluster_max_rpm = 8000

def remap(x, in_min, in_max, out_min, out_max):
    if in_max == in_min: 
        return out_min
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min))

def run(COM_PORT, log_func=print, stop_event=None, scale_rpm=False):
    """Main function to start ATS/ETS2 telemetry"""
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=0.1)  # short timeout for quick exit

    log_func(f"Listening to API at {API_URL}")
    log_func(f"Sending to {COM_PORT}")

    try:
        while stop_event is None or not stop_event.is_set():
            start = time.time()

            # Check stop_event immediately before API request
            if stop_event and stop_event.is_set():
                break

            try:
                t = requests.get(API_URL, timeout=1).json()["truck"]
            except Exception:
                continue

            ign = 2 if t["electricOn"] else 0
            speed = abs(int(t["speed"]))
            rpm = abs(int(t["engineRpm"]))
            idle_rpm = set_idle_rpm
            if scale_rpm:
                rpm = remap(rpm, 0, t["engineRpmMax"], 0, cluster_max_rpm)
                idle_rpm = remap(idle_rpm, 0, t["engineRpmMax"], 0, cluster_max_rpm)
            water = abs(int(t["waterTemperature"]))

            dg = t["displayedGear"]
            is_auto = t["shifterType"] in ("arcade", "automatic")
            if dg < 0:
                gear, gear_idx = 82, 0
            elif dg == 0:
                gear, gear_idx = 78, 0
            else:
                gear_idx = min((dg - 1) % 6 + 1, 6)
                gear = 68 if is_auto else gear_idx

            lights = 0
            lights |= 1 if t["lightsParkingOn"] else 0
            lights |= 2 if t["lightsBeamHighOn"] else 0
            lights |= 4 if t["wearEngine"] > 0.3 or t["wearTransmission"] > 0.3 else 0
            lights |= 8 if t["oilPressureWarningOn"] else 0
            lights |= 16 if t["parkBrakeOn"] or t["airPressureWarningOn"] or t["airPressureEmergencyOn"] else 0
            lights |= 32 if dg > 6 and not is_auto else 0
            lights |= 64 if t["blinkerLeftOn"] else 0
            lights |= 128 if t["blinkerRightOn"] else 0
            lights |= 256 if t["lightsAuxFrontOn"] or t["lightsAuxRoofOn"] else 0
            lights |= 512 if t["cruiseControlOn"] else 0
            lights |= 1024 if t["motorBrakeOn"] or t["retarderBrake"] > 0 else 0

            fuel_cap = int(t["fuelCapacity"])
            fuel_vol = int(t["fuel"])
            throttle = int(t["gameThrottle"]*100)
            clutch = int(t["gameClutch"]*100)

            packet = struct.pack(
                "<BBHHbBBHHHHBBB",
                START_BYTE, ign, speed, rpm, gear, gear_idx, water, lights,
                idle_rpm, fuel_cap, fuel_vol, throttle, clutch, END_BYTE
            )
            ser.write(packet)

            # Sleep in small chunks to allow immediate stop
            sleep_time = PERIOD - (time.time() - start)
            if sleep_time > 0:
                interval = 0.01
                for _ in range(int(sleep_time / interval)):
                    if stop_event and stop_event.is_set():
                        break
                    time.sleep(interval)

    finally:
        try:
            # Trigger Arduino reset via DTR
            ser.dtr = False
            time.sleep(0.1)
            ser.dtr = True
            time.sleep(0.1)
        except:
            pass
        ser.close()
        log_func("Shutting down ATS/ETS2 script")