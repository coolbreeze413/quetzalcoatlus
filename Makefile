
###############################################################################
# BUILD PLATFORM DETECTION AND FILTER
###############################################################################
# https://askubuntu.com/questions/279168/detect-if-its-ubuntu-linux-os-in-makefile
# http://linuxmafia.com/faq/Admin/release-files.html

BUILD_PLATFORM ?=

# convenience categories according to usage:
SUPPORTED_LINUX_BUILD_PLATFORMS :=
SUPPORTED_LINUX_BUILD_PLATFORMS += UBUNTU_2004
SUPPORTED_LINUX_BUILD_PLATFORMS += UBUNTU_2204

SUPPORTED_MSYS2_BUILD_PLATFORMS :=
SUPPORTED_MSYS2_BUILD_PLATFORMS += WIN32_MSYS2_MINGW64
SUPPORTED_MSYS2_BUILD_PLATFORMS += WIN32_MSYS2_UCRT64
# SUPPORTED_MSYS2_BUILD_PLATFORMS += WIN32_MSYS2_CLANG64

SUPPORTED_WIN_BUILD_PLATFORMS :=
# SUPPORTED_WIN_BUILD_PLATFORMS += WIN32_MSVC


# list of supported platforms, created using all array categories above.
SUPPORTED_BUILD_PLATFORMS :=
SUPPORTED_BUILD_PLATFORMS += $(foreach PLATFORM, $(SUPPORTED_LINUX_BUILD_PLATFORMS), $(PLATFORM))
SUPPORTED_BUILD_PLATFORMS += $(foreach PLATFORM, $(SUPPORTED_MSYS2_BUILD_PLATFORMS), $(PLATFORM))
SUPPORTED_BUILD_PLATFORMS += $(foreach PLATFORM, $(SUPPORTED_WIN_BUILD_PLATFORMS), $(PLATFORM))
# $(info supported build platforms:)
# $(info $(SUPPORTED_BUILD_PLATFORMS))


# detect
ifeq ($(OS),Windows_NT)
ifneq ($(filter $(MSYSTEM),MINGW64),)
	BUILD_PLATFORM := WIN32_MSYS2_MINGW64
else ifneq ($(filter $(MSYSTEM),UCRT64),)
	BUILD_PLATFORM := WIN32_MSYS2_UCRT64
else ifneq ($(filter $(MSYSTEM),CLANG64),)
	BUILD_PLATFORM := WIN32_MSYS2_CLANG64
else
	BUILD_PLATFORM := WIN32_MSVC
endif
else ifeq ($(OS),)
	OS=$(shell uname -s)
ifeq ($(OS),Linux)
# some supported distros do not have /etc/lsb-release, but still support lsb_release commands
# so, comment out the check, until we have something better and more generic.
# ifneq ("$(wildcard /etc/lsb-release)","")
DISTRO_NAME := $(shell lsb_release -si | tr '[:lower:]' '[:upper:]')
DISTRO_VERSION := $(subst .,,$(shell lsb_release -sr))
BUILD_PLATFORM := $(DISTRO_NAME)_$(DISTRO_VERSION)
# endif # if '/etc/lsb-release' exists
endif # ifeq ($(OS),Linux)
endif # ifeq ($(OS),Windows_NT)


# filter
ifneq ($(filter $(BUILD_PLATFORM),$(SUPPORTED_BUILD_PLATFORMS)),)
$(info using build platform: $(BUILD_PLATFORM))
else
$(error unsupported build platform: $(BUILD_PLATFORM))
$(info use one of: $(SUPPORTED_BUILD_PLATFORMS))
endif


# platform specific generic variables
ifeq ($(BUILD_PLATFORM),$(filter $(BUILD_PLATFORM),$(SUPPORTED_MSYS2_BUILD_PLATFORMS)))
export MSYSTEM_LC := $(shell echo $(MSYSTEM) | tr '[:upper:]' '[:lower:]')
endif  # $(BUILD_PLATFORM) logic
###############################################################################


###############################################################################
# MAKEFILE PATH
###############################################################################
# https://stackoverflow.com/questions/18136918/how-to-get-current-relative-directory-of-your-makefile

MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR_PATH := $(patsubst %/,%,$(dir $(MAKEFILE_PATH)))
MAKEFILE_DIR_NAME := $(notdir $(MAKEFILE_DIR_PATH))

# $(info )
# $(info MAKEFILE_LIST=$(MAKEFILE_LIST))
# $(info MAKEFILE_PATH=$(MAKEFILE_PATH))
# $(info MAKEFILE_DIR_PATH=$(MAKEFILE_DIR_PATH))
# $(info MAKEFILE_DIR_NAME=$(MAKEFILE_DIR_NAME))
# $(info )
###############################################################################


