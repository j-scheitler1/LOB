# Limit Order Book Implementation Guide

## 1. The Model To Follow

This guide is based on the archived post [How to Build a Fast Limit Order Book](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/). It is a good mental model because it separates the two priorities a real book has to enforce:

- price priority across different prices
- time priority within the same price

The article's core design is:

- one tree for buy limits
- one tree for sell limits
- one `Limit` node per active price
- one doubly linked FIFO order queue inside each `Limit`
- one direct lookup table for orders by id
- one direct lookup table for limits by price
- one pointer to the best bid and one pointer to the best ask

That gives you a "real" book structure instead of a temporary classroom shortcut.

## 2. How The Data Structures Fit Together

Your current headers already match the article surprisingly well.

### `Order`

An `Order` is one resting order in the book. In your code it already has the right fields:

- `id_number`
- `buy_or_sell`
- `shares`
- `limit`
- `entry_time`
- `event_time`
- `next_order`
- `prev_order`
- `parent_limit`

Role of `Order`:

- stores the order's identity and remaining quantity
- links to older/newer orders at the same price
- links back to the `Limit` that owns it

### `Limit`

A `Limit` is one active price level. Your `Limit` fields also match the article:

- `limit_price`
- `size`
- `total_volume`
- `parent`
- `left_child`
- `right_child`
- `head_order`
- `tail_order`

Role of `Limit`:

- acts as a tree node sorted by price
- owns the FIFO queue of orders at that price
- stores aggregated data so queries do not scan every order

Recommended meaning of the aggregated fields:

- `size` = number of resting orders at this price
- `total_volume` = sum of remaining shares at this price

### `Book`

Your `Book` already has the essential tree roots and inside pointers:

- `buy_tree`
- `sell_tree`
- `lowest_sell`
- `highest_buy`

To finish the article's design, `Book` still needs fast lookup maps:

```cpp
std::unordered_map<int, Order*> orders_by_id;
std::unordered_map<int, Limit*> buy_limits_by_price;
std::unordered_map<int, Limit*> sell_limits_by_price;
```

Using separate limit maps per side is simpler than keying only by price, because the same price can exist on both sides.

## 3. Core Invariants

If these stay true, the book stays correct.

1. The buy tree is ordered by price, and `highest_buy` points to the highest active buy price.
2. The sell tree is ordered by price, and `lowest_sell` points to the lowest active sell price.
3. Each `Limit` contains a doubly linked FIFO queue from `head_order` to `tail_order`.
4. Every resting order appears exactly once in `orders_by_id`.
5. Every active price level appears in exactly one tree and exactly one side-specific price map.
6. `total_volume` always equals the sum of remaining shares in that limit's queue.
7. When a limit becomes empty, it must be removed from the queue structure, the tree, the price map, and the inside pointer if needed.

## 4. The Three Book Operations From The Article

The archived post focuses on three book maintenance operations: `add`, `cancel`, and `execute`. That is still the right way to think about the internals even if your outer API is "submit a new limit order."

### Add

Use `add` when an order is going to rest in the book.

Algorithm:

1. Choose the correct side: buy tree or sell tree.
2. Look up the price in that side's limit map.
3. If the limit does not exist:
   - create a new `Limit`
   - insert it into the correct tree
   - add it to the side's price map
   - update `highest_buy` or `lowest_sell` if this price becomes the new inside market
4. Append the order to the limit's `tail_order`.
5. Set the order's `parent_limit`, `prev_order`, and `next_order`.
6. Increment `size` and `total_volume`.
7. Insert the order into `orders_by_id`.

Performance target:

- first order at a new price: tree insertion cost
- later orders at the same price: O(1)

### Cancel

Use `cancel` when an existing resting order is removed by id.

Algorithm:

1. Find the order in `orders_by_id`.
2. Read its `parent_limit`.
3. Unlink the order from the doubly linked queue in O(1).
4. Decrease the limit's `size` and `total_volume`.
5. Erase the order from `orders_by_id`.
6. If the limit is now empty:
   - remove the limit from its tree
   - erase it from the side's price map
   - repair `highest_buy` or `lowest_sell`

The queue unlink should be constant time because the order already knows its neighbors and parent limit.

### Execute

Use `execute` when shares are removed from the oldest order at the inside price.

