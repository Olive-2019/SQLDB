#pragma once
#include "../parser/parser_interp.h" // Value
#include "../storage/Subsystem1_Manager.h"
#include <vector>
#include <cassert>

class Tuple {
public:
    Tuple() = default;

    // data_����:���Ը���n + (�������� + ����ֵ)
    Tuple(string relname, std::vector<Value> values, const RID& rid) :rid_(rid) {
        size_ = 0;
        // ע�� �������valuesӦ�ú������ֵ��е�˳����һһ��Ӧ��
        Subsystem1_Manager sm;
        auto attrs = sm.lookup_Attrs(relname);
        for (int i = 0; i < values.size(); i++) {
            assert(attrs[i].type == values[i].type);
        }
        
        for (int i = 0; i < values.size(); i++)  size_ += attrs[i].Length;
        data_ = new char[size_];
        std::memset(data_, 0, size_);

        memcpy(data_, &size_, sizeof(uint32_t));
        uint32_t offset = sizeof(uint32_t);

        for (int i = 0; i < values.size(); i++) {
            memcpy(data_ + offset, values[i].data, attrs[i].Length);
            offset += attrs[i].Length;
        }
    }

    inline RID getRid() const { return rid_; }
    inline char* getData() const { return data_; }
    inline uint32_t getLength() const { return size_; }
    inline string getRel() const { return relname_; }

    void toRecord(char* record) const {
        memcpy(record, data_, size_);
    }

    // ��־������ר��
    void serializeTo(char* storage) const {
        memcpy(storage, &size_, sizeof(uint32_t));
        memcpy(storage + sizeof(uint32_t), data_, size_);
    }

    uint32_t deserializeFrom(const char* storage) { // ���ض������ֽ���
        uint32_t size = *reinterpret_cast<const uint32_t*>(storage);
        // Construct a tuple.
        if (this->data_) {
            delete[] this->data_;
        }
        this->size_ = size;
        this->data_ = new char[this->size_];
        memcpy(this->data_, storage + sizeof(uint32_t), this->size_);
        return this->size_;
    }

private:
    RID rid_{};
    uint32_t size_{ 0 };
    char* data_{ nullptr };
    string relname_;
};