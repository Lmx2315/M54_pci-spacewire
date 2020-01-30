/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "interrupts.h"
interrupts_cause st_cause;
WORD WINAPI interrupt_thread(void)
{

    int i = 0;
    unsigned int reg = 0;
 printf("Interupt_thread\n");
 clear_cause();
mcb_enable_interrupt(int_swic2_link);
mcb_enable_interrupt(int_swic1_link);

mcb_enable_interrupt(int_swic1_err);
mcb_enable_interrupt(int_swic2_err);

mcb_enable_interrupt(int_swic1_rx_desc);
mcb_enable_interrupt(int_swic1_rx_data);

mcb_enable_interrupt(int_swic2_rx_data);
mcb_enable_interrupt(int_swic2_rx_desc);
 printf("Interupts_ENABLE\n");
 while(1){


  mcb_wait_interrupt(INFINITE);
  while (st_cause.cause_fill == 0)
  {
      get_interrupt_cause();
      //printf("CAUSE EXIT \n");
  }
       for (i=0;i<INT_NUM;i++)
       {
        //printf("NUM:%d STATE:%u \n",i , st_cause.cause[i]);
        if (st_cause.cause[i] == 1)
        {

  switch (i) {
          case 0:
              //RESET CAUSE
              mcb_write(SWIC_STATUS(0), 1<<12);
              reset_event();
              printf("Port 0 connected \n");
                break;
            case 1:
              //RESET CAUSE
              mcb_write(SWIC_STATUS(0), 0xF);
              reset_event();
              printf("Port 0 connection FAIL \n");
              manager_swic_reset(0);
              swic_init(0);
                break;
          case 3:
            mcb_write(SWIC_STATUS(1), 1<<12);
            reset_event();
            printf("Port 1 connected \n");
            break;
          case 4:
              //RESET CAUSE
              mcb_write(SWIC_STATUS(1), 0xF);
              reset_event();
              printf("Port 1 connection FAIL \n");
              manager_swic_reset(1);
              swic_init(1);
                break;
          case 6:
              mcb_write(SWIC_STATUS(2), 1<<12);
              reset_event();
               printf("Port 2 connected \n");
            break;

          case 7:
              //RESET CAUSE
              mcb_write(SWIC_STATUS(2), 0xF);
              reset_event();
              printf("Port 2 connection FAIL \n");
              manager_swic_reset(2);
              swic_init(2);
                break;

            case 9:
               mcb_write(SWIC_STATUS(3), 1<<12);
               reset_event();
               printf("Port 3 connected \n");
            break;

            case 10:
              //RESET CAUSE
              mcb_write(SWIC_STATUS(3), 0xF);
              reset_event();
              printf("Port 3 connection FAIL \n");
              manager_swic_reset(3);
              swic_init(3);
                break;
            case 12:
               mcb_read_reg(SWIC_DMA_RX_DES_CSR(0));
               reset_event();
               init_rx_dma(0);
               printf("SWIC0 RX_DESC_DONE \n");
            break;

            case 13:
               mcb_read_reg(SWIC_DMA_RX_DATA_CSR(0));
               reset_event();
               init_rx_dma(0);
               printf("SWIC0 RX_DATA_DONE \n");
            break;
            case 14:
               mcb_read_reg(SWIC_DMA_TX_DES_CSR(0));
               reset_event();
               printf("SWIC0 TX_DES_DONE \n");
            break;
            case 15:
               mcb_read_reg(SWIC_DMA_TX_DATA_CSR(0));
               reset_event();
               printf("SWIC0 TX_DATA_DONE \n");
            break;

            case 16:
               mcb_read_reg(SWIC_DMA_RX_DES_CSR(1));
               reset_event();
               init_rx_dma(1);
               printf("SWIC1 RX_DESC_DONE \n");
            break;

            case 17:
               mcb_read_reg(SWIC_DMA_RX_DATA_CSR(1));
               reset_event();
               init_rx_dma(1);
               printf("SWIC1 RX_DATA_DONE \n");
            break;
            case 18:
               mcb_read_reg(SWIC_DMA_TX_DES_CSR(1));
               reset_event();
               printf("SWIC1 TX_DES_DONE \n");
            break;
            case 19:
               mcb_read_reg(SWIC_DMA_TX_DATA_CSR(1));
               reset_event();
               printf("SWIC1 TX_DATA_DONE \n");
            break;

             case 20:
               mcb_read_reg(SWIC_DMA_RX_DES_CSR(2));
               reset_event();
               init_rx_dma(2);
               printf("SWIC2 RX_DESC_DONE \n");
            break;

            case 21:
               mcb_read_reg(SWIC_DMA_RX_DATA_CSR(2));
               reset_event();
               init_rx_dma(2);
               printf("SWIC2 RX_DATA_DONE \n");
            break;
            case 22:
               mcb_read_reg(SWIC_DMA_TX_DES_CSR(2));
               reset_event();
               printf("SWIC2 TX_DES_DONE \n");
            break;
            case 23:
               mcb_read_reg(SWIC_DMA_TX_DATA_CSR(2));
               reset_event();
               printf("SWIC2 TX_DATA_DONE \n");
            break;

             case 24:
               mcb_read_reg(SWIC_DMA_RX_DES_CSR(3));
               reset_event();
               printf("SWIC3 RX_DESC_DONE \n");
            break;

            case 25:
               mcb_read_reg(SWIC_DMA_RX_DATA_CSR(3));
               reset_event();
               printf("SWIC3 RX_DATA_DONE \n");
            break;
            case 26:
               mcb_read_reg(SWIC_DMA_TX_DES_CSR(3));
               reset_event();
               printf("SWIC3 TX_DES_DONE \n");
            break;
            case 27:
               mcb_read_reg(SWIC_DMA_TX_DATA_CSR(3));
               reset_event();
               printf("SWIC3 TX_DATA_DONE \n");
            break;


          default:
              printf("ADD THIS INT PLEASE \n");
              reset_event();
  }
        }

 }
  clear_cause();

}
  printf("EXIT INT TREAD\n");
}


 int get_interrupt_cause(void)
{
    unsigned int active_int =0;
    unsigned int qstr =0;
    unsigned int mask =0;
    int i = 0;


    mcb_read_pmsc(PMSC_QSTR0_PCI, &qstr);
    mcb_read_pmsc(PMSC_MASKR0_PCI, &mask);

    active_int = qstr & mask;

   /*
    printf("QSTR: %x\n",qstr);
    printf("QMASKR: %x\n",mask
     */
   // printf("ACTIVE_INT: %x\n",active_int);

      if (active_int == 0)
      {
//          sleep(1);
          printf("READ CAUSE FAIL \n");
        printf("QSTR: %x\n",qstr);
        printf("QMASKR: %x\n",mask);
        printf("AI: %x\n",active_int);
          return 0;
      }


    for (i=0;i<INT_NUM;i++)
    {
        if ((active_int & 0x1) == 0)
        {
        active_int = active_int >> 1;
        //printf("ACTIVE_INT: %x\n",active_int);
        st_cause.cause[i]=0;
        }
        else
        {
            st_cause.cause[i]=1;
            active_int = active_int >> 1;
           // printf("ACTIVE_INT: %x\n",active_int);
        }


    }
    st_cause.cause_fill =1;
    return 1;
}

 void clear_cause(void)
 {
     int i=0;
     for (i=0;i<INT_NUM;i++)
     st_cause.cause[i]=0;
     st_cause.cause_fill =0;
 }
