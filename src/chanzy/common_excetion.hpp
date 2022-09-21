#pragma once
#ifdef _MSC_VER
#pragma warning(disable: 4577)
#pragma warning(disable: 4530)
#pragma warning(disable: 4514)
#pragma warning(disable: 4061)
#endif

#include <exception>
#include <string>
#include <utility>

namespace chanzy {
    
    
    using std::exception;
    using std::string;
    
    struct Exception_CH : public exception {
        Exception_CH() {
            m_massage = "CHanzy common exception";
        }
        
        explicit Exception_CH(const char* a_massage) {
            m_massage.clear();
            m_massage += "CHanzy common exception: ";
            m_massage += a_massage;
        }
        
        const char* what() const noexcept override {
            return m_massage.c_str();
        }
    
    protected:
        string m_massage; // NOLINT(misc-non-private-member-variables-in-classes)
    };
    
    struct Exception_container : public Exception_CH {
        
        Exception_container() {
            m_massage = "CHanzy container exception";
        }
        
        explicit Exception_container(const string& a_massage) {
            m_massage.clear();
            m_massage += "CHanzy container exception: ";
            m_massage += a_massage;
        }
        
    };
    
    struct Exception_parameter : public Exception_CH {
        
        Exception_parameter() {
            m_massage = "CHanzy parameter exception";
        }
        
        explicit Exception_parameter(const string& a_massage) {
            m_massage.clear();
            m_massage += "CHanzy parameter exception: ";
            m_massage += a_massage;
        }
    };
}
