import subprocess

def monitor():
    monitor = subprocess.run('pio device monitor -b 115200', shell=True)
    monitor

if __name__ == '__main__':
    monitor()
