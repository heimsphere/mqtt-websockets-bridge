SUBDIRS: echo mpd

.PHONY: subdirs $(SUBDIRS) 
subdirs: $(SUBDIRS)
     
$(SUBDIRS):
	$(MAKE) -C $@
	
default: service.o $(SUBDIRS)

.PHONY: clean
clean:
	rm -f *.o