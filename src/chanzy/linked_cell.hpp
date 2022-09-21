#pragma once
#ifdef _MSC_VER
#pragma warning(disable: 4577)
#pragma warning(disable: 4530)
#pragma warning(disable: 4514)
#pragma warning(disable: 4061)
#endif

#include "position.hpp"
#include "common_excetion.hpp"
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>


namespace chanzy {
    
    enum class LC_BOU {
        none, PBC, FBC
    };
    
    using std::floor;
    using std::cout;
    using std::cin;
    using std::endl;
    using std::vector;
    using std::array;
    
    
    template<class Cell_Type>
    class linked_cell;

//link类，不允许在single cell外构造
    template<class Cell_Type>
    class link {
        template<class T> friend class linked_cell; //必须这样来防止显示报错
    private:
        Cell_Type* m_data_ptr;
        Position m_direction;
        bool m_mirror;
        bool m_center;
        //构造函数部分
    private:
        inline link() : m_data_ptr(nullptr), m_direction(0.0, 0.0, 0.0), m_mirror(false), m_center(true) {}
        inline explicit link(Cell_Type* const a_ptr, bool a_mirror = false, const double& a_dx = 0.0, const double& a_dy = 0.0, const double& a_dz = 0.0, bool a_center = true) :
        m_data_ptr(a_ptr), m_direction(a_dx, a_dy, a_dz), m_mirror(a_mirror), m_center(a_center) {}
        //重写指针常用操作
    public:
        inline Cell_Type& operator*() { return *m_data_ptr; }
        inline const Cell_Type& operator*() const { return *m_data_ptr; }
        inline Cell_Type* operator->() { return m_data_ptr; }
        inline const Cell_Type* operator->() const { return m_data_ptr; }
        //检测是否是镜像link以及根据direction返回double数方便计算
    public:
        //接口移回到这里
        inline const Cell_Type& value() const {
            return *m_data_ptr;
        }
        inline bool check_mirror() const {
            return m_mirror;
        }
        inline const Position& get_direction() const {
            return m_direction;
        }
        inline bool check_center() const {
            return m_center;
        }
    };

/*----------------------*/
//linked_cell
    template<class Cell_Type>
    class linked_cell {
        /*----------------------*/
        //内部数据
        using cell_database = vector<Cell_Type>;
        using cell_link = link<Cell_Type>;
    public:
        using container_links = vector<cell_link>;
    private:
        cell_database m_database; //存放的数据，存放数据都使用vector
        // 不再存储所有 links 而是即时运算来节约内存，使用成员变量避免重复allocate内存，不过此时不再线程安全
        container_links t_cell_links;
        container_links t_empty_links;
        
        int m_dimension;
        int m_size_x, m_size_y, m_size_z; // = (int)floor(m_length_x/m_length_cell)
        LC_BOU m_link_type_x, m_link_type_y, m_link_type_z;
        double m_length_x, m_length_y, m_length_z;
        double m_length_cell; // 将 cell 的大小管理纳入
        
        
        /*----------------------*/
        //迭代器部分
    public:
        using cell_iterator = typename cell_database::iterator;
        using const_cell_iterator = typename cell_database::const_iterator;
        
        inline cell_iterator begin() { return cell_iterator(m_database.begin()); }
        inline const_cell_iterator begin() const { return const_cell_iterator(m_database.begin()); }
        inline cell_iterator end() { return cell_iterator(m_database.end()); }
        inline const_cell_iterator end() const { return const_cell_iterator(m_database.end()); }
        
        /*----------------------*/
        //构造函数，指定三维大小，最后可选参数指定链接的类型
    public:
        linked_cell();
        linked_cell(LC_BOU p1, LC_BOU p2, LC_BOU p3,
                    const double& a_length_x, const double& a_length_y, const double& a_length_z);
        linked_cell(const double& a_length_cell,
                    LC_BOU p1, const double& a_length_x);
        linked_cell(const double& a_length_cell,
                    LC_BOU p1, const double& a_length_x,
                    LC_BOU p2, const double& a_length_y);
        linked_cell(const double& a_length_cell,
                    LC_BOU p1, const double& a_length_x,
                    LC_BOU p2, const double& a_length_y,
                    LC_BOU p3, const double& a_length_z);
        
