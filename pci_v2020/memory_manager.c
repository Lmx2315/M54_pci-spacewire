/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "memory_manager.h"

manager_main manager;
extern unsigned int ta[1<<28];

int init_manager ()
{
    int i = 0;
    int port_en = 0;

    manager.port[0].enable = SWIC_0_EN;
    manager.port[1].enable = SWIC_1_EN;
    manager.port[2].enable = SWIC_2_EN;
    manager.port[3].enable = SWIC_3_EN;

    for (i=0;i<SWIC_NUMBER;i++){
        manager.port[i].connected =0;
        manager.port[i].rx_speed =0;
        manager.port[i].current_tx_speed =0;
        manager.port[i].tx_speed =  50; //Stable speed for MCB02
        manager.port[i].status =0;
        manager.port[i].busy =0;
        manager.port[i].fail =0;
        manager.port[i].rx_mcb_addr =0;
        if (manager.port[i].enable == 1){
            manager.port[i].rx_mcb_addr =  MCB02_RAM_BASE + MCB02_RAM_LEN/2 + RX_SIZE*port_en ;
            port_en++;
        }

        else manager.port[i].rx_mcb_addr = 0;
    }

    i=0;

    for (i=0;i<MAX_TX_DESCRIPTORS_IN_MEMORY;i++){
    manager.descriptor[i].data_addres_in_mcb_memory = 0;
    manager.descriptor[i].desc_address_in_mcb = 0;
    manager.descriptor[i].empty =1;
    manager.descriptor[i].free =1;
    manager.descriptor[i].packet_type =1; //EOP
    manager.descriptor[i].packet_size = 0;
    manager.descriptor[i].valid = 0;
    manager.descriptor[i].descriptor =0;
    manager.descriptor[i].dma_size = 0;
    manager.descriptor[i].data_addr_in_pc_memory =0;
    manager.descriptor[i].fail =0;
    }

    ta_init(); //init for memory alloc
    if(ta_check()==1)
        return 1;
    return 0;
}


unsigned int calc_addres_in_mcb(unsigned int addr )
{
    return MCB02_RAM_BASE + (addr - TA_HEAP_START);
}


int get_plase(unsigned int packet_size, char packet_type,unsigned int  data_addr_in_pc_memory)
{
   int i = 0;
   for (i=0;i<=MAX_TX_DESCRIPTORS_IN_MEMORY;i++)
   {
     if ((manager.descriptor[i].packet_type == packet_type)&& \
             (manager.descriptor[i].packet_size == packet_size) && \
             (manager.descriptor[i].free == 1) &&  \
             (manager.descriptor[i].empty == 0))
     {
        manager.descriptor[i].free = 0;
        manager.descriptor[i].data_addr_in_pc_memory = data_addr_in_pc_memory;
        //DEBUG
        printf("Using exist number %d \n",i);
         return i;
     }
   }

   i = 0;
   for (i=0;i<=MAX_TX_DESCRIPTORS_IN_MEMORY;i++)
   {
     if (manager.descriptor[i].free == 1)
     {
         manager.descriptor[i].free = 0;

         if (manager.descriptor[i].empty == 0)
         {
           ta_free(manager.descriptor[i].desr_ptr);
           manager.descriptor[i].desr_ptr = 0;
           manager.descriptor[i].desc_address_in_mcb = 0;

           ta_free(manager.descriptor[i].data_ptr);
           manager.descriptor[i].data_ptr = 0;
           manager.descriptor[i].data_addres_in_mcb_memory = 0;

           manager.descriptor[i].empty =1;
         }
         //MEMORY ALLOC
         //DESC
         manager.descriptor[i].desr_ptr = (unsigned int*)ta_alloc(4); //8 for MCB03 4 for MCB02
         if (manager.descriptor[i].desr_ptr == 0)
         {
             manager.descriptor[i].free = 1;
             manager.descriptor[i].empty = 1;
             clear_all_free();
        //DEBUG
            printf("Not enough memory for descriptor\n");

             return -2;
         }
         manager.descriptor[i].desc_address_in_mcb = calc_addres_in_mcb((unsigned int)manager.descriptor[i].desr_ptr);
         //DATA
         manager.descriptor[i].data_ptr = (unsigned int*)ta_alloc(packet_size);
         if (manager.descriptor[i].data_ptr == 0){
             ta_free(manager.descriptor[i].desr_ptr);
             manager.descriptor[i].free = 1;
             manager.descriptor[i].empty = 1;
             clear_all_free();
             //DEBUG
             printf("Not enough memory for data\n");

             return -2;
         }
         manager.descriptor[i].data_addres_in_mcb_memory = calc_addres_in_mcb((unsigned int)manager.descriptor[i].data_ptr);

         manager.descriptor[i].packet_size = packet_size;
         manager.descriptor[i].packet_type = packet_type;
         manager.descriptor[i].descriptor =0x80000000 + ((packet_type & 3)<< 29) +  packet_size;
         if (packet_size % 4 == 0)
            manager.descriptor[i].dma_size = packet_size /4 -1;
        else
            manager.descriptor[i].dma_size = packet_size /4;

        manager.descriptor[i].data_addr_in_pc_memory = data_addr_in_pc_memory;
        manager.descriptor[i].empty = 0;
        manager.descriptor[i].valid = 1;
         return i;
     }

   }
   printf("No free SLOTS\n");
    return -1;
 }

