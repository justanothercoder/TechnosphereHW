#include "allocator.h"
#include <cstring>

Pointer::Pointer() : allocator(nullptr), pointer_id(0) { }
Pointer::Pointer(const Allocator& a, size_t pointer_id) : allocator(&a), pointer_id(pointer_id) { }

void* Pointer::get() const {
    return pointer_id == 0 ? nullptr : allocator->pointers.at(pointer_id);
}

Allocator::Allocator(void* base, size_t size) : base((char*)base)
                                              , other_base(reinterpret_cast<char*>(base) + size/2)
                                              , size(size)
{
    blocks.push_back({(char*)base, size/2});
}

Pointer Allocator::alloc(size_t N)
{
    Pointer p(*this, ++last_id);

    size_t used_size = N;

    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->size >= used_size) {
            sizes[last_id] = used_size;
            if (it->size > used_size) {
                auto next = it;
                ++next;
                blocks.insert(next, {it->start + used_size, it->size - used_size});
            }
            
            pointers[last_id] = it->start;
            blocks.erase(it);
            return p;
        } 
    }
    
    throw AllocError(AllocErrorType::NoMemory, "No memory");
}

void Allocator::realloc(Pointer& p, size_t N)
{
    auto pointer = pointers[p.pointer_id];

    if (N <= sizes[p.pointer_id]) {
        size_t freed_size = sizes[p.pointer_id] - N;

        auto it = blocks.begin();
        while (it != blocks.end() && pointer + sizes[p.pointer_id] > it->start) {
            ++it;
        }

        if (pointer + sizes[p.pointer_id] == it->start) {
            it->start -= freed_size;
            it->size += freed_size;
        } else {
            blocks.insert(it, {pointer + N, freed_size});
        }

        sizes[p.pointer_id] = N;
        return;
    }

    auto it = blocks.begin();
    while (it != blocks.end() && pointer + sizes[p.pointer_id] > it->start) {
        ++it;
    }

    size_t used_size = N - sizes[p.pointer_id];
    if (pointer + sizes[p.pointer_id] == it->start && it->size >= used_size) {
        sizes[p.pointer_id] += used_size;
        if (it->size == used_size) {
            blocks.erase(it);
        } else {
            it->start += used_size;
            it->size -= used_size;
        }
        return;
    }

    Pointer q = alloc(N);
    std::memcpy(q.get(), p.get(), sizes[p.pointer_id]);
    free(p);

    p = q;
}

void Allocator::free(Pointer& p)
{
    if (pointers.find(p.pointer_id) == pointers.end()) {
        throw AllocError(AllocErrorType::InvalidFree, "Invalid free");
    }

    auto pointer = pointers[p.pointer_id];

    auto it = blocks.begin();
    while (it != blocks.end() && pointer + sizes[p.pointer_id] > it->start) {
        ++it;
    }
    blocks.insert(it, {pointer, sizes[p.pointer_id]});
    
    pointers.erase(p.pointer_id);
    sizes.erase(p.pointer_id);
    p.pointer_id = 0;
    
    merge_blocks();
}
    
void Allocator::defrag()
{
    std::swap(base, other_base);

    size_t offset = 0;
    for (auto& p : pointers) {
        if (p.first != 0) {
            std::memcpy(base + offset, p.second, sizes[p.first]);
            p.second = base + offset;
            offset += sizes[p.first];
        }
    }

    blocks = {{base + offset, size/2 - offset}};
}

void Allocator::merge_blocks()
{
    auto it = blocks.begin();
    while (it != blocks.end()) {
        do {
            auto next = it;
            ++next;

            if (next != blocks.end() && it->start + it->size == next->start) {
                it->size += next->size;
                blocks.erase(next);
            } else {
                break;
            }
        } while (true);
        ++it;
    }
}

size_t Allocator::last_id = 0;
