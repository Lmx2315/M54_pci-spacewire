/*******************************************************************************
 *                         (c) RnD Center ELVEES, JSC                          *
 ******************************************************************************/
#define INITGUID
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <setupapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <devguid.h>
#include <inttypes.h>
#include "mcb_pci_io.h"
#include "mcb_pci_public.h"
#include "mcb_pci_driver.h"
#if defined(MCB03)
#include "mcb03.h"
#elif defined(MCB02)
#include "mcb02.h"
#else
#error "Define target processor (MCB02/MCB03)"
#endif

#pragma comment (lib, "Setupapi.lib")

#define MAX_DEVPATH_LENGTH                            256
#define SWIC_MODECR_INT_NUM                           12

static int swic_modecr_mask[SWIC_MODECR_INT_NUM] = { SWIC_INT_LINK_MSK , SWIC_INT_ERR_MSK, SWIC_INT_TIME_MSK,    /* SWIC0 */
                                                     SWIC_INT_LINK_MSK , SWIC_INT_ERR_MSK, SWIC_INT_TIME_MSK,    /* SWIC1 */
                                                     SWIC_INT_LINK_MSK , SWIC_INT_ERR_MSK, SWIC_INT_TIME_MSK,    /* SWIC2 */
                                                     SWIC_INT_LINK_MSK , SWIC_INT_ERR_MSK, SWIC_INT_TIME_MSK };  /* SWIC3 */

static int swic_modecr_number[SWIC_MODECR_INT_NUM] = { 0, 0, 0,     /* SWIC0 */
                                                       1, 1, 1,     /* SWIC1 */
                                                       2, 2, 2,     /* SWIC2 */
                                                       3, 3, 3 };   /* SWIC3 */



 /* Prototypes ----------------------------------------------------------------*/
 /**
  * @brief get mcb device system path from OS
  */
static BOOL get_device_path(LPGUID InterfaceGuid, PWCHAR DevicePath, size_t BufLen);

/**
 * @brief enable SWIC_LINK, SWIC_ERR, SWIC_TIME interrupts
 */
static int enable_swic_int(Interrupt_t interrupt);

/**
* @brief disable SWIC_LINK, SWIC_ERR, SWIC_TIME interrupts
*/
static int disable_swic_int(Interrupt_t interrupt);

/**
 * @brief clear SWIC interrupt
 */
static void clear_swic_int(void);

/* Variables -----------------------------------------------------------------*/
static int is_driver_init = -1;
static int device_file_disc = -1;
static HANDLE hDevice = NULL;
static HANDLE hEvent = NULL;

/* Functions -----------------------------------------------------------------*/
int mcb_init(void)
{
	WCHAR devicePath[MAX_DEVPATH_LENGTH];
	BOOL returnValue = FALSE;
	LPDWORD ulReturnedLength;

	hDevice = INVALID_HANDLE_VALUE;

	if (!get_device_path((LPGUID)&GUID_DEVINTERFACE_ELVEES_PCI,
		devicePath,
		sizeof(devicePath) / sizeof(devicePath[0])))
	{
		return -1;
	}

	hDevice = CreateFile(devicePath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return -2;
	}

	/* Create notification event */
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL)
	{
		return -3;
	}

	/* Send event handler to the driver */
	DeviceIoControl(hDevice, (DWORD)IOCTL_ELVEES_REGISTER_EVENT, &hEvent, 4, NULL, 0, &ulReturnedLength, NULL);

	is_driver_init = 0;

	return 0;
}

int mcb_deinit(void)
{
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hDevice);
	}

	return 0;
}

int mcb_read(unsigned int addr, unsigned int *value)
{
	DWORD bytesReturned;
	uint32_t buffer[2];
	uint32_t size = sizeof(buffer);
	buffer[0] = addr;

	DeviceIoControl(hDevice, (DWORD)IOCTL_ELVEES_READ_BAR0_AREA, buffer, 4, buffer, size, &bytesReturned, NULL);

	*value = buffer[1];

	return 0;
}

