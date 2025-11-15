#pragma once


enum class CommitType {
    ACCEPT,
    READ,
    WRITE,
};

struct Buffer{

};

class NetItem;
struct CommitData {
    CommitType type;
    NetItem* netItem;
    Buffer buffer;
};

