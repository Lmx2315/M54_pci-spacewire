/*******************************************************************************
 *                         (c) RnD Center ELVEES, JSC                           *
 *******************************************************************************/
#define MCB02
#if !defined( MCB_PCI_DRIVER_H_ )
#define MCB_PCI_DRIVER_H_

#if defined(MCB03)
#define QSTR0_INT_HIGH_VALUE   32
#endif

/* Types ---------------------------------------------------------------------*/
/**
 * @brief mcb interrupts
 */
typedef enum
{
	int_swic0_link = 0,
	int_swic0_err = 1,
	int_swic0_time = 2,
	int_swic1_link = 3,
	int_swic1_err = 4,
	int_swic1_time = 5,
	int_swic2_link = 6,
	int_swic2_err = 7,
	int_swic2_time = 8,
	int_swic3_link = 9,
	int_swic3_err = 10,
	int_swic3_time = 11,
	int_swic0_rx_desc = 12,
	int_swic0_rx_data = 13,
	int_swic0_tx_desc = 14,
	int_swic0_tx_data = 15,
	int_swic1_rx_desc = 16,
	int_swic1_rx_data = 17,
	int_swic1_tx_desc = 18,
	int_swic1_tx_data = 19,
	int_swic2_rx_desc = 20,
	int_swic2_rx_data = 21,
	int_swic2_tx_desc = 22,
	int_swic2_tx_data = 23,
	int_swic3_rx_desc = 24,
	int_swic3_rx_data = 25,
	int_swic3_tx_desc = 26,
	int_swic3_tx_data = 27,
#if defined(MCB02)
	int_mba = 28,
	int_master_wmark = 29,
	int_master_error = 30,
	int_master_done = 31,
#elif defined(MCB03)
	/* MASKR1 register */
	int_mbr = 28,
	int_master = 29,
	int_emac_rx = 32,
	int_emac_tx = 33,
	int_dma_rx = 34,
	int_dma_tx = 35,
	int_hm_dpram0 = 36,
	int_hm_dpram1 = 37,
	int_hm_dpram2 = 38,
	int_hm_dpram3 = 39
#else
#error "Define target processor (MCB02/MCB03)"
#endif
}Interrupt_t;

typedef struct
{
    unsigned char *addr;
} pci_t;

/* Prototypes ----------------------------------------------------------------*/
/**
 * Initialize mcb device
 * @return 0 if success, -1 if failure
 */
int mcb_init(void);

/**
 * Deinitialize mcb
 * @return 0 if success, -1 if failure
 */
int mcb_deinit(void);

/**
 * Read value. Access to mcb memory allocated resources.
 * @param addr memory address
 * @param value 32-bit value
 * @return 0 if success, -1 if failure
 */
int mcb_read(unsigned int addr, unsigned int *value);

/**
 * Write value. Access to mcb memory allocated resources.
 * @param addr memory address
 * @param value 32-bit value
 * @return 0 if success, -1 if failure
 */
int mcb_write(unsigned int addr, unsigned int value);
/**
 * Read value. Access to mcb memory allocated resources.
 * @param addr memory address
 *
 * @return 32-bit value
 */
unsigned int mcb_read_reg(unsigned int addr);

/**
 * Read PMSC register
 * @param addr Register address
 * @param value 32-bit value
 * @return 0 if success, -1 if failure
 */
int mcb_read_pmsc(unsigned int addr, unsigned int *value);

/**
 * Write to PMSC register
 * @param addr Register address
 * @param value 32-bit value
 * @return 0 if success, -1 if failure
 */
int mcb_write_pmsc(unsigned int addr, unsigned int value);

/**
 * Enable mcb interrupt
 * @param an interrupt to enable
 * @return 1 - success, 0 - failure
 */
int mcb_enable_interrupt( Interrupt_t interrupt );

/**
 * Disable mcb interrupt
 * @param an interrupt to disable
 * @return 1 - success, 0 - failure
 */
int mcb_disable_interrupt( Interrupt_t interrupt );

/**
 * Wait for interrupt
 * @param delay_ms delay in milliseconds. To wait until interrupt occur use INFINITE
 * @return 0 if interrupt occur, < 0 - failure
 */
int mcb_wait_interrupt( unsigned long delay_ms );

/**
 * Clear all interrupts. User has to read QSTR register if it is necessary before call this function
 */
int mcb_clear_interrupts(void);

/**
 * Print all registers of PCI Master Slave Controller
 */
void mcb_pmsc_dump(void);
void reset_interrupts(void);
int mcb_read_pmsc_reg(unsigned int addr);
void clear_interrupts(void);
void reset_event(void);

#endif
