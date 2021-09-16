#include <openssl/aes.h>
#include <openssl/modes.h>
#include <openssl/rand.h> 
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define u8 unsigned char



struct ctr_state
{ 
	unsigned char ivec[AES_BLOCK_SIZE]; 
	unsigned int num;
	unsigned char ecount[AES_BLOCK_SIZE];
}; 


void init_ctr(struct ctr_state *state, const unsigned char iv[16])
{        
	state->num = 0;
	memset(state->ecount, 0, 16);
	memcpy(state->ivec, iv, 16);
}


void crypt_message(const u8* src, u8* dst, unsigned int src_len, const AES_KEY* key, const u8* iv)
{   
	struct ctr_state state;
	init_ctr(&state, iv);
	CRYPTO_ctr128_encrypt(src, dst, src_len, key, state.ivec, state.ecount, &state.num, (block128_f)AES_encrypt);
	
	//printf("state.ecount= %x\n",state.ivec[15]);
	
}

void splitinput(char buffer[255], unsigned char p[]){
	char* ptr = strtok(buffer,"=");
	
	
	while(ptr!=NULL){
		strcpy(p,ptr);
		ptr = strtok(NULL,"=");
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


void buildiv(unsigned char c[],unsigned char b[], unsigned char d[],unsigned char iv[16]){
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
	for(int k =5;k<16;k++){
		iv[k] = 0;
	}
	/*for (int i = 0; i<16;i++){
		printf("%x\n",iv[i]);
	}*/
	
}

int main(int argc, char* argv[])
{
	int len;
	char source[128];
	char cipher[128];
	char recovered[128];
	unsigned char iv[AES_BLOCK_SIZE];
	
	
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
	unsigned char plaintext[10000]; 
	unsigned char kb[32];
	
	splitinput(buffer[0],ukey);
	splitinput(buffer[1],count);
	splitinput(buffer[2],bearer);
	splitinput(buffer[3],direction);
	splitinput(buffer[4],length);
	splitinput(buffer[5],plaintext);
	
	buildiv(count,bearer,direction,iv);
	
	int a=0,b=0;
	
	/*while(a<strlen(key)){
		if(!(key[a]== ' ' && key[a+1]==' ')){
			kb[b]=key[a];
			b++;
		}
		a++;
	}*/
	//printf("%s\n",ukey);
	//printf("working here\n");
	deletespace(kb,ukey,128,32);
	//printf("%s\n",kb);
	unsigned char in_key[16];
	const unsigned char* pos = ukey;
	for(size_t count =0;count <sizeof(in_key)/sizeof(*in_key);count++ ){
		if(*pos == ' '){
			pos++;
		}
		sscanf(pos,"%2hhx",&in_key[count]);
		pos += 2;
	}
	
	
	
	
	/*while(b<16){
		printf("%x\n",in_key[b]);
		b++;
	}*/
	
	
	
	
	const unsigned char* enc_key = in_key;
	
	
	/*if(!RAND_bytes(iv, AES_BLOCK_SIZE)) 
	{
		fprintf(stderr, "Could not create random bytes.");
		exit(1);    
	}*/
	

	
	
	AES_KEY key;
	AES_set_encrypt_key(enc_key, 128, &key);
	
	//strcpy(source,"7ec61272 743bf161 4726446a 6c38ced1");
	/*source[0] = 0x7e;
	source[1]=0xc6;
	source[2]=0x12;
	source[3]=0x72;
	source[4]=0x74;
	source[5]=0x3b;
	source[6]=0xf1;
	source[7]=0x61;
	source[8]=0x47;
	source[9]=0x26;
	source[10]=0x44;
	source[11]=0x6a;
	source[12]=0x6c;
	source[13]=0x38;
	source[14]=0xce;
	source[15]=0xd1;*/
	
	unsigned char len_pt[10];
	deletespace(len_pt,length,10,10);
	unsigned long l_pt = strtoul(len_pt,NULL,10);
	//printf("%lu\n",l_pt);
	
	unsigned char pt[l_pt];
	deletespace(pt,plaintext,10000,l_pt);
	
	//printf("%d\n",pt[198]);
	
	int number_t;
	if(l_pt % 128 == 0){
		number_t = l_pt/128;
	}
	else{
		number_t = l_pt/128 +1;
	}  
	//printf("%d\n",number_t);
	
	int rem;
	int wher;
	int pad = 0;
	if(l_pt%128 != 0){
		rem = l_pt%128;
		//printf("rem is %d\n",rem);
		wher = rem/8;;
		pad = (128-rem)%32;
	}

	//printf("%d\n",pad);
	//printf("%s\n",pt);
	unsigned char pt_proc[number_t * 16];
	const unsigned char* pos_p = pt;
	for(size_t count_p =0;count_p <sizeof(pt_proc)/sizeof(*pt_proc);count_p++ ){
		if(*pos_p == ' '){
			pos_p++;
		}
		sscanf(pos_p,"%2hhx",&pt_proc[count_p]);
		pos_p += 2;
	}
	
	FILE *fp1;
	fp1 = fopen("output.txt","w+");
	fputs("Ciphertext = ",fp1);
	
	int kl=0;
	/*while(kl <155){
		printf("%x\n",pt_proc[kl]);
		kl++;
	}*/
	
	
	
	len = 16;
	int max_pt = (int)strlen(pt)-2;
	
	int p_pt = 0;
	for(int p_t=0;p_t<number_t;p_t++){
		int p_16;
		p_16= 0;
		while(p_16<16){
			if(p_pt >= number_t*16){
				break;
			}
			source[p_16] = pt_proc[p_pt];
			p_16++;
			p_pt++;
		}
		//printf("%x\n",source[7]);
		memset(recovered, 0, sizeof(recovered));
		crypt_message((const u8*)source, (u8*)cipher, len, &key, iv);
		//memset(source, 0, sizeof(source));
		b=0;
		
		if(p_t + 1 < number_t ){
			while(b<16){
				if(b % 4 == 0){
					fputs(" ",fp1);
				}
				unsigned int k = (cipher[b] & 0xff);
				fprintf(fp1,"%02x",k);
				b++;
			}
		}
		else{
			if(l_pt%128 == 0){
				while(b<16){
					if(b % 4 == 0){
						fputs(" ",fp1);
					}
					unsigned int k = (cipher[b] & 0xff);
					fprintf(fp1,"%02x",k);
					b++;
				}
			}
			else{
				//printf("enter here and wher is%d\n,%d\n",wher,b);
				while(b<(wher)){
					if(b % 4 == 0){
						fputs(" ",fp1);
					}
					unsigned int k = (cipher[b] & 0xff);
					fprintf(fp1,"%02x",k);
					b++;
				}
				if(b ==wher){
					unsigned char temp_0[4];
					int err_pos = b+4-(b %4)-1;
					int temp_er = err_pos;
					while (temp_er>=wher){
						//printf("cipher err ")
						if(pad>=8){
							cipher[temp_er]=0x00;
							pad = pad-8;
						}
						else{
							cipher[temp_er] = (cipher[temp_er]>>pad)<<pad;
						}
						temp_er--;
						
					}
					while(err_pos>=(wher)){
						if(err_pos % 4 == 0){
							fputs(" ",fp1);
						}
						unsigned int k = (cipher[wher] & 0xff);
						//printf("cipher is %x\n",)
						//printf("cipher is:%02x",k);
						fprintf(fp1,"%02x",k);
						wher++;
					}
				}
			}
		}
		iv[15]+=1;
	}
	fputs("\n",fp1);
	fclose(fp1);
	
	
	/*len = 16;
	
	
	memset(recovered, 0, sizeof(recovered));
	crypt_message((const u8*)source, (u8*)cipher, len, &key, iv);
	while(b<16){
		printf("%x\n",cipher[b]);
		b++;
	}*/
	
	return 0;
}







