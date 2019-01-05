/***********************************************************************************
Project:          SHT1x/7x demo program (V2.4)
Filename:         SHT1x_sample_code.c

Prozessor:        80C51 family
Compiler:         Keil Version 6.23a

Autor:            MST
Copyrigth:        (c) Sensirion AG
***********************************************************************************/
// Revisions:
// V2.4  calc_sht11()       Coefficients for humidity and temperature conversion
//                          changed (for V4 sensors)
//       calc_dewpoint()  New formula for dew point calculation

#include "HeadType.h"
#include "sht1x.h"

double log(double a)
{
    int N = 15;
    int k, nk;
    double x, xx, y;
    x = (a - 1) / (a + 1);
    xx = x * x;
    nk = 2 * N + 1;
    y = 1.0 / nk;
    for (k = N; k > 0; k--) {
        nk = nk - 2;
        y = 1.0 / nk + xx * y;

    }
    return 2.0 * x * y;
}
//=============================================================================
//函数名称: SHT10_GPIO_Config
//功能概要:LED灯引脚配置
//参数名称:无
//函数返回:无
//注意    :无
//=============================================================================
void SHT10_GPIO_Config(void)
{	
	//定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure 
	GPIO_InitTypeDef  GPIO_InitStructure;
	//使能GPIOC的外设时钟
	RCC_APB2PeriphClockCmd(SHT10_SCK_RCC,ENABLE);
	//选择要用的GPIO引脚		
	GPIO_InitStructure.GPIO_Pin = SHT10_SCK_IO	;
	///设置引脚模式为推免输出模式			 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	//设置引脚速度为50MHZ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//调用库函数，初始化GPIO
	GPIO_Init(SHT10_SCK_PORT, &GPIO_InitStructure);

}
/*******************************************************************************
* 函 数 名         : I2C_SDA_OUT
* 函数功能		   : SDA输出配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SHT10_SDA_OUT(void)
{
	//定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure 
	GPIO_InitTypeDef  GPIO_InitStructure;
	//使能GPIOC的外设时钟
	RCC_APB2PeriphClockCmd(SHT10_DATA_RCC,ENABLE);
	//选择要用的GPIO引脚		
	GPIO_InitStructure.GPIO_Pin = SHT10_DATA_IO	;
	///设置引脚模式为推免输出模式			 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	//设置引脚速度为50MHZ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//调用库函数，初始化GPIO
	GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
// 	GPIOB->MODER&=~(3<<(6*2));GPIOB->MODER|=1<<6*2;
}

/*******************************************************************************
* 函 数 名         : I2C_SDA_IN
* 函数功能		   : SDA输入配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SHT10_SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//停止按键IO配置
	RCC_APB2PeriphClockCmd(SHT10_DATA_RCC,ENABLE);		
	GPIO_InitStructure.GPIO_Pin = SHT10_DATA_IO;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
// 	GPIOB->MODER&=~(3<<(6*2));GPIOB->MODER|=0<<6*2;
}

//----------------------------------------------------------------------------------
char s_write_byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge
{
    unsigned char i, error = 0;
		SHT10_SDA_OUT();
    for (i = 0x80; i > 0; i /= 2) {   //shift bit for masking
        if (i & value) {
            DATA_HIGH();//DATA=1;          //masking value with i , write to SENSI-BUS
        } else {
            DATA_LOW();//DATA=0;
        }
        _nop_();                        //observe setup time
        SCK_HIGH();//SCK=1;                          //clk for SENSI-BUS
        _nop_();
        _nop_();
        _nop_();        //pulswith approx. 5 us
        SCK_LOW();//SCK=0;
        _nop_();                         //observe hold time
    }
    DATA_HIGH();//DATA=1;             //release DATA-line
    _nop_();                          //observe setup time
    SCK_HIGH();//SCK=1;                //clk #9 for ack
		SHT10_SDA_IN();
    error = READ_DATA();                     //check ack (DATA will be pulled down by SHT11)
    SCK_LOW();//SCK=0;
    return error;                     //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
char s_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
{
    unsigned char i, val = 0;
	  SHT10_SDA_IN();
    DATA_HIGH();//DATA=1;                           //release DATA-line
    for (i = 0x80; i > 0; i /= 2) {   //shift bit for masking
        SCK_HIGH();//SCK=1;                          //clk for SENSI-BUS
        if (READ_DATA()) {
            val = (val | i);      //read bit
        }
        SCK_LOW();//SCK=0;
    }
		SHT10_SDA_OUT();
    if (ack) {
        DATA_LOW();
    } else {
        DATA_HIGH();
    }
    //DATA=!ack;                        //in case of "ack==1" pull down DATA-Line
    _nop_();                          //observe setup time
    SCK_HIGH();//SCK=1;                            //clk #9 for ack
    _nop_();
    _nop_();
    _nop_();          //pulswith approx. 5 us
    SCK_LOW();//SCK=0;
    _nop_();                          //observe hold time
		SHT10_SDA_IN();
    DATA_HIGH();//DATA=1;                           //release DATA-line
    return val;
}

//----------------------------------------------------------------------------------
void s_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{
	  SHT10_SDA_OUT();
    DATA_HIGH();//DATA=1;
    SCK_LOW();  //SCK=0;                   //Initial state
    _nop_();
    SCK_HIGH();//SCK=1;
    _nop_();
    DATA_LOW();//DATA=0;
    _nop_();
    SCK_LOW();//SCK=0;
    _nop_();
    _nop_();
    _nop_();
    SCK_HIGH();//SCK=1;
    _nop_();
    DATA_HIGH();//DATA=1;
    _nop_();
    SCK_LOW();//SCK=0;
}

//----------------------------------------------------------------------------------
void s_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{
    unsigned char i;
	  SHT10_SDA_OUT();
    DATA_HIGH();//DATA=1;
    SCK_LOW(); //SCK=0;                    //Initial state
    for (i = 0; i < 9; i++) {         //9 SCK cycles
        SCK_HIGH();//SCK=1;
        SCK_LOW();//SCK=0;
    }
    s_transstart();                   //transmission start
}

//----------------------------------------------------------------------------------
char s_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset
{
    unsigned char error = 0;
    s_connectionreset();              //reset communication
    error += s_write_byte(RESET);     //send RESET-command to sensor
    return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{
    unsigned char error = 0;
    s_transstart();                   //transmission start
    error = s_write_byte(STATUS_REG_R); //send command to sensor
    *p_value = s_read_byte(ACK);      //read status register (8-bit)
    *p_checksum = s_read_byte(noACK); //read checksum (8-bit)
    return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_write_statusreg(unsigned char *p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{
    unsigned char error = 0;
    s_transstart();                   //transmission start
    error += s_write_byte(STATUS_REG_W); //send command to sensor
    error += s_write_byte(*p_value);  //send value of status register
    return error;                     //error>=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{
    unsigned char error = 0;
    unsigned int i;

    s_transstart();                   //transmission start
    switch (mode) {                   //send command to sensor
    case TEMP :
        error += s_write_byte(MEASURE_TEMP);
        break;
    case HUMI :
        error += s_write_byte(MEASURE_HUMI);
        break;
    default     :
        break;
    }
    for (i = 65535; i != 0 ; i--) {
        if (READ_DATA() == 0) break; //wait until sensor has finished the measurement
    }
    if (READ_DATA()) error += 1;             // or timeout (~2 sec.) is reached
    *(p_value)  = s_read_byte(ACK);   //read the first byte (MSB)
    *(p_value + 1) = s_read_byte(ACK); //read the second byte (LSB)
    *p_checksum = s_read_byte(noACK); //read checksum
    return error;
}

//----------------------------------------------------------------------------------------
void calc_sth11(float *p_humidity , float *p_temperature)
//----------------------------------------------------------------------------------------
// calculates temperature [°C] and humidity [%RH]
// input :  humi [Ticks] (12 bit)
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [°C]
{
    const float C1 = -2.0468;         // for 12 Bit RH
    const float C2 = +0.0367;         // for 12 Bit RH
    const float C3 = -0.0000015955;   // for 12 Bit RH
    const float T1 = +0.01;           // for 12 Bit RH
    const float T2 = +0.00008;        // for 12 Bit RH

    float rh = *p_humidity;           // rh:      Humidity [Ticks] 12 Bit
    float t = *p_temperature;         // t:       Temperature [Ticks] 14 Bit
    float rh_lin;                     // rh_lin:  Humidity linear
    float rh_true;                    // rh_true: Temperature compensated humidity
    float t_C;                        // t_C   :  Temperature [°C]

    t_C = t * 0.01 - 40.1;            //calc. temperature [°C] from 14 bit temp. ticks @ 5V
    rh_lin = C3 * rh * rh + C2 * rh + C1; //calc. humidity from ticks to [%RH]
    rh_true = (t_C - 25) * (T1 + T2 * rh) + rh_lin; //calc. temperature compensated humidity [%RH]
    if (rh_true > 100)rh_true = 100;  //cut if the value is outside of
    if (rh_true < 0.1)rh_true = 0.1;  //the physical possible range

    *p_temperature = t_C;             //return temperature [°C]
    *p_humidity = rh_true;            //return humidity[%RH]
}

//--------------------------------------------------------------------
float calc_dewpoint(float h, float t)
//--------------------------------------------------------------------
// calculates dew point
// input:   humidity [%RH], temperature [°C]
// output:  dew point [°C]
{
    float k, dew_point ;

    k = ((log(h) / (log(10))) - 2) / 0.4343 + (17.62 * t) / (243.12 + t);
    dew_point = 243.12 * k / (17.62 - k);
    return dew_point;
}

fsm_t read_sensor_sh1x(tag_param *p)
{
    static enum {
        STATE_START,
        STATE_READ_DATA,
        STATE_READ_RESET,
        STATE_CACULATE,
        STATE_WAITTING,
        STATE_CPL
    } s_state = STATE_START;

    unsigned char error = 0;
    unsigned char checksum;
    unsigned int  count;

    value humi_val, temp_val;
    float dew_point;

    switch (s_state) {
    case STATE_START:
    case STATE_READ_DATA:
        error = s_measure((unsigned char*) &humi_val.i, &checksum, HUMI);
        error += s_measure((unsigned char*) &temp_val.i, &checksum, TEMP);
        if (error != 0) {
            s_state = STATE_READ_RESET;
        } else {
            s_state = STATE_CACULATE;
        }
        break;
    case STATE_READ_RESET:
        s_connectionreset();
        s_state = STATE_START;
        break;
    case STATE_CACULATE:
        humi_val.f = (float)humi_val.i;
        temp_val.f = (float)temp_val.i;
        calc_sth11(&humi_val.f, &temp_val.f);

        p->temperatureC = (temp_val.f*100);
        p->humidityRH   = (humi_val.f*100);

        dew_point = calc_dewpoint(humi_val.f, temp_val.f);
        s_state = STATE_WAITTING;
        break;
    case STATE_WAITTING:
    /* wait approx. 0.8s to avoid heating up SHTxx---*/
    case STATE_CPL:
			  s_state = STATE_START;
        return fsm_cpl;
    }
    return fsm_ongoing;
}
pread_sensor read_sensor_hook;
void register_read_sensor(pread_sensor func)
{
    read_sensor_hook = func;
}
int sensor_init_sht1x(char *SerialNumber_SHT1x)
{
		SHT10_GPIO_Config();
    register_read_sensor(read_sensor_sh1x);
    s_connectionreset();
    return 0;
}