Algorithm:

1. Start from `highest_buy` for incoming sells or `lowest_sell` for incoming buys.
2. Take the `head_order` at that limit because FIFO must be preserved.
3. Reduce shares by the traded quantity.
4. Decrease the parent limit's `total_volume`.
5. If the resting order reaches zero shares:
   - unlink it from the queue
   - erase it from `orders_by_id`
   - decrease `size`
6. If that limit becomes empty, remove the limit from the tree and repair the inside pointer.

The queue work is O(1). The expensive part only happens when a whole price level disappears.

## 5. How To Adapt The Article To Your Matching Engine

The article describes the inner book structure. Your project likely needs one level above that: a matching loop for new limit orders.

Think of `submit_limit_order()` as:

1. While the incoming order still has shares left:
   - look at the opposite side's inside price
   - stop if there is no opposite liquidity
   - stop if the prices do not cross
   - otherwise execute against the opposite side's `head_order`
2. If the incoming order still has remaining shares after the crossing loop, call the `add` logic and let it rest in the book.

For price crossing:

- incoming buy matches while `incoming.limit >= lowest_sell->limit_price`
- incoming sell matches while `incoming.limit <= highest_buy->limit_price`

That gives you a real exchange-style limit order flow while still using the article's `add/cancel/execute` internals.

## 6. Honest Complexity Targets

These are the performance goals that matter for this design:

- `add` at an existing price: O(1)
- `add` at a new price: O(log M) if you use a balanced tree
- `cancel` after id lookup: O(1), except when an empty limit must also be removed from the tree
- `execute` at the inside queue head: O(1), except when an empty limit must also be removed from the tree
- `get_best_bid()` / `get_best_ask()`: O(1) reads from `highest_buy` / `lowest_sell`
- `get_volume_at_limit()`: O(1) from the side-specific price map

`M` is the number of active price levels, not the number of orders. That distinction is why the design scales well.

## 7. What Your Current Code Still Needs

Your current headers contain the right skeleton, but they are missing the behavior that makes the structure usable.

### `src/order.h`

Add methods or accessors for:

- reading and updating remaining shares
- reading the order side and price
- setting and clearing `next_order`, `prev_order`, and `parent_limit`

### `src/limit.h`

Add methods for:

- `append_order(Order* order)`
- `remove_order(Order* order)`
- `head()` / `tail()`
- `empty()`
- updating `size` and `total_volume`

This is the natural home for FIFO queue maintenance.

### `src/book.h`

Add:

- the order id map
- the side-specific price maps
- tree insertion and removal helpers
- inside-pointer maintenance
- matching functions such as `submit_limit_order`, `cancel_order`, `get_best_bid`, `get_best_ask`, and `get_volume_at_limit`

## 8. Practical Build Order For This Repo

If the goal is "understand it and implement a real version," this is the cleanest order:

1. Make `Limit` correctly append and remove `Order` nodes from a FIFO doubly linked list.
2. Add the `orders_by_id` and side-specific limit maps to `Book`.
3. Implement tree insertion for new price levels.
4. Implement `add_resting_order()` for orders that do not immediately match.
5. Implement `cancel_order(id)`.
6. Implement the matching loop for an incoming limit order.
7. Implement `get_best_bid`, `get_best_ask`, and `get_volume_at_limit`.
8. Add deterministic tests for empty book, same-price FIFO, partial fills, full fills, multi-price sweeps, and cancel-after-fill behavior.

## 9. A Good Definition Of "Real Version" For This Project

For this codebase, a "real version" does not mean exchange-grade networking or concurrency. It means:

- separate bid and ask sides
- strict price-time priority
- O(1) queue operations at a price level
- direct cancellation by id
- aggregated volume at each limit
- explicit inside-market tracking
- correct cleanup when orders or limits become empty

If you build that, you will have implemented the article's core idea rather than a toy vector scan.

## 10. Final Guidance

- Keep prices as integers in ticks or cents.
- Keep the first version single-instrument and single-threaded.
- Focus on invariants first, then performance.
- Do not hide the design behind `std::vector` scans if the goal is to understand how a real book works.
- If you later want to simplify the code, you can replace the manual tree with a standard container, but building it once by hand is the best way to learn the article's model.
