#include <Arduino.h>
#include <ESP32Time.h>
#include "driver/gpio.h"
#include "stdio.h"
#include <string>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "SSD1306Wire.h"

Adafruit_MPU6050 mpu;

SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h
// SSD1306Wire display(0x3c, D3, D5);  // ADDRESS, SDA, SCL  -  If not, they can be specified manually.
// SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);  // ADDRESS, SDA, SCL, OLEDDISPLAY_GEOMETRY  -  Extra param required for 128x32 displays.
// SH1106Wire display(0x3c, SDA, SCL);     // ADDRESS, SDA, SCL

#define h1_width 29
#define h1_height 50
static uint8_t h1_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x00, 0x00, 0x00, 0x50, 0x01, 0x00, 0x00, 0x80, 0x03, 0x00, 
  0x00, 0x61, 0x06, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x80, 0x1F, 0x00, 
  0x00, 0x68, 0x3E, 0x00, 0x00, 0x01, 0x7D, 0x00, 0x00, 0xC0, 0x7F, 0x00, 
  0x00, 0x50, 0xFF, 0x00, 0x00, 0x04, 0xFE, 0x00, 0x00, 0xA8, 0xFF, 0x00, 
  0x00, 0x40, 0xFE, 0x00, 0x00, 0x20, 0xFF, 0x01, 0x00, 0xC8, 0xFE, 0x01, 
  0x00, 0xA0, 0xFF, 0x01, 0x00, 0x84, 0xFF, 0x03, 0x00, 0x31, 0xFF, 0x03, 
  0x00, 0xC8, 0xFE, 0x03, 0x00, 0x64, 0xFF, 0x03, 0x40, 0x80, 0xFF, 0x03, 
  0x00, 0x9A, 0xFF, 0x03, 0x00, 0xE0, 0xFF, 0x03, 0x00, 0xA8, 0xFF, 0x03, 
  0x40, 0x92, 0xFF, 0x03, 0x00, 0xE8, 0xFF, 0x03, 0x00, 0xC6, 0xFF, 0x01, 
  0x84, 0xF8, 0xFF, 0x01, 0x00, 0x95, 0xFF, 0x01, 0xD0, 0xDA, 0xFF, 0x00, 
  0x00, 0xF5, 0xFF, 0x00, 0x60, 0xEE, 0x7F, 0x00, 0x00, 0xF8, 0x3F, 0x00, 
  0x80, 0xEF, 0x3F, 0x00, 0x00, 0x70, 0x1F, 0x00, 0x00, 0x80, 0x0E, 0x00, 
  0x00, 0xFE, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };


#define h2_width 34
#define h2_height 50
static uint8_t h2_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x34, 
  0x00, 0x00, 0x00, 0x10, 0x69, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 
  0x00, 0x00, 0xF8, 0x01, 0x00, 0x00, 0x00, 0xE2, 0x03, 0x00, 0x00, 0x00, 
  0xE4, 0x03, 0x00, 0x00, 0x80, 0xF4, 0x07, 0x00, 0x00, 0x00, 0xE8, 0x0F, 
  0x00, 0x00, 0x00, 0xE1, 0x0F, 0x00, 0x00, 0x84, 0xF6, 0x0F, 0x00, 0x00, 
  0x00, 0xD8, 0x1F, 0x00, 0x00, 0x40, 0xE8, 0x1F, 0x00, 0x00, 0x00, 0xEA, 
  0x1F, 0x00, 0x00, 0x00, 0xF2, 0x1F, 0x00, 0x00, 0x80, 0xA8, 0x3F, 0x00, 
  0x00, 0x00, 0xA0, 0x7F, 0x00, 0x00, 0x00, 0xDD, 0x3F, 0x00, 0x00, 0x00, 
  0xE0, 0x3F, 0x00, 0x00, 0x6A, 0xE5, 0x3F, 0x00, 0x80, 0x00, 0xB2, 0x7F, 
  0x00, 0x20, 0x40, 0xE8, 0x7F, 0x00, 0x00, 0x00, 0xD8, 0x7F, 0x00, 0x00, 
  0x50, 0xA4, 0x3F, 0x00, 0x00, 0x08, 0xD4, 0x7E, 0x00, 0x00, 0x80, 0x68, 
  0x7F, 0x00, 0x00, 0x00, 0xBA, 0x3F, 0x00, 0x00, 0x80, 0xE2, 0x3F, 0x00, 
  0x00, 0x50, 0x9A, 0x3F, 0x00, 0x00, 0x82, 0xE6, 0x3F, 0x00, 0x00, 0x88, 
  0xF5, 0x1F, 0x00, 0x00, 0x30, 0xAA, 0x0F, 0x00, 0x00, 0xA6, 0xF2, 0x0F, 
  0x00, 0x00, 0x50, 0xEF, 0x07, 0x00, 0x00, 0x98, 0xDD, 0x03, 0x00, 0x00, 
  0x20, 0xFA, 0x01, 0x00, 0x00, 0x80, 0x64, 0x01, 0x00, 0x00, 0x90, 0xD9, 
  0x00, 0x00, 0x00, 0x80, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define h4_width 32
#define h4_height 50
static uint8_t h4_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x01, 0x00, 
  0x00, 0x88, 0x06, 0x00, 0x00, 0x40, 0x0F, 0x00, 0x00, 0x00, 0x1E, 0x00, 
  0x00, 0xA0, 0x3F, 0x00, 0x00, 0x82, 0x3E, 0x00, 0x00, 0x00, 0x3F, 0x00, 
  0x00, 0x58, 0x7F, 0x00, 0x00, 0x60, 0xFF, 0x00, 0x00, 0x80, 0xFD, 0x00, 
  0x00, 0x62, 0xFE, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xC0, 0xFE, 0x00, 
  0x00, 0x18, 0xF6, 0x01, 0x00, 0x20, 0xFF, 0x01, 0x00, 0x00, 0xF8, 0x01, 
  0x00, 0x00, 0xF0, 0x03, 0x00, 0x00, 0xA0, 0x01, 0x00, 0x00, 0x00, 0x01, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x50, 0x0D, 0x86, 0x00, 0xA0, 0xA0, 0x3C, 0x00, 0x00, 0x0A, 0xFB, 0x00, 
  0x00, 0x62, 0xFD, 0x01, 0x00, 0x00, 0xFE, 0x01, 0x00, 0x54, 0xF9, 0x01, 
  0x00, 0x89, 0xFE, 0x01, 0x80, 0x60, 0xFD, 0x00, 0x10, 0x51, 0xFF, 0x00, 
  0x00, 0xA5, 0xFA, 0x00, 0x00, 0xE0, 0xFF, 0x00, 0x00, 0x5E, 0x7E, 0x00, 
  0x00, 0xC9, 0x3F, 0x00, 0x00, 0xB4, 0x1F, 0x00, 0x00, 0x88, 0x0D, 0x00, 
  0x00, 0xBA, 0x15, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
  0x00, 0x90, 0x01, 0x00, 0x00, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };


