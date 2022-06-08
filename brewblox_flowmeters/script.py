import subprocess

run_command = 'pio run -t upload'
pio_run = subprocess.run(run_command, shell=True)

if __name__ == '__main__':
    pio_run
