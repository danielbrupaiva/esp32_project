get_idf
idf.py set-target esp32
idf.py clean
idf.py build
idf.py flash
idf.py monitor

 idf.py -p /dev/ttyUSB0 build flash monitor