//IMAGENES FULL HD
#define guanabana_width 35
#define guanabana_height 35
static uint8_t guanabana_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 
  0xFE, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0xE7, 0x01, 
  0x08, 0x00, 0x00, 0xFF, 0x81, 0x04, 0x00, 0x80, 0xDF, 0x03, 0x00, 0x00, 
  0xC0, 0xDF, 0x67, 0x00, 0x00, 0x80, 0xFF, 0x0F, 0x20, 0x00, 0x80, 0xFF, 
  0x67, 0x08, 0x00, 0xC0, 0xFF, 0x13, 0x08, 0x00, 0xC0, 0xFF, 0x23, 0x20, 
  0x00, 0xC0, 0xFF, 0x09, 0x28, 0x00, 0xC0, 0xFF, 0x08, 0x00, 0x00, 0xC0, 
  0xFF, 0x04, 0x00, 0x00, 0xC0, 0xFF, 0x04, 0x0C, 0x00, 0x80, 0xFF, 0x00, 
  0x10, 0x00, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 
  0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x01, 0x02, 0x00, 0x00, 0x26, 
  0x01, 0x08, 0x00, 0x00, 0x80, 0xFE, 0x03, 0x00, 0x00, 0x90, 0xFD, 0x03, 
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
  0x80, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define papaya_width 44
#define papaya_height 35
static uint8_t papaya_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x48, 0x03, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x14, 0x00, 0x00, 
  0x00, 0x96, 0x24, 0x44, 0x00, 0x00, 0x80, 0xF9, 0x19, 0xA3, 0x01, 0x00, 
  0x40, 0x60, 0x21, 0xBC, 0x06, 0x00, 0x80, 0xFF, 0x5B, 0xFD, 0x0B, 0x00, 
  0x80, 0xFF, 0xD7, 0xFE, 0x1F, 0x00, 0xC0, 0xFF, 0x0F, 0xFE, 0x2F, 0x00, 
  0x60, 0xFF, 0xAF, 0xFA, 0x3F, 0x00, 0x80, 0xFD, 0x3F, 0xF9, 0xFF, 0x00, 
  0x00, 0xFE, 0xBF, 0xD6, 0xBF, 0x00, 0x40, 0xD5, 0x7F, 0xA8, 0xFF, 0x00, 
  0x80, 0xA9, 0xFD, 0x61, 0x7E, 0x00, 0x00, 0x54, 0xF6, 0x56, 0x91, 0x01, 
  0x00, 0x8A, 0xF5, 0x53, 0x55, 0x00, 0x00, 0x66, 0xFD, 0xA5, 0x28, 0x00, 
  0x00, 0xF0, 0x3E, 0x6A, 0x1F, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x02, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define rey_width 35
#define rey_height 35
static uint8_t rey_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFB, 
  0xFF, 0xFF, 0x01, 0x7C, 0xFB, 0xFF, 0xDF, 0x00, 0xE4, 0x7B, 0xEF, 0xFF, 
  0x01, 0xFC, 0x4B, 0x09, 0xFE, 0x01, 0xFC, 0x01, 0x11, 0xFE, 0x01, 0xFC, 
  0xEF, 0xFE, 0x7F, 0x00, 0xFC, 0x63, 0xE8, 0xFE, 0x00, 0xFC, 0x01, 0x60, 
  0xFE, 0x01, 0xFC, 0x09, 0x4E, 0xFC, 0x01, 0xFC, 0x49, 0x4F, 0xFE, 0x01, 
  0xFC, 0x09, 0x0E, 0xFE, 0x01, 0xFC, 0x09, 0x0F, 0xFF, 0x01, 0xFC, 0x01, 
  0x1E, 0xFF, 0x01, 0xFC, 0x49, 0x1C, 0xFE, 0x01, 0xFC, 0x09, 0x18, 0xFF, 
  0x00, 0xFC, 0x41, 0x3F, 0xFF, 0x01, 0xFC, 0x01, 0xBF, 0xFF, 0x01, 0xFC, 
  0x01, 0x1E, 0xFF, 0x00, 0xFC, 0x49, 0x1E, 0xFF, 0x01, 0xF8, 0x09, 0xBE, 
  0xFF, 0x01, 0xFC, 0x01, 0x1E, 0xFF, 0x01, 0xFC, 0x09, 0x30, 0xFF, 0x00, 
  0xFC, 0x49, 0xB8, 0xFF, 0x00, 0xF8, 0xFF, 0xFF, 0xEF, 0x01, 0xF8, 0xD3, 
  0xFF, 0xFF, 0x01, 0xFC, 0xCF, 0xFD, 0xFF, 0x01, 0xF8, 0xFF, 0x7D, 0xBD, 
  0x00, 0xFC, 0x7F, 0xFF, 0xFF, 0x01, 0xFC, 0xDD, 0xFE, 0xFA, 0x00, 0xFC, 
  0xFF, 0xBF, 0xF3, 0x01, 0xF8, 0xFF, 0xFF, 0xDF, 0x01, 0x80, 0xA0, 0x24, 
  0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define nacho_width 35
#define nacho_height 35
static uint8_t nacho_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0F, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 
  0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0xC0, 0x18, 0x00, 0x00, 
  0x00, 0xC0, 0x54, 0x00, 0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00, 0x00, 0xD0, 
  0x1F, 0x00, 0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00, 0x00, 0xC0, 0x1F, 0x00, 
  0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00, 0x00, 0xD0, 0x1F, 0x00, 0x00, 0x00, 
  0x80, 0x15, 0x00, 0x00, 0x00, 0xC0, 0x10, 0x00, 0x00, 0x00, 0xC0, 0x15, 
  0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0xC0, 0x12, 0x00, 0x00, 
  0x00, 0xC0, 0x1F, 0x00, 0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00, 0x00, 0xE0, 
  0x1F, 0x00, 0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00, 0x00, 0xD0, 0x1F, 0x00, 
  0x00, 0x00, 0xC0, 0x5F, 0x00, 0x00, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x00, 
  0x10, 0x1C, 0x00, 0x00, 0x00, 0xC0, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };


