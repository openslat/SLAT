#include "context.h"
#include <sstream>

namespace SLAT {
    std::shared_ptr<Context> Context::instance;
    omp_lock_t Context::lock;

    std::shared_ptr<Context> Context::GetInstance()
    {
        #pragma omp critical
        if (instance == NULL) {
            omp_init_lock(&Context::lock);
            instance = std::make_shared<Context>();
        }
        return instance;
    }

     std::shared_ptr<Context> Context::LockContext()
     {
        omp_set_lock(&Context::lock);
        std::shared_ptr<Context> instance = GetInstance();
        
        if (omp_get_active_level() > 0) {
            for (int i=0; i <= omp_get_active_level(); i++) {
                int thread_num = omp_get_ancestor_thread_num(i);
                if (instance->threads.size() == 0) {
                    instance->threads.resize(omp_get_team_size(i));
                }
                if (instance->threads[thread_num] == NULL) {
                    instance->threads[thread_num] = std::make_shared<Context>();
                }
                //std::cerr << instance << std::endl;
                std::shared_ptr<Context> temp = instance;
                temp = instance->threads[thread_num];
                instance = temp;//                instance = instance->threads[thread_num];
            }
        }
        return instance;
    }

    void ThreadPath(void)
    {
        std::stringstream msg;
        msg << "Thread Path: ";
        for (int i=0; i <= omp_get_active_level(); i++) {
            msg << omp_get_ancestor_thread_num(i) << " ";
        }
        std::cerr << msg.str() << std::endl;
    }
    
    void Context::DumpContext(void)
    {
        std::cerr << "-----------------------" << std::endl;
        ThreadPath();
        std::cerr << GetInstance()->Dump("");
        std::cerr << "-----------------------" << std::endl;
    }

    std::string Context::Dump(std::string prefix)
    {
        std::stringstream msg;
        prefix = prefix + "    ";
        msg << prefix;

        for (std::list<std::function<void (std::ostream &)>>::iterator i=this->text_stack.begin();
             i != this->text_stack.end();
             i++)
        {
            (*i)(msg);
            msg << " + ";
        }
        msg << std::endl;
        
        for (size_t i=0; i < this->threads.size(); i++) {
            if (this->threads[i] != NULL) {
                msg << prefix << "thread #" << i << ": " << this->threads[i]->Dump(prefix);
            } else {
                msg << prefix << "<thread #" << i << " empty>" << std::endl;
            }
        }
        return msg.str();
    }

    void Context::PushText(std::function<void (std::ostream &)>f)
    {
        std::shared_ptr<Context> instance = LockContext();        
        //DumpContext();
        instance->text_stack.push_back(f);
        instance->threads.resize(0);
        // {
        //     instance = Context::GetInstance();
        //     std::stringstream msg;
        //     msg << "PushText(" << text << "): ";
            
        //     for (int i=0; i <= omp_get_active_level(); i++) {
        //         int thread_num = omp_get_ancestor_thread_num(i);
        //         msg << thread_num << " ";
        //         instance = instance->threads[thread_num];
        //     }
        //     std::cerr << msg.str() << std::endl;
        // }
        // std::cerr << "PushText --> " << instance->text_stack.size() << std::endl;
        // {
        //     std::stringstream msg;
        //     msg << "PushText: ";
        //     f(msg);
        //     msg << std::endl;
        //     std::cerr << msg.str() << std::endl;
        // }
        //DumpContext();
        omp_unset_lock(&Context::lock);
    }
    
    void Context::PushText(std::string text)
    {
        PushText([text] (std::ostream &o) {
                o << text;
            });
    }
    
    void Context::PopText(void)
    {
        std::shared_ptr<Context> instance = LockContext();        
        //DumpContext();
        // {
        //     std::shared_ptr<Context> context = Context::GetInstance();
        //     std::stringstream msg;
        //     msg << "PopText(): ";
            
        //     for (int i=0; i <= omp_get_active_level(); i++) {
        //         int thread_num = omp_get_ancestor_thread_num(i);
        //         msg << thread_num << " ";
        //         context = context->threads[thread_num];
        //     }
        //     msg << " --> " << instance->text_stack.size();
        //     std::cerr << msg.str() << std::endl;
        // }
        
        if (instance->text_stack.size() > 0) {
            (void)instance->text_stack.pop_back();
            instance->threads.resize(0);
        } else { 
            std::cerr << "stack already empty" << std::endl;
        }
        omp_unset_lock(&Context::lock);
    }
    
    std::string Context::GetText() 
    {
        std::shared_ptr<Context> instance = GetInstance();        
        // ThreadPath();
        // DumpContext();

        std::string separator = "/";
        omp_set_lock(&Context::lock);
        std::stringstream result;
        // std::cerr << "GetText --> " << instance->text_stack.size() << std::endl;
        
        for (std::list<std::function<void (std::ostream &)>>::iterator i=instance->text_stack.begin();
             i != instance->text_stack.end();
             i++)
        {
            (*i)(result);
            result << separator;
        }

        for (int i=0; i <= omp_get_active_level(); i++) {
            separator = separator + "/";
            if (instance->threads.size() == 0) break;
            instance = instance->threads[omp_get_ancestor_thread_num(i)];
            if (instance == NULL) break;
            
            // std::cerr << "GetText -----> " << instance->text_stack.size() << std::endl;
            for (std::list<std::function<void (std::ostream &)>>::iterator j=instance->text_stack.begin();
                 j != instance->text_stack.end();
                 j++)
            {
                (*j)(result);
                result << separator;
            }
        }
        omp_unset_lock(&Context::lock);
        // std::cerr << "< GetText" << std::endl;
        //DumpContext();
        return result.str();
    }
    
}
