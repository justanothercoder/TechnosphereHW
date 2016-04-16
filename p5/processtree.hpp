#pragma once

#include <memory>
#include "command.hpp"

class ProcessTree
{
public:
    virtual int run() = 0;

    bool is_daemon = false;
};

using Tree = std::unique_ptr<ProcessTree>;

class LeafProcess : public ProcessTree
{
public:
    LeafProcess(Command command);
    int run() override;
private:
    Command command;
};

class PipeProcess : public ProcessTree
{
public:
    PipeProcess(Tree from, Tree to);
    int run() override;

private:
    Tree from;
    Tree to;
};

class AndProcess : public ProcessTree
{
public:
    AndProcess(Tree left, Tree right);
    int run() override;

private:
    Tree left;
    Tree right;
};

class OrProcess : public ProcessTree
{
public:
    OrProcess(Tree left, Tree right);
    int run() override;

private:
    Tree left;
    Tree right;
};
