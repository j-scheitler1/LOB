#ifndef LIMIT_H
#define LIMIT_H

#include <iostream>

#include "order.h"

// Role of `Limit`:
// - owns the FIFO queue of orders at that price
// - stores aggregated data so queries do not scan every order

class Limit {
    private:
        int limit_price;
        int size; // number of resting orders at this price
        int total_volume; // sum of remaining shares at this price
        Order *head_order;
        Order *tail_order;
    
    public:
        Limit(int l_price, int s, int t_volume) { // constructor
            limit_price = l_price;
            size = s;
            total_volume = t_volume;
            head_order = nullptr;
            tail_order = nullptr;
        }
        ~Limit() { // destructor

        };
        Order *get_tail_order() {
            return tail_order;
        }

        Order *get_head_order() {
            return head_order;
        }

        int get_limit_price() {
            return limit_price;
        }

        int get_size() {
            return size;
        }

        int get_total_volume() {
            return total_volume;
        }

        void increment_size(int orders) {
            size += orders;
        }

        void increment_total_volume(int shares) {
            total_volume += shares;
        }

        void remove_order(Order &order) {
            Order *previous_order = order.get_prev_order();
            Order *next_order = order.get_next_order();

            if (previous_order != nullptr) {
                previous_order->set_next_order(next_order);
            }
            else {
                head_order = next_order;
            }

            if (next_order != nullptr) {
                next_order->set_previous_order(previous_order);
            }
            else {
                tail_order = previous_order;
            }

            order.set_previous_order(nullptr);
            order.set_next_order(nullptr);
            order.set_parent_limit(nullptr);
            increment_size(-1);
        }

        void add_order(Order &order) {
            if (head_order == nullptr) {
                head_order = &order;
                tail_order = &order;
            }
            else {
                tail_order = &order;
            }
        }

        void print() {
            std::cout << "Limit Price: " << limit_price
                << " | Size: " << size
                << " | Total Volume: " << total_volume
                << std::endl;
        }
};

#endif
