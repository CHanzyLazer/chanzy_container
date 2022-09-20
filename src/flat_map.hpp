#pragma once
#ifdef _MSC_VER
#pragma warning(disable: 4577)
#pragma warning(disable: 4530)
#pragma warning(disable: 4514)
#pragma warning(disable: 4061)
#endif

#include "common_excetion.hpp"
#include <string>
#include <algorithm>
#include <vector>
#include <utility>


namespace chanzy {
    
    using std::pair;

    
/// 顺便实现了 flat map，由于是 flat 的所以可以有重复 key
    template<class Key, class Value, class Compare = std::less<Key>, class Alloc = std::allocator<pair<Key, Value>> >
    class flat_map final {
    private:
        using _pair = pair<Key, Value>;
        using _vec = std::vector<_pair, Alloc>;
        _vec m_vector;
        using _pair_out = pair<const Key&, Value&>;

    private:
        class _key_compare {
        private:
            Compare m_comp;
        public:
            inline _key_compare(): m_comp() {}
            inline bool operator()(const _pair& a_x, const _pair& a_y) const // sort 使用
            { return m_comp(a_x.first, a_y.first); }
            inline bool operator()(const Key& a_x, const _pair& a_y) const // binary_search 使用
            { return m_comp(a_x, a_y.first); }
            inline bool operator()(const _pair& a_x, const Key& a_y) const
            { return m_comp(a_x.first, a_y); }
        };

/// 提供构造函数和赋值运算
    public:
        inline flat_map() : m_vector() {}
        inline flat_map(const flat_map& a_fm) : m_vector(a_fm.m_vector) {}
        inline flat_map(const flat_map&& a_fm) noexcept: m_vector(std::move(a_fm.m_vector)) {}
        inline flat_map(std::initializer_list<_pair> a_list, const Alloc& a_al = Alloc()) : m_vector(a_list, a_al) { std::sort(m_vector.begin(), m_vector.end(), _key_compare()); }
        inline flat_map& operator=(const flat_map& a_fm) { m_vector = a_fm.m_vector; return *this; }
        inline flat_map& operator=(flat_map&& a_fm) noexcept { m_vector = std::move(a_fm.m_vector); return *this; }

/// 迭代器部分，需要自行实现一份来实现 key 为 const 的情况
    private:
        template<class Iterator_Type>
        class iterator_flat_map;
        template<class Iterator_Type>
        class const_iterator_flat_map;
        using _iterator_vec = typename _vec::iterator;
        using _const_iterator_vec = typename _vec::const_iterator;
        using _riterator_vec = typename _vec::const_reverse_iterator;
        using _const_riterator_vec = typename _vec::const_reverse_iterator;
    public:
        using iterator = iterator_flat_map<_iterator_vec>;
        using const_iterator = const_iterator_flat_map<_const_iterator_vec>;
        inline const_iterator begin() const { return const_iterator(m_vector.begin()); }
        inline iterator begin() { return iterator(m_vector.begin()); }
        inline const_iterator end() const { return const_iterator(m_vector.end()); }
        inline iterator end() { return iterator(m_vector.end()); }

