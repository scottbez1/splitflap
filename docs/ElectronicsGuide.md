# Chainlink Driver Electronics User Guide
[<< Back to Documentation Index](DocumentationIndex.md)

# Intro

If you’re just getting started and want to learn more about the Chainlink Driver, jump to the [bottom of this document](#what-is-the-chainlink-driver) which includes some more background and general info.

Please let me know if you have any questions or suggestions on how to improve these instructions - feel free to open a pull request or joing the [community Discord server](https://discord.gg/wgehm3PcrC)

Building a split-flap display? I would love to hear from you and see photos/videos of your project in progress or completed! This project has been a hobby of mine for a few years and the coolest part by far is hearing from other people making their own and seeing them in action. Feel free to shoot me an email at scott@bezeklabs.com 🙂 or, [join the Discord server](https://discord.gg/wgehm3PcrC), to share and discuss with the wider split-flap community!


# 1. Gather the parts
- Chainlink Driver PCB assembly (see [appendix](#appendix) for ordering info)
- Connectors (all included with the Chainlink Driver boards I sell on Etsy)
    - 6x JST XH 5-pin
    - 6x 3-pin male headers
    - 2x 8-pin 2-row shrouded IDC connectors
- Wires
    - 8-pin 1.27mm ribbon cable, and IDC connectors for each end (included with the Chainlink Driver boards I sell on Etsy)
    - 3.3v power wire
        - e.g. 22AWG solid hookup wire
    - 12v power/ground wires
        - 20AWG up to 14AWG depending on number of modules
- 12V Power supply (see [appendix](#appendix) for more info)
- ESP32
![Chainlink Buddy [T-Display]](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637360060741_DSC_5014_s.jpg)

    - The [TTGO T-Display](https://amzn.to/3kHwhMm) [affiliate links help support this project at no cost to you, or use a [non-affiliate link](https://www.amazon.com/LILYGO-T-Display-Arduino-Development-CH9102F/dp/B099MPFJ9M) if you’d prefer] is the recommended ESP32 board, which includes a 240x135 LCD that the splitflap firmware supports out of the box. (Also available on [aliexpress](https://www.aliexpress.com/item/33048962331.html), which may be cheaper)
    - An optional [Chainlink Buddy [T-Display]](https://bezeklabs.etsy.com/listing/1109357786/splitflap-chainlink-buddy-t-display) is also highly recommended for most users, which makes it easy and clean to connect a T-Display to the Chainlink Driver.
    - If you want to use an ESP32 module other than a T-Display, a [Chainlink Buddy [Breadboard]](https://bezeklabs.etsy.com/listing/1123863267/splitflap-chainlink-buddy-breadboard) may be helpful in lieu of the T-Display Buddy. See the [appendix](#appendix) for more info on using other ESP32 modules.



# 2. Finish assembling the PCB
- Insert and solder the connectors as shown. Note that both of the IDC connectors on the ends face the same way - notches facing left.
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637363719721_chainlinkDrawing.png)

- Insert the ribbon cable into a connector as shown, and *gently* clamp shut with a vise or pliers. Repeat with the other connector on the opposite end, making sure that both connectors face the **same** way - notches facing left.
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637363943974_ChainlinkCableIDCAssembly.png)

- Not recommended, but you can optionally install the strain relief on the connectors (included with Bezek Labs Chainlink Driver boards): fold the ribbon cable back over the connector and install the plastic piece to clamp it down until it clicks. You’ll sacrifice some cable length and end up with a bulkier/taller connector, so I don’t think it’s worthwhile, especially since your cables shouldn’t be moving around much anyway.


# 3. Assemble the Chainlink Buddy
- Solder the connectors into place (make sure the IDC connector notch faces the left, as indicated on the PCB)


## T-Display Buddy
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637364807000_DSC_5003_s.jpg)
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637364821133_DSC_5014_s.jpg)


On the T-Display itself, you will solder the 2 rows of male headers (included in the box) to the T-Display, and then the whole module will plug into the matching female headers you soldered on the Buddy board.

You may notice a few empty spaces in the lower left of the T-Display Buddy PCB labeled C1 and C2 along with 3 through-holes; don’t worry, those are optional components and not needed for a basic setup. See [Standalone T-Display operation](#8-optional-standalone-t-display-operation) for more info.


![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1647556549613_PXL_20220317_223022886.jpg)


Note: the T-Display often comes with a small Red/Black wiring harness with connector as well; it is unused with the Chainlink Buddy and can be discarded.






## Breadboard Buddy (alternative to T-Display Buddy)

If you’d prefer to use a different ESP32 module, you can use the Breadboard Buddy PCB to easily adapt the IDC cable to a breadboard.

![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637364835081_DSC_5006.jpg)
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637364842028_DSC_5009_s.jpg)



# 4. Connect Chainlink Driver to Chainlink Buddy

We’ll start by connecting and testing just the Chainlink Driver, without connecting a power supply or any modules’ motors or sensors. We’ll add those later, after confirming that the basic electronics work.


## T-Display
- Connect the ribbon cable from the Chainlink Buddy’s **Output** to the Chainlink Driver’s **Input**
- Connect a wire from the Chainlink Driver’s “Logic 3.3-5V” screw terminal to the “3.3V” screw terminal on the Chainlink Buddy
- Connect a wire from the Chainlink Driver’s “GND” screw terminal to the “GND” screw terminal on the Chainlink Buddy
- Connect a wire from the Chainlink Driver’s “Motor 5-12v” screw terminal to the “12V” screw terminal on the Chainlink Buddy
- ⚠️  Never connect a battery to the T-Display’s battery connector (small white connector on the bottom of the TTGO T-Display module) when using the Chainlink Buddy! 
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637365064613_PXL_20211118_023418663.jpg)

## Breadboard (if you’re not using the T-Display Buddy)
- Connect the ribbon cable from the Chainlink Buddy’s **Output** to the Chainlink Driver’s **Input**
- Connect a wire from the Chainlink Driver’s “Logic 3.3-5V” screw terminal to a 3.3v supply on your breadboard
- Connect a wire from the Chainlink Driver’s “GND” screw terminal to your breadboard’s ground rail
- Connect the 5 pins to your ESP32. See the [appendix](#appendix) for more info. Default pin assignments are:
    - Clock: 33
    - Motor Data: 32
    - Sensor Data: 39
    - Latch: 25
    - GND: ground
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637364110536_DSC_5010.jpg)



# 5. Install firmware

**Download and install** [**Microsoft Visual Studio Code**](https://code.visualstudio.com/)

**Install the Platform IO extension**

**Clone the repository**
Choose “Clone Git Respository” from the VS Code welcome page and then paste the git repo URL: `https://github.com/scottbez1/splitflap.git` 


![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1728232383342_Screenshot+from+2024-10-06+09-29-20.png)
![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1728232480075_image.png)


When prompted, choose to open the cloned repository.

You may need to choose to “Trust” the folder to allow the Platform IO extension to run.

![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1728245389499_image.png)


You’ll need to configure NUM_MODULES in platformio.ini to match the number of modules supported by the Chainlink Drivers you have connected. So for a single Chainlink Driver, set it to 6.


    [env:chainlink]
    extends=esp32base
    build_flags =
        ${esp32base.build_flags}
        -DCHAINLINK
        -DNUM_MODULES=6


- (If you’re *no*t **using a T-Display, see the [appendix](#appendix) for a few more changes you’ll need to make at this point)
- Mac Users: you will likely need to install updated drivers for the CH9102 USB-serial adapter used on newer versions of the ESP32 T-Display: https://learn.adafruit.com/how-to-install-drivers-for-wch-usb-to-serial-chips-ch9102f-ch9102/mac-driver-installation


To upload code to the ESP32 T-Display from VS Code, you’ll need to (see screenshot below):

> 1) Open the Platform IO sidebar (click the alien icon on the left)
> 2) Click the “env” button at the bottom of the window and then in the dropdown at the top of the screen…
> 3) …select “env:chainlink” as the environment - this will set up code completion and syntax highlighting for the ESP32
> 4) in the  Platform IO sidebar, expand the “General” section, and click `Upload and Monitor` 
![](https://paper-attachments.dropboxusercontent.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1693886806926_image.png)

- If you reset the ESP32, you should see the LEDs on the Chainlink Drivers blink quickly and a message in the serial monitor that loopbacks are ok. If you see a loopback error message, troubleshoot that before continuing.

*NOTE*: Make sure to exit the terminal window before starting calibration.  Otherwise, you might get an error as both should not be running at the same time.


# 6. Connect modules and power
## Motors
- Plug into the white connectors. Note that module connections are laid out from *right-to-left* since the Chainlink Driver PCB is intended to sit *behind* the modules, so the first module plugs into position “A” on the **right** side of the PCB
## Sensors
![](https://paper-attachments.dropbox.com/s_0012998176B94D187A680336F96A7737F168EF4FF12A85A15DE924A7E7D3E44B_1627605534995_Screenshot+from+2021-07-29+17-38-43.png)

- Plug sensor cable into corresponding 3-pin header
- Ground (black) should be on the left (toward the “Output” side), labeled “-”
- Signal (usually white) should be on the right (toward the “Input” side), labeled “S”
## 




## Power

The ESP32 and 3.3v electronics on the Chainlink Drivers are typically powered by the USB connection to a computer. 12V power for the motors is supplied via a separate power supply.


> Note: avoid leaving 12V power on when the ESP32 is not running! Doing so could damage your motors or worse, as the motor coils could be "stuck on" if the ESP32 is not running.

**T-Display**

- You can plug a DC 12v barrel jack into the T-Display Buddy when using 1 or 2 Chainlink Drivers
- ⚠️ For more than 2 Chainlink Drivers, the barrel jack cannot handle enough current, so you should instead connect your power supply directly to the Motor Power terminals of the first Chainlink Driver. Make sure you are using the [appropriate wire gauge](#power-supply-tips), and [never chain power to more than 6 Chainlink Drivers](#large-displays)

[Wiring diagram](img/chainlinkWiringv1.1.pdf)


**Breadboard**

- Connect your power supply directly to the Motor Power terminals of the first Chainlink Driver. Make sure you are using the [appropriate wire gauge](#power-supply-tips), and [never chain power to more than 6 Chainlink Drivers](#large-displays)


[Wiring diagram](img/chainlinkWiringv1.1.pdf)



# 7. (Optional) Adding more Drivers

You can easily chain several Chainlink Drivers by connecting ribbon cables, and adding additional power wires using the second set of power screw terminals (there are 2 terminals each for the 3.3v, ground, and 12v power), but make sure to follow this rule:


    ⚠️ Never chain power to more than 6 Chainlink Drivers in a row ⚠️ 

The screw terminals have a limited current rating (10A on the ones sold by Bezek Labs), so **using more than 6 Chainlink Drivers will require power to be wired differently**. See [+Chainlink Driver v1.1 Electronics User Guide: Large-displays](#large-displays) below for more info on wiring large displays.


# 8. (Optional) Standalone T-Display operation

Under the normal configuration, you would power the motors with a 12V supply and separately the T-Display is powered via its 5V USB connection to a computer.

However, it’s possible to omit the USB connection for a completely standalone setup, powered by the 12V motor supply. This works well when you’re using the Wifi capabilities of the T-Display, for example to poll an HTTP(s) API or listen to an MQTT topic.

To do this, you’ll need to add 3 components to the Chainlink Buddy [T-Display] (must be purchased separately):

- 5V switching regulator
    - [Mornsun K7805-500R3](https://www.digikey.com/en/products/detail/mornsun-america-llc/K7805-500R3/13168189) is one recommended option, but any **switching** regulator module in a 7805 form-factor rated for 500mA+ should work - just double-check the capacitor requirements from the datasheet for the input (C1) and output (C2). A classic 7805 linear regulator is NOT recommended.
- 22uF 1206 ceramic capacitor - 16V - install to C2
    - e.g. [CL31A226MOCLNNC](http://)
- 10uF 1206 ceramic capacitor - 50V - install to C1
    - e.g. [CL31A106MBHNNNE](https://www.digikey.com/en/products/detail/samsung-electro-mechanics/CL31A106MBHNNNE/5961220)
    - or just get 2 22uF capacitors and use for both C1 and C2, but make sure to choose one rated for 25V+, e.g. [CL31A226KAHNNNE](https://www.digikey.com/en/products/detail/samsung-electro-mechanics/CL31A226KAHNNNE/3888705)
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1639184673383_2021-12-10+16.26.37.jpg)
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1639184679625_2021-12-10+16.28.06.jpg)

----------
# Appendix
## What is the Chainlink Driver?
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637357775368_DSC_4991_s.jpg)


The Chainlink Driver is an updated control board for splitflap displays, building on lessons learned from the Classic Controller.

There were 2 main goals in designing it:

- support for controlling a *LOT* of splitflaps
- low cost and ease of purchasing pre-assembled boards - it’s mostly SMD instead of through-hole

Each Chainlink Driver PCBA contains all the power and signal electronics to drive the stepper motors and read the home position sensors for **up to six** modules, and can be trivially chained together to control 100+ splitflap modules using just 4 pins of an ESP32.


https://www.youtube.com/watch?v=g9EPabcxBsM&



## Where to buy a Chainlink Driver?
![](https://paper-attachments.dropbox.com/s_BBABC117AF455DD9F0525297940CD25AF9A358008ED7FF73463824486BCF5E62_1637358766420_DSC_4994_s.jpg)


The design is open source, so you can order them directly from a PCB manufacturer using the design files provided, but I also [sell them in my Bezek Labs Etsy store](https://bezeklabs.etsy.com/listing/1123280069/splitflap-chainlink-driver-v11) within the US.

Buying them from my store has the advantage of being [100% functionality tested after assembly using a custom testbed](https://twitter.com/scottbez1/status/1414986069586575360), and it helps support further development and prototyping of this project. I also include all the connectors and a ribbon cable for your convenience.

If you’d like to order them directly from a PCB manufacturer, the designs are optimized for JLCPCB’s Assembly service (not sponsored or affiliated, but they do have extremely cheap assembly as long as you stick to a limited set of components available in their part library). Note that the BOM does *not* include many of the connectors because there is a $3-per-unique-part fee, and since you’ll need to order the ribbon cable separately anyway this can keep your costs lower for small orders.


## Power supply tips

You’ll need a 12V supply with a current rating that depends on the number of modules you’ll be controlling (and even the particular motors you’re using, as not all 28byj-48 motors have the same coil resistance).

A good starting assumption is that each motor will draw 0.25A each (typical estimate for motors with 100 ohm half-winding resistance), but you should always test your motors, and include a safety margin.

So for every Chainlink Driver you use, plan for at least 1.5A capacity, but many cheap power supplies have questionable/overstated ratings, so you probably want 2A minimum.

For a large display, I’ve used a [Mean Well LRS-350-12](https://amzn.to/3cdjuwD) [affiliate links help support this project at no cost to you, or use a [non-affiliate link](https://www.amazon.com/MEAN-WELL-LRS-350-12-Computer-Project/dp/B07VTLJS18) if you’d prefer] which I found to easily supply 22+ amps with little voltage ripple.

Make sure to select appropriate gauge copper wire for power (this includes the GND wires!). The current along a chain of Chainlink Driver boards adds up quickly!

I also recommend adding fuses near the power supply as an added layer of protection, especially on larger displays. The LRS-350-12 mentioned above is capable of supplying 29A *continuously* which is a lot of current and potentially destructive in case of a fault. Since each power branch (see advanced wiring diagram) will use less than this, you should protect each branch with its own appropriately-sized fuse.

Make sure to buy power wiring from a reputable seller with a trustworthy supply chain (e.g. an electronics supplier or a big box hardware store, not AliExpress or Amazon). 
Unfortunately, as the cost of copper has gone up it’s [more common to find wire using other metals](https://www.youtube.com/watch?v=15sMogK3vTI) which may not meet its advertised specs.


## Using a different ESP32 module

In theory, any ESP32 module *with 2 cores* (i.e. *not* based on an ESP32-**S2**) should work. Most common WROOM or WROVER-based modules can work.

However, not all cheap modules are well designed (some are not breadboard friendly, some have poor power design causing brownouts during wifi/ble, etc), so I’ve found the T-Display to be a great default. You do lose a few IO pins to the LCD, but the Chainlink firmware supports the LCD out of the box and I think you’ll find it handy for debugging.

That said, if you do use another module here’s what you need to know:


1. You probably want to disable the display code to free up those pins and avoid wasting CPU cycles.  Do this by changing `-DENABLE_DISPLAY=1` to `-DENABLE_DISPLAY=0` in platformio.ini
2. Since the ESP32 has a GPIO matrix, you can change the pin assignments easily (see spi_io_config.h), but you do need to [avoid certain reserved functions](https://www.youtube.com/watch?v=LY-1DHTxRAk) so I recommend using the defaults unless you have a good reason not to.


## Large displays

The Chainlink system can support large displays, but please be aware that more care needs to be taken as the size of the display (and therefore, the power supply and electronics involved) increases. 

If you are wiring more than 6 Chainlink Driver boards, **only data connections should form a continuous chain**! This is shown in blue on the diagram below.

The power should branch into **separate chains of up to 6 Drivers** (or fewer if your motors require more current).

Chaining power continuously like the data would overload the screw terminals and wires at the beginning of the chain, potentially causing damage/fire. Note how the yellow and black wires in the diagram below branch *at the power supply* and do not continue past each sub-chain of 6 Drivers.

[Advanced wiring diagram](img/chainlinkWiringAdvanced.pdf)

I would also strongly recommend adding additional safety measures with a display of this size, considering the total power involved. For instance:

- Fuses for each branch of power
- Real-time voltage & current monitoring, and individual load switches for each “branch” to detect and respond to faults
- Relay controlling mains input to 12V power supply (intentionally redundant with 12V branch load switches)

The Chainlink Base (taking the place of the Chainlink Buddy in the system) is one possible approach to some of these measures, but it is not officially supported/recommended.

Make sure you consult a qualified electrician/engineer to review all designs (including splitflap project schematics and PCBs), and be sure to follow applicable regulatory codes.




----------

Link disclosure: As an Amazon Associate I earn from qualifying purchases.
(I’ve also chosen to include non-affiliate links next to every affiliate link should you prefer to avoid using affiliate links)