unsigned int mcb_read_reg(unsigned int addr)
{
	DWORD bytesReturned;
	uint32_t buffer[2];
	uint32_t size = sizeof(buffer);
	buffer[0] = addr;

	DeviceIoControl(hDevice, (DWORD)IOCTL_ELVEES_READ_BAR0_AREA, buffer, 4, buffer, size, &bytesReturned, NULL);
	return buffer[1];
}

int mcb_write(unsigned int addr, unsigned int value)
{
	DWORD bytesReturned;

	uint32_t buffer[2];
	uint32_t size = sizeof(buffer);
	buffer[0] = addr;
	buffer[1] = value;

	DeviceIoControl(hDevice, (DWORD)IOCTL_ELVEES_WRITE_BAR0_AREA, buffer, size, 0, 0, &bytesReturned, NULL);

	return 0;
}

int mcb_read_pmsc(unsigned int addr, unsigned int *value)
{
	/* 0x002F0000 value is used to set AD[23:16] to 0x2F for PMSC access */
	return mcb_read(0x002F0000 | addr, value);
}

int mcb_read_pmsc_reg(unsigned int addr)
{
	/* 0x002F0000 value is used to set AD[23:16] to 0x2F for PMSC access */
    sleep(1);
	return mcb_read_reg(0x002F0000 | addr);
}

int mcb_write_pmsc(unsigned int addr, unsigned int value)
{
	/* 0x002F0000 value is used to set AD[23:16] to 0x2F for PMSC access */
	return mcb_write(0x002F0000 | addr, value);
}

