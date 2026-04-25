=== Assignment 1.10 - LOB Book + Simulation ===

Run from the project root:

1. `make clean` removes old build/chart files.
2. `make` builds the C++ simulation.
3. `make sim` processes 250 generated orders and writes `simulation_events.jsonl`.
4. `make plot` creates `stock_price.svg` from those events.
5. Open `stock_price.svg` to view the simulated price movement.

What you are seeing: a map-backed limit order book with bid/ask sides. Each price level stores orders in FIFO order, and matching happens when incoming buy/sell prices cross. Use `make 2008` to clear the event file.
