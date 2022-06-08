import subprocess

pio_upload = subprocess.run('pio run -t upload', shell=True)

if __name__ == '__main__':
    pio_upload
