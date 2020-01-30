/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   memory_manager.h
 * Author: rkurmaev
 *
 * Created on July 3, 2019, 2:48 PM
 */

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcb02.h"
#include "tinyalloc.h"
#include "common.h"
#include "mcb_pci_driver.h"
#include "swic.h"
#define MAX_TX_DESCRIPTORS_IN_MEMORY 1
#define MAX_RX_DESCRIPTORS_IN_MEMORY 1


#define SWIC_0_EN 0
#define SWIC_1_EN 1
#define SWIC_2_EN 1
#define SWIC_3_EN 0
#define SWIC_EN (SWIC_0_EN+SWIC_1_EN+SWIC_2_EN+SWIC_3_EN)

#if SWIC_EN ==1
    #define RX_SIZE 0x20000
#elif SWIC_EN ==2
    #define RX_SIZE 0x10000
#elif SWIC_EN ==3
    #define RX_SIZE 0xAAA8
#elif SWIC_EN ==4
    #define RX_SIZE 0x8000
#endif

#define  ONE_STRUCT_SIZE (3*4) //3 int * 4 byte
#define  STRUCT_SIZE (3*4*4) //3 int * 4 byte * 4 str (2rx 2tx)


    typedef struct {
        char valid;
        char packet_type;
        unsigned int packet_size;
        unsigned int desc_address_in_mcb;
        unsigned int data_addres_in_mcb_memory;
        unsigned int *desr_ptr;
        unsigned int *data_ptr;
        char empty;
        char free;
        unsigned int descriptor;
        unsigned int dma_size;
        unsigned int data_addr_in_pc_memory;
        unsigned int fail;
    } swic_descriptor ;

     typedef struct {
        unsigned int enable;
        unsigned int connected;
        unsigned int current_tx_speed;
        unsigned int rx_speed;
        unsigned int status;
        unsigned int busy;
        unsigned int rx_mcb_addr;
        unsigned int tx_speed;
        unsigned int fail;


    } swic_port ;



    typedef struct{
        swic_descriptor descriptor[MAX_TX_DESCRIPTORS_IN_MEMORY];
        swic_port port[SWIC_NUMBER];
    } manager_main;


int init_manager ();
int get_plase(unsigned int packet_size, char packet_type,unsigned int  data_addr_in_pc_memory);
int free_plase(unsigned int free_num);
void print_manager(unsigned int num);
void print_port_status(unsigned int num,unsigned int update);
int manager_init_ports (void) ;
int manager_init_cable_loopback (unsigned int first, unsigned int second );

void manager_swic_reset(unsigned int index_port);


int aplay_tx_speed(unsigned int num);
int aplay_all_tx_speeds(void);
void manager_update_port_status(unsigned int num);
int init_rx_dma (unsigned int num);

int swic_send_packet (unsigned int index_port , unsigned int num);

#ifdef __cplusplus
}
#endif

#endif /* MEMORY_MANAGER_H */

