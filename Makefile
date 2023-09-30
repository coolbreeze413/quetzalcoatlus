MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR_PATH := $(patsubst %/,%,$(dir $(MAKEFILE_PATH)))
MAKEFILE_DIR_NAME := $(notdir $(MAKEFILE_DIR_PATH))
# $(info )
# $(info MAKEFILE_LIST=$(MAKEFILE_LIST))
# $(info MAKEFILE_PATH=$(MAKEFILE_PATH))
# $(info MAKEFILE_DIR_PATH=$(MAKEFILE_DIR_PATH))
# $(info MAKEFILE_DIR_NAME=$(MAKEFILE_DIR_NAME))
# $(info )

PREFIX ?= $(MAKEFILE_DIR_PATH)/install
$(info PREFIX=$(PREFIX))

SHELL := /bin/bash
DEFAULT_BRANCH := main

CMAKE_BUILD_DIR := $(MAKEFILE_DIR_PATH)/build
CMAKE_SOURCE_DIR := $(MAKEFILE_DIR_PATH)


CURRENT_DATE := $(shell date "+%d_%b_%Y")
CURRENT_DATE_FULL := $(shell date "+%d_%B_%Y")
CURRENT_TIME := $(shell date +"%H_%M_%S")
$(info CURRENT_DATE=$(CURRENT_DATE))
$(info CURRENT_TIME=$(CURRENT_TIME))


VERSION := $(shell git describe --tags --abbrev=0)
ifeq ($(VERSION),)
VERSION := v0.0.0
endif


GIT_HASH := $(shell git rev-parse HEAD)
ifeq ($(GIT_HASH),)
GIT_HASH := zyxwvutsrqponmlkjihgfedcba
endif


CURRENT_BRANCH := $(shell git symbolic-ref HEAD --short 2>/dev/null || echo "no_branch")
$(info CURRENT_BRANCH=$(CURRENT_BRANCH))


DEPLOY_PACKAGE_DIR_PATH := $(CMAKE_SOURCE_DIR)/deploy
LINUXDEPLOYQT_APPIMAGE_FILE_PATH := $(CMAKE_BUILD_DIR)/linuxdeployqt-continuous-x86_64.AppImage


.DEFAULT_GOAL := all

.PHONY: all
all: install


.PHONY: install
install: run-cmake
	@rm -rf $(PREFIX)
#$(MAKE) pre-install
	$(MAKE) -C build install
#$(MAKE) post-install
	@echo "local install ready."


# phony target to force cmake run
.PHONY: run-cmake
run-cmake:
	cmake -DCMAKE_INSTALL_PREFIX=$(PREFIX) -DQUETZALCOATLUS_VERSION=$(VERSION) -DQUETZALCOATLUS_GIT_HASH=$(GIT_HASH) -S $(CMAKE_SOURCE_DIR) -B $(CMAKE_BUILD_DIR)


.PHONY: clean
clean:
ifeq ("$(wildcard $(CMAKE_BUILD_DIR))","")
	$(info nothing to clean, CMake build directory does not exist!)
else
	$(MAKE) -C build clean
endif


.PHONY: distclean
distclean:
ifeq ("$(wildcard $(CMAKE_BUILD_DIR))","")
	$(info nothing to clean, CMake build directory does not exist!)
else
	$(MAKE) -C build clean
endif
	@rm -rf $(CMAKE_BUILD_DIR)
	@rm -rf $(PREFIX)
	@rm -rf $(DEPLOY_PACKAGE_DIR_PATH)


# https://github.com/AppImage/AppImageKit/wiki/AppDir
.PHONY: deploy
deploy: install

# create a fresh 'AppDir' : $(DEPLOY_PACKAGE_DIR_PATH)/usr/bin and $(DEPLOY_PACKAGE_DIR_PATH)/usr/lib
	@rm -rf $(DEPLOY_PACKAGE_DIR_PATH)
	@mkdir -p $(DEPLOY_PACKAGE_DIR_PATH)/usr
# copy the build into the 'AppDir'/usr, use a filter as needed, to put stuff into the deploy package
	@cd $(PREFIX) && find . | grep -f $(CMAKE_SOURCE_DIR)/deployment_filter.txt | xargs -d '\n' cp -r --parents -t $(DEPLOY_PACKAGE_DIR_PATH)/usr
# copy docs into 'AppDir'/usr/docs

# create 'AppImage' using linuxdeployqt
	@echo "creating AppImage using linuxdeployqt..."
ifeq ("$(wildcard $(LINUXDEPLOYQT_APPIMAGE_FILE_PATH))","")
	@wget -q -O $(LINUXDEPLOYQT_APPIMAGE_FILE_PATH) https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
	@chmod +x $(LINUXDEPLOYQT_APPIMAGE_FILE_PATH)
endif # if LINUXDEPLOYQT_APPIMAGE_FILE_PATH does not exist
	@cd $(DEPLOY_PACKAGE_DIR_PATH) && $(LINUXDEPLOYQT_APPIMAGE_FILE_PATH) $(DEPLOY_PACKAGE_DIR_PATH)/usr/share/applications/quetzalcoatlus.desktop -appimage -no-translations &> $(CMAKE_SOURCE_DIR)/linuxdeployqt.log

# remove the artifacts from linuxdeployqt
	@rm -rf $(DEPLOY_PACKAGE_DIR_PATH)/AppRun
	@rm -rf $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus.desktop
	@rm -rf $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus.png
	@rm -rf $(DEPLOY_PACKAGE_DIR_PATH)/.DirIcon
# rename the 'AppDir'/usr into the application name and this is our 'portable' deploy package
	@mv $(DEPLOY_PACKAGE_DIR_PATH)/usr $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)

# create a tarball archive
	@echo "creating deployable tar.gz package..."
	@cd $(DEPLOY_PACKAGE_DIR_PATH) && tar -cvzf quetzalcoatlus_$(VERSION).tar.gz quetzalcoatlus_$(VERSION)



.PHONY: update
update:
	git fetch --tags
	git fetch
	git submodule update --init --recursive


.PHONY: prune
prune:
ifneq ($(CURRENT_BRANCH), $(DEFAULT_BRANCH))
	$(error must be on the default branch \'$(DEFAULT_BRANCH)\' to make prune!)
endif
# clean up branches
# remove pointers to remote branches that don't exist
	git fetch --prune
	git fetch --tags --force --prune
# delete local branches which don't have remotes (merged only)
#git branch -vv | grep ': gone]' | awk '{print $1}' | xargs git branch -d || true
# force delete local branches without remotes (unmerged)
	git branch -vv | grep ': gone]' | awk '{print $1}' | xargs git branch -D || true


# phony target to reset repo to pristine state (use with caution!)
.PHONY: pristine
pristine:
ifneq ($(CURRENT_BRANCH), $(DEFAULT_BRANCH))
	$(error must be on the default branch \'$(DEFAULT_BRANCH)\' to make pristine!)
endif
	git fetch --tags --force --prune
	git fetch
	git reset --hard HEAD
# we want to continue with current branch itself.
	git pull --ff-only || true
	git submodule update --init --recursive
	git submodule foreach --recursive git reset --hard


.PHONY: dummy
dummy:
	$(info doing nothing here.)
