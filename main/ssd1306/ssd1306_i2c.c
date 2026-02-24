#include <string.h>
#include "ssd1306.h"
#include "i2c_manager.h"
#include "esp_log.h"

#define TAG "SSD1306_I2C"

void i2c_device_add(SSD1306_t * dev, i2c_port_t i2c_num, int16_t reset, uint16_t i2c_address) {
	dev->_address = i2c_address;
	dev->_i2c_num = i2c_num;
}

void i2c_init(SSD1306_t * dev, int width, int height) {
	dev->_width = width;
	dev->_height = height;
	dev->_pages = height / 8;
	dev->_flip = false;

	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_DISPLAY_OFF}, 1);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_MUX_RATIO, 0x3F}, 2);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_DISPLAY_OFFSET, 0x00}, 2);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_DISPLAY_START_LINE}, 1);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_SEGMENT_REMAP_1}, 1);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_COM_SCAN_MODE}, 1);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_COM_PIN_MAP, 0x12}, 2);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_CONTRAST, 0x7F}, 2);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_DISPLAY_RAM}, 1);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_DISPLAY_NORMAL}, 1);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_DISPLAY_CLK_DIV, 0x80}, 2);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_CHARGE_PUMP, 0x14}, 2);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_DISPLAY_ON}, 1);
}

void i2c_display_image(SSD1306_t * dev, int page, int seg, const uint8_t * images, int width) {
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){0xB0 | page}, 1);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){0x00 | (seg & 0xf)}, 1);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){0x10 | ((seg >> 4) & 0xf)}, 1);
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_DATA_STREAM, images, width);
}

void i2c_contrast(SSD1306_t * dev, int contrast) {
	i2c_manager_write(dev->_address, OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t[]){OLED_CMD_SET_CONTRAST, contrast}, 2);
}

// Stub for SPI if needed
void spi_init(SSD1306_t * dev, int width, int height) {}
void spi_display_image(SSD1306_t * dev, int page, int seg, const uint8_t * images, int width) {}
void spi_contrast(SSD1306_t * dev, int contrast) {}
