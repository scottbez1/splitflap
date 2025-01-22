# Splitflap v0 Ordering (the “easy” route)
[<< Back to Documentation Index](../DocumentationIndex.md)

This is the “easy” way to order the parts for your splitflap display (United States only). It optimizes for *simplicity in ordering*, not the *lowest possible cost*.

If you’d like to review other options, which could be somewhat cheaper, check out the [full ordering doc](OrderingComplete.md), but if that’s a little overwhelming, you’re in the right place here!

If you have questions, ask in the [community Discord server](https://discord.gg/Hxnftc8PyW)!


## Laser-cut mechanical parts

**Supplier:** [Ponoko](https://www.ponoko.com/) is recommended, though pricey.

**Material:** MDF is more forgiving than Acrylic during assembly (Acrylic can be brittle and crack when fitting pieces together), though admittedly doesn't look as nice. For less potential frustration, I generally recommend MDF.

For reference, the front face of each module is approximately **82.6mm wide** and **147.8mm tall**.


- [ ]  Use this file for each module, made of 3mm MDF: https://github.com/scottbez1/splitflap/releases/download/v0.7/3d_laser_vector-ponoko-3mm-mdf_1x.svg [177.42mm x 173.37mm] 
    Full ordering instructions, and alternative files for acrylic are [here](OrderingComplete.md).


## Electronics

**Supplier:** [Bezek Labs](https://bezeklabs.etsy.com) (yes, I’m biased in recommending my own store, but it really is the easiest purchasing option since these are custom boards and I also include all necessary accessories)


- [ ]  [Chainlink Driver v1.1](https://bezeklabs.etsy.com/listing/1123280069/splitflap-chainlink-driver-v11) (1 for every 6 modules)
    - Ribbon cable length: **45cm**
    - Add motors? **Yes, add 6x 28BYJ-48**
- [ ]  [Sensor kit (6x, with sensors, magnets, and headers)](https://bezeklabs.etsy.com/listing/1139795321/splitflap-sensor-kit-6x-with-sensors)  (1 for every 6 modules)
    - Include cables? **YES**
- [ ]  [Chainlink Buddy [T-Display]](https://bezeklabs.etsy.com/listing/1109357786/splitflap-chainlink-buddy-t-display) (1 for your *entire* display, up to 36 total modules)
    - Include ESP32 T-Display Module: **YES**


## Flaps

**Supplier:** [Bezek Labs](https://bezeklabs.etsy.com) (again, I’m biased, but there aren’t any commercial alternatives and making them yourself requires a lot of [time and patience](../Flaps.md#option-3-diy-flaps))

**Two good options** (this tradeoff is totally up to you):

- [ ]  [~~Pre-printed flaps~~](https://bezeklabs.etsy.com/listing/1119149706/splitflap-display-flaps-printed-last) ~~save a TON of time and look great, but are more expensive. 6-packs and 24-packs are available at a small discount. Only available with white text on a black card.~~
    - **Note (2023-12-12): I am sold out of my initial batch of printed flaps**. I am working on an updated design, expected to launch in early 2024. If you are waiting for these new printed flaps, I would also recommend holding off on ordering the laser-cut parts for now too; the updated design will come alongside a refreshed mechanical design to support 52 flaps per module rather than 40 flaps per module.

OR

- [ ]  You can buy blank flaps and [apply letter stickers to them yourself](https://www.youtube.com/watch?v=3lFECISLwyI)
    - [ ]  [blank flaps](https://bezeklabs.etsy.com/listing/979720975/blank-splitflap-display-flaps) (sold in packs of 160 flaps for **4 modules**, or 240 flaps for **6 modules**)
    - [ ]  [vinyl letter stickers](https://amzn.to/37Frsjb) (from Amazon). Blank flaps are available in white or black, and vinyl letter stickers are [available](https://amzn.to/3ieAZj9) in [many](https://amzn.to/37t1y1R) different [colors](https://amzn.to/3tk4Ddh). (1 sticker pack for every **2** modules)


## Hardware

**Supplier:** McMaster-Carr (reliable, but somewhat pricey especially with shipping) or find these common items elsewhere


- [ ]  M4 x 10mm **button-head**. 11 bolts needed per module (so you’ll need at least 66x for 6 modules). Must be button-head style (e.g. taller socket-cap bolts will likely cause clearance issues)
    - [McMaster-Carr](https://www.mcmaster.com/92095A190/) pack of 100
- [ ]  M4 hex nuts. 11 nuts needed per module (so you’ll need at least 66x for 6 modules)
    - [McMaster-Carr](https://www.mcmaster.com/91828A231/) pack of 100


## Power supply and wiring

**Supplier:** many options

For 1-12 modules:

- [ ]  22AWG wire for 3.3v power (gauge is not critical as this is low-current; solid wire is preferable to stranded for use with screw terminals)
- [ ]  12V power supply capable of *at least* **1.5A for every 6 modules**, plus some margin. Searching for “12V CCTV” power supplies is a good way to find supplies that will have the correct barrel jack connector to plug into the Chainlink Buddy [T-Display].
- [ ]  20AWG (or larger diameter, depending on length & number of modules) wire for 12v power and ground

OR, for 13-36 modules:

- [ ]  22AWG wire for 3.3v power (gauge is not critical as this is low-current; solid wire is preferable to stranded for use with screw terminals)
- [ ]  12V power supply capable of *at least* **1.5A for every 6 modules**, plus some margin. *You must connect the power supply to the screw terminals of the first Chainlink Driver* ***directly***. Do not use the Chainlink Buddy’s barrel jack connector with more than 12 modules, as it cannot handle the current requirements!
- [ ]  16AWG (or larger diameter, depending on length & number of modules) wire for 12v power and ground

OR, more than 36 modules:
> [!WARNING]
> Special considerations apply for displays with more than 36 modules! You must have a strong understanding of the limitations and risks involved with high-current DC power supplies, and should take all reasonable precautions to protect against damage/injury. Make sure to read the Chainlink Driver user guide for more info if you will be building a [large display](../ElectronicsGuide.md#Large-displays) with 36+ modules.


## That’s all!

For instructions on assembly and setup of the Chainlink electronics, see the [Chainlink Driver Electronics User Guide](../ElectronicsGuide.md) 


----------

Link disclosure: As an Amazon Associate I earn from qualifying purchases at no cost to you.