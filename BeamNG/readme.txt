1. Install Python if not installed: https://www.python.org. Make sure to add Python to PATH when asked.
2. Install the pyserial package by opening a cmd/PowerShell/Terminal window and running the command: pip install pyserial
3. Open Device Manager in Windows, go to Ports (COM & LPT), check which port is the Arduino on. You can determine it
    by unplugging and plugging it back and observing the window.
4. Open beam_gauges.py in Notepad or other editor and edit the COM_PORT variable accordingly and save the file. By default, it is COM3.
5. Copy the zip file under the mods folder to C:\Users\**username**\AppData\Local\BeamNG\BeamNG.drive\current\mods
6. Double click the beam_gauges.py file to run it. Launch BeamNG and enjoy!