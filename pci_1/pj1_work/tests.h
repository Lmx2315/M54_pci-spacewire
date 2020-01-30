/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tests.h
 * Author: rkurmaev
 *
 * Created on July 2, 2019, 2:35 PM
 */

#ifndef TESTS_H
#define TESTS_H

#ifdef __cplusplus
extern "C" {
#endif


#include "mcb02.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "mcb_pci_driver.h"
#include "swic.h"
#include "memory_manager.h"
#include "common.h"

    
    
 int RAM_memtest_random(void);
 int swic_connection(unsigned int first_port, unsigned int second_port,unsigned int speed);
int manager_connection_test (void);
int manager_reset_test (void);
    
#ifdef __cplusplus
}
#endif

#endif /* TESTS_H */

