# Splitflap v0 Assembly Guide
[<< Back to Documentation Index](../DocumentationIndex.md)

## Intro
Building a split-flap display? I would love to hear from you and see photos/videos of your project in progress or completed! This project has been a hobby of mine for a few years and the coolest part by far is hearing from other people making their own and seeing them in action. Feel free to shoot me an email at scott@bezeklabs.com 🙂 or, [join the Discord server](https://discord.gg/wgehm3PcrC) to share and discuss with the wider split-flap community!

# 1. Gather the parts

See the [complete ordering guide (v0)](OrderingComplete.md) or the [easy guide (v0)](OrderingEasy.md) for info on ordering parts


- 1 Controller PCB
- 4 Sensor PCBs
- 4 Magnets
- components for controller and sensor boards

&nbsp;

- 4 laser-cut panels
- 2 letter sticker packs
- 4 motors
- 4 sensor cables
- ~165 flaps (160 are necessary)
- ~46 M4 bolts (44 are necessary)
- ~46 M4 nuts (44 are necessary)

&nbsp;

- 12V 2A Power Supply
- Tools:
    - Metric hex key
    - Soldering supplies
    - Calipers or metric ruler
    - Hobby knife
    - Plastic wrap and glass storage container (For applying letter stickers)
    - [if hand-cutting flaps] Badge slot punch 
# 2. Sensor PCB Assembly
https://www.youtube.com/watch?v=vQMnWK6PwJ0&



> 💡 **New (not shown in video!):** The laser cut files include a jig for spacing the hall effect sensor accurately without having to measure the 5.8mm gap. See photos of it below.
>  
> The latest PCB design also has holes that are spaced further apart for the sensor to make soldering them easier. You’ll need to bend the sensor’s pins outward to fit into the holes.

The rough steps are:

- solder 90-degree pin headers *from the top side* ([0:21](https://www.youtube.com/watch?v=vQMnWK6PwJ0&t=21s))
- bend leads on hall effect sensor 90 degrees ([2:00](https://www.youtube.com/watch?v=vQMnWK6PwJ0&t=120s))
- insert hall effect sensor from the back side of the PCB (opposite the pin headers) ([2:45](https://www.youtube.com/watch?v=vQMnWK6PwJ0&t=165s))
- adjust hall effect sensor so there's a ~5.8mm gap between the back of the sensor and the PCB ([3:00](https://www.youtube.com/watch?v=vQMnWK6PwJ0&t=180s))
- solder the hall effect sensor in place ([3:30](https://www.youtube.com/watch?v=vQMnWK6PwJ0&t=210s))
- clip off excess pin length ([4:24](https://www.youtube.com/watch?v=vQMnWK6PwJ0&t=264s))

<a href="https://paper-attachments.dropbox.com/s_DBFFE29691B90C2EDDE2A687CA2E739A14BF58F2DE09EF37D4E0562373661460_1643174894237_123306848-a4ecbc00-d4d6-11eb-9b46-d47165f3abe7.jpg">
    <img src="https://paper-attachments.dropbox.com/s_DBFFE29691B90C2EDDE2A687CA2E739A14BF58F2DE09EF37D4E0562373661460_1643174894237_123306848-a4ecbc00-d4d6-11eb-9b46-d47165f3abe7.jpg" width="500"/>
</a>
<a href="https://paper-attachments.dropbox.com/s_DBFFE29691B90C2EDDE2A687CA2E739A14BF58F2DE09EF37D4E0562373661460_1643174893952_123306852-a6b67f80-d4d6-11eb-9466-73a11368e9a7.jpg">
    <img src="https://paper-attachments.dropbox.com/s_DBFFE29691B90C2EDDE2A687CA2E739A14BF58F2DE09EF37D4E0562373661460_1643174893952_123306852-a6b67f80-d4d6-11eb-9466-73a11368e9a7.jpg" width="500" />
</a>


> Sensor spacing jig cut from white acrylic


<a href="https://paper-attachments.dropbox.com/s_DBFFE29691B90C2EDDE2A687CA2E739A14BF58F2DE09EF37D4E0562373661460_1643174982515_123307566-7de2ba00-d4d7-11eb-8d17-976c0b2a1335.png">
    <img src="https://paper-attachments.dropbox.com/s_DBFFE29691B90C2EDDE2A687CA2E739A14BF58F2DE09EF37D4E0562373661460_1643174982515_123307566-7de2ba00-d4d7-11eb-8d17-976c0b2a1335.png" width="500" />
</a>

> If your laser cut parts came in a full sheet as-cut (e.g. Ponoko does this, but not Elecrow), you’ll find the jig inside the motor cutout, highlighted in red above


# 3. Mechanical Assembly

Note: if using acrylic rather than MDF/wood, *do not force parts together* if they don’t quite fit. Acrylic is extremely brittle and *will* crack. Use a small file if necessary to remove any excess material. While the design attempts to achieve a tight fit, different laser-cutters may remove more or less material, so undersized holes/slots can occur.

https://www.youtube.com/watch?v=zHM8W6Rm2i4&


The rough steps are:

- separate laser-cut parts ([0:12](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=12s))
- remove protective film ([0:34](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=34s))
- Spool:
    - put together spool struts ([1:31](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=91s))
    - attach bolt+nut to spool retaining piece ([2:16](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=136s))
    - insert spool struts into spool disk ([2:50](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=170s))
    - press other spool disk onto spool struts ([3:25](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=205s))
    - check magnet polarity and insert magnet into spool ([3:40](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=220s))
- pass motor through side hole and secure with 2 bolts ([5:05](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=305s))
- mount hall effect sensor module and secure with a bolt ([6:02](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=362s))
- press spool onto motor shaft ([7:15](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=435s))
- press fit bottom piece into left piece, with laser-engraved lettering facing up and circular hole toward the back ([7:50](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=470s))
- press fit top piece into left piece, with circular hole toward the back ([8:14](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=494s))
- press right piece into top/bottom pieces and aligned with spool axle ([8:34](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=514s))
- press front piece into top/bottom pieces, with thicker side on the left ([9:05](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=545s))
- secure enclosure pieces using captive nuts+bolts ([9:27](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=567s))
- attach flap backstop bolt/nut ([10:26](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=626s))
- to connect to another module, remove front face, insert inter-module connectors, and replace front face ([10:55](https://www.youtube.com/watch?v=zHM8W6Rm2i4&t=655s))
# 4. Applying Letter Stickers
https://www.youtube.com/watch?v=3lFECISLwyI&

## Additional notes (not in the video above):

By default, the splitflap code assumes the flaps appear in a particular order, so it’s easiest to apply the stickers as follows:

    (space), A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, (period), (comma), (apostrophe)

Note: for a display that will commonly count *down,* it may make sense to reverse the order of the numbers so that counting down doesn't require a full revolution each time.

You will need to make a few simple changes to the control software to tell it what order the letters appear if you use something other than the default.

Do not insert the flaps into the spool until step 6 (just keep them in order).

# 5. Electronics Assembly

See the [Electronics Guide](../ElectronicsGuide.md) for complete instructions on assembling and using the electronics.


# 6. Final Assembly and Calibration

Rough steps:

- Setup
    - Download and install [Microsoft Visual Studio Code](https://code.visualstudio.com/)
    - Follow these steps to set up Platform IO and the splitflap code ([watch](https://youtu.be/uyG5mEKzKcg) [screencast](https://www.youtube.com/watch?v=C8mcExG-VVo)):
        - Install the Platform IO extension
        - Choose “Clone Respository” in the Explorer sidebar and enter the git repo URL: `https://github.com/scottbez1/splitflap.git`
            - Note: downloading the source as a zip from github is **NOT recommended**, as it will make it harder to get help if any issues arise (git will keep track of which precise version of the code you checked out, along with any changes you make, which helps a lot when debugging)
        - Open the repository. You may need to choose to “Trust” the folder to allow the Platform IO extension to run.
        - Install the splitflap firmware on the device and open a serial monitor
            - Open the Platform IO sidebar and run the `chainlink` → `Upload and Monitor` task
- Confirm that you see the startup message that looks like this:
    {"type":"init", "num_modules":12, "character_list":" abcdefghijklmnopqrstuvwxyz0123456789.,'"}
- Plug in all motors and sensors
- Plug in 12V motor power
- Restart the microcontroller
    - Spools should rotate slowly until the home position is reached, then complete one revolution at full speed
    - Make sure spools are rotating the right direction
    - Confirm that all 4 RGB LEDs are green before moving on
- Open Serial Monitor - we’ll do some testing to make sure the motors are configured correctly
    - Send `=    \n` (equal space space space space newline)
    - Check that all motors complete a full revolution and stop
    - Repeat this process 10 more times and make sure that none of the modules have to recalibrate (rotate slowly as in the startup procedure) during this test
    - Confirm that the reported `"count_missed_home":0, "count_unexpected_home":0` values are both zero for all 4 modules
- Send `@` in Serial Monitor to force all modules to recalibrate, and `=    \n` to tell all the modules to go to the home position
- Install flaps

  ![Gently bend the flap with your thumb to add or remove it](../img/flaps/addRemoveSpool.jpg)

    - Start by taking the top and bottom halves of the empty (space) character. Insert the top half into the holes marked with a “-” on the spool. Insert the bottom half into the spool holes below it. 
    - Rotate the spool forward a few characters (e.g. by sending `=e\n` or by gently rotating the spool by hand) and insert the next few flaps. Repeat this until all 40 flaps are installed.
- Calibration
    -  Use the Serial Monitor to send characters to the modules 
    - If the module consistently doesn’t turn quite far enough or turns a little too far for some letters, then the home position sensor may need to be adjusted:
        - Loosen the hall-effect sensor bolt
        - If the module wasn’t turning far enough, slide the sensor toward the **rear** of the module
        - If the module was turning a little too far, slide the sensor toward the **front** of the module
        - Re-tighten the hall-effect sensor bolt
        - Send a `@` to recalibrate
        - Check if the calibration is better now

# Legacy Classic Controller Assembly

https://www.youtube.com/watch?v=VmKuWdmhPtM&&feature=youtu.be


Components with polarity have been labeled with a ⚠️  symbol below - make sure you get the orientation correct!

Recommended assembly order:

- 4 WS2812b RGB LEDs - U5, U6, U7, U8 - center near A B C D labels
    - ⚠️ Make sure to get polarity correct - LED notch aligns with right-angle silkscreen line - top left corner (on some PCBs, this marker may be missing or hard to see; just make sure the notch is pointing toward the top left corner)
- Arduino pin headers (BOTTOM SIDE):
    - 3 pin straight male header - “Arduino D4 D5 D6” label on bottom
    - 2x3 Female header - ISP label on bottom
    - 2 pin straight male header - VIN label on bottom
- 470 resistor (Yellow Purple Brown) - R9 (center top side, between the two MIC5842 spots)
- Top left corner:
    -  2 pin straight male header - JP1
    - **Yellow** LED - D1 (Mot+) - ⚠️ make sure to get polarity correct - match flat side to silkscreen
    - **Green** LED - D2 (5V)- ⚠️ make sure to get polarity correct - match flat side to silkscreen
    - 2.2k resistor (Red Red Red) - R1
    - 220 resistor (Red Red Brown) - R7
- Capacitors:
    - 100uF electrolytic capacitor - C1 - ⚠️ make sure to get polarity correct
    - 100uF electrolytic capacitor - C3 - ⚠️ make sure to get polarity correct
    - 0.1uF capacitor - C2 (to the left of “A” LED)
- Barrel Jack Connector - top right “5-12V” label
- Sensor connectors (place all of these at once with a sensor cable plugged in across all 3 to ensure proper alignment when soldering)
    - 4 pin straight male header - “GND” label
    - 4 pin straight male header - “5V” label
    - 4 pin straight male header - “A” and “D” labels
- 47k resistors (Yellow Purple Orange) - R3 R4 R5 R6 R10
- Motor connectors - P1 P2 P3 P4 - ⚠️ make sure to get polarity correct (match silkscreen)
- ICs - make absolutely sure to get the polarity correct!!!!
    - MIC5842 - U2 - ⚠️ make sure to get polarity correct (match silkscreen - notch facing **left**)
    - MIC5842 - U4 - ⚠️ make sure to get polarity correct (match silkscreen - notch facing **left**)
    - 74HC165 - U3 - ⚠️ make sure to get polarity correct (notch facing **bottom** - see right-angle silkscreen line near pin 1)


![Assembled Driver PCB with optional EXP OUT connector (EXP OUT is only needed when chaining controllers to support more than 4 modules, see below for more details)](https://paper-attachments.dropbox.com/s_DBFFE29691B90C2EDDE2A687CA2E739A14BF58F2DE09EF37D4E0562373661460_1572753846051_file.jpeg)


You should have the following parts left over:

- Jumper connector - this is an optional component that is generally not needed; you should leave it off unless you're certain you need it. **Never connect the Arduino's USB cable to anything if jumper JP1 is in place!** The JP1  to power the Arduino from the driver board’s motor power supply, allowing the Arduino to run without a computer connection.  - while the Arduino theoretically protects against 12V back-flowing to the USB connector via the VIN pin, you risk damaging your computer if that protection fails (or is not implemented for some reason on a cheap knock-off Arduino clone).