        /*----------------------*/
        //不需要重写以下函数保证链接的指针正确性，因为已经禁止拷贝了
        
        /*----------------------*/
        //内部函数
    private:
        //根据属性初始化linked_cell，包含每个cell赋默认值，按照维度和链接类型设置cell_links（为了健壮性内部会先clear数组）
        void _init_linked_cell();
        void _set_link_cell(const int& a_x, const int& a_y = 0, const int& a_z = 0);
        void _push_link_cell(int a_x, int a_y, int a_z, const int& ofs_x = 0, const int& ofs_y = 0, const int& ofs_z = 0);
        // 检测 index 是否合法
        inline bool _check_index_1d(const int& a_i) const { return a_i < this->size() && a_i >= 0; }
        inline bool _check_index_3d(const int& a_x, const int& a_y, const int& a_z) const
        { return a_x < m_size_x && a_x >= 0 && a_y < m_size_y && a_y >= 0 && a_z < m_size_z && a_z >= 0; }
        inline bool _check_position(const double& a_d_x, const double& a_d_y, const double& a_d_z) const
        { return a_d_x < m_length_x && a_d_x >= 0.0 && a_d_y < m_length_y && a_d_y >= 0.0 && a_d_z < m_length_z && a_d_z >= 0.0; }
        /*----------------------*/
        //提供的接口
    public:
        // 设置尺寸，既能初始化尺寸也能重新设置尺寸
        void set_cell(const double& a_length_cell, const double& a_length_x = 1.0, const double& a_length_y = 1.0, const double& a_length_z = 1.0);
        // 依据参数设置链接
        void set_link_parameter(LC_BOU p1, LC_BOU p2 = LC_BOU::none, LC_BOU p3 = LC_BOU::none);
        // 返回边界条件
        LC_BOU link_type_x() { return m_link_type_x; }
        LC_BOU link_type_y() { return m_link_type_y; }
        LC_BOU link_type_z() { return m_link_type_z; }
        // 返回维度
        inline int dimension() const { return m_dimension; }
        // 返回 types 的长度
        inline size_t size() const { return m_database.size(); }
        // 返回三个方向的 size
        inline int size_x() const { return m_size_x; }
        inline int size_y() const { return m_size_y; }
        inline int size_z() const { return m_size_z; }
        // 返回 cell length
        inline const double& length_cell() const { return m_length_cell; }
        // 返回三个方向的 length
        inline const double& length_x() const { return m_length_x; }
        inline const double& length_y() const { return m_length_y; }
        inline const double& length_z() const { return m_length_z; }
        // 根据单下标返回三维下标，有边界检查
        void index_3d(const int& a_i, int& x, int& y, int& z) const;
        // 根据三维下标返回单下标，有边界检查
        int index_1d(const int& a_x, const int& a_y, const int& a_z) const;
        // 根据三维坐标返回三维下标，有边界检查
        void index_3d_at(const double& a_d_x, const double& a_d_y, const double& a_d_z, int& x, int& y, int& z) const;
        // 根据三维坐标返回单下标，有边界检查
        int index_1d_at(const double& a_d_x, const double& a_d_y, const double& a_d_z) const;
        
        //根据三维下标返回迭代器
        inline cell_iterator get_iterator(const int& a_x = 0, const int& a_y = 0, const int& a_z = 0)
        { return begin() + index_1d(a_x, a_y, a_z); }
        inline const_cell_iterator get_iterator(const int& a_x = 0, const int& a_y = 0, const int& a_z = 0) const
        { return begin() + index_1d(a_x, a_y, a_z); }
        //根据三维下标返回links
        inline container_links& get_links(const int& a_x = 0, const int& a_y = 0, const int& a_z = 0)
        { if (_check_index_3d(a_x, a_y, a_z)) {_set_link_cell(a_x, a_y, a_z); return t_cell_links;} else return t_empty_links; }
        inline const container_links& get_links(const int& a_x = 0, const int& a_y = 0, const int& a_z = 0) const
        { return const_cast<linked_cell*>(this)->get_links(a_x, a_y, a_z); }
        //根据三维坐标返回links
        inline container_links& links_at(const double& a_d_x, const double& a_d_y = 0.0, const double& a_d_z = 0.0)
        { if (_check_position(a_d_x, a_d_y, a_d_z)) {int t_i, t_j, t_k; index_3d_at(a_d_x, a_d_y, a_d_z, t_i, t_j, t_k); return get_links(t_i, t_j, t_k);} else return t_empty_links; }
        inline const container_links& links_at(const double& a_d_x, const double& a_d_y = 0.0, const double& a_d_z = 0.0) const
        { return const_cast<linked_cell* >(this)->links_at(a_d_x, a_d_y, a_d_z); }
        inline container_links& links_at(const Position& a_pos)
        { return this->links_at(a_pos[0], a_pos[1], a_pos[2]); }
        inline const container_links& links_at(const Position& a_pos) const
        { return const_cast<linked_cell*>(this)->links_at(a_pos[0], a_pos[1], a_pos[2]); }
        
