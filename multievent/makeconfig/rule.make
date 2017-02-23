##
# 编译规则设置
#
# rule.make
#

#

ME_OBJS =
ifdef ME_SRC
  ME_OBJS = $(addsuffix .$(ME_FILE_EXT_OBJ), $(addprefix $(ME_OBJ_DIR)/, $(basename $(notdir $(ME_SRC)))))
endif

ME_CLEANUP ?=

ME_CLEANUP += $(ME_OBJS)

ifdef ME_BIN
  ME_VBIN = $(ME_BIN_DIR)/$(ME_BIN)
  ME_CLEANUP += $(ME_VBIN)
endif

ifdef ME_DLL
  ME_VDLL = $(ME_DLL_DIR)/$(ME_DLL)
  ME_CLEANUP += $(ME_VDLL)
endif

ifdef ME_LIB
  ME_VLIB = $(ME_LIB_DIR)/$(ME_LIB)
  ME_CLEANUP += $(ME_VLIB)
endif

# 设定源文件目录
VPATH = $(ME_SRC_DIRS)

# 主makefile的目标定义
ME_BUILD_TARGETS_LOCAL = all.local clean.local

# 在有多个makefile时, 下面这个变量代表所有非主makefile的目标
ME_BUILD_TARGETS_NESTED = $(ME_BUILD_TARGETS_LOCAL:.local=.nested)

# 设定all目标的依赖, 当有多个makefile时, 首先处理其他makefile
all: all.nested all.local

# 设定clean目标的依赖，同样的是, 当有多个makefile时, 先处理其他makefile
clean: clean.nested clean.local

# 设定all.local目标的所有依赖项
all.local: CheckDir $(ME_VLIB) $(ME_VDLL) $(ME_VBIN) $(ME_OBJS)

# 设定需要检查的目录
ME_CHECK_DIRS += $(ME_LIB_DIR) $(ME_DLL_DIR) $(ME_BIN_DIR) $(ME_OBJ_DIR)
CheckDir: $(ME_CHECK_DIRS)

# 检查目录是否存在, 如果不存在那么就创建出来
$(ME_CHECK_DIRS):
	@test -d $@ || mkdir -p $@

# 进行静态模式匹配, 把所有的cpp编译成为.o文件
$(ME_OBJ_DIR)/%.$(ME_FILE_EXT_OBJ): %.$(ME_FILE_EXT_SRC)
	$(COMPILE.cpp) $(ME_OUTPUT) $@ $<

$(ME_VBIN): $(ME_OBJS) $(ME_VDLL) $(ME_VLIB)
	$(BIN.cpp) $(ME_OUTPUT_BIN_FLAG) $@ $(ME_OBJS) $(ME_LINKS)

$(ME_VDLL): $(ME_OBJS) $(ME_VLIB)
	$(LINK.cpp) $(ME_OUTPUT_DLL_FLAG) $@ $(ME_OBJS) $(ME_LINKS)

ifeq ($(ME_COPY_TO_SYS_LIBS_FOLDER), 1)
	$(CP) $@ $(ME_SYS_LIBS_FOLDER)
endif

$(ME_VLIB): $(ME_OBJS)
	$(AR.cpp) $(ME_OUTPUT_LIB_FLAG) $@ $?
	$(CHMOD) $@
	
ifeq ($(ME_COPY_TO_SYS_LIBS_FOLDER), 1)
	$(CP) $@ $(ME_SYS_LIBS_FOLDER)
endif


# 嵌套make时, 做静态模式匹配, make非主makefile的所有makefile
%.$(ME_MULTI_MAKEFILE_EXT): %
	$(MAKE) $(ME_MMFILE_TARGET) -f $<

%.$(ME_SUB_DIRS_EXT): %
	$(MAKE) $(ME_SUB_DIRS_TARGET) -C $< -f $(ME_MAKEFILE_NAME)

%.$(ME_SUB_MAKEFILE_DIRS_EXT): %
	$(MAKE) $(ME_SUBMFILE_DIRS_TARGET) -C $< -f $(ME_SUB_MAKEFILE_NAME)

$(ME_BUILD_TARGETS_NESTED):
ifdef ME_MULTI_MAKEFILES
	$(MAKE) -f $(ME_MAKEFILE_NAME) $(ME_MULTI_MAKEFILES) ME_MMFILE_TARGET=$(@:.nested=)
endif

ifdef ME_SUB_DIRS
	$(MAKE) -f $(ME_MAKEFILE_NAME) $(ME_SUB_DIRS) ME_SUB_DIRS_TARGET=$(@:.nested=)
endif

ifdef ME_SUB_MAKEFILE_DIRS
	$(MAKE) -f $(ME_MAKEFILE_NAME) $(ME_SUB_MAKEFILE_DIRS) ME_SUBMFILE_DIRS_TARGET=$(@:.nested=)
endif

clean.local:
ifneq ($(ME_CLEANUP), )
	$(RM) $(ME_CLEANUP)
endif













