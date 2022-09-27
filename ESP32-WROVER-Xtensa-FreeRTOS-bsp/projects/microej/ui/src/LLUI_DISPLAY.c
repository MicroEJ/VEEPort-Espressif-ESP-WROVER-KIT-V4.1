/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include <stdbool.h>
#include "LLUI_DISPLAY_configuration.h"
#include "LLUI_DISPLAY_impl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "microej.h"
#include "bsp_util.h"
#include "framerate.h"

#include "sni.h"

#include "esp_system.h"
#include "esp_attr.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "src/microej/microej_decode.h"

/* Set the default orientation mode as LANDSCAPE. Comment to use the PORTRAIT mode*/
#define DEFAULT_ORIENTATION_MODE_LANDSCAPE

/* Set the correct configuration for ESP32-WROVER-KIT v3 */
#define DEFAULT_TFT_DISPLAY_WIDTH   240
#define DEFAULT_TFT_DISPLAY_HEIGHT  320
#define DISP_COLOR_BITS_16          0x55
#define DEFAULT_GAMMA_CURVE         0

/* LCD pin connection */
#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22
#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL  5

/* Backlight pin settings */
#define PIN_BCKL_ON   0
#define PIN_BCKL_OFF  1

/* Clock configuration */
#define LCD_INIT_SPI_CLOCK          26000000
#define ST7789V_HIGH_SPI_CLOCK      40000000
#define ILI9341_HIGH_SPI_CLOCK      40000000

/* Common commands */
#define TFT_RDDID            0x04
#define TFT_SLPOUT           0x11
#define TFT_GAMSET           0x26
#define TFT_DISPON           0x29
#define TFT_CASET            0x2A
#define TFT_RASET            0x2B
#define TFT_RAMWR            0x2C
#define TFT_MADCTL           0x36
#define TFT_COLMOD           0x3A
#define TFT_WRMEMC           0x3C
#define TFT_WRDISBV          0x51
#define TFT_PVGAMCTRL        0xE0
#define TFT_NVGAMCTRL        0xE1

/* ST7789V commands */
#define ST7789V_TFT_RAMCTRL  0xB0
#define ST7789V_TFT_PORCTRL  0xB2
#define ST7789V_TFT_GCTRL    0xB7
#define ST7789V_TFT_VCOMS    0xBB
#define ST7789V_TFT_LCMCTRL  0xC0
#define ST7789V_TFT_VDVVRHEN 0xC2
#define ST7789V_TFT_VRHS     0xC3
#define ST7789V_TFT_VDVS     0xC4
#define ST7789V_TFT_FRCTRL2  0xC6
#define ST7789V_TFT_PWCTRL1  0xD0
#define ST7789V_TFT_CMD2EN   0xDF

/* ILI9341 commands */
#define ILI9341_TFT_FRMCTR1  0xB1
#define ILI9341_TFT_DISCTRL  0xB6
#define ILI9341_TFT_ETMOD    0xB7
#define ILI9341_TFT_PWCTRL1  0xC0
#define ILI9341_TFT_PWCTRL2  0xC1
#define ILI9341_TFT_VMCTRL1  0xC5
#define ILI9341_TFT_VMCTRL2  0xC7
#define ILI9341_TFT_RDID4    0xD3
#define ILI9341_TFT_IFCTL    0xF6

/* RGB-BGR setting */
#define ST7789V_TFT_RGB_BGR  0x00
#define ILI9341_TFT_RGB_BGR  0x08

/* Bit-fields of the MADCTL register */
#define MADCTL_MY            0x80
#define MADCTL_MX            0x40
#define MADCTL_MV            0x20

/* Delay after sending a command */
#define TFT_CMD_DELAY        0x80

/* LCD type */
typedef enum {
    LCD_ST7789V,
    LCD_ILI9341,
    LCD_MAX_TYPE
} lcd_type_t;

/* Screen orientation */
typedef enum {
    PORTRAIT,
    LANDSCAPE,
    PORTRAIT_FLIP,
    LANDSCAPE_FLIP
} lcd_orientation_mode_t;

