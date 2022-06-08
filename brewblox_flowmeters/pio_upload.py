import subprocess

upload_command = 'pio run -t upload'
pio_upload = subprocess.run(upload_command, shell=True)


if __name__ == '__main__':
    pio_upload