int free_plase(unsigned int free_num)
{
    if (free_num > MAX_TX_DESCRIPTORS_IN_MEMORY)
        return -1;
    manager.descriptor[free_num].free = 1;
    return 1;
}

int clear_all_free(void)
{
    int i = 0;
    for (i=0;i<=MAX_TX_DESCRIPTORS_IN_MEMORY;i++)
   {
       if((manager.descriptor[i].free == 1) && (manager.descriptor[i].empty == 0))
        {
            ta_free(manager.descriptor[i].desr_ptr);
            ta_free(manager.descriptor[i].data_ptr);
        }

    }
}

void manager_update_port_status(unsigned int num)
{
    if (manager.port[num].enable ==0)
    {
        printf("Port using disable. Please enable it first \n");
        return;
    }
    manager.port[num].connected = swic_is_connected(num);
    manager.port[num].rx_speed = swic_get_rx_speed(num);
    manager.port[num].current_tx_speed = mcb_read_reg(SWIC_TX_SPEED(num));
    manager.port[num].status = mcb_read_reg(SWIC_STATUS(num));
}

int aplay_tx_speed(unsigned int num)
{
    if (swic_set_tx_speed(num,manager.port[num].tx_speed) == 1)
    {
        manager_update_port_status(num);
        return 1;
    }
    printf ("Speed set fail! Port number: %u\n ", num);
        return 0;
}


int aplay_all_tx_speeds(void)
{
    unsigned int i =0;
    unsigned int res =1;

    for (i=0;i<SWIC_NUMBER;i++){
    if (manager.port[i].enable ==1)
    {

       if (aplay_tx_speed(i)!=1)
           res = 0;
    }
}
    return res;
}


int manager_init_ports (void)
{
    unsigned int i =0;
    unsigned int res =1;

    for (i=0;i<SWIC_NUMBER;i++){
    if (manager.port[i].enable ==1)
    {
        manager_swic_reset(i);
        swic_init(i);
        sleep(1);
        if (swic_is_connected(i) == 0)
        {
            printf ("Connection establish fail! Port number: %u \n ", i);
            manager.port[i].connected =0;
            res --;
        }
        else
        manager.port[i].connected = 1;

        init_rx_dma (i);
    }
    }
    return res;
}