/* Rotation type */
typedef enum {
    TFT_INVERT_ROTATION,
    TFT_INVERT_ROTATION1,
    TFT_INVERT_ROTATION2,
    TFT_NO_INVERT_ROTATION
} lcd_rotation_t;

/* Rotation configuration */
#define ST7789V_TFT_ROTATION TFT_INVERT_ROTATION1
#define ILI9341_TFT_ROTATION TFT_NO_INVERT_ROTATION

/* Define size to allocate for Display buffer. (320 * 240 * 2) x 1 : 1 for BACK_BUFFER */
#define DISPLAY_BUFFER_SIZE	0x25800

/* Declare MicroEJ Display buffer */
static uint8_t EXT_RAM_ATTR display_buffer[DISPLAY_BUFFER_SIZE] = {0};

/* Define the address of the back buffer */
#define BACK_BUFFER   ((int32_t)(&display_buffer[0]))

/* LCD transfer task defines */
#define LCD_TRANSFER_STACK_SIZE       (4 * 1024)
#define LCD_TRANSFER_TASK_PRIORITY    (9)
#define LCD_TRANSFER_TASK_STACK_SIZE  (LCD_TRANSFER_STACK_SIZE / 4)
#define LCD_TRANSFER_TASK_NAME        "LCD Transfer"

/* LCD update region structure */
typedef struct {
    uint8_t* srcAddr;
    uint32_t xmin;
    uint32_t ymin;
    uint32_t xmax;
    uint32_t ymax;
} lcd_region_t;

/* Initialization sequence for ST7789V LCD panel */
DRAM_ATTR static const uint8_t ST7789V_init[] = {
    18,                                                           // 18 commands in list
    ST7789V_TFT_CMD2EN,     4, 0x5A, 0x69, 0x02, 0x01,            // Command 2 Enable
    ST7789V_TFT_PORCTRL,    5, 0x0C, 0x0C, 0x00, 0x33, 0x33,      // Porch Setting
    ST7789V_TFT_GCTRL,      1, 0x45,                              // Gate Control
    ST7789V_TFT_VCOMS,      1, 0x2B,                              // VCOMS Setting
    ST7789V_TFT_LCMCTRL,    1, 0x2C,                              // LCM Control
    ST7789V_TFT_VDVVRHEN,   2, 0x01, 0xFF,                        // VDV and VRH Command Enable
    ST7789V_TFT_VRHS,       1, 0x11,                              // VRH Set
    ST7789V_TFT_VDVS,       1, 0x20,                              // VDV Set
    ST7789V_TFT_FRCTRL2,    1, 0x0F,                              // Frame Rate Control in Normal Mode
    ST7789V_TFT_PWCTRL1,    2, 0xA4, 0xA1,                        // Power Control 1
    TFT_PVGAMCTRL,         14, 0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43, 0x47, 0x09, 0x15, 0x12, 0x16, 0x19, // Positive Voltage Gamma Control
    TFT_NVGAMCTRL,         14, 0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44, 0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19, // Negative Voltage Gamma Control
    ST7789V_TFT_RAMCTRL,    2, 0x00, 0xF8,                        // RAM Control
    TFT_MADCTL,             1, (MADCTL_MX | ST7789V_TFT_RGB_BGR), // Memory Data Access Control
    TFT_COLMOD,             1, DISP_COLOR_BITS_16,                // Interface Pixel Format
    TFT_WRDISBV,            1, 0xFF,                              // Write Display Brightness
    TFT_SLPOUT,             TFT_CMD_DELAY, 120,                   // Sleep Out
    TFT_DISPON,             TFT_CMD_DELAY, 120,                   // Display On
};

