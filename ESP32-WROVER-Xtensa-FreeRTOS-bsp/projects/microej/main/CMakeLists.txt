#
# Makefile
#
# Copyright 2020-2023 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.
#

set(srcs 

    "microej.c"

    "../platform/lib/microejapp.S"

    "../bluetooth/src/bt_helper_read.c"
    "../bluetooth/src/bt_helper_write.c"
    "../bluetooth/src/bt_manager.c"
    "../bluetooth/src/bt_pump.c"
    "../bluetooth/src/LLBLUETOOTH_impl.c"

    "../core/src/LLBSP_esp32.c"
    "../core/src/LLDEVICE.c"
    "../core/src/LLMJVM_FreeRTOS.c"
    "../core/src/microej_time_freertos.c"
    "../core/src/microej_main.c"

    "../ecom-comm/src/comms.c"
    "../ecom-comm/src/LLCOMM_UART_impl.c"

    "../ecom-network/src/LLECOM_NETWORK_impl.c"
    "../ecom-network/src/ecom_network_helper_ESP32.c"

    "../ecom-wifi/src/LLECOM_WIFI_impl.c"
    "../ecom-wifi/src/WIFI_ESP32_driver.c"
    "../ecom-wifi/src/ecom_wifi_helper_ESP32.c"

    "../espressif/src/com_espressif_esp_idf_esp_ota_ops.c"
    "../espressif/src/com_espressif_esp_idf_esp_system.c"
    "../espressif/src/com_espressif_esp_idf_nvs.c"
	
    "../fs/src/fs_helper_fatfs.c"
    "../fs/src/LLFS_ESP32_init_sdcard.c"
    "../fs/src/LLFS_ESP32_init_spiflash.c"
    "../fs/src/LLFS_File_impl.c"
    "../fs/src/LLFS_impl.c"
	
    "../hal/src/LLHAL_GPIO.c"
	
    "../kf/src/LLKERNEL_SDRAM.c"
	
    "../microej-util/src/interrupts.c"

    "../net/src/async_select_cache.c"
    "../net/src/async_select_osal.c"
    "../net/src/async_select.c"
    "../net/src/LLNET_CHANNEL_bsd.c"
    "../net/src/LLNET_Common.c"
    "../net/src/LLNET_DATAGRAMSOCKETCHANNEL_bsd.c"
    "../net/src/LLNET_DNS_native_impl.c"

    "../net/src/LLNET_MULTICASTSOCKETCHANNEL_bsd.c"
    "../net/src/LLNET_NETWORKADDRESS_bsd.c"
    "../net/src/LLNET_NETWORKINTERFACE_lwip.c"
    "../net/src/LLNET_SOCKETCHANNEL_bsd.c"
    "../net/src/LLNET_STREAMSOCKETCHANNEL_bsd.c"
    "../net/src/lwip_util.c"

    "../security/src/LLSEC_CIPHER_impl.c"
    "../security/src/LLSEC_DIGEST_impl.c"
    "../security/src/LLSEC_KEY_FACTORY_impl.c"
    "../security/src/LLSEC_KEY_PAIR_GENERATOR_impl.c"
    "../security/src/LLSEC_MAC_impl.c"
    "../security/src/LLSEC_PRIVATE_KEY_impl.c"
    "../security/src/LLSEC_PUBLIC_KEY_impl.c"
    "../security/src/LLSEC_RANDOM_impl.c"
    "../security/src/LLSEC_SIG_impl.c"
    "../security/src/LLSEC_X509_CERT_impl.c"

    "../ssl/src/LLNET_SSL_CONTEXT_impl.c"
    "../ssl/src/LLNET_SSL_ERRORS.c"
    "../ssl/src/LLNET_SSL_SOCKET_impl.c"
    "../ssl/src/LLNET_SSL_utils_mbedtls.c"
    "../ssl/src/LLNET_SSL_verifyCallback.c"
	
    "../uart/src/uart_switch.c"
	
    "../ui/src/framerate.c"
    "../ui/src/framerate_impl_FreeRTOS.c"
    "../ui/src/LLDW_PAINTER_impl.c"
    "../ui/src/LLUI_DISPLAY.c"
    "../ui/src/LLUI_INPUT.c"
    "../ui/src/LLUI_LED_impl.c"
    "../ui/src/LLUI_PAINTER_impl.c"
    "../ui/src/LLUI_DISPLAY_HEAP_impl.c"
    "../ui/src/LLUI_INPUT_LOG_impl.c"
    "../ui/src/microui_event_decoder.c"
	
    "../util/src/microej_allocator.c"
    "../util/src/microej_async_worker.c"
    "../util/src/osal_FreeRTOS.c"
    "../util/src/microej_pool.c"
    "../watchdog-timer/src/LLWATCHDOG_TIMER_impl.c"
    "../watchdog-timer/src/watchdog_timer_checkpoint_freertos.c"
    "../watchdog-timer/src/watchdog_timer_helper.c"

    "../thirdparty/libwebp/src/dec/alpha_dec.c"
    "../thirdparty/libwebp/src/dec/buffer_dec.c"
    "../thirdparty/libwebp/src/dec/frame_dec.c"
    "../thirdparty/libwebp/src/dec/idec_dec.c"
    "../thirdparty/libwebp/src/dec/io_dec.c"
    "../thirdparty/libwebp/src/dec/quant_dec.c"
    "../thirdparty/libwebp/src/dec/tree_dec.c"
    "../thirdparty/libwebp/src/dec/vp8_dec.c"
    "../thirdparty/libwebp/src/dec/vp8l_dec.c"
    "../thirdparty/libwebp/src/dec/webp_dec.c"
    "../thirdparty/libwebp/src/dsp/alpha_processing.c"
    "../thirdparty/libwebp/src/dsp/cost.c"
    "../thirdparty/libwebp/src/dsp/cpu.c"
    "../thirdparty/libwebp/src/dsp/dec.c"
    "../thirdparty/libwebp/src/dsp/dec_clip_tables.c"
    "../thirdparty/libwebp/src/dsp/enc.c"
    "../thirdparty/libwebp/src/dsp/filters.c"
    "../thirdparty/libwebp/src/dsp/lossless.c"
    "../thirdparty/libwebp/src/dsp/lossless_enc.c"
    "../thirdparty/libwebp/src/dsp/rescaler.c"
    "../thirdparty/libwebp/src/dsp/ssim.c"
    "../thirdparty/libwebp/src/dsp/upsampling.c"
    "../thirdparty/libwebp/src/dsp/yuv.c"
    "../thirdparty/libwebp/src/microej/microej_decode.c"
    "../thirdparty/libwebp/src/microej/microej_utils.c"
    "../thirdparty/libwebp/src/utils/bit_reader_utils.c"
    "../thirdparty/libwebp/src/utils/bit_writer_utils.c"
    "../thirdparty/libwebp/src/utils/color_cache_utils.c"
    "../thirdparty/libwebp/src/utils/filters_utils.c"
    "../thirdparty/libwebp/src/utils/huffman_encode_utils.c"
    "../thirdparty/libwebp/src/utils/huffman_utils.c"
    "../thirdparty/libwebp/src/utils/quant_levels_dec_utils.c"
    "../thirdparty/libwebp/src/utils/quant_levels_utils.c"
    "../thirdparty/libwebp/src/utils/random_utils.c"
    "../thirdparty/libwebp/src/utils/rescaler_utils.c"
    "../thirdparty/libwebp/src/utils/thread_utils.c"
    "../thirdparty/libwebp/src/utils/utils.c")

set(include_dirs 
    "../bluetooth/inc"
    "../core/inc"
    "../ecom-comm/inc"
    "../ecom-network/inc"
    "../ecom-wifi/inc"
    "../espressif/inc"
    "../fs/inc"
    "../microej-util/inc"
    "../net/inc"
    "../platform/inc"
    "../security/inc"
    "../ssl/inc"
    "../trace/inc"
    "../uart/inc"
    "../ui/inc"
    "../thirdparty/libwebp"
    "${IDF_PATH}/components/freertos/include/freertos"
    "../util/inc"
    "../watchdog-timer/inc")

if(CONFIG_SYSVIEW_ENABLE)
    list(APPEND srcs 
    "../trace/src/LLMJVM_MONITOR_sysview.c" 
    "../trace/src/LLTRACE_sysview.c")

    list(APPEND include_dirs 
    "../trace/inc")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs})

# Added to ignore the casting between incompatible native function prototypes and SNI_closeFunction prototype.
set(compile_flags 
    "-Wno-cast-function-type")

# Added to ignore the unused label "start_sysview_logging" used to start 
# SysView logging.
set_source_files_properties(microej.c PROPERTIES COMPILE_FLAGS -Wno-unused-label)

target_compile_options(${COMPONENT_LIB} PRIVATE ${compile_flags})

add_prebuilt_library(microejruntime "../platform/lib/microejruntime.a")
add_prebuilt_library(microejapp "../platform/lib/microejapp.o")

add_custom_command(TARGET ${COMPONENT_LIB} PRE_LINK
				   COMMAND python ${CMAKE_CURRENT_LIST_DIR}/rename-sections.py ${CMAKE_OBJCOPY} ${CMAKE_CURRENT_LIST_DIR}/../platform/lib/microejapp.o)

target_link_libraries(${COMPONENT_LIB} PUBLIC microejruntime microejapp)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--start-group" CACHE INTERNAL "-Wl,--end-group" FORCE)
