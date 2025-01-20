# Splitflap v2 Assembly Guide
[<< Back to Documentation Index](../DocumentationIndex.md)

# Electronics Part 1: Sensor PCBs

Start by removing the rails on the outside of the panel following the printed instructions, then snap apart the individual sensor boards.

![Snap off the rails](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719290808619_Untitled_1.1.1.jpg)
![Break apart the sensors](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719290808826_Untitled_1.1.2.jpg)


You’ll probably want to use some pliers to clean up the spacer segments that were between the sensor boards.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719290808879_Untitled_1.1.3.jpg)
![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719290808902_Untitled_1.1.4.jpg)


Then insert the right-angle headers from the side with the pins labeled (and where “MOTOR” is visible). The pin headers should extend down past the bottom of the PCB.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719290808980_Untitled_1.2.1.jpg)


Flip the PCB and solder the headers in place.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719290809216_Untitled_1.3.1.jpg)
![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719290808726_Untitled_1.4.1.jpg)




# Electronics Part 2: Chainlink Driver & Chainlink Buddy [T-Display]
## Assemble the Chainlink Driver PCB

Insert and solder the connectors as shown. Note that both of the IDC connectors on the ends face the same way - notches facing left.

![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637363719721_chainlinkDrawing.png)


Insert the ribbon cable into a connector as shown, and *gently* clamp shut with a vise or pliers. Repeat with the other connector on the opposite end, making sure that both connectors face the **same** way - notches facing left.

![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637363943974_ChainlinkCableIDCAssembly.png)


(Not recommended, but you can optionally install the strain relief on the connectors: fold the ribbon cable back over the connector and install the plastic piece to clamp it down until it clicks. You’ll sacrifice some cable length and end up with a bulkier/taller connector, so I don’t think it’s worthwhile, especially since your cables shouldn’t be moving around much anyway.)


## Assemble the Chainlink Buddy

Solder the connectors into place (make sure the IDC connector notch faces the left, as indicated on the PCB)

**T-Display Buddy**

![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637364807000_DSC_5003_s.jpg)
![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637364821133_DSC_5014_s.jpg)


On the T-Display itself, you will solder the 2 rows of male headers (included in the box) to the T-Display, and then the whole module will plug into the matching female headers you soldered on the Buddy board.

You may notice a few empty spaces in the lower left of the T-Display Buddy PCB labeled C1 and C2 along with 3 through-holes; don’t worry, those are optional components for an advanced build and are not needed for a basic setup. 
See [Standalone T-Display operation](../ElectronicsGuide.md#8-optional-standalone-t-display-operation) for more info.


![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1647556549613_PXL_20220317_223022886.jpg)


Note: the T-Display often comes with a small Red/Black wiring harness with connector as well; it cannot be used alongside the Chainlink Buddy and should be discarded.





**Breadboard Buddy (alternative to T-Display Buddy)**
If you’d prefer to use a different ESP32 module, you can use the Breadboard Buddy PCB to easily adapt the IDC cable to a breadboard.

![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637364835081_DSC_5006.jpg)
![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637364842028_DSC_5009_s.jpg)



## Connect Chainlink Driver to Chainlink Buddy

We’ll start by connecting and testing just the Chainlink Driver, without connecting a power supply or any modules’ motors or sensors. We’ll add those later, after confirming that the basic electronics work.

**T-Display**

- Connect the ribbon cable from the Chainlink Buddy’s **Output** to the Chainlink Driver’s **Input**
- Connect a wire from the Chainlink Driver’s “Logic 3.3-5V” screw terminal to the “3.3V” screw terminal on the Chainlink Buddy
- Connect a wire from the Chainlink Driver’s “GND” screw terminal to the “GND” screw terminal on the Chainlink Buddy
- Connect a wire from the Chainlink Driver’s “Motor 5-12v” screw terminal to the “12V” screw terminal on the Chainlink Buddy
- ⚠️  Never connect a battery to the T-Display’s battery connector (small white connector on the bottom of the TTGO T-Display module) when using the Chainlink Buddy! 
![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637365064613_PXL_20211118_023418663.jpg)


**Breadboard (if you’re not using the T-Display Buddy)**
If you are using a different ESP32 module with the Breadboard Buddy rather than the T-Display Buddy, see the instructions in the full [Chainlink User Guide](../ElectronicsGuide.md).


# Mechanical: Laser-cut Assembly
![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719290809012_Untitled_1.5.1.jpg)


⚠️⚠️⚠️
If you used acrylic parts, DO NOT FORCE THEM together if they don’t fit immediately. The acrylic WILL shatter! Use a small jeweler’s file to shave down the parts as necessary. 
⚠️⚠️⚠️


## Flap drum

Take an M4x10 bolt and nut and loosely assemble them on the spacer piece. Make sure the bolt can still spin somewhat freely. Set this aside.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292210757_Untitled_1.6.1.jpg)


