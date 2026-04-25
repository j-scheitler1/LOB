#ifndef RANDOM_H
#define RANDOM_H

#include <iostream>
#include <random>

#define TRADERS 15

inline std::default_random_engine generator;
inline std::uniform_int_distribution<int> trader_selection_distribution(0, TRADERS - 1);
inline std::uniform_int_distribution<int> trader_outlook_distribution(0, 100);
inline std::uniform_int_distribution<int> order_shares_distribution(1, 100);
inline std::uniform_int_distribution<int> order_price_offset_distribution(-5, 5);

inline int get_trader_selection() {
    return trader_selection_distribution(generator);
}

inline int get_trader_outlook() {
    return trader_outlook_distribution(generator);
}

inline int get_order_shares() {
    return order_shares_distribution(generator);
}

inline int get_order_price_offset() {
    return order_price_offset_distribution(generator);
}

#endif
