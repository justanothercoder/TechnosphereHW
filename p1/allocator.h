#include <stdexcept>
#include <string>
#include <list>
#include <map>

enum class AllocErrorType {
    InvalidFree,
    NoMemory,
};

class AllocError: std::runtime_error {
private:
    AllocErrorType type;

public:
    AllocError(AllocErrorType _type, std::string message):
            runtime_error(message),
            type(_type)
    {}

    AllocErrorType getType() const { return type; }
};

class Allocator;

class Pointer {
    friend class Allocator;
public:
    Pointer();
    void *get() const;

private:
    Pointer(const Allocator& a, size_t pointer_id);

    const Allocator* allocator = nullptr;
    size_t pointer_id;
};

class Allocator {
    friend class Pointer;
    static size_t last_id;
public:
    Allocator(void *base, size_t size);
    
    Pointer alloc(size_t N);
    void realloc(Pointer &p, size_t N);
    void free(Pointer &p);

    void defrag();
    std::string dump() { return ""; }

private:
    
    void merge_blocks();

    char* base;
    char* other_base;
    size_t size;

    struct Block {
        char* start;
        size_t size;
    };

    std::list<Block> blocks;
    std::map<size_t, char*> pointers;
    std::map<size_t, size_t> sizes;
};

