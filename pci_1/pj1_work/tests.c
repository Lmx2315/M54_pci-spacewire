/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "tests.h"
#include "string.h"
#include <Windows.h>
#include <MATH.h>

#define u32 unsigned int
#define u16 unsigned short

// длина массива в 32-разрядных словах
#define ARRAY_LEN 8
unsigned int OutputArray [ARRAY_LEN] __attribute__ ((aligned (8))) = {0,};
unsigned int InputArray1 [ARRAY_LEN] __attribute__ ((aligned (8))) = {0,};
unsigned int InputArray2 [ARRAY_LEN] __attribute__ ((aligned (8))) = {0,};

extern char TEST_var;

int RAM_memtest_random(void)
{
struct timeval start, end;
long elapsed_ms, seconds, useconds;
int rand_val = 0;
int flags = 1;
int i=0;

printf("RAM_MEMTEST STARTED\n");
gettimeofday(&start, NULL);


for (i=MCB02_RAM_BASE;i<MCB02_RAM_BASE+ MCB02_RAM_LEN;i=i+4)
{
    rand_val = rand();
    mcb_write(i,rand_val);
    if(mcb_read_reg(i)!= rand_val)
    {
        flags =0;
        printf("ERORR RAM_MEMTEST STARTED\n");
        return 0;
    }
    //printf("i= %x \n",i);
}
    gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	elapsed_ms = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("RANDOM memtest finished, %ld ms elapsed, speed rate %.02f kbytes/sec\n",
           elapsed_ms, (float)i / elapsed_ms);
    return 1;
}

int swic_connection(unsigned int first_port, unsigned int second_port,unsigned int speed)
{
    swic_init(first_port);
    swic_init(second_port);
    usleep(1);
    if (swic_is_connected(first_port)&& swic_is_connected(second_port))
      printf("Base speed connection OK \n");
    else{
       printf("Base speed connection FAILED \n");
       return 0;
        }

    if (swic_set_tx_speed(first_port,speed) && swic_set_tx_speed(second_port,speed)){
        printf("After setup speed connection OK \n");
        swic_get_rx_speed(first_port);
        swic_get_rx_speed(second_port);
        return 1;
    }
    else{
        printf("After setup speed connection FAILED \n");
        return 0;
        }
}

int manager_connection_test (void)
{
    unsigned int send = 0x12345678;
    unsigned int send2 =0x87654321;
    unsigned int place = 0;
    extern manager_main manager;
    int i = 0;
    init_manager();

    if (manager_init_cable_loopback (1,2)!= 1)
    {
        printf("Connecting fail \n");
        return 1;
    }
    if (aplay_all_tx_speeds()!=1)
    {
        printf("Speed set fail \n");
        return 1;
    }

   // print_port_status(1,1);
   // print_port_status(2,1);


    for (i=0; i< (MAX_RX_DESCRIPTORS_IN_MEMORY /2);i++)
    {
    place = get_plase (4,1, &send);
    if (place <0 )
    {
        printf("Get plase fail \n");
        return 1;
    }
    if (swic_send_packet (1 , place)!=1)
    {
        printf("Send fail \n");
        return 1;
    }

    place = get_plase (4,1, &send2);
     if (place <0 )
    {
        printf("Get plase fail \n");
        return 1;
    }

    if (swic_send_packet (1 , place)!=1)
    {
        printf("Send fail \n");
        return 1;
    }
    }
    return 0;
     //dump_from_mcb(manager.port[2].rx_mcb_addr,256);
}


int manager_reset_test (void)
{
    unsigned int send = 0x12345678;
    unsigned int send2 =0x87654321;
    unsigned int place = 0;
    unsigned int place2 = 0;
    unsigned int num = 0;
    extern manager_main manager;
    int i = 0;
    init_manager();

    while(manager_init_cable_loopback (1,2)!= 1);

    while(aplay_all_tx_speeds()!=1);

    while(1)
    {
        printf("TEST NUM %u \n",num);
    for (i=0; i< (MAX_RX_DESCRIPTORS_IN_MEMORY /2);i++)
    {
    place = get_plase (2048,1, &send);
    if (place <0 )
    {
        printf("Get plase fail \n");
        return 1;
    }
    while (swic_send_packet (1 , place)!=1);

    place2 = get_plase (2048,1, &send2);
     if (place2 <0 )
    {
        printf("Get plase fail \n");
        return 1;
    }

    while(swic_send_packet (2 , place2)!=1);
    }
    num++;
    }
    return 0;
}
void array_init (u32 *a,u32 n,u16 test)
{
	unsigned int i=0;
	unsigned int z=0;

	for (i=0;i<n;i++)
	{
		z=((test&0xffff)<<16)|i;
		a[i]=z;
	}
}

