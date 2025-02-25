/**
 * @file ssd1306.h
 * @brief Simple driver for SSD1306 OLED displays with custom font support.
 */

#ifndef _INC_SSD1306_H
#define _INC_SSD1306_H

#include <pico/stdlib.h>
#include <hardware/i2c.h>

/**
 * @brief Defines commands used in SSD1306.
 */
typedef enum {
    SET_CONTRAST = 0x81,
    SET_ENTIRE_ON = 0xA4,
    SET_NORM_INV = 0xA6,
    SET_DISP = 0xAE,
    SET_MEM_ADDR = 0x20,
    SET_COL_ADDR = 0x21,
    SET_PAGE_ADDR = 0x22,
    SET_DISP_START_LINE = 0x40,
    SET_SEG_REMAP = 0xA0,
    SET_MUX_RATIO = 0xA8,
    SET_COM_OUT_DIR = 0xC0,
    SET_DISP_OFFSET = 0xD3,
    SET_COM_PIN_CFG = 0xDA,
    SET_DISP_CLK_DIV = 0xD5,
    SET_PRECHARGE = 0xD9,
    SET_VCOM_DESEL = 0xDB,
    SET_CHARGE_PUMP = 0x8D
} ssd1306_command_t;

/**
 * @brief Holds the configuration for the display.
 */
typedef struct {
    uint8_t width;       /**< Width of display. */
    uint8_t height;      /**< Height of display. */
    uint8_t pages;       /**< Number of display pages (calculated on initialization). */
    uint8_t address;     /**< I2C address of display. */
    i2c_inst_t *i2c_i;   /**< I2C connection instance. */
    bool external_vcc;   /**< Whether the display uses external VCC. */
    uint8_t *buffer;     /**< Display buffer. */
    size_t bufsize;      /**< Buffer size. */
    const uint8_t (*font)[5]; /**< Pointer to font data (5x7 default). */
} ssd1306_t;

/**
 * @brief Initializes the display.
 */
bool ssd1306_init(ssd1306_t *p, uint16_t width, uint16_t height, uint8_t address, i2c_inst_t *i2c_instance);

/**
 * @brief Deinitializes the display.
 */
void ssd1306_deinit(ssd1306_t *p);

/**
 * @brief Turns off the display.
 */
void ssd1306_poweroff(ssd1306_t *p);

/**
 * @brief Turns on the display.
 */
void ssd1306_poweron(ssd1306_t *p);

/**
 * @brief Sets the contrast of the display.
 */
void ssd1306_contrast(ssd1306_t *p, uint8_t val);

/**
 * @brief Inverts the display colors.
 */
void ssd1306_invert(ssd1306_t *p, uint8_t inv);

/**
 * @brief Displays the buffer content on the OLED.
 */
void ssd1306_show(ssd1306_t *p);

/**
 * @brief Clears the display buffer.
 */
void ssd1306_clear(ssd1306_t *p);

/**
 * @brief Sets the font to be used when drawing characters.
 */
void ssd1306_set_font(ssd1306_t *p, const uint8_t (*font)[5]);

/**
 * @brief Draws a character on the display using the current font.
 */
void ssd1306_draw_char(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, char c);

/**
 * @brief Draws a string on the display using the current font.
 */
void ssd1306_draw_string(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, const char *s);

#endif // _INC_SSD1306_H
