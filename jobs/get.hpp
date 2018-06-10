#ifndef GET_H
#define GET_H

#include "job.hpp"
#include "../persistence/cache.hpp"

class Get : public Job {
public:
    Get() : Job() { }
    ~Get();

    void execute();

private:
    bool parse(std::string *error);
    std::string id;
};

#endif // GET_H
