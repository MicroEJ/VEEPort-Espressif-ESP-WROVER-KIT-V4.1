/*
 * Copyright 2015-2017 IS2T. All rights reserved.
 * Modification and distribution is permitted under certain conditions.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
#ifndef _LLDISPLAY_EXTRA_DRAWING
#define _LLDISPLAY_EXTRA_DRAWING

/**
 * Defines some structures used by LLDISPLAY_UTILS.
 */
#include <stdint.h>

/* Includes ------------------------------------------------------------------*/

#include "LLDISPLAY_EXTRA.h"

/* Structs -------------------------------------------------------------------*/

/*
 * Define an image with its size and its format (@see LLDISPLAY_UTILS#IMAGE_xxx).
 */
typedef struct
{
	uint16_t width;		/* Image width in pixels */
	uint16_t height;	/* Image height in pixels */
	uint8_t format;		/* Image format */
	uint8_t lut_nb_colors_minus_1;	/* LUT number of colors - 1 (0 means 1 color) */
} LLDISPLAY_SImage;

/*
 * Define a rectangle.
 */
typedef struct
{
	uint16_t x;			/* Top-left X-coordinate */
	uint16_t y;			/* Top-left Y-coordinate */
	uint16_t width;		/* Rectangle width in pixels */
	uint16_t height;	/* Rectangle height in pixels */
} LLDISPLAY_SRectangle;

/*
 * Define a RAW image to allocate in the working buffer which will be used by a 3rd-party
 * image decoder. The "(in)" fields are expected values to set to allocate the working buffer.
 * The "(out)" fields are values configured by the allocator and the decoder must use them.
 */
typedef struct
{
	uint16_t width;			/* (in) Image width in pixels */
	uint16_t height;		/* (in) Image height in pixels */
	uint8_t format;			/* (in) Image format */
	uint8_t row_align;		/* (in) RAW image row alignment in bits */
	uint8_t fully_opaque;	/* (in) set '1' to force to consider the image as a fully opaque image, otherwise it depends on format */
	uint8_t reserved1;		/* reserved */
	uint32_t address;		/* (out) RAW image address */
	uint16_t row_stride;	/* (out) RAW image row stride in pixels */
	uint16_t reserved2;		/* reserved */
	uint32_t reserved3;		/* reserved */
} LLDISPLAY_SDecoderImageData;

/*
 * Define the area of the source image to draw at the specified position. A color
 * is specified when the source image is an "Alpha" image (format LLDISPLAY_UTILS#IMAGE_A1
 * to LLDISPLAY_UTILS#IMAGE_A8); otherwise this field is useless.
 * @see LLDISPLAY_EXTRA#drawImage()
 */
typedef struct
{
	uint16_t src_x;			/* Source area top-left X-coordinate */
	uint16_t src_y;			/* Source area top-left Y-coordinate */
	uint16_t src_width;		/* Source area width in pixels */
	uint16_t src_height;	/* Source area height in pixels */
	uint16_t dest_x;		/* Destination area top-left X-coordinate */
	uint16_t dest_y;		/* Destination area top-left Y-coordinate */
	uint32_t color;			/* Color to apply for "Alpha" source image format */
	uint8_t alpha;			/* Global alpha value to apply */
} LLDISPLAY_SDrawImage;

/*
 * Define the area of the source image to draw at the specified position. A color
 * is specified when the source image is an "Alpha" image (format LLDISPLAY_UTILS#IMAGE_A1
 * to LLDISPLAY_UTILS#IMAGE_A8); otherwise this field is useless.
 * @see LLDISPLAY_EXTRA#flipImage()
 */
typedef struct
{
	uint16_t src_x;			/* Source area top-left X-coordinate */
	uint16_t src_y;			/* Source area top-left Y-coordinate */
	uint16_t src_width;		/* Source area width in pixels */
	uint16_t src_height;	/* Source area height in pixels */
	uint16_t dest_x;		/* Destination area top-left X-coordinate */
	uint16_t dest_y;		/* Destination area top-left Y-coordinate */
	uint32_t color;			/* Color to apply for "Alpha" source image format */
	uint8_t alpha;			/* Global alpha value to apply */
	uint8_t flip;			/* Flip mode to apply */
} LLDISPLAY_SFlipImage;

/*
 * Define the area of the source image to draw at the specified position. A color
 * is specified when the source image is an "Alpha" image (format LLDISPLAY_UTILS#IMAGE_A1
 * to LLDISPLAY_UTILS#IMAGE_A8); otherwise this field is useless.
 * Define too the destination area size (width and height).
 * @see LLDISPLAY_EXTRA#scaleImage()
 */
typedef struct
{
	uint16_t src_x;			/* Source area top-left X-coordinate */
	uint16_t src_y;			/* Source area top-left Y-coordinate */
	uint16_t src_width;		/* Source area width in pixels */
	uint16_t src_height;	/* Source area height in pixels */
	uint16_t dest_x;		/* Destination area top-left X-coordinate */
	uint16_t dest_y;		/* Destination area top-left Y-coordinate */
	uint32_t color;			/* Color to apply for "Alpha" source image format */
	uint8_t alpha;			/* Global alpha value to apply */
	uint8_t reserved;
	uint16_t dest_width;	/* Destination area width in pixels */
	uint16_t dest_height;	/* Destination area height in pixels */
} LLDISPLAY_SScaleImage;

/*
 * Define the area of the source image to draw at the specified position. A color
 * is specified when the source image is an "Alpha" image (format LLDISPLAY_UTILS#IMAGE_A1
 * to LLDISPLAY_UTILS#IMAGE_A8); otherwise this field is useless.
 * Define too the rotation angle and center position point. The reference point of
 * destination area is the image destination when the angle equals 0.
 * @see LLDISPLAY_EXTRA#rotateImage()
 */
typedef struct
{
	uint16_t src_x;			/* Source area top-left X-coordinate */
	uint16_t src_y;			/* Source area top-left Y-coordinate */
	uint16_t src_width;		/* Source area width in pixels */
	uint16_t src_height;	/* Source area height in pixels */
	uint16_t dest_x;		/* Top-left X-coordinate reference point of destination area */
	uint16_t dest_y;		/* Top-left Y-coordinate reference point of destination area */
	uint32_t color;			/* Color to apply for "Alpha" source image format */
	uint8_t alpha;			/* Global alpha value to apply */
	uint8_t reserved;
	uint16_t angle;			/* 0 -> 360 */
	uint16_t cx;			/* Rotation center X-coordinate */
	uint16_t cy;			/* Rotation center Y-coordinate */
} LLDISPLAY_SRotateImage;

#endif