Hold the 4 struts to form a box, with the stepped tab ends facing up…

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292210784_Untitled_1.7.1.jpg)


… and then place the spacer on top with bolt facing up/out.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292210866_Untitled_1.7.2.jpg)


Spin the bolt/nut combo to so the nut’s edges are parallel to 2 of the strut tabs, and then place the wheel with hexagonal center hole onto the end.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292211039_Untitled_1.7.4.jpg)


Then attach the other wheel to the other end’s tabs. You may need to hold the whole assembly together at this point if the laser-cut parts are a bit loose.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292210924_Untitled_1.7.5.jpg)


I recommend using CA glue (with accelerator!) to permanently assemble the flap drum. My personal go-to glue is Bob Smith Industries thick CA glue ([paid affiliate link](https://amzn.to/3REVhEM), or use this [non-affiliate link](https://a.co/d/05jtD1j4)). Note: make sure you’ve installed the M4 nut+bolt earlier, as you won’t be able to install it after gluing!

Apply a small dot of CA glue to each joint between the center struts and the wheel ends (8 dots in total). You will have 30 seconds to a minute of working time at this step.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292210572_Untitled_1.8.1.jpg)
![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292210652_Untitled_1.10.1.jpg)


If the assembly is somewhat loose, **make sure the wheel ends are not twisted**, as this will cause flaps to hang at an angle! See photo below for why this is problematic:

![❌ AVOID TWISTING THE DRUM WHEN GLUING IT! ❌  Although you’re not installing flaps quite yet, a twisted drum will eventually look like this — flaps hang at an angle and may bind up against the sides of the front window!!!](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292210724_Untitled_1.11.1.jpg)


When the drum is fully assembled and you’ve ensured it’s not twisted, spray CA accelerator to quickly lock the glue in place. If the parts are particularly loose, you may want to hold the assembly together with gloved hands while you spray CA accelerator.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292211000_Untitled_1.8.2.jpg)


💡 **Important CA glue tip:** If you don’t use accelerator, you’ll likely end up with a white haze or residue all over the acrylic parts — this is caused by small amounts of CA glue vaporizing and curing with the moisture in the air, which then gets deposited onto your parts (and surrounding table, computer, etc!) to make a permanent white-ish haze. I’ve never had issues with CA haze/fogging when using Bob Smith Industries glue with the included accelerator spray, which is why I recommend it! ([paid affiliate link](https://amzn.to/3REVhEM), or use this [non-affiliate link](https://a.co/d/05jtD1j4))


After waiting a few minutes for the accelerated CA glue to cure, use an allen key to tighten the internal M4 bolt and lock it in place.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719292210679_Untitled_1.9.1.jpg)



## Motor + Sensor + Left enclosure

Take the wider of the side enclosure pieces (with a rounded rectangular hole cut out of the middle) and insert the motor’s cable through the hole in the orientation shown below.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719372206689_Untitled_1.12.1.jpg)
![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719372221515_Untitled_1.12.2.jpg)


Then place the sensor PCB over the motor, and insert 2 M4x10 bolts into the motor mount holes.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719372564839_Untitled_1.12.3.jpg)
![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719372662839_Untitled_1.12.4.jpg)


Flip the assembly over and start threading a nut onto each bolt, then use an allen key to finish tightening the nuts.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719372564684_Untitled_1.12.4.jpg)
![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719372564756_Untitled_1.12.5.jpg)

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719372713065_Untitled_1.12.7.jpg)

## Magnet

Next we’ll install the magnet into the flap drum. The orientation of the magnet is critical, as the sensor only works with one pole of the magnet and not the other!

Luckily we can use the LED on the sensor PCB to help determine the right way to install the magnet.

Plug the sensor into your Chainlink Driver, and connect the Chainlink Driver to your ESP32/Chainlink Buddy, ensuring you’ve connected the 3.3v logic power screw terminals. 

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719373078065_Untitled_1.13.1.jpg)


When you hold up the magnet, the LED should light for one side of the magnet and not the other.

![The LED on the sensor PCB will light when the magnet is in the correct orientation](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719373078137_Untitled_1.14.2.jpg)
![This is the wrong orientation - the LED is not lit](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719373077998_Untitled_1.14.1.jpg)


Install the magnet into the flap drum in the orientation that caused the LED to light.

I’ve found that attaching the magnet to one jaw of a set of pliers and then **gently** pressing it into the flap drum works well — DO NOT FORCE IT if it requires more than a tiny bit of pressure, as you will crack the acylic; use a file to widen the hole and then try again.