#define rana_width 60
#define rana_height 60
static uint8_t rana_bits[] = {
  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x40, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xFC, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x86, 0x03, 
  0x18, 0x07, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x03, 0xE4, 0x0C, 0x00, 0x00, 
  0x00, 0x00, 0xDB, 0x6E, 0xF7, 0x09, 0x00, 0x00, 0x00, 0x00, 0x8D, 0xFE, 
  0x17, 0x0B, 0x00, 0x00, 0x00, 0x00, 0xCB, 0x00, 0x30, 0x09, 0x00, 0x00, 
  0x00, 0x00, 0xDB, 0x00, 0xF0, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x7B, 0x00, 
  0x60, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 
  0x00, 0x80, 0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x80, 0x40, 0xFC, 
  0x05, 0x38, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x9F, 0xFF, 0x1F, 0x00, 0x00, 
  0x00, 0x85, 0x21, 0x01, 0xA0, 0x12, 0x04, 0x00, 0x80, 0x9F, 0x01, 0x00, 
  0x00, 0x98, 0x1F, 0x00, 0xC0, 0x30, 0x07, 0x00, 0x00, 0xCC, 0x31, 0x00, 
  0x60, 0x60, 0x0E, 0x00, 0x00, 0x67, 0x60, 0x00, 0x60, 0xC0, 0xB8, 0x00, 
  0xE0, 0x31, 0x60, 0x00, 0x20, 0x80, 0xE1, 0x97, 0x7E, 0x18, 0x40, 0x00, 
  0x60, 0x80, 0x61, 0xFE, 0x77, 0x18, 0x40, 0x00, 0x60, 0x00, 0x33, 0x00, 
  0x40, 0x0C, 0x60, 0x00, 0x60, 0x00, 0x36, 0x00, 0x40, 0x04, 0x61, 0x00, 
  0x40, 0x18, 0x16, 0x00, 0xC0, 0x86, 0x21, 0x00, 0xC0, 0x18, 0x1C, 0x00, 
  0xC0, 0x83, 0x31, 0x00, 0x80, 0x30, 0x38, 0x00, 0x80, 0xC3, 0x30, 0x00, 
  0x80, 0x21, 0x18, 0x00, 0x80, 0x61, 0x18, 0x00, 0x00, 0x61, 0x18, 0x00, 
  0xC0, 0x61, 0x08, 0x00, 0x00, 0xC3, 0x30, 0x02, 0x84, 0x30, 0x0C, 0x00, 
  0x00, 0xC6, 0x11, 0x02, 0x84, 0x10, 0x06, 0x00, 0x00, 0x8C, 0x31, 0x02, 
  0xCC, 0x18, 0x03, 0x00, 0x00, 0x1C, 0x13, 0x03, 0x84, 0x8C, 0x01, 0x00, 
  0x00, 0x10, 0x12, 0x02, 0x8C, 0x8C, 0x01, 0x00, 0x00, 0x70, 0x36, 0x03, 
  0xC4, 0xE6, 0x00, 0x00, 0x00, 0x60, 0x3C, 0x02, 0xC4, 0x63, 0x00, 0x00, 
  0x00, 0xC0, 0x1D, 0x03, 0x8C, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x03, 
  0xC4, 0x0F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x50, 0xAA, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   };

#define dirty_width 10
#define dirty_height 10
static uint8_t dirty_bits[] = {
  0xFF, 0x03, 0xBF, 0x03, 0xFF, 0x03, 0xFF, 0x03, 0xE5, 0x03, 0x67, 0x03, 
  0xFF, 0x03, 0xFA, 0x01, 0xFF, 0x03, 0xFF, 0x03, };

#define cora_width 10
#define cora_height 10
static uint8_t cora_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0xFC, 0x00, 0xFC, 0x00, 0xFC, 0x00, 
  0x78, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define hambre_width 10
#define hambre_height 10
static uint8_t hambre_bits[] = {
  0xE0, 0x00, 0xE0, 0x01, 0xF8, 0x03, 0xD8, 0x03, 0xFC, 0x03, 0xEC, 0x00, 
  0x7C, 0x00, 0x3B, 0x00, 0x0F, 0x00, 0x06, 0x00, };

#define sadge_width 10
#define sadge_height 10
static uint8_t sadge_bits[] = {
  0x00, 0x00, 0x38, 0x00, 0xC4, 0x00, 0x96, 0x00, 0x82, 0x01, 0x32, 0x01, 
  0xCE, 0x00, 0xCC, 0x00, 0x30, 0x00, 0x00, 0x00, };





//PIN DEFINITION
#define BUTTON_PIN_FLAG 0
#define BUTTON_PINMid GPIO_NUM_35
#define BUTTON_PINLeft GPIO_NUM_34
#define BUTTON_PINRight GPIO_NUM_39


//VARIABLES

//Buttons
bool left = false;
bool mid = false;
bool right = false;

//Fixes
int start = 0;

//Stats Variables
int life = 100;
int happiness = 100;
int saturation = 100;
int hygiene = 100;
int money = 100;
int age = 1;
String emociones = "aaaa";

//StateMachine
int state = 0;

//StatePantalla
bool flagPantalla = false;
int taskPantalla = 0;
int miniTask = 0;

//menuVarible
int menuState = 0;
bool enterFlag = false;
char txbuff[50];
String actividades[5] = {"Salir", "Jugar", "Comer", "Asear", "Comprar"};

//jugarVariable
int jugarState = 0;
bool jugarFlag = false;
char txbuffJugar[50];
String juegos[5] = {"Salir", "Moriohtraques", "Lozanoaventuras", "Where's Freddy", "Amogus"};

//comerVariable
int comerState = 0;
bool comerFlag = false;
char txbuffComer[50];
String productosComida[3] = {"Salir", "Guanabana", "Papaya"};
int cantidadComida[2] = {0,0};

//asearVariable
int asearState = 0;
bool asearFlag = false;
char txbuffAsear[50];
String productosAseo[3] = {"Salir", "Jabón Rey", "Tío Nacho"};
int cantidadAseo[2] = {0,0};

//comprarVariable
int comprarState = 0;
bool comprarFlag = false;
char txbuffComprar[50];
String productos[5] = {"Salir", "Jabón Rey", "Tío Nacho", "Guanabana", "Papaya"};

//moriohTraquesVariable
int moriohTraquesState = 0;
bool moriohTraquesFlag = false;
int moriohTraquesCounter = 0;
char txbuffMoriohTraques[50];


//boolhuevos
bool huevo1 = false;
bool huevo2 = false;
bool huevo3 = false;
bool huevoFinal = false;


//TASK Handlers

//Botones
TaskHandle_t botonLeft = NULL;
TaskHandle_t botonMid = NULL;
TaskHandle_t botonRight = NULL;

