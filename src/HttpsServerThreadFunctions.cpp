#include <HttpsServerThreadFunctions.hpp>

char _HTTPS_INTERNAL_buffer[CLI_MAX][1024];
char _HTTPS_INTERNAL_headerBuffer[CLI_MAX][1024];



//using namespace cv;
/*
char *html = "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n\t<meta charset=\"UTF-8\">\r\n\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n\t<title>Camera</title>\r\n</head>\r\n<style>\r\n\t*{\r\n\t\tbackground-color: darkblue;\r\n\t\r\n}\r\n</style>\r\n<body>\r\n\t<h1>Bla bla bla</h1>\r\n\t<img src=\"lol.jpg\" alt=\"lol.jpg\">\r\n</body>\r\n</html>\r\n                                                                                                                                                                                                                ";
*/
const char *_HTTPS_INTERNAL_respond_root = "HTTP/1.0 200 OK\r\nServer: MyserverWithC\r\nDate: Tue, 03 Sep 2024 09:30:45 GMT\r\nContent-type: text/html; charset=utf-8\r\nContent-Length: %d\r\nEtag: \"c561c68d0ba92bbeb8b0f612a9199f72%8x\"\r\nLast-Modified: Fri, 09 Dec 2022 17:51:38 GMT\r\nConnection: keep-alive\r\n\r\n";//,size of index.html,random Int

const char *_HTTPS_INTERNAL_respond_lol = "HTTP/1.0 200 OK\r\nCache-Control: no-cache, no-store, must-revalidate\r\nExpires: 0\r\nPragma: no-cache\r\nServer: MyserverWithC\r\nDate: Tue, 03 Sep 2024 09:30:45 GMT\r\nContent-type: */*; charset=utf-8\r\nContent-Length: %d\r\nEtag: \"smcgwnmxox4pjfimorehguthrevnitiulsdmgchscvgfkvdajxbh%8x\"\r\nLast-Modified: Fri, 09 Dec 2022 17:51:38 GMT\r\nConnection: close\r\n\r\n";//,size of file,random Int

const char *_HTTPS_INTERNAL_respond_404 = "HTTP/1.0 404 NOT FOUND\r\nServer: MyserverWithC\r\nDate: Tue, 03 Sep 2024 09:30:45 GMT\r\nContent-type: text/plain; charset=utf-8\r\nContent-Length: 0\r\n\r\n";

const char *_HTTPS_INTERNAL_respond_POST = "HTTP/1.0 200 OK\r\nServer: MyserverWithC\r\nDate: Tue, 03 Sep 2024 09:30:45 GMT\r\nContent-type: text/plain; charset=utf-8\r\nContent-Length: 0\r\n\r\n";


//hardware info:
//default paths:

int _HTTPS_INTERNAL_serfd;
sockaddr_in _HTTPS_INTERNAL_server,_HTTPS_INTERNAL_client;
socklen_t _HTTPS_INTERNAL_clilen;


unsigned int _HTTPS_INTERNAL_randFtag;


struct stat _HTTPS_INTERNAL_st;

SSL_CTX *_HTTPS_INTERNAL_ctx;

int _HTTPS_INTERNAL_netalife = 1;

pthread_t _HTTPS_INTERNAL_clith[CLI_MAX];

int _HTTPS_INTERNAL_optval = 1;
sockaddr_in _HTTPS_INTERNAL_clients[CLI_MAX];
int _HTTPS_INTERNAL_clilst[CLI_MAX][2];//{{fd,opflag},{fd,opflag},...}
//opflag:{xxxx,{hwREQ},threadSync}
/*
	hwREQ:
	0:nop
	1:ACoff
	2:ACon
	3:w
	4:s
	5:a
	6:d
	7:l
	
*/


extern SSL_CTX *create_context()
{
	const SSL_METHOD *method;
	SSL_CTX *ctx;

	method = TLS_server_method();

	ctx = SSL_CTX_new(method);
	if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	return ctx;
}

