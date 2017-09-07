/**
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 **/
 
//����ļ�����LWIPЭ��ջ��һ����
#include "lwip/opt.h"
#include "lwip/def.h"
#include "fs.h"
#include "fsdata.h"
#include <string.h>

//����HTTPD_USE_SUSTUM_FSDATAΪ1ʱ,ʹ��fsdata_custom.c����fsdata.c
#ifndef HTTPD_USE_CUSTUM_FSDATA
#define HTTPD_USE_CUSTUM_FSDATA 0  //ʹ��fsdata.c
#endif

#if HTTPD_USE_CUSTUM_FSDATA
#include "fsdata_custom.c"
#else /* HTTPD_USE_CUSTUM_FSDATA */
#include "fsdata.c"
#endif /* HTTPD_USE_CUSTUM_FSDATA */


//��������ܴ򿪵��ļ���Ŀ
#ifndef LWIP_MAX_OPEN_FILES
#define LWIP_MAX_OPEN_FILES     10
#endif

//�����ļ�ϵͳ�ڴ����ṹ��
struct fs_table {
  struct fs_file file;  //
  u8_t inuse;   //0��ʾδʹ��,1��ʾʹ��
};

//����һ��fs_tabble����,���а���LWIP_MAX_OPEN_FILES��Ԫ��
struct fs_table fs_memory[LWIP_MAX_OPEN_FILES];

#if LWIP_HTTPD_CUSTOM_FILES
int fs_open_custom(struct fs_file *file, const char *name);
void fs_close_custom(struct fs_file *file);
#endif /* LWIP_HTTPD_CUSTOM_FILES */

//�ļ��ڴ����뺯�� ��LWIP_MAX_OPEN_FILES���ļ�ͬʱ�����ڴ�
static struct fs_file *
fs_malloc(void)
{
  int i;
  for(i = 0; i < LWIP_MAX_OPEN_FILES; i++) {
    if(fs_memory[i].inuse == 0) {
      fs_memory[i].inuse = 1;
      return(&fs_memory[i].file);
    }
  }
  return(NULL);
}

//�ͷ��ڴ�,һ���ͷŵ�LWIP_MAX_OPEN_FILES���������ڴ�
static void
fs_free(struct fs_file *file)
{
  int i;
  for(i = 0; i < LWIP_MAX_OPEN_FILES; i++) {
    if(&fs_memory[i].file == file) {
      fs_memory[i].inuse = 0;
      break;
    }
  }
  return;
}

//��һ���ļ�
//name:Ҫ�򿪵��ļ���
struct fs_file *
fs_open(const char *name)
{
  struct fs_file *file;
  const struct fsdata_file *f;

  file = fs_malloc();  //�����ڴ�
  if(file == NULL) {
    return NULL;
  }

#if LWIP_HTTPD_CUSTOM_FILES
  if(fs_open_custom(file, name)) {
    file->is_custom_file = 1;
    return file;
  }
  file->is_custom_file = 0;
#endif /* LWIP_HTTPD_CUSTOM_FILES */

  for(f = FS_ROOT; f != NULL; f = f->next) {
    if (!strcmp(name, (char *)f->name)) {  //���ݺ�����name��������
      file->data = (const char *)f->data;
      file->len = f->len;
      file->index = f->len;
      file->pextension = NULL;
      file->http_header_included = f->http_header_included;
#if HTTPD_PRECALCULATED_CHECKSUM  //���ʹ��HTTPD_PRECALCULATED�Ļ�
      file->chksum_count = f->chksum_count;
      file->chksum = f->chksum;
#endif /* HTTPD_PRECALCULATED_CHECKSUM */
#if LWIP_HTTPD_FILE_STATE   //���ʹ��LEIP_HTTPD_FILE_STATE
      file->state = fs_state_init(file, name);
#endif /* #if LWIP_HTTPD_FILE_STATE */
      return file;
    }
  }
  fs_free(file);  //�ͷ�file�ڴ�
  return NULL;
}

//�ر��ļ�
//����:Ҫ�رյ��ļ�
void
fs_close(struct fs_file *file)
{
#if LWIP_HTTPD_CUSTOM_FILES
  if (file->is_custom_file) {
    fs_close_custom(file);
  }
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#if LWIP_HTTPD_FILE_STATE
  fs_state_free(file, file->state);
#endif /* #if LWIP_HTTPD_FILE_STATE */
  fs_free(file);  //�ͷ�file���ڴ�
}

//��ȡ�ļ�
//����:file Ҫ��ȡ���ļ�
//����:buffer ��ȡ�����ŵĻ�����
//����:count  Ҫ��ȡ�ĸ���
int
fs_read(struct fs_file *file, char *buffer, int count)
{
  int read; //ʵ�ʶ�ȡ�����ݸ���

  if(file->index == file->len) {
    return -1;
  }

  read = file->len - file->index;
  if(read > count) {
    read = count;
  }

  MEMCPY(buffer, (file->data + file->index), read);
  file->index += read;

  return(read);  //���ض�ȡ�����ݸ���
}

//�ļ������
int fs_bytes_left(struct fs_file *file)
{
  return file->len - file->index;
}
