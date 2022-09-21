#pragma once

namespace chanzy {
    /// 使用代理嵌套 bool 的方式来实现真正的 vector<bool>
    template<class Type>
    struct Proxy {
        Type value; // NOLINT(misc-non-private-member-variables-in-classes)
        explicit Proxy(const Type& a_in) : value(a_in) {}
    };
    
    using boolean = Proxy<bool>;
    
    /* 参考了 nlohmann 的写法 */
    /// 实现返回使用自定义迭代器的容器
    template<class Container_Type, class Iterator_Type = typename Container_Type::const_iterator, class RIterator_Type = Iterator_Type>
    class container_proxy {
    private:
        const Container_Type& m_container;
    public:
        // construct iteration proxy from container
        inline explicit container_proxy(const Container_Type& a_container) : m_container(a_container) {}
        
        // return iterator begin (needed for range-based for)
        inline Iterator_Type begin() const {
            return Iterator_Type(m_container.begin());
        }
        // return iterator end (needed for range-based for)
        inline Iterator_Type end() const {
            return Iterator_Type(m_container.end());
        }
        inline typename Container_Type::size_type size() const {
            return m_container.size();
        }
        
        inline RIterator_Type rbegin() const {
            return RIterator_Type(m_container.rbegin());
        }
        inline RIterator_Type rend() const {
            return RIterator_Type(m_container.rend());
        }
    };

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
#endif
    namespace convertor {
        /// 提供一个默认转换器，即隐式类型转换
        class Implicit {
        public:
            template<typename In_Type, typename Out_Type>
            inline Out_Type operator()(In_Type a_in) const { return a_in; }
        };
        
        // 提供一个 pair to key 转换器
        class Pair_to_Key_ref {
        public:
            template<typename In_Type, typename Out_Type>
            inline Out_Type operator()(In_Type a_pair) const { return a_pair.first; }
        };
        
        class Pair_to_Key_poi {
        public:
            template<typename In_Type, typename Out_Type>
            inline Out_Type operator()(In_Type a_pair) const { return &(a_pair->first); }
        };
        
        // 提供一个 pair to value 转换器
        class Pair_to_Value_ref {
        public:
            template<typename In_Type, typename Out_Type>
            inline Out_Type operator()(In_Type a_pair) const { return a_pair.second; }
        };
        
        class Pair_to_Value_poi {
        public:
            template<typename In_Type, typename Out_Type>
            inline Out_Type operator()(In_Type a_pair) const { return &(a_pair->second); }
        };
    }
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    
    
    /// 实现根据已有迭代器自动构建自定义迭代器
    // 主要功能是对于迭代器返回类型的 const 进行自定义修饰
    template<class Iterator_Type,
        class Reference = typename Iterator_Type::reference, class Pointer = typename Iterator_Type::pointer,
        class Ref_Convertor = convertor::Implicit, class Poi_Convertor = convertor::Implicit>
    class custom_iterator {
    private:
        Iterator_Type m_it;
        
        inline Reference ref_convertor(typename Iterator_Type::reference a_ref) const {
            return Ref_Convertor().template operator()<typename Iterator_Type::reference, Reference>(a_ref);
        }
        inline Pointer poi_convertor(typename Iterator_Type::pointer a_poi) const {
            return Poi_Convertor().template operator()<typename Iterator_Type::pointer, Pointer>(a_poi);
        }
    protected:
        inline const Iterator_Type& _iter() const {
            return m_it;
        }
    public:
        using difference_type = typename Iterator_Type::difference_type;
        
        explicit custom_iterator(const Iterator_Type& a_it) : m_it(a_it) {}
        // 提供一个非 const 转换为 const 的接口，由于策略比较复杂不再允许隐式转换
        template<class Iterator_Type_R>
        explicit custom_iterator(const custom_iterator<Iterator_Type_R>& rhs): m_it(rhs.m_it) {}
        
        // 应该是和 map 一样的处理方法，可以测试一下是否有速率的影响
        inline Reference operator*() const {
            return ref_convertor(*m_it);
        }
        inline Pointer operator->() const {
            return poi_convertor(&(*m_it));
        }
        // Random access iterator requirements
        inline Reference operator[](difference_type a_pos) const {
            return ref_convertor(m_it[a_pos]);
        }
        
        /// 比较运算符
        inline bool operator==(const custom_iterator& rhs) const {
            return m_it == rhs.m_it;
        }
        inline bool operator!=(const custom_iterator& rhs) const {
            return m_it != rhs.m_it;
        }
        // Random access iterator requirements
        bool operator<(const custom_iterator& rhs) const {
            return m_it < rhs.m_it;
        }
        bool operator>(const custom_iterator& rhs) const {
            return m_it > rhs.m_it;
        }
        bool operator<=(const custom_iterator& rhs) const {
            return m_it <= rhs.m_it;
        }
        bool operator>=(const custom_iterator& rhs) const {
            return m_it >= rhs.m_it;
        }
        
        /// 移动迭代器
        inline custom_iterator& operator++() {
            ++m_it; return *this;
        }
        inline custom_iterator& operator--() {
            --m_it; return *this;
        }
        // Random access iterator requirements
        inline custom_iterator& operator+=(difference_type a_pos) {
            m_it += a_pos; return *this;
        }
        inline custom_iterator operator+(difference_type a_pos) const {
            return custom_iterator(m_it + a_pos);
        }
        inline custom_iterator& operator-=(difference_type a_pos) {
            m_it -= a_pos; return *this;
        }
        inline custom_iterator operator-(difference_type a_pos) const {
            return custom_iterator(m_it - a_pos);
        }
        
        
        inline difference_type operator-(const custom_iterator& rhs) const {
            return m_it - rhs.m_it;
        }
    };
    
    
}
