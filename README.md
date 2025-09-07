# FEIT SL100-60/RGBTW/AG (BK7231N) ESPHome Conversion

This documents the steps taken to convert FEIT Electric's 'Smart Outdoor Home Lights'. After several days of watching YouTube videos, waiting for Amazon, Discord threads, sweaty palms, and a LOT of trial-and-error, I got them flashed with ESPHome, added to Home Assistant, and impressed the girl. (That last one being the most important in the end.)

# TLDR;
  There isn't one. I *guess* you could just copy/paste the header and ESPHome YAML and have the single RGB effect I included for reference -- but for those who are interested in how I got to this point, read. the. whole. thing.  

## Products used in this project 


[FEIT SL100-60/RGBTW/AG](https://www.amazon.com/Feit-Electric-WiFi-Enabled-Weatherproof-RGBTW/dp/B0DKZHD295) 
I picked 2 of these up from Walmart in the clearance aisle for significantly less than what they are selling for on Amazon. The price was literally too good to say no. I also ended up picking up a few [FEIT SL50-36/RGB/AG](https://www.amazon.com/Feit-Electric-WiFi-Enabled-Weatherproof-RGBTW/dp/B0CKKLD2N8) as well, not initially noticing that they were not tunable white (-TW), only RGB. Again, clearance aisle in Walmart -- I *had* to .

[BDM Frame](https://www.amazon.com/dp/B0C9RLYHG7)
[USB to TTL adapter](https://www.amazon.com/dp/B00LODGRV8)
I chose to use a BDM frame and probe pens in lieu of soldering directly on the board, and since I didnt have a USB TTL adapter, I needed to buy one. I *did* purchase extra [probe pens](https://www.amazon.com/dp/B0D4TWT9C9) and [dupont wires](https://www.amazon.com/dp/B01EV70C78), but didn't end up needing them like I'd thought; so I'll just keep them in case I need them for a future endeavour with something else.


## First things first
Sometimes, I'll see devices on the shelf I'll try to Google the brand and see if I can find any information on them, sometimes passing on the purchase because I can't, or there just isnt enough certainty they'll work in HA. I don't like -correction- I **can't stand** having a seperate 'phone 'app' for every damn thing I buy that's "smart". Just app to control all the smart stuff. So when I can, I'll try get something that I already have an Integration for in HA, which in this case, ended up being [Local Tuya](https://github.com/xZetsubou/hass-localtuya). 

I didnt actually know that these things were Tuya-based when I bought them. I found out after I had some time to tinker. What a pleasant surprise! Having local control of these lights will be AWESOME.

## Local Tuya

I was able to get the device added to Smart Life. Cool. That means I can get it into Local Tuya. Yep. Light entity created. OK - so lets see if we can add an Effect to send to the lights with the integration. Hmm. Houston, we have a problem.

Relevant [tinytuya](https://github.com/jasonacox/tinytuya) output: 
```
    {
        "name": "FEIT LEDs",
        "id": <redacted>,
        "key": <redacted>,
        "mac": <redacted>,
        "uuid": <redacted>,
        "sn": <redacted>,
        "category": "dc",
        "product_name": "Outdoor Home Lights",
        "product_id": "ymdcywmakafqt42s",
        "biz_type": 18,
        "model": "",
        "sub": false,
        "icon": "https://images.tuyaus.com/smart/icon/default_icon/ay1517349698958w8yNe/33f50a2c33f6dbbc39e587871a47b1ce.png",
        "mapping": {
            "20": {
                "code": "switch_led",
                "type": "Boolean",
                "values": {}
            },
            "21": {
                "code": "work_mode",
                "type": "Enum",
                "values": {
                    "range": [
                        "white",
                        "colour",
                        "scene",
                        "music"
                    ]
                }
            },
            "22": {
                "code": "bright_value",
                "type": "Integer",
                "values": {
                    "min": 10,
                    "max": 1000,
                    "scale": 0,
                    "step": 1
                }
            },
            "23": {
                "code": "temp_value",
                "type": "Integer",
                "values": {
                    "min": 0,
                    "max": 1000,
                    "scale": 0,
                    "step": 1
                }
            },
            "24": {
                "code": "colour_data",
                "type": "Json",
                "raw_values": "{\"h\":{\"min\":0,\"scale\":0,\"unit\":\"\",\"max\":360,\"step\":1},\"s\":{\"min\":0,\"scale\":0,\"unit\":\"\",\"max\":1000,\"step\":1},\"v\":{\"min\":0,\"scale\":0,\"unit\":\"\>
                "values": {
                    "h": {
                        "min": 0,
                        "scale": 0,
                        "unit": "",
                        "max": 360,
                        "step": 1
                    },
                    "s": {
                        "min": 0,
                        "scale": 0,
                        "unit": "",
                        "max": 1000,
                        "step": 1
                    },
                    "v": {
                        "min": 0,
                        "scale": 0,
                        "unit": "",
                        "max": 1000,
                        "step": 1
                    }
                }
            },
            "26": {
                "code": "countdown",
                "type": "Integer",
                "values": {
                    "unit": "s",
                    "min": 0,
                    "max": 86400,
                    "scale": 0,
                    "step": 1
                }
            },
            "27": {
                "code": "music_data",
                "type": "String",
                "values": "{\"maxlen\":255}"
            },
            "28": {
                "code": "control_data",
                "type": "String",
                "values": "{\"maxlen\":255}"
            }
        },
        "ip": "192.168.1.10",
        "version": "3.5"
    },
```
Where's the DPID for setting an effect (scene)? UGH!

## Down the rabbit hole..

Okay. So I can't create a custom effect from within the Local Tuya configuration options. That's not gonna work for me. So, what are my options...

- Leave it as is, and use the Smart Life App to enable effects on the lights and use HA for the color wheel and brightness control. (gag)
- I've found posts about OTA flashing to different firmware to be compatible with HA. Being that these LEDS are individually addressable, I think this will be better. However, I've never heard of OpenBeken, and I would rather not use MQTT for control. It works fine, but I'd rather use something more friendly ... Hmmm... like ESPHome

So - Option 2 it is. But where do I start? I've never done this before. Google don't fail me now!

Step 1.? Open the controller box. Figure out what's inside... specifically. Ah! a board called CBU, and under the RF Shield was a BK7231N


## Discord, Google and YouTube
My initial questions on Discord didnt get much traction, so I put Google to work while I waited for anyone to (possibly) give more insight into this. I found links to [tuya-cloudcutter](https://github.com/tuya-cloudcutter/tuya-cloudcutter), which unfortunately didn't work for this device. There were SO many articles on this, that and the other... *MAN*, this is **BS!** OpenBeken? the hell is that? - Then, i found [this](https://www.youtube.com/watch?v=t0o8nMbqOSA) video. Hey! Thats the same module **\*I\*** have! Just not the same device... *and* he got ESP on it. That's it! Watched it again. And again. (And again). This is when I decided I would have to buy the BDM frame and other necessary equipment. I had to have watched this video 100 times over waiting for my stuff to come in from Amazon.


## Headaches, Frustration, and Windows (eww.)

I had the damnedest time getting ltchiptool to install on my laptop that is running Mint. I dunno why, and at this point I really don't care anymore. WxPython and dependencies and.. I digress. I ended up erasing the bootloader (0x0) via commandline (the gui would crash, so...) and could not for the life of me figure out how to fix it. I ended up downloading and running the [OpenBK7321T_app](https://github.com/openshwprojects/OpenBK7231T_App) on a seperate laptop with Windows and was able to get OpenBeken installed on the chip, with the bootloader (thankfully)... buuuut OpenBeken isnt ESP. Not want I wanted. 

So I have OpenBeken on the chip. Back to Discord... and Google. 
"I can put OpenBeken on it, but what does that actually accomplish?" - "If that's the chip, you could probably put esphome on it." Ahh, the hope refreshed! BUT -- I have to get ESPHome on it now.  

Insert [this](https://www.coreforge.com/blog/2023/12/almost-bricking-bk7231n-modules/) article. Welp! That explains it -- I flashed the esphome firmware to the wrong address. ("Dumbass.") So, I Google some more,  download ESPHome firmware, and flashed it over the USB UART. OK! Got it! ... But where's my device? I don't see it on the Wifi, and I can't find it pulling a DHCP reservation. Move the probe pins to the logging UART pinouts, and see it's not connecting to WiFi -- because its not configured. *sigh.* "Dumbass." So, I *can* get ESPHome on it, but theres more to this ... still. I'm going to bed.

## ESPHome Dashboard ... and more Google-fu
 Ok. This is becoming a PITA. I have ESPHome on it, no WiFi, but I've slept. Time to Google. I find [this](https://esphome.io/components/libretiny/) page. Then [this](https://docs.libretiny.eu/boards/cbu/#usage). Wait. THIS! THIS IS IT! HA! IT'S OVER! 

Create a skeleton YAML:
```
esphome:
  name: feit-100-leds
  platform: bk72XX
  board: cpu

wifi:
  ssid: !secret  wifi_ssid
  password: !secret  wifi_password

# Optional fallback hotspot
ap:
  ssid: "Fallback Hotspot"
  password: "Fallback_Password"

logger:

api:

ota:
```
SEND IT!

I'll be damned if it didn't work! Time to figure out the rest. I was able to make some of the LEDs turn on with commands in OpenBeken, so .... lets use that! ("Dumbass.")

No.

## ChatGPT.. Sometimes not the smartest idea.

The OpenBeken used the SM16703P driver. So I went to ChatGPT to create the light entity YAML using it. If you didnt know, Garbage In, Garbage out. Final verdict from ChatGPT: "You cannot directly drive SM16703P LEDs with ESPHome on a BK7231 using standard platforms.

Back to Discord... and an unexpected response.
- thread: What is this from?
- Me: ChatGPT. I don't use it unless I am tring to find 'good' information from the web as a whole 
- thread:  Perhaps you should reconsider what "good" means.
[https://esphome.io/components/light/beken_spi_led_strip/](https://esphome.io/components/light/beken_spi_led_strip/)

... "Dumbass."

I had figured out with the OpenBeken firmware that the 'even' index (0,2,4, etc.) controlled the RGB LEDs. The 'odd' (1,3,5, etc.), being the tunable white LEDs. 

So how to I use ESPHome to differentiate? It was suggested in the thread to use partitions: "One terrible idea would be to create a partition using every other LED from the string". Google. Whoah. That'd be 20 partitions per single 'set' of 10 LEDs. What a mess! "I did say it was a terrible solution, but it should work. 🙂". I decided to switch to the RGB-only set to get a better understanding of what exactly I was trying to do. 

I can't say for certain how or what I came across specifically that suggested a custom header for effects. Probably ChatGPT. (I had previously tried my hand with another proect using ESPHome that used lambdas to do 'effects' on a small strip of 5 LEDs and it was all hard-coded with ESPHome.) I asked it to create an effect I could drop into the YAML, but after iteration after iteration with no success, somehow it came up with the solution to create custom header with c++ code, and gave the corresponding YAML use in ESPHome that added it to the Effects dropdown on the light entity. I'll be damned if it didn't work! And so, the saga continued.

## ChatGPT. How I hate you sometimes.

More effects. More success. They work- on the RGB-only LEDs. Now it's time to tackle the tunable whites. I mentioned before how the odd/even indeces controlled the rgb vs tw aspect. so I had to figure out how to get my headers to do the same. Easy, right? Not for someone who doesnt do this type of thing very often. 

Wrong prompts, bad code. Trial. Error. Repeat. Again and again. 
- I have a rubber duck toy that sits on my desk. I tell you this, because I suggest you get one. Ask it questions. It'll answer. Start from scratch with something simple. THEN extend it. "Dumbass." 

Okay. Simple. Turn 1 single LED on (RGB). \[OK.\] Now, turn the next one on (TW). \[Both LEDs light up\]. No. JUST the second one. \[OK.\] Now adjust brightness. \[All the LEDs adjust.\] *sigh* "Ok duck. What am I missing?" Ahh. the TW leds use RGB too, -BUT- they apparently only use RG values, B is unused. change THOSE values, it'll work.

Success.

## Effects, with the even (RGB) indeces

I grab my trusty working Rainbow Effect header and tell chatGPT to fix it so that it only uses the even indeces for the effect...... It worked! HA! YAAAY! -wait- the first bulb is out... what happened? Duck? (zero is an even number, dumbass. Include the zero index.) GOT 'EM! 

## 'Effects', with the odd (TW) indeces
I put that in quotes because the way I am using the strip doesnt *really* contstitute calling it and effect. It's either on or off. Adjustable brightness, and nothing fancy. Who needs that? I do not.
