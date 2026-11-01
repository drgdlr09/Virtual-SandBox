# Virtual SandBox - A Brief Overview
For Half-Life Week 4, I designed Virtual Sandbox, a pocket-sized desk toy where every pixel on a 0.96" OLED displays acts as a grain of sand. When you tilt or move the device, the sand pours towards whichever side is the lowest, just as real sand would. I wanted to make something simple and satisfying to mess around with, and this was a great way to learn about accelerometers and batteries for the first time. A button on the device allows you change the amount of sand that is displayed, and a toggle switch allows you to turn the device on and off.

<img width="520" height="741" alt="Screenshot 2026-11-01 at 1 55 04 PM" src="https://github.com/user-attachments/assets/a72e536a-3458-4571-8291-221f445267a5" />
<img width="590" height="739" alt="Screenshot 2026-11-01 at 1 55 24 PM" src="https://github.com/user-attachments/assets/46ef4bea-5a0b-4dbe-a82a-8f841d6fadf8" />

# How it Works
Virtual SandBox uses a XIAO ESP32-S3 microcontroller, a 0.96" 128*64 OLED display to display the sand, a LIS3DH accelerometer to track tilt and motion, a push button to change amount of sand, a sliding toggle switch to turn the device on/off, and a 3.7V LiPo battery to power the device.

Virtual SandBox is built around the XIAO ESP32-S3, which was a great fit for this project because it has plenty of memory to simulate thousands of grains of sand at once. It also has battery solder pads which allowed me to make the device portable.

The OLED screen and accelerometer both use I2C, so they share the D4 pin for SDA and D5 pin for SCL.

The button is wired between D0 and ground. Every time you press it, the amount of sand on the screen changes. It cycles between 250, 500, 1000, 2000, and 3000 grains, giving you lots of flexibility to decide how you want to interact with the device.

The battery is the part I’m most proud of. The XIAO’s battery pads are on its underside, so I mounted it on header pins to leave a small gap underneath. The battery’s positive wire connects to a pad on the PCB labeled BAT+IN, which runs through the slide switch to another pad labeled BAT+OUT. A short wire goes from there to the BAT+ pad under the XIAO. The battery’s ground wire goes straight to the BAT- pad. This way the switch is soldered to the board instead of hanging off a wire.

*NOTE: When the slide switch is off, the battery is completely disconnected from the XIAO, so it won’t charge over USB-C. The switch has to be on while charging.

<img width="500" height="405" alt="Screenshot 2026-11-01 at 1 54 15 PM" src="https://github.com/user-attachments/assets/69044931-3df0-4968-95c2-d6fab050228f" />

# The PCB
The PCB is 32x52mm with rounded corners so it's comfortable to hold. It's designed like a stack, with the screen and button on the front and the battery, toggle switch, accelerometer, and ESP32 on the back to give it a clean look. The button is placed in a comfortable position, where your thumb would go if you're holding the device.

<img width="244" height="398" alt="Screenshot 2026-11-01 at 1 54 42 PM" src="https://github.com/user-attachments/assets/28a52d10-5b22-4060-bc44-be04d816ed49" />

# Firmware
The firmware is written in Arduino and uses Adafruit’s PixelDust library for the sand physics. About 45 times a second, it reads the tilt from the accelerometer, moves every grain of sand in that direction, and draws each grain as one pixel on the screen. PixelDust also makes sure grains can’t overlap, so they pile up like real sand instead of passing through each other.

# Bill of Materials
| Part | Quantity | Unit Price | Total | Vendor |
|---|---|---|---|---|
| XIAO ESP32-S3 | 1 | 6.84 | 6.84 | AliExpress |
| 0.96" OLED Display | 1 | 2.48 | 2.48 | AliExpress |
| LIS3DH Accelerometer | 1 | 1.09 | 1.09 | AliExpress |
| 3.7V LiPo Battery | 1 | 6.02 | 6.02 | AliExpress |
| SS12D00 Slide Switches | 1 | 0.93 | 0.93 | AliExpress |
| Button Set | 1 | 1.09 | 1.09 | AliExpress |
| PCB | 5 | 0.12 | 0.60 | JLCPCB |

**TOTAL PRICE (INCLUDING $11.46 SHIPPING/TAX): $30.51**