![Magnet stuck to pliers in correct orientation](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719373459435_Untitled_1.15.1.jpg)
![Gently pressing until flush](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719373459488_Untitled_1.15.2.jpg)

![Installed magnet](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719373459372_Untitled_1.15.3.jpg)


Double-check you got the magnet orientation correct when installing the flap drum onto the motor. Again, it should gently press-fit onto the motor shaft; use a file to widen the hole first if it requires too much force to press into place.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719373664063_Untitled_1.16.1.jpg)


If the magnet is a little loose, you can add a drop of CA glue on the back side (from the inside of the drum) to lock it in place.


## Top/Bottom/Right Enclosure Pieces

The top and bottom pieces are interchangeable with each other. Find the matching tabs and slots of the top piece and the left side (with the motor)…

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719380017490_Untitled_1.17.1.jpg)


… insert a nut into the notch in the top piece (I like to support it from behind with a finger as shown below) and screw in an M4x10 bolt to connect the pieces.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719380017547_Untitled_1.17.2.jpg)


Repeat that process with the bottom piece, and then attach the right side piece as well.


![Place the backstop bolt in the center of its slot to start](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719380017394_Untitled_1.17.3.jpg)


Before installing the front face, insert the M4x10 flap backstop bolt in the middle of the slot on the right side piece and secure in place with a nut. You can fine-tune the position later if desired, but the middle is usually a good starting point.

![Flap backstop bolt sticks in towards the middle of the module](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719380017446_Untitled_1.17.4.jpg)


Now attach the front face using M4 bolts/nuts to secure it to the top and bottom pieces.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719380017573_Untitled_1.18.1.jpg)



## Installing flaps

Now you can install the flaps. The first flap you install should be blank and the bottom half of the letter “A”.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719380955841_image.png)


Install the first flap into the position shown below — 90 degrees “ahead” of the magnet on the flap drum. (this position technically isn’t critical since the home position is software-calibrated later, but it’s a nice convention to keep the modules consistent with each other)

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719380017613_Untitled_1.19.1.jpg)



![Gently bend the flap to insert it](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719380017298_Untitled_1.19.2.jpg)


To install a flap, let it rest between your index and middle fingers, and press on the middle with your thumb to bend the flap. Insert one of the flap’s pins into the flap drum, then carefully align the other pin and gently release pressure from your thumb to insert the second pin as the flap unbends.

Repeat with the next 51 flaps, making sure to confirm the ordering as you do so.

![](https://paper-attachments.dropboxusercontent.com/s_A1DD8FEE10023EEA001FB219B5E96B0C6F11F98D2D2085C80F42EAB5A5BAB282_1719380017368_Untitled_1.19.3.jpg)



# Firmware
- Download and install [Microsoft Visual Studio Code](https://code.visualstudio.com/)
- Follow these steps to set up Platform IO and the splitflap code:
    - Install the Platform IO extension
    - Choose “Clone Respository” in the Explorer sidebar and enter the git repo URL: `https://github.com/scottbez1/splitflap.git`
    - Once it's done cloning, select the option to open it
    - You may need to choose to “Trust” the folder to allow the Platform IO extension to run.

You’ll need to configure NUM_MODULES in `platformio.ini` to match the number of modules supported by the Chainlink Drivers you have connected. So for a single Chainlink Driver, set it to 6.


    [env:chainlink]
    extends=esp32base
    build_flags =
        ${esp32base.build_flags}
        -DCHAINLINK
        -DNUM_MODULES=6


- Mac Users: you will likely need to install updated drivers for the CH9102 USB-serial adapter used on newer versions of the ESP32 T-Display: https://learn.adafruit.com/how-to-install-drivers-for-wch-usb-to-serial-chips-ch9102f-ch9102/mac-driver-installation
- To upload code to the ESP32 T-Display from VS Code, you’ll need to (see screenshot below):
> 1) Open the Platform IO sidebar (click the alien icon on the left)
> 2) Click the “env” button at the bottom of the window and then in the dropdown at the top of the screen…
> 3) …select “env:chainlink” as the environment - this will set up code completion and syntax highlighting for the ESP32
> 4) in the  Platform IO sidebar, expand the “General” section, and click `Upload and Monitor` 
![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1693886806926_image.png)

- If you reset the ESP32, you should see the LEDs on the Chainlink Drivers blink quickly and a message in the serial monitor that loopbacks are ok. If you see a loopback error message, troubleshoot that before continuing.



## Connect modules and power

**Motors**

- Plug into the white connectors. Note that module connections are laid out from *right-to-left* since the Chainlink Driver PCB is intended to sit *behind* the modules, so the first module plugs into position “A” on the **right** side of the PCB

**Sensors**

