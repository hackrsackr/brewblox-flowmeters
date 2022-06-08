import subprocess

def monitor():
    pio_monitor = subprocess.run('pio device monitor -b 115200', shell=True)
    pio_monitor

if __name__ == '__main__':
    monitor()
