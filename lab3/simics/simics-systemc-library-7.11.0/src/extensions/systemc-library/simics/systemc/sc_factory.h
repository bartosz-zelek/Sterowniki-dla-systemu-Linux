/*                                                             -*- C++ -*-

  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SC_FACTORY_H
#define SIMICS_SYSTEMC_SC_FACTORY_H

// The following two class_decorator inclusions must be before cc-api.h
#include <simics/systemc/class_decorator_include.h>
#include <simics/systemc/class_decorator.h>

#include <simics/cc-api.h>

#include <simics/systemc/module_loaded.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {

/** Utility class to help register a setup and teardown function for the
    SystemC top-model with Simics. */
/**
  Provides a convenient way to instantiate SystemC models in Simics.
  The user can register a setup and a teardown function that creates and
  destroys the simulation respectively. The RegisterModel instance will
  automatically define a Simics class that will, when instantiated, call the
  setup function and when it is deleted will call the teardown function. This
  will enable the SystemC model to be instantiated in Simics or a stand-alone
  simulation easily. The corresponding stand-alone simulation would look like
  this:

      sc_main(...) {
        T *result = setup(...);
        sc_start();
        teardown(result);
      }

  \internal
  Potential improvements:
  - Add an attribute to trigger teardown before deleting the simics object.
  - Make this agnostic of Simics so that it can be used also by stand-alone
    SC models (i.e., provide pre-cooked sc_main).
*/
class RegisterModel {
  public:
    template<typename SetupFun, typename TeardownFun>
    RegisterModel(const char *class_name, const char *class_desc,
                  const char *class_doc,
                  SetupFun setup, TeardownFun teardown) {
        addModelData(new ModelData<RegisterModel>(class_name, class_desc,
                                                  class_doc, setup, teardown));
    }

    template<typename SetupFun>
    RegisterModel(const char *class_name, const char *class_desc,
                  const char *class_doc, SetupFun setup) {
        addModelData(new ModelData<RegisterModel>(class_name, class_desc,
                                                  class_doc, setup));
    }

    template<typename TAdapter, typename TModelData>
    static void registerWithFramework() {
        std::vector<TModelData*>& models = getRegisteredModels<TModelData>();
        for (auto it = models.begin(); it != models.end(); ++it) {
            TModelData &model = **it;
            make_class<TAdapter>(model.name(), model.desc(), model.doc(),
                                 model.factory());
        }
    }

    /** \internal */
    class FactoryInterface {
      public:
        virtual ~FactoryInterface() {}
        virtual void setup(int argc, char *argv[]) = 0;
        virtual void teardown() = 0;
    };

    template<typename TBase>
    class ModelData {
      public:
        // Setup/teardown without return/argument
        ModelData(const char *class_name, const char *class_desc,
                  const char *class_doc,
                  void (*setup)(int, char **), void (*teardown)())
            : ModelData(class_name, class_desc, class_doc,
                        new FactoryVoid(setup, teardown)) {}

        // Only setup without return value, no teardown
        ModelData(const char *class_name, const char *class_desc,
                  const char *class_doc,
                  void (*setup)(int, char **))
            : ModelData(class_name, class_desc, class_doc,
                        new FactoryVoid(setup, nullptr)) {}

        // Setup/teardown with arbitrary return/argument pointer
        template<typename T>
        ModelData(const char *class_name, const char *class_desc,
                  const char *class_doc,
                  T *(*setup)(int, char **), void (*teardown)(T *))
            : ModelData(class_name, class_desc, class_doc,
                        new Factory<T>(setup, teardown)) {}

        // Setup only with arbitrary return pointer (needed?)
        template<typename T>
        ModelData(const char *class_name, const char *class_desc,
                  const char *class_doc,
                  T *(*setup)(int, char **))
            : ModelData(class_name, class_desc, class_doc,
                        new Factory<T>(setup, nullptr)) {}

        ~ModelData() {
            // Actually this will never be deleted as ModelData objects are
            // held in a vector throughout the simulation time
            delete factory_;
        }

        // Non-copyable
        ModelData(const ModelData &) = delete;
        const ModelData& operator=(const ModelData&) = delete;

        const std::string &name() const { return name_; }
        const std::string &desc() const { return desc_; }
        const std::string &doc() const { return doc_; }
        FactoryInterface *factory() const { return factory_; }

      private:
        ModelData(const char *class_name, const char *class_desc,
                  const char *class_doc, FactoryInterface *factory)
            : name_(class_name), desc_(class_desc), doc_(class_doc),
              factory_(factory) {}

        std::string name_;
        std::string desc_;
        std::string doc_;
        FactoryInterface *factory_;
    };

  protected:
    RegisterModel() {}
    template<typename TModelData>
    static void addModelData(TModelData *model_data) {
        getRegisteredModels<TModelData>().push_back(model_data);
#ifdef MODULE_NAME
        ModuleLoaded::set_module_name(MODULE_NAME);
#endif
    }

  private:
    template<typename T>
    class Factory : public FactoryInterface {
      public:
        typedef T* (*SetupFun)(int, char **);
        typedef void (*TeardownFun)(T *);

        Factory(SetupFun setup, TeardownFun teardown)
            : setup_(setup), teardown_(teardown), setup_data_(nullptr) {}

        // FactoryInterface
        void setup(int argc, char *argv[]) override {
            setup_data_ = setup_(argc, argv);
        }
        void teardown() override {
            if (teardown_) {
                teardown_(setup_data_);
                teardown_ = nullptr;
            }
        }

      private:
        SetupFun setup_;
        TeardownFun teardown_;
        T *setup_data_;
    };

    class FactoryVoid : public FactoryInterface {
      public:
        typedef void (*SetupFun)(int, char **);
        typedef void (*TeardownFun)();

        FactoryVoid(SetupFun setup, TeardownFun teardown)
            : setup_(setup), teardown_(teardown) {}

        // FactoryInterface
        void setup(int argc, char *argv[]) override {
            setup_(argc, argv);
        }
        void teardown() override {
            if (teardown_) {
                teardown_();
                teardown_ = nullptr;
            }
        }

      private:
        SetupFun setup_;
        TeardownFun teardown_;
    };

    template<typename TModelData>
    static std::vector<TModelData*>& getRegisteredModels() {
        static std::vector<TModelData*> models;
        return models;
    }
};

}  // namespace systemc
}  // namespace simics

#endif
