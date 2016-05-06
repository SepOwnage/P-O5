/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * MAIN.c
 *
 * This implements the general sequence of the this
 * encryption protocol.
 */

#include <time.h>
# include <string.h>
#include <stdio.h>
#include "global.h"
#include "util.h"
#include "STS_protocol.h"
#include "aes.h"
#include "CCM.h"

//variables used for timing purposes
clock_t b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9;
/*
//declare big arrays as globals the leave them of the stack
RSA_ctx RSA_ctx_master, RSA_ctx_slave;
ENC_ctx ENC_ctx_master, ENC_ctx_slave;
*/
message_ctx message1, message2;

STS_ctx STS_ctx_master, STS_ctx_slave;

/*
 * Implementation of the Station To Station protocol used
 * in this encryption protocol for the key agreement between
 * the to parties
 *
 * ENC_ctx_master       pointer to the encryption master context used for encryption
 * ENC_ctx_slave        pointer to the encryption slave context used for encryption
 * RSA_ctx_master       pointer to the RSA master context used in the STS protocol
 * RSA_ctx_slave        pointer to the RSA slave context used in the STS protocol
 */
void STSprotocol(ENC_ctx *ENC_ctx_master, ENC_ctx *ENC_ctx_slave,
    const RSA_ctx *RSA_ctx_master, const RSA_ctx *RSA_ctx_slave)
{
    unsigned char status;

    do{
            do {
                if (PRINT_errors){
                    printf("\n start of new protocol");
                    printf("\n");
                }
                status = 1;

                if(TIMING){b4 = clock();}
                STS_message1_master(&message1, &STS_ctx_master);
                if(TIMING){e4 = clock();}
                if (PRINT_messages){print_message(&message1);}

                if(TIMING){b5 = clock();}
                STS_message1_slave(&message2, &STS_ctx_slave, RSA_ctx_slave, &message1);
                if(TIMING){e5 = clock();}
                if (PRINT_messages){print_message(&message2);}

                if(TIMING){b6 = clock();}
                STS_message2_master(&message1, ENC_ctx_master, &STS_ctx_master, RSA_ctx_master, &message2);
                if(TIMING){e6 = clock();}
                if (PRINT_messages){print_message(&message1);}

                if(TIMING){b7 = clock();}
                STS_message2_slave(&message2, ENC_ctx_slave, &STS_ctx_slave, RSA_ctx_slave, &message1);
                if(TIMING){e7 = clock();}
                if (PRINT_messages){print_message(&message2);}

                if(TIMING){b8 = clock();}
                STS_message3_master(&message1, &status, ENC_ctx_master, &STS_ctx_master, RSA_ctx_master, &message2);
                if(TIMING){e8 = clock();}
                if (PRINT_messages){print_message(&message1);}
            }while (status ==0);

            if(TIMING){b9 = clock();}
            STS_message3_slave(&message2, ENC_ctx_slave, &STS_ctx_slave, RSA_ctx_slave, &message1);
            if(TIMING){e9 = clock();}
            if (message2.tag == 0x05)
                {
                    status = 0;
                    if (PRINT_messages){print_message(&message2);}
                };
    }while (status ==0);
    if (PRINT_errors){
        printf("\n protocol successful");
        printf("\n");
    }

}

/*
 * Creating a chiphered message, with wright sequence number
 *
 * ENC_ctx_sender       pointer to the encryption sender context used for encryption
 * input                pointer to the input array of size input_size
 * ciphermessage        pointer to the output message context to hold the ciphered data
 */

void sendData(ENC_ctx *ENC_ctx_sender, const unsigned char *input, const unsigned int input_size, message_ctx *ciphermessage)
{   if (PRINT_inputs_outputs){
        printf("\n-->Encrypt: %s\n", input);
        print_hex_memory(input, input_size);
        printf("\n");
    }

    CCM_encrypt(ciphermessage, ENC_ctx_sender, input, input_size);
    ciphermessage->tag=0xFF;

    if (PRINT_messages){
        print_message(ciphermessage);
        printf("\n");
    }
}

/*
 * Decripting an input message, with checking the sequence number
 *
 * ENC_ctx_receiver     pointer to the encryption receiver context used for decryption
 * ciphermessage        pointer to the input message context holding the ciphered data
 * out                  pointer to the output array of the decrypted message
 * output_size          pointer to the value of the size of the output message
 */