extern void configure_context(SSL_CTX *ctx,const char *chain_path,const char *key_path)
{
    /* Set the key and cert*/ 
	if (SSL_CTX_use_certificate_chain_file(ctx, chain_path) <= 0) {
		ERR_print_errors_fp(stderr);
		printf("assign chain failed\n");
		exit(EXIT_FAILURE);
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, key_path, SSL_FILETYPE_PEM) <= 0 ) {
		ERR_print_errors_fp(stderr);
		printf("assign key failed\n");
		exit(EXIT_FAILURE);
	}

	/*if (SSL_CTX_(ctx, key_path, SSL_FILETYPE_PEM) <= 0 ) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
*/

}



/*
 * Augist reminded me that any file in the fs can be accessed with get command
 *
 * counter measure: 
 * 	prevent any up track path '../' from any command.
 * */

inline int strmatch(const char *a,int a_l,const char *b,int b_l){
	int sl_min = (a_l > b_l)? b_l : a_l;
	char i;
	for(i  = 0;i < sl_min;i++){
		if(a[i] != b[i])break;
	}

	return i == sl_min;
}
int ifyouwanttofuckme(const char *buffer,int stl){
	int YOUWANTTOFUCKMYSERVER = 0;
	if(buffer[0]=='/'){
		//directly search from root is forbiddened
		return -1;
	}
	for(int i = 0;i < stl && buffer[i];i++){

		if(strmatch("../",3,buffer + i,3)){
			YOUWANTTOFUCKMYSERVER = -1;
			break;

		}
	}

	return YOUWANTTOFUCKMYSERVER;

}

void sigMa(int){

}

unsigned long millis(){

	timeval tv;
	gettimeofday(&tv,NULL);

	return 1000 * tv.tv_sec + tv.tv_usec / 1000;
}

unsigned long micros(){
	timeval tv;
	gettimeofday(&tv,NULL);

	return 1000000 * tv.tv_sec + tv.tv_usec;
}


extern unsigned int c_str2u_int(const char *s,char lim = 100){
	unsigned int prt = 0;
	for(char i = 0;i<lim;i++){

		if(!s[i]){
			break;
		}
		prt *= 10;
		prt += s[i] - '0';
	}

	return prt;
}