//Pasivos: Stats, Rana
SemaphoreHandle_t pasivos = NULL;
SemaphoreHandle_t inventarioSemaphore = NULL;
TaskHandle_t idleH = NULL;
TaskHandle_t statsH = NULL;
TaskHandle_t menuH = NULL;
TaskHandle_t tiempoH = NULL;
TaskHandle_t inventoryH = NULL;
TaskHandle_t imprimir = NULL; 

//Activos: Menu y demas
SemaphoreHandle_t salir = NULL;
SemaphoreHandle_t jugarSemaphore = NULL;
SemaphoreHandle_t comerSemaphore = NULL;
SemaphoreHandle_t asearSemaphore = NULL;
SemaphoreHandle_t comprarSemaphore = NULL;
SemaphoreHandle_t statsManagerSemaphore = NULL;
SemaphoreHandle_t videojuegos = NULL;
SemaphoreHandle_t printDisplay = NULL;
TaskHandle_t jugarH = NULL;
TaskHandle_t comerH = NULL;
TaskHandle_t asearH = NULL;
TaskHandle_t comprarH = NULL;
QueueHandle_t actividadQ;
QueueHandle_t asearQ;
QueueHandle_t comerQ;
QueueHandle_t comprarQ;
QueueHandle_t jugarQ;
QueueHandle_t moriohTraquesQ;

//FUNCTIONS

void drawStats() {
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Stats");
  //linea 10
  display.drawString(0, 20, "Vida ");
  display.drawProgressBar(55, 20, 50, 10, life);
  //linea 20
  display.drawString(0, 30, "Felicidad");
  display.drawProgressBar(55, 30, 50, 10, happiness);
  //linea 30
  display.drawString(0, 40, "Higiene");
  display.drawProgressBar(55, 40, 50, 10, hygiene);
  //linea 40
  display.drawString(0, 50, "Saturación");
  display.drawProgressBar(55, 50, 50, 10, saturation);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 0, "Años: " + String(age));
  display.drawString(128, 20, String(life) + "%");
  display.drawString(128, 30, String(happiness) + "%");
  display.drawString(128, 40, String(hygiene) + "%");
  display.drawString(128, 50, String(saturation) + "%");

  display.display();


}
void drawMoney() {
  display.drawRect(0, 35, 115, 15);
  display.drawString(59, 35, "Phroglins: " + String(money));
  display.display();
}

void drawMenu(int selected) {
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Menu");
  display.drawRect(120, 20, 5, 44);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  switch(selected){
    case(0):{      
      display.drawRect(0, 20, 115, 15);
      display.drawString(59, 20, "Salir");
      display.drawString(59, 35, "Jugar");
      display.drawString(59, 50, "Comer");
      display.fillRect(120, 20, 5, 8.8);
      break;
    }
    case(1):{
      display.drawRect(0, 35, 115, 15);
      display.drawString(59, 20, "Salir");
      display.drawString(59, 35, "Jugar");
      display.drawString(59, 50, "Comer");
      display.fillRect(120, 28.8, 5, 8.8);
      break;
    }
    case(2):{
      display.drawRect(0, 50, 115, 14);
      display.drawString(59, 20, "Salir");
      display.drawString(59, 35, "Jugar");
      display.drawString(59, 50, "Comer");
      display.fillRect(120, 37.6, 5, 8.8);
      break;
    }
    case(3):{
      display.drawRect(0, 50, 115, 14);
      display.drawString(59, 20, "Jugar");
      display.drawString(59, 35, "Comer");
      display.drawString(59, 50, "Asear");
      display.fillRect(120, 45.2, 5, 8.8);
      break;
    }
    case(4):{
      display.drawRect(0, 50, 115, 14);
      display.drawString(59, 20, "Comer");
      display.drawString(59, 35, "Asear");
      display.drawString(59, 50, "Comprar");
      display.fillRect(120, 54, 5, 9);
      break;
    }
    default:{
      display.drawString(0,0,"LA EMBARRARON ESTE NUMERO NO EXISTE");
      break;
    }

  
  
  }
  display.display();
}


void drawInventario(int select){
  Serial.print(select);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Inventario");
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  switch(select){
    case(0):{      
      display.drawRect(0, 50, 120, 14);
      display.drawXbm(40, 15, guanabana_width, guanabana_height, guanabana_bits);
      display.drawString(59, 50, "Guanabana x"+String(cantidadComida[0]));
      break;
    }
    case(1):{
      display.drawRect(0, 50, 120, 14);
      display.drawXbm(40, 15, papaya_width, papaya_height, papaya_bits);
      display.drawString(59, 50, "Papaya x"+String(cantidadComida[1]));

      
      break;
    }
    case(2):{
      display.drawRect(0, 50, 120, 14);
      display.drawXbm(40, 15, nacho_width, nacho_height, nacho_bits);
      display.drawString(59, 50, "Tio nacho x"+String(cantidadAseo[1]));
      break;
    }
    case(3):{
      display.drawRect(0, 50, 120, 14);
      display.drawXbm(40, 15, rey_width, rey_height, rey_bits);
      display.drawString(59, 50, "Jabon Rey x"+String(cantidadAseo[0]));
      break;
    }
    default:{
      display.drawString(0,0,"LA EMBARRARON ESTE NUMERO NO EXISTE");
      break;
    }
  }
  display.display();
}


void drawMain(String selected){
  display.drawString(0, 0, "Sapolio");
  display.drawXbm(35, 15, rana_width, rana_height, rana_bits);
  //draw in a random spot of the screen
  if(selected.indexOf("s")!=-1){
  display.drawXbm(random(0,128),random(20,64),dirty_width,dirty_height,dirty_bits);
  }
  if(selected.indexOf("c")!=-1){
  display.drawXbm(random(0,128),random(20,64),cora_width,cora_height,cora_bits);
  }
  if(selected.indexOf("h")!=-1){
  display.drawXbm(random(0,128),random(20,64),hambre_width,hambre_height,hambre_bits);
  }
  if(selected.indexOf("t")!=-1){
  display.drawXbm(random(0,128),random(20,64),sadge_width,sadge_height,sadge_bits);
  }
  display.display();
  delay(1000);
}





void drawHuevo(int select){
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Huevo");
  
  display.clear();
  switch(select){
    case(0):{
      display.drawXbm(40, 15, h1_width, h1_height, h1_bits);
      break;
    }
    case(1):{
      display.drawXbm(40, 15, h2_width, h2_height, h2_bits);
      break;
    }
    case(2):{
      display.drawXbm(40, 15, h4_width, h4_height, h4_bits);
      break;
    }
  }
  display.display();
  
}

