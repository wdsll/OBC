/*
 * s32k142_tja1145.c
 *
 *  Created on: 2019��7��11��
 *      Author: Administrator
 */
#include <main.h>

uint8_t State_TJA1145_MODE;
uint8_t DataSend[2];
uint8_t DataRev[2];
void Delay(__IO uint32_t nCount)//delay��ʱ
{
 for(; nCount != 0; nCount--)
	 ;
}

void TJA1145_Init(void)
{
//	/* Initialize LPSPI0 (Send)*/
	DSPI_MasterInit(DSPI1_INSTANCE, &dspi1State, &dspi1_MasterInitConfig0);
    /* Configure delay between transfer, delay between SCK and PCS and delay between PCS and SCK */
    DSPI_MasterSetDelay(DSPI1_INSTANCE, 1, 1, 1);
	Delay(5000);

	State_TJA1145_MODE = TJA1145_NORMAL;
	TJA1145_SetModel(TJA1145_NORMAL);
}

void SPI_TransmitData(void)
{
	DSPI_MasterTransferBlocking(DSPI1_INSTANCE, DataSend, DataRev, 2, 10);//must have DataRev
	Delay(5000);
}

void TJA1145_SPISendData(uint8_t regAddr, uint8_t regData)
{
  DataSend[0] = (regAddr << 1); //register address
  DataSend[1] = regData;

  SPI_TransmitData();
}

//enStatus=false,unlock register
//enStatus=true, lock register
void TJA1145_setLockControl(uint8_t enStatus)
{
  DataSend[0] = (0x0A << 1); //register address
  if(enStatus) //lock register
	  DataSend[1] = 0x7F;
  else //unlock register
	  DataSend[1] = 0x00;

  SPI_TransmitData();
}

//TJA1145FD_CDR_50_KBIT_PER_S	= 0,
//TJA1145FD_CDR_100_KBIT_PER_S	= 1,
//TJA1145FD_CDR_125_KBIT_PER_S	= 2,
//TJA1145FD_CDR_250_KBIT_PER_S	= 3,
//TJA1145FD_CDR_500_KBIT_PER_S	= 5,
//TJA1145FD_CDR_1000_KBIT_PER_S	= 7
void TJA1145_setDataRate(uint8_t enStatus)
{
  DataSend[0] = (0x26 << 1); //register address
  DataSend[1] = enStatus;

  SPI_TransmitData();
}


//TJA1145FD_IDE_EXTENDED_FRAME_FORMAT_29_BIT	= 1,
//TJA1145FD_IDE_STANDARD_FRAME_FORMAT_11_BIT	= 0
void TJA1145_setIdentifier(uint32_t id, uint8_t idFormat)
{
  uint8_t IdReg0, IdReg1, IdReg2_0, IdReg2_1, IdReg3 = 0x0;

  if(idFormat == 1) //TJA1145FD_IDE_EXTENDED_FRAME_FORMAT_29_BIT	= 1
  {
	// configure content of identifier registers
    IdReg0 = (uint8_t) ((id & 0x000000ff));
    IdReg1 = (uint8_t) ((id & 0x0000ff00) >> 8);
    IdReg2_0 = (uint8_t) ((id & 0x00030000) >> 16);
    IdReg2_1 = (uint8_t) ((id & 0x00fc0000) >> 18);
    IdReg3 = (uint8_t) ((id & 0xff000000) >> 24);

    DataSend[0] = (0x27 << 1); //register address
    DataSend[1] = IdReg0;
    SPI_TransmitData();
    DataSend[0] = (0x28 << 1); //register address
    DataSend[1] = IdReg1;
    SPI_TransmitData();
    DataSend[0] = (0x29 << 1); //register address
    DataSend[1] = 0x00;		// start value
    DataSend[1] |= (uint8_t)(IdReg2_1 << 2);
    DataSend[1] |= (uint8_t)(IdReg2_0);
    SPI_TransmitData();
    DataSend[0] = (0x2A << 1); //register address
    DataSend[1] = IdReg3;
    SPI_TransmitData();
  }
  else //TJA1145FD_IDE_STANDARD_FRAME_FORMAT_11_BIT	= 0
  {
    // configure content of identifier registers (identifier registers 2 & 3)
    IdReg2_0 = 0x0;
    IdReg2_1 = (uint8_t) (id & 0x3F);
    IdReg3 = (uint8_t) ((id & 0x7C0) >> 6);

    DataSend[0] = (0x29 << 1); //register address
    DataSend[1] = 0x00;		// start value
    DataSend[1] |= (uint8_t)(IdReg2_1 << 2);
    DataSend[1] |= (uint8_t)(IdReg2_0);
    SPI_TransmitData();
    DataSend[0] = (0x2A << 1); //register address
    DataSend[1] = IdReg3;
    SPI_TransmitData();
  }
}