/* Initialization sequence for ILI9341 LCD panel */
DRAM_ATTR static const uint8_t ILI9341_init[] = {
    18,                                                           // 18 commands in list
    ILI9341_TFT_VMCTRL1,    2, 0x35, 0x3E,                        // VCOM Control 1
    ILI9341_TFT_VMCTRL2,    1, 0xBE,                              // VCOM Control 2
    ILI9341_TFT_ETMOD,      1, 0x07,                              // Entry Mode Set
    ILI9341_TFT_DISCTRL,    4, 0x0A, 0x82, 0x27, 0x00,            // Display Function Control
    ILI9341_TFT_FRMCTR1,    2, 0x00, 0x1B,                        // Frame Rate Control (In Normal Mode / Full colors)
    ILI9341_TFT_PWCTRL1,    1, 0x26,                              // Power Control 1
    ILI9341_TFT_PWCTRL2,    1, 0x11,                              // Power Control 2
    TFT_PVGAMCTRL,         15, 0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00, // Positive Gamma Control
    TFT_NVGAMCTRL,         15, 0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F, // Negative Gamma Correction
    ILI9341_TFT_IFCTL,      3, 0x01, 0x30, 0x20,                  // 16bits Data Format Selection
    TFT_MADCTL,             1, (MADCTL_MX | ILI9341_TFT_RGB_BGR), // Memory Access Control
    TFT_COLMOD,             1, DISP_COLOR_BITS_16,                // Pixel Format Set
    TFT_WRDISBV,            1, 0xFF,                              // Write Display Brightness
    TFT_CASET,              4, 0x00, 0x00, 0x00, 0xEF,            // Column Address Set
    TFT_RASET,              4, 0x00, 0x00, 0x01, 0x3F,            // Row Address Set
    TFT_RAMWR,              0,                                    // Memory Write
    TFT_SLPOUT,             TFT_CMD_DELAY, 5,                     // Sleep Out
    TFT_DISPON,             TFT_CMD_DELAY, 5,                     // Display On
};

/* LCD type */
static lcd_type_t lcd_type = LCD_MAX_TYPE;

/*
 * LCD display operation type
 * - if little endian, the frame can be transfered as received from the MicroUI
 * - if big endian, the frame received from MicroUI has to be converted before being transfered
 */
static bool lcd_little_endian = true;

/* SPI device handle */
static spi_device_handle_t spi_handle;

/* LCD update region */
static lcd_region_t region;

/* Synchronization semaphores */
static SemaphoreHandle_t transfer_sem;

/* LCD width and height */
static int32_t width = DEFAULT_TFT_DISPLAY_WIDTH;
static int32_t height = DEFAULT_TFT_DISPLAY_HEIGHT;

/* Transaction descriptors */
static spi_transaction_t trans[6];

/*
 * Send a command to the LCD. Uses spi_device_transmit, which waits until
 * the transfer is complete.
 */
static void lcd_cmd(spi_device_handle_t spi, const uint8_t cmd) {
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));            // Zero out the transaction
    t.length = 8;                        // Command is 8 bits
    t.tx_buffer = &cmd;                  // The data is the cmd itself
    t.user = (void *)0;                  // D/C needs to be set to 0
    spi_device_transmit(spi, &t);        // Transmit!
}

/*
 * Send data to the LCD. Uses spi_device_transmit, which waits until
 * the transfer is complete.
 */
static void lcd_data(spi_device_handle_t spi, const uint8_t *data, int len) {
    spi_transaction_t t;

    if (len == 0) return;                // no need to send anything

    memset(&t, 0, sizeof(t));            // Zero out the transaction
    t.length = len * 8;                  // len is in bytes, transaction length is in bits.
    t.tx_buffer = data;                  // Data
    t.user = (void *)1;                  // D/C needs to be set to 1
    spi_device_transmit(spi, &t);        // Transmit!
}

/*
 * Read data from the LCD. Uses spi_device_transmit, which waits until
 * the transfer is complete.
 */
