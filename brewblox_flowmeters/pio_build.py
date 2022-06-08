import subprocess

def build():
    pio_build = subprocess.run('pio run', shell=True)
    pio_build

if __name__ == '__main__':
    build()
