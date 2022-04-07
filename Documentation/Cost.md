# Rough Cost Breakdown

* [Housing](#housing)
* [Hardware & Consumables](#hardware--consumables)
* [Flaps](#flaps)
* [PCB](#pcb)
* [Electronics & Motor](#electronics--motor)

This is an _incomplete_ list of supplies needed to build a split-flap display module to get a rough sense of the overall cost.
Many items have minimum package sizes, so per-unit-prices will be higher when building fewer units due to the need to
buy more items than will actually be used.

Each section includes a subtotal based on building _4 modules_, which is a relatively economical compromise where
shipping costs and MOQs no longer dominate the cost.

Overall, expect to spend somewhere around $200 to $250 for 4 standard 40-character modules.

## Housing

The laser-cut housing/mechanics can be ordered from Ponoko (high quality, US-based) or Elecrow. Or you can cut it yourself if
you have a laser-cutter capable of cutting 3mm MDF or acrylic.

### Ponoko

| Price  | Units per order | Name                               | Supplier | Notes |
|--------|-----------------|------------------------------------|----------|-------|
| $52.97 | 4               | 4x MDF 3 mm, laser-cut, shipping   | Ponoko   |       |
| $14    | N/A             | Setup fee                          | Ponoko   |       |

**Total:** $16.75 per unit (at 4 units)

### Elecrow
| Price | Units per order | Name                                   | Supplier | Notes |
|-------|-----------------|----------------------------------------|----------|-------|
| $15   | 5               | Wood laser cut, 3 mm, no engraving     | Elecrow  |       |
| $22   | 5               | Shipping                               | Elecrow  |       |

**Total:** $9.25 per unit (at 4 units)

## Hardware & Consumables

| Price | Units per order | Name                                | Supplier     | Notes                     |
|-------|-----------------|-------------------------------------|--------------|---------------------------|
| $5-10 | 4               | 50 pack M4-10 Button-Head Hex Screw | Many options | 11 screws needed per unit |
| $5-10 | 4               | 50 pack M4 Hex Nut                  | Many options | 11 nuts needed per unit   |

**Total:** $2.50-5.00 per unit (at 4 units)

## Flaps

There are two options for flaps: hand-cut or professionally die-cut.

### Hand-Cut (Option 1)

Each card needs to be cut in half and then have two pieces punched out of either side. Each display unit requires 40 flaps, or 20 cards per unit. 100 cards is enough for 5 units in theory, but 4 in practice because there are no spares for mistakes.

| Price | Units per order | Name                   | Supplier | Notes                            |
|-------|-----------------|------------------------|----------|----------------------------------|
| $14   | 4               | Blank white CR80 cards | Amazon   | One package ($14) covers 4 units |
| $9.17 | N/A             | Badge slot punch tool  | Amazon   | One-time, up front cost          |
| $7    | 2               | Vinyl letter stickers  | Amazon   | One package ($7) covers 2 units  |

**Total:** $7.00 per unit (at 4 units) + $9.17 upfront for the punch tool + a lot of time and effort

### Die-Cut (Option 2)

These professional die-cut flaps are manufactured specifically for this project (no manual cutting/punching required) and come in both white and matte black. Sold by Bezek Labs, so proceeds help support continued development on this project.

| Price  | Units per order | Name                  | Supplier                                                                                 | Notes                                       |
|--------|-----------------|-----------------------|------------------------------------------------------------------------------------------|---------------------------------------------|
| $53    | 4               | Die-cut flaps         | [Etsy - BezekLabs](https://www.etsy.com/listing/979720975/blank-splitflap-display-flaps) | One package ($45 + shipping) covers 4 units |
| $7     | 2               | Vinyl letter stickers | Amazon                                                                                   | One package ($7) covers 2 units             |

**Total:** $16.75 per unit (at 4 units)

## PCB

Circuit boards are available either in small quantities from the Bezek Labs store, or in bulk from PCB manufacturers such as SeeedStudio.

### Etsy - BezekLabs

| Price | Units per order | Name           | Supplier                                                                                         | Notes                    |
|-------|-----------------|----------------|--------------------------------------------------------------------------------------------------|--------------------------|
| $3.99 | 4               | Controller PCB | [Etsy - BezekLabs](https://www.etsy.com/listing/980030861/splitflap-classic-controller-pcb-bare) | One PCB supports 4 units |
| $2.99 | 4               | Sensor PCB set | [Etsy - BezekLabs](https://www.etsy.com/listing/966380990/splitflap-sensor-pcb-set-4x-bare)      | 4 PCBs per package       |

**Total:** $1.75 per unit (at 4 units)

### SeeedStudio (Bulk)

| Price | Units per order | Name           | Supplier    | Notes                              |
|-------|-----------------|----------------|-------------|------------------------------------|
| $5    | 80              | Controller PCB | SeeedStudio | 20 PCBs per order, 4 units per PCB |
| $5    | 200             | Sensor PCB     | SeeedStudio |                                    |
| $15   |                 | Shipping       |             | Combined shipping for both items   |

**Total:** $6.25 per unit (at 4 units), or as low as $0.32 per unit (at 80 units)

## Electronics & Motor

| Price  | Units per order | Name                             | Supplier                | Notes                                                                                  |
|--------|-----------------|----------------------------------|-------------------------|----------------------------------------------------------------------------------------|
| $14.00 | 4               | Controller board components      | DigiKey                 | One PCB supports 4 units                                                               |
| $2.00  | 1               | Sensor board components          | DigiKey                 |                                                                                        |
| $2.00  | 1               | 28byj-48 motor (12V preferred)   | AliExpress              | See [motor notes](https://github.com/scottbez1/splitflap/wiki/Motor-info) for details  |
| $10.00 | 10              | 3-pin servo cable (male to male) | Amazon or AliExpress    |                                                                                        |
| $11.00 | Up to 12        | 12V power supply                 | Amazon                  | One per display (up to 12 units)                                                       |

**Total:** $5.00 per unit (motor, sensor components, cable) + $14.00 per 4 units (controller components) + $11.00 per display (up to 12 units)

You can find a more detailed breakout of electronics' bill of materials (BOM) in [the ordering instructions](https://paper.dropbox.com/doc/Ordering-Splitflap-v0.5--ATdB3cgS5GyvwTWq1kpunkD4Ag-iCwwMo0hRkkAlwdutccWf).