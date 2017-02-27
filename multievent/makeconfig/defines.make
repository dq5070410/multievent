##
# 编译选项相关定义
#
# defines.make
#
#

# ----------------------------------------------------------
# 默认编译参数
# ----------------------------------------------------------
ME_COMPILE_DEFAULT += $(ME_COMPILE_PIPE) $(ME_COMPILE_WARNING) $(ME_LINK_PIC)

ifeq ($(ME_DEBUG), 1)
  ME_COMPILE_DEFAULT += $(ME_COMPILE_DEBUG)
endif

ifeq ($(ME_OPTIMIZE), 1)
  ME_COMPILE_DEFAULT += $(ME_COMPILE_OPTIMIZE)
endif

ifeq ($(ME_PROFILE), 1)
  ME_COMPILE_DEFAULT += $(ME_COMPILE_PROFILE)
  
endif

ifeq ($(ME_64_BIT), 1)
  ME_COMPILE_DEFAULT += $(ME_COMPILE_64_BIT)
endif

# ----------------------------------------------------------
# C++ 默认编译参数
# ----------------------------------------------------------
ME_COMPILE_CPP += $(ME_COMPILE_DEFAULT)

# ----------------------------------------------------------
# 默认编译的预定义宏
# ----------------------------------------------------------
ME_COMPILE_FLAG ?=
ME_COMPILE_FLAG += $(ME_COMPILE_DEFAULT) $(ME_DEFAULT_GNU_FILE)

ifeq ($(ME_LINUX), 1)
  ME_COMPILE_FLAG += -DME_LINUX
endif

ifeq ($(ME_DEBUG), 1)
  ME_COMPILE_FLAG += -DME_DEBUG
endif

ifeq ($(ME_NO_ASSERTE), 1)
  ME_COMPILE_FLAG += -DME_DISABLE_ASSERTE
endif

ifeq ($(ME_THREAD), 1)
  ME_COMPILE_FLAG += -D_REENTRANT
endif

# ----------------------------------------------------------
# C++ 默认编译选项
# ----------------------------------------------------------
ME_INCLUDE_DIRS ?=
ME_COMPILE_CPP_FLAG += $(ME_COMPILE_FLAG) $(ME_INCLUDE_DIRS)

# ----------------------------------------------------------
# 默认需要link的库
# ----------------------------------------------------------
ME_LINKS ?=
ME_LINKS += $(ME_LINK_DL) $(ME_LINK_RT)

ifeq ($(ME_THREAD), 1)
  ME_LINKS += $(ME_LINK_PTHREAD)
endif

# ----------------------------------------------------------
# 默认需要link的库所在的目录
#
# 无需赋值, 默认即为系统的lib目录
# ----------------------------------------------------------
ME_LINKS_DIRS ?=

ifdef ME_BIN_DIR
  ME_LINKS_DIRS += -L$(ME_BIN_DIR)
endif

ifdef ME_DLL_DIR
  ME_LINKS_DIRS += -L$(ME_DLL_DIR)
endif

ifdef ME_LIB_DIR
  ME_LINKS_DIRS += -L$(ME_LIB_DIR)
endif

# ----------------------------------------------------------
# 默认运行时路径
#
# 这个变量仅仅在你编译可执行文件时有效
# 它用于标识, 在程序运行时, 要到什么目录下去查找需要
# load的.so
# ----------------------------------------------------------
#ME_BIN_DIR ?=
#ME_BIN_RUNTIME_PATH += -Wl,-rpath -Wl,$(ME_BIN_DIR)

#ME_DLL_DIR ?=
#ME_BIN_RUNTIME_PATH += -Wl,-rpath -Wl,$(ME_DLL_DIR)

ME_BIN_RUNTIME_PATH ?=

ifdef ME_COPY_TO_SYS_LIBS_FOLDER
  ifdef ME_BIN_DIR
    ME_BIN_RUNTIME_PATH += -Wl,-rpath -Wl,$(shell cd $(ME_BIN_DIR) && pwd)
  endif

  ifdef ME_DLL_DIR
    ME_BIN_RUNTIME_PATH += -Wl,-rpath -Wl,$(shell cd $(ME_DLL_DIR) && pwd)
  endif
endif

# ----------------------------------------------------------
# 默认link的标志
# ----------------------------------------------------------
ME_LINK_DEFAULT_FLAG +=

ifeq ($(ME_PROFILE), 1)
  ME_LINK_DEFAULT_FLAG += $(ME_COMPILE_PROFILE)
endif

# ----------------------------------------------------------
# 默认link(静态库.a)的标志
# ----------------------------------------------------------
ME_LINK_LIB_FLAG += $(ME_LINK_DEFAULT_FLAG)

# ----------------------------------------------------------
# 默认link(动态库.so)的标志
# ----------------------------------------------------------
ME_LINK_DLL_FLAG += $(ME_LINK_DEFAULT_FLAG) $(ME_LINK_SHARED) $(ME_LINK_PIC) $(ME_LINKS_DIRS)

# ----------------------------------------------------------
# 默认link(可执行文件)的标志
# ----------------------------------------------------------
ME_LINK_BIN_FLAG += $(ME_LINK_DEFAULT_FLAG) $(ME_LINKS_DIRS)

# ----------------------------------------------------------
# 默认静态库输出参数
# ----------------------------------------------------------
ME_OUTPUT_LIB_FLAG ?=

# ----------------------------------------------------------
# 默认动态库输出参数
# ----------------------------------------------------------
ME_OUTPUT_DLL_FLAG ?= $(ME_OUTPUT)

# ----------------------------------------------------------
# 默认执行文件输出参数
# ----------------------------------------------------------
ME_OUTPUT_BIN_FLAG ?= $(ME_OUTPUT)

# ----------------------------------------------------------
# 默认编译动作
# ----------------------------------------------------------
COMPILE.cpp = $(CPP) $(ME_COMPILE_CPP_FLAG) $(ME_COMPILE_NOT_LINK)

# ----------------------------------------------------------
# 默认link动作
# ----------------------------------------------------------
LINK.cpp = $(LD) $(ME_LINK_DLL_FLAG)

# ----------------------------------------------------------
# 默认打包动作
# ----------------------------------------------------------
AR.cpp = $(AR) $(ME_LINK_LIB) $(ME_LINK_LIB_FLAG)

# ----------------------------------------------------------
# 默认生成执行文件的动作
# ----------------------------------------------------------
BIN.cpp = $(BIN) $(ME_LINK_BIN_FLAG) $(ME_BIN_RUNTIME_PATH)




