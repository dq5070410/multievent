##
# 使用者需要给出的定义
#
# userdef.make
#
#

# ----------------------------------------------------------
# ME_USER_DEPTH必须在你自己的makefile中定义, 这个变量用于存放
# 你当前的makefile到userdef.make这个文件"相对距离"
# 
# 例子1:
# 你的makefile放在/root/svn/src, 而userdef.make放在/root, 
# 那么ME_USER_DEPTH就应该定义为: ../..
#
# 例子2:
# 你的makefile放在/root/svn/src, 而userdef.make放在/root/makeinclude,
# ME_USER_DEPTH应该定义为: ../makeinclude
# ----------------------------------------------------------
ME_MAKEFILE_ROOT = $(ME_USER_DEPTH)

# ----------------------------------------------------------
# ME_USER_SRC必须在你自己的makefile中定义, 指定需要编译
# 哪些文件
# ----------------------------------------------------------
ME_SRC = $(ME_USER_SRC)

# ----------------------------------------------------------
# 定义输出文件名
# ----------------------------------------------------------
ifdef ME_USER_BIN
  ME_BIN = $(ME_USER_BIN)
  ME_OBJ_FOLDER = $(ME_USER_BIN)
endif

ifdef ME_USER_DLL
  ME_DLL = lib$(ME_USER_DLL).$(ME_FILE_EXT_DLL)
  ME_OBJ_FOLDER = $(ME_USER_DLL)
endif

ifdef ME_USER_LIB
  ME_LIB = lib$(ME_USER_LIB).$(ME_FILE_EXT_LIB)
  ME_OBJ_FOLDER = $(ME_USER_LIB)
endif

# ----------------------------------------------------------
# 定义输出路径
# ----------------------------------------------------------
ifeq ($(ME_DEBUG), 1)
  ME_OUTPUT_DIR = $(ME_MAKEFILE_ROOT)/../bin/debug
else
  ME_OUTPUT_DIR = $(ME_MAKEFILE_ROOT)/../bin/release
endif

ifdef ME_USER_OBJ_DIR
  ME_OBJ_DIR = $(ME_USER_OBJ_DIR)
else
  ifdef ME_OBJ_FOLDER
    ME_OBJ_DIR = $(ME_OUTPUT_DIR)/obj/$(ME_OBJ_FOLDER)
  else
    ME_OBJ_DIR = $(ME_OUTPUT_DIR)/obj
  endif
endif

ifdef ME_BIN
ifdef ME_USER_BIN_DIR
  ME_BIN_DIR = $(ME_USER_BIN_DIR)
else
  ME_BIN_DIR = $(ME_OUTPUT_DIR)
endif
endif

ifdef ME_DLL
ifdef ME_USER_DLL_DIR
  ME_DLL_DIR = $(ME_USER_DLL_DIR)
else
  ME_DLL_DIR = $(ME_OUTPUT_DIR)
endif
endif

ifdef ME_LIB
ifdef ME_USER_LIB_DIR
  ME_LIB_DIR = $(ME_USER_LIB_DIR)
else
  ME_LIB_DIR = $(ME_OUTPUT_DIR)
endif
endif

# ----------------------------------------------------------
# 用户头文件所在目录
# ----------------------------------------------------------
ifdef ME_USER_INCLUDE_DIRS
  ME_INCLUDE_DIRS +=  $(addprefix $(ME_COMPILE_INCLUDE_DIR), $(ME_USER_INCLUDE_DIRS))
endif

# ----------------------------------------------------------
# 用户源文件所在目录
# ----------------------------------------------------------
ME_SRC_DIRS_DEFAULT = .

ifdef ME_USER_SRC_DIRS
  ME_SRC_DIRS += $(ME_USER_SRC_DIRS)
else
  ME_SRC_DIRS += $(ME_SRC_DIRS_DEFAULT)
endif

# ----------------------------------------------------------
# 用户预定义宏
# ----------------------------------------------------------
ifdef ME_USER_DEFINES
  ME_COMPILE_FLAG += $(addprefix $(ME_COMPILE_DEFINE), $(ME_USER_DEFINES))
endif

# ----------------------------------------------------------
# 用户需要import的库
# ----------------------------------------------------------
ifdef ME_USER_LINKS
  ME_LINKS += $(addprefix $(ME_LINK_TARGET), $(ME_USER_LINKS))
endif

# ----------------------------------------------------------
# 用户需要import的库所在的目录
# ----------------------------------------------------------
ifdef ME_USER_LINKS_DIRS
  ME_LINKS_DIRS += $(addprefix $(ME_LINK_DIR), $(ME_USER_LINKS_DIRS))
endif

# ----------------------------------------------------------
# 用户makefile的名字
#
# 如果不指定, 默认为Makefile
#
# 这是最简单的make模式, 只有一个makefile
# ----------------------------------------------------------
ifdef ME_USER_MAKEFILE_NAME
  ME_MAKEFILE_NAME = $(ME_USER_MAKEFILE_NAME)
else
  ME_MAKEFILE_NAME = Makefile
endif

# ----------------------------------------------------------
# 如果用户有多个makefile
#
# 那么就需要用户定义ME_USER_MULTI_MAKEFILES, 这个模式下,
# 会逐一的make所有定义在ME_USER_MULTI_MAKEFILES的文件
# ----------------------------------------------------------
ME_MULTI_MAKEFILE_EXT = mmfiles

ifdef ME_USER_MULTI_MAKEFILES
  ME_MULTI_MAKEFILES = $(addsuffix .$(ME_MULTI_MAKEFILE_EXT), $(ME_USER_MULTI_MAKEFILES))
endif

# ----------------------------------------------------------
# 如果用户有多个子目录需要编译
#
# 这个模式会逐一去make定义在ME_USER_SUB_DIRS子目录下
# 所有的叫做ME_MAKEFILE_NAME的makefile
# ----------------------------------------------------------
ME_SUB_DIRS_EXT = subdirs

ifdef ME_USER_SUB_DIRS
  ME_SUB_DIRS = $(addsuffix .$(ME_SUB_DIRS_EXT), $(ME_USER_SUB_DIRS))
endif

# ----------------------------------------------------------
# 如果用户有多个子目录需要编译
#
# 这个模式会逐一去make定义在ME_USER_SUB_DIRS子目录下
# 所有的叫做ME_SUB_DIR_MAKEFILE_NAME的makefile
# ----------------------------------------------------------
ifdef ME_USER_SUB_MAKEFILE_NAME
  ME_SUB_MAKEFILE_NAME = $(ME_USER_SUB_MAKEFILE_NAME)
endif

ME_SUB_MAKEFILE_DIRS_EXT = subdirmf

ifdef ME_USER_SUB_MAKEFILE_DIRS
  ME_SUB_MAKEFILE_DIRS = $(addsuffix .$(ME_SUB_MAKEFILE_DIRS_EXT), $(ME_USER_SUB_MAKEFILE_DIRS))
endif