int manager_init_cable_loopback (unsigned int first, unsigned int second )
{


    if (manager.port[first].enable ==1)
    {
        manager_swic_reset(first);
        swic_init(first);
        init_rx_dma (first);

    }
    else
        return -1;

        if (manager.port[second].enable ==1)
    {
        manager_swic_reset(second);
        swic_init(second);
        init_rx_dma (second);

    }
        else
            return -1;

    sleep(1);
     if ((swic_is_connected(first) == 1) && (swic_is_connected(second) == 1))
     {
         manager.port[first].connected = 1;
         manager.port[second].connected = 1;
         return 1;
     }
     manager.port[first].connected = 0;
     manager.port[second].connected = 0;
    return 0;


}


int init_rx_dma (unsigned int num)
{
 unsigned int addr = 0;
 unsigned int wsize = 0;
 unsigned int res = 1;

 wsize = RX_SIZE /4 - MAX_RX_DESCRIPTORS_IN_MEMORY -1;
 if ((mcb_read_reg(SWIC_DMA_RX_DES_RUN(num))& 0x1)==1)
 {
     //printf("RX DES DMA %u ALRADY RUN  \n",num);
     res = 0;
 }
 if ((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(num))& 0x1)==1)
 {
     //printf("RX DATA DMA %u ALRADY RUN  \n",num);
     res =0;
     //return res;
 }
 addr = manager.port[num].rx_mcb_addr;
 mcb_write(SWIC_DMA_RX_DES_IR(num),addr);
 addr = addr + MAX_RX_DESCRIPTORS_IN_MEMORY*4;
 mcb_write(SWIC_DMA_RX_DATA_IR(num),addr);
 mcb_write(SWIC_DMA_RX_DES_CSR(num),(((MAX_RX_DESCRIPTORS_IN_MEMORY) << 16)|1)); //START RX DES DMA
 mcb_write(SWIC_DMA_RX_DATA_CSR(num),(((wsize & 0xFFFF) << 16)|1)); //START RX DATA DMA
 return res;
 }


void print_manager(unsigned int num)
{
    if (num > MAX_TX_DESCRIPTORS_IN_MEMORY)
    {
        printf("Wrong number!!! \n");
        return;
    }

    printf("\n");
    printf("Information about %u  \n",num);
    printf("\n");
    printf("\n");
    printf("Data address:0x%x \n",manager.descriptor[num].data_addres_in_mcb_memory);
    printf("Descriptor address:0x%x \n",manager.descriptor[num].desc_address_in_mcb);
    printf("Empty:%u \n",(unsigned int)manager.descriptor[num].empty);
    printf("Free:%u \n",(unsigned int)manager.descriptor[num].free);
    printf("Packet type:%u \n",(unsigned int)manager.descriptor[num].packet_type);
    printf("Packet size:%u \n",manager.descriptor[num].packet_size);
    printf("Descriptor :0x%x \n",manager.descriptor[num].descriptor);
    printf("DMA size:%u \n",manager.descriptor[num].dma_size);
    printf("Data Addr_in_pc :0x%x \n",manager.descriptor[num].data_addr_in_pc_memory);
    printf("Valid:%u \n",(unsigned int)manager.descriptor[num].valid);
 }



void print_port_status(unsigned int num,unsigned int update)
{
    if (num > SWIC_NUMBER){
        printf("Wrong number!!! \n");
        return;
    }
    printf("\n");
    printf("Information about port %u  \n",num);
    printf("\n");
    printf("\n");

    if (update ==1)
     manager_update_port_status(num);

    printf("Enable:%u \n",(unsigned int)manager.port[num].enable);
    printf("Connected:%u \n",(unsigned int)manager.port[num].connected);
    printf("RX SPEED:0x%x \n",manager.port[num].rx_speed);
    printf("CURRENT TX SPEED:0x%x \n",manager.port[num].current_tx_speed);
    printf("SET TX SPEED:%u \n",manager.port[num].tx_speed);
    printf("STATUS:0x%x \n",manager.port[num].status);
    printf("RX BUF ADDR:0x%x \n",manager.port[num].rx_mcb_addr);
    printf("Busy:%u \n",(unsigned int)manager.port[num].busy);
}



