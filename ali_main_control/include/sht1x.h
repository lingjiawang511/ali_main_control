#ifndef SHT1X_H
#define SHT1X_H

#define SHT10_READ_TIME			1000   //5s钟读一次

#define				SHT10_SCK_IO					GPIO_Pin_6 //和原理图调换一下
#define				SHT10_SCK_PORT			  GPIOB
#define				SHT10_SCK_RCC					RCC_APB2Periph_GPIOB
#define				SHT10_DATA_IO					GPIO_Pin_7
#define				SHT10_DATA_PORT				GPIOB
#define				SHT10_DATA_RCC			  RCC_APB2Periph_GPIOB


#define SCK_LOW()   GPIO_ResetBits(SHT10_SCK_PORT, SHT10_SCK_IO)
#define SCK_HIGH()  GPIO_SetBits(SHT10_SCK_PORT, SHT10_SCK_IO)
#define DATA_LOW()  GPIO_ResetBits(SHT10_DATA_PORT, SHT10_DATA_IO)
#define DATA_HIGH() GPIO_SetBits(SHT10_DATA_PORT, SHT10_DATA_IO)
#define READ_DATA() GPIO_ReadInputDataBit(SHT10_DATA_PORT,SHT10_DATA_IO)

#define _nop_() do{__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();}while(0)

typedef union {
    unsigned int i;
    float f;
} value;
typedef enum {
    fsm_error   = -1,
    fsm_cpl     = 0,
    fsm_ongoing = 1
} fsm_t;
enum {TEMP,HUMI};

typedef struct {
    int  humidityRH;       //variable for relative humidity[%RH] as float
    int  temperatureC;     //variable for temperature[脗隆脙拢C] as float
} tag_param;
typedef fsm_t (*pread_sensor)(tag_param *);

#define noACK 0
#define ACK   1
//adr  command  r/w
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0

extern pread_sensor read_sensor_hook;
extern unsigned int sht10_read_time;

void sht10_rst(void);
void sht10_star(void);
char sht10_regrst(void);
char s_write_byte(unsigned char value);
char s_read_byte(unsigned char ack);
void calc_sth11(float *p_humidity , float *p_temperature);
void s_connectionreset(void);
float calc_dewpoint(float h,float t);
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
int sensor_init_sht1x(void);
#endif