        using reverse_iterator = iterator_flat_map<_riterator_vec>;
        using const_reverse_iterator = const_iterator_flat_map<_const_riterator_vec>;
        inline const_reverse_iterator rbegin() const { return const_reverse_iterator(m_vector.rbegin()); }
        inline reverse_iterator rbegin() { return reverse_iterator(m_vector.rbegin()); }
        inline const_reverse_iterator rend() const { return const_reverse_iterator(m_vector.rend()); }
        inline reverse_iterator rend() { return reverse_iterator(m_vector.rend()); }

/// 内部接口
    private:
        inline _const_iterator_vec _lower_bound(const Key& a_key) const                             { return std::lower_bound(m_vector.begin(), m_vector.end(), a_key, _key_compare()); }
        inline _iterator_vec _lower_bound(const Key& a_key)                                         { return std::lower_bound(m_vector.begin(), m_vector.end(), a_key, _key_compare()); }
        inline _const_iterator_vec _upper_bound(const Key& a_key) const                             { return std::upper_bound(m_vector.begin(), m_vector.end(), a_key, _key_compare()); }
        inline _iterator_vec _upper_bound(const Key& a_key)                                         { return std::upper_bound(m_vector.begin(), m_vector.end(), a_key, _key_compare()); }
        inline pair<_const_iterator_vec, _const_iterator_vec> _equal_range(const Key& a_key) const  { return std::equal_range(m_vector.begin(), m_vector.end(), a_key, _key_compare()); }
        inline pair<_iterator_vec, _iterator_vec> _equal_range(const Key& a_key)                    { return std::equal_range(m_vector.begin(), m_vector.end(), a_key, _key_compare()); }
        inline bool _binary_search(const Key& a_key) const                                          { return std::binary_search(m_vector.begin(), m_vector.end(), a_key, _key_compare()); }
/// 常用接口
    public:
        using size_type = typename _vec::size_type;
        using difference_type = typename _vec::difference_type;
        // 返回最大最小值
        inline const Key& front() const { return m_vector.front().first; }
        inline const Key& back() const { return m_vector.back().first; }
        // 直接向末尾添加元素，不合要求的返回 false
        inline bool push_back(const _pair& a_pair);
        inline bool push_back(_pair&& a_pair);
        // 直接添加不进行判断的版本
        inline void push_back_(const _pair& a_pair);
        inline void push_back_(_pair&& a_pair);
        // 直接实现 binary_search 的几个方法
        inline const_iterator lower_bound(const Key& a_key) const                       { return const_iterator(this->_lower_bound(a_key)); }
        inline iterator lower_bound(const Key& a_key)                                   { return iterator(this->_lower_bound(a_key)); }
        inline const_iterator upper_bound(const Key& a_key) const                       { return const_iterator(this->_upper_bound(a_key)); }
        inline iterator upper_bound(const Key& a_key)                                   { return iterator(this->_upper_bound(a_key)); }
        inline pair<const_iterator, const_iterator> equal_range(const Key& a_key) const { auto t_pair = this->_equal_range(a_key); return {const_iterator(t_pair.first), const_iterator(t_pair.second)}; }
        inline pair<iterator, iterator> equal_range(const Key& a_key)                   { auto t_pair = this->_equal_range(a_key); return {iterator(t_pair.first), iterator(t_pair.second)}; }
        inline bool binary_search(const Key& a_key) const                               { return this->_binary_search(a_key); }
        inline difference_type count(const Key& a_key) const;
        inline const_iterator find(const Key& a_key) const; // 实现 map 的方法
        inline iterator find(const Key& a_key); // 实现 map 的方法
        // 类似 map 的添加元素，不保留重复元素
        inline pair<iterator, bool> insert_map(const _pair& a_pair);
        inline pair<iterator, bool> insert_map(_pair&& a_pair);
        // 直接添加，保留重复元素
        inline iterator insert(const _pair& a_pair) { return iterator(m_vector.insert(this->_upper_bound(a_pair.first), a_pair)); }
        inline iterator insert(_pair&& a_pair) { return iterator(m_vector.insert(this->_upper_bound(a_pair.first), std::move(a_pair))); }
        inline pair<iterator, bool> insert(const_iterator a_where, const _pair& a_pair);
        inline pair<iterator, bool> insert(const_iterator a_where, _pair&& a_pair);
        inline iterator insert_(const_iterator a_where, const _pair& a_pair);
        inline iterator insert_(const_iterator a_where, _pair&& a_pair);
        // 根据迭代器删除元素，一定会保留原本排序所以直接删除即可
        inline iterator erase(const_iterator a_where);
        inline iterator erase(const_iterator a_first, const_iterator a_last);
        // 寻找元素删除，即 list 中的 remove 接口，不过会返回移除了的数目
        inline difference_type remove(const Key& a_key);
        // 为了符合习惯，at 和 [] 需要和 map 的逻辑一致
        inline const Value& at(const Key& a_key) const;
        inline Value& at(const Key& a_key);
        inline Value& operator[](const Key& a_key);
        // 随机访问
        inline _pair_out at_pos(const size_type a_pos) const { _pair& t_pair = m_vector.at(a_pos); return {t_pair.first, t_pair.second}; }
        inline _pair_out at_pos(const size_type a_pos) { _pair& t_pair = m_vector.at(a_pos); return {t_pair.first, t_pair.second}; }


/// 不常用的实用接口
    public:
        inline void swap(flat_map& a_fm) { m_vector.swap(a_fm.m_vector); }
        inline void clear() { m_vector.clear(); }
        inline bool empty() const { return m_vector.empty(); }
        inline void shrink_to_fit() { m_vector.shrink_to_fit(); }
        inline size_type size() const { return m_vector.size(); }
        inline void reserve(const size_type a_size) { m_vector.reserve(a_size); }
        inline size_type capacity() const { return m_vector.capacity(); }
    };


/// 迭代器部分，将原本的返回 pair 改为专门的 key 和 value 函数
    template<class Key, class Value, class Compare, class Alloc>
    template<class Iterator_Type>
    class flat_map<Key, Value, Compare, Alloc>::iterator_flat_map {
    private:
        friend class flat_map<Key, Value, Compare, Alloc>;
        friend class const_iterator_flat_map<Iterator_Type>;
        Iterator_Type m_it;
    protected:
        inline const Iterator_Type& _iter() const {
            return m_it;
        }
    public:
        // 在原本基础上再提供 key 和 value 接口
        inline const Key& key() const {
            return m_it->first;
        }
        inline Value& value() const {
            return m_it->second;
        }
    public:
        inline explicit iterator_flat_map(const Iterator_Type& a_it): m_it(a_it) {}
    /// 还是需要自己实现这些接口，麻烦
    public:
        using difference_type = typename Iterator_Type::difference_type;