int swic_send_packet (unsigned int index_port , unsigned int num)
{
    unsigned int reg=0;
    unsigned int try=0;

    if (manager.port[index_port].busy == 0)
    {

    manager.port[index_port].busy =1;
    manager_update_port_status(index_port);

    if (manager.port[index_port].connected == 0)
    {
        printf("No link in %u for transmit package \n",index_port);
        printf("Trying reconnect port %u \n",index_port);
        manager_swic_reset(index_port);
        swic_init(index_port);
        aplay_tx_speed(index_port);

        manager_update_port_status(index_port);
        if (manager.port[index_port].connected == 0)
        {
            manager.descriptor[num].fail =1;
            manager.port[index_port].fail =1;
            manager.port[index_port].busy =0;
            //DEBUG
            manager.descriptor[num].free =1;
            return -1;
        }
    }
    //loading to memory
    //DEBUG
    //printf("BEFORE \n");
    //dump_from_mcb(manager.descriptor[num].desc_address_in_mcb,1);
    //dump_from_mcb(manager.descriptor[num].data_addres_in_mcb_memory,manager.descriptor[num].packet_size/4+1);
    //DEBUG END
    mcb_write(manager.descriptor[num].desc_address_in_mcb,manager.descriptor[num].descriptor);
    copy_to_mcb (manager.descriptor[num].data_addres_in_mcb_memory, manager.descriptor[num].data_addr_in_pc_memory,manager.descriptor[num].packet_size);
    //DEBUG
    //printf("AFTER \n");
    //dump_from_mcb(manager.descriptor[num].desc_address_in_mcb,1);
    //dump_from_mcb(manager.descriptor[num].data_addres_in_mcb_memory,manager.descriptor[num].packet_size/4+1);
    //DEBUG END

    //DMA PREPARE
    mcb_write(SWIC_DMA_TX_DES_IR(index_port),manager.descriptor[num].desc_address_in_mcb);
    mcb_write(SWIC_DMA_TX_DATA_IR(index_port),manager.descriptor[num].data_addres_in_mcb_memory);
    mcb_write(SWIC_DMA_TX_DATA_CSR(index_port),((manager.descriptor[num].dma_size & 0xFFFF) << 16));

    //DMA RUN
    mcb_write(SWIC_DMA_TX_DES_CSR(index_port),1); //RUN DMA_TX_DES

    reg = mcb_read_reg(SWIC_DMA_TX_DATA_CSR(index_port));
    reg|=1;
    mcb_write(SWIC_DMA_TX_DATA_CSR(index_port),reg); //RUN DMA_TX_DATA;
    while (((mcb_read_reg (SWIC_DMA_TX_DATA_CSR(index_port)) & 1) == 1));
    /*
    {
        if (try > 5)
        {
            printf("Sending packet %u from port %u FAIL \n",num,index_port);
            manager.descriptor[num].fail =1;
            manager.port[index_port].fail =1;
            manager.port[index_port].busy =0;
                        //DEBUG
            manager.descriptor[num].free =1;
            return -1;
        }
        usleep(2000);
        try++;
    }
     */

   manager.descriptor[num].free =1;
   manager.port[index_port].busy =0;
    return 1;
    }
 printf("PORT %u BUSY \n", index_port);
 return 0;
}