void clear (u32 adr,u32 n)
{
	u32 i=0;
	for (i=0;i<n;i++) mcb_write(adr+i*4,0x00000000);
}

int sw_send (u32 n,u32 port,u32 *M)
{
u32 error=0;
	error = get_plase (n*4,port, M);//заполняем стурктуру дескрипторов для отправки
	if (error <0 )	{ printf("Get plase fail \n");}

	error = swic_send_packet (port,0); //порт 1, дескриптор 0 - отсылаем
	if (error!=1) 	{ printf("Send fail \n");}

	return 1;
}

char* GetClipBoard(void)
{
    HWND hwnd = GetDesktopWindow();
    char* fromClipboard;//в эту переменную сохраним текст из буфера обмена
    if (OpenClipboard(hwnd))//открываем буфер обмена
    {
        if(IsClipboardFormatAvailable(CF_TEXT))
                {
            HANDLE hData = GetClipboardData(CF_TEXT);//извлекаем текст из буфера обмена
                        fromClipboard = (char*)GlobalLock(hData);//блокируем память и получаем содержимое
            GlobalUnlock(hData);//разблокируем память
            CloseClipboard();//закрываем буфер обмена
                        //вывод на консоль
            //AddClipboardFormatListener(hwnd);
        }
        else
        {
            HANDLE hData = GetClipboardData(CF_HDROP);
                    if(hData>0)
            {
                                int i, nBufSize=2;
                                for(i=0;nBufSize>1;i++)
                {
                                    nBufSize = DragQueryFile(hData, i, NULL, 0)+1;
                                    if(nBufSize>1)
                    {
                                                char *fName = (char*)malloc(nBufSize*sizeof(char));
                                                DragQueryFile(hData, i, fName, nBufSize );
                                                if(fName!=NULL)
                        {
                                                    fromClipboard = (char*)malloc(strlen(fName)+1);
                                                    CharToOem(fName, fromClipboard);
                                                    //вывод на консоль - printf("%s\n",buf);
                                                    free(fromClipboard);
                                                }
                                    }
                                }
                    }
                    CloseClipboard();//закрываем буфер обмена
        }
        //SetClipboardViewer(hwnd);
    }
        return fromClipboard;
}

int CopyBoard(const char *str)
{
    HGLOBAL hglb;
    char *s;
    int ll = strlen(str) + 1;

    if (!(hglb = GlobalAlloc(GHND, ll))){
        return 1;
    }
    if (!(s = (char *)GlobalLock(hglb))){
        return 1;
    }
    strcpy(s, str);
    GlobalUnlock(hglb);
    if (!OpenClipboard(NULL) || !EmptyClipboard()) {
        GlobalFree(hglb);
    return 1;
    }
    SetClipboardData(CF_TEXT, hglb);
    CloseClipboard();
    return 0;
}


#define Bufer_size 64000
char strng[64];
char msg[Bufer_size];
extern char buff[64000];//буфер UDP смотри в main.c
u32 text_lengh=0;

void Transf (char * s)
{
	u32 l=0;
	u32 i=0;
	l=strlen(s);
	if ((text_lengh+l)>Bufer_size-5) text_lengh=0;
	for (i=text_lengh;i<(text_lengh+l);i++) msg[i]=s[i-text_lengh];
	text_lengh=text_lengh+l;
}

void d_out (char s[],int a)
{
	Transf(s);
	sprintf(strng,"%d",a);
	Transf(strng);
}

void dn_out (char s[],int a)
{
	Transf(s);
	sprintf(strng,"%d\n\r",a);
	Transf(strng);
}

void Transf_Data (char * s,int leng)
{
	u32 l=0;
	u32 i=0;
	l=leng;
	if ((text_lengh+l)>Bufer_size-5) text_lengh=0;
	for (i=text_lengh;i<(text_lengh+l);i++) msg[i]=s[i-text_lengh];
	text_lengh=text_lengh+l;
}

void data_out (int a)
{
    strng[0]=(a>>24)&0xff;
    strng[1]=(a>>16)&0xff;
    strng[2]=(a>> 8)&0xff;
    strng[3]=(a>> 0)&0xff;
	Transf_Data(strng,4);
}

FILE *fp;
char name[] = "my.txt";
//char BUFFER_FILE [32000];
#define SIZE_DATA 8192 //в словах
char * Resive_text;

