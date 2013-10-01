#include <stdlib.h>
#include <stdio.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>

int aes_encrypt(char * file) {

   struct termios oflags, nflags;
   char * password = calloc(64,sizeof(char));

   /*  disabling echo */
   tcgetattr(fileno(stdin), &oflags);
   nflags = oflags;
   nflags.c_lflag &= ~ECHO;
   nflags.c_lflag |= ECHONL;

   if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
      perror("tcsetattr");
      return 1;
   }
   
   char * password_test = calloc(64,sizeof(char));
   do {

      printf("Please enter a password shorter than "
          "64 characters twice. If the two passwords "
          "are different, you have to type them again.\n"
          "First time:");
      fgets(password, 64*sizeof(char), stdin);
   
      printf("Second time:");
      fgets(password_test, 64*sizeof(char) , stdin);
      

   } while (strcmp(password,password_test) != 0);
   
   free(password_test);
   
   /*  restore terminal */
   if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
      perror("tcsetattr");
      free(password);
      return 1;
   }
   
   AES_KEY aes_key;
   AES_set_encrypt_key((const unsigned char *)password,256,&aes_key);
   free(password);

   unsigned char * IV = malloc(128*sizeof(unsigned char));
   RAND_bytes(IV,128*sizeof(unsigned char));
   
   FILE * f = fopen(file,"r+");
   
   if (!f) {
      fprintf(stderr, "Could not file\n");
      return 1;
   }

   fseek(f,0,SEEK_END);
   size_t filesize = ftell(f);
   fseek(f,0,SEEK_SET);
   
   int num = 0;

   unsigned char * in = malloc (filesize * sizeof(unsigned char));
   fread(in,1,filesize * sizeof(unsigned char),f);
   unsigned char * out = malloc (filesize * sizeof(unsigned char));
   AES_cfb128_encrypt(in, out, filesize, &aes_key, IV, &num, AES_ENCRYPT);
   fseek(f,0,SEEK_SET);
   fwrite(IV,1,128*sizeof(unsigned char),f);
   fwrite(out,1,filesize * sizeof(unsigned char),f);
   free(IV);
   free(out);
   free(in);
   fclose(f);
   
   return 0;
}

int aes_decrypt(char * file) {
   
   struct termios oflags, nflags;
   char * password = calloc(64,sizeof(char));

   /*  disabling echo */
   tcgetattr(fileno(stdin), &oflags);
   nflags = oflags;
   nflags.c_lflag &= ~ECHO;
   nflags.c_lflag |= ECHONL;

   if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
      perror("tcsetattr");
      return 1;
   }
   
   char * password_test = calloc(64,sizeof(char));
   do {

      printf("Please enter a password shorter than "
          "64 characters twice. If the two passwords "
          "are different, you have to type them again.\n"
          "First time:");
      fgets(password, 64*sizeof(char), stdin);
   
      printf("Second time:");
      fgets(password_test, 64*sizeof(char) , stdin);
      

   } while (strcmp(password,password_test) != 0);
   
   free(password_test);
   
   /*  restore terminal */
   if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
      perror("tcsetattr");
      free(password);
      return 1;
   }
   
   AES_KEY aes_key;
   AES_set_encrypt_key((const unsigned char *)password,256,&aes_key);
   free(password);

   FILE * f = fopen(file,"r+");
   if (!f) {
      fprintf(stderr, "Could not file\n");
      return 1;
   }

   unsigned char * IV = malloc(128*sizeof(unsigned char));
   fread(IV,1,128*sizeof(unsigned char),f);

   fseek(f,0,SEEK_END);
   size_t filesize = ftell(f) - 128*sizeof(unsigned char);
   fseek(f,128*sizeof(unsigned char),SEEK_SET);
   
   int num = 0;

   unsigned char * in = malloc (filesize * sizeof(unsigned char));
   fread(in,1,filesize * sizeof(unsigned char),f);
   unsigned char * out = malloc (filesize * sizeof(unsigned char));
   AES_cfb128_encrypt(in, out, filesize, &aes_key, IV, &num, AES_DECRYPT);
   f=freopen(file,"w",f); 
   fwrite(out,1,filesize * sizeof(unsigned char),f);
   free(IV);
   free(out);
   free(in);
   fclose(f);
   

   return 0;
}
