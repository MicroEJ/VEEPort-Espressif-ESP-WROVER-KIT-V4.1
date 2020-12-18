..
	Copyright 2020 MicroEJ Corp. All rights reserved.
    For demonstration purpose only.
    MicroEJ Corp. PROPRIETARY. Use is subject to license terms.

========================
 Update esp-idf process.
========================

Description
===========

- Remove the actual esp-idf:

	- ``git rm -r ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf``
	- Commit this modification.

- Change the working directory:

	-``cd ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/``

- Clone the new esp-idf using the recommended process:

	- ``git clone -b v3.3.1 --recursive https://github.com/espressif/esp-idf.git esp-idf``
	- where ``v3.3.1`` is the branch tag related to the esp-idf version

- Remove all .git files and directories:

	-  ``rm -r esp-idf/.git esp-idf/.github esp-idf/components/asio/asio/.git esp-idf/components/aws_iot/aws-iot-device-sdk-embedded-C/.git esp-idf/components/bt/lib/.git esp-idf/components/coap/libcoap/.git esp-idf/components/esp32/lib/.git esp-idf/components/esptool_py/esptool/.git esp-idf/components/expat/expat/.git esp-idf/components/json/cJSON/.git esp-idf/components/libsodium/libsodium/.git esp-idf/components/lwip/lwip/.git esp-idf/components/mbedtls/mbedtls/.git esp-idf/components/micro-ecc/micro-ecc/.git esp-idf/components/mqtt/esp-mqtt/.git esp-idf/components/nghttp/nghttp2/.git esp-idf/components/nimble/nimble/.git esp-idf/components/protobuf-c/protobuf-c/.git esp-idf/components/spiffs/spiffs/.git esp-idf/components/unity/unity/.git esp-idf/components/nghttp/nghttp2/third-party/mruby/.git esp-idf/components/nghttp/nghttp2/third-party/neverbleed/.git esp-idf/examples/build_system/cmake/import_lib/main/lib/tinyxml2/.git``

- Change the working directory to ``P0065_ESP32-WROVER-Platform`` and add the new idf:

	- ``git add -f ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf``

- Commit and push the new esp-idf

	- if needed, update the ``.gh.keep_binary`` file

- Add MicroEJ updates on top of the esp-idf

	- apply the esp-idf.patch ``git apply ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf.patch``
	- commit and push them

- Patch was created using the following process:

	- git diff between relevant commits to obtain > esp-idf.patch (esp-idf.patch that contain only relevant updates added by MicroEJ on top of esp-idf)