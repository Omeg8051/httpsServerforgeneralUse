appName := camera2pipe

compiler := g++ -Winline -fpermissive -Wnarrowing

srcDir := ./src

objDir := ./obj

#cvobj := $(shell find $(CV_LIB_PATH)/libopencv_* | grep -F -v -e "so.")

loc_INC_DIR := ./include

srcs := $(shell find $(srcDir) | grep -F -e .c -e .cpp)

INCS = ./include

locINCS = $(shell find $(loc_INC_DIR) | grep -F -e .h)

#ALT_INCS = ${CV_INCLUDE_PATH}


vpath %.c $(sort $(dir $(SRCS)))

default: server
#	@echo $(cvobj)
	@echo "make all"


server:
	@echo "compiling"
	@$(compiler) -c $(srcDir)/server_TLS.cpp -I $(INCS)
	@$(compiler) -c $(srcDir)/HttpsServerThreadFunctions.cpp -I $(INCS)
	@echo $(srcs)
	@mv ./*.o $(objDir)/
	@echo "linking"
	@$(compiler) -o ./server_vid.out $(objDir)/server_TLS.o $(objDir)/HttpsServerThreadFunctions.o $(cvobj) -lcrypto -lssl

debug:
	@echo "compiling"
	@$(compiler) -g -c $(srcDir)/server_TLS.cpp -I $(INCS)
	@$(compiler) -g -c $(srcDir)/HttpsServerThreadFunctions.cpp -I $(INCS)
	@echo $(srcs)
	@mv ./*.o $(objDir)/
	@echo "linking"
	@$(compiler) -g -o ./server_vid.out $(objDir)/server_TLS.o $(objDir)/HttpsServerThreadFunctions.o $(cvobj) -lcrypto -lssl


