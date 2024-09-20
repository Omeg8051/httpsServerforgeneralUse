//#include <opencv2/opencv.hpp>


#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include <HttpsServerThreadFunctions.hpp>

#include <openssl/ssl.h>
#include <openssl/err.h>


using namespace std;

const char  *chain_path_d = "./cert.pem",*key_path_d="./key.pem";

const char *chain_path = chain_path_d,*key_path = key_path_d;

ServerThreadInput server_cfg;


//VideoCapture *cam;
//Mat frm;
char e;
//vector<unsigned char> jpg;
//vector<int> encodectl;
int *compqval;
unsigned long ot,pgt;
//unsigned char fps, fpsd;
int devpipefd;
char lkfrm = 0;
unsigned long tftm[2] = {0,500};
	
pthread_t network;
char syncHandler;

int (*clilst)[2];

SSL_CTX *ctx;

void sigInt(int){
	printf("terminated from SIGTERM\n");
	e = 'q';
	int ff = fcntl(0,F_GETFL);
	//printf("fdl:%x\n",ff);
	fcntl(0,F_SETFL,ff | O_NONBLOCK);


}




int main (int argc,const char **argv){
	signal(SIGINT,sigInt);
	
	//printf("hello world\n");
	//if(argc <= 1){
	//	printf("\n");
	//	return -1;
	//}
	//init all used variables from available options like what an actuall useful programe does
	//this sucks :3
	int tmp_arg;
	server_cfg.port = 6969;
	for(int j = 1;j<argc;j++){
		//detect if an option
		if(argv[j][0] != '-'){
			//not an option. skip it
			continue;
		}
		switch(argv[j][1]){
			case 'p':
				//port
				j++;
				tmp_arg = c_str2u_int(argv[j],strlen(argv[j]));
				printf("use assigned port:<%d>\n",tmp_arg);
				//leave some space here if I want to filter the result.
				server_cfg.port = tmp_arg;
			break;

			case 'k':
				j++;
				key_path = argv[j];
				printf("private key path:<%s>\n",key_path);


			break;
			
			case 'c':
				j++;
				chain_path = argv[j];
				printf("chain path:<%s>\n",chain_path);


			break;
			
			case 'h':
				
			
			default:

				if(argv[j][1] != 'h'){
					printf("unknown argument: %c\n",argv[j][1]);
				}
				printf("useage:\n%s[ -<c|k|v|d|e|p> <argument>][...]\n\n\tc:\tset <argument> as chain path.\n\tk:\tset <argument> as private key path.\n\tv:\tset <argument> as video device path.\n\td:\tset <argument> as platform device path.\n\te:\tset <argument> as platform device control exe path, or \"echo\" to disable this function\n\tp:\tset <argument> as server port.\n\n(more on the device driver)\nthe driver accepts single charactor commands from the stdin:\n\t{w,s,a,d}:\tmove the platform\n\t{l}:\ttoggle night vision IR lamp\n\t{0,1}:\tAC function; 0->off; 1->on\n",argv[0]);

				return 0;
			break;	
		}
	}

	//init SSL_CTX

	

	clilst = getClientList();
	server_cfg.ctx = create_context();
	configure_context(server_cfg.ctx,chain_path,key_path);
	
	pthread_create(&network,NULL,netFuncAcc,&server_cfg);
		
	
	//oprn camera. The whole point is to be able to use camera. So end prog if failed.
	/*cam = new VideoCapture(camera_path);

	if(cam == 0){
		printf("camera %s can't be opened\n",camera_path);
		e = 'q';
	} else {
		//cam->set(CAP_PROP_FORMAT,CAP_PROP_CODEC_PIXEL_FORMAT);

		//for(int i = 0;i<20;i++)

		//open server(envoke network)
		pthread_create(&network,NULL,netFuncAcc,NULL);
		encodectl.push_back(IMWRITE_JPEG_QUALITY);
		encodectl.push_back(50);
		compqval = encodectl.data();
		//printf("quality:%d\n",compqval[1]);
							
	
	}

		

	if(mkfifo("hwpipe",0666)){
		printf("./hwpipe exist or not enough permission to alter file system\n");
	}
	printf("opening platform hardware\n");
		
	hwpipefd = open("hwpipe",O_RDWR,0666);
	if(hwpipefd < 0){
		printf("cant open platform hardware. no platform support\n");
		hwpipefd = 0;
	} else {
		printf("opened platform hardware\n");
		write(hwpipefd,"x",1);
		//system("/home/omeg/proj/camPlatctl/platctl /dev/ttyUSB0 -f < hwpipe&");
		system(buffer[0]);
	}
	*/

	while(e != 'q'){
		usleep(200000);
		char wha[100];
		char i;
		//int cmdl = read(STDIN_FILENO,wha,100);
		//printf(ANSI_COLOR_YELLOW "[INFO]cmd:%s\n" ANSI_COLOR_RESET,wha);
		switch(wha[0]){
			case 'l':
				printf("list clients:{");
				for(i = 0;i < CLI_MAX;i++){
					if(clilst[i][0]){
						printf("{%d,%d}",clilst[i][0],clilst[i][1]);
					}
				}
				printf("}\n");
				break;
			
			
			default:

				break;	
		}
		
	}

	
	stopServer();
	pthread_join(network,NULL);
	//I keep forgot to wait for all the client thread to join
	waitAllCliEnd();
	
	printf("process ended gracefully\n");
	return 0;
}

