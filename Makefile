
TARGET := regex

all: $(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): main.cpp
	g++ main.cpp -o $@ -lboost_regex