void init_LANE (u32 data)
{
//	int error=0;
    u32 i =0;
	u32 z =0;
	u32 z1=0;
	u32 z2=0;

	u32 dma1_des =0;
	u32 dma1_data=0;

	u32 dma2_des =0;
	u32 dma2_data=0;

	u32 N_pack1=0;
	u32 N_pack2=0;

	u32 N_v1=0;
	u32 N_v2=0;

	u32 flag=0;
	extern manager_main manager; //тут хранятся адреса дескрипторов RX и TX
	u32 addr = 0;
	u32 col=SIZE_DATA;
	u32 status1 = 0;
	u32 status2 = 0;

//	array_init (&OutputArray,col,data);//заполняем отправляемый массив тестовыми данными

	/*
	for (i=0;i<col;i++){	printf ("%x\n",OutputArray[i]);	}
	*/
    printf("\ninit_manager\n");
	init_manager();

    printf("\nmanager_swic_reset\n");
	manager_swic_reset_temp(1);
	manager_swic_reset_temp(2);
//  manager_swic_reset(1);
//	manager_swic_reset(2);
	printf("\ninit_LANE\n");
//---------------------------чистим память от мусора------------------------------------------------------
/*
	addr = manager.port[1].rx_mcb_addr;
	addr = addr + MAX_RX_DESCRIPTORS_IN_MEMORY*4;

    clear(addr,col);//обнуляем память в области приёма данных col - число 32 битных слов
	clear(manager.descriptor[1].data_addres_in_mcb_memory,col);//обнуляем память в области передачи данных

	addr = manager.port[2].rx_mcb_addr;
	addr = addr + MAX_RX_DESCRIPTORS_IN_MEMORY*4;

    clear(addr,col);//обнуляем память в области приёма данных
	clear(manager.descriptor[2].data_addres_in_mcb_memory,col);//обнуляем память в области передачи данных
*/
//---------------------------------------------------------------------------------------------------------

//-----инициализируем SPACEWIRE----------------------
	swic_init(1);
	swic_init(2);

	printf("\nwait LINK 1,2\n\n");

if (TEST_var==0)
{
	while ((((z1>>5)&0x7)!=5)&&(((z2>>5)&0x7)!=5)) //ждём установки линка по обоим портам
	{
		z1=mcb_read_reg(SWIC_STATUS(1)); // Чтением регистра статуса дожидаемся установления соединения
		z2=mcb_read_reg(SWIC_STATUS(2)); // Чтением регистра статуса дожидаемся установления соединения
	};
}
	printf ("STATUS[7:5]:%x\n",(z1>>5)&0x7);
	printf ("STATUS[7:5]:%x\n",(z2>>5)&0x7);

	swic_set_speed(1,50);//50 мбит/с
	swic_set_speed(2,50);

		while ((((z>>5)&0x7)!=5)&&(((z>>5)&0x7)!=5)) //ждём установки линка по обоим портам
	{
		z=mcb_read_reg(SWIC_STATUS(1)); // Чтением регистра статуса дожидаемся установления соединения
		z=mcb_read_reg(SWIC_STATUS(2)); // Чтением регистра статуса дожидаемся установления соединения
	};

	printf ("STATUS[7:5]:%x\n",(z>>5)&0x7);
	printf ("STATUS[7:5]:%x\n",(z>>5)&0x7);
//----------------------------------------------

//	setvbuf (fp,BUFFER_FILE,_IOFBF,32000);

if ((fp = fopen(name,"w"))==NULL)
	{
		printf("Не удалось открыть файл\r\n");
		getchar();
		return 0;
	}
	//удалось открыть файл

//----------------------------------------------
//CopyBoard("0");

while (1)
{
		//-----------RX stage------------
		init_rx_dma (1);
		init_rx_dma (2);
		//------------TX stage------------
		//	sw_send (col,1,OutputArray);//col - количество 32 битных слов
		//	sw_send (col,2,OutputArray);
//		sleep(1);//ждём
		sw_out (i++);
//		manager_swic_reset(1);
//		manager_swic_reset(2);


if (TEST_var==0)
{
	z=mcb_read_reg(SWIC_STATUS(1)); //регистр статус соединения
//	printf ("S[7:5]:%x\n",(z>>5)&0x7);
	if (((z>>5)&0x7)!=5) break;
	z=mcb_read_reg(SWIC_STATUS(2)); //регистр статус соединения
//	printf ("S[7:5]:%x\n",(z>>5)&0x7);
	if (((z>>5)&0x7)!=5) break;
}


}
//fclose(fp);

}

void dump_to_file (unsigned int port,unsigned int addr_mcb,unsigned int size)
{
    u32 i=0;
    u32 j=0;
    unsigned int reg = 0;

	fprintf(fp,"\n%u:\n",port);
//	fprintf(fp,"%u:\n",size);
    for(i=0;i<size;i++)//изменил начало индекса!!!!
    {
        j=(2*i+port-1);
        //j=i;
        mcb_read(addr_mcb+j*4, &reg);
        fprintf(fp,"%x",reg);
		if (i<(size-1)) fprintf(fp,",");
		else 		    fprintf(fp,";");
    }
}

