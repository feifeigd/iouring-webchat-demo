#pragma once

#include "net/Buffer.h"

enum class CommitType {
    ACCEPT,
    READ,
    WRITE,

    CANCEL,
    CLOSE,
};


class NetItem;
struct CommitData {
    CommitType type;
    NetItem* netItem;
    Buffer buffer;
};

