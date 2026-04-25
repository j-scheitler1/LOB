#ifndef ORDER_H
#define ORDER_H

#include <iostream>
#include <string>

class Limit; // declare limit to let compiler know about limit in parent_limit


// Role of `Order`:
// - stores the order's identity and remaining quantity
// - links to older/newer orders at the same price
// - links back to the `Limit` that owns it

class Order {
    private:
        int id_number;
        bool buy_or_sell; // buy = true, sell = false
        int shares;
        int limit; // price
        int entry_time;
        int event_time;
        Order *next_order;
        Order *prev_order;
        Limit *parent_limit;
    
    public:
        Order(int id, bool type, int s, int l, int entry, int event) { // constructor
            id_number = id;
            buy_or_sell = type;
            shares = s;
            limit = l;
            entry_time = entry;
            event_time = event;
            next_order = nullptr;
            prev_order = nullptr;
            parent_limit = nullptr;
        }
        ~Order() { // destructor

        }; 

        void set_parent_limit(Limit *limit) {
            parent_limit = limit;
        }

        void set_previous_order(Order *p_order) {
            prev_order = p_order;
        }

        void set_next_order(Order *n_order) {
            next_order = n_order;
        }

        void update_shares(int s, char op) {
            if (op == '+') {
                shares += s;
            } 
            else if (op == '-') {
                shares -= s;
            }
        }

        Limit *get_parent_limit() {
            return parent_limit;
        }

        Order *get_next_order() {
            return next_order;
        }

        Order *get_prev_order() {
            return prev_order;
        }

        int get_id() {
            return id_number;
        }

        bool get_type() {
            return buy_or_sell;
        }

        int get_shares() {
            return shares;
        }

        int get_limit_price() {
            return limit;
        }
};

#endif