        //使用默认初始化清空每个cell，不改变link
        void clear_cell();
        
        /*----------------------*/
        //debug接口
    public:
        //根据维数打印cell，和matlab一致按列打印
        void print_cell() const;
        //根据三维下标打印对应cell的link信息
        void print_link(const int& a_x, const int& a_y = 0, const int& a_z = 0) const;
        //按照迭代器顺序从0开始递增赋值数据，类型必须支持前置++
        void assign_increase();
        
        /*----------------------*/
        //重载运算符，[]和（）都应只返回single_cell<Cell_Type>的数据data（Cell_Type）的引用
    public:
        inline Cell_Type& operator()(const int& a_x, const int& a_y = 0, const int& a_z = 0)
        { return m_database[index_1d(a_x, a_y, a_z)]; }
        inline const Cell_Type& operator()(const int& a_x, const int& a_y = 0, const int& a_z = 0) const
        { return m_database[index_1d(a_x, a_y, a_z)]; }
        inline Cell_Type& operator[](const int& i) { return m_database[i]; }
        inline const Cell_Type& operator[](const int& i) const { return m_database[i]; }
        // 使用 at 方法得到对应坐标的 cell
        inline Cell_Type& at(const double& a_d_x, const double& a_d_y = 0.0, const double& a_d_z = 0.0)
        { return m_database[index_1d_at(a_d_x, a_d_y, a_d_z)]; }
        inline const Cell_Type& at(const double& a_d_x, const double& a_d_y = 0.0, const double& a_d_z = 0.0) const
        { return m_database[index_1d_at(a_d_x, a_d_y, a_d_z)]; }
        // 能够接收 position
        inline Cell_Type& at(const Position& a_pos) { return this->at(a_pos[0], a_pos[1], a_pos[2]); }
        inline const Cell_Type& at(const Position& a_pos) const { return this->at(a_pos[0], a_pos[1], a_pos[2]); }
        // 使用 at_ptr 方法得到对应坐标的 cell 的指针，进行边界检测，对于不符合的返回 nullptr
        inline Cell_Type* at_ptr(const double& a_d_x, const double& a_d_y = 0.0, const double& a_d_z = 0.0)
        { if (_check_position(a_d_x, a_d_y, a_d_z))return &(this->at(a_d_x, a_d_y, a_d_z)); return nullptr; }
        inline const Cell_Type* at_ptr(const double& a_d_x, const double& a_d_y = 0.0, const double& a_d_z = 0.0) const
        { if (_check_position(a_d_x, a_d_y, a_d_z))return &(this->at(a_d_x, a_d_y, a_d_z)); return nullptr; }
        inline Cell_Type* at_ptr(const Position& a_pos) { return this->at_ptr(a_pos[0], a_pos[1], a_pos[2]); }
        inline const Cell_Type* at_ptr(const Position& a_pos) const
        { return this->at_ptr(a_pos[0], a_pos[1], a_pos[2]); }
        
