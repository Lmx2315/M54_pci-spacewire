#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "mcb_pci_driver.h"
#include "memory_manager.h"
#include "swic.h"
#include "interrupts.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define PORT 666    // порт сервера

unsigned int num = 0;
unsigned int i = 0;
unsigned int erorrs =0;

char TEST_var=0;

HANDLE thread;
DWORD thrId;
DWORD thrId2;
BOOL WINAPI consoleHandler(DWORD signal) {

    if (signal == CTRL_C_EVENT)
        printf("Ctrl-C handled\n"); // do cleanup
//        swic_reset(0); //TODO SWIC_RESET_ALL
//        swic_reset(3);
        mcb_deinit();
        exit(0);


    return TRUE;
}

WORD WINAPI saboteur_thread(void)
{
    unsigned int reg=0;
    unsigned int port = 0;

//    sleep(100);
    while(1)
    {
    port = rand()% 1 +1;
    usleep(rand()% 10);

    reg = mcb_read_reg(SWIC_MODE_CR(port)) &0xFFFF0000;
    mcb_write(SWIC_MODE_CR(port), (reg| 0x01)); //DISCONECT 1 or 2
 //    sleep(5);
    }

}


struct timeval start_0_3, end_0_3, start_3_0, end_3_0;
long elapsed_ms, seconds, useconds;

//------------------UDP--------------------

	int udp_socket, n_bytes;
	char buffer[1024];
	struct sockaddr_in local_addr, client_addr,dest_adr;
	struct sockaddr_storage server_storage;
	socklen_t addr_size, client_addr_size;
    SOCKET my_sock;

char buff[64000];


int UDP_init()
{
    int i;
    printf("UDP DEMO echo-Server\n");

    // шаг 1 - подключение библиотеки
    if (WSAStartup(0x202,(WSADATA *) &buff[0]))
    {
      printf("WSAStartup error: %d\n",
             WSAGetLastError());
      return -1;
    }

    // шаг 2 - создание сокета

    my_sock=socket(AF_INET,SOCK_DGRAM,0);
    if (my_sock==INVALID_SOCKET)
    {
      printf("Socket() error: %d\n",WSAGetLastError());
      WSACleanup();
      return -1;
    }

    // шаг 3 - связывание сокета с локальным адресом
    local_addr.sin_family=AF_INET;
    local_addr.sin_addr.s_addr=INADDR_ANY;
    local_addr.sin_port=htons(PORT);

    if (bind(my_sock,(struct sockaddr *) &local_addr,
        sizeof(local_addr)))
    {
      printf("bind error: %d\n",WSAGetLastError());
      closesocket(my_sock);
      WSACleanup();
      return -1;
    }

    return 0;
  }

void UDP_work(int buf_size)
{
    int i;
//  шаг 4 обработка пакетов, присланных клиентами
//  int client_addr_size = sizeof(client_addr);
//  int bsize=recvfrom(my_sock,&buff[0],sizeof(buff)-1,0,(struct sockaddr *) &client_addr, &client_addr_size);

    dest_adr.sin_family     =AF_INET;
    dest_adr.sin_addr.s_addr=inet_addr("127.0.0.1");//адресс куда пересылаем данные
    dest_adr.sin_port       =htons(8888);//порт куда пересылаем данные

// if (bsize==SOCKET_ERROR) printf("recvfrom() error: %d\n",WSAGetLastError());

// Определяем IP-адрес клиента и прочие атрибуты
// HOSTENT *hst;
// hst=gethostbyaddr((char *)&client_addr.sin_addr,4,AF_INET);
// printf("+%s [%s:%d] new DATAGRAM!\n",(hst)?hst->h_name:"Unknown host",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

// добавление завершающего нуля
//     buff[bsize]=0;

// Вывод на экран
// printf("C=>S:%s\n",&buff[0]);

// посылка датаграммы клиенту
// sendto(my_sock,&buff[0],bsize,0,(struct sockaddr *)&dest_adr, sizeof(dest_adr));
   sendto(my_sock,&buff,buf_size,0,(struct sockaddr *)&dest_adr, sizeof(dest_adr));

}

int main(int argc, char * argv[])
{
    int i=0;
	mcb_deinit();
	SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        printf("\nERROR: Could not set control handler");
        return 1;
    }

        if (mcb_init())
        {
        printf("mcb02 :: [ERROR] initialization\n");
        while(1);
        return -1;
    } else {
        printf("mcb02 :: [SUCESS] initialization\n");
    }

// reset_interrupts();
// mcb_pmsc_dump();
// printf("VENDOR ID 0x %x \n",mcb_read_pmsc_reg(PMSC_DEVICE_VENDOR_ID ));
// CreateThread(NULL, 0, &interrupt_thread, 0, 0, &thrId); //Thread for interupts
// CreateThread(NULL, 0, &saboteur_thread, 0, 0, &thrId2); //Thread for disconnect

//	RAM_memtest_random();

    printf ("\r\n");
    printf ("argc: %d \r\n",argc);
	printf (argv[1]);
	printf ("\r\n");

	if (argc>1) TEST_var=1; else TEST_var=0;

    UDP_init();
    //while(1) UDP_work();
    //manager_connection_test();
	//manager_reset_test();
	//mcb_pmsc_dump();

	printf("TEST COMPLITE ERORRS: %u \n",erorrs);
	while(1)
    {
       init_LANE (0xaaaa);
    }

  mcb_deinit();
//printf("Press press any key to exit\n");
//    getchar();

    return 0;
}

