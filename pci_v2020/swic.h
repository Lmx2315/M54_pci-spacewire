/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   swic.h
 * Author: rkurmaev
 *
 * Created on July 2, 2019, 4:02 PM
 */

#ifndef SWIC_H
#define SWIC_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include "erlcommon.h"
#include "mcb02.h"
#include "mcb_pci_driver.h"
    
#define DIV_BUFS_VIA_PORTS 1
#define PORTS_USED      2
    
//#define DIV_BUFS_VIA_SIZE  1
#define MAX_PACKET

#ifdef DIV_BUFS_VIA_PORTS

    
#define MAX_PACKET_LEN_BYTE (MCB02_RAM_LEN/PORTS_USED/2 - 8 )
//                RAM_SIZE / USED POTRS /2 (RX and TX buff)-8 (64 bit desc size)
#define BUFERS_AVAILABLE (PORTS_USED*2)
    

#endif
    

    

    
void swic_init(unsigned int index_port);
void swic_init_loop(unsigned int index_port);
int swic_is_connected(unsigned int index_port);
int swic_set_tx_speed(unsigned int index_port, unsigned int speed);
int swic_get_rx_speed(unsigned int index_port);
void dump_swic_regs(unsigned int index_port);

#ifdef __cplusplus
}
#endif

#endif /* SWIC_H */