        /*----------------------*/
        //实现移动构造函数和拷贝构造函数，来方便使用 vector 来存储
    public:
        linked_cell(const linked_cell& a_rhs);
        linked_cell(linked_cell&& a_rhs) noexcept;
        linked_cell& operator=(const linked_cell& a_rhs);
        linked_cell& operator=(linked_cell&& a_rhs) noexcept;
    };
    
/*----------------------*/
//自定义迭代器
// 直接移除 //

/*----------------------*/
//实现部分
    template<class Cell_Type>
    inline void linked_cell<Cell_Type>::_init_linked_cell() {
        m_database.clear(); //设置前先清空
        m_database.assign(m_size_x * m_size_y * m_size_z, Cell_Type());
        t_cell_links.clear();
        t_empty_links.clear();
    }
    
    template<class Cell_Type>
    void linked_cell<Cell_Type>::_set_link_cell(const int& a_x, const int& a_y, const int& a_z) {
        t_cell_links.clear();//push_back前先清空
        //设置周围link，顺序为self, x-+, y-+, z-+
        t_cell_links.push_back(cell_link(&m_database[index_1d(a_x, a_y, a_z)]));
        if (m_dimension >= 1) {
            _push_link_cell(a_x, a_y, a_z, -1);
            _push_link_cell(a_x, a_y, a_z, +1);
        }
        if (m_dimension >= 2) {
            for (int ofs_i = -1; ofs_i <= 1; ++ofs_i) {
                _push_link_cell(a_x, a_y, a_z, +ofs_i, -1);
            }
            for (int ofs_i = -1; ofs_i <= 1; ++ofs_i) {
                _push_link_cell(a_x, a_y, a_z, +ofs_i, +1);
            }
        }
        if (m_dimension >= 3) {
            for (int ofs_j = -1; ofs_j <= 1; ++ofs_j) {
                for (int ofs_i = -1; ofs_i <= 1; ++ofs_i) {
                    _push_link_cell(a_x, a_y, a_z, +ofs_i, +ofs_j, -1);
                }
            }
            for (int ofs_j = -1; ofs_j <= 1; ++ofs_j) {
                for (int ofs_i = -1; ofs_i <= 1; ++ofs_i) {
                    _push_link_cell(a_x, a_y, a_z, +ofs_i, +ofs_j, +1);
                }
            }
        }
    }
    
    template<class Cell_Type>
    void linked_cell<Cell_Type>::_push_link_cell(int a_x, int a_y, int a_z, const int& ofs_x, const int& ofs_y, const int& ofs_z) {
        double d_x = 0.0, d_y = 0.0, d_z = 0.0;
        bool t_mirror = false;
        
        if (m_link_type_x == LC_BOU::PBC) {
            t_mirror |= true;
            a_x += ofs_x;
            if (a_x >= m_size_x) {
                a_x -= m_size_x;
                d_x = m_length_x;
            } else if (a_x < 0) {
                a_x += m_size_x;
                d_x = -m_length_x;
            }
        } else if (m_link_type_x == LC_BOU::FBC) {
            a_x += ofs_x;
            if (a_x < 0 || a_x >= m_size_x) { return; }
        } else {
            if (ofs_x != 0) { return; }
        }
        
        if (m_link_type_y == LC_BOU::PBC) {
            t_mirror |= true;
            a_y += ofs_y;
            if (a_y >= m_size_y) {
                a_y -= m_size_y;
                d_y = m_length_y;
            } else if (a_y < 0) {
                a_y += m_size_y;
                d_y = -m_length_y;
            }
        } else if (m_link_type_y == LC_BOU::FBC) {
            a_y += ofs_y;
            if (a_y < 0 || a_y >= m_size_y) { return; }
        } else {
            if (ofs_y != 0) { return; }
        }
        
        if (m_link_type_z == LC_BOU::PBC) {
            t_mirror |= true;
            a_z += ofs_z;
            if (a_z >= m_size_z) {
                a_z -= m_size_z;
                d_z = m_length_z;
            } else if (a_z < 0) {
                a_z += m_size_z;
                d_z = -m_length_z;
            }
        } else if (m_link_type_z == LC_BOU::FBC) {
            a_z += ofs_z;
            if (a_z < 0 || a_z >= m_size_z) { return; }
        } else {
            if (ofs_z != 0) { return; }
        }
        
        t_cell_links.push_back(cell_link(&(this->operator()(a_x, a_y, a_z)), t_mirror, d_x, d_y, d_z, (ofs_x == 0 && ofs_y == 0 && ofs_z == 0)));
    }
    
/*----------------------*/
//设置尺寸，直接使用vector的resize函数
    template<class Cell_Type>
    void
    linked_cell<Cell_Type>::set_cell(const double& a_length_cell, const double& a_length_x, const double& a_length_y, const double& a_length_z) {
        m_length_cell = a_length_cell;
        m_length_x = a_length_x;
        m_length_y = a_length_y;
        m_length_z = a_length_z;
        m_size_x = (int) floor(m_length_x / m_length_cell);
        m_size_y = (int) floor(m_length_y / m_length_cell);
        m_size_z = (int) floor(m_length_z / m_length_cell);
        if (m_size_x < 1) m_size_x = 1;
        if (m_size_y < 1) m_size_y = 1;
        if (m_size_z < 1) m_size_z = 1;
        m_dimension = 3;
        if (m_size_z == 1) {
            --m_dimension;
            if (m_size_y == 1) {
                --m_dimension;
            }
        }
        m_database.resize(m_size_x * m_size_y * m_size_z, Cell_Type());
    }
    
//依据参数设置链接
    template<class Cell_Type>
    inline void linked_cell<Cell_Type>::set_link_parameter(const LC_BOU p1, const LC_BOU p2, const LC_BOU p3) {
        m_link_type_x = p1;
        m_link_type_y = p2;
        m_link_type_z = p3;
    }
    
