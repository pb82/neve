#ifndef UPDATE_H
#define UPDATE_H

#include "job.hpp"
#include "../json/parser.hpp"
#include "../json/printer.hpp"
#include "../actions/compiler.hpp"
#include "../persistence/cache.hpp"

/**
 * @brief The Update class
 * Handles creation of new actions. Parses the request payload and
 * compiles the source code to bytecode
 */
class Update : public Job {
public:
    Update() : Job() { }
    ~Update();

    void execute();

private:
    JSON::Parser parser;
};

#endif // UPDATE_H
