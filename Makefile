PRIV_DIR = $(MIX_APP_PATH)/priv
ELIXIR_NCNN_SO = $(PRIV_DIR)/elixir_ncnn.so

# ncnn
NCNN_VERSION = 20211208
NCNN_CACHE_DIR = $(shell pwd)/3rd_party/cache
NCNN_FULL_SOURCE_URL = https://github.com/Tencent/ncnn/releases/download/20211208/ncnn-20211208-full-source.zip
NCNN_FULL_SOURCE_ZIP = $(NCNN_CACHE_DIR)/ncnn-$(NCNN_VERSION).zip
NCNN_ROOT_DIR = $(shell pwd)/3rd_party/ncnn-$(NCNN_VERSION)
CMAKE_NCNN_BUILD_DIR = $(MIX_APP_PATH)/cmake_ncnn
NCNN_TOOLCHAIN_FILE = $(shell sh ./guess_best_ncnn_toolchain.sh)
LIB_NCNN_A = $(PRIV_DIR)/lib/libncnn.a

C_SRC = $(shell pwd)/c_src
LIB_SRC = $(shell pwd)/lib
# this .cmake file makes this project compatible with nerves and it should have no effect on normal build
TOOLCHAIN_FILE ?= $(shell pwd)/nerves/toolchain.cmake
CMAKE_ELIXIR_NCNN_BUILD_DIR = $(MIX_APP_PATH)/cmake_elixir_ncnn
MAKE_BUILD_FLAGS ?= "-j1"

.DEFAULT_GLOBAL := build

build: $(ELIXIR_NCNN_SO)

# in simple words
# 1. download "$(NCNN_FULL_SOURCE_URL)" to "3rd_party/cache/ncnn-$(OPENCV_VER).zip"
# 2. unzip -o "3rd_party/cache/opencv_$(OPENCV_VER).zip" -d "OPENCV_ROOT_DIR"
#    3rd_party
#    ├── cache
#    │   └── ncnn-$(NCNN_VERSION).zip
#    └── ncnn
#        └── ncnn-$(NCNN_VERSION)

$(NCNN_CACHE_DIR):
	@ mkdir -p "$(NCNN_CACHE_DIR)"

$(NCNN_FULL_SOURCE_ZIP): $(NCNN_CACHE_DIR)
	@ if [ ! -e "$(NCNN_FULL_SOURCE_ZIP)" ]; then \
		if [ -e "$(shell which curl)" ]; then \
			curl -fSL "$(NCNN_FULL_SOURCE_URL)" -o $(NCNN_FULL_SOURCE_ZIP) ; \
		elif [ -e "$(shell which wget)" ]; then \
			wget "$(NCNN_FULL_SOURCE_URL)" -O $(NCNN_FULL_SOURCE_ZIP) ; \
		else \
			echo "cannot find curl or wget, cannot download ncnn source code" ; \
			exit 1 ; \
		fi \
	fi

$(NCNN_ROOT_DIR): $(NCNN_FULL_SOURCE_ZIP)
	@ if [ ! -d "$(NCNN_ROOT_DIR)" ]; then \
    	rm -rf "$(NCNN_ROOT_DIR)" ; \
    	unzip -qq -o "$(NCNN_FULL_SOURCE_ZIP)" -d "$(NCNN_ROOT_DIR)" ; \
    fi

build_ncnn: $(NCNN_ROOT_DIR)
	@ if [ ! -e "$(LIB_NCNN_A)" ]; then \
		mkdir -p $(CMAKE_NCNN_BUILD_DIR) && \
		cd $(CMAKE_NCNN_BUILD_DIR) && \
		if [ ! -z "${NCNN_TOOLCHAIN_FILE}" ]; then \
			cmake -S $(NCNN_ROOT_DIR) \
				-D CMAKE_BUILD_TYPE=RELEASE \
				-D CMAKE_INSTALL_PREFIX=$(PRIV_DIR) \
				-D NCNN_ARM82=OFF \
				-D NCNN_ARM82DOT=OFF \
				-D NCNN_SHARED_LIB=ON \
				-D CMAKE_CXX_FLAGS="-fPIC" \
				-D NCNN_BUILD_TOOLS=OFF \
				-D NCNN_BUILD_EXAMPLES=OFF \
				-D NCNN_BUILD_TESTS=OFF \
				-D CMAKE_TOOLCHAIN_FILE=$(NCNN_ROOT_DIR)/toolchains/$(NCNN_TOOLCHAIN_FILE) ; \
		else \
	  		cmake -S $(NCNN_ROOT_DIR) \
				-D CMAKE_BUILD_TYPE=RELEASE \
				-D CMAKE_INSTALL_PREFIX=$(PRIV_DIR) \
				-D NCNN_ARM82=OFF \
				-D NCNN_ARM82DOT=OFF \
				-D NCNN_SHARED_LIB=ON \
				-D CMAKE_CXX_FLAGS="-fPIC" \
				-D NCNN_BUILD_TOOLS=OFF \
				-D NCNN_BUILD_EXAMPLES=OFF \
				-D NCNN_BUILD_TESTS=OFF \
				-D CMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) ; \
		fi && \
		make "$(MAKE_BUILD_FLAGS)" && \
		make install ; \
	fi

$(ELIXIR_NCNN_SO): build_ncnn
	@ mkdir -p $(PRIV_DIR)
	@ mkdir -p $(CMAKE_ELIXIR_NCNN_BUILD_DIR)
	@ cd "$(CMAKE_ELIXIR_NCNN_BUILD_DIR)" && \
		cmake -D C_SRC="$(C_SRC)" \
		-D CMAKE_TOOLCHAIN_FILE="$(TOOLCHAIN_FILE)" \
		-D PRIV_DIR="$(PRIV_DIR)" \
		-D ERTS_INCLUDE_DIR="$(ERTS_INCLUDE_DIR)" \
		-S "$(shell pwd)" && \
		make "$(MAKE_BUILD_FLAGS)"
	@ cp "$(CMAKE_ELIXIR_NCNN_BUILD_DIR)/elixir_ncnn.so" "$(ELIXIR_NCNN_SO)"