static void lcd_read(spi_device_handle_t spi, const uint8_t cmd, uint8_t *data, int len) {
    spi_transaction_t t;

    if (len == 0) return;                // no need to send anything

    lcd_cmd(spi, cmd);                   // Send the command
    memset(&t, 0, sizeof(t));            // Zero out the transaction
    t.length = len * 8;                  // len is in bytes, transaction length is in bits.
    t.rx_buffer = data;                  // Data
    t.user = (void *)1;                  // D/C needs to be set to 1
    spi_device_transmit(spi, &t);        // Transmit!
}

/*
 * Companion code to parse the initialization table.
 * Reads and issues a series of LCD commands stored in byte array
 */
static void lcd_send_command_list(spi_device_handle_t spi, const uint8_t *addr) {
    uint8_t numCommands, numArgs, cmd;
    uint16_t ms;

    numCommands = *addr++;                  // Number of commands to follow
    while (numCommands--) {                 // For each command...
        cmd      = *addr++;                 // Save command
        numArgs  = *addr++;                 // Number of arguments to follow
        ms       = numArgs & TFT_CMD_DELAY; // If high bit set, delay follows arguments
        numArgs &= ~TFT_CMD_DELAY;          // Mask out delay bit

        lcd_cmd(spi, cmd);
        lcd_data(spi, (uint8_t *)addr, numArgs);

        addr += numArgs;

        if (ms) {
            ms = *addr++;                   // Read post-command delay time (ms)
            vTaskDelay(ms / portTICK_RATE_MS);
        }
    }
}

/*
 * Set the screen rotation
 */
static void lcd_set_rotation(spi_device_handle_t spi, lcd_orientation_mode_t rotation) {
    lcd_rotation_t rot;
    uint8_t madctl;
    uint8_t rgb;

    /* Select RGB-BGR setting and rotation type */
    if (lcd_type == LCD_ST7789V) {
        rgb = ST7789V_TFT_RGB_BGR;
        rot = ST7789V_TFT_ROTATION;
    } else if (lcd_type == LCD_ILI9341) {
        rgb = ILI9341_TFT_RGB_BGR;
        rot = ILI9341_TFT_ROTATION;
    } else {
        return;
    }

    if ((rotation == LANDSCAPE) || (rotation == LANDSCAPE_FLIP)) {
        /* In landscape modes must be width > height */
        if (width < height) {
            uint16_t tmp = width;
            width  = height;
            height = tmp;
        }
    } else {
        /* In portrait modes must be width < height */
        if (width > height) {
            uint16_t tmp = width;
            width  = height;
            height = tmp;
        }
    }

    /* Apply LCD rotation */
    if (rot == TFT_INVERT_ROTATION) {
        switch (rotation) {
            case PORTRAIT:
            madctl = (MADCTL_MV | rgb);
            break;
            case LANDSCAPE:
            madctl = (MADCTL_MX | rgb);
            break;
            case PORTRAIT_FLIP:
            madctl = (MADCTL_MV | rgb);
            break;
            case LANDSCAPE_FLIP:
            madctl = (MADCTL_MY | rgb);
            break;
            default:
            return;
        }
    } else if (rot == TFT_INVERT_ROTATION1) {
        switch (rotation) {
            case PORTRAIT:
            madctl = (MADCTL_MY | MADCTL_MX | rgb);
            break;
            case LANDSCAPE:
            madctl = (MADCTL_MY | MADCTL_MV | rgb);
            break;
            case PORTRAIT_FLIP:
            madctl = (rgb);
            break;
            case LANDSCAPE_FLIP:
            madctl = (MADCTL_MX | MADCTL_MV | rgb);
            break;
            default:
            return;
        }
    } else if (rot == TFT_INVERT_ROTATION2) {
        switch (rotation) {
            case PORTRAIT:
            madctl = (MADCTL_MX | MADCTL_MV | rgb);
            break;
            case LANDSCAPE:
            madctl = (rgb);
            break;
            case PORTRAIT_FLIP:
            madctl = (MADCTL_MY | MADCTL_MV | rgb);
            break;
            case LANDSCAPE_FLIP:
            madctl = (MADCTL_MY | MADCTL_MX | rgb);
            break;
            default:
            return;
        }
    } else if (rot == TFT_NO_INVERT_ROTATION) {
        switch (rotation) {
            case PORTRAIT:
            madctl = (MADCTL_MX | rgb);
            break;
            case LANDSCAPE:
            madctl = (MADCTL_MV | rgb);
            break;
            case PORTRAIT_FLIP:
            madctl = (MADCTL_MY | rgb);
            break;
            case LANDSCAPE_FLIP:
            madctl = (MADCTL_MX | MADCTL_MY | MADCTL_MV | rgb);
            break;
            default:
            return;
        }
    } else {
        return;
    }

    /* Send the command */
    lcd_cmd(spi, TFT_MADCTL);
    lcd_data(spi, &madctl, 1);
}

