#include <algorithm>
#include <iostream>
#include <chrono>
#include <deque>
#include <fstream>
#include <iomanip>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "book.h"
#include "limit.h"
#include "order.h"
#include "trader.h"
#include "random.h"

static double get_stock_price(Order &simulation_order, Limit *best_bid, Limit *best_ask) {
    if (best_bid != nullptr && best_ask != nullptr) {
        return (best_bid->get_limit_price() + best_ask->get_limit_price()) / 2.0;
    }
    if (best_bid != nullptr) {
        return best_bid->get_limit_price();
    }
    if (best_ask != nullptr) {
        return best_ask->get_limit_price();
    }

    return simulation_order.get_limit_price();
}

static void write_simulation_json(
    std::ostream &json_file,
    int event_time,
    Trader &selected_trader,
    Order &simulation_order,
    Book &book,
    bool pretty
) {
    Limit *best_bid = book.get_best_bid();
    Limit *best_ask = book.get_best_ask();
    double stock_price = get_stock_price(simulation_order, best_bid, best_ask);
    const char *newline = pretty ? "\n" : "";
    const char *indent_one = pretty ? "  " : "";
    const char *indent_two = pretty ? "    " : "";

    json_file << std::fixed << std::setprecision(2);
    json_file << "{" << newline;
    json_file << indent_one << "\"event_time\": " << event_time << "," << newline;
    json_file << indent_one << "\"stock_price\": " << stock_price << "," << newline;
    json_file << indent_one << "\"selected_trader\": {" << newline;
    json_file << indent_two << "\"id\": " << selected_trader.get_id() << "," << newline;
    json_file << indent_two << "\"positive_outlook\": " << selected_trader.get_positive_outlook() << "," << newline;
    json_file << indent_two << "\"buyer\": " << (selected_trader.is_buyer() ? "true" : "false") << newline;
    json_file << indent_one << "}," << newline;
    json_file << indent_one << "\"order\": {" << newline;
    json_file << indent_two << "\"id\": " << simulation_order.get_id() << "," << newline;
    json_file << indent_two << "\"side\": \"" << (simulation_order.get_type() ? "buy" : "sell") << "\"," << newline;
    json_file << indent_two << "\"remaining_shares\": " << simulation_order.get_shares() << "," << newline;
    json_file << indent_two << "\"limit_price\": " << simulation_order.get_limit_price() << newline;
    json_file << indent_one << "}," << newline;
    json_file << indent_one << "\"book\": {" << newline;
    json_file << indent_two << "\"best_bid\": ";
    if (best_bid == nullptr) {
        json_file << "null," << newline;
    }
    else {
        json_file << best_bid->get_limit_price() << "," << newline;
    }
    json_file << indent_two << "\"best_ask\": ";
    if (best_ask == nullptr) {
        json_file << "null" << newline;
    }
    else {
        json_file << best_ask->get_limit_price() << newline;
    }
    json_file << indent_one << "}" << newline;
    json_file << "}" << newline;
}

int main(int argc, char *argv[])
{
    generator.seed(std::random_device{}());

    int max_orders = 0;
    if (argc > 1) {
        max_orders = std::stoi(argv[1]);
    }

    std::vector<Trader> traders;
    traders.reserve(TRADERS);
    for (int i = 0; i < TRADERS; i++) {
        traders.emplace_back(i);
    }

    Book simulation_book;
    std::deque<Order> simulation_orders;
    int next_order_id = 1000;
    int base_price = 100;
    int event_time = 0;
    const int orders_per_batch = 250;
    std::ofstream history_file("simulation_events.jsonl", std::ios::trunc);

    while (max_orders <= 0 || event_time < max_orders) {
        int orders_this_batch = orders_per_batch;
        if (max_orders > 0) {
            orders_this_batch = std::min(orders_per_batch, max_orders - event_time);
        }

        for (int i = 0; i < orders_this_batch; i++) {
            int selected_trader_index = get_trader_selection();
            Trader &selected_trader = traders[selected_trader_index];

            simulation_orders.push_back(selected_trader.create_order(
                next_order_id,
                base_price,
                event_time
            ));
            Order &simulation_order = simulation_orders.back();
            simulation_book.add_order(simulation_order);

            write_simulation_json(
                history_file,
                event_time,
                selected_trader,
                simulation_order,
                simulation_book,
                false
            );
            history_file << "\n";

            next_order_id++;
            event_time++;
        }

        history_file.flush();
        if (max_orders > 0) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
}
