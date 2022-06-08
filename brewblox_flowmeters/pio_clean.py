import subprocess

clean_command = 'pio run -t upload'
pio_clean = subprocess.run(clean_command, shell=True)


if __name__ == '__main__':
    pio_clean