/*
 * Set gamma curve
 */
static void lcd_set_gamma_curve(spi_device_handle_t spi, uint8_t gm) {
    uint8_t gamma_curve = 1 << (gm & 0x03);
    lcd_cmd(spi, TFT_GAMSET);
    lcd_data(spi, &gamma_curve, 1);
}

/*
 * Initialize the display
 */
static void lcd_display_init(spi_device_handle_t spi) {
    uint32_t lcd_id = 0;

    /* Set the D/C line low */
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_DC, 0);

    /* Disable the backlight */
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_BCKL, PIN_BCKL_OFF);

    /* Reset the display */
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(20 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(150 / portTICK_RATE_MS);

    /* Read the LCD id */
    lcd_read(spi, TFT_RDDID, (uint8_t *)&lcd_id, 3);

    if (lcd_id == 0) {
        /* Read the LCD ID4 */
        lcd_read(spi, ILI9341_TFT_RDID4, (uint8_t *)&lcd_id, 4);

        /*
         * Reading the ID4 is possible only if EXTC pin is high, as this command belongs to the external command list.
         * If the ID4 is 0, it means the EXTC pin is low or not connected, so all subsequent commands from the external
         * command list will be ignored by the LCD, including the ILI9341_TFT_IFCTL command which should set the little-endian interface
         * inside the LCD.
         * So, in case the EXTC is low or not connected, we have to do the little-endian / big-endian conversion before transferring the
         * frame to the LCD.
         */
        if (lcd_id == 0) {
            lcd_little_endian = false;
        }

        /* ILI9341 LCD detected */
        lcd_type = LCD_ILI9341;

        /* Perform ILI9341 initialization */
        lcd_send_command_list(spi, ILI9341_init);
#ifdef DEFAULT_ORIENTATION_MODE_LANDSCAPE
        /* Set screen rotation */
        lcd_set_rotation(spi, LANDSCAPE);
#else
        /* Set screen rotation */
        lcd_set_rotation(spi, PORTRAIT_FLIP);
#endif
    } else {
        /* ST7789V LCD detected */
        lcd_type = LCD_ST7789V;

        /* Perform ST7789V initialization */
        lcd_send_command_list(spi, ST7789V_init);
#ifdef DEFAULT_ORIENTATION_MODE_LANDSCAPE
        /* Set screen rotation */
        lcd_set_rotation(spi, LANDSCAPE_FLIP);
#else
        /* Set screen rotation */
        lcd_set_rotation(spi, PORTRAIT);
#endif

    }

    /* Set default gamma curve */
    lcd_set_gamma_curve(spi, DEFAULT_GAMMA_CURVE);

    /* Enable backlight */
    gpio_set_level(PIN_NUM_BCKL, PIN_BCKL_ON);
}

/*
 * This function is called (in irq context!) just before a transmission starts.
 * It will set the D/C line to the value indicated in the user field.
 */
