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

 }

 void *memset_short(void *dst, int c, size_t n)
{
    if (n) {
        short *d = dst;

        do {
            *d++ = c;
        } while (--n);
    }
    return dst;
 }

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
}

void print_hex_memory_short(const void *mem, int length) {
printf("\n");
  int i;
  unsigned short *p = (unsigned short *)mem;
  for (i=0;i<length;i++) {
    printf(" 0x%04x", p[i]);
    if ((i+1)%(BLOCK_LENGTH/2)==0)
      printf("\n");
  }
}

/*
 * Computes XOR of in1 and in2 for char of length XORlen (char)
 */
void XOR(void *out, const void *in1, const void *in2, const unsigned int XORlen){
    unsigned int i = 0;
    //unsigned long long j = 0;

    /*unsigned int * outint = out;
    const unsigned int * in1int = in1;
    const unsigned int * in2int = in2;*/

    unsigned char * restrict outchar = out;
	const unsigned char * restrict in1char = in1;
	const unsigned char * restrict in2char = in2;


	/*while(i+3<XORlen){
		outint[j] = in1int[j] ^ in2int[j];
		j += 1;
		i += 4;
	}*/

    while(i<XORlen){
		outchar[i] = in1char[i] ^ in2char[i];
		i++;
	}
}

//little endian style
void transform_char_to_short(unsigned short *out, const unsigned char *in1, int length){
    unsigned int i;

    for (i=0; i<(length-1)/2; i++){
        out[i] = (unsigned short) (in1[2*i]+(in1[2*i+1])*256);
    }
    if (2*i+1 == length-1){
            out[i] = in1[2*i]+(in1[2*i+1])*256;
    }
    else{
        out[i] = in1[2*i];
    }
}
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
}


void convert_message_to_raw_message(unsigned char *out, const message_ctx *in){

    out[0] = in->tag;
    reverse_memcpy(out+1,&(in->seq_number), 4);
    out[5] = in->length;
    memcpy(out+6,in->data,in->length);
}

void convert_raw_message_to_message(message_ctx *out, const unsigned char *in){

    out->tag = in[0];
    out->seq_number = (unsigned int) in[4] + in[3]*256 + in[2]*256*256 + in[1]*256*256*256;
    out->length = in[5];
    memcpy(out->data,in+6,out->length);
}
