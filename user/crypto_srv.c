#include "kernel/types.h"
#include "user/user.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "kernel/fcntl.h"

#include "kernel/crypto.h"

int main(void) {
  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  printf("crypto_srv: starting\n");

  // TODO: implement the cryptographic server here
  if (getpid() != 2)
  {
    exit(1);
  }
  for(;;){
    void * dst_va;
    uint64 dst_size; 
    if(take_shared_memory_request(&dst_va, &dst_size) != 0){
      printf("Fail take shared memory\n");
      exit(1);
    }
    printf("after take\n");
    struct crypto_op *crypto_req = (struct crypto_op*) dst_va;
    if(!((crypto_req->type == CRYPTO_OP_TYPE_ENCRYPT)||(crypto_req->type == CRYPTO_OP_TYPE_DECRYPT))){
      printf("Wrong type\n");
      asm volatile ("fence rw,rw" : : : "memory");
      crypto_req->state = CRYPTO_OP_STATE_ERROR;
      continue;
    }
    else if (crypto_req->state != CRYPTO_OP_STATE_INIT)
    {
      printf("Wrong state\n");
      asm volatile ("fence rw,rw" : : : "memory");
      crypto_req->state = CRYPTO_OP_STATE_ERROR;
      continue;
    }
    else if(crypto_req->key_size == 0 || crypto_req->data_size ==0 ){
      printf("Unreasonable key or data size\n");
      asm volatile ("fence rw,rw" : : : "memory");
      crypto_req->state = CRYPTO_OP_STATE_ERROR;
      continue;
    }
    uchar * key = crypto_req->payload;
    uchar * data = crypto_req->payload + crypto_req->key_size;
    for(int index=0; index < crypto_req->data_size; index++){
      data[index] ^= key[index % crypto_req->key_size]; 
    }
    asm volatile ("fence rw,rw" : : : "memory");
    crypto_req->state = CRYPTO_OP_STATE_DONE;
    if(remove_shared_memory_request(dst_va, dst_size) !=0 ){
      exit(1);
    }
  }
  exit(0);
}
