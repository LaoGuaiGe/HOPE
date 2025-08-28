#include "lsm6ds3.h"
//#include "bsp_i2c.h"
//#include "hw_uart.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "myiic.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"


Angle angle_new;

//����ʱ��20ms
const static float dt = 0.02f;

static void delay_syms(long X)     
{
	delay_ms(X);
}

/*******************************************************************************
 * ��������LSM6DS3TRC_ReadOneByte
 * ����  ����LSM6DS3TRCָ����ַ����ʼ��ȡһ���ֽ�����
 * ����  ��reg_addr��ַ
 * ���  ����ȡ������dat
 * ����  ��
 * ��ע  ��
 *******************************************************************************/
unsigned char lsm6ds3_read_one_byte(unsigned char reg_addr)
{
	uint8_t dat = 0;	
		
    IIC_Start();//������ʼ�ź�
    
    IIC_Send_Byte((LSM6DS3TRC_I2CADDR<<1) | 0x00);//���豸��ַ    
    delay_syms(1);	
    		
  	if(IIC_Wait_Ack())	/* ����豸��ACKӦ�� */
	{
		IIC_Stop();//����һ��ֹͣ����
	}
	
    IIC_Send_Byte(reg_addr);//�Ĵ�����ַ
    delay_syms(1);			
    
  	if(IIC_Wait_Ack())	/* ����豸��ACKӦ�� */
	{
		 IIC_Stop();//����һ��ֹͣ����	  			
	}

		
    IIC_Start();//�����ظ���ʼ�źţ�׼����ȡ����
    
	IIC_Send_Byte((LSM6DS3TRC_I2CADDR<<1) | 0x01);//���豸��ַ����ȡģʽ��
    delay_syms(1);	
    		
  	if(IIC_Wait_Ack())	/* ����豸��ACKӦ�� */
	{
		IIC_Stop();//����һ��ֹͣ����	  			
	}
	
    dat = IIC_Read_Byte(0);
    
	IIC_Stop();//����ֹͣ�ź�	
	  
    return dat;		
}

/*******************************************************************************
 * ��������lsm6ds3_ReadCommand
 * ����  ����LSM6DS3TRC��ȡ����
 * ����  ��uint8_t reg_addr, uint8_t *rev_data, uint8_t length
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
void lsm6ds3_read_command(uint8_t reg_addr, uint8_t *rev_data, uint8_t length)
{	
	while(length)
	{
		*rev_data++ = lsm6ds3_read_one_byte(reg_addr++);
		length--; 
	}		
}

/*******************************************************************************
 * ��������lsm6ds3_WriteCommand
 * ����  ����LSM6DS3TRCд������
 * ����  ��uint8_t reg_addr, uint8_t *send_data, uint16_t length
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
void lsm6ds3_write_command(uint8_t reg_addr, uint8_t *send_data, uint16_t length)
{
	IIC_Start();	
		
	delay_syms(10);	

	IIC_Send_Byte((LSM6DS3TRC_I2CADDR<<1) | 0x00);//�����豸��ַ	

	if(IIC_Wait_Ack())	/* ����豸��ACKӦ�� */
	{
		IIC_Stop();//����һ��ֹͣ����	  								
	}
	else
	{	
	}	
		
	delay_syms(10);			
			
	IIC_Send_Byte(reg_addr);//���ͼĴ�����ַ	

	delay_syms(10);	

	if(IIC_Wait_Ack())	/* ����豸��ACKӦ�� */
	{
		IIC_Stop();//����һ��ֹͣ����	  					
	}
	else
	{	
	}	
			
	delay_syms(10);		
			
	IIC_Send_Byte(*send_data);//��������		

	delay_syms(10);	

	if(IIC_Wait_Ack())	/* ����豸��ACKӦ�� */
	{
		IIC_Stop();//����һ��ֹͣ����	  						
	}
	else
	{	
	}	
			
	delay_syms(10);	

	IIC_Stop();//����һ��ֹͣ����	
}

/*******************************************************************************
 * ��������IIC_CheckDevice
 * ����  �����I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
 * ����  ��_Address���豸��I2C���ߵ�ַ
 * ���  ��
 * ����  ��
 * ��ע  ��
 *******************************************************************************/