![](https://paper-attachments.dropboxusercontent.com/s_0012998176B94D187A680336F96A7737F168EF4FF12A85A15DE924A7E7D3E44B_1627605534995_Screenshot+from+2021-07-29+17-38-43.png)

- Plug sensor cable into corresponding 3-pin header
- Ground (black) should be on the left, labeled “-”
- Signal (usually white or yellow) should be on the right (toward the “Input” side), labeled “S”
## 



**Power**
The ESP32 and 3.3v electronics on the Chainlink Drivers are typically powered by the USB connection to a computer. 12V power for the motors is supplied via a separate power supply.


> Note: avoid turning on 12V power when the ESP32 is not running! Doing so could damage your motors or worse, as the motor coils could be "stuck on" if the ESP32 is not running.

**T-Display**

- You can plug a DC 12v barrel jack into the T-Display Buddy when using 1 or 2 Chainlink Drivers for up to 12 modules.
- ⚠️ For more than 2 Chainlink Drivers, the barrel jack **cannot handle enough current**, so you should instead connect your power supply directly to the Motor Power terminals of the first Chainlink Driver.
Make sure you are using the [appropriate wire gauge](../ElectronicsGuide.md#power-supply-tips), and [never chain power to more than 6 Chainlink Drivers](../ElectronicsGuide.md#large-displays)
https://www.dropbox.com/s/4xpvtycwrw21g0e/chainlinkBuddyWiringv1.1.pdf?dl=0


**Breadboard**

- Connect your power supply directly to the Motor Power terminals of the first Chainlink Driver. Make sure you are using the [appropriate wire gauge](../ElectronicsGuide.md#power-supply-tips), and [never chain power to more than 6 Chainlink Drivers](../ElectronicsGuide.md#large-displays)


For further wiring info, see the full [Chainlink User Guide](../ElectronicsGuide.md).


# Initial Setup & Calibration
- Turn on your 12V supply and plug the ESP32 into your computer.
- You should see the modules start to find their home positions.
    - If none of modules move, try restarting the ESP32. If they still don’t move, turn off your 12V supply and see the [troubleshooting section](#troubleshooting)
- In Chrome or Edge (unfortunately Firefox will not work, as of 2024-10-05), go to https://scottbez1.github.io/splitflap/ and connect to your display via Web Serial
- If a module is showing as an orange square, you should first try re-homing the module by left-clicking it. You should see the module spin and find its home position. If this does not resolve the sensor error, see the [troubleshooting section](#troubleshooting)
- Right-click a module to open the calibration flow, and follow the instructions to complete calibration. Save the calibration before continuing to calibrate the other modules.
    - If saving the calibration fails, see the [troubleshooting section](#troubleshooting)



https://www.dropbox.com/scl/fi/aqilx2zk5q8ic1kwi2ki7/PXL_20240330_182017129.TS.mp4?rlkey=im0qr2vuhdlxhmnnzx7f57r09&dl=0



# Troubleshooting
- Modules don’t move at all when the ESP32 and 12V supply are turned on
    - Troubleshooting steps:
        - Run the “Monitor” PlatformIO action in VSCode and then press the reset button on the ESP32. You want to see a message like `{"type":"init", "num_modules":6}` and not any messages about loopback errors (see section below if you do see a `Loopback ERROR`)
        - Make sure your screw terminals are securely clamped onto the appropriate wires
        - Use a voltmeter to confirm that there is 11-13V measured between the Motor and GND screw terminals on each Chainlink Driver board
- Saving module calibration fails
    - Symptoms: `Failed to open config file` or `Failed to mount FFat` messages when saving calibration
    - Troubleshooting steps:
        - In VSCode, use the PlatformIO action to erase the flash on your ESP32, then re-upload the firmware and try calibration again
- Bad chainlink data signal - `Loopback ERROR`
    - Symptoms: `Loopback ERROR` log message. This means the control signals to the motors or from the sensors are not working along the chain of Chainlink Driver boards.
    - Troubleshooting steps:
        - Check all of your solder joints, especially those on the IDC connectors, to make sure they are solid and not bridged
        - Make sure the IDC ribbon cable connectors are fully clamped onto the ribbon cables
        - Check all of the screw terminals are securely clamped onto the appropriate wires
        - Make sure the NUM_MODULES variable in platformio.ini (under the `env:chainlink` section) is set to 6 times the number of Chainlink Drivers you have connected
        - Press the reset button on the ESP32 and confirm that the red LEDs on the Chainlink Drivers each flash sequentially once
        

if you have questions not covered here, please reach out in the [splitflap Discord server](#troubleshootings).



----------

Link disclosure: As an Amazon Associate I earn from qualifying purchases at no cost to you.
(I’ve also chosen to include non-affiliate links next to every affiliate link should you prefer to avoid using affiliate links)

