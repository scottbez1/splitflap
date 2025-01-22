# Splitflap v2 Ordering guide
[<< Back to Documentation Index](../DocumentationIndex.md)

This is the “easy” way to order the parts for your splitflap display (United States only). It optimizes for *simplicity in ordering*, not the *lowest possible cost*.

If you’d like to review other options, which could be somewhat cheaper, check out the [full ordering doc](OrderingComplete.md), but if that’s a little overwhelming, you’re in the right place here!

If you have questions, please ask in the [community Discord server](https://discord.gg/Hxnftc8PyW)!


## Laser-cut mechanical parts

**Supplier:** [Elecrow](https://www.elecrow.com/acrylic-cutting.html) has reasonable quality at a decent price

**Material:** Acrylic

The v2 design officially supports either 52 flaps per module or 40 flaps per module. The 52-flap design is the default recommendation here and is perfect if you will be buying the pre-printed "Epilogue" flap sets, but you can check out the advanced ordering instructions for details on the 40 flap variant.

For reference, the front face of each module is approximately **82.6mm wide** and **143.53mm tall**.

- [ ] Use this file to order a 52-flap design, made of 3mm acrylic (Matte Black (P502) is recommended): [zip](https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-52-elecrow-3mm-acrylic_1x.zip)
    - Go to [Elecrow acrylic laser cutting](https://www.elecrow.com/acrylic-cutting.html) and upload the zip file
    - Enter dimensions: <img height="18" src="https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-52-elecrow-3mm-acrylic_1x_dimensions.svg" />
    - Select thickness: 3mm
    - Engrave: No
    - Acrylic color: Matte Black (P502) is recommended

Full ordering instructions, and alternative files for acrylic/mdf or for 40- rather than 52-flap modules can be found [here](OrderingComplete.md).

I’ve previously recommended ordering from Ponoko - and while I still believe they have great quality and customer service, they’ve gotten too expensive to be my default recommendation any longer 😞 


## Electronics

**Supplier:** [Bezek Labs](https://bezeklabs.etsy.com) (yes, I’m biased in recommending my own store, but it really is the easiest purchasing option since these are custom boards and I also include all necessary accessories). 


- [ ] [Chainlink Driver](https://bezeklabs.etsy.com/listing/1123280069/splitflap-chainlink-driver-v11) (1 for every 6 character modules)
    - Ribbon cable length: **45cm**
    - Add motors? **Yes, add 6x 28BYJ-48**
- [ ] [Sensor kit v2 (6x with magnets and headers)](https://bezeklabs.etsy.com/listing/1696745674/splitflap-sensor-kit-v2-beta-6x-with)  (1 for every 6 character modules)
    - Include cables? **YES**
- [ ] [Chainlink Buddy [T-Display]](https://bezeklabs.etsy.com/listing/1109357786/splitflap-chainlink-buddy-t-display) (1 for your *entire* display, up to 36 total character modules)
    - Include ESP32 T-Display Module: **YES**


## Flaps

**Supplier:** [Bezek Labs](https://bezeklabs.etsy.com) (again, I’m biased, but there aren’t any commercial alternatives and making them yourself requires a lot of [time and patience](../Flaps.md#option-3-diy-flaps))

**Two good options** (this tradeoff is totally up to you):

- [ ] [”Epilogue” pre-printed v2 flaps](https://bezeklabs.etsy.com/listing/1685633114/splitflap-epilogue-printed-flaps-beta-52) save a TON of time and look great, but are definitely more expensive. 6-packs and 24-packs are available at a small discount. Only available with white text on a black card.

OR

- [ ] You can buy blank flaps and [apply letter stickers to them yourself](https://www.youtube.com/watch?v=3lFECISLwyI)
    - [ ] [blank flaps](https://bezeklabs.etsy.com/listing/979720975/blank-splitflap-display-flaps) (purchase the 1x52 or 6x52 packs for a 52-flap design. The 6x packs are available at a small discount)
    - [ ] [vinyl letter stickers](https://amzn.to/37Frsjb) (from Amazon). Blank flaps are available in white or black, and vinyl letter stickers are [available](https://amzn.to/3ieAZj9) in [many](https://amzn.to/37t1y1R) different [colors](https://amzn.to/3tk4Ddh). (1 sticker pack for every **2** modules)


## Hardware

**Supplier:** McMaster-Carr (reliable, but somewhat pricey especially with shipping) or find these common items elsewhere


- [ ] M4 x 10mm **button-head**. 10 bolts needed per module (so you’ll need at least 60x for 6 modules). Must be button-head style (e.g. taller socket-cap bolts will likely cause clearance issues)
    - [McMaster-Carr](https://www.mcmaster.com/92095A190/) pack of 100
- [ ] M4 hex nuts. 10 nuts needed per module (so you’ll need at least 60x for 6 modules)
    - [McMaster-Carr](https://www.mcmaster.com/91828A231/) pack of 100


## Power supply and wiring

**Supplier:** many options, no specific recommendations

For 1-12 modules:

- [ ] 22AWG wire for 3.3v power (gauge is not critical as this is low-current; solid wire is preferable to stranded for use with screw terminals)
- [ ] 12V power supply capable of *at least* **1.5A for every 6 modules**, plus some margin. Searching for “12V CCTV” or LED power supplies is a good way to find supplies that will have the correct barrel jack connector to plug into the Chainlink Buddy [T-Display]. 
    - Assume that the current ratings on supplies sold by Amazon, AliExpress, etc are overstated, so make sure to account for this and select a supply with a higher current rating than needed.
- [ ] 20AWG (or larger diameter, depending on length & number of modules) wire for 12v power and ground

OR, for 13-36 modules:

- [ ] 22AWG wire for 3.3v power (gauge is not critical as this is low-current; solid wire is preferable to stranded for use with screw terminals)
- [ ] 12V power supply capable of *at least* **1.5A for every 6 modules**, plus some margin. *You must connect the power supply to the screw terminals of the first Chainlink Driver* ***directly***. Do not use the Chainlink Buddy’s barrel jack connector with more than 12 modules, as it cannot handle the current requirements!
    - Assume that the current ratings on supplies sold by Amazon, AliExpress, etc are overstated, so make sure to account for this and select a supply with a higher current rating than needed.
- [ ] 16AWG (or larger diameter, depending on length & number of modules) wire for 12v power and ground

OR, more than 36 modules:
> [!WARNING]
> Special considerations apply for displays with more than 36 modules! You must have a strong understanding of the limitations and risks involved with high-current DC power supplies, and should take all reasonable precautions to protect against damage/injury. Make sure to read the Chainlink Driver user guide for more info if you will be building a [large display](../ElectronicsGuide.md#Large-displays) with 36+ modules.


## That’s all!

For instructions on assembly, see the [Splitflap v2 Assembly Guide](Assembly.md) and [Chainlink Driver Electronics User Guide](../ElectronicsGuide.md) 



----------

Link disclosure: As an Amazon Associate I earn from qualifying purchases at no cost to you.