uint8_t i2c_check_device(uint8_t _Address)
{
	uint8_t ucAck;
 
	
    IIC_Start();		/* ���������ź� */

    IIC_Send_Byte(_Address );
    ucAck = IIC_Wait_Ack();	/* ����豸��ACKӦ�� */

    IIC_Stop();			/* ����ֹͣ�ź� */

    return ucAck;

}

/*******************************************************************************
 * ��������lsm6ds3_CheckOk
 * ����  ���ж�LSM6DS3TRC�Ƿ�����
 * ����  ��void
 * ���  �� 1 ��ʾ������ 0 ��ʾ������
 * ����  ��
 * ��ע  ��
 *******************************************************************************/
uint8_t lsm6ds3_check_ok(void)
{
	if(i2c_check_device( LSM6DS3TRC_I2CADDR ) == 1)
	{
	    //printf("Device exist\r\n");
		return 1;
	}
	else
	{
		/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
	    //printf("Device not exist\r\n");		
		IIC_Stop();
		return 0;
	}
}

/*******************************************************************************
 * ��������lsm6ds3_GetChipID
 * ����  ����ȡLSM6DS3TRC����ID
 * ����  ��void
 * ���  ������ֵtrue��ʾ0x6a������false��ʾ����0x6a
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
uint8_t lsm6ds3_get_Chip_id(void)
{
	uint8_t buf = 0;

	lsm6ds3_read_command(LSM6DS3TRC_WHO_AM_I, &buf, 1);//Who I am ID
	//printf("buf 0x%02X\r\n",buf);		
	if (buf == 0x6a)
	{
	    //printf("ID ok\r\n");	
		return 1;
	}
	else
	{
	    //printf("ID error\r\n");	
		return 0;
	}
}

/*******************************************************************************
 * ��������lsm6ds3_Reset
 * ����  ��LSM6DS3TRC���������üĴ���
 * ����  ��void
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
void lsm6ds3_reset(void)
{
	uint8_t buf[1] = {0};
	//reboot modules
	buf[0] = 0x80;
	lsm6ds3_write_command(LSM6DS3TRC_CTRL3_C, buf, 1);//BOOT->1
    delay_syms(15);
	//reset register
	lsm6ds3_read_command(LSM6DS3TRC_CTRL3_C, buf, 1);//��ȡSW_RESET״̬
	buf[0] |= 0x01;
	lsm6ds3_write_command(LSM6DS3TRC_CTRL3_C, buf, 1);//��CTRL3_C�Ĵ�����SW_RESETλ��Ϊ1
	while (buf[0] & 0x01)
	lsm6ds3_read_command(LSM6DS3TRC_CTRL3_C, buf, 1);//�ȵ�CTRL3_C�Ĵ�����SW_RESETλ����0
}

/*******************************************************************************
 * ��������lsm6ds3_Set_BDU
 * ����  ��LSM6DS3TRC���ÿ����ݸ���
 * ����  ��uint8_t flag
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
void lsm6ds3_set_BDU(uint8_t flag)
{
	uint8_t buf[1] = {0};
	lsm6ds3_read_command(LSM6DS3TRC_CTRL3_C, buf, 1);

	if (flag == 1)
	{
		buf[0] |= 0x40;//����BDU
		lsm6ds3_write_command(LSM6DS3TRC_CTRL3_C, buf, 1);
	}
	else
	{
		buf[0] &= 0xbf;//����BDU
		lsm6ds3_write_command(LSM6DS3TRC_CTRL3_C, buf, 1);
	}

	lsm6ds3_read_command(LSM6DS3TRC_CTRL3_C, buf, 1);
}

/*******************************************************************************
 * ��������lsm6ds3_Set_Accelerometer_Rate
 * ����  ��LSM6DS3TRC���ü��ٶȼƵ����ݲ�����
 * ����  ��uint8_t rate
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
void lsm6ds3_set_accelerometer_rate(uint8_t rate)
{
	uint8_t buf[1] = {0};
	lsm6ds3_read_command(LSM6DS3TRC_CTRL1_XL, buf, 1);
	buf[0] |= rate;//���ü��ٶȼƵ����ݲ�����
	lsm6ds3_write_command(LSM6DS3TRC_CTRL1_XL, buf, 1);
}

/*******************************************************************************
 * ��������lsm6ds3_Set_Gyroscope_Rate
 * ����  ��LSM6DS3TRC������������������
 * ����  ��uint8_t rate
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
void lsm6ds3_set_gyroscope_rate(uint8_t rate)
{
	uint8_t buf[1] = {0};
	lsm6ds3_read_command(LSM6DS3TRC_CTRL2_G, buf, 1);
	buf[0] |= rate;//������������������
	lsm6ds3_write_command(LSM6DS3TRC_CTRL2_G, buf, 1);
}

/*******************************************************************************
 * ��������lsm6ds3_Set_Accelerometer_Fullscale
 * ����  ��LSM6DS3TRC���ٶȼ�������ѡ��
 * ����  ��uint8_t value
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
void lsm6ds3_set_accelerometer_fullscale(uint8_t value)
{
	uint8_t buf[1] = {0};
	lsm6ds3_read_command(LSM6DS3TRC_CTRL1_XL, buf, 1);
	buf[0] |= value;//���ü��ٶȼƵ�������
	lsm6ds3_write_command(LSM6DS3TRC_CTRL1_XL, buf, 1);
}

/*******************************************************************************
 * ��������lsm6ds3_Set_Gyroscope_Fullscale
 * ����  ��LSM6DS3TRC������������ѡ��
 * ����  ��uint8_t value
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
void lsm6ds3_set_gyroscope_fullscale(uint8_t value)
{
	uint8_t buf[1] = {0};
	lsm6ds3_read_command(LSM6DS3TRC_CTRL2_G, buf, 1);
	buf[0] |= value;//���������ǵ�������
	lsm6ds3_write_command(LSM6DS3TRC_CTRL2_G, buf, 1);
}

/*******************************************************************************
 * ��������LSM6DS3TRC_Set_Accelerometer_Bandwidth
 * ����  ��LSM6DS3TRC���ü��ٶȼ�ģ��������
 * ����  ��uint8_t BW0XL, uint8_t ODR
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��BW0XLģ��������, ODR���������
 *******************************************************************************/
