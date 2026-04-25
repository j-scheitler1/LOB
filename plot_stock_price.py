import argparse
import html
import json
import time
from collections import deque
from pathlib import Path


def read_events(event_file, max_points):
    times = deque(maxlen=max_points)
    prices = deque(maxlen=max_points)

    if not event_file.exists():
        return times, prices

    with event_file.open("r", encoding="utf-8") as file:
        for line in file:
            line = line.strip()
            if not line:
                continue

            try:
                event = json.loads(line)
            except json.JSONDecodeError:
                continue

            times.append(event["event_time"])
            prices.append(event["stock_price"])

    return list(times), list(prices)


def scale(value, source_min, source_max, target_min, target_max):
    if source_max == source_min:
        return (target_min + target_max) / 2

    percent = (value - source_min) / (source_max - source_min)
    return target_min + percent * (target_max - target_min)


def build_polyline(times, prices, width, height, margin):
    min_time = min(times)
    max_time = max(times)
    min_price = min(prices)
    max_price = max(prices)

    points = []
    for event_time, price in zip(times, prices):
        x = scale(event_time, min_time, max_time, margin, width - margin)
        y = scale(price, min_price, max_price, height - margin, margin)
        points.append(f"{x:.2f},{y:.2f}")

    return " ".join(points), min_time, max_time, min_price, max_price


def write_chart(event_file, output_file, max_points):
    width = 1000
    height = 500
    margin = 60
    times, prices = read_events(event_file, max_points)

    title = "LOB Simulated Stock Price"

    if not times:
        svg = f"""<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">
  <rect width="100%" height="100%" fill="white"/>
  <text x="{width / 2}" y="{height / 2}" text-anchor="middle" font-family="Arial" font-size="22">Waiting for {html.escape(str(event_file))}</text>
</svg>
"""
        output_file.write_text(svg, encoding="utf-8")
        return

    polyline, min_time, max_time, min_price, max_price = build_polyline(
        times,
        prices,
        width,
        height,
        margin,
    )

    svg = f"""<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">
  <rect width="100%" height="100%" fill="white"/>
  <text x="{width / 2}" y="30" text-anchor="middle" font-family="Arial" font-size="24">{html.escape(title)}</text>

  <line x1="{margin}" y1="{height - margin}" x2="{width - margin}" y2="{height - margin}" stroke="#222" stroke-width="2"/>
  <line x1="{margin}" y1="{margin}" x2="{margin}" y2="{height - margin}" stroke="#222" stroke-width="2"/>

  <text x="{margin}" y="{height - 20}" text-anchor="middle" font-family="Arial" font-size="13">{min_time}</text>
  <text x="{width - margin}" y="{height - 20}" text-anchor="middle" font-family="Arial" font-size="13">{max_time}</text>
  <text x="{margin - 10}" y="{height - margin}" text-anchor="end" font-family="Arial" font-size="13">{min_price:.2f}</text>
  <text x="{margin - 10}" y="{margin}" text-anchor="end" font-family="Arial" font-size="13">{max_price:.2f}</text>

  <text x="{width / 2}" y="{height - 8}" text-anchor="middle" font-family="Arial" font-size="15">Event</text>
  <text x="18" y="{height / 2}" text-anchor="middle" font-family="Arial" font-size="15" transform="rotate(-90 18 {height / 2})">Price</text>

  <polyline points="{polyline}" fill="none" stroke="#2563eb" stroke-width="2"/>
</svg>
"""

    output_file.write_text(svg, encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description="Plot LOB stock price events as an SVG.")
    parser.add_argument("--input", default="simulation_events.jsonl")
    parser.add_argument("--output", default="stock_price.svg")
    parser.add_argument("--max-points", type=int, default=5000)
    parser.add_argument("--watch", action="store_true")
    parser.add_argument("--interval", type=float, default=2.0)
    args = parser.parse_args()

    event_file = Path(args.input)
    output_file = Path(args.output)

    while True:
        write_chart(event_file, output_file, args.max_points)
        print(f"Wrote {output_file}")

        if not args.watch:
            break

        time.sleep(args.interval)


if __name__ == "__main__":
    main()
