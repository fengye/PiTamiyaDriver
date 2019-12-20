# A Raspberry Pi driver for Tamiya ESC TEU-105BK

----
## Why
Allow a Raspberry Pi to drive Tamiya RC car using official ESC. This could be a good start for self-driving car modding.

## Hardware
* A working Raspberry Pi 2/3/4
* Tamiya RC car with ESC TEU-105BK. I had [this one](https://www.amazon.co.jp/gp/product/B00AH48YNY/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1) with [this radio controller set](https://www.amazon.co.jp/gp/product/B0077K0WM2/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1)
* A sufficient power bank or some capable buck converter that can tolerant 7.2v and convert to 5V to Raspberry Pi, like [this one](https://www.pololu.com/product/3782)
* 
* Charging cable if using power bank
* Breadboard cables
* Logic level converter like [this one](https://www.sparkfun.com/products/12009). This is needed because Pi outputs 3.3V PWM signals which is insufficient to notify Tamiya ESC. 3.3V PWM is fine for Tamiya servo if you power it with 5V though.

----
## Video
[![Drive Tamiya Servo with Raspberry Pi](https://i.ibb.co/Br2LP79/https-i-ytimg-com-vi-9u24-HN4k-N-c-maxresdefault.jpg)](https://youtu.be/9u24HN4kN-c "Drive Tamiya Servo with Raspberry Pi")
[![Drive Tamiya ESC/motor with Raspberry Pi](https://i.ibb.co/5kKKD51/https-i-ytimg-com-vi-0-KVZfvp-Ii-S0-maxresdefault.jpg)](https://youtu.be/0KVZfvpIiS0 "Drive Tamiya ESC/motor with Raspberry Pi")

