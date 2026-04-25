#include <algorithm>
#include <iostream>

#include "book.h"
#include "order.h"

#define ORDER_SIZE 1

#define REMOVE_SHARES '-'
#define ADD_SHARES '+'

static void execute_sell_order(Order &order, Book &book) {
    while(order.get_shares() > 0) {
        Limit *limit = book.get_highest_buy_limit();
        if (limit == nullptr || order.get_limit_price() > limit->get_limit_price()) {
            break;
        }

        Order *current_order = limit->get_head_order();
        if (current_order == nullptr) {
            book.remove_buy_limit(limit->get_limit_price());
            continue;
        }

        int traded_shares = std::min(order.get_shares(), current_order->get_shares());
        order.update_shares(traded_shares, REMOVE_SHARES);
        current_order->update_shares(traded_shares, REMOVE_SHARES);
        limit->increment_total_volume(-traded_shares);

        if (current_order->get_shares() == 0) {
            int completed_order_id = current_order->get_id();

            limit->remove_order(*current_order);
            book.remove_order_from_map(completed_order_id);

            if (limit->get_size() == 0) {
                book.remove_buy_limit(limit->get_limit_price());
            }
        }
    }
    return;
}

static void execute_buy_order(Order &order, Book &book) {
    while (order.get_shares() > 0) {
        Limit *limit = book.get_lowest_sell_limit();
        if (limit == nullptr || order.get_limit_price() < limit->get_limit_price()) {
            break;
        }

        Order *current_order = limit->get_head_order();
        if (current_order == nullptr) {
            book.remove_sell_limit(limit->get_limit_price());
            continue;
        }

        int traded_shares = std::min(order.get_shares(), current_order->get_shares());
        order.update_shares(traded_shares, REMOVE_SHARES);
        current_order->update_shares(traded_shares, REMOVE_SHARES);
        limit->increment_total_volume(-traded_shares);

        if (current_order->get_shares() == 0) {
            int completed_order_id = current_order->get_id();

            limit->remove_order(*current_order);
            book.remove_order_from_map(completed_order_id);

            if (limit->get_size() == 0) {
                book.remove_sell_limit(limit->get_limit_price());
            }
        }
    }
}

static void add_buy_order(Order &b_order, Book *book) {
    if (book->get_lowest_sell_limit() != nullptr) { // check that sell limit is there
        if (b_order.get_limit_price() >= book->get_lowest_sell_limit()->get_limit_price()) {
            execute_buy_order(b_order, *book);
            if (b_order.get_shares() == 0) {
                return;
            }
        }
    }

    Limit *limit = book->get_buy_limit_by_price(b_order.get_limit_price());
    if (limit == nullptr) {
        limit = new Limit (
            b_order.get_limit_price(), 
            0,
            0
        );
        book->add_buy_limit(limit); // finish implementation for adding to tree
    }

    Order *previous_order = limit->get_tail_order();
    if (previous_order != nullptr) {
        previous_order->set_next_order(&b_order);
    }
    limit->add_order(b_order);

    b_order.set_parent_limit(limit);
    b_order.set_previous_order(previous_order);
    b_order.set_next_order(nullptr);

    limit->increment_size(ORDER_SIZE);
    limit->increment_total_volume(b_order.get_shares());
    
    book->add_order_to_map(b_order);
}

static void add_sell_order(Order &s_order, Book *book) {
    if (book->get_highest_buy_limit() != nullptr &&
        s_order.get_limit_price() <= book->get_highest_buy_limit()->get_limit_price()) {
        execute_sell_order(s_order, *book);
        if (s_order.get_shares() == 0) {
            return;
        }
    }

    Limit *limit = book->get_sell_limit_by_price(s_order.get_limit_price());
    if (limit == nullptr) {
        limit = new Limit (
            s_order.get_limit_price(),
            0,
            0
        );


        book->add_sell_limit(limit); // finish implementation for adding to tree
    }

    Order *previous_order = limit->get_tail_order();
    if (previous_order != nullptr) {
        previous_order->set_next_order(&s_order);
    }
    limit->add_order(s_order);

    s_order.set_parent_limit(limit);
    s_order.set_previous_order(previous_order);
    s_order.set_next_order(nullptr);

    limit->increment_size(ORDER_SIZE);
    limit->increment_total_volume(s_order.get_shares());

    book->add_order_to_map(s_order);

    // need to check for execution of orders here I think
}

void Book::add_order(Order &order) {
    if (order.get_type() == true) {
        add_buy_order(order, this);
    }
    else if (order.get_type() == false) {
        add_sell_order(order, this);
	}
}

bool Book::cancel_order(int order_id) {
    Order *order = get_order_by_id(order_id);
    if (order == nullptr) {
        return false;
    }

    Limit *limit = order->get_parent_limit();
    if (limit == nullptr) {
        remove_order_from_map(order_id);
        return false;
    }

    limit->increment_total_volume(-order->get_shares());
    limit->remove_order(*order);
    remove_order_from_map(order_id);

    if (limit->get_size() == 0) {
        if (order->get_type()) {
            remove_buy_limit(limit->get_limit_price());
        }
        else {
            remove_sell_limit(limit->get_limit_price());
        }
    }

    return true;
}

void Book::add_buy_limit(Limit *current) {
    if (current == nullptr) {
        return;
    }

    add_buy_limit_by_price(current);
}
void Book::add_sell_limit(Limit *current) {
    if (current == nullptr) {
        return;
    }

    add_sell_limit_by_price(current);
}