        inline Value& operator*() const { return *m_it; }
        inline Value* operator->() const { return &(*m_it); }
        // Random access iterator requirements
        inline Value& operator[](difference_type a_pos) const { return m_it[a_pos]; }
        /// 比较运算符
        inline bool operator==(const const_iterator& rhs) const { return m_it == rhs.m_it; }
        inline bool operator!=(const const_iterator& rhs) const { return m_it != rhs.m_it; }
        // Random access iterator requirements
        bool operator<(const const_iterator& rhs) const { return m_it < rhs.m_it; }
        bool operator>(const const_iterator& rhs) const { return m_it > rhs.m_it; }
        bool operator<=(const const_iterator& rhs) const { return m_it <= rhs.m_it; }
        bool operator>=(const const_iterator& rhs) const { return m_it >= rhs.m_it; }
        /// 移动迭代器
        inline iterator& operator++() { ++m_it; return *this; }
        inline iterator& operator--() { --m_it; return *this; }
        // Random access iterator requirements
        inline iterator& operator+=(difference_type a_pos) { m_it += a_pos; return *this; }
        inline iterator operator+(difference_type a_pos) const { return iterator(m_it + a_pos); }
        inline iterator& operator-=(difference_type a_pos) { m_it -= a_pos; return *this; }
        inline iterator operator-(difference_type a_pos) const { return iterator(m_it - a_pos); }

        inline difference_type operator-(const const_iterator& rhs) const { return m_it - rhs.m_it; }
    };

    template<class Key, class Value, class Compare, class Alloc>
    template<class Iterator_Type>
    class flat_map<Key, Value, Compare, Alloc>::const_iterator_flat_map {
    private:
        friend class flat_map<Key, Value, Compare, Alloc>;
        Iterator_Type m_it;
        inline const Iterator_Type& _iter() const {
            return m_it;
        }
    public:
        // 在原本基础上再提供 key 和 value 接口
        inline const Key& key() const {
            return m_it->first;
        }
        inline const Value& value() const {
            return m_it->second;
        }
    public:
        inline explicit const_iterator_flat_map(const Iterator_Type& a_it): m_it(a_it) {}
        // 提供直接转换成 const 迭代器的接口
        inline const_iterator_flat_map(const iterator& a_it): m_it(a_it._iter()) {} // NOLINT(google-explicit-constructor)
    /// 还是需要自己实现这些接口，麻烦
    public:
        using difference_type = typename Iterator_Type::difference_type;

