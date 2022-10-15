#include "Utilities.h"
#include <Windows.h>
#pragma comment(lib,"Ws2_32")
#pragma region Utilities
char* GetIP(char *hostname){
	in_addr iaddr;
	hostent *host;
	host=gethostbyname(hostname);
	if(!host)
		return hostname;
	
	iaddr.s_addr=*(unsigned long*)host->h_addr;
	char *t_ip;
	t_ip=inet_ntoa(iaddr);
	return t_ip;
}
int GetGameVersion(const char *file){ 
	static int KnownVer = -1;
	if (KnownVer != -1) return KnownVer;
	if (!file) return 6156;	// guess
    FILE *f=fopen(file,"rb"); 
    int c=0; 
    if(f){ 
        fseek(f,60,SEEK_SET); 
        if(!feof(f)) 
            c=fgetc(f); 
        fclose(f); 
        if(c==16) 
            KnownVer = 6156; 
        else if(c==248) 
            KnownVer = 5767; 
		else if(c=='\b')
			KnownVer = 6729;
        else KnownVer = 0; 
    }
	return KnownVer;
}
#pragma endregion