void readData(ENC_ctx *ENC_ctx_receiver,const message_ctx *ciphermessage, unsigned char *out, unsigned int *output_size)
{
	CCM_decrypt(out, output_size, ENC_ctx_receiver,ciphermessage);

    if (PRINT_inputs_outputs && *output_size != 0){
        printf("\n-->Decrypt: %s\n", out);
        print_hex_memory(out, *output_size);
        printf("\n");
    }
}


/*
int cryptoMain()
{   unsigned char message[] = "1->Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
    unsigned char decrypt[sizeof(message)];
    unsigned int decrypt_size;
    unsigned char message2[] = "2->Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
    unsigned char decrypt2[sizeof(message2)];

    char doSTS = 1;

    if(TIMING){b0 = clock();}

    if (doSTS){
		//Create RSA ctx master & slave
		calculate_parameters_RSA(&RSA_ctx_master);
		calculate_parameters_RSA(&RSA_ctx_slave);

		//Create ENC ctx master & slave
		ENC_ctx_master.counter = 0;
		ENC_ctx_slave.counter = 0;

		//Start protocol
		if(TIMING){b1 = clock();}
		STSprotocol(&ENC_ctx_master, &ENC_ctx_slave, &RSA_ctx_master, &RSA_ctx_slave);
		if(TIMING){e1 = clock();}
    } else {

		unsigned char KEY[AES_KEY_SIZE] = "\x83\xdf\x55\x05\x00\x00\x00\x00\x83\xdf\x55\x05\x00\x00\x00\x00";
		unsigned char STARTNONCE[NONCE_LENGTH] = "\x83\xdf\x55\x05\x00\x00\x00\x00";

		memcpy(ENC_ctx_slave.nonce, STARTNONCE,NONCE_LENGTH);
		aes_set_encrypt_key(&(ENC_ctx_slave.key),KEY, AES_KEY_SIZE*8);

		memcpy(ENC_ctx_master.nonce, STARTNONCE,NONCE_LENGTH);
		aes_set_encrypt_key(&(ENC_ctx_master.key),KEY , AES_KEY_SIZE*8);
    }

    //Send Message1
    if(TIMING){b2 = clock();}
    sendData(&ENC_ctx_master, message, sizeof(message), &message1);
    if(TIMING){e2 = clock();}

    //Receive Message1
    if(TIMING){b3 = clock();}

    readData(&ENC_ctx_slave, &message1, decrypt, &decrypt_size);
    if(TIMING){e3 = clock();}

    //Send Message2
    sendData(&ENC_ctx_master, message2, sizeof(message2), &message1);

    //Receive Message2
    readData(&ENC_ctx_slave, &message1, decrypt2, &decrypt_size);

    //test resending message
    readData(&ENC_ctx_slave, &message1, decrypt2, &decrypt_size);

    if(TIMING){e0 = clock();}

    if(TIMING & doSTS){
        printf("Time 0 (Full main):             %d clocks   %fs\n", (int) (e0-b0), ((double) (e0-b0))/CLOCKS_PER_SEC);
        printf("Time 1 (Full STS):              %d clocks   %fs\n", (int) (e1-b1), ((double) (e1-b1))/CLOCKS_PER_SEC);

        printf("Time 2 (Master encrypt):        %d clocks   %fs\n", (int) (e2-b2), ((double) (e2-b2))/CLOCKS_PER_SEC);
        printf("Time 3 (Slave decrypt):         %d clocks   %fs\n", (int) (e3-b3), ((double) (e3-b3))/CLOCKS_PER_SEC);

        printf("Time 4 (STS_message1_master):   %d clocks   %fs\n", (int) (e4-b4), ((double) (e4-b4))/CLOCKS_PER_SEC);
        printf("Time 5 (STS_message1_slave):    %d clocks   %fs\n", (int) (e5-b5), ((double) (e5-b5))/CLOCKS_PER_SEC);
        printf("Time 6 (STS_message2_master):   %d clocks   %fs\n", (int) (e6-b6), ((double) (e6-b6))/CLOCKS_PER_SEC);
        printf("Time 7 (STS_message2_slave):    %d clocks   %fs\n", (int) (e7-b7), ((double) (e7-b7))/CLOCKS_PER_SEC);
        printf("Time 8 (STS_message3_master):   %d clocks   %fs\n", (int) (e8-b8), ((double) (e8-b8))/CLOCKS_PER_SEC);
        printf("Time 9 (STS_message3_slave):    %d clocks   %fs\n", (int) (e9-b9), ((double) (e9-b9))/CLOCKS_PER_SEC);
    }

    return 0;
}*/
