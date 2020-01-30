/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   interrupts.h
 * Author: rkurmaev
 *
 * Created on July 17, 2019, 4:42 PM
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "mcb_pci_driver.h"
#include "memory_manager.h"
#include "swic.h"

#define INT_NUM 32

 typedef struct { 
        unsigned int cause[INT_NUM];
        unsigned int cause_fill;
    } interrupts_cause ;    
    
WORD WINAPI interrupt_thread(void);
int get_interrupt_cause(void);

 void clear_cause(void);

#ifdef __cplusplus
}
#endif

#endif /* INTERRUPTS_H */

