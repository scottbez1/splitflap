# DIY Split-Flap Display

This is a work in progress DIY [split-flap display](https://en.wikipedia.org/wiki/Split-flap_display).
Prototype three-character display: [video](https://www.youtube.com/watch?v=bslkflVv-Hw).

![animated rendering](https://s3.amazonaws.com/splitflap-travis/branches/master/3d_animation.gif)
[![prototype video](renders/prototypeVideoThumbnail.jpg)](https://www.youtube.com/watch?v=bslkflVv-Hw)

[![Build Status](https://travis-ci.org/scottbez1/splitflap.svg?branch=master)](https://travis-ci.org/scottbez1/splitflap)

The goal is to make a low-cost display that's easy to fabricate at home in small/single quantities (e.g. custom materials can be ordered from Ponoko or similar, and other hardware is generally available).

**New!** Kits based on these designs will be available for sale soon (summer/fall 2018)! If you're interested, please see this page for more information and to fill out a brief interest survey: https://goo.gl/forms/O17gYmE1ZUTwa7M73

The 3d model is built using OpenSCAD in `3d/splitflap.scad`, the driver board is designed in KiCad in `electronics/splitflap.pro`, and the driver firmware is written using Arduino in `arduino/splitflap/splitflap.ino`.

You can view an interactive 3d model of the design on the [project website](https://scottbez1.github.io/splitflap/).

### Current Status ###
This design is currently at a *prototype* stage. The source files provided here were able to produce multiple working prototypes, but may still have some minor issues and/or incomplete documentation.

| Component | Status | Notes |
| --- | --- | --- |
| Enclosure/Mechanics | *Release Candidate* | Need documentation on ordering. |
| Electronics | *Release Candidate* | Need documentation on ordering and assembly. |
| Firmware | *Beta* | Works, but could use a better serial protocol for control/feedback. |
| Control Software | *none* | No work started; currently manual control using Arduino Serial Monitor |

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
* ???		/unit -- Qty ?? -- Heatshrink

#### Flaps ####

* $6.39		/2 units -- vinyl letter stickers (minimum letter duplication per pack is 2) [on Amazon](http://www.amazon.com/Duro-Decal-Permanent-Adhesive-Letters/dp/B0027601CM)
* $12		/5 units
* or $36	/25 units -- CR80 cards (each CR80 card becomes 2 flaps, each unit requires 40 flaps) on [Amazon](http://www.amazon.com/Plastic-printers-DataCard-Evolis-Magicard/dp/B007M413BC) or [Amazon](http://www.amazon.com/White-Blank-CR80-020-Graphic-Quality/dp/B007PKD6MW)

#### PCB ####

* $14		/10 units -- PCB for reflectance sensor [on seeedstudio](http://www.seeedstudio.com/service/index.php?r=pcb)

#### Electronics & Motor ####

* ~$2		/unit -- 28byj-48 motor (12V preferred!) Ebay is a good place to find these motors. (see [motor notes](https://github.com/scottbez1/splitflap/wiki/Motor-info) for specific details)
* ~$10		/10 units -- male-to-male 3-pin servo cable (for sensors) e.g. on [Amazon](https://www.amazon.com/VIMVIP-10pcs-300mm-Extension-Futaba/dp/B00N8OX7VO)
* ~$16		/unit -- Digikey electrionic component purchases. ~$14/controller and ~$2/sensor board. More detailed breakout of BOM in [Wiki - Order Parts](https://github.com/scottbez1/splitflap/wiki/Order-parts)
* $7 -- 12V power supply on [Digikey](https://www.digikey.com/product-detail/en/cui-inc/SWI5-12-N-P5/102-4122-ND/6579912)

Tools (one-time, up front cost):
* $9.17 -- badge slot punch (for cutting notches out of cards to make flaps) [on Amazon](http://www.amazon.com/gp/product/B009YDRRB4)

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
The design can be rendered to 2d for laser cutting by running `3d/generate_2d.py [--panelize <number>]`, which outputs to `3d/build/laser_parts/combined.svg`. The optional `--panelize` argument allows for rendering a panel of modules in a single SVG, for bulk laser-cutting.

Internally, the design uses a `projection_renderer` module (`3d/projection_renderer.scad`), which takes a list of child elements to render, and depending on the `render_index` renders a single child at a time. It also _adds_ material to each shape to account for the kerf that will be cut away by the laser.

The `generate_2d.py` script interacts with the `projection_renderer` module by first using it to determine the number of subcomponents to render, then runs OpenSCAD to export each component to an SVG file. It does some post-processing on the SVG output (notably adds "mm" to the document dimensions), and then combines all components into the single `combined.svg` output.

Once the `combined.svg` file is generated, you'll want to double-check there aren't any redundant cut lines that are shared by multiple adjacent pieces, to save time/cost when cutting. They should be detected automatically (and highlighted in red in the rendering above), but it doesn't hurt to double-check. In Inkscape, select the "Edit paths by nodes" tool and select an edge to delete - the endpoints should turn blue. Then click "Delete segment between two non-endpoint nodes", and repeat this for all other redundant cut lines.

Latest (Experimental!) Laser Cut Vector File: [svg](https://s3.amazonaws.com/splitflap-travis/branches/master/3d_laser_vector.svg)
(In order to get the design laser-cut from Ponoko, you'll need to copy all of the shapes from that file into one of [Ponoko's templates](http://www.ponoko.com/starter-kits/inkscape))

##### Animated gif #####
The design can be rendered to a rotating 3d animated gif (seen above) by running `3d/generate_gif.py`, which outputs to `3d/build/animation/animation.gif`

The `generate_gif.py` script runs multiple OpenSCAD instances in parallel to render the design from 360 degrees to individual png frames, which are then combined into the final gif animation. As part of building the animation, `generate_gif.py` renders the design with multiple configurations (opaque enclosure, see-through enclosure, no-enclosure and no flaps) by setting the `render_enclosure` and `render_flaps` variables.

##### STL models/web viewer #####
The design can be rendered to a series of STL files (one per color used in the model) in order to be displayed in an [interactive web-based 3d viewer](https://scottbez1.github.io/splitflap/). Similar to the `projection_renderer` used to render individual components for laser-cutting, the [ColoredStlExporter](https://github.com/scottbez1/splitflap/blob/master/3d/colored_stl_exporter.py) detects all the colors used in the model and renders them one-by-one to separate STL files, along with a manifest that maps each STL file to its RGB color. The STL files and manifest are loaded using three.js to display an interactive model on a web site using WebGL. See this blog post for more details on how the export and three.js renderer work: [OpenSCAD Rendering Tricks, Part 3: Web viewer](http://scottbezek.blogspot.com/2016/08/openscad-rendering-tricks-part-3-web.html).


### Driver Electronics ###
The driver board is designed to plug into an Arduino like a shield, and can control 4 stepper motors. Up to 3 driver boards can be chained together, for up to 12 modules controlled by a single Arduino. The designs for the controller can be found under `electronics/splitflap.pro` (KiCad project). Nearly everything is a through-hole component rather than SMD, so it's very easy to hand-solder.

The driver uses 2 MIC5842 low-side shift-register drivers, with built-in transient-suppression diodes, to control the motors, and a 74HC165 shift register to read from 4 optical home position sensors. There are optional WS2812B RGB LEDs which can be used to indicate the status of each of the 4 channels.

<a href="https://s3.amazonaws.com/splitflap-travis/branches/master/schematic.pdf">
<img height="320" src="https://s3.amazonaws.com/splitflap-travis/branches/master/schematic.png"/>
</a>

The PCB layout is 10cm x 5cm which makes it fairly cheap to produce using a low-cost PCB manufacturer (e.g. Seeed Studio), and can be populated in two separate configurations (since many low-cost PCB manufacturers have a minimum order of identical PCBs):

1. As a 4-channel driver board that plugs into an Arduino or chains to other driver boards
1. As a home sensor board for a single character, with GP2S60 IR reflectance sensor and 3-pin connector

This way, with an order of 5 identical PCBs you can populate a single 4-channel driver board and four home sensor boards for a complete electronics set for 4 split-flap units.

<a href="https://s3.amazonaws.com/splitflap-travis/branches/master/pcb_raster.png">
<img width="640" src="https://s3.amazonaws.com/splitflap-travis/branches/master/pcb_raster.png"/>
</a>

##### Latest PCB Renderings #####
These are automatically updated on every commit with the latest rendering from the `master` branch. See this blog post for more details on how that works: [Automated KiCad, OpenSCAD rendering using Travis CI](http://scottbezek.blogspot.com/2016/04/automated-kicad-openscad-rendering.html).

Latest (experimental!) PCB Gerbers: [zip](https://s3.amazonaws.com/splitflap-travis/branches/master/pcb_gerber.zip)
Latest (experimental!) PCB Packet: [pdf](https://s3.amazonaws.com/splitflap-travis/branches/master/pcb_packet.pdf)
Latest (experimental!) rough bill of materials: [csv](https://s3.amazonaws.com/splitflap-travis/branches/master/bom.csv)

#### Rendering ####
The PCB layout can be rendered to an svg or png (seen above) by running `electronics/generate_svg.py`. This uses KiCad's [python scripting API](https://github.com/blairbonnett-mirrors/kicad/blob/master/demos/python_scripts_examples/plot_board.py) to render several layers to individual svg files, manipulates them to apply color and opacity settings, and then merges them to a single svg. For additional details, see this blog post: [Scripting KiCad Pcbnew exports](http://scottbezek.blogspot.com/2016/04/scripting-kicad-pcbnew-exports.html).

For reviewing the design, a pdf packet with copper, silkscreen, and drill info can be produced by running `electronics/generate_pdf.py`.

Gerber files for fabrication can be exported by running `electronics/generate_gerber.py`. This generates gerber files and an Excellon drill file with Seeed Studio's [naming conventions](http://support.seeedstudio.com/knowledgebase/articles/422482-fusion-pcb-order-submission-guidelines) and produces a `.zip` which can be sent for fabrication.

EESchema isn't easily scriptable, so to export the schematic and bill of materials `electronics/scripts/export_schematic.py` and `export_bom.py` start an X Virtual Frame Buffer (Xvfb) and open the `eeschema` GUI within that virtual display, and then send a series of hardcoded key presses via `xdotool` to interact with the GUI and click through the dialogs. This is very fragile but seems to work ok for now. For additional details, see this blog post: [Using UI automation to export KiCad schematics](http://scottbezek.blogspot.com/2016/04/automated-kicad-schematic-export.html).

### Driver Firmware ###
The driver firmware is written using Arduino and is available at `arduino/splitflap/splitflap.ino`. 

The firmware currently runs a basic closed-loop controller that accepts letters over USB serial and drives the stepper motors using a precomputed acceleration ramp for smooth control. The firmware automatically calibrates the spool position at startup, using the IR reflectance sensor, and will automatically recalibrate itself if it ever detects that the spool position has gotten out of sync. If a commanded rotation is expected to bring the spool past the "home" position, it will confirm that the sensor is triggered neither too early nor too late; otherwise it will search for the "home" position to get in sync before continuing to the desired letter.

### Computer Control Software ###
There is currently no example computer software demonstrating how to communicate with the driver firmware over USB. This is planned for the future, but the protocol is currently undocumented and likely to change as the firmware continues to be developed. In the meantime, the best "documentation" of the protocol is the [firmware source code](https://github.com/scottbez1/splitflap/blob/master/arduino/splitflap/splitflap.ino) itself.

## License ##
I'd love to hear your thoughts and questions about this project, and happy to incorporate any feedback you might have into these designs! Please feel free (and encouraged) to [open GitHub issues](https://github.com/scottbez1/splitflap/issues/new), email me directly, reach out [on Twitter](https://twitter.com/scottbez1), and [get involved](https://github.com/scottbez1/splitflap/pulls) in the open source development and let's keep chatting and building together!

The vast majority of this project is licensed under Apache v2 (see [LICENSE.txt](LICENSE.txt) for full details).

    Copyright 2015-2016 Scott Bezek and the splitflap contributors
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