//TJA1145FD_IDE_EXTENDED_FRAME_FORMAT_29_BIT	= 1,
//TJA1145FD_IDE_STANDARD_FRAME_FORMAT_11_BIT	= 0
void TJA1145_setIdentifierMask(uint32_t id, uint8_t idFormat)
{
  uint8_t IdReg0, IdReg1, IdReg2_0, IdReg2_1, IdReg3 = 0x0;

  if(idFormat == 1) //TJA1145FD_IDE_EXTENDED_FRAME_FORMAT_29_BIT	= 1
  {
	// configure content of identifier registers
    IdReg0 = (uint8_t) ((id & 0x000000ff));
    IdReg1 = (uint8_t) ((id & 0x0000ff00) >> 8);
    IdReg2_0 = (uint8_t) ((id & 0x00030000) >> 16);
    IdReg2_1 = (uint8_t) ((id & 0x00fc0000) >> 18);
    IdReg3 = (uint8_t) ((id & 0xff000000) >> 24);

    DataSend[0] = (0x2B << 1); //register address
    DataSend[1] = IdReg0;
    SPI_TransmitData();
    DataSend[0] = (0x2C << 1); //register address
    DataSend[1] = IdReg1;
    SPI_TransmitData();
    DataSend[0] = (0x2D << 1); //register address
    DataSend[1] = 0x00;		// start value
    DataSend[1] |= (uint8_t)(IdReg2_1 << 2);
    DataSend[1] |= (uint8_t)(IdReg2_0);
    SPI_TransmitData();
    DataSend[0] = (0x2E << 1); //register address
    DataSend[1] = IdReg3;
    SPI_TransmitData();
  }
  else //TJA1145FD_IDE_STANDARD_FRAME_FORMAT_11_BIT	= 0
  {
    // configure content of identifier registers (identifier registers 2 & 3)
    IdReg2_0 = 0x0;
    IdReg2_1 = (uint8_t) (id & 0x3F);
    IdReg3 = (uint8_t) ((id & 0x7C0) >> 6);

    DataSend[0] = (0x2D << 1); //register address
    DataSend[1] = 0x00;		// start value
    DataSend[1] |= (uint8_t)(IdReg2_1 << 2);
    DataSend[1] |= (uint8_t)(IdReg2_0);
    SPI_TransmitData();
    DataSend[0] = (0x2E << 1); //register address
    DataSend[1] = IdReg3;
    SPI_TransmitData();
  }
}

//enMask = 0,set DataMask0
//enMask = 1,set DataMask1
//enMask = 2,set DataMask2
//enMask = 3,set DataMask3
//enMask = 4,set DataMask4
//enMask = 5,set DataMask5
//enMask = 6,set DataMask6
//enMask = 7,set DataMask7
void TJA1145_setDataMask(uint8_t enMask, uint8_t enValue)
{
  DataSend[0] = ((0x68 + enMask) << 1); //register address
  DataSend[1] = enValue;

  SPI_TransmitData();
}

void TJA1145_setDataDLC(uint8_t enValue)
{
  DataSend[0] = (0x2F << 1); //register address
  DataSend[1] = 0x40;		// start value PNDM=1
  DataSend[1] |= (uint8_t)(enValue);

  SPI_TransmitData();
}


void TJA1145_SetModel(uint8_t model)
{
	/* Initialize LPSPI0 (Send)*/
	//SPI_MasterInit(&spi1Instance, &spi1_MasterConfig0);
	/* Configure delay between transfer, delay between SCK and PCS and delay between PCS and SCK */
	//DSPI_MasterSetDelay(SPI3_INSTANCE, 1, 1, 1);
	//Delay(5000);
	State_TJA1145_MODE = model;

    if(model == TJA1145_STANDBY)
	{
    	TJA1145_setLockControl(false);
		TJA1145_SPISendData(0x01, 0x04);//goto standby
		TJA1145_setLockControl(true);
	}
	else if(model == TJA1145_NORMAL)
	{
		TJA1145_setLockControl(false);
		TJA1145_setDataRate(5);//set 500Kbps
		TJA1145_SPISendData(0x01, 0x07);//got normal
		TJA1145_SPISendData(0x20, 0x02);//CAN Active
		TJA1145_setLockControl(true);
	}
	else if(model == TJA1145_SLEEP)
	{
		TJA1145_setLockControl(false);
		// TJA1145_SPISendData(0x20, 0x40);//Enable CAN-FD,Any CAN Message
		// TJA1145_SPISendData(0x23, 0x01);//Enable CAN-Wake up

		TJA1145_setDataRate(5);//set 500Kbps
		TJA1145_setIdentifier(0x557,0);//set receive can id
		TJA1145_setDataMask(0,0x08);//DATA MASK0  Filter Data = Byte[0]_Bit3 = 1, Data[0] = 0x08
		TJA1145_setDataDLC(1);//DLC=1
		TJA1145_SPISendData(0x20, 0x72);//Enable CAN-FD,Enable Parameter
		TJA1145_SPISendData(0x23, 0x01);//Enable CAN-Wake up

		TJA1145_SPISendData(0x60, 0xFF);//Clear Event Flag
		TJA1145_SPISendData(0x61, 0xFF);//Clear Event Flag
		TJA1145_SPISendData(0x63, 0xFF);//Clear Event Flag
		TJA1145_SPISendData(0x64, 0xFF);//Clear Event Flag
		TJA1145_SPISendData(0x01, 0x01);//goto Sleep
		TJA1145_setLockControl(true);
		SET_PIN_SELF_LOCK_L();
	}
}







