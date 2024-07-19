// Copyright [2024] <CRA/BestReviewer>
#pragma once
#include<string>
#define interface struct

interface SSDInterface {
    virtual void Write(const int &LBA, const std::string &data) = 0;
    virtual void Read(const int &LBA) = 0;
    virtual void Erase(const int &LBA, const int &size) = 0;
    virtual void Flush() = 0;
};