void data_to_file (unsigned int port,unsigned int addr_mcb,unsigned int size)
{
    int i=0;
	u32 j=0;
    unsigned int reg = 0;

	fprintf(fp,"\n%u:\n",port);

    for(i=0;i<size;i++)//изменил начало индекса!!!!
    {
		j=(2*i+port-1);
        mcb_read(addr_mcb+j*4, &reg);
        fwrite(&reg,32,1,fp);
    }

}

void dump_to_data (unsigned int port,unsigned int addr_mcb,unsigned int size)
{
    u32 i=0;
    u32 j=0;
    unsigned int reg = 0;

    data_out(port);
  for(i=0;i<size;i++)//изменил начало индекса!!!!
    {
        j=(2*i+port-1);
       //j=i;
        mcb_read(addr_mcb+j*4, &reg);
  		data_out(reg);
    }
}

void test_to_data (unsigned int port,unsigned int size)
{
	 int i=0;

    unsigned int reg = 0;
	double A1=10000;
	double A2=3000;
	double A3=100;
	double F1=0;
	double F2=0;
	double F3=0;
	double freq1=0;
	double freq2=400;
	double freq3=700;
	double Fclk=6250;//KHz
	double pi=3.1415926535;

    data_out(port);
  for(i=0;i<size;i++)//изменил начало индекса!!!!
    {
		F1=((int)(A1*(cos(i*2*pi*freq1/Fclk)))<<16)+A1*(sin(i*2*pi*freq1/Fclk));
		F2=((int)(A2*(cos(i*2*pi*freq2/Fclk)))<<16)+A2*(sin(i*2*pi*freq2/Fclk));
		F3=((int)(A3*(cos(i*2*pi*freq3/Fclk)))<<16)+A3*(sin(i*2*pi*freq3/Fclk));
        reg=F1+F2+F3;
  		data_out(reg);
//      printf ("reg:%d\n",reg);
    }
}

void sw_out (unsigned int a)
{
u32 dma1_des =0;
u32 dma1_data=0;
u32 dma2_des =0;
u32 dma2_data=0;

u32 N_pack1=0;
u32 N_pack2=0;

u32 N_v1=0;
u32 N_v2=0;

u32 flag=0;
extern manager_main manager; //тут хранятся адреса дескрипторов RX и TX
u32 addr = 0;
u32 col=SIZE_DATA;
u32 status1 = 0;
u32 status2 = 0;
u32 N_col=0;

//while ((dma1_des&1)&(dma2_des&1)&(dma1_data&1)&(dma2_data&1))
if (TEST_var==0)
{
    while
    ((dma1_des&1)&(dma2_des&1)&(dma1_data&1)&(dma2_data&1))
 // ((dma1_des&1)&(dma2_des&1)&(dma1_data&1)&(dma2_data&1))
	{
		dma2_des =mcb_read_reg(SWIC_DMA_RX_DES_RUN (2));
		dma2_data=mcb_read_reg(SWIC_DMA_RX_DATA_RUN(2));
		dma1_des =mcb_read_reg(SWIC_DMA_RX_DES_RUN (1));
		dma1_data=mcb_read_reg(SWIC_DMA_RX_DATA_RUN(1));
	}
}
//--------------------------------------------------------------------------
N_col=MAX_RX_DESCRIPTORS_IN_MEMORY*col/2;
//выводим дампы отправленных и принятых данных
	addr = manager.port[2].rx_mcb_addr;
	addr = addr + MAX_RX_DESCRIPTORS_IN_MEMORY*4;

//    strnset(msg,0x00,Bufer_size);//очищаем транспортный буфер
    text_lengh=0;//сбрасываем счётчик данных в msg
//	dump_to_string(1,addr,N_col);
//	dump_to_string(1,addr,col);
//	dump_to_file  (1,addr,N_col);
//  data_to_file(1,addr,N_col);

if (TEST_var==0) { dump_to_data (2,addr,N_col);}
else {test_to_data (1,N_col);}
UDP_transmit ();

//    strnset(msg,0x00,Bufer_size);//очищаем транспортный буфер
    text_lengh=0;//сбрасываем счётчик данных в msg

//	addr = manager.port[2].rx_mcb_addr;
//	addr = addr + MAX_RX_DESCRIPTORS_IN_MEMORY*4;

if (TEST_var==0) { dump_to_data (1,addr,N_col);}
else {test_to_data (2,N_col);}

//	data_to_file(2,addr,N_col);
//	dump_to_string(2,addr,N_col);
//	dump_to_string(2,addr,col);
//	dump_to_file  (2,addr,N_col);
	UDP_transmit ();
}

void UDP_transmit ()
{
    int i=0;
    int k=0;
    int l=0;
    k=text_lengh;
    for (i=0;i<k;i++)
    {
        buff[i]=msg[i];
  //     if (i<50) printf ("buff[i]:%d\n",buff[i]);
    }
    UDP_work(k);

}
