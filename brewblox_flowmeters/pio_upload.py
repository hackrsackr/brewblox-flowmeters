import subprocess
from pio_monitor import monitor

def upload():
    pio_upload = subprocess.run('pio run -t upload', shell=True)
    pio_upload

if __name__ == '__main__':
    upload()
    monitor()
