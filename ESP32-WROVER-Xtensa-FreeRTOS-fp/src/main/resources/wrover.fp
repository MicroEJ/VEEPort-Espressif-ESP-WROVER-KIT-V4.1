<?xml version="1.0"?>
<frontpanel 
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xmlns="https://developer.microej.com" 
	xsi:schemaLocation="https://developer.microej.com .widget.xsd">
	
	<device name="ESP32-WROVER" skin="Board.jpg">
			<ej.fp.widget.Display x="63" y="92" width="320" height="240"/>
			<ej.fp.widget.LED label="0" x="38" y="355" ledOff="LedOff.png" ledOn="RedLed.png"/>
			<!-- <ej.fp.widget.LED label="0" x="33" y="355" ledOff="LedOff.png" ledOn="BlueLed.png"/>  --> <!-- The Blue LED of the RGB matrix is not available as it is used by the custom UART logger -->
			<!-- <ej.fp.widget.LED label="1" x="38" y="355" ledOff="LedOff.png" ledOn="RedLed.png"/>   -->
			<!-- <ej.fp.widget.LED label="2" x="43" y="355" ledOff="LedOff.png" ledOn="GreenLed.png"/> --> <!-- The Green LED of the RGB matrix is not available as it is used by the MicroSD -->
	</device>
</frontpanel>