void reset_run_rx_dma(unsigned int index_port)
{
unsigned int addr=0;
unsigned int wsize = 0;
 addr = manager.port[index_port].rx_mcb_addr;
 mcb_write(SWIC_DMA_RX_DES_IR(index_port),addr);
 addr = addr + MAX_RX_DESCRIPTORS_IN_MEMORY*4;
 mcb_write(SWIC_DMA_RX_DATA_IR(index_port),addr);
 mcb_write(SWIC_DMA_RX_DES_CSR(index_port),(((MAX_RX_DESCRIPTORS_IN_MEMORY-1) << 16)));
 mcb_write(SWIC_DMA_RX_DES_RUN(index_port),0x1 ); //START RX DES DMA
 wsize = RX_SIZE /4 - MAX_RX_DESCRIPTORS_IN_MEMORY -1;
 mcb_write(SWIC_DMA_RX_DATA_CSR(index_port),(((wsize & 0xFFFF) << 16))); //START RX DATA DMA
 mcb_write(SWIC_DMA_RX_DATA_RUN(index_port),0x1 ); //START RX DATA DMA
}

void reset_send_packege(unsigned int index_port)
{
   unsigned int reset_data[2];
   reset_data[0]= 0xDEADC0DE;
   reset_data[1]= 0x12345678;
   unsigned int num = 0;
   unsigned int reg=0;
   num = get_plase(8,1,(unsigned int)&reset_data);

   mcb_write(manager.descriptor[num].desc_address_in_mcb,manager.descriptor[num].descriptor);
   copy_to_mcb (manager.descriptor[num].data_addres_in_mcb_memory, manager.descriptor[num].data_addr_in_pc_memory,manager.descriptor[num].packet_size);

   mcb_write(SWIC_DMA_TX_DES_IR(index_port),manager.descriptor[num].desc_address_in_mcb);
   mcb_write(SWIC_DMA_TX_DATA_IR(index_port),manager.descriptor[num].data_addres_in_mcb_memory);
   mcb_write(SWIC_DMA_TX_DATA_CSR(index_port),((manager.descriptor[num].dma_size & 0xFFFF) << 16));

    //DMA RUN
    mcb_write(SWIC_DMA_TX_DES_CSR(index_port),1); //RUN DMA_TX_DES
    reg = mcb_read_reg(SWIC_DMA_TX_DATA_CSR(index_port));
    reg|=1;
    mcb_write(SWIC_DMA_TX_DATA_CSR(index_port),reg); //RUN DMA_TX_DATA;
    free_plase(num);

}

void auto_reset_run_rx_dma(unsigned int index_port)
{
unsigned int addr=0;
unsigned int wsize = 0;

addr = manager.port[index_port].rx_mcb_addr + ONE_STRUCT_SIZE*2;

 //RX_DES
 //IR
 mcb_write(SWIC_DMA_RX_DES_IR(index_port),addr);
 mcb_write(manager.port[index_port].rx_mcb_addr,addr);
 //CP
mcb_write(manager.port[index_port].rx_mcb_addr + 0x4,manager.port[index_port].rx_mcb_addr);
mcb_write(SWIC_DMA_RX_DES_CP(index_port),manager.port[index_port].rx_mcb_addr);
//CSR
mcb_write(manager.port[index_port].rx_mcb_addr + 0x8,((MAX_RX_DESCRIPTORS_IN_MEMORY-7) << 16)|(1<<12)|0x1);
mcb_write(SWIC_DMA_RX_DES_CSR(index_port),((MAX_RX_DESCRIPTORS_IN_MEMORY-7) << 16)|(1<<12)|0x1); //7 = 6(size chain init struct) +1


//RX_DATA
addr = addr + MAX_RX_DESCRIPTORS_IN_MEMORY*4;
wsize = RX_SIZE /4 - MAX_RX_DESCRIPTORS_IN_MEMORY -1;
//IR
mcb_write(SWIC_DMA_RX_DATA_IR(index_port),addr);
mcb_write((manager.port[index_port].rx_mcb_addr+ONE_STRUCT_SIZE),addr);

//CP
mcb_write((manager.port[index_port].rx_mcb_addr+ONE_STRUCT_SIZE+0x4),manager.port[index_port].rx_mcb_addr+ONE_STRUCT_SIZE);
mcb_write(SWIC_DMA_RX_DATA_CP(index_port),manager.port[index_port].rx_mcb_addr+ONE_STRUCT_SIZE);

//CSR
mcb_write((manager.port[index_port].rx_mcb_addr+ONE_STRUCT_SIZE+0x8),(((wsize & 0xFFFF) << 16)|(0x1<<12)|0x1));
mcb_write(SWIC_DMA_RX_DATA_CSR(index_port),(((wsize & 0xFFFF) << 16)|(0x1<<12)|0x1));
}


