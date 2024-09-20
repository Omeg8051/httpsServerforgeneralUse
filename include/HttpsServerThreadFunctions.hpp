#ifndef AC6CC0CA_B2D6_485F_86CC_5E81A52AE0F0
#define AC6CC0CA_B2D6_485F_86CC_5E81A52AE0F0


#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>


#include <openssl/ssl.h>
#include <openssl/err.h>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"




#define CLI_MAX 10
#define FPS_INTERVAL 10
#define DOC_TRANS_CHUNK 1024




struct ServerThreadInput
{
	SSL_CTX *ctx;
	unsigned short port;
};



extern SSL_CTX *create_context();
extern void configure_context(SSL_CTX *ctx,const char *chain_path,const char *key_path);
extern unsigned int c_str2u_int(const char *s,char lim = 100);
/**
 * input_args is a pinter to a ServerThreadInput*
 */
extern void* netFuncAcc(void* input_args);
extern void stopServer();
extern int (*getClientList(void))[2];
extern void waitAllCliEnd();
#endif /* AC6CC0CA_B2D6_485F_86CC_5E81A52AE0F0 */
