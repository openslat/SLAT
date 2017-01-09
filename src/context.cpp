#include "context.h"
#include <sstream>

namespace SLAT {
    std::shared_ptr<Context> Context::instance;
    omp_lock_t Context::lock;

    void Context::Initialise()
    {
        if (instance == NULL) {
            omp_init_lock(&Context::lock);
            instance = std::make_shared<Context>();
        }
    }

    std::shared_ptr<Context> Context::GetInstance()
    {
        if (instance == NULL) {
            omp_init_lock(&Context::lock);
            instance = std::make_shared<Context>();
        }
        return instance;
    }

     std::shared_ptr<Context> Context::LockContext()
     {
        std::shared_ptr<Context> instance = GetInstance();
        omp_set_lock(&Context::lock);
        for (int i=0; i <= omp_get_active_level(); i++) {
            int thread_num = omp_get_ancestor_thread_num(i);
            if (instance->threads.size() == 0) {
                instance->threads.resize(omp_get_num_threads());
            }
            if (instance->threads[thread_num] == NULL) {
                instance->threads[thread_num] = std::make_shared<Context>();
            }
            instance = instance->threads[thread_num];
        }
        return instance;
    }

    
    void Context::DumpContext(void)
    {
        std::cerr << GetInstance()->Dump("");
    }

    std::string Context::Dump(std::string prefix)
    {
        std::stringstream msg;
        msg << "Dump:";
        prefix = prefix + "    ";
        msg << prefix;

        for (std::list<std::string>::iterator i=this->text_stack.begin();
             i != this->text_stack.end();
             i++)
        {
            msg << *i << " + ";
        }
        msg << std::endl;
        
        for (size_t i=0; i < this->threads.size(); i++) {
            if (this->threads[i] != NULL) {
                msg << this->threads[i]->Dump(prefix);
            } else {
                msg << prefix << "<thread #" << i << " empty>" << std::endl;
            }
        }
        return msg.str();
    }
    
    std::shared_ptr<Context> Context::GetContext()
     {
         std::shared_ptr<Context> instance = LockContext();
         omp_unset_lock(&Context::lock);
         return instance;
    }

    void Context::PushText(std::string text)
    {
        std::shared_ptr<Context> instance = LockContext();        
        //DumpContext();
        instance->text_stack.push_back(text);
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
        omp_unset_lock(&Context::lock);
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

        omp_set_lock(&Context::lock);
        std::stringstream result;
        
        for (std::list<std::string>::iterator i=instance->text_stack.begin();
             i != instance->text_stack.end();
             i++)
        {
            result << *i << "#";
        }


        for (int i=0; i <= omp_get_active_level(); i++) {
            instance = instance->threads[i];
            if (instance == NULL) break;
            
            for (std::list<std::string>::iterator j=instance->text_stack.begin();
                 j != instance->text_stack.end();
                 j++)
            {
                result << *j << "#";
            }
        }
        omp_unset_lock(&Context::lock);
        return result.str();
    }
    
}
