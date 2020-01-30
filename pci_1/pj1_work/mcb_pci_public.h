/*++

Copyright (c) RnD Center ELVEES, JSC 

Module Name:

	mcb_pci_public.h

Author:

	vmitrofanov@elvees.com

--*/

//
// Define an interface GUID for MCB-PCI devices.
// This GUID is used to register (IoRegisterDeviceInterface)
// an instance of an userspace interface
//
// {4d36e97d-e325-11ce-bfc1-08002be10000}

DEFINE_GUID(GUID_DEVINTERFACE_ELVEES_PCI, 0x4d36e97d, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x00, 0x00);


