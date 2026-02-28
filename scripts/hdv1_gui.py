import tkinter as tk
from tkinter import ttk, messagebox
import threading
import serial.tools.list_ports
import queue
import time
import beamng
import ats_ets2

# =========================
# Configuration
# =========================
GAME_MODULES = {
    "ATS/ETS2": ats_ets2,
    "BeamNG.drive": beamng,
}

# =========================
# Globals
# =========================
script_thread = None
stop_event = None
log_queue = queue.Queue()
last_lines = []

# =========================
# Helper functions
# =========================
def get_com_ports():
    ports = serial.tools.list_ports.comports()
    return [p.device for p in ports]

def log(message):
    """Add a new line to the GUI log"""
    log_queue.put(message)

def update_log():
    global last_lines
    try:
        while True:
            line = log_queue.get_nowait()
            last_lines.append(line)
            if len(last_lines) > 2:
                last_lines = last_lines[-2:]  # keep last 2 lines
    except queue.Empty:
        pass

    log_text.config(state="normal")
    log_text.delete(1.0, tk.END)
    log_text.insert(tk.END, "\n".join(last_lines))
    log_text.config(state="disabled")
    log_text.see(tk.END)

    root.after(100, update_log)

# =========================
# Script control
# =========================
def run_script():
    global script_thread, stop_event

    game = game_var.get()
    port = port_var.get()
    if not game or not port:
        messagebox.showerror("Error", "Please select a game and COM port")
        return

    stop_script()  # stop previous script if any

    # Disable dropdowns and toggle button
    game_menu.config(state="disabled")
    port_menu.config(state="disabled")
    scale_checkbox.config(state="disabled")
    start_button.config(text="Stop", command=stop_script)

    # Clear log
    log_text.config(state="normal")
    log_text.delete(1.0, tk.END)
    log_text.config(state="disabled")

    stop_event = threading.Event()

    def target():
        module = GAME_MODULES[game]
        try:
            module.run(port, log_func=log, stop_event=stop_event, scale_rpm=scale_rpm_var.get())
        except Exception as e:
            log(f"Error: {e}")

    script_thread = threading.Thread(target=target, daemon=True)
    script_thread.start()
    status_label.config(text=f"Running {game} on {port}", fg="green")

def stop_script():
    global script_thread, stop_event
    if stop_event:
        stop_event.set()
    if script_thread:
        script_thread.join(timeout=2)
    script_thread = None
    stop_event = None

    # Re-enable GUI controls
    game_menu.config(state="readonly")
    port_menu.config(state="readonly")
    scale_checkbox.config(state="normal")
    start_button.config(text="Start", command=run_script)
    status_label.config(text="No script running", fg="gray")

def refresh_ports(event=None):
    ports = get_com_ports()
    port_menu["values"] = ports
    if ports:
        if port_var.get() not in ports:
            port_var.set(ports[0])
    else:
        port_var.set("")

# =========================
# GUI Setup
# =========================
root = tk.Tk()
root.title("Hyundai Dash V1 Launcher")
root.geometry("360x300")
root.resizable(False, False)

game_var = tk.StringVar()
port_var = tk.StringVar()

scale_rpm_var = tk.BooleanVar(value=False)

# Widgets
tk.Label(root, text="Select Game").pack(pady=(10, 5))
game_menu = ttk.Combobox(
    root,
    textvariable=game_var,
    values=list(GAME_MODULES.keys()),
    state="readonly"
)
game_menu.pack()

tk.Label(root, text="Select COM Port").pack(pady=(10, 5))
port_menu = ttk.Combobox(
    root,
    textvariable=port_var,
    values=get_com_ports(),
    state="readonly"
)
port_menu.pack()

scale_checkbox = tk.Checkbutton(
    root,
    text="Scale RPM",
    variable=scale_rpm_var
)
scale_checkbox.pack(pady=(5, 5))

start_button = tk.Button(
    root,
    text="Start",
    command=run_script,
    height=2,
    width=18
)
start_button.pack(pady=10)

status_label = tk.Label(root, text="No script running", fg="gray")
status_label.pack()

tk.Label(root, text="Script Output:").pack(pady=(10, 0))
log_text = tk.Text(root, height=2, width=45, state="disabled", bg="#f0f0f0")
log_text.pack(pady=(0, 10))

# =========================
# Events
# =========================
root.protocol("WM_DELETE_WINDOW", lambda: [stop_script(), root.destroy()])
root.bind("<FocusIn>", refresh_ports)
update_log()
refresh_ports()

# =========================
# Start GUI
# =========================
root.mainloop()
