#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H
#include "sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip_comm.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ս�������� V3
//TCP Client ���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/3/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

#define TCP_CLIENT_RX_BUFSIZE	1500	//����tcp client���������ݳ���
#define TCP_CLIENT_TX_BUFSIZE	200		//����tcp client��������ݳ���
#define REMOTE_PORT				8087	//����Զ��������IP��ַ
#define LWIP_SEND_DATA			0X80    //���������ݷ���
#define	TCP_CLIENT_PORT			8087	//����tcp clientҪ���ӵ�Զ�˶˿�


//tcp����������״̬
enum tcp_client_states
{
	ES_TCPCLIENT_NONE = 0,		//û������
	ES_TCPCLIENT_CONNECTED,		//���ӵ��������� 
	ES_TCPCLIENT_CLOSING,		//�ر�����
}; 
//LWIP�ص�����ʹ�õĽṹ��
struct tcp_client_struct
{
	u8 state;               //��ǰ����״
	struct tcp_pcb *pcb;    //ָ��ǰ��pcb
	struct pbuf *p;         //ָ�����/�����pbuf
};  

void tcp_client_set_remoteip(void);
void tcp_client_test(void);//TCP Client���Ժ���
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err);
err_t tcp_client_usersent(struct tcp_pcb *tpcb);
void tcp_client_error(void *arg,err_t err);
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb);
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es);
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es );
#endif