void lsm6ds3_set_accelerometer_bandwidth(uint8_t BW0XL, uint8_t ODR)
{
	uint8_t buf[1] = {0};
	lsm6ds3_read_command(LSM6DS3TRC_CTRL1_XL, buf, 1);
	buf[0] |= BW0XL;
	lsm6ds3_write_command(LSM6DS3TRC_CTRL1_XL, buf, 1);

	lsm6ds3_read_command(LSM6DS3TRC_CTRL8_XL, buf, 1);
	buf[0] |= ODR;
	lsm6ds3_write_command(LSM6DS3TRC_CTRL8_XL, buf, 1);
}

/*******************************************************************************
 * ��������LSM6DS3TRC_Get_Status
 * ����  ����LSM6DS3TRC״̬�Ĵ�����ȡ����״̬
 * ����  ��void
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��
 *******************************************************************************/
uint8_t lsm6ds3_get_status(void)
{
	uint8_t buf[1] = {0};
	lsm6ds3_read_command(LSM6DS3TRC_STATUS_REG, buf, 1);
	return buf[0];
}



/*******************************************************************************
 * ��������LSM6DS3TRC_Get_Acceleration
 * ����  ����LSM6DS3TRC��ȡ���ٶȼ�����
 * ����  ��uint8_t fsxl, float *acc_float
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��ת��Ϊ�������ļ��ٶ�ֵ
 *******************************************************************************/
