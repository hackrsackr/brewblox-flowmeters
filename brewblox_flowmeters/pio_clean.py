import subprocess

def clean():
    pio_clean = subprocess.run('pio run -t clean', shell=True)
    pio_clean

if __name__ == '__main__':
    clean()