        inline const Value& operator*() const { return *m_it; }
        inline const Value* operator->() const { return &(*m_it); }
        // Random access iterator requirements
        inline const Value& operator[](difference_type a_pos) const { return m_it[a_pos]; }
        /// 比较运算符
        inline bool operator==(const const_iterator& rhs) const { return m_it == rhs.m_it; }
        inline bool operator!=(const const_iterator& rhs) const { return m_it != rhs.m_it; }
        // Random access iterator requirements
        bool operator<(const const_iterator& rhs) const { return m_it < rhs.m_it; }
        bool operator>(const const_iterator& rhs) const { return m_it > rhs.m_it; }
        bool operator<=(const const_iterator& rhs) const { return m_it <= rhs.m_it; }
        bool operator>=(const const_iterator& rhs) const { return m_it >= rhs.m_it; }
        /// 移动迭代器
        inline const_iterator& operator++() { ++m_it; return *this; }
        inline const_iterator& operator--() { --m_it; return *this; }
        // Random access iterator requirements
        inline const_iterator& operator+=(difference_type a_pos) { m_it += a_pos; return *this; }
        inline const_iterator operator+(difference_type a_pos) const { return const_iterator(m_it + a_pos); }
        inline const_iterator& operator-=(difference_type a_pos) { m_it -= a_pos; return *this; }
        inline const_iterator operator-(difference_type a_pos) const { return const_iterator(m_it - a_pos); }

        inline difference_type operator-(const const_iterator& rhs) const { return m_it - rhs.m_it; }
    };