    template<class Cell_Type>
    inline void linked_cell<Cell_Type>::index_3d(const int& a_i, int& x, int& y, int& z) const {
#ifdef CHANZY_DEBUG
        if (!_check_index_1d(a_i))
            throw Exception_container("index out of range");
#endif
        x = a_i % m_size_x;
        y = (a_i / m_size_x) % m_size_y;
        z = a_i / (m_size_x * m_size_y);
    }
    
    template<class Cell_Type>
    inline int linked_cell<Cell_Type>::index_1d(const int& a_x, const int& a_y, const int& a_z) const {
#ifdef CHANZY_DEBUG
        if (!_check_index_3d(a_x, a_y, a_z))
            throw Exception_container("index out of range");
#endif
        return (a_x + m_size_x * a_y + m_size_x * m_size_y * a_z);
    }
    
    template<class Cell_Type>
    inline void
    linked_cell<Cell_Type>::index_3d_at(const double& a_d_x, const double& a_d_y, const double& a_d_z, int& x, int& y,
                                        int& z) const {
#ifdef CHANZY_DEBUG
        if (!_check_position(a_d_x, a_d_y, a_d_z))
            throw Exception_container("index out of range");
#endif
        x = (int) floor(a_d_x / m_length_cell);
        if (x >= m_size_x) x = m_size_x - 1;
        y = (int) floor(a_d_y / m_length_cell);
        if (y >= m_size_y) y = m_size_y - 1;
        z = (int) floor(a_d_z / m_length_cell);
        if (z >= m_size_z) z = m_size_z - 1;
    }
    
    template<class Cell_Type>
    inline int
    linked_cell<Cell_Type>::index_1d_at(const double& a_d_x, const double& a_d_y, const double& a_d_z) const {
#ifdef CHANZY_DEBUG
        if (!_check_position(a_d_x, a_d_y, a_d_z))
            throw Exception_container("index out of range");
#endif
        int t_i = (int) floor(a_d_x / m_length_cell);
        if (t_i >= m_size_x) t_i = m_size_x - 1;
        int t_j = (int) floor(a_d_y / m_length_cell);
        if (t_j >= m_size_y) t_j = m_size_y - 1;
        int t_k = (int) floor(a_d_z / m_length_cell);
        if (t_k >= m_size_z) t_k = m_size_z - 1;
        return index_1d(t_i, t_j, t_k);
    }
    
