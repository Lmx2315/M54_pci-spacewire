#ifndef MCB03_H_
#define MCB03_H_

/* Global settings */
#define SWIC_NUMBER                    0x4

/* RAM map -------------------------------------------------------------------*/
#define MCB03_DPRAM0                   0x01000000
#define MCB03_DPRAM1                   0x01020000
#define MCB03_DPRAM2                   0x01040000
#define MCB03_DPRAM3                   0x01060000

#define MCB03_DPRAM0_LEN               0x00020000
#define MCB03_DPRAM1_LEN               0x00020000
#define MCB03_DPRAM2_LEN               0x00020000
#define MCB03_DPRAM3_LEN               0x00020000

#define MCB03_RAM_BASE                 MCB03_DPRAM0
#define MCB03_RAM_LEN                  MCB03_DPRAM0_LEN + \
                                       MCB03_DPRAM1_LEN + \
                                       MCB03_DPRAM2_LEN + \
                                       MCB03_DPRAM3_LEN

/* PMSC ----------------------------------------------------------------------*/
#define PMSC_DEVICE_VENDOR_ID          0x00000000
#define PMSC_STATUS_COMMAND            0x00000004
#define PMSC_CLASS_REVISION            0x00000008
#define PMSC_LATENCY_TIMER             0x0000000C
#define PMSC_BAR0                      0x00000010
#define PMSC_INTERRUPT_LINE            0x0000003C
#define PMSC_SEM                       0x00000044
#define PMSC_MBR_PCI                   0x00000048
#define PMSC_CSR_PCI                   0x0000004C
#define PMSC_CSR_MASTER                0x00000050
#define PMSC_IR_MASTER                 0x00000054
#define PMSC_AR_PCI                    0x00000058
#define PMSC_QSTR0_PCI                 0x0000005C
#define PMSC_MASKR0_PCI                0x00000060
#define PMSC_QSTR1_PCI                 0x0000007C
#define PMSC_MASKR1_PCI                0x00000080

#define PMSC_STATUS_COMMAND_DIS_INT    (1UL << 10)

/* SWIC ----------------------------------------------------------------------*/
#define SWIC0_DMA_BASE                 0x01500000
#define SWIC1_DMA_BASE                 0x01700000
#define SWIC2_DMA_BASE                 0x01900000
#define SWIC3_DMA_BASE                 0x01B00000
#define SWIC_DMA_DIFF                  (SWIC1_DMA_BASE -  SWIC0_DMA_BASE)

#define SWIC0_BASE                     0x01400000
#define SWIC1_BASE                     0x01600000
#define SWIC2_BASE                     0x01800000
#define SWIC3_BASE                     0x01A00000
#define SWIC_DIFF                      (SWIC1_BASE - SWIC0_BASE)

#define SWIC_MODE_CR(x)                (0xC + SWIC0_BASE + (x)*SWIC_DIFF)
#define SWIC_STATUS(x)                 (0x4 + SWIC0_BASE + (x)*SWIC_DIFF)
#define SWIC_TX_SPEED(x)               (0x10 + SWIC0_BASE + (x)*SWIC_DIFF)
#define SWIC_RX_SPEED(x)               (0x18 + SWIC0_BASE + (x)*SWIC_DIFF)

#define SWIC_DMA_RX_DES_CSR(x)         (0x0+ 0x0 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_RX_DES_CP(x)         (0x4+ 0x0 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_RX_DES_IR(x)         (0x8+ 0x0 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_RX_DES_RUN(x)         (0xC+ 0x0 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)

#define SWIC_DMA_RX_DATA_CSR(x)         (0x0+ 0x40 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_RX_DATA_CP(x)         (0x4+ 0x40 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_RX_DATA_IR(x)         (0x8+ 0x40 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_RX_DATA_RUN(x)         (0xC+ 0x40 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)

#define SWIC_DMA_TX_DES_CSR(x)         (0x0+ 0x80 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_TX_DES_CP(x)         (0x4+ 0x80 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_TX_DES_IR(x)         (0x8+ 0x80 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_TX_DES_RUN(x)         (0xC+ 0x80 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)

#define SWIC_DMA_TX_DATA_CSR(x)         (0x0+ 0xC0 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_TX_DATA_CP(x)         (0x4+ 0xC0 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_TX_DATA_IR(x)         (0x8+ 0xC0 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)
#define SWIC_DMA_TX_DATA_RUN(x)         (0xC+ 0xC0 + SWIC0_DMA_BASE + (x)*SWIC_DMA_DIFF)


#define SWIC_INT_LINK_MSK              (1UL << 18)
#define SWIC_INT_ERR_MSK               (1UL << 19)
#define SWIC_INT_TIME_MSK              (1UL << 20)
#define SWIC_INT_TCODE_MSK             (1UL << 22)
#define SWIC_INT_INT_MSK               (1UL << 23)
#define SWIC_INT_CC11_MSK              (1UL << 24)
#define SWIC_INT_CC01_MSK              (1UL << 25)
#define SWIC_INT_TOUT_MSK              (1UL << 26)
#define SWIC_INT_ALL                   (SWIC_INT_LINK_MSK  | \
                                        SWIC_INT_ERR_MSK   | \
                                        SWIC_INT_TIME_MSK  | \
                                        SWIC_INT_TCODE_MSK | \
                                        SWIC_INT_INT_MSK   | \
                                        SWIC_INT_CC11_MSK  | \
                                        SWIC_INT_CC01_MSK  | \
                                        SWIC_INT_TOUT_MSK)


#endif