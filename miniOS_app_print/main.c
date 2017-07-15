#include 	"lib.h"

int main(int argc, char **argv)
{
	char * str;
	if(argc > 1)
		str = argv[1];
	else
		str = "NULL";
	
	while(1){
		write(str);
		sleep(100);	//ÐÝÃß100*10ms
	}

	return 0; 
}
 