void drawTienda(int selected){
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Tienda");
  display.drawRect(120, 20, 5, 44);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  switch(selected){
    case(0):{      
      display.drawRect(0, 20, 115, 15);
      display.drawString(59, 20, "Salir");
      display.drawString(59, 35, "Jabon Rey");
      display.drawString(59, 50, "Tio Nacho");
      display.fillRect(120, 20, 5, 8.8);
      break;
    }
    case(1):{
      display.drawRect(0, 35, 115, 15);
      display.drawString(59, 20, "Salir");
      display.drawString(59, 35, "Jabon Rey");
      display.drawString(59, 50, "Tio Nacho");
      display.fillRect(120, 28.8, 5, 8.8);
      break;
    }
    case(2):{
      display.drawRect(0, 50, 115, 14);
      display.drawString(59, 20, "Salir");
      display.drawString(59, 35, "Jabon Rey");
      display.drawString(59, 50, "Tio Nacho");
      display.fillRect(120, 37.6, 5, 8.8);
      break;
    }
    case(3):{
      display.drawRect(0, 50, 115, 14);
      display.drawString(59, 20, "Jabon Rey");
      display.drawString(59, 35, "Tio Nacho");
      display.drawString(59, 50, "Guanabana");
      display.fillRect(120, 45.2, 5, 8.8);
      break;
    }
    case(4):{
      display.drawRect(0, 50, 115, 14);
      display.drawString(59, 20, "Tio Nacho");
      display.drawString(59, 35, "Guanabana");
      display.drawString(59, 50, "Papaya");
      display.fillRect(120, 54, 5, 9);
      break;
    }
    default:{
      display.drawString(0,0,"LA EMBARRARON ESTE NUMERO NO EXISTE");
      break;
    }

  
  
  }
  display.display();

}

void drawComer(int selected) {
    
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Comida");
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  switch(selected){
    case(1):{      
      display.drawRect(0, 50, 120, 14);
      display.drawXbm(40, 15, guanabana_width, guanabana_height, guanabana_bits);
      display.drawString(59, 50, "Guanabana x"+String(cantidadComida[0]));
      break;
    }
    case(2):{
      display.drawRect(0, 50, 120, 14);
      display.drawXbm(40, 15, papaya_width, papaya_height, papaya_bits);
      display.drawString(59, 50, "Papaya x"+String(cantidadComida[1]));

      
      break;
    }
    case(0):{
      display.drawRect(0, 35, 115, 15);
      display.drawString(59, 35, "Salir");
      break;
    }
    default:{
      display.drawString(0,0,"LA EMBARRARON ESTE NUMERO NO EXISTE");
      break;
    }
}
display.display();
}

void drawAsear(int selected) {
    
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Aseo");
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  switch(selected){
    case(2):{
      display.drawRect(0, 50, 120, 14);
      display.drawXbm(40, 15, nacho_width, nacho_height, nacho_bits);
      display.drawString(59, 50, "Tio nacho x"+String(cantidadAseo[1]));
      break;
    }
    case(1):{
      display.drawRect(0, 50, 120, 14);
      display.drawXbm(40, 15, rey_width, rey_height, rey_bits);
      display.drawString(59, 50, "Jabon Rey x"+String(cantidadAseo[0]));
      break;
    }
    case(0):{
      display.drawRect(0, 35, 115, 15);
      display.drawString(59, 35, "Salir");
      break;
    }
    default:{
      display.drawString(0,0,"LA EMBARRARON ESTE NUMERO NO EXISTE");
      break;
    }
}
display.display();
}


//Complementary Stats
void printStats() {
  Serial.print("Life: ");
  Serial.println(life);
  Serial.print("Happiness: ");
  Serial.println(happiness);
  Serial.print("Saturation: ");
  Serial.println(saturation);
  Serial.print("Hygiene: ");
  Serial.println(hygiene);
  Serial.print("Age: ");
  Serial.println(age);
  Serial.print("Phroglins: ");
  Serial.println(money);

    delay(2000);
}

void pantalla(void *p) {

  while(1) {
    if(flagPantalla) {
      display.clear();
      Serial.print("imprimimos bb");
      if(taskPantalla == 0) {
        if(miniTask == 0) {
          drawMain(emociones);
        }

      }
      else if(taskPantalla == 1) //Menu
      {
        drawMenu(miniTask);
      } 
      else if(taskPantalla == 2) //Jugar
      {
        //a
      }
      else if(taskPantalla == 3) //Stats
      {
        Serial.print("imprimimos STATS");
        drawStats();
        vTaskDelay(3000 * configTICK_RATE_HZ / 1000);
        display.clear();
        drawMoney();
      }
      else if(taskPantalla == 4) //Inventario
      {
        Serial.print(miniTask);
        drawInventario(miniTask);
      }
      else if(taskPantalla == 5) //Comer
      {
        drawComer(miniTask);
      }
      else if(taskPantalla == 6) //Asear
      {
        drawAsear(miniTask);
      }
      else if(taskPantalla == 7) //Tienda
      {
        drawTienda(miniTask);
      }
      Serial.print(taskPantalla);

    flagPantalla = false;
    }
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
  }


}

//Stats
void stats(void *p) {
  while(1) {
    if (xSemaphoreTake(pasivos, portMAX_DELAY)) {
      taskPantalla = 3;
      miniTask = 0;
      flagPantalla = true;
      vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    }
  }
}

//Complementary inventory
void printInventory() {
  Serial.print("Jabón Rey: ");
  Serial.println(cantidadAseo[0]);
  Serial.print("Tío Nacho: ");
  Serial.println(cantidadAseo[1]);
  Serial.print("Guanabana: ");
  Serial.println(cantidadComida[0]);
  Serial.print("Papaya: ");
  Serial.println(cantidadComida[1]);
  delay(2000);
}

//Inventory
void inventory(void *p) {
  while(1) {
    if (xSemaphoreTake(inventarioSemaphore, portMAX_DELAY)) {
        taskPantalla = 4;
        flagPantalla = true;
        

    }
  }
}

//Menu
void menu() {
  xSemaphoreTake(salir, portMAX_DELAY);
  while(1 && !enterFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && menuState>0) 
    {
      menuState -= 1;
      left = false;
      taskPantalla = 1;
      miniTask = menuState;
      flagPantalla = true;
    }
    else if (mid) {
      enterFlag = true;
      mid = false;
    }
    else if (right && menuState<4) {
      menuState += 1;
      right = false;
      taskPantalla = 1;
      miniTask = menuState;
      flagPantalla = true;
      
    }
    Serial.print(actividades[menuState]);
    Serial.print(enterFlag);
  }
  sprintf(txbuff, actividades[menuState].c_str());
  xQueueSend(actividadQ, (void*)txbuff, (TickType_t)0);
  enterFlag = false;
  menuState = 0;
  xSemaphoreGive(salir);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}


