#ifndef MOE_NAMED_TYPE_H_
#define MOE_NAMED_TYPE_H_

// A slightly modified Jonathan Boccara named type implementation.
// cf. http://www.fluentcpp.com/2016/12/08/strong-types-for-strong-interfaces/
namespace moe
{
    template <typename T, typename Phantom>
    class NamedType
    {
    public:
        explicit NamedType(const T& val) :
            m_value(val)
        {}
        explicit NamedType(T&& val) :
            m_value(val)
        {}

        operator T&()
        {
            return m_value;
        }

        operator const T&() const
        {
            return m_value;
        }

        bool operator==(const NamedType& other)
        {
            return m_value == other.m_value;
        }

    private:
        T   m_value;
    };

}


#endif // MOE_NAMED_TYPE_H_
