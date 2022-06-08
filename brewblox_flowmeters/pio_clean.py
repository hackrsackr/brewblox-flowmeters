import subprocess

pio_clean = subprocess.run('pio run -t clean', shell=True)

if __name__ == '__main__':
    pio_clean
