# fhir-ebutton
This repository has the Fritzing sketches for the hardware and the .ino code for the software of the FHIReButton used in the https://devpost.com/software/icu-bed-availability-service #WIRVSVIRUS hackaton

# How to use it
## WIFI Connection
The FHIReButton needs WIFI connection. When it is shipped it cannot connect automatically to the internet. Therefore it needs to be given the SSID and Password to access the WIFI Access Point. If during the use of the button the access to the WIFI changes (or the button is moved into another WIFI area) then one has to put the DIPSWITCH number 4 to DOWN.
In both of these cases, the device will generate its own WIFI endpoint that can be accessed by any WIFI device (e.g. smartphone, tablet, laptop, PC) connecting (without password) to the access point having the name "FHIR-eButton-#########" where the symbols #### are the number of the chipID of the processor inside the device. This number is also printed on the box with which the device is shipped.
When connected to the WIFI endpoint of the FHIR-eButton-#### automatically a web page is shown, there one should click on the setup, and then in the 3 fields select which SSID to use, provide the password, and also provide the endpoint of the FHIR server to be used for storing the data. The default value is provided and points to the public FHIR server hapi.fhir.org
## Configuration
The button comes preconfigured for 8 specific kinds of "devices":
|  Device type | Codes |
| ----------- | ----------------------- |
| Bed                                  | 229772003 |
| Hospital bed                         |  91537007 |
|Cardiac bed                          | 182590005  |
| High air loss bed                    | 182592002 |
| Hospital bed      + Maske Beatmung   | 182592002 + 425447009|
| High air loss bed + Tube Beatmung    | 182592002 + 467238001 |
| High air loss bed + Luftröhrenshnitt | 182592002 +   2248009|
|High air loss bed + ECMO             | 182592002 +      3965 |