    template<class Cell_Type>
    void linked_cell<Cell_Type>::clear_cell() {
        cell_iterator cell_it = this->begin();
        cell_iterator cell_it_end = this->end();
        for (; cell_it != cell_it_end; ++cell_it) {
            *cell_it = Cell_Type();
        }
        t_cell_links.clear();
        t_empty_links.clear();
    }
    
/*----------------------*/
    template<class Cell_Type>
    void linked_cell<Cell_Type>::print_cell() const {
        if (m_dimension == 0) {
            cout << "none" << endl;
        } else if (m_dimension == 1) {
            for (int i = 0; i < m_size_x; ++i) {
                cout << this->operator()(i, 0, 0) << " ";
            }
            cout << endl;
        } else if (m_dimension == 2) {
            for (int i = 0; i < m_size_x; ++i) {
                for (int j = 0; j < m_size_y; ++j) {
                    cout << this->operator()(i, j, 0) << " ";
                }
                cout << endl;
            }
        } else if (m_dimension == 3) {
            for (int k = 0; k < m_size_z; ++k) {
                cout << "z = " << k << endl;
                for (int i = 0; i < m_size_x; ++i) {
                    for (int j = 0; j < m_size_y; ++j) {
                        cout << this->operator()(i, j, k) << " ";
                    }
                    cout << endl;
                }
            }
        }
    }
    
    template<class Cell_Type>
    void linked_cell<Cell_Type>::print_link(const int& a_x, const int& a_y, const int& a_z) const {
        auto& t_links = this->get_links(a_x, a_y, a_z);
        for (cell_link t_link : t_links) {
            cout << *t_link << " ";
        }
        cout << endl;
        cout << "links capacity: " << t_cell_links.capacity() << endl;
    }
    
    template<class Cell_Type>
    void linked_cell<Cell_Type>::assign_increase() {
        Cell_Type t_sum = Cell_Type();
        cell_iterator t_it = this->begin();
        for (; t_it != this->end(); ++t_it) {
            *t_it = t_sum;
            ++t_sum;
        }
    }
    
/*----------------------*/
    template<class Cell_Type>
    inline linked_cell<Cell_Type>::linked_cell(
            const LC_BOU p1, const LC_BOU p2, const LC_BOU p3,
            const double& a_length_x, const double& a_length_y, const double& a_length_z) :
            m_dimension(0),
            m_size_x(0),
            m_size_y(0),
            m_size_z(0),
            m_link_type_x(p1),
            m_link_type_y(p2),
            m_link_type_z(p3),
            m_length_x(a_length_x),
            m_length_y(a_length_y),
            m_length_z(a_length_z) {
        m_length_cell = std::numeric_limits<double>::max();
    }
    
    template<class Cell_Type>
    inline linked_cell<Cell_Type>::linked_cell(const double& a_length_cell,
                                               const LC_BOU p1, const double& a_length_x) :
            m_length_cell(a_length_cell),
            m_dimension(1),
            m_size_x((int) floor(a_length_x / a_length_cell)),
            m_size_y(1),
            m_size_z(1),
            m_link_type_x(p1),
            m_link_type_y(LC_BOU::none),
            m_link_type_z(LC_BOU::none),
            m_length_x(a_length_x),
            m_length_y(0.0),
            m_length_z(0.0) {
        if (m_size_x < 1) m_size_x = 1;
        _init_linked_cell();
    }
    
    template<class Cell_Type>
    inline linked_cell<Cell_Type>::linked_cell(const double& a_length_cell,
                                               const LC_BOU p1, const double& a_length_x,
                                               const LC_BOU p2, const double& a_length_y) :
            m_length_cell(a_length_cell),
            m_dimension(2),
            m_size_x((int) floor(a_length_x / a_length_cell)),
            m_size_y((int) floor(a_length_y / a_length_cell)),
            m_size_z(1),
            m_link_type_x(p1),
            m_link_type_y(p2),
            m_link_type_z(LC_BOU::none),
            m_length_x(a_length_x),
            m_length_y(a_length_y),
            m_length_z(0.0) {
        if (m_size_x < 1) m_size_x = 1;
        if (m_size_y < 1) m_size_y = 1;
        _init_linked_cell();
    }
    
