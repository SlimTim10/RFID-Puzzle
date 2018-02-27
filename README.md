# RFID Puzzle

## Description

This project consists of two major components: the puzzle and an endpoint. The puzzle uses RFID readers hidden underneath a surface, where objects with RFID tags must be placed directly atop. The puzzle is solved when the correct objects are placed in the correct locations. At this point, a signal is transmitted to the endpoint via RF. The endpoint is used to indicate that the puzzle has been solved, from a distance. For example, a locked door opens or a key falls down from a hidden spot out of reach. The physical separation of the puzzle and endpoint is to create a sense of magic and wonder in solving the puzzle.

## Dependencies

- [RadioHead](https://github.com/PaulStoffregen/RadioHead) RF library
- [PN532](https://github.com/Seeed-Studio/PN532) NFC library

## Hardware

### Puzzle

- [Arduino Uno](http://arduino.cc/en/Main/arduinoBoardUno)
- [RF transmitter](https://www.digikey.ca/product-detail/en/seeed-technology-co-ltd/113990017/1597-1224-ND/5488259)
- [NFC shield](https://www.digikey.ca/product-detail/en/seeed-technology-co-ltd/113030001/1597-1219-ND/5488133)
- RFID tags

### Endpoint

- [Arduino Uno](http://arduino.cc/en/Main/arduinoBoardUno)
- [RF transmitter](https://www.digikey.ca/product-detail/en/seeed-technology-co-ltd/113990017/1597-1224-ND/5488259)
- [Servo motor](https://www.digikey.ca/product-detail/en/dfrobot/SER0011/1738-1232-ND/7087129)