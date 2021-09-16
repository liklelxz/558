#include <openssl/aes.h>
#include <openssl/modes.h>
#include <openssl/rand.h> 
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



  /* For CMAC Calculation */
  unsigned char const_Rb[16] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87
  };
  unsigned char const_Zero[16] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  /* Basic Functions */

  void xor_128(unsigned char *a, unsigned char *b, unsigned char *out)
  {
      int i;
      for (i=0;i<16; i++)
      {
          out[i] = a[i] ^ b[i];
      }
  }

  void print_hex(char *str, unsigned char *buf, int len)
  {
      int i;

      for ( i=0; i<len;i++) {
          if((i % 16) == 0 && i != 0 ) printf("%s",str);
          printf("%02x", buf[i]);
          if ( (i % 4) == 3 ) printf(" ");
          if ( (i % 16) == 15 ) printf("\n");
      }
      if ( (i % 16) != 0 ) printf("\n");
  }


  void print128(unsigned char *bytes)
  {
      int         j;
      for (j=0; j<16;j++) {
          printf("%02x",bytes[j]);
          if ( (j%4) == 3 ) printf(" ");
      }
  }

  void print96(unsigned char *bytes)
  {
      int         j;
      for (j=0; j<12;j++) {
          printf("%02x",bytes[j]);
          if ( (j%4) == 3 ) printf(" ");
      }
  }

  

  void leftshift_onebit(unsigned char *input,unsigned char *output)
  {
      int         i;
      unsigned char overflow = 0;

      for ( i=15; i>=0; i-- ) {
          output[i] = input[i] << 1;
          output[i] |= overflow;
          overflow = (input[i] & 0x80)?1:0;
      }
      return;
  }

  void generate_subkey(unsigned char *key, unsigned char *K1, unsigned
                       char *K2)
  {
      unsigned char L[16];
      unsigned char Z[16];
      unsigned char tmp[16];
      int i;
	  
      for ( i=0; i<16; i++ ) Z[i] = 0;
	  AES_KEY k_enc;
	  AES_set_encrypt_key((const unsigned char*)key, 128, &k_enc);
      AES_encrypt((const unsigned char*)Z,L,(const AES_KEY*)&k_enc);

      if ( (L[0] & 0x80) == 0 ) { /* If MSB(L) = 0, then K1 = L << 1 */
          leftshift_onebit(L,K1);
      } else {    /* Else K1 = ( L << 1 ) (+) Rb */



          leftshift_onebit(L,tmp);
          xor_128(tmp,const_Rb,K1);
      }

      if ( (K1[0] & 0x80) == 0 ) {
          leftshift_onebit(K1,K2);
      } else {
          leftshift_onebit(K1,tmp);
          xor_128(tmp,const_Rb,K2);
      }
      return;
  }

  void padding ( unsigned char *lastb, unsigned char *pad, int length )
  {
      int j;
	  int error  = (length % 128)/8;
	  if((length %128) %8 == 0){
	  	 error -= 1;
	  }
	  int pad_l = (128-(length % 128)) % 8;
	  
	  
      /* original last block */
      for ( j=0; j<16; j++ ) {
          if(j<error){
          	pad[j] = lastb[j];
          }
          else if(j == error){
          	if(pad_l == 0){
          		pad[j]=lastb[j];
          	}
          	else{
          		//printf("enter here!\n");
          		//printf("%d\n",1<<5);
          		pad[j] =(lastb[j] | (1<<(pad_l-1))) ;
          		
          	}
          }
          else{
             if(pad_l == 0){
             	pad[j] = 0x80;
             	pad_l =1;
             }
             else{
             	pad[j] = 0x00;
             }
          }
        }
       for(int k=0;k<16;k++){
       		//printf("%x\n",pad[k]);
       }
  }

  void AES_CMAC ( unsigned char *key, unsigned char *input, int length,
                  unsigned char *mac )
  {
      unsigned char       X[16],Y[16], M_last[16], padded[16];
      unsigned char       K1[16], K2[16];
      AES_KEY k_enc;
	  AES_set_encrypt_key((const unsigned char*)key, 128, &k_enc);
      int n, i, flag;
      generate_subkey(key,K1,K2);

      n = (length+127) / 128;       /* n is number of rounds */
	  //printf("n = %d\n",n);
      if ( n == 0 ) {
          n = 1;
          flag = 0;
      } else {
          if ( (length%128) == 0 ) { /* last block is a complete block */
              flag = 1;
          } else { /* last block is not complete block */
              flag = 0;
          }



      }

      if ( flag ) { /* last block is complete block */
          xor_128(&input[16*(n-1)],K1,M_last);
      } else {
          padding(&input[16*(n-1)],padded,length);
          xor_128(padded,K2,M_last);
      }

      for ( i=0; i<16; i++ ) X[i] = 0;
      for ( i=0; i<n-1; i++ ) {
          xor_128(X,&input[16*i],Y); /* Y := Mi (+) X  */
          AES_encrypt((const unsigned char *)Y,X,(const AES_KEY*)&k_enc);      /* X := AES-128(KEY, Y); */
      }

      xor_128(X,M_last,Y);
      AES_encrypt((const unsigned char *)Y,X,(const AES_KEY*)&k_enc);

      for ( i=0; i<16; i++ ) {
          mac[i] = X[i];
      }
  }
  
  void deletespace(unsigned char n[],unsigned char d[],int len,int max){
	
	  int k=0,c=0;
	  while(k<len){
		  if(d[k] == ' '){
			  k++;
			  continue;
		  }
		  if(k<len){
			  if(!(c<max)){
				  break;
			  }
			  n[c] =d[k];
			  c++;
			  k++;
		  }
	  }
  }

  void splitinput(char buffer[255], unsigned char p[]){
	  char* ptr = strtok(buffer,"=");
	
	
	  while(ptr!=NULL){
		  strcpy(p,ptr);
		  ptr = strtok(NULL,"=");
	  }
  }
  void buildiv(unsigned char c[],unsigned char b[], unsigned char d[],unsigned char iv[]){
	  unsigned char co[128],be[128],di[128];
	  unsigned char co1[128],be1[128],di1[128];
	  deletespace(co,c,128,128);
	  deletespace(be,b,128,128);
	  deletespace(di,d,128,128);
	  //printf("%s\n",co);
	  //printf("%s\n",be);
	  //printf("%s\n",di);
	  int k = 3;
	  unsigned long l1 = strtol(co,NULL,16);
	  unsigned long l2 = strtol(be,NULL,16);
	  unsigned long l3 = strtol(di,NULL,16);

	  while(k>=0){
		  iv[k]= l1 & 0xff;
		  //printf("%x\n",iv[k]);
		  l1 = l1 >> 8;
		  k--;
	  }
	  iv[4] = ((l2 << 1) | l3) <<2;
	  for(int k =5;k<8;k++){
		 iv[k] = 0;
	  }
	  /*for (int i = 0; i<16;i++){
		  printf("%x\n",iv[i]);
	  }*/
	
  }

  int main(int argc, char* argv[])
  {
      unsigned char L[16], K1[16], K2[16], T[16], TT[12];
      /*unsigned char M[64] = {
          0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
          0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
          0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
          0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
          0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
          0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
          0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
          0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
      };
      unsigned char key[16] = {
          0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
          0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
      };*/
      
      
      char buffer[10][10000];
	  FILE *fp;
	  fp = fopen(argv[1],"r");
	  ssize_t read;
	  int j =0;
	  while(fgets(buffer[j],10000,fp)!= NULL){
		  j++;
	  }
	  fclose(fp);
	
	  unsigned char ukey[128];
	  unsigned char count[128];
	  unsigned char bearer[128];
	  unsigned char direction[128];
	  unsigned char length[10];
	  unsigned char message[10000]; 
	  unsigned char kb[32];
	
	  splitinput(buffer[3],ukey);
	  splitinput(buffer[0],count);
	  splitinput(buffer[1],bearer);
	  splitinput(buffer[2],direction);
	  splitinput(buffer[4],length);
	  splitinput(buffer[5],message);
      
      //deletespace(kb,ukey,128,32);
	  //deletespace(pt,plaintext,10000,l_pt)
	  
      unsigned char key[16];
	  const unsigned char* pos = ukey;
	  for(size_t count =0;count <sizeof(key)/sizeof(*key);count++ ){
		  if(*pos == ' '){
		  	  pos++;
		  }
		  sscanf(pos,"%2hhx",&key[count]);
		  pos += 2;
	  }
      
      unsigned char len_pt[10];
	  deletespace(len_pt,length,10,10);
	  unsigned long l_pt = strtoul(len_pt,NULL,10);
	  //printf("n=%lu\n",l_pt);
	  
      unsigned char mess[10000];
      deletespace(mess,message,10000,10000);
      int len = (int)strlen(mess) - 1;
      //printf("%s\n",mess);
      //printf("%d\n",len);
      int mlen = 8+len/2;
      //printf("%d\n",mlen);
      unsigned char M[mlen];
      buildiv(count,bearer,direction,M);
      
	  const unsigned char* pos1 = mess;
	  for(size_t count =8;count <sizeof(M)/sizeof(*M);count++ ){
		  if(*pos1 == ' '){
		  	  pos1++;
		  }
		  sscanf(pos1,"%2hhx",&M[count]);
		  pos1 += 2;
	  }
      
      //for(int mk2 = 0;mk2<mlen;mk2++ ) printf("%x\n",M[mk2]);
      
      //printf("--------------------------------------------------\n");
      //printf("K              "); print128(key); printf("\n");

      //printf("\nSubkey Generation\n");
      AES_KEY k_enc;
      AES_set_encrypt_key((const unsigned char*)key,128,&k_enc);
      AES_encrypt((const unsigned char*)const_Zero,L,(const AES_KEY*)&k_enc);
      //printf("AES_128(key,0) "); print128(L); printf("\n");
      generate_subkey(key,K1,K2);



      //printf("K1             "); print128(K1); printf("\n");
      //printf("K2             "); print128(K2); printf("\n");

     

      //printf("\nExample 2: len = %d\n",mlen);
      //printf("M              "); print_hex("                ",M,mlen);
      AES_CMAC(key,M,l_pt+64,T);
      FILE *fp1;
	  fp1 = fopen("output.txt","w");
	  fputs("MACT = ",fp1);
	  for (j=0; j<4;j++) {
          fprintf(fp1,"%02x",T[j]);
      }
 	  fputs("\n",fp1);
	  fclose(fp1);
      //printf("AES_CMAC       "); print128(T); printf("\n");
     
	  /*FILE *fp1;
	  fp1 = fopen("output1.txt","W+");
	  fputs("MACT = ",fp1);
	  for (j=0; j<4;j++) {
          fprintf(fp1,"%02x",T[j]);
      }
 
	  fclose(fp1);*/
      return 0;
  }