//Asear
void asear() {
  xSemaphoreTake(asearSemaphore, portMAX_DELAY);
  while(!asearFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && asearState>0) 
    {
      asearState -= 1;
      left = false;
      taskPantalla = 6;
      miniTask = asearState;
      flagPantalla = true;
    }
    else if (mid) {
      asearFlag = true;
      mid = false;
    }
    else if (right && asearState<2) {
      asearState += 1;
      right = false;
      taskPantalla = 6;
      miniTask = asearState;
      flagPantalla = true;
    }
    Serial.print(productosAseo[asearState]);
    Serial.print(asearFlag); 
  }
  sprintf(txbuffAsear, productosAseo[asearState].c_str());
  xQueueSend(asearQ, (void*)txbuffAsear, (TickType_t)0);
  asearFlag = false;
  asearState = 0;
  xSemaphoreGive(asearSemaphore);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}


//Comer
void comer() {
  xSemaphoreTake(comerSemaphore, portMAX_DELAY);
  while(!comerFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && comerState>0) 
    {
      comerState -= 1;
      left = false;
      taskPantalla = 5;
      miniTask = comerState;
      flagPantalla = true;
      vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    }
    else if (mid) {
      comerFlag = true;
      mid = false;
      
    }
    else if (right && comerState<2) {
      comerState += 1;
      right = false;
      taskPantalla = 5;
      miniTask = comerState;
      flagPantalla = true;
      vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    }
    Serial.print(productosComida[comerState]);
    Serial.print(comerFlag); 
  }
  sprintf(txbuffComer, productosComida[comerState].c_str());
  xQueueSend(comerQ, (void*)txbuffComer, (TickType_t)0);
  comerFlag = false;
  comerState = 0;
  xSemaphoreGive(comerSemaphore);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}


//Comprar
void comprar() {
  xSemaphoreTake(comprarSemaphore, portMAX_DELAY);
  while(!comprarFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && comprarState>0) 
    {
      comprarState -= 1;
      left = false;
      taskPantalla = 7;
      miniTask = comprarState;
      flagPantalla = true;
    }
    else if (mid) {
      comprarFlag = true;
      mid = false;
    }
    else if (right && comprarState<4) {
      comprarState += 1;
      right = false;
      taskPantalla = 7;
      miniTask = comprarState;
      flagPantalla = true;
    }
    Serial.print(productos[comprarState]);
    Serial.print(comprarFlag); 
  }
  sprintf(txbuffComprar, productos[comprarState].c_str());
  xQueueSend(comprarQ, (void*)txbuffComprar, (TickType_t)0);
  comprarFlag = false;
  comprarState = 0;
  xSemaphoreGive(comprarSemaphore);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}


int inityTarget = 42;
int initxTarget = 64;
int inity = 22;
int initx = 64;
float startMilis = millis();
float currentMilis = millis();
float deltaTime =0;
bool newStateNeeded = true;
bool hasEnded = false;
int score = 0;

void drawGame(){
  xSemaphoreTake(jugarSemaphore, portMAX_DELAY);

  while(!hasEnded) {
  currentMilis = millis();
  deltaTime = currentMilis-startMilis;
  //if (deltaTime>=100)
  //{
  display.clear();
  startMilis = millis();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  //display.drawString(30,0,String(a.acceleration.x)+","+String(a.acceleration.y)+","+String(a.acceleration.z));
    //Serial.print("toy");
    delay(50-score*0.5);
  if(newStateNeeded){
    bool flag = false;
    while(!flag){
      inityTarget = random(25,64);
      initxTarget = random(10,120);
      inity = random(25,64);
      initx = random(10,120);
      if(abs(initxTarget-initx)>=10 && abs(inityTarget-inity)>=10){
        flag = true;
        newStateNeeded = false;
        
      }
    }
  }else{

    if (a.acceleration.x > 2)
    {
      initx = initx + 1;
    }
    if (a.acceleration.x < -2)
    {
      initx = initx - 1;
    }
    if (a.acceleration.y > 2)
    {
      inity = inity + 1;
    }
    if (a.acceleration.y < -2)
    {
      inity = inity - 1;
    }
  //}
  //display.drawString(20,50,String(deltaTime));
    
    //if the circle toutches the border of the screen the game ends
    if(initx<=0 || initx>=128 || inity<=20 || inity>=64){
      hasEnded = true;
    }
    //if the circle touches the target the score increases and a new state is needed
    if(abs(initxTarget-initx)<=2 && abs(inityTarget-inity)<=2){
      score++;
      newStateNeeded = true;
    }
  

  }
    
  if(hasEnded){
    display.drawString(0,0,"GAME OVER");
    xSemaphoreGive(jugarSemaphore);
    
  }
  else{
  display.drawCircle(initxTarget,inityTarget,2);
  display.drawCircle(initx,inity,1);
  display.drawString(40,0,"Score: "+String(score));
  }
  //displaySize 128,64
  //draw a circle in randome spot of the screen
  //display.drawCircle(random(0,128),random(20,64),1);
  
  //in 30 seconds make newStateNeeded true
  
  //float currentMilis = millis();
  //float time = currentMilis-startMilis;
  //if(time>=3000){
  //newStateNeeded = true;
  //}
  display.display();

  }
  
}

//Jugar
void jugar() {
  xSemaphoreTake(jugarSemaphore, portMAX_DELAY);
  while(!jugarFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && jugarState>0) 
    {
      jugarState -= 1;
      left = false;
    }
    else if (mid) {
      jugarFlag = true;
      mid = false;
    }
    else if (right && jugarState<4) {
      jugarState += 1;
      right = false;
    }
    Serial.print(juegos[jugarState]);
    Serial.print(jugarFlag); 
  }
  sprintf(txbuffJugar, juegos[jugarState].c_str());
  xQueueSend(jugarQ, (void*)txbuffJugar, (TickType_t)0);
  jugarFlag = false;
  xSemaphoreGive(jugarSemaphore);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}

//Buttons
void buttonTaskLeft(void *p) 
{
  while(1)
  {
    left = true;
    vTaskSuspend(NULL);
    Serial.print("AmogosLeft \r\n");
    vTaskDelay(100 * configTICK_RATE_HZ / 1000);
    
  }
}

void buttonTaskMid(void *p) 
{
  while(1)
  {
    mid = true;
    vTaskSuspend(NULL);
    Serial.print("AmogosMid \r\n");
    vTaskDelay(100 * configTICK_RATE_HZ / 1000);
  }
}

void buttonTaskRight(void *p) 
{
  while(1)
  {
    right = true;
    vTaskSuspend(NULL);
    Serial.print("AmogosRight \r\n");
    vTaskDelay(100 * configTICK_RATE_HZ / 1000);
  }
}

