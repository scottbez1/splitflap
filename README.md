# DIY Split-Flap Display

This is a work in progress DIY [split-flap display](https://en.wikipedia.org/wiki/Split-flap_display).
Prototype four-character display: [video](https://www.youtube.com/watch?v=vq4o_88kN8g).

![animated rendering](https://s3.amazonaws.com/splitflap-travis/branches/master/3d_animation.gif)
[![prototype video](renders/prototypeVideoThumbnail.jpg)](https://www.youtube.com/watch?v=vq4o_88kN8g)

[![Build Status](https://travis-ci.org/scottbez1/splitflap.svg?branch=master)](https://travis-ci.org/scottbez1/splitflap)

The goal is to make a low-cost display that's easy to fabricate at home in small/single quantities (e.g. custom materials can be ordered from Ponoko or similar, and other hardware is generally available).

The 3d model is built using OpenSCAD in `3d/splitflap.scad`, the driver board is designed in KiCad in `electronics/splitflap.pro`, and the driver firmware is written using Arduino (with PlatformIO support) in `arduino/splitflap/Splitflap/Splitflap.ino`.

You can view an interactive 3d model of the design on the [project website](https://scottbez1.github.io/splitflap/).

**New:** Join the [community Slack group](https://join.slack.com/t/splitflap/shared_invite/zt-dpvol87b-3zUaxXrUd8WauPXr1uBj5Q) to discuss or ask questions about the project! (It's new so it may be a bit quiet, but don't be shy!)

Want to help support prototyping and development?
<a href="https://www.buymeacoffee.com/scottbez1" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee" height="22" ></a>

### Current Status ###
This design is currently nearly stable. The source files provided here have been used to produce multiple working prototypes, but may still have some minor issues and/or incomplete documentation (for more reliable design files, make sure to use files from the official [Releases](https://github.com/scottbez1/splitflap/releases) rather than the bleeding edge designs linked on this page).

| Component | Status | Notes |
| --- | --- | --- |
| Enclosure/Mechanics | *Stable* | |
| Electronics | *Stable* | |
| Firmware | *Stable* | |
| Control Software | *Beta* | Example python code for driving the display is in the [software](https://github.com/scottbez1/splitflap/tree/master/software) directory|

I'd love to hear your thoughts and questions about this project, and happy to incorporate any feedback you might have into these designs! Please feel free (and encouraged) to [open GitHub issues](https://github.com/scottbez1/splitflap/issues/new), email me directly, reach out [on Twitter](https://twitter.com/scottbez1), and [get involved](https://github.com/scottbez1/splitflap/pulls) in the open source development and let's keep chatting and building together!

### Design Highlights ###
* laser cut enclosure and mechanisms from a single material
* cheap, widely available 28byj-48 stepper motor (less expensive than NEMA-17 motors, and doesn't require an expensive high current stepper driver)
* CR80 PVC cards for flaps, cheap in bulk
* store-bought vinyl stickers for flap letters
* control up to 12 modules from a single Arduino

![2d laser cut rendering](https://s3.amazonaws.com/splitflap-travis/branches/master/3d_laser_raster.png)

### Rough Cost Breakdown ###

This is an incomplete list of supplies needed to build a split-flap display module, to get a rough sense of the overall cost, as of May 2018.

#### Housing ####

* $2.00		/unit -- MDF 3.2mm P1 [on Ponoko](http://www.ponoko.com/make-and-sell/show-material/64-mdf-natural)
* $8.67		/unit -- laser cutting on Ponoko (can save ~$0.70 by skipping engraved label)
* ~$9		/unit -- shipping

* ~$20  	/unit -- Total (can be cheaper if ordering multiple units at the same time)

#### Hardware & Consumables ####

* $2.40		/unit -- Qty 12 -- M4-10mm button head screw, hex drive, ISO7380, (e.g. Fastenal A2-70 stainless steel MB2530010A20000)
* $0.67		/unit -- Qty 12 -- M4 hex nut, (e.g. Fastenal A2-70 stainless steel MN2530000A20000)

#### Flaps ####

* $6.39		/2 units -- vinyl letter stickers (minimum letter duplication per pack is 2) [on Amazon](http://www.amazon.com/Duro-Decal-Permanent-Adhesive-Letters/dp/B0027601CM)
* $14		/4 units -- blank white CR80 cards (each CR80 card becomes 2 flaps, each unit requires 40 flaps = 20 cards. 100 cards is enough for 5 units in theory, but 4 in practice because there are no spares for mistakes) on [Amazon](http://www.amazon.com/Plastic-printers-DataCard-Evolis-Magicard/dp/B007M413BC)
  * or $50 /4 units -- Professional pre-cut high quality flaps (no manual cutting/punching needed), custom made for this project, on [Tindie](https://www.tindie.com/products/14844/). Proceeds help support continued development and prototyping.

#### PCB ####

* $4 /4 units -- PCB for controller [on Tindie](https://www.tindie.com/products/15608/)
  * or ~$25		/40 units -- PCB for controller [on seeedstudio](http://www.seeedstudio.com/service/index.php?r=pcb)
* $3		/4 units -- PCB for hall-effect sensor [on Tindie](https://www.tindie.com/products/14793/)
  * or ~$25		/200 units -- PCB for hall-effect sensor [on seeedstudio](http://www.seeedstudio.com/service/index.php?r=pcb)

#### Electronics & Motor ####

* ~$2		/unit -- 28byj-48 motor (12V preferred!) Ebay or AliExpress are good places to find these motors. (see [motor notes](https://github.com/scottbez1/splitflap/wiki/Motor-info) for specific details)
* ~$10		/10 units -- male-to-male 3-pin servo cable (for sensors) e.g. on [Amazon](https://www.amazon.com/VIMVIP-10pcs-300mm-Extension-Futaba/dp/B00N8OX7VO)
* ~$16		/unit -- Digikey electrionic component purchases. ~$14/controller and ~$2/sensor board. More detailed breakout of BOM in [Ordering instructions](https://paper.dropbox.com/doc/Ordering-Splitflap-v0.5--ATdB3cgS5GyvwTWq1kpunkD4Ag-iCwwMo0hRkkAlwdutccWf)
* $8-15 /4 units -- 12V power supply

Tools (one-time, up front cost):
* $9.17 -- badge slot punch for cutting notches out of cards to make flaps (not needed if using die-cut flaps from Tindie) [on Amazon](http://www.amazon.com/gp/product/B009YDRRB4)

## Build Your Own: Instructions ##
This design is still a work in progress; a build log/instructions for building a prototype split-flap display of your own is posted in [the wiki](https://github.com/scottbez1/splitflap/wiki).

<a href="https://github.com/scottbez1/splitflap/wiki">
<img height="320" src="https://github.com/scottbez1/splitflap/wiki/images/assembly/laserPieces.jpg"/>
<img height="320" src="https://github.com/scottbez1/splitflap/wiki/images/flaps/punchedCard.jpg"/>
</a>

Important: most of the diagrams and downloadable files on this README page are auto-generated from the latest *experimental* code, which may be untested or broken. If you are interested in building one without digging into the design details much, I recommend using a stable design, as described in [the wiki](https://github.com/scottbez1/splitflap/wiki).

## Design & Modification Guide ##

### 3D Design ###
The main design file is `3d/splitflap.scad`

You'll need a recent version of OpenSCAD (e.g. 2015-03), which may need to be installed through the PPA:
`sudo add-apt-repository ppa:openscad/releases`

In general, solid objects such as the enclosure sides or spool components are built from 2d primitives and then extruded to the appropriate thickness for 3d rendering, rather than using 3d primitives. This simplifies the design without losing expressiveness; the perpendicular laser cut beam doesn't allow for cuts that vary in the Z dimension anyway.

Note that while the design is parameterized and many values may be tweaked, there is currently no error checking for invalid parameters or combinations of parameters. Please take care to validate the design if you change any parameters. For instance, while most of the design will adjust to a changed `num_modules` value, certain values may cause some elements to intersect with other elements or protrude beyond their expected dimensions.

#### Rendering ####
##### Laser-cut vector files #####
The design can be rendered to 2d for laser cutting by running `3d/scripts/generate_2d.py [--panelize <number>]`, which outputs to `3d/build/laser_parts/combined.svg`. The optional `--panelize` argument allows for rendering a panel of modules in a single SVG, for bulk laser-cutting.

Internally, the design uses a `projection_renderer` module (`3d/projection_renderer.scad`), which takes a list of child elements to render, and depending on the `render_index` renders a single child at a time. It also _adds_ material to each shape to account for the kerf that will be cut away by the laser.

The `generate_2d.py` script interacts with the `projection_renderer` module by first using it to determine the number of subcomponents to render, then runs OpenSCAD to export each component to an SVG file. It does some post-processing on the SVG output (notably adds "mm" to the document dimensions), and then combines all components into the single `combined.svg` output.

Once the `combined.svg` file is generated, you'll want to double-check there aren't any redundant cut lines that are shared by multiple adjacent pieces, to save time/cost when cutting. They should be detected automatically (and highlighted in red in the rendering above), but it doesn't hurt to double-check. In Inkscape, select the "Edit paths by nodes" tool and select an edge to delete - the endpoints should turn blue. Then click "Delete segment between two non-endpoint nodes", and repeat this for all other redundant cut lines.

Latest (Experimental!) Laser Cut Vector File: [svg](https://s3.amazonaws.com/splitflap-travis/branches/master/3d_laser_vector.svg)
(In order to get the design laser-cut from Ponoko, you'll need to copy all of the shapes from that file into one of [Ponoko's templates](http://www.ponoko.com/starter-kits/inkscape))

##### Animated gif #####
The design can be rendered to a rotating 3d animated gif (seen above) by running `3d/scripts/generate_gif.py`, which outputs to `3d/build/animation/animation.gif`

The `generate_gif.py` script runs multiple OpenSCAD instances in parallel to render the design from 360 degrees to individual png frames, which are then combined into the final gif animation. As part of building the animation, `generate_gif.py` renders the design with multiple configurations (opaque enclosure, see-through enclosure, no-enclosure and no flaps) by setting the `render_enclosure` and `render_flaps` variables.

##### STL models/web viewer #####
The design can be rendered to a series of STL files (one per color used in the model) in order to be displayed in an [interactive web-based 3d viewer](https://scottbez1.github.io/splitflap/). Similar to the `projection_renderer` used to render individual components for laser-cutting, the [ColoredStlExporter](3d/scripts/colored_stl_exporter.py) detects all the colors used in the model and renders them one-by-one to separate STL files, along with a manifest that maps each STL file to its RGB color. The STL files and manifest are loaded using three.js to display an interactive model on a web site using WebGL. See this blog post for more details on how the export and three.js renderer work: [OpenSCAD Rendering Tricks, Part 3: Web viewer](http://scottbezek.blogspot.com/2016/08/openscad-rendering-tricks-part-3-web.html).


### Driver Electronics ###
The driver board is designed to plug into an Arduino like a shield, and can control 4 stepper motors.
Up to 3 driver boards can be chained together, for up to 12 modules controlled by a single Arduino.
The designs for the controller can be found under `electronics/splitflap.pro` (KiCad 4 project).
Nearly everything is a through-hole component rather than SMD, so it's very easy to hand-solder.

The driver uses 2 MIC5842 low-side shift-register drivers, with built-in transient-suppression diodes, to control the motors, and a 74HC165 shift register to read from 4 hall-effect magnetic home position sensors.
There are optional WS2812B RGB LEDs which can be used to indicate the status of each of the 4 channels.

<a href="https://s3.amazonaws.com/splitflap-travis/branches/master/schematic.pdf">
<img height="320" src="https://s3.amazonaws.com/splitflap-travis/branches/master/schematic.png"/>
</a>

The PCB layout is 10cm x 5cm which makes it fairly cheap to produce using a low-cost PCB manufacturer (e.g. Seeed Studio).

<a href="https://s3.amazonaws.com/splitflap-travis/branches/master/pcb_raster.png">
<img width="640" src="https://s3.amazonaws.com/splitflap-travis/branches/master/pcb_raster.png"/>
</a>

Each module also needs a hall-effect sensor board, with an AH3391Q (or similar) sensor and connector.
These boards are small (about 16mm x 16 mm) and are available on a second PCB design that's panelized.
The panelization is configurable (see [generate_panelize_config.py](https://github.com/scottbez1/splitflap/blob/master/electronics/generate_panelize_config.py))
and is optimized for production at SeeedStudio.

<a href="https://s3.amazonaws.com/splitflap-travis/branches/master/sensor_pcb_raster.png">
<img width="640" src="https://s3.amazonaws.com/splitflap-travis/branches/master/sensor_pcb_raster.png"/>
</a>

##### Latest PCB Renderings #####
These are automatically updated on every commit with the latest rendering from the `master` branch.
See this blog post for more details on how that works: [Automated KiCad, OpenSCAD rendering using Travis CI](http://scottbezek.blogspot.com/2016/04/automated-kicad-openscad-rendering.html).

For Stable PCB designs, make sure to check out the [Releases](https://github.com/scottbez1/splitflap/releases) instead of using these experimental files.

Latest (experimental!) Controller PCB Gerbers: [zip](https://s3.amazonaws.com/splitflap-travis/branches/master/pcb_gerber.zip)

Latest (experimental!) Controller PCB Packet: [pdf](https://s3.amazonaws.com/splitflap-travis/branches/master/pcb_packet.pdf)

Latest (experimental!) Controller PCB (Panelized) Gerbers: [zip](https://s3.amazonaws.com/splitflap-travis/branches/master/panelized_pcb_gerber.zip)

Latest (experimental!) Controller PCB (Panelized) Packet: [pdf](https://s3.amazonaws.com/splitflap-travis/branches/master/panelized_pcb_packet.pdf)

Latest (experimental!) Sensor PCB (Panelized) Gerbers: [zip](https://s3.amazonaws.com/splitflap-travis/branches/master/sensor_pcb_gerber.zip)

Latest (experimental!) Sensor PCB (Panelized) Packet: [pdf](https://s3.amazonaws.com/splitflap-travis/branches/master/sensor_pcb_packet.pdf)

Latest (experimental!) rough bill of materials: [csv](https://s3.amazonaws.com/splitflap-travis/branches/master/bom.csv)

#### Rendering ####
The PCB layout can be rendered to an svg or png (seen above) by running `electronics/generate_svg.py file.kicad_pcb`.
This uses KiCad's [python scripting API](https://github.com/blairbonnett-mirrors/kicad/blob/master/demos/python_scripts_examples/plot_board.py)
to render several layers to individual svg files, manipulates them to apply color and opacity settings, and then merges them to a single svg.
For additional details, see this blog post: [Scripting KiCad Pcbnew exports](http://scottbezek.blogspot.com/2016/04/scripting-kicad-pcbnew-exports.html).

For reviewing the design, a pdf packet with copper, silkscreen, and drill info can be produced by running `electronics/generate_pdf.py file.kicad_pcb`.

Gerber files for fabrication can be exported by running `electronics/generate_gerber.py file.kicad_pcb`.
This generates gerber files and an Excellon drill file with Seeed Studio's [naming conventions](http://support.seeedstudio.com/knowledgebase/articles/422482-fusion-pcb-order-submission-guidelines) and produces a `.zip` which can be sent for fabrication.

EESchema isn't easily scriptable, so to export the schematic and bill of materials `electronics/scripts/export_schematic.py` and `export_bom.py` start an X Virtual Frame Buffer (Xvfb) and open the `eeschema` GUI within that virtual display, and then send a series of hardcoded key presses via `xdotool` to interact with the GUI and click through the dialogs. This is very fragile but seems to work ok for now. For additional details, see this blog post: [Using UI automation to export KiCad schematics](http://scottbezek.blogspot.com/2016/04/automated-kicad-schematic-export.html).

### Driver Firmware ###
The driver firmware is written using Arduino and is available at `arduino/splitflap/Splitflap/Splitflap.ino`. 

The firmware currently runs a basic closed-loop controller that accepts letters over USB serial and drives the stepper motors using a precomputed acceleration ramp for smooth control. The firmware automatically calibrates the spool position at startup, using the hall-effect magnetic sensor, and will automatically recalibrate itself if it ever detects that the spool position has gotten out of sync. If a commanded rotation is expected to bring the spool past the "home" position, it will confirm that the sensor is triggered neither too early nor too late; otherwise it will search for the "home" position to get in sync before continuing to the desired letter.

### Computer Control Software ###
The display can be controlled by a computer connected to the Arduino over USB serial. A basic python library for interfacing with the Arduino and a demo application that displays random words can be found in the [software](https://github.com/scottbez1/splitflap/tree/master/software) directory.

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
