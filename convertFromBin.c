#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

int main(){
	FILE* binfile = fopen("timestamps.dat", "rb");
	FILE* secfile = fopen("sec.txt", "w");
	FILE* usecfile = fopen("usec.txt", "w");
	
	uint32_t sec, usec;
	
	while(1){
		//For pc use sizeof(time_t) and sizeof(suseconds_t)
		//For pi use sizeof(uint32_t)
		if (!fread(&sec, sizeof(uint32_t), 1, binfile)) 
			break;
		fread(&usec, sizeof(uint32_t), 1, binfile);
		fprintf(secfile, "%lld,", sec);
		fprintf(usecfile, "%lld,", usec);
	}
	
	fclose(binfile);
	fclose(secfile);
	fclose(usecfile);
	
}