void IRAM_ATTR buttonLeft_isr_handler(void *arg) {
  xTaskResumeFromISR(botonLeft);
}

void IRAM_ATTR buttonMid_isr_handler(void *arg) {
  xTaskResumeFromISR(botonMid);
}

void IRAM_ATTR buttonRight_isr_handler(void *arg) {
  xTaskResumeFromISR(botonRight);
}

void buttonInit() {
  gpio_pad_select_gpio(BUTTON_PINMid);
  gpio_pad_select_gpio(BUTTON_PINLeft);
  gpio_pad_select_gpio(BUTTON_PINRight);

  gpio_set_direction(BUTTON_PINMid, GPIO_MODE_INPUT);
  gpio_set_direction(BUTTON_PINLeft, GPIO_MODE_INPUT);
  gpio_set_direction(BUTTON_PINRight, GPIO_MODE_INPUT);

  gpio_set_intr_type(BUTTON_PINMid, GPIO_INTR_NEGEDGE);
  gpio_set_intr_type(BUTTON_PINLeft, GPIO_INTR_NEGEDGE);
  gpio_set_intr_type(BUTTON_PINRight, GPIO_INTR_NEGEDGE);

  gpio_install_isr_service(BUTTON_PIN_FLAG);

  gpio_isr_handler_add(BUTTON_PINMid, buttonMid_isr_handler, NULL);
  gpio_isr_handler_add(BUTTON_PINLeft, buttonLeft_isr_handler, NULL);
  gpio_isr_handler_add(BUTTON_PINRight, buttonRight_isr_handler, NULL);

  }

//depresion complementaria

void sePaso() {
  xSemaphoreTake(statsManagerSemaphore, portMAX_DELAY);
  happiness -= 0.02;
  saturation -= 0.02;
  hygiene -= 0.02;
  age += 0.01;
  xSemaphoreGive(statsManagerSemaphore);
}

void disminuirVida() {
  xSemaphoreTake(statsManagerSemaphore, portMAX_DELAY);
  int perdida = 0;
  if (happiness < 20) {
    perdida += (21-happiness)*0.01;
  }
  if (saturation < 20) {
    perdida += (21-saturation)*0.01;
  }
  if (hygiene < 20) {
    perdida += (21-hygiene)*0.01;
  }
  life -= perdida;
  xSemaphoreGive(statsManagerSemaphore);
}

void muerte() {
  happiness = 0;
  saturation = 0;
  hygiene = 0;
  age = 0;
  money = 0;
  Serial.print("kgaste");
  ESP.restart();
}

//depresion
void inevitablePasoTiempo(void *p) {
  const TickType_t unSec = 1000 / portTICK_PERIOD_MS;
  while(1)
  {
    //vTaskDelay(unSec*60);
    vTaskDelay(unSec*60);
    sePaso();
    if (life < 1) {
      muerte();
    }

  }
}

void moriohTraques() {
  xSemaphoreTake(videojuegos, portMAX_DELAY);
  while(!moriohTraquesFlag) {
    //vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    int dir = rand() % 3;
    Serial.print(dir);
    Serial.print("Amogos");
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left) 
    {
      moriohTraquesState = 0;
      left = false;
    }
    else if (mid) {
      moriohTraquesState = 1;
      mid = false;
    }
    else if (right) {
      moriohTraquesState = 2;
      right = false;
    }
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (dir != moriohTraquesState)
    {
      moriohTraquesFlag = true;
    }
    Serial.print(moriohTraquesFlag); 
    moriohTraquesCounter += 1;
  }
  char sol[50] = {(char)moriohTraquesCounter};
  sprintf(txbuffMoriohTraques, sol);
  xQueueSend(moriohTraquesQ, (void*)txbuffMoriohTraques, (TickType_t)0);
  moriohTraquesFlag = false;
  moriohTraquesCounter = 0;
  xSemaphoreGive(videojuegos);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
  
  
}