extern void* netFuncCli(void* ctlhandle){
	signal(SIGPIPE, SIG_IGN);
	//unsigned long lsttf = millis();
	//unsigned long tfi = 500;
	int plidx;
	int *ctlh = (int *)ctlhandle;
	char cliIdx = ctlh[1] >> 8 & 0xFF;
	char localEnd = 1;
	char tff = 0;;
	ctlh[1] = 0;
	char locolkfrm;

	signal(SIGPIPE,sigMa);

	printf(ANSI_COLOR_YELLOW "[INFO]client %d connected\n" ANSI_COLOR_RESET,cliIdx);
				
	inet_ntop(AF_INET,&(_HTTPS_INTERNAL_clients[cliIdx]).sin_addr.s_addr,_HTTPS_INTERNAL_buffer[cliIdx],1024);
	printf("================\naddr: %s\nport: %d\nfd: %d\n================\n",_HTTPS_INTERNAL_buffer[cliIdx],ntohs(_HTTPS_INTERNAL_clients[cliIdx].sin_port),ctlh[0]);


	//TLS setup
	SSL *ssl;
	
	ssl = SSL_new(_HTTPS_INTERNAL_ctx);
	SSL_set_fd(ssl, ctlh[0]);

	if (SSL_accept(ssl) <= 0) {
		ERR_print_errors_fp(stdout);
		//localEnd = 0;
		printf(ANSI_COLOR_RED "[ERROR]SSL accept failed:%d\n" ANSI_COLOR_RESET,errno);
	} else {
		printf(ANSI_COLOR_YELLOW "[INFO]SSL accept succi\n" ANSI_COLOR_RESET);
	}

	//usleep(2000);

	int ff = fcntl(ctlh[0],F_GETFL);
	//printf("fdl:%x\n",ff);
	fcntl(ctlh[0],F_SETFL,ff | O_NONBLOCK);

	
		
	while(_HTTPS_INTERNAL_netalife && localEnd){
		//int lenrd = read(ctlh[0],buffer[cliIdx],1024);
		
		int lenrd = SSL_read(ssl,_HTTPS_INTERNAL_buffer[cliIdx],1024);
		int lenwr = 0;
		//printf("%d\n",errno);
		/*if(((millis() - tftm[0]) > tftm[1]) && tff){
			//tfi = 300;
			while((locolkfrm == lkfrm))usleep(1000);
			printf("tf%d\n",millis() - tftm[0]);
			tftm[0] = millis();
			locolkfrm = lkfrm;
			int fsize = jpg.size();
			int hsize = sprintf(headerBuffer[cliIdx],mjpeg_sepe,fsize);
			//lenwr = send(ctlh[0],headerBuffer[cliIdx],hsize,0);
			lenwr = SSL_write(ssl,headerBuffer[cliIdx],hsize);
			unsigned char *c = jpg.data();
			lenwr += SSL_write(ssl,c,fsize);
			
			//for(int i = 0;i<fsize;i++){
				
				//lenwr += send(ctlh[0],c+i,1,0);
				
				
			//}

			//release held main thread
					
		}*/
		_HTTPS_INTERNAL_randFtag = micros();
		if(lenrd == -1 && (errno == SSL_ERROR_NONE || errno == EWOULDBLOCK || errno == SSL_ERROR_WANT_READ))continue;
		if(lenrd == 0)break;
		if(lenrd == -1)break;
		_HTTPS_INTERNAL_buffer[cliIdx][lenrd] = 0;
		char *plbuffer;
		
		//printf("============\n%s=============\n%d\n",buffer[cliIdx],lenrd);
		//printf("=============\n%d\n",lenrd);
		int ff = fcntl(ctlh[0],F_GETFL);
		//printf("\t\t[FDCH]fdlend:%x\n",ff);
		fcntl(ctlh[0],F_SETFL,ff & ~O_NONBLOCK);
		
		switch (_HTTPS_INTERNAL_buffer[cliIdx][0])
		{
			case 'G':
				if(_HTTPS_INTERNAL_buffer[cliIdx][4]=='/'){
					if(_HTTPS_INTERNAL_buffer[cliIdx][5]==' '){
						//transfer index
						//printf("OPEN <index.html>\n");
						int lol = open("index.html",O_RDONLY);
	
						if(lol > 0){
							//transfer FILE if can open
							stat("index.html", &_HTTPS_INTERNAL_st);
							int fsize = _HTTPS_INTERNAL_st.st_size;
							int hsize = sprintf(_HTTPS_INTERNAL_headerBuffer[cliIdx],_HTTPS_INTERNAL_respond_root,fsize,_HTTPS_INTERNAL_randFtag);
							//lenwr = send(ctlh[0],headerBuffer[cliIdx],hsize,0);
							lenwr = SSL_write(ssl,_HTTPS_INTERNAL_headerBuffer[cliIdx],hsize);
						
							//make file treansfer in chunks because TLS don't like us call send on each byte
							for(int i = fsize;i>0;i-=DOC_TRANS_CHUNK){
								int trsz= (i<DOC_TRANS_CHUNK)?i:DOC_TRANS_CHUNK;

								read(lol,_HTTPS_INTERNAL_headerBuffer[cliIdx],trsz);
								//lenwr += send(ctlh[0],&c,1,0);
								int twr = SSL_write(ssl,_HTTPS_INTERNAL_headerBuffer[cliIdx],trsz);
								if(twr < 0){
									printf(ANSI_COLOR_RED "[ERROR]send error:<%d>\n" ANSI_COLOR_RESET,errno);
									break;
								}
								lenwr += twr;
							}

						} else {
							//transfer 404
							//lenwr = send(ctlh[0],respond_404,146,0);
							lenwr = SSL_write(ssl,_HTTPS_INTERNAL_respond_404,146);
						}
						
						close(lol);
						/*
					
					
						lenwr = send(clifd,respond_root,260,0);
						lenwr += send(clifd,html,528,0);
						*/
	
					} else {
						
						//transfer <FILE>
					
						//add file up track protection

					
						int idxh = 4;
						while(_HTTPS_INTERNAL_buffer[cliIdx][idxh] != ' '){
							idxh++;
						}
	
	
						_HTTPS_INTERNAL_buffer[cliIdx][idxh] = 0;
						
						
						printf("OPEN <%s>\n",_HTTPS_INTERNAL_buffer[cliIdx]+5);
						
						int strrl = strlen(_HTTPS_INTERNAL_buffer[cliIdx] + 5);

						//check if the get req include upward file search or search from root
						int lol = ifyouwanttofuckme(_HTTPS_INTERNAL_buffer[cliIdx] + 5, strrl - 3) * -1;

						if(lol != -1){
						
							lol = open(_HTTPS_INTERNAL_buffer[cliIdx]+5,O_RDONLY);
						}
						else {
							printf("no going up allowed!!\n");

						}
					
						if(lol > 0){
							//transfer FILE if can open
							stat(_HTTPS_INTERNAL_buffer[cliIdx]+5, &_HTTPS_INTERNAL_st);
							int fsize = _HTTPS_INTERNAL_st.st_size;
							int hsize = sprintf(_HTTPS_INTERNAL_headerBuffer[cliIdx],_HTTPS_INTERNAL_respond_lol,fsize,_HTTPS_INTERNAL_randFtag);
							printf("filesize:%d\n",fsize);
						
							//lenwr = send(ctlh[0],headerBuffer[cliIdx],hsize,0);
							lenwr = SSL_write(ssl,_HTTPS_INTERNAL_headerBuffer[cliIdx],hsize);
							

							//make file treansfer in chunks because TLS don't like us call send on each byte
							for(int i = fsize;i>0;i-=DOC_TRANS_CHUNK){
								int trsz= (i<DOC_TRANS_CHUNK)?i:DOC_TRANS_CHUNK;

								read(lol,_HTTPS_INTERNAL_headerBuffer[cliIdx],trsz);
								//lenwr += send(ctlh[0],&c,1,0);
								int twr = SSL_write(ssl,_HTTPS_INTERNAL_headerBuffer[cliIdx],trsz);
								if(twr < 0){
									printf(ANSI_COLOR_RED "[ERROR]send error:<%d>\n" ANSI_COLOR_RESET,errno);
									break;
									}
								lenwr += twr;
							}
	
						

						} else {
							//transfer 404
							//lenwr = send(ctlh[0],respond_404,146,0);
							lenwr = SSL_write(ssl,_HTTPS_INTERNAL_respond_404,146);
						
						}
						
						close(lol);
					}
				}
				break;

			case 'P':
				//printf("post\n");
				//lenwr = send(ctlh[0],respond_POST,139,0);
				lenwr = SSL_write(ssl,_HTTPS_INTERNAL_respond_POST,139);

				//post payload start with a # so go to index to the first # occurance in buffer[]
				plidx = 0;
				while(_HTTPS_INTERNAL_buffer[cliIdx][plidx] != '#' && (plidx < 1024)){
					plidx++;
				}
				plbuffer = _HTTPS_INTERNAL_buffer[cliIdx]+plidx;
			

				ctlh[1] = 0;
				break;
			default:
				break;
			}
		printf("total Written: %d bytes\n",lenwr);
		
		ff = fcntl(ctlh[0],F_GETFL);
		//printf("\t\t[FDCH]fdlend:%x\n",ff);
		fcntl(ctlh[0],F_SETFL,ff | O_NONBLOCK);
		


	}

	SSL_shutdown(ssl);
	SSL_free(ssl);
	close(ctlh[0]);
	//printf("%d\n",errno);
	printf(ANSI_COLOR_YELLOW "[INFO]client %d quit\n" ANSI_COLOR_RESET,cliIdx);
	//ctlh[1] = -1;
	ctlh[0] = 0;
	//clilst[cliIdx][0] = 0;
	//ff = fcntl(serfd,F_GETFL);
	////printf("fdlend:%x\n",ff);
	//fcntl(serfd,F_SETFL,ff & ~O_NONBLOCK);
	//usleep(100000);

	return NULL;
}


