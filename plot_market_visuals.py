import json


INPUT_FILE = "simulation_events.jsonl"
OUTPUT_FILE = "order_book_data.js"
MAX_LEVELS = 12


def read_events():
    events = []

    try:
        with open(INPUT_FILE, "r") as file:
            for line in file:
                line = line.strip()
                if line != "":
                    events.append(json.loads(line))
    except FileNotFoundError:
        pass

    return events


def add_level(levels, price, shares):
    price = float(price)

    if price not in levels:
        levels[price] = 0

    levels[price] += float(shares)


def clean_book(bids, asks, best_bid, best_ask):
    if best_bid is None:
        bids.clear()
    else:
        for price in list(bids.keys()):
            if price > best_bid:
                del bids[price]

    if best_ask is None:
        asks.clear()
    else:
        for price in list(asks.keys()):
            if price < best_ask:
                del asks[price]


def get_top_levels(levels, reverse_order):
    rows = []

    for price, volume in sorted(levels.items(), reverse=reverse_order):
        if volume > 0:
            rows.append({
                "price": price,
                "volume": volume
            })

        if len(rows) == MAX_LEVELS:
            break

    return rows


def make_snapshots(events):
    bids = {}
    asks = {}
    snapshots = []

    for i, event in enumerate(events):
        order = event.get("order", {})
        book = event.get("book", {})
        trader = event.get("selected_trader", {})

        side = order.get("side")
        price = order.get("limit_price")
        shares = order.get("remaining_shares")

        if price is not None and shares is not None and shares > 0:
            if side == "buy":
                add_level(bids, price, shares)
            elif side == "sell":
                add_level(asks, price, shares)

        best_bid = book.get("best_bid")
        best_ask = book.get("best_ask")

        if best_bid is not None:
            best_bid = float(best_bid)

        if best_ask is not None:
            best_ask = float(best_ask)

        clean_book(bids, asks, best_bid, best_ask)

        snapshots.append({
            "event_time": event.get("event_time", i),
            "stock_price": event.get("stock_price"),
            "best_bid": best_bid,
            "best_ask": best_ask,
            "trader_id": trader.get("id"),
            "side": side,
            "limit_price": price,
            "bids": get_top_levels(bids, True),
            "asks": get_top_levels(asks, False)
        })

    return snapshots


def write_data(snapshots):
    data = "window.snapshots = " + json.dumps(snapshots) + ";\n"

    with open(OUTPUT_FILE, "w") as file:
        file.write(data)


events = read_events()
snapshots = make_snapshots(events)
write_data(snapshots)
