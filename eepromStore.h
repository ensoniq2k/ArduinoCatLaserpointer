// Next adress is always the last adress plus the size of its data type

#define ADRESS_X_MIN 0
#define ADRESS_X_MAX ADRESS_X_MIN + sizeof(uint8_t)
#define ADRESS_Y_MIN ADRESS_X_MAX + sizeof(uint8_t)
#define ADRESS_Y_MAX ADRESS_Y_MIN + sizeof(uint8_t)
#define ADRESS_FONT_TYPE ADRESS_Y_MAX + sizeof(uint8_t)

#define ADRESS_ ADRESS_FONT_TYPE + sizeof(Fonts)