//Idle
void idle(void *p) {
  while(1) {
    Serial.print("Phrog");
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left) 
    {
      state = 1;
      left = false;
      start += 1;
    }
    else if (mid) {
      state = 2;
      mid = false;
      start += 1;
    }
    else if (right) {
      state = 3;
      right = false;
      start += 1;
    }
    else if (start < 1 && huevo1 == false) {
      huevo1 = true;
      display.clear();
      drawHuevo(0);
      vTaskDelay(100 * configTICK_RATE_HZ / 1000);
      display.display();
    }
    else if (start >= 1 && start <= 3 && huevo2 == false) {
      huevo2 = true;
      display.clear();
      drawHuevo(1);
      vTaskDelay(100 * configTICK_RATE_HZ / 1000);
      display.display();
    }
    else if(start >3 && start <= 5 && huevo3 == false) {
      huevo3 = true;
      display.clear();
      drawHuevo(2);
      vTaskDelay(100 * configTICK_RATE_HZ / 1000);
      display.display();
    }
    else if(start == 6 && huevoFinal == false){
      huevoFinal = true;
      display.clear();
      drawMain("");
      display.display();
    }
    while(state>0 && start > 6)
    {
      Serial.print("()");
      if (state == 1) {
        state = 0;
        xSemaphoreGive(pasivos);
        vTaskDelay(6000 * configTICK_RATE_HZ / 1000);
        
      }
      if (state == 2) {
        state = 0;
        taskPantalla = 1;
        miniTask = 0;
        flagPantalla = true;
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        menu();
      }
      if (state == 3) {
        state = 0;
        taskPantalla = 4;
        miniTask = 0;
        xSemaphoreGive(inventarioSemaphore);
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        miniTask = 1;
        xSemaphoreGive(inventarioSemaphore);
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        miniTask = 2;
        xSemaphoreGive(inventarioSemaphore);
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        miniTask = 3;
        xSemaphoreGive(inventarioSemaphore);
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
      }
      taskPantalla = 0;
      miniTask = 0;
      flagPantalla = true;
    }

    char rxbuff[50];
    if(xQueueReceive(actividadQ, &(rxbuff), (TickType_t)5)){
      String comparar = rxbuff;
      if(comparar == "Asear") {
        Serial.print("SONOS SAPOO");
        taskPantalla = 6;
        miniTask = 0;
        flagPantalla = true;
        asear();
      }
      else if(comparar == "Comer") {
        Serial.print("SONOS COME MOCOS");
        taskPantalla = 5;
        miniTask = 0;
        flagPantalla = true;
        comer();
      }
      else if(comparar == "Comprar") {
        Serial.print("SONOS INFLA LOS STOCKS");
        taskPantalla = 7;
        miniTask = 0;
        flagPantalla = true;
        comprar();
      }
      else if(comparar == "Jugar") {
        Serial.print("MORIOH JOGA BONITO");
        flagPantalla = false;
        drawGame();
      }
      vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    }

    char rxbuffAsear[50];
        if(xQueueReceive(asearQ, &(rxbuffAsear), (TickType_t)5)){
          String compararAseo = rxbuffAsear;
          if(compararAseo == "Jabón Rey" && cantidadAseo[0] > 0) {
            Serial.print("JAAAAAAAABOOOOOOOOON");
            happiness += 0.5;
            hygiene += 25;
            age -= 0.02;
            happiness = (happiness<0)?0:happiness;
            hygiene = (hygiene>100)?100:hygiene;
            age = (age<1)?1:age;
            cantidadAseo[0] -= 1;
          }
          else if (compararAseo == "Jabón Rey" && cantidadAseo[1] <= 0) {
            Serial.print("NO JAAAAAAAABOOOOOOOOON");
          }
          if(compararAseo == "Tío Nacho" && cantidadAseo[1] > 0) {
            Serial.print("TTTTTTTTTIOOOOOO");
            happiness -= 0.5;
            hygiene += 15;
            age -= 0.04;
            happiness = (happiness<0)?0:happiness;
            hygiene = (hygiene>100)?100:hygiene;
            age = (age<1)?1:age;
            cantidadAseo[1] -= 1;
          }
          else if (compararAseo == "Tío Nacho" && cantidadAseo[1] <= 0) {
            Serial.print("NO TIO");
          }
          taskPantalla = 0;
          miniTask = 0;
          flagPantalla = true;
          vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        }

      char rxbuffComer[50];
        if(xQueueReceive(comerQ, &(rxbuffComer), (TickType_t)5)){
          String compararComida = rxbuffComer;
          if(compararComida == "Guanabana" && cantidadComida[0] > 0) {
              happiness += 1;
              saturation += 10;
              //hygiene -= 0.02;
              //age += 0.01;
              happiness = (happiness>100)?100:happiness;
              saturation = (saturation>100)?100:saturation;
              cantidadComida[0] -= 1;
            Serial.print("EMPANAAAAADA");
          }
          else if(compararComida == "Guanabana" && cantidadComida[0] <= 0) {
            Serial.print("NO HAY Guanabana >:(");
          }
          if(compararComida == "Papaya" && cantidadComida[1] > 0) {
              happiness += 1;
              saturation += 15;
              //hygiene -= 0.02;
              //age += 0.01;
              happiness = (happiness>100)?100:happiness;
              saturation = (saturation>100)?100:saturation;
              cantidadComida[1] -= 1;
            Serial.print("Papaya CON QUESSSOOOOOOOOOO");
          }
          else if(compararComida == "Papaya" && cantidadComida[1] <= 0) {
            Serial.print("NO HAY Papaya >:(");
          }
          taskPantalla = 0;
          miniTask = 0;
          flagPantalla = true;
          vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        }

      char rxbuffComprar[50];
        if(xQueueReceive(comprarQ, &(rxbuffComprar), (TickType_t)5)){
          String compararCompra = rxbuffComprar;
          if(compararCompra == "Jabón Rey" && money > 19) {
            Serial.print("JAAAAAAAABOOOOOOOOON COMPRA");
            cantidadAseo[0] += 1;
            money -= 20;
          }
          else if(compararCompra == "Tío Nacho" && money > 79) {
            Serial.print("TTTTTTTTTIOOOOOO COMPRA");
            cantidadAseo[1] += 1;
            money -= 80;
          }
          else if(compararCompra == "Guanabana" && money > 9) {
            Serial.print("EMPANAAAAADA COMPRA");
            cantidadComida[0] += 1;
            money -= 10;
          }
          else if(compararCompra == "Papaya" && money > 34) {
            Serial.print("Papaya CON QUESSSOOOOOOOOOO COMPRA");
            cantidadComida[1] += 1;
            money -= 35;
          }
          else {
            Serial.print("POBRE");
          }
          taskPantalla = 0;
          miniTask = 0;
          flagPantalla = true;
          vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        }

      if(hasEnded){

        Serial.print(score);
        Serial.print("llegamos");
        money += score*2;
        hasEnded = false;
        inity = random(25,64);
        initx = random(10,120);
        score = 0;
          
        taskPantalla = 0;
        miniTask = 0;
        flagPantalla = true;
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);

      }
  }
}


//Setup
void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }
  Serial.println("Found a MPU-6050 sensor");

  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  buttonInit();

  taskPantalla = 0;
  miniTask = 0;
  flagPantalla = true;

  //Botones

  // Create two tasks
  pasivos = xSemaphoreCreateBinary();
  inventarioSemaphore = xSemaphoreCreateBinary();
  salir = xSemaphoreCreateMutex();
  asearSemaphore = xSemaphoreCreateMutex();
  comerSemaphore = xSemaphoreCreateMutex();
  comprarSemaphore = xSemaphoreCreateMutex();
  statsManagerSemaphore = xSemaphoreCreateMutex();
  jugarSemaphore = xSemaphoreCreateMutex();
  videojuegos = xSemaphoreCreateMutex();
  printDisplay = xSemaphoreCreateMutex();

  actividadQ = xQueueCreate(1,sizeof(txbuff));
  asearQ = xQueueCreate(1,sizeof(txbuffAsear));
  comerQ = xQueueCreate(1,sizeof(txbuffComer));
  comprarQ = xQueueCreate(1,sizeof(txbuffComprar));
  jugarQ = xQueueCreate(1,sizeof(txbuffJugar));
  moriohTraquesQ = xQueueCreate(1,sizeof(txbuffMoriohTraques));

  xTaskCreate(pantalla, "imprimirTask", 4096, NULL, 200, &imprimir);
  xTaskCreate(inevitablePasoTiempo, "tiempoTask", 4096, NULL, 10, &tiempoH);
  xTaskCreate(buttonTaskLeft, "buttonPressLeft", 4096, NULL, 10, &botonLeft);
  xTaskCreate(buttonTaskMid, "buttonPressMid", 4096, NULL, 10, &botonMid);
  xTaskCreate(buttonTaskRight, "buttonPressRight", 4096, NULL, 10, &botonRight);
  xTaskCreate(idle, "idletask", 4096, NULL, 200, &idleH);
  xTaskCreate(stats, "statsTask", 4096, NULL, 10, &statsH);
  xTaskCreate(inventory, "statsTask", 4096, NULL, 10, &inventoryH);

  // Start the FreeRTOS scheduler
  vTaskStartScheduler();

  xSemaphoreGive(salir);

  while(1) {
  }
}


void loop() {
  // This loop will not execute since FreeRTOS is running the tasks
}