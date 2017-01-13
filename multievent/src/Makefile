ME_USER_DLL = multievent
ME_USER_DEPTH = ../makeconfig
ME_USER_SRC = $(shell du -a | awk '{print $$2}' | grep cpp | grep -v svn | grep -v DllMain.cpp)
ME_USER_SRC_DIRS = $(shell du | awk '{print $$2}' | grep -v svn)
ME_USER_INCLUDE_DIRS = ../interface
ME_USER_DEFINES = ME_DISABLE_TRACE ME_DISABLE_ASSERTE

include $(ME_USER_DEPTH)/build.make

