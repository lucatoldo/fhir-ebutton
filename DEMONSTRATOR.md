# Demonstration 
- device is without power
- ensure all DIP are in "down" position
- turn UP the DIP in position 1 and in position 4
this means that the button will be used for a High AIr Loss bed with Tube respiration (low ICU)
- ensure the STATUS switch is in "down" position
- insert batteries in the device
The Yellow light should blink and remain up. This shows that it needs to connect to WIFI.
If it is the first time or if there are problems connecting with the WIFI then a WIFI zone will appear called FHIReButton-xxxxxxxx where
the xxxxx is the ID of the FHIReButton. User should connect to it and then connect to the address 192.168.4.1
There select the SSID and provide the password required to connect. Also can specify the address of the FHIR server to store the data.
- The yellow light should disapper and instead a RED light should light.
This means the button is not used but ready .
- Check the content of the FHIR databse:
using browser or POSTMAN do a GET http://hapi.fhir.org/baseR4/Device?manufacturer=FHIReButton.
This should show that no resources are present.
- turn UP the STATUS switch
the blue light should turn on, the red light turn off.
using browser or POSTMAN do a GET http://hapi.fhir.org/baseR4/Device?manufacturer=FHIReButton.will return 1 record
with the exact information of the device, and also the digital signature: that is an encrypted version of a signature of the 
document content. It can be verified for validity using the public key of the button.
- turn DOWN the STATUS switch
the blu light should turn down, the red light turn up.
using browser or POSTMAN do a GET http://hapi.fhir.org/baseR4/Device?manufacturer=FHIReButton.will return 2 records, however checking for the identifiers of the button will see that it is the same.
What changes between the two is the signature since the document contains the status "available" and before was "used".

