CXX = g++
CXXFLAGS = -std=c++11

SRC_DIR = ./src
BUILD_DIR = build
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

BINARIES = $(BIN_DIR)/get_net_info $(BIN_DIR)/faffetch

all: $(BINARIES)

$(BIN_DIR)/get_net_info: $(BUILD_DIR)/get_net_info.o | $(BIN_DIR)
	$(CXX) $^ -o $@ -lcurl

$(BIN_DIR)/faffetch: $(BUILD_DIR)/main.o $(BUILD_DIR)/helpers.o | $(BIN_DIR)
	$(CXX) $^ -pthread -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp  | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