    template<class Cell_Type>
    inline linked_cell<Cell_Type>::linked_cell(const double& a_length_cell,
                                               const LC_BOU p1, const double& a_length_x,
                                               const LC_BOU p2, const double& a_length_y,
                                               const LC_BOU p3, const double& a_length_z) :
            m_length_cell(a_length_cell),
            m_dimension(3),
            m_size_x((int) floor(a_length_x / a_length_cell)),
            m_size_y((int) floor(a_length_y / a_length_cell)),
            m_size_z((int) floor(a_length_z / a_length_cell)),
            m_link_type_x(p1),
            m_link_type_y(p2),
            m_link_type_z(p3),
            m_length_x(a_length_x),
            m_length_y(a_length_y),
            m_length_z(a_length_z) {
        if (m_size_x < 1) m_size_x = 1;
        if (m_size_y < 1) m_size_y = 1;
        if (m_size_z < 1) m_size_z = 1;
        _init_linked_cell();
    }
    
    template<class Cell_Type>
    linked_cell<Cell_Type>::linked_cell() :
            m_dimension(0),
            m_size_x(0),
            m_size_y(0),
            m_size_z(0),
            m_link_type_x(LC_BOU::none),
            m_link_type_y(LC_BOU::none),
            m_link_type_z(LC_BOU::none),
            m_length_x(1.0),
            m_length_y(1.0),
            m_length_z(1.0) {
        m_length_cell = std::numeric_limits<double>::max();
    }
    
    template<class Cell_Type>
    linked_cell<Cell_Type>::linked_cell(const linked_cell& a_rhs) {
        m_database = a_rhs.m_database;
        m_dimension = a_rhs.m_dimension;
        m_size_x = a_rhs.m_size_x;
        m_size_y = a_rhs.m_size_y;
        m_size_z = a_rhs.m_size_z;
        m_link_type_x = a_rhs.m_link_type_x;
        m_link_type_y = a_rhs.m_link_type_y;
        m_link_type_z = a_rhs.m_link_type_z;
        m_length_x = a_rhs.m_length_x;
        m_length_y = a_rhs.m_length_y;
        m_length_z = a_rhs.m_length_z;
        m_length_cell = a_rhs.m_length_cell;
    }
    template<class Cell_Type>
    linked_cell<Cell_Type>::linked_cell(linked_cell&& a_rhs) noexcept {
        m_database.swap(a_rhs.m_database); // 这样应该就够了
        m_dimension = a_rhs.m_dimension;
        m_size_x = a_rhs.m_size_x;
        m_size_y = a_rhs.m_size_y;
        m_size_z = a_rhs.m_size_z;
        m_link_type_x = a_rhs.m_link_type_x;
        m_link_type_y = a_rhs.m_link_type_y;
        m_link_type_z = a_rhs.m_link_type_z;
        m_length_x = a_rhs.m_length_x;
        m_length_y = a_rhs.m_length_y;
        m_length_z = a_rhs.m_length_z;
        m_length_cell = a_rhs.m_length_cell;
    }
    template<class Cell_Type>
    linked_cell<Cell_Type>& linked_cell<Cell_Type>::operator=(const linked_cell& a_rhs) {
        m_database = a_rhs.m_database;
        m_dimension = a_rhs.m_dimension;
        m_size_x = a_rhs.m_size_x;
        m_size_y = a_rhs.m_size_y;
        m_size_z = a_rhs.m_size_z;
        m_link_type_x = a_rhs.m_link_type_x;
        m_link_type_y = a_rhs.m_link_type_y;
        m_link_type_z = a_rhs.m_link_type_z;
        m_length_x = a_rhs.m_length_x;
        m_length_y = a_rhs.m_length_y;
        m_length_z = a_rhs.m_length_z;
        m_length_cell = a_rhs.m_length_cell;
        return *this;
    }
    template<class Cell_Type>
    linked_cell<Cell_Type>& linked_cell<Cell_Type>::operator=(linked_cell&& a_rhs) noexcept {
        m_database.swap(a_rhs.m_database); // 这样应该就够了
        m_dimension = a_rhs.m_dimension;
        m_size_x = a_rhs.m_size_x;
        m_size_y = a_rhs.m_size_y;
        m_size_z = a_rhs.m_size_z;
        m_link_type_x = a_rhs.m_link_type_x;
        m_link_type_y = a_rhs.m_link_type_y;
        m_link_type_z = a_rhs.m_link_type_z;
        m_length_x = a_rhs.m_length_x;
        m_length_y = a_rhs.m_length_y;
        m_length_z = a_rhs.m_length_z;
        m_length_cell = a_rhs.m_length_cell;
        return *this;
    }
    
}