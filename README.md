# paperdash retrofit weather station

## compile and upload new firmware
```properties
platformio run --target upload
```

## app development
```properties
# build new dist files
yarn --cwd app build

# upload to device filesystem
platformio run --target uploadfs

# erase flash memory
pio run --target erase
```

## access fs over http

> get sensor json db
http://192.168.178.65/fs/sensors.json
