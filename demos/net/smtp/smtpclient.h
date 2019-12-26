//////////////////////////////////////////////////////////////
 // smtpclient.h for send mail by navy 2004.3.22
 // for win32 or linux or unix
 // SMTP��׼���ſͻ��˿⣬���ڷ����ʼ�
//
 // complie in linux or unix
 // >g++ -o smtptest -O2 -Wall smtptest.cpp libsmtpclt.a
 //
 // complie in solaris
 // >g++ -o smtptest -O2 -Wall smtptest.cpp libsmtpclt.a -lsocket -lnsl -lrt
 //
 // complie in win32
 // >cl /nologo /ML /W3 /GX /O2 smtptest.cpp ws2_32.lib libsmtpclt.lib
 //
 // how to use? the example as follow:
 //
 // ---------------------------------------
 // #include <stdio.h>
 // #include <stdlib.h>
 // #include <string.h>
 // #include "smtpclient.h"
 //
 // main() {
 //    char smtphost[]     = "smtp.263.net";
 //    char smtpuser[]     = "sender@263.net";
 //    char smtppasswd[]   = "123456";     
 //    char smtpsender[]   = "sender@263.net";
 //
 //    char smtpto[]       = "to@263.net";
 //    char smtpcc[LINE_LEN];
 //    char smtpbody[]     = "Subject: testsubject\r\n"
 //                          "From: testfrom<abc@263.net>\r\n"
 //                          "To: testto<abc@263.net>\r\n"
 //                          "testest\r\n.body\nfdafafdsa";
 //   
 //    SMTP smtp;
 //
 //    //SmtpInit(&smtp, smtphost, smtpsender, smtpto);
 //    SmtpInitAuth(&smtp, smtphost, smtpsender, smtpto, smtpuser, smtppasswd);
 //   
 //    SmtpInitCc(&smtp, smtpcc, sizeof(smtpcc));
 //    SmtpAddCc(&smtp, "aaaa@263.net");
 //    SmtpAddCc(&smtp, "bbbb@263.net");
 //   
 //    SmtpSetBody(&smtp, smtpbody, strlen(smtpbody));
 //   
 //    SmtpSendMail(&smtp);
 // }
 // -----------------------------------------
 //
 //////////////////////////////////////////////////////////////


 #ifndef _SMTPCLIENT_H
 #define _SMTPCLIENT_H

#include <mxos.h>

 #ifdef   __cplusplus
 extern "C" {
 #endif

 /////////////////////////////////////////////////////////////////////
 //
 // ������һЩ����SOCKETͨѶ�Ļ����ĺ궨��Ͱ���ͷ��������WIN32��UNIX
 //
 /////////////////////////////////////////////////////////////////////



 #define EMAIL_ADDR_MAX_LEN       64

 enum {
     SMTP_SECURITY_NONE = 1,
     SMTP_SECURITY_SSL,
     SMTP_SECURITY_TLS,
     SMTP_SECURITY_STARTTLS,
 };

 enum {
     CMD_OK = 0,
     CMD_FAIL = 1,
     CMD_EMAIL_DISABLED = 2,
     CMD_EMAIL_CONNT_FAIL = 3,
     CMD_EMAIL_AUTH_FAIL = 4,
     CMD_EMAIL_MSG_FAIL = 5,
 };

struct addr_list {
    struct addr_list *next;
    char addr[EMAIL_ADDR_MAX_LEN+1];
};

 // ����SMTP���ſͻ��˽ṹ
struct SMTP_S {
    int  sockfd;
    char smtpserver[64];             //SMTP��������ַ
    uint8_t smtpmode;
    mtls_t ssl;
    uint16_t  smtpport;                  //SMTP������PORT
    char user[EMAIL_ADDR_MAX_LEN+1];  //������֤�û���
    char passwd[65];                //������֤�û�����
    char sender[EMAIL_ADDR_MAX_LEN+1];               //������EMAIL
    struct addr_list *to_list;                   //������EMAIL�б�
    struct addr_list *cc_list;   //�����ռ��˺ͳ�����EMAIL��ַ�б�
    int  sentsize;                  //�ѷ��͵����ݴ�С
    int  sentlines;                 //�ѷ��͵�����
    int  recvsize;                  //�ѽ��յ����ݴ�С
    int  recvlines;                 //�ѽ��յ�����
    int  errcode;                   //�������������?
    char errinfo[64];             //��������Ϣ
    char smtpcmd[32];               //���͵�SMTPЭ������
    char smtprcv[32];               //��ȡ��SMTPЭ����
};

typedef struct SMTP_S SMTP;         //SMTP�ṹ����


// ��ʼ��SMTP�ṹ�����������÷��ŷ�������ַ��������EMAIL��������EMAIL���������û���֤��������
// ���룺psmtp ���Žṹ��phost SMTP��������ַ��psender �����ˣ� pto �����ˣ�puser ��֤�û�����ppasswd ��֤����
// ���أ�SMTP�ṹ��ָ�룬NULL ʧ��

SMTP* SmtpInit(char *server, uint16_t port, const char *psender, 
      const char *puser, const char *ppasswd);

// ��ӳ������ʼ���ַ����?
// ���룺psmtp ���Žṹ��pemail �ʼ���ַ
// ���أ�0 �ɹ�����0 ʧ��
int SmtpAddCc(SMTP *psmtp, const char *pemail);

// ɾ���������ʼ���ַ����
// ���룺psmtp ���Žṹ��pemail �ʼ���ַ
// ���أ�0 �ɹ�����0 ʧ��
int SmtpRemoveCc(SMTP *psmtp, const char *pemail);

// ��ӽ������ʼ���ַ����?
// ���룺psmtp ���Žṹ��pemail �ʼ���ַ
// ���أ�0 �ɹ�����0 ʧ��
int SmtpAddTo(SMTP *psmtp, const char *pemail);
// ɾ���������ʼ���ַ����
// ���룺psmtp ���Žṹ��pemail �ʼ���ַ
// ���أ�0 �ɹ�����0 ʧ��
int SmtpRemoveTo(SMTP *psmtp, const char *pemail);

void SmtpSetSSLMode(SMTP *psmtp, uint8_t mode);

// ���ź���������������ӡ���֤�����źͶϿ����?������
// ���룺psmtp ���Žṹ
// ���أ�0 �ɹ�����0 ʧ��
// psmtp->errcode psmtp->errinfo���ش�������Ϣ
int SmtpSendMail(SMTP *psmtp, char *subject, char *body);

/* �ͷ�SMTP */
int SmtpDeinit(SMTP *psmtp);


void SmtpSetSmtpMode(SMTP *psmtp, uint8_t mode);


 #ifdef   __cplusplus
 }
 #endif
 #endif
