
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

SOURCES=\
	$(patsubst src/%.h,$(DIST_DIR)/src/%.h,\
		$(filter-out	$(wildcard src/*/csjp_utility*.h),\
				$(wildcard src/*/*.h))) \
	$(patsubst src/%.cpp,$(DIST_DIR)/src/%.cpp,\
		$(filter-out	$(wildcard src/*/csjp_utility*.cpp),\
				$(wildcard src/*/*.cpp))) \
	$(patsubst src/%.cpp,$(DIST_DIR)/src/%.cpp,\
				$(wildcard src/*/test/*.cpp))

$(info SOURCES=$(SOURCES))

source: \
	$(DIST_DIR)/Makefile.in \
	\
	$(DIST_DIR)/src/container/test/container_speed.gnuplot \
	\
	$(DIST_DIR)/src/devel/csjp_utility.h \
	$(DIST_DIR)/src/devel/csjp_utility.cpp \
	\
	$(SOURCES) \
	\
	$(DIST_DIR)/dot/containers.dot