void manager_swic_reset(unsigned int index_port)
{

    unsigned int rx_wcx =0;
    unsigned int reg =0;
    unsigned int num;
    unsigned int reset_data = 0x87654321;


  manager.port[index_port].busy =1;

//   printf("!!!RESET BUFERS FOR PORT %u \n", index_port);

   reg = mcb_read_reg(SWIC_MODE_CR(index_port)) & 0xFFFF0000;
   reg = reg| 0x01;
   mcb_write(SWIC_MODE_CR(index_port), reg); //DISABLE LINK
  // mcb_write(SWIC_STATUS(index_port),0xFFFFFFF); //RESET STATUS

   //STOP ALL DMA's

   /*
    * NOTE:
    * RX_DMA's must receive one or more packet's for stop.
    */


 mcb_write(SWIC_DMA_TX_DES_RUN(index_port),0); //STOP DMA_TX_DES
 mcb_write(SWIC_DMA_TX_DATA_RUN(index_port),0); //STOP DMA_TX_DATA
 mcb_write(SWIC_DMA_RX_DES_RUN(index_port),0); //STOP DMA_RX_DES
 mcb_write(SWIC_DMA_RX_DATA_RUN(index_port),0); //STOP DMA_RX_DATA

    /*
    * NOTE:
    * This function implement auto restart RX DMA's, if it's stop.
    * The function uses self-initialization chains.
    */
   auto_reset_run_rx_dma(index_port);

 while  (swic_is_connected(index_port) == 0)
 {
     swic_init_loop(index_port);
 }

  printf("WAITING LOOPBACK OK \n");


   // TX_FIFO CLAENING
   //DESC EXIST / DATA EXIST

   /*
    * NOTE:
    * Waiting for receive packet's from TX_FIFO.
    */

   usleep(100);

      num = get_plase(4,1,(unsigned int)&reset_data);


     // TX_FIFO CLAENING
//DESC EXIST / NO DATA
         while(1) //RESTART RX DMA WHILE GET DATA
   {
       rx_wcx = ((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port)) >> 16) & 0xFFFF);

      mcb_write(manager.descriptor[num].data_addres_in_mcb_memory,reset_data);
      mcb_write(SWIC_DMA_TX_DATA_IR(index_port),manager.descriptor[num].data_addres_in_mcb_memory);
      mcb_write(SWIC_DMA_TX_DATA_CSR(index_port),(0x0 << 16)|0x1);

      if (((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port)) >> 16) & 0xFFFF) == rx_wcx)
          break;
        }

  /*
  * Warning: NOW 1 32bit data in TX_FIFO without descriptor
  */

 // TX_FIFO CLAENING

 //TX_DESC DOES NOT EXIST /DATA EXIST

            while(1) //RESTART RX DMA WHILE GET DATA
   {
       rx_wcx = ((mcb_read_reg(SWIC_DMA_RX_DES_RUN(index_port)) >> 16) & 0xFFFF);
        //Write one  1 byte descriptor
       mcb_write(manager.descriptor[num].desc_address_in_mcb,(0x80000000 + (1 << 29) +  0x1)); //1 byte descriptor
       mcb_write(SWIC_DMA_TX_DES_IR(index_port),manager.descriptor[num].desc_address_in_mcb);
       mcb_write(SWIC_DMA_TX_DES_CSR(index_port),0x1); //RUN DMA_TX_DES

       if (((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port)) >> 16) & 0xFFFF) == rx_wcx)
      {
      mcb_write(manager.descriptor[num].data_addres_in_mcb_memory,reset_data);
      mcb_write(SWIC_DMA_TX_DATA_IR(index_port),manager.descriptor[num].data_addres_in_mcb_memory);
      mcb_write(SWIC_DMA_TX_DATA_CSR(index_port),(0x0 << 16)|0x1);
          break;
      }
         }

      free_plase(num);

   /*
    * NOTE:
    * RX_DMA's must receive one or more packet's for stop.
    */
 mcb_write(SWIC_DMA_RX_DES_RUN(index_port),0); //STOP DMA_RX_DES
 mcb_write(SWIC_DMA_RX_DATA_RUN(index_port),0); //STOP DMA_RX_DATA

 reset_send_packege(index_port); //send one packet to apply RX DMA's stop.

  mcb_write(SWIC_MODE_CR(index_port), 0x01); //Link disable
  mcb_write(SWIC_STATUS(index_port),0xFFFFFFFF);

 manager.port[index_port].busy =0;

}