void lsm6ds3_get_acceleration(uint8_t fsxl, float *acc_float)
{
	uint8_t buf[6];
	int16_t acc[3];
	lsm6ds3_read_command(LSM6DS3TRC_OUTX_L_XL, buf, 6);//��ȡ���ٶȼ�ԭʼ����
	acc[0] = buf[1] << 8 | buf[0];
	acc[1] = buf[3] << 8 | buf[2];
	acc[2] = buf[5] << 8 | buf[4];

	switch (fsxl)//���ݲ�ͬ������ѡ����������ݵ�ת��ϵ��
	{
	case LSM6DS3TRC_ACC_FSXL_2G:
		acc_float[0] = ((float)acc[0] * 0.061f);
		acc_float[1] = ((float)acc[1] * 0.061f);
		acc_float[2] = ((float)acc[2] * 0.061f);		
		break;

	case LSM6DS3TRC_ACC_FSXL_16G:
		acc_float[0] = ((float)acc[0] * 0.488f);
		acc_float[1] = ((float)acc[1] * 0.488f);
		acc_float[2] = ((float)acc[2] * 0.488f);
		break;

	case LSM6DS3TRC_ACC_FSXL_4G:
		acc_float[0] = ((float)acc[0] * 0.122f);
		acc_float[1] = ((float)acc[1] * 0.122f);
		acc_float[2] = ((float)acc[2] * 0.122f);
		break;

	case LSM6DS3TRC_ACC_FSXL_8G:
		acc_float[0] = ((float)acc[0] * 0.244f);
		acc_float[1] = ((float)acc[1] * 0.244f);
		acc_float[2] = ((float)acc[2] * 0.244f);
		break;
	}
}

/*******************************************************************************
 * ��������LSM6DS3TRC_Get_Gyroscope
 * ����  ����LSM6DS3TRC��ȡ����������
 * ����  ��uint8_t fsg, float *gry_float
 * ���  ��void
 * ����  ���ڲ�����
 * ��ע  ��ת��Ϊ�������Ľ��ٶ�ֵ
 *******************************************************************************/
void lsm6ds3_get_gyroscope(uint8_t fsg, float *gry_float)
{
	uint8_t buf[6];
	int16_t gry[3];
	
	lsm6ds3_read_command(LSM6DS3TRC_OUTX_L_G, buf, 6);//��ȡ������ԭʼ����

	gry[0] = buf[1] << 8 | buf[0];
	gry[1] = buf[3] << 8 | buf[2];
	gry[2] = buf[5] << 8 | buf[4];		
	switch (fsg)//���ݲ�ͬ������ѡ����������ݵ�ת��ϵ��
	{
	case LSM6DS3TRC_GYR_FSG_245:
		gry_float[0] = ((float)gry[0] * 8.750f);
		gry_float[1] = ((float)gry[1] * 8.750f);
		gry_float[2] = ((float)gry[2] * 8.750f);
		break;
	case LSM6DS3TRC_GYR_FSG_500:
		gry_float[0] = ((float)gry[0] * 17.50f);
		gry_float[1] = ((float)gry[1] * 17.50f);
		gry_float[2] = ((float)gry[2] * 17.50f);
		break;
	case LSM6DS3TRC_GYR_FSG_1000:
		gry_float[0] = ((float)gry[0] * 35.00f);
		gry_float[1] = ((float)gry[1] * 35.00f);
		gry_float[2] = ((float)gry[2] * 35.00f);
		break;
	case LSM6DS3TRC_GYR_FSG_2000:
		gry_float[0] = ((float)gry[0] * 70.00f);
		gry_float[1] = ((float)gry[1] * 70.00f);
		gry_float[2] = ((float)gry[2] * 70.00f);
		break;
	}
}

// ������У׼����,��С���Ư��
static float gyro_zero_z = 0.0f;
static void lsm6ds3_soft_calibrate_z0(void) 
{
    uint16_t calibration_samples = 500;//У׼������
    float gz_sum = 0.0f;
    int16_t GyroZ;
    uint8_t buf[2]={0};
    uint16_t i;
    uint8_t status = 0;

    for (i = 0; i < calibration_samples; i++) 
    {
        ///////////����Z��ı任���ɽ�������/////////
        status = lsm6ds3_get_status();
        if( status & LSM6DS3TRC_STATUS_GYROSCOPE )
        {
            // ��ȡZ������
            lsm6ds3_read_command(LSM6DS3TRC_OUTZ_L_G, buf, 2);
            GyroZ = buf[1] << 8 | buf[0];
            gz_sum += (float)GyroZ;
        }
        delay_syms(20);//Ҫ��dtͬ��
    }
    gyro_zero_z = gz_sum / calibration_samples;
}

