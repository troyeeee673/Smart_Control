#include "dht11.h"
#include "delay.h"

/**
 * @brief  DHT11复位，发送起始信号
 */
void DHT11_Rst(void)
{
    DHT11_IO_OUT();        // 设置为输出模式
    DHT11_DQ_OUT(0);       // DQ拉低
    DelayUs(20000);       // 拉低至少18ms
    DHT11_DQ_OUT(1);       // DQ拉高
    DelayUs(30);          // 主机拉高20~40us
    DHT11_IO_IN();         // 设置为输入，等待DHT11应答
}

/**
 * @brief  检测DHT11是否存在，读取应答信号
 * @retval 0:检测成功  1:检测失败
 */
u8 DHT11_Check(void)
{
    u8 retry = 0;
    while ((DHT11_DQ_IN == 1) && retry < 100) // DHT11会拉低80us
    {
        retry++;
        DelayUs(1);
    }
    if(retry >= 100) return 1;  // 超时，无应答

    retry = 0;
    while ((DHT11_DQ_IN == 0) && retry < 100) // DHT11拉高80us
    {
        retry++;
        DelayUs(1);
    }
    if(retry >= 100) return 1;

    return 0;
}

/**
 * @brief  读取DHT11的1bit数据
 * @retval 返回读到的bit值 0 / 1
 */
u8 DHT11_Read_Bit(void)
{
    u8 bit_val;
    while(DHT11_DQ_IN == 0);  // 等待低电平结束

    DelayUs(40);             // 等待40us判断电平
    if(DHT11_DQ_IN == 1)
    {
        bit_val = 1;
        while(DHT11_DQ_IN == 1); // 等待高电平结束
    }
    else
    {
        bit_val = 0;
    }
    return bit_val;
}

/**
 * @brief  读取1字节数据
 * @retval 返回读到的字节
 */
u8 DHT11_Read_Byte(void)
{
    u8 i, byte = 0;
    for(i = 0; i < 8; i++)
    {
        byte <<= 1;
        byte |= DHT11_Read_Bit();
    }
    return byte;
}

/**
 * @brief  DHT11初始化
 * @retval 0:成功 1:失败
 */
u8 DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能PA时钟

    // 先配置PA0推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    DHT11_Rst();
    return DHT11_Check();
}

/**
 * @brief 读取温湿度
 * @param temp:温度指针  humi:湿度指针
 * @retval 0读取成功，1读取失败（校验错误/无应答）
 */
u8 DHT11_Read_Data(u8 *temp,u8 *humi)
{
    u8 buf[5];
    u8 i;

    DHT11_Rst();
    if(DHT11_Check() == 0)
    {
        for(i = 0; i < 5; i++)
        {
            buf[i] = DHT11_Read_Byte();
        }
        if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) // 校验和判断
        {
            *humi = buf[0];
            *temp = buf[2];
            return 0;
        }
    }
    return 1;
}
