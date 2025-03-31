# Raspberry Pi 5 Setup for Sheep Study
#### 1. Configure UART Serial Port
1. Open Raspberry Pi configuration tool:
    ```bash
    sudo raspi-config
    ```
2. Navigate to **Interface Options** > **Serial Port**.
3. When prompted:
    - Select **No** to disable the login shell over serial.
    - Select **Yes** to enable the serial hardware.
4. Verify the configuration:
    ```bash
    ls -l /dev/serial*
    ```
    Ensure `/dev/ttyAMA10` is listed. If `ttyAMA10` does not appear, double-check the configuration.
#### 2. Update Python Script for the New Raspberry Pi
1. Open the Python script (`reciver.py`) in a text editor.
2. Update the following:
    - Set `log_directory` to match the new Raspberry Pi's name.
    - Ensure the serial port is correctly specified:
        ```python
        ser = serial.Serial('/dev/ttyAMA0', 115200, timeout=0)
        ```
3. Save the changes.
#### 3. Set Up a Virtual Environment
1. Create a virtual environment:
    ```bash
    python3 -m venv ~/myenv
    ```
2. Activate the virtual environment:
    ```bash
    source ~/myenv/bin/activate
    ```
    **Note**: This step must be repeated every time you reboot the Raspberry Pi.
#### 4. Install Required Python Libraries
While inside the virtual environment:
```bash
pip install boto3 pyserial pycryptodome pandas
```
#### 5. Configure AWS Credentials
1. Edit the ~/.bashrc file to add AWS credentials:
    ```bash
    nano ~/.bashrc
    ```
    Add the following lines:
    ```bash
    export AWS_ACCESS_KEY_ID="your-access-key-id"
    export AWS_SECRET_ACCESS_KEY="your-secret-access-key"
    ```
2. Apply the changes:
    ```bash
    source ~/.bashrc
    ```
    Verify the credentials are loaded:
    ```bash
    echo $AWS_ACCESS_KEY_ID
    echo $AWS_SECRET_ACCESS_KEY
    ```
    Both commands should output the respective values.

## You Are Ready to Go!
Follow these steps for every new Raspberry Pi setup. If any issues arise, contact the team for assistance.