void mcb_pmsc_dump(void)
{
	unsigned int reg_value = 0;

	mcb_read_pmsc(PMSC_DEVICE_VENDOR_ID, &reg_value);
              printf("device_vendor: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_STATUS_COMMAND, &reg_value);
	printf("status_command: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_CLASS_REVISION, &reg_value);
	printf("class_revision: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_LATENCY_TIMER, &reg_value);
	printf("latency_timer: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_BAR0, &reg_value);
	printf("bar0: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_INTERRUPT_LINE, &reg_value);
	printf("interrupt_line: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_SEM, &reg_value);
	printf("sem: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_MBR_PCI, &reg_value);
	printf("mbr_pci: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_CSR_PCI, &reg_value);
		printf("csr_pci: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_CSR_MASTER, &reg_value);
		printf("csr_master: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_IR_MASTER, &reg_value);
		printf("ir_master: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_AR_PCI, &reg_value);
		printf("ar_pci: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_QSTR0_PCI, &reg_value);
		printf("qstr0_pci: %08x\n", reg_value);

	mcb_read_pmsc(PMSC_MASKR0_PCI, &reg_value);
		printf("maskr0_pci: %08x\n", reg_value);

#if defined(MCB03)
	if (!mcb_read_pmsc(PMSC_QSTR1_PCI, &reg_value))
		printf("qstr1_pci: %08x\n", reg_value);

	if (!mcb_read_pmsc(PMSC_MASKR1_PCI, &reg_value))
		printf("maskr1_pci: %08x\n", reg_value);
#endif
}


int mcb_enable_interrupt(Interrupt_t interrupt)
{
	unsigned int maskr = 0;
        unsigned int reg = 0;

	if (is_driver_init < 0)
	{
		return -1;
	}

	/* Enable interrupts in SWIC IP module */
	enable_swic_int(interrupt);

	/* Enable interrupts in QSTRx */
#if defined(MCB03)
	if (interrupt < QSTR0_INT_HIGH_VALUE)
	{
#endif
		if (mcb_read_pmsc(PMSC_MASKR0_PCI, &maskr))
		{
			return -1;
		}

		/* Calculate maskr0 interrupt offset */
		maskr |= 1 << interrupt;
                
               mcb_read_pmsc(PMSC_STATUS_COMMAND, &reg);
               reg &= ~PMSC_STATUS_COMMAND_DIS_INT;
               mcb_write_pmsc(PMSC_MASKR0_PCI, maskr);
	
		return mcb_write_pmsc(PMSC_STATUS_COMMAND, reg);
                
                
#if defined(MCB03)
	}
#endif

#if defined(MCB03)
	if (mcb_read_pmsc(PMSC_MASKR1_PCI, &maskr))
	{
		return -1;
	}

	/* Calculate maskr1 interrupt offset */
	maskr |= 1 << (interrupt << QSTR0_INT_HIGH_VALUE);
	return mcb_write_pmsc(PMSC_MASKR1_PCI, maskr);
#endif

}

int mcb_disable_interrupt(Interrupt_t interrupt)
{
	unsigned int maskr = 0;

	if (is_driver_init < 0)
	{
		return -1;
	}

	/* Disable interrupts in SWIC IP module */
	disable_swic_int(interrupt);

	/* Disable interrupts in QSTRx */
#if defined(MCB03)
	if (interrupt < QSTR0_INT_HIGH_VALUE)
	{
#endif
		if (mcb_read_pmsc(PMSC_MASKR0_PCI, &maskr))
		{
			return -1;
		}

		/* Calculate maskr0 interrupt offset */
		maskr &= ~(1 << interrupt);
		return mcb_write_pmsc(PMSC_MASKR0_PCI, maskr);
#if defined(MCB03)
	}
#endif

#if defined(MCB03)
	if (mcb_read_pmsc(PMSC_MASKR1_PCI, &maskr))
	{
		return -1;
	}

	/* Calculate maskr1 interrupt offset */
	maskr &= ~(1 << (interrupt << QSTR0_INT_HIGH_VALUE));
	return mcb_write_pmsc(PMSC_MASKR1_PCI, maskr);
#endif

}

int mcb_wait_interrupt(unsigned long delay_ms)
{
	unsigned int reg = 0;

	if (hEvent == NULL)
	{
             printf("mcb03 :ERROR_hEvent FINDE\n");
		return -1;
	}

	if (WaitForSingleObject(hEvent, (DWORD)delay_ms) == WAIT_TIMEOUT)
	{
            printf("mcb03 :ERROR_hEvent WAIT\n");
		return -2;
	}

	return 0;
}

int mcb_clear_interrupts(void)
{
	unsigned int reg = 0;

	clear_swic_int();

	if (ResetEvent(hEvent) == FALSE)
	{
		return -3;
	}
        
	mcb_read_pmsc(PMSC_STATUS_COMMAND, &reg);
	reg &= ~PMSC_STATUS_COMMAND_DIS_INT;
	mcb_write_pmsc(PMSC_STATUS_COMMAND, reg);
        
	return 0;
}

void reset_interrupts(void)
{
 unsigned int i = 0;
 unsigned int reg = 0;

	for (i = 0; i < SWIC_NUMBER; ++i)
        {
             mcb_read_reg(SWIC_DMA_RX_DATA_CSR(i));
             mcb_read_reg(SWIC_DMA_TX_DATA_CSR(i));
             mcb_read_reg(SWIC_DMA_RX_DES_CSR(i));
             mcb_read_reg(SWIC_DMA_TX_DES_CSR(i));
             mcb_write(SWIC_STATUS(i), 0xFFFFFFFF);
             //reg = mcb_read_reg(SWIC_MODE_CR(i));
             //reg&= ~SWIC_INT_ALL;
             //mcb_write(SWIC_MODE_CR(i), reg);
         }
 
 mcb_read_pmsc(PMSC_STATUS_COMMAND, &reg);
 reg |= (1<<10);
mcb_write_pmsc(PMSC_STATUS_COMMAND, reg);

mcb_read_pmsc(PMSC_STATUS_COMMAND, &reg);
reg &= ~PMSC_STATUS_COMMAND_DIS_INT;
mcb_write_pmsc(PMSC_STATUS_COMMAND, reg);

mcb_write_pmsc(PMSC_MASKR0_PCI, 0x0);
 ResetEvent(hEvent);
    
}


void clear_interrupts(void)
{
 unsigned int i = 0;
 unsigned int reg = 0;

	for (i = 0; i < SWIC_NUMBER; ++i)
        {
             mcb_read_reg(SWIC_DMA_RX_DATA_CSR(i));
             mcb_read_reg(SWIC_DMA_TX_DATA_CSR(i));
             mcb_read_reg(SWIC_DMA_RX_DES_CSR(i));
             mcb_read_reg(SWIC_DMA_TX_DES_CSR(i));
             mcb_write(SWIC_STATUS(i), 0xFFFFFFFF);
         }
 
 ResetEvent(hEvent);
    
}

void reset_event(void)
{
    unsigned int reg = 0;
    ResetEvent(hEvent);
mcb_read_pmsc(PMSC_STATUS_COMMAND, &reg);
reg &= ~PMSC_STATUS_COMMAND_DIS_INT;  
 mcb_write_pmsc(PMSC_STATUS_COMMAND, reg);
}


void clear_swic_int(void)
{
	unsigned int reg = 0;
	unsigned int active_int = 0;
	unsigned int i = 0;

	for (i = 0; i < SWIC_NUMBER; ++i)
	{
		mcb_read(SWIC_MODE_CR(i), &reg);

		/* Save active interrupts mask */
		active_int = reg & SWIC_INT_ALL;

		/* Clear interrupts */
		reg &= ~SWIC_INT_ALL;
		mcb_write(SWIC_MODE_CR(i), reg);

		/* Clear all interrupts */
		mcb_write(SWIC_STATUS(i), 0xFFFFFFFF);

		/* Restore interrupts mask */
		mcb_write(SWIC_MODE_CR(i), active_int);
	}
}

int enable_swic_int(Interrupt_t interrupt)
{
	int num = swic_modecr_number[interrupt];
	int mask = swic_modecr_mask[interrupt];
	unsigned int reg = 0;

	/* Check interrupts corresponding to SWIC MODECR register */
	if (interrupt < SWIC_MODECR_INT_NUM)
	{
		if (mcb_read(SWIC_MODE_CR(num), &reg))
		{
			return -1;
		}

		reg |= mask;

		if (mcb_write(SWIC_MODE_CR(num), reg))
		{
			return -1;
		}
	}

	return 0;
}

int disable_swic_int(Interrupt_t interrupt)
{
	int num = swic_modecr_number[interrupt];
	int mask = swic_modecr_mask[interrupt];
	unsigned int reg = 0;

	/* Check interrupts corresponding to SWIC MODECR register */
	if (interrupt < SWIC_MODECR_INT_NUM)
	{
		if (mcb_read(SWIC_MODE_CR(num), &reg))
		{
			return -1;
		}

		reg &= ~mask;

		if (mcb_write(SWIC_MODE_CR(num), reg))
		{
			return -1;
		}
	}

	return 0;
}


BOOL get_device_path(LPGUID InterfaceGuid, PWCHAR DevicePath, size_t BufLen)
{
	HDEVINFO HardwareDeviceInfo;
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData = NULL;
	ULONG Length, RequiredLength = 0;
	BOOL bResult;
	HRESULT hr;
	DWORD Error;

	HardwareDeviceInfo = SetupDiGetClassDevs(
		InterfaceGuid,
		NULL,
		NULL,
		(DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

	if (HardwareDeviceInfo == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	bResult = SetupDiEnumDeviceInterfaces(HardwareDeviceInfo,
		0,
		InterfaceGuid,
		0,
		&DeviceInterfaceData);
	Error = GetLastError();

	if (bResult == FALSE) {
		SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
		return FALSE;
	}

	SetupDiGetDeviceInterfaceDetail(
		HardwareDeviceInfo,
		&DeviceInterfaceData,
		NULL,
		0,
		&RequiredLength,
		NULL
	);

	DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LMEM_FIXED, RequiredLength);

	if (DeviceInterfaceDetailData == NULL) {
		SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
		return FALSE;
	}

	DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	Length = RequiredLength;

	bResult = SetupDiGetDeviceInterfaceDetail(
		HardwareDeviceInfo,
		&DeviceInterfaceData,
		DeviceInterfaceDetailData,
		Length,
		&RequiredLength,
		NULL);

	if (bResult == FALSE) {
		SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
		LocalFree(DeviceInterfaceDetailData);
		return FALSE;
	}

	hr = StringCchCopy(DevicePath,
		BufLen,
		DeviceInterfaceDetailData->DevicePath);

	SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
	LocalFree(DeviceInterfaceDetailData);

	return (!FAILED(hr));
}