//�ַ�˳���ת
static void reverse(char *str, int len) {
    int i;
    for (i= 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}
//����ת�ַ���
static void int_to_str(int num, char *str) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
    } else {
        while (num > 0) {
            str[i++] = '0' + (num % 10);
            num /= 10;
        }
    }
    str[i] = '\0';
    reverse(str, i);
}
//������ת�ַ���
void float_to_string(float num, char *str) {
    char int_str[20];
    char dec_str[7]; // �洢6λС��

    int str_index = 0;

    // ������
    if (num < 0) {
        str[str_index++] = '-';
        num = -num;
    }

    float num_abs = num;
    int int_part = (int)num_abs;
    float decimal_part = num_abs - (float)int_part;

    // ת����������
    int_to_str(int_part, int_str);
    int int_len = strlen(int_str);
    memcpy(str + str_index, int_str, int_len);
    str_index += int_len;

    // ת��С������
    int i;
    for ( i = 0; i < 2; i++) {
        decimal_part *= 10.0f;
        int digit = (int)decimal_part;
        dec_str[i] = '0' + digit;
        decimal_part -= (float)digit;
    }
    dec_str[2] = '\0';

    // ȥ��ĩβ����
    int decimal_len = 2;
    while (decimal_len > 0 && dec_str[decimal_len - 1] == '0') {
        decimal_len--;
    }

    // ���С�������ЧС��λ
    if (decimal_len > 0) {
        str[str_index++] = '.';
        memcpy(str + str_index, dec_str, decimal_len);
        str_index += decimal_len;
    }

    str[str_index] = '\0'; // ��ֹ��
}

float acc[3] = {0,0,0};
float gyr[3] = {0,0,0};

#define Kp 500.0f
#define Ki 0.005f

float halfT=0.001f;
float q0=1, q1 = 0, q2 = 0, q3 = 0;
float exInt = 0, eyInt = 0, ezInt = 0;

float z_offset = 0.0922f;//�ֶ�ƫ�� 2000
//float z_offset = 0.0825f;//�ֶ�ƫ�� 

//�ο��ԣ�https://www.bilibili.com/video/BV1MP411i7gy?spm_id_from=333.788.player.switch&bvid=BV1MP411i7gy&vd_source=f31bbb273c619a995d0a639f7f6cbc90
//pitch��rollЧ���ܺã����ǻ��Ǵ���������+yaw������ƫ��
//YAWƯ������ο�������:
//https://www.bilibili.com/video/BV1z2VzzhEUR/?spm_id_from=333.1007.top_right_bar_window_default_collection.content.click&vd_source=f31bbb273c619a995d0a639f7f6cbc90

//angle->z += kalman_filter_update(&kalmanZ, gyr[2]*3.0f+z_offset); //2000������
//angle->z += kalman_filter_update(&kalmanZ, gyr[2]/3.0f+z_offset); //245������

