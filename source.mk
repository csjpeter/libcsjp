
debian:

android:

windows:


$(DIST_DIR)/src/devel/csjp_utility.h: \
		src/devel/csjp_utility_mxe.h \
		src/devel/csjp_utility_android.h \
		src/devel/csjp_utility.h
	@test -d $(dir $@) || mkdir -p $(dir $@)
	@case $(TARGET_DIST) in \
		(android-*) \
			echo cp -pd src/devel/csjp_utility_android.h $@ ; \
			cp -pd src/devel/csjp_utility_android.h $@ \
		;; \
		(mxe) \
			echo cp -pd src/devel/csjp_utility_mxe.h $@ ; \
			cp -pd src/devel/csjp_utility_mxe.h $@ \
		;; \
		(*) \
			echo cp -pd src/devel/csjp_utility.h $@ ; \
			cp -pd src/devel/csjp_utility.h $@ \
		;; \
	esac

$(DIST_DIR)/src/devel/csjp_utility.cpp: \
		src/devel/csjp_utility_mxe.cpp \
		src/devel/csjp_utility_android.cpp \
		src/devel/csjp_utility.cpp
	@test -d $(dir $@) || mkdir -p $(dir $@)
	@case $(TARGET_DIST) in \
		(android-*) \
			echo cp -pd src/devel/csjp_utility_android.cpp $@ ; \
			cp -pd src/devel/csjp_utility_android.cpp $@ \
		;; \
		(mxe) \
			echo cp -pd src/devel/csjp_utility_mxe.cpp $@ ; \
			cp -pd src/devel/csjp_utility_mxe.cpp $@ \
		;; \
		(*) \
			echo cp -pd src/devel/csjp_utility.cpp $@ ; \
			cp -pd src/devel/csjp_utility.cpp $@ \
		;; \
	esac

$(DIST_DIR)/Makefile.in: Makefile.in Makefile.$(PACKAGING).in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	@echo "all: build\n" > $@
	./generator.$(PACKAGING).sh Makefile.$(PACKAGING).in >> $@
	./generator.$(PACKAGING).sh Makefile.in >> $@

$(DIST_DIR)/%: %
	@test -d $(dir $@) || mkdir -p $(dir $@)
	cp -pd $< $@

source: \
	$(DIST_DIR)/Makefile.in \
	\
	$(DIST_DIR)/src/devel/csjp_exception.h \
	$(DIST_DIR)/src/devel/csjp_test.h \
	$(DIST_DIR)/src/devel/csjp_logger.h \
	$(DIST_DIR)/src/devel/csjp_skeleton.h \
	$(DIST_DIR)/src/devel/csjp_carray.h \
	$(DIST_DIR)/src/devel/csjp_object.h \
	$(DIST_DIR)/src/devel/csjp_defines.h \
	$(DIST_DIR)/src/devel/csjp_ctypes.h \
	$(DIST_DIR)/src/devel/csjp_utility.h \
	$(DIST_DIR)/src/devel/csjp_utility.cpp \
	$(DIST_DIR)/src/devel/csjp_logger.cpp \
	$(DIST_DIR)/src/devel/csjp_exception.cpp \
	\
	$(DIST_DIR)/src/container/csjp_ref_array.h \
	$(DIST_DIR)/src/container/csjp_array.h \
	$(DIST_DIR)/src/container/csjp_pod_array.h \
	$(DIST_DIR)/src/container/csjp_sorter_owner_container.h \
	$(DIST_DIR)/src/container/csjp_container.h \
	$(DIST_DIR)/src/container/csjp_bintree.h \
	$(DIST_DIR)/src/container/csjp_reference_container.h \
	$(DIST_DIR)/src/container/csjp_ownerbintree.h \
	$(DIST_DIR)/src/container/csjp_owner_container.h \
	$(DIST_DIR)/src/container/csjp_sorter_container.h \
	$(DIST_DIR)/src/container/csjp_sorter_reference_container.h \
	$(DIST_DIR)/src/container/csjp_json.h \
	$(DIST_DIR)/src/container/csjp_json.cpp \
	$(DIST_DIR)/src/container/test/bintree.cpp \
	$(DIST_DIR)/src/container/test/sorter_container.cpp \
	$(DIST_DIR)/src/container/test/container.cpp \
	$(DIST_DIR)/src/container/test/container_speed.cpp \
	$(DIST_DIR)/src/container/test/ref_array.cpp \
	$(DIST_DIR)/src/container/test/array.cpp \
	$(DIST_DIR)/src/container/test/pod_array.cpp \
	$(DIST_DIR)/src/container/test/container_speed.gnuplot \
	$(DIST_DIR)/src/container/test/json.cpp \
	\
	$(DIST_DIR)/src/human/csjp_unichar.h \
	$(DIST_DIR)/src/human/csjp_text.h \
	$(DIST_DIR)/src/human/csjp_text.cpp \
	$(DIST_DIR)/src/human/test/unichar.cpp \
	$(DIST_DIR)/src/human/test/text.cpp \
	$(DIST_DIR)/src/human/csjp_unichar.cpp \
	\
	$(DIST_DIR)/src/core/csjp_string_chunk.h \
	$(DIST_DIR)/src/core/csjp_mutex.h \
	$(DIST_DIR)/src/core/csjp_string.h \
	$(DIST_DIR)/src/core/csjp_file.h \
	$(DIST_DIR)/src/core/csjp_stopper.h \
	$(DIST_DIR)/src/core/test/mutex.cpp \
	$(DIST_DIR)/src/core/test/file.cpp \
	$(DIST_DIR)/src/core/test/string_chunk.cpp \
	$(DIST_DIR)/src/core/test/string.cpp \
	$(DIST_DIR)/src/core/csjp_mutex.cpp \
	$(DIST_DIR)/src/core/csjp_file.cpp \
	$(DIST_DIR)/src/core/csjp_string_chunk.cpp \
	$(DIST_DIR)/src/core/csjp_string.cpp \
	\
	$(DIST_DIR)/dot/containers.dot

