# Kestrel Perch

USB to UART bridge for the Raspberry Pi Pico.

## Features

6 UART ports, 2 of which are hardware UARTs and 4 are PIO-based UARTs.
8 Relays controlled via USB commands.
Cli commands for controlling relays.

## Default UART configuration

Baud rate: 115200
Data bits: 8
Parity: None
Stop bits: 1

UART GPIO pin mapping:
```bash
/dev/ttyACM0 => 2 (TX); 3 (RX) | PIO
/dev/ttyACM1 => 4 (TX); 5 (RX) | PIO
/dev/ttyACM2 => 6 (TX); 7 (RX) | PIO
/dev/ttyACM3 => 8 (TX); 9 (RX) | HW-UART
/dev/ttyACM4 => 10 (TX); 11 (RX) | PIO
/dev/ttyACM5 => 12 (TX); 13 (RX) | HW-UART
```

Relay GPIO pin mapping:
```bash
Relay 1 => 21
Relay 2 => 20
Relay 3 => 19
Relay 4 => 18
Relay 5 => 17
Relay 6 => 16
Relay 7 => 15
Relay 8 => 14
```

## Questions and Suggestions

The official documentation is available at: `docs/`. If you have any questions or suggestions, feel free to use the [ISSUES](https://github.com/JN513/Kestrel-Perch/issues) section on GitHub. Contributions are welcome, and all [Pull requests](https://github.com/JN513/Kestrel-Perch/pulls) will be reviewed and merged if possible.

## Contribution

If you'd like to contribute to the project, please feel free to do so. The [CONTRIBUTING.md](https://github.com/JN513/Kestrel-Perch/blob/main/CONTRIBUTING.md) file contains the necessary instructions.

## License

This project is licensed under the [BSD 2-Clause license](https://github.com/JN513/Kestrel-Perch/blob/main/LICENSE), which grants full freedom for use.