# DIY Split-Flap Display

This is a work in progress DIY [split-flap display](https://en.wikipedia.org/wiki/Split-flap_display).
Prototype four-character display: [video](https://www.youtube.com/watch?v=vq4o_88kN8g).

![animated rendering](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/3d/3d_animation.gif)
[![prototype video](renders/prototypeVideoThumbnail.jpg)](https://www.youtube.com/watch?v=vq4o_88kN8g)

[![Build Status](https://github.com/scottbez1/splitflap/actions/workflows/3d.yml/badge.svg?branch=dev/newBoard)](https://github.com/scottbez1/splitflap/actions/workflows/3d.yml)
[![Build Status](https://github.com/scottbez1/splitflap/actions/workflows/electronics.yml/badge.svg?branch=dev/newBoard)](https://github.com/scottbez1/splitflap/actions/workflows/electronics.yml)
[![Build Status](https://github.com/scottbez1/splitflap/actions/workflows/electronics_legacy.yml/badge.svg?branch=dev/newBoard)](https://github.com/scottbez1/splitflap/actions/workflows/electronics_legacy.yml)

The goal is to make a low-cost display that's easy to fabricate at home in small/single quantities (e.g. custom materials can be ordered from Ponoko or similar, and other hardware is generally available).

The 3d model is built using OpenSCAD in [`3d/splitflap.scad`](3d/splitflap.scad), the driver board is designed in KiCad in [`electronics/splitflap.pro`](electronics/splitflap.pro), and the driver firmware is written using Arduino (with PlatformIO support) in [`arduino/splitflap/Splitflap/Splitflap.ino`](arduino/splitflap/Splitflap/Splitflap.ino).

You can view an interactive 3d model of the design on the [project website](https://scottbez1.github.io/splitflap/).

**New:** Join the [community Slack group](https://join.slack.com/t/splitflap/shared_invite/zt-dpvol87b-3zUaxXrUd8WauPXr1uBj5Q) to discuss or ask questions about the project!

Want to help support prototyping and development?
<a href="https://www.buymeacoffee.com/scottbez1" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee" height="22" ></a>

<a href="https://www.youtube.com/watch?v=UAQJJAQSg_g" target="_blank"><img src="https://img.youtube.com/vi/UAQJJAQSg_g/maxresdefault.jpg" alt="How a split-flap display works" width="640"></a>


### Current Status ###
[You can download the **latest stable release** from the official 'releases' page.](https://github.com/scottbez1/splitflap/releases/latest)

Releases have been tested and used to produce working units, but as this is a DIY project, there may always be minor issues and/or incomplete documentation.

| Component | Status | Notes |
| --- | --- | --- |
| Enclosure/Mechanics | *Stable* | |
| Classic Controller Electronics | *Stable* | |
| New Chainlink Electronics | *Alpha* | Next-generation/alternative electronics, designed for easier assembly and larger displays |
| Firmware | *Stable* | |
| Control Software Example | *Stable* | Example python code for driving the display is in the [software](software) directory|


I'd love to hear your thoughts and questions about this project, and happy to incorporate any feedback you might have into these designs! Please feel free (and encouraged) to [open GitHub issues](https://github.com/scottbez1/splitflap/issues/new), email me directly, reach out [on Twitter](https://twitter.com/scottbez1), and [get involved](https://github.com/scottbez1/splitflap/pulls) in the open source development and let's keep chatting and building together!

#### :warning: Latest Experimental Files: ####

**Important:** the files linked below are EXPERIMENTAL and have been auto-generated from the latest commit. They may contain changes that are untested or outright broken. Do not use these unless you are actively making changes to the project and know what you're doing. Otherwise, use the [latest stable release](https://github.com/scottbez1/splitflap/releases/latest).

* Lasercut Vector File ([svg](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/3d/3d_laser_vector.svg))
* Classic Controller electronics
    * Bill of Materials ([csv](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/bom.csv))
    * PCB ([gerbers](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/pcb_gerber.zip) / [pdf](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/pcb_packet.pdf)), panelized ([gerbers](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/panelized_pcb_gerber.zip) / [pdf](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/panelized_pcb_packet.pdf))
* Chainlink Driver electronics
    * Schematic [pdf](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics/chainlinkDriver-schematic.pdf)
    * PCB overview [pdf](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics/chainlinkDriver-pcb-packet.pdf)
    * PCB gerbers [zip](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics/jlc/gerbers.zip)
    * PCB bom (for JLCPCB assembly) [csv](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics/jlc/bom.csv)
    * PCB component positions (for JLCPCB assembly) [csv](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics/jlc/pos.csv)
* Sensor PCB, panelized ([gerbers](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/sensor_pcb_gerber.zip) / [pdf](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/sensor_pcb_packet.pdf))

If you are interested in building a display, I recommend using [one of the stable releases instead](https://github.com/scottbez1/splitflap/releases).

### Design Highlights ###
* laser cut enclosure and mechanisms from a single material
* cheap, widely available 28byj-48 stepper motor (less expensive than NEMA-17 motors, and doesn't require an expensive high current stepper driver)
* CR80 PVC cards for flaps, cheap in bulk
* store-bought vinyl stickers for flap letters
* control up to 12 modules from a single Arduino Uno

![2d laser cut rendering](https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/3d/3d_laser_raster.png)

## Build Your Own  ##

* [**Ordering guide (v0.5/0.6)**](https://paper.dropbox.com/doc/Ordering-Splitflap-v0.5--AS8OCZ~75DLuHBGHhxn94YAMAg-iCwwMo0hRkkAlwdutccWf)
* [**Latest stable release**](https://github.com/scottbez1/splitflap/releases/latest)
* [**Assembly instructions (v0.5/0.6)**](https://paper.dropbox.com/doc/Splitflap-Kit-v0.5v1.0-Instructions--ASQDu1uoa6n4_t1pva1bM1MgAg-bxXJlke5ROmamcx4OH44r)

### Rough Cost Breakdown ###

This is an incomplete list of supplies needed to build a split-flap display module, to get a rough sense of the overall cost, as of May 2018.

#### Housing ####

| Price | Name          | Supplier | Notes |
|-------|---------------|----------|-------|
| $2.00 | MDF 3.2 mm    | Ponoko   |       |
| $8.67 | Laser cutting | Ponoko   |       |
| $9    | Shipping      | Ponoko   |       |

**Total:** $19.67 per unit

#### Hardware & Consumables ####

| Price | Name                        | Quantity | Supplier   | Notes |
|-------|-----------------------------|----------|------------|-------|
| $2.40 | M4-10 Button-Head Hex Screw | 12       | AliExpress |       |
| $0.67 | M4 Hex Nut                  | 12       | AliExpress |       |

**Total:** $3.07 per unit

#### Flaps ####

There are two options for flaps: hand-cut and die-cut.

##### Hand-Cut (Option 1) #####

| Price | Name                   | Supplier                                                                                | Notes                            |
|-------|------------------------|-----------------------------------------------------------------------------------------|----------------------------------|
| $3.50 | Blank white CR80 cards | [Amazon](http://www.amazon.com/Plastic-printers-DataCard-Evolis-Magicard/dp/B007M413BC) | One package ($14) covers 4 units |
| $9.17 | Badge slot punch tool  | [Amazon](http://www.amazon.com/gp/product/B009YDRRB4)                                   | One-time, up front cost          |
| $3.50 | Vinyl letter stickers  | [Amazon](http://www.amazon.com/Duro-Decal-Permanent-Adhesive-Letters/dp/B0027601CM)     | One package ($7) covers 2 units  |

**Total:** $7.00 per unit, + $9.17 upfront for the punch tool

Each card needs to be cut in half and then have two pieces punched out of either side. Each display unit requires 40 flaps, or 20 cards per unit. 100 cards is enough for 5 units in theory, but 4 in practice because there are no spares for mistakes.

##### Die-Cut (Option 2) #####

| Price  | Name                  | Supplier                                                                            | Notes                            |
|--------|-----------------------|-------------------------------------------------------------------------------------|----------------------------------|
| $12.50 | Die-cut flaps         | [Tindie](https://www.tindie.com/products/14844/)                                    | One package ($50) covers 4 units |
| $3.50  | Vinyl letter stickers | [Amazon](http://www.amazon.com/Duro-Decal-Permanent-Adhesive-Letters/dp/B0027601CM) | One package ($7) covers 2 units  |

These professional die-cut flaps are pre-cut with the exact dimensions needed for this project (no manual cutting/punching required) and come in both white and matte black. Proceeds help support continued development and prototyping.

**Total:** $16.00 per unit

#### PCB ####

Circuit boards are available either in small quantities from Tindie, or in bulk from PCB manufacturers such as SeeedStudio.

##### Tindie #####

| Price | Name           | Quantity | Supplier                                         | Notes                    |
|-------|----------------|----------|--------------------------------------------------|--------------------------|
| $3.49 | Controller PCB | 1        | [Tindie](https://www.tindie.com/products/15608/) | One PCB supports 4 units |
| $2.49 | Sensor PCB     | 4        | [Tindie](https://www.tindie.com/products/14793/) | 4 PCBs per package       |

**Total:** $5.98 per 4 units ($1.49 each)

##### SeeedStudio (Bulk) #####

| Price | Name           | Quantity | Supplier                                                          | Notes                    |
|-------|----------------|----------|-------------------------------------------------------------------|--------------------------|
| $25   | Controller PCB | 40       | [SeeedStudio](http://www.seeedstudio.com/service/index.php?r=pcb) | One PCB supports 4 units |
| $25   | Sensor PCB     | 200      | [SeeedStudio](http://www.seeedstudio.com/service/index.php?r=pcb) |                          |

**Total:** $50.00 per 40 units ($0.80 each)

#### Electronics & Motor ####

| Price  | Name                             | Supplier                                                                           | Notes                                                                                  |
|--------|----------------------------------|------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------|
| $14.00 | Motor driver components          | DigiKey                                                                            | One PCB supports 4 units                                                               |
| $2.00  | Sensor board components          | DigiKey                                                                            |                                                                                        |
| $2.00  | 28byj-48 motor (12V preferred)   | AliExpress                                                                         | See [motor notes](https://github.com/scottbez1/splitflap/wiki/Motor-info) for details  |
| $1.00  | 3-pin servo cable (male to male) | [Amazon](https://www.amazon.com/VIMVIP-10pcs-300mm-Extension-Futaba/dp/B00N8OX7VO) |                                                                                        |
| $11.00 | 12V power supply                 | Amazon                                                                             | One per display (up to 12 units)                                                       |

**Total:** $5.00 per unit (motor + sensor comp.) + $14.00 per 4 units (motor driver), + $11.00 per display (up to 12 units)

You can find a more detailed breakout of electronics' bill of materials (BOM) in [the ordering instructions](https://paper.dropbox.com/doc/Ordering-Splitflap-v0.5--ATdB3cgS5GyvwTWq1kpunkD4Ag-iCwwMo0hRkkAlwdutccWf).

## Design & Modification Guide ##

### 3D Design ###
The main design file is [`3d/splitflap.scad`](3d/splitflap.scad)

You'll need a recent version of OpenSCAD (e.g. 2015-03), which may need to be installed through the PPA:
`sudo add-apt-repository ppa:openscad/releases`

In general, solid objects such as the enclosure sides or spool components are built from 2d primitives and then extruded to the appropriate thickness for 3d rendering, rather than using 3d primitives. This simplifies the design without losing expressiveness; the perpendicular laser cut beam doesn't allow for cuts that vary in the Z dimension anyway.

Note that while the design is parameterized and many values may be tweaked, there is currently no error checking for invalid parameters or combinations of parameters. Please take care to validate the design if you change any parameters. For instance, while most of the design will adjust to a changed `num_modules` value, certain values may cause some elements to intersect with other elements or protrude beyond their expected dimensions.

#### Rendering ####
##### Laser-cut vector files #####
The design can be rendered to 2d for laser cutting by running [`3d/scripts/generate_2d.py [--panelize <number>]`](3d/scripts/generate_2d.py), which outputs to `3d/build/laser_parts/combined.svg`. The optional `--panelize` argument allows for rendering a panel of modules in a single SVG, for bulk laser-cutting.

Internally, the design uses a `projection_renderer` module ([`3d/projection_renderer.scad`](3d/projection_renderer.scad)), which takes a list of child elements to render, and depending on the `render_index` renders a single child at a time. It also _adds_ material to each shape to account for the kerf that will be cut away by the laser.

The [`generate_2d.py`](3d/scripts/generate_2d.py) script interacts with the `projection_renderer` module by first using it to determine the number of subcomponents to render, then runs OpenSCAD to export each component to an SVG file. It does some post-processing on the SVG output (notably adds "mm" to the document dimensions), and then combines all components into the single `combined.svg` output.

Once the `combined.svg` file is generated, you'll want to double-check there aren't any redundant cut lines that are shared by multiple adjacent pieces, to save time/cost when cutting. They should be detected automatically (and highlighted in red in the rendering above), but it doesn't hurt to double-check. In Inkscape, select the "Edit paths by nodes" tool and select an edge to delete - the endpoints should turn blue. Then click "Delete segment between two non-endpoint nodes", and repeat this for all other redundant cut lines.

##### Animated gif #####
The design can be rendered to a rotating 3d animated gif (seen above) by running [`3d/scripts/generate_gif.py`](3d/scripts/generate_gif.py), which outputs to `3d/build/animation/animation.gif`

The `generate_gif.py` script runs multiple OpenSCAD instances in parallel to render the design from 360 degrees to individual png frames, which are then combined into the final gif animation. As part of building the animation, `generate_gif.py` renders the design with multiple configurations (opaque enclosure, see-through enclosure, no-enclosure and no flaps) by setting the `render_enclosure` and `render_flaps` variables.

##### STL models/web viewer #####
The design can be rendered to a series of STL files (one per color used in the model) in order to be displayed in an [interactive web-based 3d viewer](https://scottbez1.github.io/splitflap/). Similar to the `projection_renderer` used to render individual components for laser-cutting, the [ColoredStlExporter](3d/scripts/colored_stl_exporter.py) detects all the colors used in the model and renders them one-by-one to separate STL files, along with a manifest that maps each STL file to its RGB color. The STL files and manifest are loaded using three.js to display an interactive model on a web site using WebGL. See this blog post for more details on how the export and three.js renderer work: [OpenSCAD Rendering Tricks, Part 3: Web viewer](http://scottbezek.blogspot.com/2016/08/openscad-rendering-tricks-part-3-web.html).

#### 3D Printed Tools ####
The project also includes a number of optional 3D printed designs to make assembly easier. These include:

* [a flap scoring jig](3d/tools/scoring_jig.scad) for precisely marking the cut point when splitting CR80 cards
* [a flap punch jig](3d/tools/punch_jig.scad) for aligning the punch when making the pin cutouts on either side of a flap
* [a flap container](3d/tools/flap_container.scad) for storing and organizing stacks of completed flaps
* [a sensor PCB holder](3d/tools/pcb_case.scad) for storing and protecting soldered sensor boards

All of these designs are parametric and customizable within OpenSCAD. To print them, open up the relevant file in OpenSCAD and use `File -> Export -> Export as STL` to render the design as an STL file for your slicer.

### Classic Driver Electronics ###
The Classic driver board is designed to plug into an Arduino like a shield, and can control 4 stepper motors.
Up to 3 driver boards can be chained together, for up to 12 modules controlled by a single Arduino.
The designs for the controller can be found under [`electronics/splitflap.pro`](electronics/splitflap.pro) (KiCad 4 project).
Nearly everything is a through-hole component rather than SMD, so it's very easy to hand-solder.

The driver uses 2 MIC5842 low-side shift-register drivers, with built-in transient-suppression diodes, to control the motors, and a 74HC165 shift register to read from 4 hall-effect magnetic home position sensors.
There are optional WS2812B RGB LEDs which can be used to indicate the status of each of the 4 channels.

<a href="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/schematic.pdf">
<img height="320" src="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/schematic.png"/>
</a>

The PCB layout is 10cm x 5cm which makes it fairly cheap to produce using a low-cost PCB manufacturer (e.g. Seeed Studio).

<a href="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/pcb_raster.png">
<img width="640" src="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/pcb_raster.png"/>
</a>


### Chainlink Electronics ###
The Chainlink series is an alternative to the Classic controller/driver, which is designed to be easier and cheaper to build in higher quantities (simplified BOM, mostly SMD) and supports longer chains of driver boards to control large displays (think 100+ modules). This electroncis design is still in very active development.

There are two PCBs for a Chainlink system:
* **Chainlink Driver** boards accept data and power and connect to the individual split-flap modules. They can be chained together to construct a larger display.
* **Chainlink Central Controller** boards hold the microcontroller (ESP32 with USB, Wifi, BLE capabilities), central power management/distribution, and sends data to the chained Driver boards.

#### Chainlink Driver ####
<a href="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics/chainlinkDriver-3d.png">
<img width="640" src="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics/chainlinkDriver-3d.png"/>
</a>

Key features:
* Supports 6 modules instead of just 4 - fewer PCBs means lower cost and less wiring
* Longer PCB (200mm) gets connectors closer to each module so wires have more slack
* Module order goes from right-to-left since this is intended to be installed *behind* the modules
* Primarily SMD and all components (except the pin headers and connectors) are available on JLCPCB for SMD/THT assembly
* Uses a 74HC125 buffer for the clock and latch lines to support longer chains
* Neopixels replaced by shift-register-driven single-color LED per module (one fewer data line needed in the chain, lower current draw, and 3.3v IO friendly)
* 2 bits of loopback (connecting 2 spare output bits on shift registers to 2 spare inputs) allows the controller to validate data integrity down the whole chain
* TPL7407L mosfet low-side drivers instead of ULN2003A/MIC5842 (lower on-resistance means less voltage drop in the driver = more torque, less waste heat, and less total current consumption)


<a href="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics/chainlinkDriver-schematic.pdf">
<img width="640" src="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics/chainlinkDriver-schematic.png"/>
</a>


#### Chainlink Central Controller ####
Not designed yet. Follow development in the slack group!

### Module Electronics ###
Each module also needs a hall-effect sensor board, with an AH3391Q (or similar) sensor and connector.
These boards are small (about 16mm x 16 mm) and are available on a second PCB design that's panelized.
The panelization is configurable (see [generate_panelize_config.py](electronics/scripts/panelize/generate_panelize_config.py))
and is optimized for production at SeeedStudio.

<a href="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/sensor_pcb_raster.png">
<img width="640" src="https://s3.amazonaws.com/splitflap-artifacts/dev/newBoard/electronics-legacy/sensor_pcb_raster.png"/>
</a>


#### Rendering ####
The renderings and links above are automatically updated on every commit with the latest rendering. See this blog post for more details on how that works: [Automated KiCad, OpenSCAD rendering using Travis CI](http://scottbezek.blogspot.com/2016/04/automated-kicad-openscad-rendering.html).

The PCB layout can be rendered to an svg or png (seen above) by running [`electronics/scripts/generate_svg.py file.kicad_pcb`](electronics/scripts/generate_svg.py).
This uses KiCad's [Python scripting API](https://docs.kicad-pcb.org/doxygen/md_Documentation_development_pcbnew-plugins.html)
to render several layers to individual svg files, manipulates them to apply color and opacity settings, and then merges them to a single svg.
For additional details, see this blog post: [Scripting KiCad Pcbnew exports](http://scottbezek.blogspot.com/2016/04/scripting-kicad-pcbnew-exports.html).

For reviewing the design, a pdf packet with copper, silkscreen, and drill info can be produced by running [`electronics/scripts/generate_pdf.py file.kicad_pcb`](electronics/scripts/generate_pdf.py).

Gerber files for fabrication can be exported by running [`electronics/scripts/generate_gerber.py file.kicad_pcb`](electronics/scripts/generate_gerber.py).
This generates gerber files and an Excellon drill file with Seeed Studio's [naming conventions](http://support.seeedstudio.com/knowledgebase/articles/1176532-how-to-generate-the-gerber-manufacturing-files) and produces a `.zip` which can be sent for fabrication.

EESchema isn't easily scriptable, so to export the schematic and bill of materials [`electronics/scripts/export_schematic.py`](electronics/scripts/export_schematic.py) and [`export_bom.py`](electronics/scripts/export_bom.py) start an X Virtual Frame Buffer (Xvfb) and open the `eeschema` GUI within that virtual display, and then send a series of hardcoded key presses via `xdotool` to interact with the GUI and click through the dialogs. This is very fragile but seems to work ok for now. For additional details, see this blog post: [Using UI automation to export KiCad schematics](http://scottbezek.blogspot.com/2016/04/automated-kicad-schematic-export.html).

### Driver Firmware ###
The driver firmware is written using Arduino and is available at [`arduino/splitflap/Splitflap/Splitflap.ino`](arduino/splitflap/Splitflap/Splitflap.ino). 

The firmware currently runs a basic closed-loop controller that accepts letters over USB serial and drives the stepper motors using a precomputed acceleration ramp for smooth control. The firmware automatically calibrates the spool position at startup, using the hall-effect magnetic sensor, and will automatically recalibrate itself if it ever detects that the spool position has gotten out of sync. If a commanded rotation is expected to bring the spool past the "home" position, it will confirm that the sensor is triggered neither too early nor too late; otherwise it will search for the "home" position to get in sync before continuing to the desired letter.

### Computer Control Software ###
The display can be controlled by a computer connected to the Arduino over USB serial. A basic python library for interfacing with the Arduino and a demo application that displays random words can be found in the [software](software) directory.

Commands to the display are sent in a basic plain-text format, and messages _from_ the display are single-line JSON objects, always with a `type` entry describing which type of message it is.

When the Arduino starts up, it sends an initialization message that looks like:
```
{"type":"init", "num_modules":4}
```

The display will automatically calibrate all modules, and when complete it will send a status update message:
```
{
    "type":"status",
    "modules":[
        {"state":"normal", "flap":" ", "count_missed_home":0, "count_unexpected_home":0},
        {"state":"sensor_error", "flap":"e", "count_missed_home":0, "count_unexpected_home":0},
        {"state":"sensor_error", "flap":"e", "count_missed_home":0, "count_unexpected_home":0},
        {"state":"sensor_error", "flap":"e", "count_missed_home":0, "count_unexpected_home":0}
    ]
}
```
(Note: this is sent as a single line, but has been reformatted for readability above)

In this case the Arduino was programmed to support 4 modules, but only 1 module is connected, so the other 3 end up in `"sensor_error"` state. More on status updates below.

At this point you can command the display to show some letters. To do this, send a message to the Arduino that looks like this:
```
=hiya\n
```
The `=` indicates a movement command, followed by any number of letters, followed by a newline. You don't have to send the exact number of modules - if you send fewer letters than modules, only the first N modules will be updated and the remainder won't move. For instance, you could send `=a\n` as shorthand to only set the first module (even if there are 12 modules connected). Any letters that can't be displayed are considered a no-op for that module.

Whenever ALL modules come to a stop, the Arduino will send a status update message (just like the one following initialization, shown above). Here's what the fields mean in each module's status entry:
- **state** - `normal` indicates it's working as intended, `sensor_error` indicates the module can't find the home position and has given up trying (it will no longer respond to movement commands until told to recalibrate - see below). `panic` indicates the firmware detected a programming bug and has gone into failsafe mode (it will no longer respond to movement commands and requires a full reset of the Arduino to recover - should never happen).
- **flap** - which letter is shown by this module
- **count\_missed\_home** - number of times the module expected to pass the home position but failed to detect it. If this is non-zero, it indicates either a flaky sensor or that the motor may have jammed up. The module automatically attempts to recalibrate whenever it misses the home position, so if this number is non-zero and the module is still in the `normal` state, it means the module successfully recovered from the issue(s). However, if this number keeps going up over continued use, it may indicate a recurrent transient issue that warrants investigation.
- **count\_unexpected\_home** - number of times the module detected the home position when it wasn't supposed to. This is rare, but would indicate a flaky/broken sensor that is tripping at the wrong time. Just like with missed home errors, unexpected home errors will cause the module to attempt to recalibrate itself.

If you want to make all modules recalibrate their home position, send a single @ symbol (no newline follows):
```
@
```
This recalibrates all modules, including any that were in the `sensor_error` state; if recalibration succeeds they will return to the `normal` state and start responding to movement commands again.


## License ##
I'd love to hear your thoughts and questions about this project, and happy to incorporate any feedback you might have into these designs! Please feel free (and encouraged) to [open GitHub issues](https://github.com/scottbez1/splitflap/issues/new), email me directly, reach out [on Twitter](https://twitter.com/scottbez1), and [get involved](https://github.com/scottbez1/splitflap/pulls) in the open source development and let's keep chatting and building together!

The vast majority of this project is licensed under Apache v2 (see [LICENSE.txt](LICENSE.txt) for full details).

    Copyright 2015-2018 Scott Bezek and the splitflap contributors
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
