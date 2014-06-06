#添加规则
include ../Rules.mk

#引用头文件路径
INC_DIRS    += -I../SvrUtil -I../Profiler -I../Log -I../libGateway

#生成库文件
LIB_GATEWAY= ../$(BIN_DIR)/libappframe.a

#宏定义修改
MACROS += -DFRAME_EXPORT

#编译标签
all: $(OBJ_FILES)
	$(AR) $(LIB_GATEWAY) $(GEN_OBJS)

clean: force
	$(RM) $(OUT_DIR)

force:
