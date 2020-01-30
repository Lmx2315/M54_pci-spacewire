/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "common.h"

void FillHandShake(unsigned int array,unsigned char dev_number,unsigned char port_number) 
{
    
    *((volatile unsigned char *)array) = port_number;
    *((volatile unsigned char *)array+1) = 0xFF;
    *((volatile unsigned char *)array+2)= 0xFF;
    *((volatile unsigned char *)array+3) = 0xFF;
    *((volatile unsigned char *)array+4)= dev_number+48;
}

void FillArray(unsigned int *array, unsigned int len, int first) {
        unsigned int i;

        for (i=0;i<len;i++) {
                array[i] = i+first;
        }
}

void FillArray_char(unsigned char *array, unsigned int len) {
        unsigned int i;
        unsigned char val;

        for (i=0;i<len;i++) {
                val = i%255;
                array[i] = val;
        }
}

void FillArray_char_comm(unsigned char *array, unsigned int len,unsigned char port) {
        unsigned int i;
        unsigned char val;
        array[0]= port;
        for (i=1;i<len;i++) {
                val = i%255;
                array[i] = val;
        }
}

int VerifyArray_Char(unsigned int *array_one,unsigned int *array_two, unsigned int len) {
        unsigned int i;

        for (i=0;i<len;i++) {
                if(*((volatile unsigned char *)array_one+i)!= *((volatile unsigned char *)array_two+i)) return 0;
        }
        return 1;
}

int VerifyArray_Char_Comm(unsigned int *array_one,unsigned int *array_two, unsigned int len) {
        unsigned int i;

        for (i=0;i<len-1;i++) {
                if(*((volatile unsigned char *)array_one+i)!= *((volatile unsigned char *)array_two+i+1)) return 0;
        }
        return 1;
}


enum ERL_ERROR copy_to_mcb(unsigned int addr_mcb, unsigned int addr, unsigned int size)
{
    int i = 0;
    if (addr_mcb & 0x3 !=0)
    {
        return ERL_UNITIALIZED_ARG;
    }
    
    for(i=0;i<size/4;i++)
    {
        mcb_write((addr_mcb+i*4),*((volatile unsigned int *)addr+i));   
    }
    }
    
enum ERL_ERROR dump_from_mcb(unsigned int addr_mcb,unsigned int size)
{
    int i=0;
    unsigned int reg = 0;
    for(i=0;i<size;i++)
    {
        mcb_read(addr_mcb+i*4, &reg);
        printf("0x%08X:0x%08X \n",addr_mcb +i*4,reg);
    }  
}

enum ERL_ERROR copy_from_mcb(unsigned int addr_mcb, void *addr, unsigned int size)
{
    unsigned int aligned_addr =0;
    int i = 0;
    aligned_addr = addr_mcb;
     if ((addr_mcb & 0x3)!= 0)
     {
         printf("Address not aligned!!! Using auto align!!!\n");
         aligned_addr = (addr_mcb & 0xFFFFFFFC);
     }
    if((size%4)== 0){
        for(i=0;i<size/4;i++)
    {
        *((volatile unsigned int *)addr+i) = mcb_read_reg((aligned_addr+i*4));   
    }
    }
    else {
                for(i=0;i<size/4-1;i++)
    {
        *((volatile unsigned int *)addr+i) = mcb_read_reg((aligned_addr+i*4));   
    }
    switch(size%4)
{
    case 1:
        *((volatile unsigned char *)addr+i) = (mcb_read_reg((aligned_addr+i*4))& 0xFF);
        break;
    case 2:
        *((volatile unsigned short *)addr+i*2) = (mcb_read_reg((aligned_addr+i*4))& 0xFFFF);
        break;
    case 3:
        *((volatile unsigned short *)addr+i*2) = (mcb_read_reg((aligned_addr+i*4))& 0xFFFF00);
        *((volatile unsigned char *)addr+i*4) = (mcb_read_reg((aligned_addr+i*4))& 0xFF);
        break;
}            
    }
    }




