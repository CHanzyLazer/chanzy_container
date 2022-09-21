#pragma once
#ifdef _MSC_VER
#pragma warning(disable: 4577)
#pragma warning(disable: 4530)
#pragma warning(disable: 4514)
#pragma warning(disable: 4061)
#endif

#include "common_excetion.hpp"
#include <algorithm>
#include <vector>
#include <utility>

namespace chanzy {

/// 用于加速 vector 的搜寻来避免重复元素，和加速 set 的随机选取过程的数据结构
    template<class Entry, class Compare = std::less<Entry>, class Alloc = std::allocator<Entry> >
    class sorted_vector final {
    private:
        using _vec = std::vector<Entry, Alloc>;
        _vec m_vector;
        
/// 提供构造函数和赋值运算
    public:
        inline sorted_vector() : m_vector() {}
        inline sorted_vector(const sorted_vector& a_sv) : m_vector(a_sv.m_vector) {}
        inline sorted_vector(const sorted_vector&& a_sv) noexcept: m_vector(std::move(a_sv.m_vector)) {}
        inline sorted_vector(std::initializer_list<Entry> a_list, const Alloc& a_al = Alloc()) : m_vector(a_list, a_al) { std::sort(m_vector.begin(), m_vector.end(), Compare()); }
        inline sorted_vector& operator=(const sorted_vector& a_sv) { m_vector = a_sv.m_vector; return *this; }
        inline sorted_vector& operator=(sorted_vector&& a_sv) noexcept { m_vector = std::move(a_sv.m_vector); return *this; }
        
/// 迭代器部分
    public:
        using iterator = typename _vec::const_iterator; // 暂使用这个方法实现不能修改其值
        using const_iterator = typename _vec::const_iterator;
        inline const_iterator begin() const { return m_vector.begin(); }
        inline iterator begin() { return m_vector.begin(); }
        inline const_iterator end() const { return m_vector.end(); }
        inline iterator end() { return m_vector.end(); }
        
