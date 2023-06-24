#define EMPTY_EEPROM_1BYTE 255
#define EMPTY_EEPROM_2BYTE 65535

// Next adress is always the last adress plus the size of its data type
#define ADRESS_X_MIN 0
#define ADRESS_X_MAX ADRESS_X_MIN + sizeof(uint8_t)
#define ADRESS_Y_MIN ADRESS_X_MAX + sizeof(uint8_t)
#define ADRESS_Y_MAX ADRESS_Y_MIN + sizeof(uint8_t)
#define ADRESS_FONT_TYPE ADRESS_Y_MAX + sizeof(uint8_t)

#define ADRESS_RUNTIME ADRESS_FONT_TYPE + sizeof(Fonts)
#define ADRESS_SLEEPTIME ADRESS_RUNTIME + sizeof(uint16_t)

#define ADRESS_LASER_BRIGHTNESS ADRESS_SLEEPTIME + sizeof(uint16_t)

#define ADRESS_WAKEUP_TIMER_ACTIVE ADRESS_LASER_BRIGHTNESS + sizeof(uint8_t)
#define ADRESS_ ADRESS_WAKEUP_TIMER_ACTIVE + sizeof(uint8_t) //Arduino handles bool as one byte in flash
