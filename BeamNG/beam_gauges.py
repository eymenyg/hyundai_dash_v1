import serial
import socket
import time
import struct

RATE = 15
PERIOD = 1.0 / RATE

UDP_HOST = "127.0.0.1"
UDP_PORT = 9532
COM_PORT = "COM3"
BAUD_RATE = 115200

START_BYTE = 0xAA
END_BYTE   = 0x55


def udp_to_com():
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind((UDP_HOST, UDP_PORT))
    udp_socket.settimeout(1.0)

    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=0.5)

    print(f"Listening on UDP {UDP_HOST}:{UDP_PORT}")

    try:
        while True:
            start = time.time()

            try:
                data, _ = udp_socket.recvfrom(64)
            except socket.timeout:
                continue

            if len(data) < 24:
                continue

            # Incoming BeamNG UDP (12x uint16)
            (
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
                clutch
            ) = struct.unpack('<HHHHHHHHHHHH', data[:24])

            # Clamp to Nano-sized fields
            ign        &= 0xFF
            gear       = int(gear & 0xFF)
            gear_idx   &= 0xFF
            water      &= 0xFF
            throttle   = min(throttle, 100)
            clutch     = min(clutch, 100)

            # Pack into FINAL dashboard packet
            packet = struct.pack(
                "<BBHHbBBHHHHBBB",
                START_BYTE,
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
                END_BYTE
            )

            ser.write(packet)

            elapsed = time.time() - start
            if elapsed < PERIOD:
                time.sleep(PERIOD - elapsed)

    except KeyboardInterrupt:
        print("Shutting down...")

    finally:
        ser.close()
        udp_socket.close()


if __name__ == "__main__":
    udp_to_com()
