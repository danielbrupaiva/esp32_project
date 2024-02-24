# ESP32 WebServer application

## WebServer aplication using ESP32 for remotely control and monitoring sensors at field

## ESP-IDF v4.4

[Link] https://docs.espressif.com/projects/esp-idf/en/v4.4.6/esp32/index.html

### Useful commnads

$ get_idf

$ idf.py menuconfig

$ idf.py set-target esp32

$ idf.py clean

$ idf.py build

$ idf.py flash

$ idf.py monitor

$ idf.py -p /dev/ttyUSB0 build flash monitor

## TODO

- Implement function to read index.html file direct from "disk"
- Implement HTTPS (TLS)
- Implement mDNS
- Implement Watchdog timer -> task monitor (queue)
- Implement UART connection

## WIFI

SENAI

- Pos_Sist_Emb
- LPFS2022