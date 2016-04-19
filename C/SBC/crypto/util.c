/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * util.c
 *
 * This file contains implementation of utility functions
 */

 #include <string.h>
 #include "global.h"



 /*
  * reverse version of the memcpy function of the
  * string lib.
  *
  * dst     pointer to the output array with size >= len
  * src     pointer to the source to copy reverse with size >= leng
  * len     the amount of bytes that needs to be copied
  */
 void *reverse_memcpy(void *dst, const void *src, size_t len) {
     size_t i;
     char *d = dst;
     const char *s = src;

            for (i=0; i<len; i++) {
                    d[len-1-i] = s[i];
            }
     return dst;

 };

/*
 * Print all length chars from a given memory position
 * in a block orderd fasion.
 *
 * mem      pointer to start printing from
 * length   the amount of chars to print
 */
void print_hex_memory(const void *mem, int length) {
printf("\n");
  int i;
  unsigned char *p = (unsigned char *)mem;
  for (i=0;i<length;i++) {
    printf("\\x%02x", p[i]);
    if ((i+1)%BLOCK_LENGTH==0)
      printf("\n");
  }
};

/*
 * Computes XOR of in1 and in2 for char of length XORlen (char)
 */
void XOR(unsigned char *out, const unsigned char *in1, const unsigned char *in2, unsigned long long XORlen){
    unsigned long long i;

    for(i=0; i< XORlen; i++){
            out[i] = in1[i] ^ in2[i];
    }
};


//structure of the messages send through the channel
//tag can be:
//0x00: First message of STS_protocol (master)
//0x01: Second message of STS_protocol (slave)
//0x02: third message of STS_protocol (master)
//0x03: 4th message of STS_protocol (slave)
//0x04: 5th message of STS_protocol (master)
//0x05: optimal 6th message of STS_protocol (slave)
//0xFF: message containing enciphered data
void print_message(const message_ctx *message){
    if (message->tag == 0x00){
        printf("\n message1_master (generator c)");
        printf("\n length = %d bytes",message->length);
        printf("\n data:");
        print_hex_memory(message->data,message->length);
        printf("\n");
    }
    else if (message->tag == 0x01){
        printf("\n message1_slave (c^b mod p)");
        printf("\n length = %d bytes",message->length);
        printf("\n data:");
        print_hex_memory(message->data,message->length);
        printf("\n");
    }
    else if (message->tag == 0x02){
        printf("\n message2_master (c^a mod p)");
        printf("\n length = %d bytes",message->length);
        printf("\n data:");
        print_hex_memory(message->data,message->length);
        printf("\n");
    }
    else if (message->tag == 0x03){
        printf("\n message2_slave (E_k{S_b{hash(c^b, c^a)}})");
        printf("\n length = %d bytes",message->length);
        printf("\n associated data (sequence number) = %x ",message->seq_number);
        printf("\n data:");
        print_hex_memory(message->data,message->length);
        printf("\n");
    }
    else if (message->tag == 0x04){
        printf("\n message3_master (E_k{S_a{hash(c^a, c^b)}})");
        printf("\n length = %d bytes",message->length);
        printf("\n associated data (sequence number) = %d ",message->seq_number);
        printf("\n data:");
        print_hex_memory(message->data,message->length);
        printf("\n");
    }
    else if (message->tag == 0x05){
        printf("\n message3_slave (protocol unsuccessful, key not accepted)");
        printf("\n");
    }
    else if (message->tag == 0xFF){
        printf("\n ciphered data");
        printf("\n length = %d bytes",message->length);
        printf("\n associated data (sequence number) = %d ",message->seq_number);
        printf("\n data:");
        print_hex_memory(message->data,message->length);
        printf("\n");
    }
};


void convert_message_to_raw_message(unsigned char *out, const message_ctx *in){

    out[0] = in->tag;
    reverse_memcpy(out+1,&(in->seq_number), 4);
    out[5] = in->length;
    memcpy(out+6,in->data,in->length);
};

void convert_raw_message_to_message(message_ctx *out, const unsigned char *in){

    out->tag = in[0];
    out->seq_number = (unsigned int) in[4] + in[3]*256 + in[2]*256*256 + in[1]*256*256*256;
    out->length = in[5];
    memcpy(out->data,in+6,out->length);
};


