import subprocess

monitor_command = 'pio device monitor -b 115200'
pio_monitor = subprocess.run(monitor_command, shell=True)

if __name__ == '__main__':
    pio_monitor
