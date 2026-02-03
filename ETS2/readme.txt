1. Install Python if not installed: https://www.python.org. Make sure to add Python to PATH when asked.
2. Install the pyserial package by opening a cmd/PowerShell/Terminal window and running the command: pip install pyserial
3. Open Device Manager in Windows, go to Ports (COM & LPT), check which port is the Arduino on. You can determine it
    by unplugging and plugging it back and observing the window.
4. Open ets2_gauges.py in Notepad or other editor and edit the COM_PORT variable accordingly and save the file. By default, it is COM3.
5. Install ETS2 Telemetry Server: https://github.com/Funbit/ets2-telemetry-server?tab=readme-ov-file#installation
6. Run ETS2 Telemetry Server and launch ETS2,
6. Double click the ets2_gauges.py file to run it. Enjoy!