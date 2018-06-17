#ifndef DELETE_H
#define DELETE_H

#include "job.hpp"
#include "../persistence/cache.hpp"

class Delete : public Job {
public:
    Delete() : Job() { }
    ~Delete();

    void execute();

private:
    bool parse(std::string *error);
    std::string id;
};

#endif // DELETE_H