###############################################################################
# MAKEFILE PARALLEL JOBS DETECT (LINUX only!!)
###############################################################################
# https://stackoverflow.com/a/33616144 : get the jobs passed in to make...

JOBS := 0
ifeq ($(OS),Linux)
MAKE_PID := $(shell echo $$PPID)
JOB_FLAG := $(filter -j%, $(subst -j ,-j,$(shell ps T | grep "^\s*$(MAKE_PID).*$(MAKE)")))
JOBS     := $(subst -j,,$(JOB_FLAG))

# $(info )
# $(info MAKE_PID=$(MAKE_PID))
# $(info JOB_FLAG=$(JOB_FLAG))
# $(info JOBS=$(JOBS))
# $(info )
$(info using parallel jobs: $(JOBS))
endif
###############################################################################


###############################################################################
# DATETIME
###############################################################################
# underscores are better for using in filename/dirname
# DATE example: 01_JUL_2024
# TIME example: 21_36_45
# TZ example: +05:30 IST
DATE_FORMAT := "%d_%^b_%Y"
TIME_FORMAT := "%H_%M_%S"
TZ_FORMAT := "%:z %Z"
CURRENT_DATE := $(shell date +$(DATE_FORMAT))
CURRENT_TIME := $(shell date +$(TIME_FORMAT))
CURRENT_TZ := $(shell date +$(TZ_FORMAT))
$(info date time tz: $(CURRENT_DATE) $(CURRENT_TIME) $(CURRENT_TZ))

# DATE example: 01_JULY_2024
DATE_FORMAT_FULL := "%d_%^B_%Y"
CURRENT_DATE_FULL := $(shell date +$(DATE_FORMAT_FULL))
# $(info date full: $(CURRENT_DATE_FULL))
###############################################################################


PREFIX ?= $(MAKEFILE_DIR_PATH)/install
$(info PREFIX=$(PREFIX))

SHELL := /bin/bash

CMAKE_BUILD_DIR := $(MAKEFILE_DIR_PATH)/build
CMAKE_SOURCE_DIR := $(MAKEFILE_DIR_PATH)

# for CMake, select the generator based on the build platform
ifeq ($(BUILD_PLATFORM),$(filter $(BUILD_PLATFORM),$(SUPPORTED_MSYS2_BUILD_PLATFORMS)))
CMAKE_GENERATOR ?= "MSYS Makefiles"
else ifeq ($(BUILD_PLATFORM),$(filter $(BUILD_PLATFORM),$(SUPPORTED_WIN_BUILD_PLATFORMS)))
CMAKE_GENERATOR ?= "Ninja"
else ifeq ($(BUILD_PLATFORM),$(filter $(BUILD_PLATFORM),$(SUPPORTED_LINUX_BUILD_PLATFORMS)))
CMAKE_GENERATOR ?= "Unix Makefiles"
endif # $(BUILD_PLATFORM) logic


VERSION := $(shell git describe --tags --abbrev=0)
ifeq ($(VERSION),)
VERSION := v0.0.0
endif
$(info VERSION=$(VERSION))


GIT_REPO_URL := $(shell git remote get-url origin | sed -E 's,https\://api\:.+github,github,g')
ifeq ($(GIT_REPO_URL),)
GIT_REPO_URL := unknown_url
endif
$(info GIT_REPO_URL=$(GIT_REPO_URL))


GIT_HASH := $(shell git rev-parse HEAD)
ifeq ($(GIT_HASH),)
GIT_HASH := zyxwvutsrqponmlkjihgfedcba
endif
$(info GIT_HASH=$(GIT_HASH))


DEFAULT_BRANCH := main
CURRENT_BRANCH := $(shell git symbolic-ref HEAD --short 2>/dev/null || echo "no_branch")
$(info DEFAULT_BRANCH=$(CURRENT_BRANCH))
$(info CURRENT_BRANCH=$(CURRENT_BRANCH))


# Qt specific variables to choose a specific Qt installation.
QMAKE_PATH ?=
ifneq ($(QMAKE_PATH),)
QMAKE_DIR_PATH := $(patsubst %/,%,$(dir $(QMAKE_PATH)))
CMAKE_PREFIX_PATH := $(abspath $(QMAKE_DIR_PATH)/..)
export PATH := $(CMAKE_PREFIX_PATH)/bin:$(PATH)
$(info QMAKE_PATH=$(QMAKE_PATH))
$(info CMAKE_PREFIX_PATH=$(CMAKE_PREFIX_PATH))
$(info PATH=$(PATH))
endif


