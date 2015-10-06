# DIY Split-Flap Display

This is a work in progress DIY [split-flap display](https://en.wikipedia.org/wiki/Split-flap_display).

![animated rendering](renders/animation.gif)

The goal is to make a low-cost display that's easy to fabricate at home (e.g. custom materials can be ordered from Ponoko or similar, and other hardware is generally available).

The 3d model is built using OpenSCAD in splitflap.scad.

### Design highlights ###
* laser cut MDF enclosure and mechanisms (material thickness is parameterized, so design can be adjusted to accomodate different materials)
* cheap, widely available 28byj-48 stepper motor (less expensive than NEMA-17 motors, and doesn't require an expensive high current stepper driver)
* CR80 PVC cards for flaps, cheap in bulk (still need to figure out fabrication of these, or switch to laser-cut plastic...)
* vinyl stickers for flap letters?

### TODO ###
* Enclosure
    * mounting holes?
    * interlocking mechanism?
    * etched timestamp/commit hash?
* Fabrication
    * 2d projection and panelizing for laser cutter
* Driver
    * need some kind of home-position sensor
        * IR Reflectance sensor near spool? QRE1113?

### Concerns ###
* 5mm rod friction fit - maybe need to design mount in wood that flexes to hold rod tightly (instead of requiring precise laser cut hole diameter)