    template<class Key, class Value, class Compare, class Alloc>
    inline bool flat_map<Key, Value, Compare, Alloc>::push_back(const _pair& a_pair) {
        if (!this->empty() && Compare()(a_pair.first, this->back())) return false;
        m_vector.push_back(a_pair);
        return true;
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline bool flat_map<Key, Value, Compare, Alloc>::push_back(_pair&& a_pair) {
        if (!this->empty() && Compare()(a_pair.first, this->back())) return false;
        m_vector.push_back(std::move(a_pair));
        return true;
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline void flat_map<Key, Value, Compare, Alloc>::push_back_(const _pair& a_pair) {
#ifdef CHANZY_DEBUG
        if (!this->empty() && Compare()(a_pair.first, this->back()))
            throw Exception_container("push_back_ key must make sure the map in order");
#endif
        m_vector.push_back(a_pair);
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline void flat_map<Key, Value, Compare, Alloc>::push_back_(_pair&& a_pair) {
#ifdef CHANZY_DEBUG
        if (!this->empty() && Compare()(a_pair.first, this->back()))
            throw Exception_container("push_back_ key must make sure the map in order");
#endif
        m_vector.push_back(std::move(a_pair));
    }


    template<class Key, class Value, class Compare, class Alloc>
    inline typename flat_map<Key, Value, Compare, Alloc>::const_iterator
    flat_map<Key, Value, Compare, Alloc>::find(const Key& a_key) const {
        const_iterator t_it = this->lower_bound(a_key);
        if (t_it != this->end() && t_it.key() == a_key) return t_it;
        return this->end();
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline typename flat_map<Key, Value, Compare, Alloc>::iterator
    flat_map<Key, Value, Compare, Alloc>::find(const Key& a_key) {
        iterator t_it = this->lower_bound(a_key);
        if (t_it != this->end() && t_it.key() == a_key) return t_it;
        return this->end();
    }


    template<class Key, class Value, class Compare, class Alloc>
    inline typename flat_map<Key, Value, Compare, Alloc>::difference_type
    flat_map<Key, Value, Compare, Alloc>::count(const Key& a_key) const {
        std::pair<const_iterator, const_iterator> t_pair = this->equal_range(a_key);
        return t_pair.second - t_pair.first;
    }


    template<class Key, class Value, class Compare, class Alloc>
    inline std::pair<typename flat_map<Key, Value, Compare, Alloc>::iterator, bool>
    flat_map<Key, Value, Compare, Alloc>::insert_map(const _pair& a_pair) {
        auto t_it = this->_lower_bound(a_pair.first);
        if (t_it != this->end() && t_it->first == a_pair.first) return {iterator(t_it), false};
        t_it = m_vector.insert(t_it, a_pair); // 注意迭代器会失效
        return {iterator(t_it), true};
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline std::pair<typename flat_map<Key, Value, Compare, Alloc>::iterator, bool>
    flat_map<Key, Value, Compare, Alloc>::insert_map(_pair&& a_pair) {
        auto t_it = this->_lower_bound(a_pair.first);
        if (t_it != this->end() && t_it->first == a_pair.first) return {iterator(t_it), false};
        t_it = m_vector.insert(t_it, std::move(a_pair));
        return {iterator(t_it), true};
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline std::pair<typename flat_map<Key, Value, Compare, Alloc>::iterator, bool>
    flat_map<Key, Value, Compare, Alloc>::insert(const const_iterator a_where, const _pair& a_pair) {
        if ((a_where != this->end() && Compare()(*a_where, a_pair.first)) || (a_where != this->begin() && Compare()(a_pair.first, *(a_where-1))))
            return {a_where, false};
        return {iterator(m_vector.insert(a_where._iter(), a_pair)), true};
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline std::pair<typename flat_map<Key, Value, Compare, Alloc>::iterator, bool>
    flat_map<Key, Value, Compare, Alloc>::insert(const const_iterator a_where, _pair&& a_pair) {
        if ((a_where != this->end() && Compare()(*a_where, a_pair.first)) || (a_where != this->begin() && Compare()(a_pair.first, *(a_where-1))))
            return {a_where, false};
        return {iterator(m_vector.insert(a_where._iter(), std::move(a_pair))), true};
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline typename flat_map<Key, Value, Compare, Alloc>::iterator
    flat_map<Key, Value, Compare, Alloc>::insert_(const const_iterator a_where, const _pair& a_pair) {
#ifdef CHANZY_DEBUG
        if ((a_where != this->end() && Compare()(*a_where, a_pair.first)) || (a_where != this->begin() && Compare()(a_pair.first, *(a_where-1))))
            throw Exception_container("insert_ key must make sure the map in order");
#endif
        return iterator(m_vector.insert(a_where._iter(), a_pair));
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline typename flat_map<Key, Value, Compare, Alloc>::iterator
    flat_map<Key, Value, Compare, Alloc>::insert_(const const_iterator a_where, _pair&& a_pair) {
#ifdef CHANZY_DEBUG
        if ((a_where != this->end() && Compare()(*a_where, a_pair.first)) || (a_where != this->begin() && Compare()(a_pair.first, *(a_where-1))))
            throw Exception_container("insert_ key must make sure the map in order");
#endif
        return iterator(m_vector.insert(a_where._iter(), std::move(a_pair)));
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline typename flat_map<Key, Value, Compare, Alloc>::iterator
    flat_map<Key, Value, Compare, Alloc>::erase(const flat_map::const_iterator a_where) {
        return iterator(m_vector.erase(a_where._iter()));
    }
    template<class Key, class Value, class Compare, class Alloc>
    inline typename flat_map<Key, Value, Compare, Alloc>::iterator
    flat_map<Key, Value, Compare, Alloc>::erase(const const_iterator a_first, const const_iterator a_last) {
        return iterator(m_vector.erase(a_first._iter(), a_last._iter()));
    }


    template<class Key, class Value, class Compare, class Alloc>
    inline typename flat_map<Key, Value, Compare, Alloc>::difference_type
    flat_map<Key, Value, Compare, Alloc>::remove(const Key& a_key) {
        auto t_pair = this->_equal_range(a_key);
        auto t_out = t_pair.second - t_pair.first;
        if (t_out <= 0) return 0;
        m_vector.erase(t_pair.first, t_pair.second);
        return t_out;
    }



    template<class Key, class Value, class Compare, class Alloc>
    const Value& flat_map<Key, Value, Compare, Alloc>::at(const Key& a_key) const {
        const_iterator t_it = this->lower_bound(a_key);
#ifdef CHANZY_DEBUG
        if (t_it != this->end() && t_it.key() != a_key)
            throw Exception_container("invalid key");
#endif
        return t_it.value();
    }
    template<class Key, class Value, class Compare, class Alloc>
    Value& flat_map<Key, Value, Compare, Alloc>::at(const Key& a_key) {
        iterator t_it = this->lower_bound(a_key);
#ifdef CHANZY_DEBUG
        if (t_it != this->end() && t_it.key() != a_key)
            throw Exception_container("invalid key");
#endif
        return t_it.value();
    }
    template<class Key, class Value, class Compare, class Alloc>
    Value& flat_map<Key, Value, Compare, Alloc>::operator[](const Key& a_key) {
        auto t_it = this->_lower_bound(a_key);
        if (t_it == m_vector.end() || t_it->first != a_key)
            t_it = m_vector.insert(t_it, {a_key, Value()});
        return t_it->second;
    }



}


