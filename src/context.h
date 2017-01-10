#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <vector>
#include <list>
#include <memory>
#include <iostream>
#include <omp.h>

namespace SLAT {
    class Context {
    public:
        static void Initialise();
        static std::string GetText();
        static void PushText(std::string text);
        static void PushText(std::function<void (std::ostream &)>);;
        static void PopText(void);
        static void DumpContext(void);
    private:
        std::string Dump(std::string prefix);
        static std::shared_ptr<Context> GetContext();
        static std::shared_ptr<Context> LockContext();
        static std::shared_ptr<Context> GetInstance();
        std::list<std::function<void (std::ostream &)>> text_stack;
        std::vector<std::shared_ptr<Context>> threads;
        static std::shared_ptr<Context> instance;
        static omp_lock_t lock;
    };

    class TempContext {
    public:
        TempContext(std::string text) {
            Context::PushText(text);
        };

        TempContext(std::function<void (std::ostream &)>f) {
            Context::PushText(f);
        };

        ~TempContext() {
            Context::PopText();
        };
    };
}
#endif
