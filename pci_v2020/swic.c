/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "swic.h"

void swic_init(unsigned int index_port)
{
    unsigned int reg=0;
    mcb_write(SWIC_TX_SPEED(index_port),0x302);
    
    reg = mcb_read_reg(SWIC_MODE_CR(index_port));
    reg &= 0xFFFFFFF8;
    reg |= 0x6;
    mcb_write(SWIC_MODE_CR(index_port),reg);
}

void swic_init_loop(unsigned int index_port)
{
 
    unsigned int reg=0;
    reg = mcb_read_reg(SWIC_MODE_CR(index_port)) &0xFFFF0000;
    
    mcb_write(SWIC_MODE_CR(index_port), 0x01); //Link disable
    //mcb_write(SWIC_STATUS(index_port),0xFFFFFFFF);
    mcb_write(SWIC_TX_SPEED(index_port),0x302);
    mcb_write(SWIC_MODE_CR(index_port), (reg| 0x06|(1<<12))); //Set codec Loopback mode 
}

int swic_is_connected(unsigned int index_port)
{
    unsigned int status = 0;
    status = mcb_read_reg(SWIC_STATUS(index_port));
    
    if ((status & 0x30E0) == 0x30A0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int swic_set_tx_speed(unsigned int index_port, unsigned int speed)
{
 unsigned int tx_speed = 0;
 unsigned int speed_coef = 0;

 speed_coef = speed /(MCB_XTI/2);
 
 if (speed < 5 )
     speed_coef = 0x1;
 if (speed > 400)
     speed_coef = 0x50;
 
 tx_speed = mcb_read_reg(SWIC_TX_SPEED(index_port));
 tx_speed &= 0xFFFFFF00;
 tx_speed |= (speed_coef & 0xFF);
 
 mcb_write(SWIC_TX_SPEED(index_port),tx_speed);
 
 return swic_is_connected(index_port);
}

int swic_get_rx_speed(unsigned int index_port) {
    int reg = 0;
    mcb_read(SWIC_RX_SPEED(index_port), &reg);
    reg &= 0xFF;
    //printf("mcb02 :RX_SPEED%d: 0x%x \n", index_port, reg);
    //TODO ADD PRINT in Mb/s
    return (reg);
}
int swic_set_speed(unsigned int index_port, unsigned int speed)
{
	
	
 unsigned int tx_speed = 0;
 unsigned int speed_coef = 0;

 speed_coef = speed /(MCB_XTI/2);
 
 if (speed < 5 )
     speed_coef = 0x1;
 if (speed > 400)
     speed_coef = 0x50;
 
 tx_speed = mcb_read_reg(SWIC_TX_SPEED(index_port));
 tx_speed &= 0xFFFFFF00;
 tx_speed |= (speed_coef & 0xFF);
 
 mcb_write(SWIC_TX_SPEED(index_port),tx_speed);
 mcb_write(SWIC_RX_SPEED(index_port),tx_speed);
 
 return swic_is_connected(index_port);
}
void dump_swic_regs(unsigned int index_port)
{
printf("SWIC_NUM:%u \n", index_port);    
printf("SWIC_HW_VER:%x \n", mcb_read_reg(SWIC_HW_VER(index_port)));
printf("SWIC_STATUS:%x \n", mcb_read_reg(SWIC_STATUS(index_port)));
printf("SWIC_RX_CODE:%x \n", mcb_read_reg(SWIC_RX_CODE(index_port)));
printf("SWIC_MODE_CR:%x \n", mcb_read_reg(SWIC_MODE_CR(index_port)));
printf("SWIC_TX_SPEED:%x \n", mcb_read_reg(SWIC_TX_SPEED(index_port)));
printf("SWIC_TX_CODE:%x \n", mcb_read_reg(SWIC_TX_CODE(index_port)));
printf("SWIC_RX_SPEED:%x \n", mcb_read_reg(SWIC_RX_SPEED(index_port)));
printf("SWIC_CNT_RX_PACK:%x \n", mcb_read_reg(SWIC_CNT_RX_PACK(index_port)));
printf("SWIC_CNT_RX0_PACK:%x \n", mcb_read_reg(SWIC_CNT_RX0_PACK(index_port)));
printf("SWIC_ISR_L:%x \n", mcb_read_reg(SWIC_ISR_L(index_port)));
printf("SWIC_ISR_H:%x \n", mcb_read_reg(SWIC_ISR_H(index_port)));
printf("SWIC_TRUE_TIME:%x \n", mcb_read_reg(SWIC_TRUE_TIME(index_port)));
printf("SWIC_TOUT_CODE:%x \n", mcb_read_reg(SWIC_TOUT_CODE(index_port)));
printf("SWIC_ISR_TOUT_L:%x \n", mcb_read_reg(SWIC_ISR_TOUT_L(index_port)));
printf("SWIC_ISR_TOUT_H:%x \n", mcb_read_reg(SWIC_ISR_TOUT_H(index_port)));
printf("SWIC_LOG_ADDR:%x \n", mcb_read_reg(SWIC_LOG_ADDR(index_port)));



}

