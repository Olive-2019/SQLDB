#pragma once
#include "../parser/parser_interp.h" // Value
#include "../storage/Subsystem1_Manager.h"
#include <vector>
#include <cassert>

class Tuple {
public:
    Tuple() = default;

    // data_构成:属性个数n + (属性类型 + 属性值)
    Tuple(string relname, std::vector<Value> values, const RID& rid)
        : relname_(relname), rid_(rid) {
        size_ = 0;
        // 注意 到这里的values应该和数据字典中的顺序是一一对应的

        auto attrs = Subsystem1_Manager::mgr.lookup_Attrs(relname);

        for (int i = 0; i < values.size(); i++) {
            assert(attrs[i].type == values[i].type);
            size_ += attrs[i].Length;
        }

        data_ = new char[size_];
        std::memset(data_, 0, size_);
        int32_t offset = 0;
        for (int i = 0; i < values.size(); i++) {
            std::memcpy(data_ + offset, (char*)values[i].data, attrs[i].Length);
            offset += attrs[i].Length;
        }      
    }

    int Type2Len(AttrType type) {
        int res = 0;
        if (type == INT) res = sizeof(int);
        if (type == FLOAT) res = sizeof(float);
        if (type == STRING) res = 20; // 根据storage的Test
        return res;
    }

    void setRid(RID rid) { rid_ = rid; }
    void setRel(string rel) { relname_ = rel; }

    inline RID getRid() const { return rid_; }
    inline char* getData() const { return data_; }
    inline uint32_t getLength() const { return size_; }
    inline string getRel() const { return relname_; }

    // 日志管理器专用
    void serializeTo(char* storage) const {
        memcpy(storage, &size_, sizeof(int32_t));
        memcpy(storage + sizeof(int32_t), data_, size_);
    }

    int32_t deserializeFrom(const char* storage) { // 返回读出的字节数
        int32_t size = *reinterpret_cast<const int32_t*>(storage);
        // Construct a tuple.
        if (this->data_) {
            delete[] this->data_;
        }
        this->size_ = size;
        this->data_ = new char[this->size_];
        memcpy(this->data_, storage + sizeof(int32_t), this->size_);
        return this->size_ + sizeof(int32_t);
    }

private:
    RID rid_{};
    int32_t size_{ 0 };
    char* data_{ nullptr };
    string relname_;
};