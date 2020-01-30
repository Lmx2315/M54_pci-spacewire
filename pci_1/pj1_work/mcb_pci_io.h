/*++

Copyright (c) RnD Center ELVEES, JSC

Module Name:

	mcb_pci_io.h

Abstract:

	MCB02 IO control codes

Author:

	vmitrofanov@elvees.com

--*/

#ifndef __USER__H
#define __USER__H

#define FSCTL_ELVEES_BASE						0xa000
#define _ELVEES_CTL_CODE(_Function, _access)	CTL_CODE(FSCTL_ELVEES_BASE, _Function, METHOD_BUFFERED, _access)

//
// IO control codes
//
#define IOCTL_ELVEES_READ_BAR0_AREA				_ELVEES_CTL_CODE(0x201, FILE_READ_ACCESS)
#define IOCTL_ELVEES_WRITE_BAR0_AREA			_ELVEES_CTL_CODE(0x202, FILE_WRITE_ACCESS)
#define IOCTL_ELVEES_REGISTER_EVENT				_ELVEES_CTL_CODE(0x205, FILE_WRITE_ACCESS)

#endif // __USER__H