        using reverse_iterator = typename _vec::const_reverse_iterator;
        using const_reverse_iterator = typename _vec::const_reverse_iterator;
        inline const_reverse_iterator rbegin() const { return m_vector.rbegin(); }
        inline reverse_iterator rbegin() { return m_vector.rbegin(); }
        inline const_reverse_iterator rend() const { return m_vector.rend(); }
        inline reverse_iterator rend() { return m_vector.rend(); }

/// 常用接口
    public:
        using size_type = typename _vec::size_type;
        using difference_type = typename _vec::difference_type;
        // 返回最大最小值
        inline const Entry& front() const { return m_vector.front(); }
        inline const Entry& back() const { return m_vector.back(); }
        // 直接向末尾添加元素，不合要求的返回 false
        inline bool push_back(const Entry& a_entry);
        inline bool push_back(Entry&& a_entry);
        // 直接添加不进行判断的版本
        inline void push_back_(const Entry& a_entry);
        inline void push_back_(Entry&& a_entry);
        // 直接实现 binary_search 的几个方法
        inline const_iterator lower_bound(const Entry& a_entry) const                               { return std::lower_bound(m_vector.begin(), m_vector.end(), a_entry, Compare()); }
        inline iterator lower_bound(const Entry& a_entry)                                           { return std::lower_bound(m_vector.begin(), m_vector.end(), a_entry, Compare()); }
        inline const_iterator upper_bound(const Entry& a_entry) const                               { return std::upper_bound(m_vector.begin(), m_vector.end(), a_entry, Compare()); }
        inline iterator upper_bound(const Entry& a_entry)                                           { return std::upper_bound(m_vector.begin(), m_vector.end(), a_entry, Compare()); }
        inline std::pair<const_iterator, const_iterator> equal_range(const Entry& a_entry) const    { return std::equal_range(m_vector.begin(), m_vector.end(), a_entry, Compare()); }
        inline std::pair<iterator, iterator> equal_range(const Entry& a_entry)                      { return std::equal_range(m_vector.begin(), m_vector.end(), a_entry, Compare()); }
        inline bool binary_search(const Entry& a_entry) const                                       { return std::binary_search(m_vector.begin(), m_vector.end(), a_entry, Compare()); }
        inline difference_type count(const Entry& a_entry) const;
        // 实现 set 的方法
        inline const_iterator find(const Entry& a_entry) const;
        inline iterator find(const Entry& a_entry);
        // 类似 set 的添加元素，不保留重复元素
        inline std::pair<iterator, bool> insert_set(const Entry& a_entry);
        inline std::pair<iterator, bool> insert_set(Entry&& a_entry);
        // 直接添加，保留重复元素
        inline iterator insert(const Entry& a_entry) { return m_vector.insert(this->upper_bound(a_entry), a_entry); }
        inline iterator insert(Entry&& a_entry) { return m_vector.insert(this->upper_bound(a_entry), std::move(a_entry)); }
        inline std::pair<iterator, bool> insert(const_iterator a_where, const Entry& a_entry);
        inline std::pair<iterator, bool> insert(const_iterator a_where, Entry&& a_entry);
        inline iterator insert_(const_iterator a_where, const Entry& a_entry);
        inline iterator insert_(const_iterator a_where, Entry&& a_entry);
        // 根据迭代器删除元素，一定会保留原本排序所以直接删除即可
        inline iterator erase(const const_iterator a_where) { return m_vector.erase(a_where); }
        inline iterator erase(const const_iterator a_first, const const_iterator a_last) { return m_vector.erase(a_first, a_last); }
        // 寻找元素删除，即 list 中的 remove 接口，不过会返回移除了的数目
        inline difference_type remove(const Entry& a_entry);
        // 随机访问
        inline const Entry& at_pos(const size_type a_pos) const { return m_vector.at(a_pos); } // 和 flat map 兼容
        inline const Entry& at(const size_type a_pos) const { return m_vector.at(a_pos); }
        inline const Entry& operator[](const size_type a_pos) const { return m_vector[a_pos]; }
        
/// 不常用的实用接口
    public:
        inline void swap(sorted_vector& a_sv) { m_vector.swap(a_sv.m_vector); }
        inline void clear() { m_vector.clear(); }
        inline bool empty() const { return m_vector.empty(); }
        inline void shrink_to_fit() { m_vector.shrink_to_fit(); }
        inline size_type size() const { return m_vector.size(); }
        inline void reserve(const size_type a_size) { m_vector.reserve(a_size); }
        inline size_type capacity() const { return m_vector.capacity(); }
    };
    
    
    template<class Entry, class Compare, class Alloc>
    inline bool sorted_vector<Entry, Compare, Alloc>::push_back(const Entry& a_entry) {
        if (!this->empty() && Compare()(a_entry, this->back())) return false;
        m_vector.push_back(a_entry);
        return true;
    }
    template<class Entry, class Compare, class Alloc>
    inline bool sorted_vector<Entry, Compare, Alloc>::push_back(Entry&& a_entry) {
        if (!this->empty() && Compare()(a_entry, this->back())) return false;
        m_vector.push_back(std::move(a_entry));
        return true;
    }
    template<class Entry, class Compare, class Alloc>
    inline void sorted_vector<Entry, Compare, Alloc>::push_back_(const Entry& a_entry) {
#ifdef CHANZY_DEBUG
        if (!this->empty() && Compare()(a_entry, this->back()))
            throw Exception_container("push_back_ value must make sure the vector in order");
#endif
        m_vector.push_back(a_entry);
    }
    template<class Entry, class Compare, class Alloc>
    inline void sorted_vector<Entry, Compare, Alloc>::push_back_(Entry&& a_entry) {
#ifdef CHANZY_DEBUG
        if (!this->empty() && Compare()(a_entry, this->back()))
            throw Exception_container("push_back_ value must make sure the vector in order");
#endif
        m_vector.push_back(std::move(a_entry));
    }
    
    
    template<class Entry, class Compare, class Alloc>
    inline typename sorted_vector<Entry, Compare, Alloc>::const_iterator
    sorted_vector<Entry, Compare, Alloc>::find(const Entry& a_entry) const {
        const_iterator t_it = this->lower_bound(a_entry);
        if (t_it != this->end() && *t_it == a_entry) return t_it;
        return this->end();
    }
    template<class Entry, class Compare, class Alloc>
    inline typename sorted_vector<Entry, Compare, Alloc>::iterator
    sorted_vector<Entry, Compare, Alloc>::find(const Entry& a_entry) {
        iterator t_it = this->lower_bound(a_entry);
        if (t_it != this->end() && *t_it == a_entry) return t_it;
        return this->end();
    }
    
    
    template<class Entry, class Compare, class Alloc>
    inline typename sorted_vector<Entry, Compare, Alloc>::difference_type
    sorted_vector<Entry, Compare, Alloc>::count(const Entry& a_entry) const {
        std::pair<const_iterator, const_iterator> t_pair = this->equal_range(a_entry);
        return t_pair.second - t_pair.first;
    }
    
    
    template<class Entry, class Compare, class Alloc>
    inline std::pair<typename sorted_vector<Entry, Compare, Alloc>::iterator, bool>
    sorted_vector<Entry, Compare, Alloc>::insert_set(const Entry& a_entry) {
        iterator t_it = this->lower_bound(a_entry);
        if (t_it != this->end() && *t_it == a_entry) return {t_it, false};
        t_it = m_vector.insert(t_it, a_entry); // 注意迭代器会失效
        return {t_it, true};
    }
    template<class Entry, class Compare, class Alloc>
    inline std::pair<typename sorted_vector<Entry, Compare, Alloc>::iterator, bool>
    sorted_vector<Entry, Compare, Alloc>::insert_set(Entry&& a_entry) {
        iterator t_it = this->lower_bound(a_entry);
        if (t_it != this->end() && *t_it == a_entry) return {t_it, false};
        t_it = m_vector.insert(t_it, std::move(a_entry));
        return {t_it, true};
    }
    template<class Entry, class Compare, class Alloc>
    inline std::pair<typename sorted_vector<Entry, Compare, Alloc>::iterator, bool>
    sorted_vector<Entry, Compare, Alloc>::insert(const const_iterator a_where, const Entry& a_entry) {
        if ((a_where != this->end() && Compare()(*a_where, a_entry)) || (a_where != this->begin() && Compare()(a_entry, *(a_where - 1))))
            return {a_where, false};
        return {m_vector.insert(a_where, a_entry), true};
    }
    template<class Entry, class Compare, class Alloc>
    inline std::pair<typename sorted_vector<Entry, Compare, Alloc>::iterator, bool>
    sorted_vector<Entry, Compare, Alloc>::insert(const const_iterator a_where, Entry&& a_entry) {
        if ((a_where != this->end() && Compare()(*a_where, a_entry)) || (a_where != this->begin() && Compare()(a_entry, *(a_where - 1))))
            return {a_where, false};
        return {m_vector.insert(a_where, std::move(a_entry)), true};
    }
    template<class Entry, class Compare, class Alloc>
    inline typename sorted_vector<Entry, Compare, Alloc>::iterator
    sorted_vector<Entry, Compare, Alloc>::insert_(const const_iterator a_where, const Entry& a_entry) {
#ifdef CHANZY_DEBUG
        if ((a_where != this->end() && Compare()(*a_where, a_entry)) || (a_where != this->begin() && Compare()(a_entry, *(a_where - 1))))
            throw Exception_container("insert_ value must make sure the vector in order");
#endif
        return m_vector.insert(a_where, a_entry);
    }
    template<class Entry, class Compare, class Alloc>
    inline typename sorted_vector<Entry, Compare, Alloc>::iterator
    sorted_vector<Entry, Compare, Alloc>::insert_(const const_iterator a_where, Entry&& a_entry) {
#ifdef CHANZY_DEBUG
        if ((a_where != this->end() && Compare()(*a_where, a_entry)) || (a_where != this->begin() && Compare()(a_entry, *(a_where - 1))))
            throw Exception_container("insert_ value must make sure the vector in order");
#endif
        return m_vector.insert(a_where, std::move(a_entry));
    }
    
    
    template<class Entry, class Compare, class Alloc>
    inline typename sorted_vector<Entry, Compare, Alloc>::difference_type
    sorted_vector<Entry, Compare, Alloc>::remove(const Entry& a_entry) {
        std::pair<const_iterator, const_iterator> t_pair = this->equal_range(a_entry);
        difference_type t_out = t_pair.second - t_pair.first;
        if (t_out <= 0) return 0;
        this->erase(t_pair.first, t_pair.second);
        return t_out;
    }
    
}


