CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = main
SRCS = src/main.cpp src/book.cpp
HEADERS = src/order.h src/limit.h src/book.h src/random.h src/trader.h
PLOT = plot_stock_price.py
EVENTS = simulation_events.jsonl
CHART = stock_price.svg

.PHONY: all sim plot clean 2008

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

sim: $(TARGET)
	./$(TARGET) 250

plot:
	python3 $(PLOT) --input $(EVENTS) --output $(CHART)

clean:
	rm -rf $(TARGET) $(CHART) build __pycache__

2008:
	: > $(EVENTS)
