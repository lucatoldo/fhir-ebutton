# fhir-ebutton
This repository has the Fritzing sketches for the hardware and the .ino code for the software of the FHIReButton used in the https://devpost.com/software/icu-bed-availability-service #WIRVSVIRUS hackaton

# How to use it
## WIFI Connection
The FHIReButton needs WIFI connection. When it is shipped it cannot connect automatically to the internet. Therefore it needs to be given the SSID and Password to access the WIFI Access Point. If during the use of the button the access to the WIFI changes (or the button is moved into another WIFI area) then one has to put the DIPSWITCH number 4 to DOWN.
In both of these cases, the device will generate its own WIFI endpoint that can be accessed by any WIFI device (e.g. smartphone, tablet, laptop, PC) connecting (without password) to the access point having the name "FHIR-eButton-#########" where the symbols #### are the number of the chipID of the processor inside the device. This number is also printed on the box with which the device is shipped.
When connected to the WIFI endpoint of the FHIR-eButton-#### automatically a web page is shown, there one should click on the setup, and then in the 3 fields select which SSID to use, provide the password, and also provide the endpoint of the FHIR server to be used for storing the data. The default value is provided and points to the public FHIR server hapi.fhir.org
## Configuration
The button comes preconfigured for 7 specific kinds of "devices" according to the following table.
After changing the settings the device needs to be restarted in order to use them.
|  Device type | Codes | DIP1 | DIP2 | DIP3 |
| ----------- | ----------------------- | - | - | - |
| Bed                                  | 229772003 |1|1|1|
|Cardiac bed                          | 182590005  |0|0|1|
| High air loss bed                    | 182592002 |0|1|1|
| Hospital bed      + Maske Beatmung   | 182592002 + 425447009|0|1|1|
| High air loss bed + Tube Beatmung    | 182592002 + 467238001 |1|0|0|
| High air loss bed + Luftröhrenshnitt | 182592002 +   2248009|1|0|1|
|High air loss bed + ECMO             | 182592002 +      3965 |1|1|0|

## Normal operations
In normal operations, only the "main" status switch needs to be used.
In UP position it means that the medical device is in use, and a blue light is on.
In Down position it means that the medical device is not used, and a red light is on.

## What happens
When turning UP the main switch, the device is sending a POST request to the FHIR server informing it that the device is now occupied.

When turning Down the main switch the device is sending a POST request to the FHIR server informing that the device is now available for being used.

If for some reasons the device is no longer usable, then all DIPS should be moved in position 0 and the main in position down.
This sends a POST request to the FHIR server informing that the device does no longer exist.

## Thoughts for improvements
- allow provisioning of security certificate at configuration time (currently it is hardcoded in the .h file).
- allow customisation of the 7 possible "devices"
- change the format of the message (currently it is sending a Device resource, however perhaps a better approach would be to send a Bundle with a Location, a Device and a Provenance resource)
- enforce security on the button usage, for example by requiring a RFID tag to belayed over the device to ensure
that only authorised people (those with the RFID tag) can change the status of the devices
- add geolocation information dynamically (using WIFI information and also a GPS device) or statically (through user configuration)
- provide a configurable way to specify the output for use with non FHIR endpoints
- include a feedback loop to the user, if the POST message to the repository have been successful
- add audio feedback (e.g. beep)
- check energy level of the battery in case its below ,min requirement ..