extern void* netFuncAcc(void* input_args)
{
	ServerThreadInput *server_cfg = (ServerThreadInput *)input_args;
	_HTTPS_INTERNAL_serfd = socket(AF_INET,SOCK_STREAM,0);


	//for(char i = 0;i<10;i++){
	//	clilst[i][1] = -1;
	//}
	if(_HTTPS_INTERNAL_serfd<0){
		//printf("error opening socket fd: %d\n",errno);
	} else {
		_HTTPS_INTERNAL_server.sin_family = AF_INET;
		_HTTPS_INTERNAL_server.sin_addr.s_addr = 0;
		_HTTPS_INTERNAL_server.sin_port = htons(server_cfg->port);

		setsockopt(_HTTPS_INTERNAL_serfd, SOL_SOCKET, SO_REUSEPORT, &_HTTPS_INTERNAL_optval, sizeof(_HTTPS_INTERNAL_optval));

		if(bind(_HTTPS_INTERNAL_serfd,(sockaddr *)&_HTTPS_INTERNAL_server,sizeof(sockaddr))){
			//printf("error binding to port::error:%d\n",errno);
		} else {
			//printf("server fd:%x\n",fcntl(serfd,F_GETFL));
			listen(_HTTPS_INTERNAL_serfd,10);
			
		}

		_HTTPS_INTERNAL_ctx = server_cfg->ctx;

		while(_HTTPS_INTERNAL_netalife){
			printf("waiting for client\n");
			int cfd = accept(_HTTPS_INTERNAL_serfd,(sockaddr*)&_HTTPS_INTERNAL_client,&_HTTPS_INTERNAL_clilen);
			if(cfd < 0) {
				printf(ANSI_COLOR_RED "[ERROR]client accept failed\n" ANSI_COLOR_RESET);
				continue;
			}
			char p = 0;
			for (;p < CLI_MAX && (_HTTPS_INTERNAL_clilst[p][0]);p++){
				
			}
			if(p >= CLI_MAX){
				close(cfd);
				printf(ANSI_COLOR_RED "[ERROR]client full\n" ANSI_COLOR_RESET);
				continue;
			}
			_HTTPS_INTERNAL_clilst[p][0] = cfd;
			_HTTPS_INTERNAL_clilst[p][1] = p << 8;
			_HTTPS_INTERNAL_clients[p] = _HTTPS_INTERNAL_client;

			pthread_create(&(_HTTPS_INTERNAL_clith[p]),NULL,netFuncCli,_HTTPS_INTERNAL_clilst[p]);

			
		}
		close(_HTTPS_INTERNAL_serfd);
			
	
			
	}
	return NULL;
		
}

extern void stopServer(){
	shutdown(_HTTPS_INTERNAL_serfd,SHUT_RDWR);
	_HTTPS_INTERNAL_netalife = 0;

}

extern int (*getClientList(void))[2]{
	return _HTTPS_INTERNAL_clilst;
}

extern void waitAllCliEnd(){
	for(char f = 0;f < CLI_MAX;f++){
		pthread_join(_HTTPS_INTERNAL_clith[f],NULL);
	}
}