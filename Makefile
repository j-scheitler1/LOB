CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = main
SRCS = src/main.cpp src/book.cpp
HEADERS = src/order.h src/limit.h src/book.h src/random.h src/trader.h
PLOT = plot_market_visuals.py

.PHONY: all visuals package-replay clean remove

all: $(TARGET)
	./$(TARGET) 250

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

visuals:
	python3 $(PLOT)

package-replay:
	./package_replay.sh

clean:
	rm -rf $(TARGET) order_book_data.js order_book_replay_bundle.tar.gz build __pycache__

remove:
	: > simulation_events.jsonl
