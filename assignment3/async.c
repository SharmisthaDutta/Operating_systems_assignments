#include "scheduler.h"
#include<stdio.h>
#include<stdlib.h>
#include <aio.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>


ssize_t read_wrap(int fd, void * buf, size_t count){
// your code here
int numreq =1;
struct aiocb *async_buf_ptr =calloc(numreq, sizeof(struct aiocb));
int offset,returnvalue;

async_buf_ptr-> aio_fildes = fd;
async_buf_ptr->aio_nbytes = count;
async_buf_ptr->aio_buf=buf;
async_buf_ptr->aio_sigevent.sigev_notify = SIGEV_NONE;
offset = lseek(fd,offset, SEEK_SET);
/* copied
Upon successful completion, lseek() returns the resulting offset
       location as measured in bytes from the beginning of the file.  On
       error, the value (off_t) -1 is returned and errno is set to indicate
       the error. */
if(offset==-1)
perror("error");
if(offset!=-1)
async_buf_ptr->aio_offset = offset;
returnvalue = aio_read(async_buf_ptr);
if(returnvalue ==0){
  while(aio_error(async_buf_ptr)==EINPROGRESS){
  yield();
}
 if(aio_error(async_buf_ptr) ==ECANCELED)
  perror("cancelled");
 if(aio_error(async_buf_ptr)>0){
  perror("async i/o operation failed");
}
  //success
  if(aio_error(async_buf_ptr) ==0){
  return aio_return(async_buf_ptr);
 }
}
}

