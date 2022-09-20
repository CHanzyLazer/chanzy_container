#pragma once
#include "flat_map.hpp"
#include "linked_cell.hpp"
#include "position.hpp"
#include "sorted_vector.hpp"
#include "proxy.hpp"
#include <iostream>

using namespace std;
using namespace chanzy;


template<typename Entry>
inline ostream& operator<<(ostream& a_cout, const sorted_vector<Entry>& a_sv) {
    for (auto& t_e : a_sv) {
        a_cout << t_e << ", ";
    }
    return a_cout;
}
template<typename Key, typename Value>
inline ostream& operator<<(ostream& a_cout, const flat_map<Key, Value>& a_fm) {
    for (auto t_it = a_fm.begin(); t_it != a_fm.end(); ++t_it) {
        a_cout << "{" << t_it.key() << ": ";
        a_cout << t_it.value() << "}, ";
    }
    return a_cout;
}

inline void test_sorted_vector() {
    cout << "========= test_sorted_vector =========" << endl;

    sorted_vector<double> t_sv {1.0, 2.0, 1.0, 0.8, 3.0};
    cout << t_sv << endl;
    auto t_it = t_sv.lower_bound(1.3);
    cout << *t_it << endl;
    t_it = t_sv.find(0.8);
    cout << *t_it << endl;
    t_sv.push_back(3.1);
    t_sv.push_back(3.0);
    cout << t_sv << endl;
    t_sv.push_back_(3.1);
    cout << t_sv << endl;
}

inline void test_flat_map() {
    cout << "========= test_flat_map =========" << endl;

    clock_t t_start, t_end;
    t_start = clock();
    flat_map<double, double> t_fm;
    for (int i = 0; i < 1000000; ++i) {
        t_fm.insert({(double)i, (double)i/10.0});
    }
    t_end = clock();
    cout << "flat_map insert time = " << double(t_end - t_start) / CLOCKS_PER_SEC << " s" << endl;

}

inline void test_proxy() {
    cout << "========= test_proxy =========" << endl;

    using e_pair = pair<double, double>;
    using vec = vector<e_pair>;
    vec t_v {{1.0, 0.1}, {1.0, 0.1}, {3.0, 0.3}, {2.0, 0.2}};

    auto t_it = t_v.begin();
    chanzy::custom_iterator<vec::iterator, const double&, double*, chanzy::convertor::Pair_to_Value_ref, chanzy::convertor::Pair_to_Value_poi> t_p_it (t_it);
    cout << *t_p_it << endl;
    *(t_p_it.operator->()) += 1;
    cout << *t_p_it << endl;
}

inline void test_linked_cell() {
    cout << "========= linked_cell =========" << endl;

    linked_cell<Position> t_lc;
    t_lc.set_link_parameter(LC_BOU::PBC, LC_BOU::PBC);
    t_lc.set_cell(1.0, 3.0, 3.0, 3.0);
    for (int i = 0; i < t_lc.size_x(); ++i) for (int j = 0; j < t_lc.size_y(); ++j) for (int k = 0; k < t_lc.size_y(); ++k) {
        t_lc(i, j, k) = {double(i), double(j), double(k)};
    }
    auto& t_links = t_lc.links_at(2.5, 2.5, 2.5);
    for (auto& t_link : t_links) {
        cout << (t_link.value()) << endl;
        cout << (t_link.value() + t_link.get_direction()) << endl;
        cout << "===========" << endl;
    }
}


int main(int argc, char* argv[]) {
    test_sorted_vector();
    test_flat_map();
    test_linked_cell();
    test_proxy();

#ifndef CHANZY_DEBUG
    cout << "press any key to continue...";
    cin.get();
#endif
    return 0;
}