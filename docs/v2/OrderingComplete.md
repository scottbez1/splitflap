# Splitflap v2 Comprehensive Ordering Guide
[<< Back to Documentation Index](../DocumentationIndex.md)

The instructions below will walk you through ordering all the parts (excluding common tools) needed to build a splitflap v2 display with **6 modules** using the latest Chainlink electronics.

Ready to assemble? Jump ahead to the [Assembly Guide](Assembly.md) 

If you have any questions, please feel free to reach out to me (scott@bezeklabs.com). Or, join the [Discord server](https://discord.gg/wgehm3PcrC), to discuss or ask questions from the wider split-flap community!


😩 Feeling overwhelmed by this document? I don’t blame you, there’s a LOT here… So I put together a much shorter document that favors simplicity over comprehensiveness/lowest-cost,
for people in the US: [+Splitflap Ordering (the “easy” route)](OrderingEasy.md) 


1. [Order laser-cut parts](#order-laser-cut-parts)
3. [Order electronics](#order-electronics)
2. [Order hardware](#order-misc-hardware)

----------
# Order laser-cut parts

There are many places you can get laser-cut parts, but I’ve personally used Ponoko and Elecrow for ordering shipped to the US with an easy online-only process. I’ve previously recommended ordering from Ponoko - and while I still believe they have great quality and customer service so you'll get a great product, they’ve gotten too expensive to be my default recommendation any longer 😞 

For advanced makers: if you want to cut a single front panel that will be shared by an entire row (or grid) of modules, there is a script to do so with lots of configurable options like spacing, rows/cols, and even has a mode for generating geometry for CNC routing with necessary “dog-bones” applied to inside corners — see `generate_combined_front_panel.py`

💡 Note: v2 laser-cut parts are not dimensionally compatible with older v0 modules! If you’re extending an older display, make sure to use the older files (see the v0 [ordering instruction](../v0/OrderingComplete.md))


## Elecrow (recommended)

- Download the appropriate file:
  - 52-flap design
    - 3mm acrylic
      - Download [zip](https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-52-elecrow-3mm-acrylic_1x.zip)
      - Go to [Elecrow acrylic laser cutting](https://www.elecrow.com/acrylic-cutting.html) and upload the zip file
      - Enter dimensions: 195.60 mm x 196.25 mm
      - Select thickness: 3mm
      - Engrave: No
      - Acrylic color: Matte Black (P502) is recommended
    - 3mm wood
      - Download [zip](https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-52-elecrow-3mm-wood_1x.zip)
      - Go to [Elecrow wood laser cutting](https://www.elecrow.com/5pcs-wood-laser-cutting-service.html)
      - Dimension: 20cm Max * 20cm Max(Thickness: 3mm)
  - 40-flap design
    - 3mm acrylic
      - Download [zip](https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-40-elecrow-3mm-acrylic_1x.zip)
      - Go to [Elecrow acrylic laser cutting](https://www.elecrow.com/acrylic-cutting.html) and upload the zip file
      - Enter dimensions: 179.36 mm x 176.77 mm
      - Select thickness: 3mm
      - Engrave: No
      - Acrylic color: Matte Black (P502) is recommended
    - 3mm wood
      - Download [zip](https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-40-elecrow-3mm-wood_1x.zip)
      - Go to [Elecrow wood laser cutting](https://www.elecrow.com/5pcs-wood-laser-cutting-service.html)
      - Dimension: 20cm Max * 20cm Max(Thickness: 3mm)

![](img/elecrow_acrylic.png)
![](https://paper-attachments.dropbox.com/s_175A71EF755D30C645040845CEAC36F09A2519808F9780643CBAC746263B696C_1592618387746_Screenshot+from+2020-06-19+18-59-27.png)

## Ponoko (also recommended, but pricey)
- Download the appropriate file
    - 52-flap design
        - 3mm Acrylic: [svg](https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-52-ponoko-3mm-acrylic_1x.svg) <img height="18" src="https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-52-ponoko-3mm-acrylic_1x_dimensions.svg" />
        - 3mm MDF: [svg](https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-52-ponoko-3mm-mdf_1x.svg) <img height="18" src="https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-52-ponoko-3mm-mdf_1x_dimensions.svg" />
    - 40-flap design
        - 3mm Acrylic: [svg](https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-40-ponoko-3mm-acrylic_1x.svg) <img height="18" src="https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-40-ponoko-3mm-acrylic_1x_dimensions.svg" />
        - 3mm MDF: [svg](https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-40-ponoko-3mm-mdf_1x.svg) <img height="18" src="https://s3.amazonaws.com/splitflap-artifacts/refs/tags/releases/mechanics/v2/3d/3d_laser_vector-40-ponoko-3mm-mdf_1x_dimensions.svg" />


- Go to [Ponoko](https://www.ponoko.com/) and sign up for an account.
- [Upload](https://www.ponoko.com/designs) the SVG file from above. Confirm the dimensions match the values above, and check that blue lines are set for “Cutting” and black fill is set for “Area Engraving” (or turn this off to skip engraved features)
![](https://paper-attachments.dropbox.com/s_175A71EF755D30C645040845CEAC36F09A2519808F9780643CBAC746263B696C_1590128918128_Screenshot+from+2020-05-21+23-28-11.png)

- Select a material - [MDF](https://www.ponoko.com/materials/mdf-fiberboard) or [Acrylic](https://www.ponoko.com/materials/black-matte-acrylic) are recommended — choose 3.0mm (0.12 inches) thickness. Make sure to check the material’s maximum size against the dimensions provided next to the file links above; some materials may be too small to order the 4x versions.

💡 **Important note if ordering Matte 1-Side acrylic at Ponoko:** you ***must*** immediately send special instructions to Ponoko after placing your order, requesting to have the design engraved on the *glossy* side. This will result in the design being correctly cut so the matte surface will be on the exterior of the display when assembled. From Ponoko support (as of 2021-05-13): 

> The matte is [by default] cut with the matte side up (so engraving happens on this side as well).
> 
> If you would like to place an order and have us engrave on the glossy side, just **reply to the order confirmation page** with your request and we'll be sure to make note in the order details.

----------
# Order electronics
Coming soon.

# Order misc hardware
Coming soon.


----------
# Assemble

Ready to assemble your display? See the [complete assembly instructions document](Assembly.md).
