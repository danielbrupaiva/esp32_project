# ESP-IDF PROJECT

[Link] https://docs.espressif.com/projects/esp-idf/en/v4.4.6/esp32/index.html

## Useful commnads

$ get_idf

$ idf.py set-target esp32

$ idf.py clean

$ idf.py build

$ idf.py flash

$ idf.py monitor

$ idf.py -p /dev/ttyUSB0 build flash monitor

## TODO

- Task sync.
- InterProcess Comunication ("ping-pong")

- Study NVS usage
- Implement Watchdog timer -> task monitor (queue)
- Implement MQTT
- Implement WebServer

- Implement I2C connection
- Implement UART connection

- Convert to Zephyr RTOS ?