void manager_swic_reset_temp(unsigned int index_port)
{

    unsigned int rx_wcx =0;
    manager.port[index_port].busy =1;
   printf("!!!RESET BUFERS FOR PORT %u \n", index_port);
   mcb_write(SWIC_MODE_CR(index_port), 0x01); //DISABLE LINK
   mcb_write(SWIC_STATUS(index_port),0xFFFFFFF); //RESET STATUS


   if (((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port))& 1) == 0) || ((mcb_read_reg(SWIC_DMA_RX_DES_RUN(index_port))& 1) == 0) )
       //RESTART RX DMA IF IT STOP
       //TODO ADD GET DATA FROM MEMORY
        reset_run_rx_dma(index_port);

   while(1) //RESTART RX DMA WHILE GET DATA
   {
       rx_wcx = ((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port)) >> 16) & 0xFFFF);

       printf("WCX:0x%x \n",rx_wcx);
       printf("DMA_RUN_DES  0x%x \n",((mcb_read_reg(SWIC_DMA_RX_DES_RUN(index_port)))));
       printf("DMA_RUN_DATA 0x%x \n",((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port)))));
       //sleep(1);
      if (((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port)) >> 16) & 0xFFFF) == rx_wcx)
          break;
        //TODO ADD GET DATA FROM MEMORY
       printf("0x%x \n",((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port)) >> 16) & 0xFFFF));
       reset_run_rx_dma(index_port);
   }

 mcb_write(SWIC_DMA_TX_DES_RUN(index_port),0); //STOP DMA_TX_DES
 mcb_write(SWIC_DMA_TX_DATA_RUN(index_port),0); //STOP DMA_TX_DATA

 //INIT LOOPBACK
 swic_init_loop(index_port);
 printf("WAITING LOOPBACK \n");
// printf ("STATUS FIFO 0x:%x",(mcb_read_reg(SWIC_STATUS(index_port))>> 8)&0xF);
 //while  (swic_is_connected(index_port) == 0);

 printf("WAITING LOOPBACK OK \n");
   while(1) //RESTART RX DMA WHILE GET DATA
   {
       rx_wcx = ((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port)) >> 16) & 0xFFFF);

      if (((mcb_read_reg(SWIC_DMA_RX_DATA_RUN(index_port)) >> 16) & 0xFFFF) == rx_wcx)
          break;
        //TODO ADD GET DATA FROM MEMORY

       reset_run_rx_dma(index_port);
   }

 mcb_write(SWIC_DMA_RX_DES_RUN(index_port),0); //STOP DMA_RX_DES
 mcb_write(SWIC_DMA_RX_DATA_RUN(index_port),0); //STOP DMA_RX_DATA

 reset_send_packege(index_port); //APLAY RX_DMA_STOP

  mcb_write(SWIC_MODE_CR(index_port), 0x01); //Link disable
  mcb_write(SWIC_STATUS(index_port),0xFFFFFFFF);

 manager.port[index_port].busy =0;

}




