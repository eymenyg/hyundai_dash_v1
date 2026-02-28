# How to use the software
1. Download the repository by clicking the green **Code** button and then **Download ZIP**
    - For BeamNG.drive, copy the **hyundai_dash_v1_bng_(latest-date).zip** file under the **BeamNG** folder to **C:\Users\\*username*\\AppData\Local\BeamNG\BeamNG.drive\current\mods**
    - For ATS/ETS2, install [ETS2 Telemetry Server](https://github.com/Funbit/ets2-telemetry-server?tab=readme-ov-file#installation) and run it
        - From the **Network Interfaces** menu, select **Loopback Pseudo-Interface 1**. The server IP should be **127.0.0.1**
    - For Assetto Corsa, install the **hyundaidashv1_ac_(latest-date).zip** under the **Assetto Corsa** folder. You will also need [Content Manager](https://acstuff.club/app/) and [Custom Shaders Pack](https://acstuff.club/patch/).
2. Download **hdv1_gui.exe** from the **Releases** section on the right side of the page
3. Open Device Manager in Windows, go to **Ports (COM & LPT)**, check which port is the Arduino on. You can determine it
    by unplugging and plugging it back and observing the window.
3. Run **hdv1_gui.exe**, select the game and the port. You can also build the executable from the scripts by yourself.

# (Optional) Building the executable
1. Install Python if not installed: [Python.org](https://www.python.org). Make sure to add Python to PATH when asked.
2. Install the pyserial and pyinstaller packages by opening a cmd/PowerShell/Terminal window and running the command: `pip install pyserial pyinstaller`
3. Navigate to the *scripts* folder and run `pyinstaller --onefile --windowed hdv1_gui.py`. The hdv1_gui.exe will be under the *dist* folder.
