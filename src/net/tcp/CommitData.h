#pragma once


enum class CommitType {
    ACCEPT,
    READ,
    WRITE,

    CANCEL,
    CLOSE,
};

struct Buffer{

};

class NetItem;
struct CommitData {
    CommitType type;
    NetItem* netItem;
    Buffer buffer;
};

