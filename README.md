# How to use the software

1. Install Python if not installed: https://www.python.org. Make sure to add Python to PATH when asked.
2. Install the pyserial package by opening a cmd/PowerShell/Terminal window and running the command: `pip install pyserial`
3. Open Device Manager in Windows, go to Ports (COM & LPT), check which port is the Arduino on. You can determine it
    by unplugging and plugging it back and observing the window.
4. Download the repository by clicking the green **Code** button and then **Download ZIP**
    - For BeamNG.drive, copy the hyundai_gauges_(latest).zip file under the BeamNG/mods folder to C:\Users\\*username*\\AppData\Local\BeamNG\BeamNG.drive\current\mods
    - For ATS/ETS2, install [ETS2 Telemetry Server](https://github.com/Funbit/ets2-telemetry-server?tab=readme-ov-file#installation) and run it
6. Download hdv1_gui.exe from the **Releases** section on the right side of the page and run. You can also build the exe from the scripts by yourself.

# Building the exe yourself
1. Install pyinstaller: `pip install pyinstaller`
2. Put the script .py files into one folder
3. Run `pyinstaller --onefile --windowed hdv1_gui.py`