DEPLOY_PACKAGE_DIR_PATH := $(CMAKE_SOURCE_DIR)/deploy

# variables required for building delpoy package
ifeq ($(BUILD_PLATFORM),$(filter $(BUILD_PLATFORM),$(SUPPORTED_MSYS2_BUILD_PLATFORMS)))

export SEVENZIP_DIR_PATH := $(CMAKE_BUILD_DIR)/7zip
export SEVENZIP_R_FILE_PATH := $(SEVENZIP_DIR_PATH)/7zr.exe
export SEVENZIP_FILE_PATH := $(SEVENZIP_DIR_PATH)/7z.exe
# 7-Zip needs the SFX path to be in windows format, and it should have "\\" delimiters(treated as a string)
export SEVENZIP_SFX_FILE_PATH_W=$(shell cygpath -w "$(SEVENZIP_DIR_PATH)/7z.sfx" | sed 's/\\/\\\\/g')
export SEVENZIP_CONSFX_FILE_PATH_W=$(shell cygpath -w "$(SEVENZIP_DIR_PATH)/7zCon.sfx" | sed 's/\\/\\\\/g')

else ifeq ($(BUILD_PLATFORM),$(filter $(BUILD_PLATFORM),$(SUPPORTED_LINUX_BUILD_PLATFORMS)))

LINUXDEPLOYQT_APPIMAGE_FILE_PATH := $(CMAKE_BUILD_DIR)/linuxdeployqt-continuous-x86_64.AppImage
# we need to extract the AppImage and then run it, so that we use the same steps inside docker-based builder as well:
# ref: https://github.com/AppImage/AppImageKit/issues/405#issuecomment-305989062
# so, we will be using the AppRun file after extraction in the below location:
LINUXDEPLOYQT_APPRUN_FILE_PATH := $(CMAKE_BUILD_DIR)/linuxdeployqt/squashfs-root/AppRun

endif # $(BUILD_PLATFORM) logic


.DEFAULT_GOAL := all

.PHONY: all
all: install


.PHONY: install
install: run-cmake
	@rm -rf $(PREFIX)
#$(MAKE) pre-install
	$(MAKE) -C build install
#$(MAKE) post-install
	@echo
	@echo
	@echo "local install ready:" $(shell realpath --relative-to="$(CMAKE_SOURCE_DIR)" "$(PREFIX)")"/bin/quetzalcoatlus"


# phony target to force cmake run
.PHONY: run-cmake
run-cmake:
	cmake -G $(CMAKE_GENERATOR) \
		-DCMAKE_INSTALL_PREFIX=$(PREFIX) \
		-DCMAKE_PREFIX_PATH=$(CMAKE_PREFIX_PATH) \
		-DBUILD_DATE=$(CURRENT_DATE) \
		-DBUILD_VERSION=$(VERSION) \
		-DBUILD_GIT_HASH=$(GIT_HASH) \
		-DBUILD_GIT_REPO_URL=$(GIT_REPO_URL) \
		-S $(CMAKE_SOURCE_DIR) -B $(CMAKE_BUILD_DIR)


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

