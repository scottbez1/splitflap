# 28BYJ-48 Motor Buying Guide
[<< Back to Documentation Index](DocumentationIndex.md)

There are a *lot* of different motors that go by the same “28BYJ-48” name.

If you want to be certain you're getting good motors (and you're in the US), you can opt to add motors to your purchase of a [Chainlink Driver](https://bezeklabs.etsy.com/listing/1123280069/splitflap-chainlink-driver) from the [Bezek Labs](https://bezeklabs.etsy.com) store.
I qualify each batch of motors I receive so you can be sure they'll work great (or I'll replace/refund them).

### Latest AliExpress recommendation (as of 2024-06-25)

As of 2024-06-25, I recommend SeasBoat Store as a fairly reliable AliExpress seller over the last few years, having ordered many hundreds of motors from them myself (but as always, they may send you different motors, so it’s no guarantee).

They have 2 listings available:
- a [“Choice” listing](https://s.click.aliexpress.com/e/_mKVNenq) that is more expensive individually, but qualifies for free shipping over $10 (or use this [non-affiliate link](https://www.aliexpress.us/item/2255800976774653.html)). This may be chepaer for a smaller quantity of motors.
- a [non-Choice listing](https://s.click.aliexpress.com/e/_m0W7bZq) with a lower per-unit cost but with a shipping charge (or use this [non-affiliate link](https://www.aliexpress.us/item/3256807022196320.html)). This is likely cheaper for larger orders.

Make sure you select the “12V” option when adding to your cart!

Depending how many motors you’re ordering and where you are shipping to, you may want to use one listing or the other.

## What's different about different "28BYJ-48" motors?

The two areas of variation are:

- gear ratio
    - common values are 16.032:1, 64:1, and 63.68395:1
- coil resistance
    - I’ve seen variants ranging from 26 ohms to 185 ohms

(For more background reading, see [Graham Wideman’s detailed post](http://web.archive.org/web/20180528105141/http://grahamwideman.wikispaces.com/Motors-+28BYJ-48+Stepper+motor+notes), though it does not even cover all the variants I have seen)

Both of those factors - gear ratio and coil resistance - have a pretty big impact on output torque, max speed, and control sequence (steps per revolution), but unfortunately most sellers don’t provide that info, or worse, they’ll often provide **incorrect info**.

Making things worse, it’s not uncommon for sellers on AliExpress to change the item they sell without changing the store listing, so even if you’ve purchased from a listing in the past, there’s no guarantee they will continue to offer the same items. This has happened to me with a seller I used previously and no longer recommend.

🔑  The motors that I’ve found work best for splitflap displays have a 64:1 gear ratio and a half-coil-resistance (measured from the red wire to the blue wire) of ~100 ohms.


----------

In my experience, I’ve found these tips improve your chances of getting good motors:


- 🔑 **Only buy motors labeled 12V**
    - It seems like there are more variants marketed as “5V” so you’re less likely to get a consistent item when buying 5V motors
- 🔑 **Only buy motors that have laser-engraved labels; avoid motors with sticker labels**
    - Laser-engraving is more expensive than stickers, so it generally suggests a higher quality product. I’ve received sticker motors with *identical* “serial” numbers printed on the sticker, whereas the numbers on laser-engraved motors have always been unique in my experience
- 🔑 **Message the seller beforehand to confirm the motors are as you expect**
    - Ask whether the motors have a sticker or laser-engraved numbers on the side 
    - You can try asking what the resistance is, e.g. between the red wire and blue wire, but I’ve had little luck with this in the past (maybe just lost in translation)
    - Asking about steps/rev or gear ratio is generally not useful; they will give you an answer but it may not be correct because they can’t easily test it
