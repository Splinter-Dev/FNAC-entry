#include <functional>
#include <raylib.h>

enum class AttributeType { 
    Short,
    Int, 
    Float, 
    Bool, 
    String,
    Vec3,
};

struct Entity;

struct Attribute {
    const char* name;
    AttributeType type;
    std::function<void*(Entity&)> getPtr;
};

template<typename T, typename FieldT>
Attribute MakeAttribute(const char* name,
                        FieldT T::* member,
                        AttributeType type)
{
    return {
        name,
        type,
        [member](Entity& e) -> void* {
            T& obj = static_cast<T&>(e);
            return &(obj.*member);
        }
    };
}
