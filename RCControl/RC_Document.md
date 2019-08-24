# RC Modules Documentation

**Hardware Setup for RC Receiver:**
Required hardware: 
1. Sparkfun SAMD21 Mini Breakout
2. Spektrum AR610X Receiver
3. Power supply
4. USB cable

| SPEKTRUM AR610X  | SPARKFUN SAMD21 MINI BREAKOUT  |
| ---------------- | :----------------------------: |
| AUX1             | 7                              |
| GEAR             | 8                              |
| RUDD             | 0                              |
| ELEV             | 1                              |
| AILE             | 6                              |
| THRO             | 9                              |
| BND/BRA          | NA (Use for Binding only)      |


The RC code utilizes the `attachInterrupt()` from Arduino to detect different in frequency that the board is receiving while the controller values change. This is almost similar to an `async` which will optimize the code so that we won't have to constantly check for data every iteration in the `loop()`.

Whenever the RC receiver gets a signal, the value will go high and activate the `attachInterrupt()` to keep track when the value return back to low. Once it completed this step, the time period where the change is recordeded will be the value of each switches on the Receiver channels.

# Note for future implementations:
1. The CAN BUS Protocol is written with the RC code; however, the CAN BUS has yet been tested at this moment. Recommend finding a way to test current CAN BUS connection for the RC code.
2. There is a __Note__  section in the main `Loop()` of the RCControlRx.ino. This Note is there for the future implementation where the CAN BUS passed the data to other Low/High level board and received the ACK message back. Without setting the `receiverData.newData = false;` the RC code might not be able to receive new data coming from the RC controller. 
3. 