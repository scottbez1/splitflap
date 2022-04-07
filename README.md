# DIY Split-Flap Display

This is a DIY [split-flap display](https://en.wikipedia.org/wiki/Split-flap_display).
Prototype four-character display: [video](https://www.youtube.com/watch?v=vq4o_88kN8g).

![animated rendering](https://s3.amazonaws.com/splitflap-artifacts/master/3d/3d_animation.gif)
[![prototype video](renders/prototypeVideoThumbnail.jpg)](https://www.youtube.com/watch?v=vq4o_88kN8g)

[![Build Status](https://github.com/scottbez1/splitflap/actions/workflows/3d.yml/badge.svg?branch=master)](https://github.com/scottbez1/splitflap/actions/workflows/3d.yml)
[![Build Status](https://github.com/scottbez1/splitflap/actions/workflows/electronics.yml/badge.svg?branch=master)](https://github.com/scottbez1/splitflap/actions/workflows/electronics.yml)
[![Build Status](https://github.com/scottbez1/splitflap/actions/workflows/pio.yml/badge.svg?branch=master)](https://github.com/scottbez1/splitflap/actions/workflows/pio.yml)

The goal is to make a low-cost display that's easy to fabricate at home in small/single quantities (e.g. custom materials can be ordered from Ponoko or similar, and other hardware is generally available).

You can view an interactive 3d model of the design on the [project website](https://scottbez1.github.io/splitflap/).

The [splitflap community Slack group](https://join.slack.com/t/splitflap/shared_invite/zt-dpvol87b-3zUaxXrUd8WauPXr1uBj5Q) is the best place to keep up with the latest changes or ask questions about the project!

Want to help support development or just say "thanks"? Consider a one-time or monthly sponsorship:

| [:heart: Sponsor scottbez1 on GitHub](https://github.com/sponsors/scottbez1) |
|---|

[![Video: how a split-flap display works](renders/howItWorksThumbnail.jpg)](https://www.youtube.com/watch?v=UAQJJAQSg_g)

**Using this project in a commercial setting or for paid client work?** Go right ahead - it's open source (just make sure to follow the terms of the Apache License)! I would, however, ask that you consider [sponsoring the project](https://github.com/sponsors/scottbez1). I've been developing and maintaining this project in my free time for over 6 years, and I'd love to continue working on it. Sponsorships allow me to pay for prototypes and development tools that make this project possible. Unlike pure software projects, every iteration has real hardware costs; sponsorships allow me to keep iterating and improving the project faster. Thank you!


# Current Status
[You can download the **latest stable release** from the official 'releases' page.](https://github.com/scottbez1/splitflap/releases)

Releases have been tested and used to produce working units, but as this is a continuously evolving open-source project, there may always be minor issues and/or incomplete documentation from time to time.

| Component | Status | Notes |
| --- | --- | --- |
| Enclosure/Mechanics | *Stable* | |
| Classic Controller Electronics | *Stable* | |
| Firmware | *Stable* | |
| Control Software Example | *Stable* | Example python code for driving the display is in the [software](software) directory|


:bulb: There's a new electronics system, called the *Chainlink* system (more on this [below](#chainlink-electronics)), which makes assembly easier and supports larger displays (100+ modules), using an ESP32 for the controller. It's a stable design and will become the recommended electronics in the near future (Q1 2022), but documentation is still in progress so you may hit more bumps in the road for now.

Here's a video of a large 108-module display powered by 18 Chainlink Driver boards and a Chainlink Base:

[![Video: animations on 108-module display](https://raw.githubusercontent.com/wiki/scottbez1/splitflap/images/animationsThumb.gif)](https://youtu.be/g9EPabcxBsM)

I'd love to hear your thoughts and questions about this project, and happy to incorporate any feedback you might have into these designs! Please feel free (and encouraged) to [open GitHub issues](https://github.com/scottbez1/splitflap/issues/new), email me directly, reach out [on Twitter](https://twitter.com/scottbez1), and [get involved](https://github.com/scottbez1/splitflap/pulls) in the open source development and let's keep chatting and building together!

# Build Your Own

* [**Ordering guide (v0.5/0.6/0.7)**](https://paper.dropbox.com/doc/Ordering-Splitflap-v0.5--AS8OCZ~75DLuHBGHhxn94YAMAg-iCwwMo0hRkkAlwdutccWf)
* [**Latest stable release**](https://github.com/scottbez1/splitflap/releases)
* [**Assembly instructions (v0.5/0.6/0.7)**](https://paper.dropbox.com/doc/Splitflap-Kit-v0.5v1.0-Instructions--ASQDu1uoa6n4_t1pva1bM1MgAg-bxXJlke5ROmamcx4OH44r)

# Table of Contents
- [Rough Cost Breakdown](Documentation/Cost.md)
- [Design Overview](Documentation/Design.md)
- [Contributing/Modifying](#contributingmodifying)
  * [3D Design](#3d-design)
  * [Electronics Design](#electronics-design)



# Contributing/Modifying

Looking to make some modifications or play around with the design on your local machine? Jump right in! Note that all of the scripts and automation are developed for Ubuntu. Mac OS support is planned,
but not currently implemented (but feel free to open a PR if you want to help!).

## 3D Design

The main design file is [`3d/splitflap.scad`](3d/splitflap.scad)

You'll need a recent version of OpenSCAD (e.g. 2015-03), which may need to be installed through the PPA:
`sudo add-apt-repository ppa:openscad/releases`

In general, solid objects such as the enclosure sides or spool components are built from 2d primitives and then extruded to the appropriate thickness for 3d rendering, rather than using 3d primitives. This simplifies the design without losing expressiveness; the perpendicular laser cut beam doesn't allow for cuts that vary in the Z dimension anyway.

Note that while the design is parameterized and many values may be tweaked, there is currently no error checking for invalid parameters or combinations of parameters. Please take care to validate the design if you change any parameters. For instance, while most of the design will adjust to a changed `num_modules` value, certain values may cause some elements to intersect with other elements or protrude beyond their expected dimensions.

### Rendering
#### Laser-cut vector files
The design can be rendered to 2d for laser cutting by running [`3d/scripts/generate_2d.py [--panelize <number>]`](3d/scripts/generate_2d.py), which outputs to `3d/build/laser_parts/combined.svg`. The optional `--panelize` argument allows for rendering a panel of modules in a single SVG, for bulk laser-cutting.

Internally, the design uses a `projection_renderer` module ([`3d/projection_renderer.scad`](3d/projection_renderer.scad)), which takes a list of child elements to render, and depending on the `render_index` renders a single child at a time. It also _adds_ material to each shape to account for the kerf that will be cut away by the laser.

The [`generate_2d.py`](3d/scripts/generate_2d.py) script interacts with the `projection_renderer` module by first using it to determine the number of subcomponents to render, then runs OpenSCAD to export each component to an SVG file. It does some post-processing on the SVG output (notably adds "mm" to the document dimensions), and then combines all components into the single `combined.svg` output.

Once the `combined.svg` file is generated, you'll want to double-check there aren't any redundant cut lines that are shared by multiple adjacent pieces, to save time/cost when cutting. They should be detected automatically (and highlighted in red in the rendering above), but it doesn't hurt to double-check. In Inkscape, select the "Edit paths by nodes" tool and select an edge to delete - the endpoints should turn blue. Then click "Delete segment between two non-endpoint nodes", and repeat this for all other redundant cut lines.

#### Animated gif
The design can be rendered to a rotating 3d animated gif (seen above) by running [`3d/scripts/generate_gif.py`](3d/scripts/generate_gif.py), which outputs to `3d/build/animation/animation.gif`

The `generate_gif.py` script runs multiple OpenSCAD instances in parallel to render the design from 360 degrees to individual png frames, which are then combined into the final gif animation. As part of building the animation, `generate_gif.py` renders the design with multiple configurations (opaque enclosure, see-through enclosure, no-enclosure and no flaps) by setting the `render_enclosure` and `render_flaps` variables.

#### STL models/web viewer
The design can be rendered to a series of STL files (one per color used in the model) in order to be displayed in an [interactive web-based 3d viewer](https://scottbez1.github.io/splitflap/). Similar to the `projection_renderer` used to render individual components for laser-cutting, the [ColoredStlExporter](3d/scripts/colored_stl_exporter.py) detects all the colors used in the model and renders them one-by-one to separate STL files, along with a manifest that maps each STL file to its RGB color. The STL files and manifest are loaded using three.js to display an interactive model on a web site using WebGL. See this blog post for more details on how the export and three.js renderer work: [OpenSCAD Rendering Tricks, Part 3: Web viewer](http://scottbezek.blogspot.com/2016/08/openscad-rendering-tricks-part-3-web.html).


## Electronics Design
All of the electronics are developed using KiCad 5. Panelization is provided by [KiKit](https://github.com/yaqwsx/KiKit) and gerber/BOM generation is provided by [KiBot](https://github.com/INTI-CMNB/KiBot).

### Rendering
The mechanical and electrical design renderings and links above are automatically updated on every commit with the latest rendering. See this blog post for more details on how that works: [Automated KiCad, OpenSCAD rendering using Travis CI](http://scottbezek.blogspot.com/2016/04/automated-kicad-openscad-rendering.html).

The PCB layout can be rendered to an svg or png (seen above) by running [`electronics/scripts/generate_svg.py file.kicad_pcb`](electronics/scripts/generate_svg.py).
This uses KiCad's [Python scripting API](https://docs.kicad-pcb.org/doxygen/md_Documentation_development_pcbnew-plugins.html)
to render several layers to individual svg files, manipulates them to apply color and opacity settings, and then merges them to a single svg.
For additional details, see this blog post: [Scripting KiCad Pcbnew exports](http://scottbezek.blogspot.com/2016/04/scripting-kicad-pcbnew-exports.html).

For reviewing the design, a pdf packet with copper, silkscreen, and drill info can be produced by running [`electronics/scripts/generate_pdf.py file.kicad_pcb`](electronics/scripts/generate_pdf.py).

Gerber files for fabrication can be exported by running [`electronics/scripts/generate_gerber.py file.kicad_pcb`](electronics/scripts/generate_gerber.py).
This generates gerber files and an Excellon drill file with Seeed Studio's [naming conventions](http://support.seeedstudio.com/knowledgebase/articles/1176532-how-to-generate-the-gerber-manufacturing-files) and produces a `.zip` which can be sent for fabrication.

EESchema isn't easily scriptable, so to export the schematic [`electronics/scripts/export_schematic.py`](electronics/scripts/export_schematic.py) starts an X Virtual Frame Buffer (Xvfb) and open the `eeschema` GUI within that virtual display, and then send a series of hardcoded key presses via `xdotool` to interact with the GUI and click through the dialogs. This is very fragile but seems to work ok for now. For additional details, see this blog post: [Using UI automation to export KiCad schematics](http://scottbezek.blogspot.com/2016/04/automated-kicad-schematic-export.html).



# License
I'd love to hear your thoughts and questions about this project, and happy to incorporate any feedback you might have into these designs! Please feel free (and encouraged) to [open GitHub issues](https://github.com/scottbez1/splitflap/issues/new), email me directly, reach out [on Twitter](https://twitter.com/scottbez1), and [get involved](https://github.com/scottbez1/splitflap/pulls) in the open source development and let's keep chatting and building together!

This project is licensed under Apache v2 (see [LICENSE.txt](LICENSE.txt)).

    Copyright 2015-2021 Scott Bezek and the splitflap contributors
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
