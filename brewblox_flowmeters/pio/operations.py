import subprocess

def upload():
    upload = subprocess.run('pio run -t upload', shell=True)
    upload

def monitor():
    monitor = subprocess.run('pio device monitor -b 115200', shell=True)
    monitor

def build():
    build = subprocess.run('pio run', shell=True)
    build

def clean():
    clean = subprocess.run('pio run -t clean', shell=True)
    clean