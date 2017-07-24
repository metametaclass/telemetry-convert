# Description #

This project aims to create a converter from any telemetry protocol into FrSky S.Port. This will enable devices and sensors written to use protocols such as Multiplex M-Link to be used with FrSky S.Port telemetry.

In addition this project aims to support live telemetry data from Unilog2 by SM-Modellbau.

The setup will be as follows:

FrSky S.Port Receiver <=====> telemetry\_converted\_hardware <========> foreign sensor/unilog2


Currently only Multiplex M-Link is supported as input. If you would like to extend the code please contact me and I will be happy to assist.

If you use the unilog2 please make sure to configure telmetry protocol as autodetect.

You can find a description of the FrSky S.Port protocol here: https://code.google.com/p/telemetry-convert/wiki/FrSkySPortProtocol

Converter board: [Teensy 3.0](http://www.pjrc.com/store/teensy3.html) (cheap and powerful, can be programmed in arduino environment)


# Getting started with Unilog2 telemetry conversion #

  * Get a [Teensy 3.0 board](http://www.pjrc.com/store/teensy3.html). In Germany you can get it from Watterott: http://www.watterott.com/de/Teensy-v30-MK20DX128

  * Set up you Teensy 3.0:
    * Install necessary drivers and code upload infrastructure: http://www.pjrc.com/teensy/first_use.html
    * extend you existing arduino environment for teensy, as described here: http://www.pjrc.com/teensy/teensyduino.html

  * Download code from tab source, compile and upload to Teensy

  * Connect your unilog2 to the Teensy:

https://googledrive.com/host/0B_Mu7qEifBKuOTIzdTVoNkJZakE/connect_unilog2.JPG

  * Configure your unilog2 to use telemetry autodetect and configure m-link addresses:

https://googledrive.com/host/0B_Mu7qEifBKuOTIzdTVoNkJZakE/setup_unilog2.JPG

  * You should now receive telemetry data from unilog2 on your Taranis. Verify by going to the Telemetry-Page.