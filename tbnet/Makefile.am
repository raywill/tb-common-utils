SUBDIRS=src test src/test
UNITTESTDIRS=src/test
.PHONY: $(UNITTESTDIRS)
test: $(UNITTESTDIRS)
$(UNITTESTDIRS):
	$(MAKE) -C $@ test

