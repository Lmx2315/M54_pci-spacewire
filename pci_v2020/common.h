/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   common.h
 * Author: rkurmaev
 *
 * Created on February 22, 2019, 3:53 PM
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcb_pci_driver.h"
#include "erlcommon.h"
#include <stdio.h>

void FillArray(unsigned int *array, unsigned int len, int first);
void FillArray_char(unsigned char *array, unsigned int len);
void FillArray_char_comm(unsigned char *array, unsigned int len,unsigned char port);
void FillHandShake(unsigned int array,unsigned char dev_number,unsigned char port_number);
int VerifyArray_Char(unsigned int *array_one,unsigned int *array_two, unsigned int len);
int VerifyArray_Char_Comm(unsigned int *array_one,unsigned int *array_two, unsigned int len);
enum ERL_ERROR copy_to_mcb(unsigned int addr_mcb, unsigned int addr, unsigned int size);
enum ERL_ERROR dump_from_mcb(unsigned int addr_mcb,unsigned int size);
enum ERL_ERROR copy_from_mcb(unsigned int addr_mcb, void *addr, unsigned int size);


#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */

