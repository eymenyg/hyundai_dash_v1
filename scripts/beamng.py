import serial
import socket
import time
import struct

RATE = 15
PERIOD = 1.0 / RATE
UDP_HOST = "127.0.0.1"
UDP_PORT = 9532
BAUD_RATE = 115200
START_BYTE = 0xAA
END_BYTE = 0x55

def run(COM_PORT, log_func=print, stop_event=None):
    """Main function to start BeamNG telemetry"""
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind((UDP_HOST, UDP_PORT))
    udp_socket.settimeout(0.1)  # short timeout to check stop_event quickly

    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=0.1)  # short timeout

    log_func(f"Listening on UDP {UDP_HOST}:{UDP_PORT}")
    log_func(f"Sending to {COM_PORT}")

    try:
        while stop_event is None or not stop_event.is_set():
            start = time.time()

            # Check stop_event before recv
            if stop_event and stop_event.is_set():
                break

            try:
                data, _ = udp_socket.recvfrom(64)
            except socket.timeout:
                continue

            if len(data) < 24:
                continue

            (
                ign, speed, rpm, gear, gear_idx, water, lights,
                idle_rpm, fuel_cap, fuel_vol, throttle, clutch
            ) = struct.unpack('<HHHHHHHHHHHH', data[:24])

            # Clamp values
            ign &= 0xFF
            gear = int(gear & 0xFF)
            gear_idx &= 0xFF
            water &= 0xFF
            throttle = min(throttle, 100)
            clutch = min(clutch, 100)

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
        ser.close()
        udp_socket.close()
        log_func("Shutting down BeamNG script")
