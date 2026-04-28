# Assignment 1.10 - Limit Order Book Simulation

This project simulates a simple limit order book and a dummy market. Orders are stored by price on bid and ask sides, and orders match when buy and sell prices cross.

## Run the Simulation

1. `make clean`
2. `make remove`
3. `make`
4. `make visuals`

`make` builds and runs a 250-order simulation, writing `simulation_events.jsonl`.

`make visuals` updates `order_book_data.js` for the replay page.

Open `order_book_replay.html` to view the replay. The page shows each order event over time, including the stock price, best bid, best ask, last submitted order, and the current bid/ask book levels.

## Note for the TA

I noticed you cannot open the `.html` file directly in browser from Pyrite, so you can run the script that I have added to package the files needed and open them locally.

After completing the four steps above:

1. Run `make package-replay` to create a `tar.gz` file
2. Copy the tarball to your local computer:
   
   `scp username@pyrite.cs.iastate.edu:/path/to/LOB/order_book_replay_bundle.tar.gz .`

3. Extract the tarball.
   Mac, Linux, or Windows PowerShell:
   
   `tar -xzf order_book_replay_bundle.tar.gz`

4. Open `order_book_replay.html` in a browser.
   Mac Or Linux:
   
   `open order_book_replay.html`

   Windows PowerShell:
   
   `start order_book_replay.html`
