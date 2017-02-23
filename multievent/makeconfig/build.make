##
# 总的make include
#
# build.make
#
#

ifdef ME_USER_DEPTH
  ifdef ME_USER_SRC
    include $(ME_USER_DEPTH)/config.make
    include $(ME_USER_DEPTH)/platform.make
    include $(ME_USER_DEPTH)/userdef.make
    include $(ME_USER_DEPTH)/defines.make
    include $(ME_USER_DEPTH)/rule.make
  else
    all clean : UserSrcMissed
  endif
else
  all clean : UserDepthMissed
endif

UserDepthMissed:
	@echo ERROR: you must define ME_USER_DEPTH before including build.make

UserSrcMissed:
	@echo ERROR: there's nothing to build unless you expcilit name ME_USER_SRC