ifeq ($(BUILD_PLATFORM),$(filter $(BUILD_PLATFORM),$(SUPPORTED_MSYS2_BUILD_PLATFORMS)))

	@rm -rf $(DEPLOY_PACKAGE_DIR_PATH)
	@mkdir -p $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)
	@cd $(PREFIX) && find . | grep -f $(CMAKE_SOURCE_DIR)/deployment_filter.txt | xargs -d '\n' cp -r --parents -t $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)
	@echo "detect and copy all libs into lib/..."
	@mkdir -p $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/lib
	@for f in `ldd $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/bin/quetzalcoatlus.exe | cut -d' ' -f 3 | grep $(MSYSTEM_LC) | uniq`; do cp -fv $$f $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/lib; done
	@echo "move all libs into bin/..."
	@cp -fv $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/lib/* $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/bin
	@rm -rf $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/lib

# # for GTK3 applications, we need to copy additional files:
# # reference for additional dll files: https://github.com/microsoft/vcpkg/issues/9936
# 	@echo "copy libs/configs for gtk3..."
# 	@cp -v /$(MSYSTEM_LC)/bin/liblzma-5.dll $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/bin
# 	@cp -v /$(MSYSTEM_LC)/bin/librsvg-2-2.dll $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/bin
# 	@cp -v /$(MSYSTEM_LC)/bin/libxml2-2.dll $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/bin
# # reference for additional gdbus.exe: https://discourse.gnome.org/t/gtk-warning-about-gdbus-exe-not-being-found-on-windows-msys2/2893
# 	@cp -v /$(MSYSTEM_LC)/bin/gdbus.exe $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/bin
# # reference for gdk/glib/icons/mime: 
# # https://copyprogramming.com/howto/how-do-i-distribute-a-gtkmm-application-on-windows#problems-with-deploying-a-gtk-application
# # https://www.gtk.org/docs/installations/windows#building-and-distributing-your-application
# 	@mkdir -p $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/lib
# 	@cp -r /$(MSYSTEM_LC)/lib/gdk-pixbuf-2.0 $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/lib
# 	@mkdir -p $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/share/glib-2.0
# 	@cp -r /$(MSYSTEM_LC)/share/glib-2.0/schemas $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/share/glib-2.0
# 	@mkdir -p $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/share/icons
# 	@cp -r /$(MSYSTEM_LC)/share/icons/Adwaita $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/share/icons
# 	@cp -r /$(MSYSTEM_LC)/share/icons/hicolor $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/share/icons
# 	@mkdir -p $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/share/mime
# 	@cp -r /$(MSYSTEM_LC)/share/mime/magic $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/share/mime

	@echo "creating deployment package using windeployqt..."
	@windeployqt $(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION)/bin/quetzalcoatlus.exe --no-translations
	@echo "creating 7z archive using 7-Zip..."
ifeq ("$(wildcard $(SEVENZIP_FILE_PATH))","")
	@mkdir -p $(SEVENZIP_DIR_PATH)
	@wget --quiet https://www.7-zip.org/a/7zr.exe --directory-prefix=$(SEVENZIP_DIR_PATH)
	@wget --quiet https://www.7-zip.org/a/7z2301-x64.exe --directory-prefix=$(SEVENZIP_DIR_PATH)
	@cd $(SEVENZIP_DIR_PATH) && \
	$(SEVENZIP_DIR_PATH)/7zr.exe x $(SEVENZIP_DIR_PATH)/7z2301-x64.exe -y > /dev/null
endif # if $(SEVENZIP_FILE_PATH) does not exist
	@cd $(DEPLOY_PACKAGE_DIR_PATH) && \
	$(SEVENZIP_FILE_PATH) a -mx3 -t7z quetzalcoatlus_$(VERSION).7z quetzalcoatlus_$(VERSION) > /dev/null
	@echo "[msys2] deploy ready:" $(shell realpath -m --relative-to="$(CMAKE_SOURCE_DIR)" "$(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION).7z")

else ifeq ($(BUILD_PLATFORM),$(filter $(BUILD_PLATFORM),$(SUPPORTED_LINUX_BUILD_PLATFORMS)))

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
# we need to extract the AppImage and then run it, so that we use the same steps inside docker-based builder as well:
# ref: https://github.com/AppImage/AppImageKit/issues/405#issuecomment-305989062
	@mkdir -p $(CMAKE_BUILD_DIR)/linuxdeployqt
	@cd $(CMAKE_BUILD_DIR)/linuxdeployqt && $(LINUXDEPLOYQT_APPIMAGE_FILE_PATH) --appimage-extract
endif # if LINUXDEPLOYQT_APPIMAGE_FILE_PATH does not exist
	@cd $(DEPLOY_PACKAGE_DIR_PATH) && $(LINUXDEPLOYQT_APPRUN_FILE_PATH) $(DEPLOY_PACKAGE_DIR_PATH)/usr/share/applications/quetzalcoatlus.desktop -appimage -no-translations &> $(CMAKE_SOURCE_DIR)/linuxdeployqt.log

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
	@echo "[linux] deploy ready:" $(shell realpath -m --relative-to="$(CMAKE_SOURCE_DIR)" "$(DEPLOY_PACKAGE_DIR_PATH)/quetzalcoatlus_$(VERSION).tar.gz")

endif # $(BUILD_PLATFORM) logic


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


# extremely convenient target to print any Makefile variable for debugging
# https://stackoverflow.com/a/25817631/3379867
# use it as : 'make print-VARIABLE_NAME'
print-%  : ; @echo $* = $($*)

# examples:
# make distclean
#
# linux/win default Qt version...
# make deploy -j$(nproc) 2>&1 | tee build_default.log
#
# linux : specify Qt version and build with that...
# make deploy -j$(nproc) QMAKE_PATH=/usr/lib/x86_64-linux-gnu/qt5/bin/qmake 2>&1 | tee build_5_12_8.log
# make deploy -j$(nproc) QMAKE_PATH=/home/${USER}/qt/5.15.2/gcc_64/bin/qmake 2>&1 | tee build_5_15_2.log
# make deploy -j$(nproc) QMAKE_PATH=/home/${USER}/qt/6.5.3/gcc_64/bin/qmake 2>&1 | tee build_6_5_3.log
