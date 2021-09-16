#include<stdio.h>
#include<stdlib.h>
#include<string.h>


void splitinput(char buffer[255], unsigned char p[128]){
	char* ptr = strtok(buffer,"=");
	
	
	while(ptr!=NULL){
		strcpy(p,ptr);
		ptr = strtok(NULL,"=");
	}
}


int main(int argc, char *argv[])
{
	
	printf("%03x\n",0);
	char buffer[10][255];
	FILE *fp;
	fp = fopen(argv[1],"r");
	ssize_t read;
	int i = 0;
	while(fgets(buffer[i],255,fp)!= NULL){
		i++;
	}
	//int k=0;
	unsigned char key[128];
	unsigned char count[128];
	unsigned char bearer[128];
	unsigned char direction[128];
	unsigned char length[128];
	unsigned char plaintext[1024]; 
	unsigned char kb[128];
	
	splitinput(buffer[0],key);
	splitinput(buffer[1],count);
	splitinput(buffer[2],bearer);
	splitinput(buffer[3],direction);
	
	splitinput(buffer[5],plaintext);
	
	
	printf("%s\n",key);
	
	int a=0,b=0;
	/*while(a<strlen(key)){
		if(!(key[a]== ' ' && key[a+1]==' ')){
			kb[b]=key[a];
			b++;
		}
		a++;
	}*/
	
	unsigned char in_key[128];
	
	int k=0,c=0;
	while(k<128){
		if(count[k] == ' '){
			k++;
			continue;
		}
		if(k<128){
			in_key[c] =count[k];
			c++;
			k++;
		}
	}
	printf("%s\n",in_key);
	/*do{
		while(*p == ' '){
			p++;
		}
		
	}while(key = *p++);*/
	
	const unsigned char* pos = key;
	for(size_t count =0;count <sizeof(in_key)/sizeof(*in_key);count++ ){
		if(*pos == ' '){
			pos++;
		}
		sscanf(pos,"%2hhx",&in_key[count]);
		pos += 2;
	}
	while(a<17){
		printf("%x\n",in_key[a]);
		a++;
	}
	/*for(int j=0;j<16;j++){
		for(int k=0;k<strlen(key);k++){
			if(strcmp((const char*)key[k]," ")==0){
				continue;
			}
			else{
				in_key[j] = strcat((const char*)key[k],(const char*)key[k+1]);
				break;
			}
		}
	}*/
	//printf("key is %x\n",in_key[3]);
	
	
	return 0;
	
}
