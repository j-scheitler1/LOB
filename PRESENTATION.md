# Limit Order Book + Market Simulation

## What is a Limit Order?
A limit order is an order to buy or sell a security at a certain price. (a single stock in this case)

## What is a Limit Order Book?
A limit order book is a dynamic system that records and organizes all outstanding limit orders

### Structure of the LOB
The book has two sides to it
- bids, which are the buy orders
- asks, which are the sell orders
Each side in this project is organized by price with a map `key: price, value: Limit *`

Inside each Limit, the orders are stored as a FIFO doubly linked list:
- `head order` is the oldest order at that price
- `tail order` is the newest order at that price

### Matching Logic
When a buy order arrives, it checks the lowest sell price (best ask).
If the buy price is greater than or equal to that ask, order executes.


The same happens on the sell side, but checking for the highest buy price

The traded amount is the smaller of the incoming order's remaining shares and the resting order's remaining shares, so partial fills are supported.


If an order is fully filled, it is removed. If the incoming order still has shares left, then it is added to its corresponding side of the book.

## The Simulation
The simulation creates 15 'traders' that are randomly selected to place an order. The simulation quits once 250 orders have been placed

The traders each have their own outlook on the stock based off of a uniform distribution. This turns them into either a buyer or a seller.

When a trader is selected, they create an order with a random amount of shares and a price offset from the base price of the stock