static void IRAM_ATTR lcd_spi_pre_transfer_callback(spi_transaction_t *t) {
    int32_t dc = (int32_t)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

/*
 * This function send a number of transactions over SPI.
 * The transactions are queued and the function does not wait for them to complete
 */
static void IRAM_ATTR lcd_send_transactions(spi_device_handle_t spi, int32_t start, int32_t end) {
    for (int32_t x = start; x <= end; x++) {
        spi_device_queue_trans(spi, &trans[x], portMAX_DELAY);
    }
}

/*
 * This function waits until a number of transactions are sent completely over SPI.
 */
static void IRAM_ATTR lcd_wait_transactions(spi_device_handle_t spi, int32_t num) {
    spi_transaction_t *rtrans;

    for (int32_t x = 0; x < num; x++) {
        spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
    }
}

/**
 * This function will convert a frame buffer from little-endian to big-endian, pixel by pixel
 */
static void IRAM_ATTR lcd_convert_frame_endianness(uint8_t *buffer, uint32_t length) {
    /* Pixel data is 16-bit, so endianness matter. We need to swap the endianness of all the pixels */
    for (uint32_t offset = 0; offset < length; offset += (LLUI_DISPLAY_BPP / 8)) {
        uint16_t pixel = *(uint16_t *)(buffer + offset);
        pixel = ((pixel & 0x00FF) << 8) | ((pixel & 0xFF00) >> 8);
        *(uint16_t *)(buffer + offset) = pixel;
    }
}

/*
 * LCD Transfer task main function
 */
static void IRAM_ATTR lcd_transfer_task(void * pvParameters) {
    spi_device_handle_t *spi = (spi_device_handle_t *)pvParameters;
    uint8_t *line[2];

    /* Initialize transaction descriptors */
    for (int32_t x = 0; x < (sizeof(trans) / sizeof(trans[0])); x++) {
        memset(&trans[x], 0, sizeof(spi_transaction_t));
        if ((x & 1) == 0) {
            /* Even transfers are commands */
            trans[x].length = 8;
            trans[x].user = (void *)0;
        } else {
            /* Odd transfers are data */
            trans[x].length = 8 * 4;
            trans[x].user = (void *)1;
        }
        trans[x].flags = SPI_TRANS_USE_TXDATA;
    }

    /* Allocate 2 display lines in DMA capable region */
    line[0] = (uint8_t *)heap_caps_malloc(width * (LLUI_DISPLAY_BPP / 8), MALLOC_CAP_DMA);
    line[1] = (uint8_t *)heap_caps_malloc(width * (LLUI_DISPLAY_BPP / 8), MALLOC_CAP_DMA);

    /* Fill the fixed part of the transfer, common for all transactions that we will do */
    trans[0].tx_data[0] = TFT_CASET; // Column Address Set
    trans[2].tx_data[0] = TFT_RASET; // Row Address Set
    trans[5].flags = 0;              // Undo SPI_TRANS_USE_TXDATA flag for the last transfer which contains the data

    /* Enter the task loop */
    do {
        uint8_t sending_line = 0;
        uint8_t calc_line = 1;
        int32_t offset;

        /* Wait for a transfer to be requested */
        xSemaphoreTake(transfer_sem, portMAX_DELAY);

        /* Calculate the initial offset by skipping the lines and columns which do not need to be flushed */
        offset = (region.ymin * width * (LLUI_DISPLAY_BPP / 8)) + (region.xmin * (LLUI_DISPLAY_BPP / 8));

        /* Fill the transfer-variable part of the transfer, common for all transactions in one display update */
        trans[1].tx_data[0] = region.xmin >> 8;                            // Start Column High
        trans[1].tx_data[1] = region.xmin & 0xff;                          // Start Column Low
        trans[1].tx_data[2] = region.xmax >> 8;                            // End Column High
        trans[1].tx_data[3] = region.xmax & 0xff;                          // End Column Low
        trans[3].tx_data[0] = region.ymin >> 8;                            // Start Row High
        trans[3].tx_data[1] = region.ymin & 0xff;                          // Start Row Low
        trans[3].tx_data[2] = region.ymax >> 8;                            // End Row High
        trans[3].tx_data[3] = region.ymax & 0xff;                          // End Row Low
        trans[4].tx_data[0] = TFT_RAMWR;                                   // Write Memory
        trans[5].length = (region.xmax - region.xmin + 1) * LLUI_DISPLAY_BPP; // Data length, in bits

        /* Send the first 4 transactions, setting the column and row settings */
        lcd_send_transactions(*spi, 0, 3);

        /* Prepare the first line to be transfered */
        memcpy((uint8_t *)line[sending_line], (uint8_t *)(region.srcAddr + offset), (region.xmax - region.xmin + 1) * (LLUI_DISPLAY_BPP / 8));

        /* Convert the line frame from little-endian to big-endian if needed */
        if (!lcd_little_endian) {
            lcd_convert_frame_endianness((uint8_t *)line[sending_line], (region.xmax - region.xmin + 1) * (LLUI_DISPLAY_BPP / 8));
        }

        /* Wait for the first 4 transactions to be done */
        lcd_wait_transactions(*spi, 4);

        for (int32_t x = region.ymin; x < (region.ymax + 1); x++) {
            /* Prepare the data transactions */
            if (x != region.ymin) {
                trans[4].tx_data[0] = TFT_WRMEMC;            // Write Memory Continue
            }
            trans[5].tx_buffer = (void *)line[sending_line]; // Finally send the data

            /* Send the last 2 transactions, containing the memory write command and data */
            lcd_send_transactions(*spi, 4, 5);

            /*
             * When we are here, the SPI driver is busy (in the background) getting the transactions sent.
             * That happens mostly using DMA, so the CPU doesn't have much to do here. We're not going to
             * wait for the transaction to finish because we will spend the time preparing the next line.
             */
            offset += width * (LLUI_DISPLAY_BPP / 8);

            /* Prepare the next line to be transfered */
            memcpy((uint8_t *)line[calc_line], (uint8_t *)(region.srcAddr + offset), (region.xmax - region.xmin + 1) * (LLUI_DISPLAY_BPP / 8));
            sending_line = calc_line;
            calc_line = (calc_line == 1) ? 0 : 1;

            /* Convert the line frame from little-endian to big-endian if needed */
            if (!lcd_little_endian) {
                lcd_convert_frame_endianness((uint8_t *)line[sending_line], (region.xmax - region.xmin + 1) * (LLUI_DISPLAY_BPP / 8));
            }

            /* Wait for the last 2 transactions to be done */
            lcd_wait_transactions(*spi, 2);
        }

        /* The LCD update is finished */
        LLUI_DISPLAY_flushDone(false);

    } while (1);
}

/* API -----------------------------------------------------------------------*/

void LLUI_DISPLAY_IMPL_initialize(LLUI_DISPLAY_SInitData* init_data)
{

    spi_bus_config_t buscfg = {
        .miso_io_num     = PIN_NUM_MISO,
        .mosi_io_num     = PIN_NUM_MOSI,
        .sclk_io_num     = PIN_NUM_CLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = DEFAULT_TFT_DISPLAY_WIDTH * DEFAULT_TFT_DISPLAY_HEIGHT * (LLUI_DISPLAY_BPP / 8),
        .intr_flags      = ESP_INTR_FLAG_IRAM
    };

    spi_device_interface_config_t devcfg = {
        .command_bits     = 0,                                // No command bits
        .address_bits     = 0,                                // No address bits
        .dummy_bits       = 0,                                // No dummy bits
        .mode             = 0,                                // SPI mode 0
        .duty_cycle_pos   = 128,                              // 50%/50% duty of the clock
        .cs_ena_pretrans  = 0,                                // No extra cycles for CS to stay active before the transmission
        .cs_ena_posttrans = 0,                                // No extra cycles for CS to stay active after the transmission
        .clock_speed_hz   = LCD_INIT_SPI_CLOCK,               // Low clock for LCD initialization
        .spics_io_num     = PIN_NUM_CS,                       // CS pin
        .flags            = 0,                                // Work in full-duplex for LCD initialization
        .queue_size       = 4,                                // The max number of transactions queued at a certain time
        .pre_cb           = lcd_spi_pre_transfer_callback,    // Specify pre-transfer callback to handle D/C line
        .post_cb          = NULL                              // No post-transfer callback
    };

    /* Initialize the SPI bus */
    spi_bus_initialize(HSPI_HOST, &buscfg, 1);

    /* Attach the LCD to the SPI bus */
    spi_bus_add_device(HSPI_HOST, &devcfg, &spi_handle);

    /* Initialize the LCD */
    lcd_display_init(spi_handle);

    /* Remove the LCD from the SPI bus */
    spi_bus_remove_device(spi_handle);

    /* Initialize the init_data struct, must be done after lcd_display_init() */ 
    init_data->binary_semaphore_0 = (LLUI_DISPLAY_binary_semaphore*)xSemaphoreCreateBinary();
    init_data->binary_semaphore_1 = (LLUI_DISPLAY_binary_semaphore*)xSemaphoreCreateBinary();
    init_data->back_buffer_address = (uint8_t*) BACK_BUFFER;
    init_data->lcd_width = (uint32_t) width;
    init_data->lcd_height = (uint32_t) height;

    /* Configure the LCD for operation */
    if (lcd_type == LCD_ILI9341) {
        devcfg.clock_speed_hz = ILI9341_HIGH_SPI_CLOCK;       // High clock for LCD operation
    } else if (lcd_type == LCD_ST7789V) {
        devcfg.clock_speed_hz = ST7789V_HIGH_SPI_CLOCK;       // High clock for LCD operation
    }
    devcfg.flags = SPI_DEVICE_HALFDUPLEX;                     // Work in half-duplex to achieve speed > 26MHz

    /* Attach the LCD to the SPI bus */
    spi_bus_add_device(HSPI_HOST, &devcfg, &spi_handle);

    /* Create synchronization semaphores */
    vSemaphoreCreateBinary(transfer_sem);
    xSemaphoreTake(transfer_sem, 0);

    /* Create transfer task */
    xTaskCreate(lcd_transfer_task, LCD_TRANSFER_TASK_NAME, LCD_TRANSFER_TASK_STACK_SIZE,
            (void *)&spi_handle, LCD_TRANSFER_TASK_PRIORITY, NULL);
}

uint8_t* LLUI_DISPLAY_IMPL_flush(MICROUI_GraphicsContext* gc, uint8_t* srcAddr, uint32_t xmin, uint32_t ymin, uint32_t xmax, uint32_t ymax)
{
#ifdef FRAMERATE_ENABLED
    framerate_increment();
#endif
    /* Store the parameters to be accessible by the transfer task */
    region.srcAddr = srcAddr;
    region.xmin = xmin;
    region.ymin = ymin;
    region.xmax = xmax;
    region.ymax = ymax;

    /* Wake up the task that will transfer the data to the LCD memory */
    xSemaphoreGive(transfer_sem);

    return srcAddr;
}

void LLUI_DISPLAY_IMPL_binarySemaphoreTake(void* sem){
    xSemaphoreTake((xSemaphoreHandle)sem, portMAX_DELAY);
}

void LLUI_DISPLAY_IMPL_binarySemaphoreGive(void* sem, bool under_isr){
    if (under_isr)
	{
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR((xSemaphoreHandle)sem, &xHigherPriorityTaskWoken);
		if (xHigherPriorityTaskWoken != pdFALSE)
		{
			// Force a context switch here.
			portYIELD_FROM_ISR();
		}
	}
	else
	{
		xSemaphoreGive((xSemaphoreHandle)sem);
	}
}

LLUI_DISPLAY_Status LLUI_DISPLAY_IMPL_decodeImage(uint8_t* addr, uint32_t length, MICROUI_ImageFormat expectedFormat, MICROUI_Image* data, bool* isFullyOpaque)
{
	return MICROEJ_DECODE_webp(addr, length, expectedFormat, data, isFullyOpaque);
}
