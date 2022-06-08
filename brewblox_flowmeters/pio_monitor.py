import subprocess

pio_monitor = subprocess.run('pio device monitor -b 115200', shell=True)

if __name__ == '__main__':
    pio_monitor
