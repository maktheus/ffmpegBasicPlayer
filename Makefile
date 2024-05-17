# Nome do executável
TARGET = mp4player

# Diretórios
SRC_DIR = src
OBJ_DIR = obj

# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I$(SRC_DIR) `pkg-config --cflags libavformat libavcodec libavutil libswscale libswresample`
LDFLAGS = `pkg-config --libs libavformat libavcodec libavutil libswscale libswresample`

# Arquivos fonte e objetos
SRCS = main.cpp $(SRC_DIR)/MP4Player.cpp
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/MP4Player.o

# Regras de construção
all: $(TARGET)

# Regra para criar o diretório de objetos se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Regra para compilar o executável
$(TARGET): $(OBJ_DIR) $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

# Regras para compilar os arquivos objeto
$(OBJ_DIR)/main.o: main.cpp $(SRC_DIR)/MP4Player.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/MP4Player.o: $(SRC_DIR)/MP4Player.cpp $(SRC_DIR)/MP4Player.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para limpar os arquivos compilados
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
