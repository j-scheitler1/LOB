#ifndef BOOK_H
#define BOOK_H

#include "limit.h"

#include <map>

class Book {
    private:
        Limit *lowest_sell;
        Limit *highest_buy;
        std::map<int, Order*> orders_by_id;
        std::map<int, Limit*> buy_limits_by_price;
        std::map<int, Limit*> sell_limits_by_price;

        void update_highest_buy_from_map() {
            highest_buy = buy_limits_by_price.empty()
                ? nullptr
                : buy_limits_by_price.rbegin()->second;
        }

        void update_lowest_sell_from_map() {
            lowest_sell = sell_limits_by_price.empty()
                ? nullptr
                : sell_limits_by_price.begin()->second;
        }

        void delete_limits(std::map<int, Limit*> &limits_by_price) {
            for (auto &entry : limits_by_price) {
                delete entry.second;
            }

            limits_by_price.clear();
        }
        
    public:
        Book() {
            lowest_sell = nullptr;
            highest_buy = nullptr;
        }
        ~Book() {
            delete_limits(buy_limits_by_price);
            delete_limits(sell_limits_by_price);
            lowest_sell = nullptr;
            highest_buy = nullptr;
        }

        void add_order(Order &order);
        void add_buy_limit(Limit *limit);
        void add_sell_limit(Limit *limit);
        bool cancel_order(int order_id);

        void add_order_to_map(Order &order) {
            int order_id = order.get_id();
            orders_by_id[order_id] = &order;
        }

        void remove_order_from_map(int order_id) {
            orders_by_id.erase(order_id);
        }

        void add_buy_limit_by_price(Limit *limit) {
            int limit_price = limit->get_limit_price();
            buy_limits_by_price[limit_price] = limit;
            update_highest_buy_from_map();
        }
        void remove_buy_limit(int limit_price) {
            auto it = buy_limits_by_price.find(limit_price);
            if (it == buy_limits_by_price.end()) {
                return;
            }

            Limit *limit = it->second;
            buy_limits_by_price.erase(limit_price);
            update_highest_buy_from_map();
            delete limit;
        }

        void add_sell_limit_by_price(Limit *limit) {
            int limit_price = limit->get_limit_price(); 
            sell_limits_by_price[limit_price] = limit;
            update_lowest_sell_from_map();
        }
        void remove_sell_limit(int limit_price) {
            auto it = sell_limits_by_price.find(limit_price);
            if (it == sell_limits_by_price.end()) {
                return;
            }

            Limit *limit = it->second;
            sell_limits_by_price.erase(limit_price);
            update_lowest_sell_from_map();
            delete limit;
        }

        Limit *get_highest_buy_limit() {
            return highest_buy;
        }

        Limit *get_lowest_sell_limit() {
            return lowest_sell;
        }

        Limit *get_best_bid() {
            return highest_buy;
        }

        Limit *get_best_ask() {
            return lowest_sell;
        }

        int get_volume_at_limit(int limit_price, bool buy_or_sell) {
            Limit *limit = buy_or_sell
                ? get_buy_limit_by_price(limit_price)
                : get_sell_limit_by_price(limit_price);

            return limit == nullptr ? 0 : limit->get_total_volume();
        }

        Order *get_order_by_id(int order_id) {
            auto it = orders_by_id.find(order_id);
            if (it != orders_by_id.end()) {
                return it->second;
            }
            return nullptr;
        }
        Limit *get_buy_limit_by_price(int limit_price) {
            auto it = buy_limits_by_price.find(limit_price);
            if (it != buy_limits_by_price.end()) {
                return it->second;
            }
            return nullptr;
        }
        Limit *get_sell_limit_by_price(int limit_price) {
            auto it = sell_limits_by_price.find(limit_price);
            if (it != sell_limits_by_price.end()) {
                return it->second;
            }
            return nullptr;
        }
};
#endif
