/*
  © 2017 Intel Corporation
*/

/* 	 
  empty-interface-interface.h - Skeleton code to base new interface modules on
*/
	 
/* This module is a template for defining new interface types. See the
   "Defining New Interface Types" section of the
   "Simics Model Builder User's Guide" for further documentation.

   The corresponding DML definition can be found in empty-interface_interface.dml */

#ifndef EMPTY_INTERFACE_INTERFACE_H
#define EMPTY_INTERFACE_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* If you need to define new struct types, a definition like this will make it
   possible to allocate such structs from Python using empty_interface_data_t().

   Before doing this, you will have to load the empty-interface_interface
   Simics module, and import the empty_interface_interface Python module. */
typedef struct {
        int level;
        char *name;
} empty_interface_data_t;
SIM_PY_ALLOCATABLE(empty_interface_data_t);

/* This defines a new interface type. Its corresponding C data type will be
   called "empty_interface_interface_t". */
SIM_INTERFACE(empty_interface) {
        bool (*example_method)(conf_object_t *obj, int id,
                               empty_interface_data_t *data);
#ifndef PYWRAP
        /* methods that cannot be exported to Python, for example as it refers
           to unknown data types, must be enclosed by "#ifndef PYWRAP" ...
           "#endif". See the "Restrictions" subsection of "Defining New
           Interface Types" mentioned above. */
        void (*no_python_method)(conf_object_t *obj, size_t bufsize, void *buf);
#endif
};

/* Use a #define like this whenever you need to use the name of the interface
   type; the C compiler will then catch any typos at compile-time. */
#define EMPTY_INTERFACE_INTERFACE "empty_interface"

#ifdef __cplusplus
}
#endif

#endif /* ! EMPTY_INTERFACE_INTERFACE_H */
