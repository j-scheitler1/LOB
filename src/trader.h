#ifndef TRADER_H
#define TRADER_H

#include "order.h"
#include "random.h"

class Trader {
    private:
        int id;
        int positive_outlook;
        bool buyer;

        bool is_buyer(int outlook) {
            return outlook >= 50;
        }

    public:
        Trader(int trader_id) {
            id = trader_id;
            positive_outlook = get_trader_outlook();
            buyer = is_buyer(positive_outlook);
        }

        int get_id() {
            return id;
        }

        int get_positive_outlook() {
            return positive_outlook;
        }

        bool is_buyer() {
            return buyer;
        }

        Order create_order(int order_id, int base_price, int event_time) {
            int shares = get_order_shares();
            int price_offset = get_order_price_offset();
            int limit_price = base_price + price_offset;

            return Order(order_id, buyer, shares, limit_price, event_time, event_time);
        }
};

#endif
