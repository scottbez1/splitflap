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
- [Contributing/Modifying](Documentation/Contribute.md)


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