void lsm6ds3_get_angle(Angle* angle)
{
    int i = 0;
    uint8_t status = 0;
    float norm;
    float vx,vy,vz;
    float ex,ey,ez;

    status = lsm6ds3_get_status();

    if( (status&LSM6DS3TRC_STATUS_ACCELEROMETER) && (status&LSM6DS3TRC_STATUS_GYROSCOPE) )
    {
        lsm6ds3_get_acceleration(LSM6DS3TRC_ACC_FSXL_2G, acc);		     
        for( i = 0; i < 3; i++ )
        {
                acc[i] = acc[i] / 1000.0f;
        }
        lsm6ds3_get_gyroscope(LSM6DS3TRC_GYR_FSG_2000, gyr);		
        for( i = 0; i < 3; i++ )
        {   
            if( i == 2 ) 
            {
                gyr[i] = gyr[i] - (int16_t)gyro_zero_z;
            }
            gyr[i] = gyr[i] /1000.0f;      
        }
        //��������������ά������Ϊ��λ����
        norm = sqrt(acc[0]*acc[0] + acc[1]*acc[1] + acc[2]*acc[2]);
        acc[0] = acc[0] / norm;//��λ��
        acc[1] = acc[1] / norm;
        acc[2] = acc[2] / norm;
        //���Ʒ��������
        vx = 2* (q1*q3 - q0*q2);
        vy = 2* (q0*q1 + q2*q3);
        vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

        ex = (acc[1]*vz - acc[2]*vy);
        ey = (acc[2]*vx - acc[0]*vz);
        ez = (acc[0]*vy - acc[1]*vx);

        //������������������
        exInt = exInt + ex*Ki;
        eyInt = eyInt + ey*Ki;
        ezInt = ezInt + ez*Ki;
        //������������ǲ���
        gyr[0] = gyr[0] + Kp*ex + exInt;
        gyr[1] = gyr[1] + Kp*ey + eyInt;
        gyr[2] = gyr[2] + Kp*ez + ezInt;        
        //������Ԫ���ʺ�������
        q0 = q0 + (-q1*gyr[0] - q2*gyr[1]- q3*gyr[2])*halfT;
        q1 = q1 + (q0*gyr[0] + q2*gyr[2] - q3*gyr[1])*halfT;
        q2 = q2 + (q0*gyr[1] - q1*gyr[2] + q3*gyr[0])*halfT;
        q3 = q3 + (q0*gyr[2] + q1*gyr[1] - q2*gyr[0])*halfT;
        //��������Ԫ��
        norm= sqrt(q0*q0+ q1*q1+ q2*q2+ q3*q3);
        q0 = q0 / norm;//w
        q1 = q1 / norm;//x
        q2 = q2 / norm;//y
        q3 = q3 / norm;//z

        angle->x = asin(2 * q2 * q3 + 2 * q0 * q1 ) * 57.3;
        angle->y =atan2(-2 * q1 * q3 + 2 * q0 * q2, q0*q0-q1*q1-q2*q2+q3*q3)*57.3;
        // angle->x = asin(2 * q2 * q3 + 2 * q0 * q1 ) * 57.3;
        // angle->y = atan2(-2 * q1 * q3 + 2 * q0 * q2, q0*q0-q1*q1-q2*q2+q3*q3)*57.3;
        // angle->z = atan2(2*(q1*q2 - q0*q3), q0*q0-q1*q1+q2*q2-q3*q3) * 57.3;

        gyr[2] = gyr[2] * dt;
        angle->z +=  gyr[2] * 3.0f + z_offset;
    }
}
//�Ƕȹ���
void lsm6ds3_angle_return_zero(void)
{
    angle_new.x = 0;
    angle_new.y = 0;
    angle_new.z = 0;
    lsm6ds3_reset();
}

unsigned char lsm6ds3_init(void)
{
	// ��ʼ��IIC����
	IIC_Init();
	
    //����豸�Ƿ����
    if( lsm6ds3_check_ok() == 0 ) return 1;

    //�豸����
    lsm6ds3_reset();

    //�ڶ�ȡMSB��LSB֮ǰ����������Ĵ���
    lsm6ds3_set_BDU(1);

    //���ü��ٶȼƵ����ݲ����� 1/52=19.2ms
    lsm6ds3_set_accelerometer_rate(LSM6DS3TRC_ACC_RATE_52HZ);

    //���������ǵ����ݲ����� 1/52=19.2ms
    lsm6ds3_set_gyroscope_rate(LSM6DS3TRC_GYR_RATE_52HZ);

    //���ü��ٶȼ�������ѡ��
    lsm6ds3_set_accelerometer_fullscale(LSM6DS3TRC_ACC_FSXL_2G);

    //����������ȫ����ѡ��
    lsm6ds3_set_gyroscope_fullscale(LSM6DS3TRC_GYR_FSG_2000);

    //���ü��ٶȼ�ģ��������
    lsm6ds3_set_accelerometer_bandwidth(LSM6DS3TRC_ACC_BW0XL_400HZ, LSM6DS3TRC_ACC_LOW_PASS_ODR_100);
    
    delay_syms(100);

//    //���У׼,����yaw�����Ư��
//    lsm6ds3_soft_calibrate_z0();
    return 0;
}

