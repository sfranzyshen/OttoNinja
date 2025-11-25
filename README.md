# OttoNinja Web App Control (Arduino Sketch and Filesystem)

This is the OttoNinja Web App Control Interface from [Otto Web App Control](https://github.com/OttoDIY/OttoWebAppControl) ... This project allows you to control an OttoNinja robot using a web interface served directly from an ESP8266. The web interface provides a joystick and buttons for controlling the robot's movements and modes.

## Features

-   **Web-based control:** Control your OttoNinja from any device with a web browser on the same network.
-   **Joystick and button controls:** Provides a virtual joystick for proportional control and buttons for standard movements.
-   **Walk and Roll modes:** Switch between walking and rolling modes.
-   **mDNS support:** Access the web interface using the hostname `http://ottoninja.local` instead of an IP address.
-   **LittleFS file system:** The web interface files are stored on the ESP8266's flash memory using LittleFS.

## Hardware Requirements

-   An ESP8266-based board (e.g., NodeMCU, Wemos D1 Mini).
-   An OttoNinja robot with servo motors connected to the appropriate pins as defined in `OttoNinja.ino`.

## Software Setup

### 1. Arduino IDE Setup

-   Install the latest version of the [Arduino IDE](https://www.arduino.cc/en/software).

### 2. ESP8266 Board Manager

-   In the Arduino IDE, go to **File > Preferences**.
-   In the "Additional Boards Manager URLs" field, add the following URL:
    ```
    http://arduino.esp8266.com/stable/package_esp8266com_index.json
    ```
-   Go to **Tools > Board > Boards Manager...**.
-   Search for "esp8266" and install the "esp8266 by ESP8266 Community" package.
-   Select your ESP8266 board from the **Tools > Board** menu (e.g., "NodeMCU 1.0 (ESP-12E Module)").

### 3. Libraries

The following libraries are required. They can be installed from the Arduino IDE's Library Manager (**Tools > Manage Libraries...**).

-   `ESP8266WiFi` (included with the ESP8266 board package)
-   `Servo` (standard Arduino library)
-   `ESP8266mDNS` (included with the ESP8266 board package)

### 4. Uploading the Filesystem Image

The web interface files are located in the `data` directory. These files need to be uploaded to the ESP8266's LittleFS file system.

-   Install the Arduino ESP8266 LittleFS Filesystem Uploader plugin:
    1.  Go to the [releases page](https://github.com/esp8266/arduino-esp8266fs-plugin/releases) of the ESP8266FS plugin.
    2.  Download the latest `ESP8266FS-X.X.X.zip` file.
    3.  In your Arduino IDE, go to the `tools` directory (create it if it doesn't exist).
    4.  Unzip the downloaded file into the `tools` directory.
    5.  Restart the Arduino IDE.
-   In the Arduino IDE, you should now see an "ESP8266 LittleFS Data Upload" option in the **Tools** menu.
-   Click on **Tools > ESP8266 LittleFS Data Upload** to upload the files from the `data` directory to the ESP8266.

## Configuration

The main configuration is done at the top of the `OttoNinja.ino` sketch.

### WiFi Mode

You can choose between two WiFi modes: Station (STA) mode and Access Point (AP) mode.

-   **`WIFI_STA` (Station Mode):** The ESP8266 connects to your existing WiFi network. This is the recommended mode.
-   **`WIFI_AP` (Access Point Mode):** The ESP8266 creates its own WiFi network that you can connect to.

To select the mode, change the `CONNECT_MODE` definition:

```cpp
#define CONNECT_MODE 2 // 1 for station ... 2 for access point
```

### WiFi Settings

-   **`ssid`:** Your WiFi network name (for STA mode) or the name of the network the ESP8266 will create (for AP mode).
-   **`password`:** Your WiFi password (for STA mode) or the password for the network the ESP8266 will create (for AP mode).
-   **`esp_hostname`:** The hostname for the device. By default, it is "ottoninja", and you can access the web interface at `http://ottoninja.local`.
-   **`ap_ip`, `ap_gateway`, `ap_subnet`:** Network settings for AP mode.

```cpp
const char* ssid = "your_ssid"; // write your WiFi name
const char* password = "your_password"; // write your WiFi password
const char* esp_hostname = "ottoninja"; // Desired hostname

// AP mode settings
IPAddress ap_ip(192,168,1,1);     
IPAddress ap_gateway(192,168,1,1);   
IPAddress ap_subnet(255,255,255,0); 
```

## Usage

1.  **Upload the code:** Open `OttoNinja.ino` in the Arduino IDE, select the correct board and port, and click "Upload".
2.  **Upload the filesystem:** Click **Tools > ESP8266 LittleFS Data Upload**.
3.  **Connect:**
    -   **STA Mode:** The ESP8266 will connect to your WiFi network. Open the Serial Monitor to see the IP address assigned to the robot.
    -   **AP Mode:** Connect your computer or phone to the WiFi network created by the ESP8266 (e.g., "ottoninja").
4.  **Control:** Open a web browser and navigate to the robot's IP address or `http://ottoninja.local`. You should see the web interface and be able to control your OttoNinja.
