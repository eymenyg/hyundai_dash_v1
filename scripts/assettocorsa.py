import serial
import socket
import time
import struct

RATE = 20
PERIOD = 1.0 / RATE
UDP_HOST = "127.0.0.1"
UDP_PORT = 9533
BAUD_RATE = 115200
START_BYTE = 0xAA
END_BYTE = 0x55

cluster_max_rpm = 8000

def remap(x, in_min, in_max, out_min, out_max):
    if in_max == in_min: 
        return out_min
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min))

def run(COM_PORT, log_func=print, stop_event=None, scale_rpm=False):
    """Main function to start Assetto Corsa telemetry"""
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind((UDP_HOST, UDP_PORT))
    #udp_socket.settimeout(0.1)  # short timeout to check stop_event quickly
    udp_socket.setblocking(False)

    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=0.1)  # short timeout

    log_func(f"Listening on UDP {UDP_HOST}:{UDP_PORT}")
    log_func(f"Sending to {COM_PORT}")
    
    try:
        while stop_event is None or not stop_event.is_set():
            start = time.time()

            # Check stop_event before recv
            if stop_event and stop_event.is_set():
                break

            latest = None

            while True:
                try:
                    data, _ = udp_socket.recvfrom(64)
                    latest = data
                except BlockingIOError:
                    break

            if latest is None:
                time.sleep(0.001)
                continue

            data = latest

            if len(data) < 26:
                continue

            (
                ign, speed, rpm, gear, gear_idx, water, lights,
                idle_rpm, max_rpm, fuel_cap, fuel_vol, throttle, clutch
            ) = struct.unpack('<HHHHHHHHHHHHH', data[:26])

            # Clamp values
            ign &= 0xFF
            gear = int(gear & 0xFF)
            gear_idx &= 0xFF
            water &= 0xFF
            throttle = min(throttle, 100)
            clutch = min(clutch, 100)
            
            if scale_rpm:
                rpm = remap(rpm, 0, max_rpm, 0, cluster_max_rpm)
                idle_rpm = remap(idle_rpm, 0, max_rpm, 0, cluster_max_rpm)

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
    udp_socket.close()
    log_func("Shutting down Assetto Corsa script")
