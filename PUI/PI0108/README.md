# Philips Dynalite Compatible User Interface 

User interface (Wall Switch Panel) is Philips Dynalite compatible device supporting Dynet1 protocol.
The device is based on Arduino Nano board.
The latest files can be found https://github.com/rightoneX/DynetIntegration

## Function 

>Button Function Declaration
- 0 - disable  / Button will be disabled, no functions
- 1 - push / Button sends one command only
- 2 - toggle / Toggles between two commands, On/Off
- 3 - push with release / When buttons pushed - one command, onc the button is released - second command
- 4 - toggle with release / As above with four commands
- 5 - run task / Runs a sequence on button activation

### Configuration

>Button Quantity
- The button quantity can be setup by adjusting [buttonQuantity] value from 1 to 8 

>Setting up the IO pins for buttons 
- The button pin connection can be set via buttonPin[buttonQuantity]

