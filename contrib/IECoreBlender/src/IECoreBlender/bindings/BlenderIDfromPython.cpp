//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2014, Blender Foundation. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of the Blender Foundation nor the names of any
//       other contributors to this software may be used to endorse or
//       promote products derived from this software without specific prior
//       written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "IECoreBlender/Bindings.h"

#include "MEM_guardedalloc.h" // MEM_freeN
#include "RNA_blender_cpp.h"

using namespace boost::python;

namespace IECoreBlender
{

struct BlenderIDFromPython
{
    template <class T>
    BlenderIDFromPython &from_python()
    {
        converter::registry::push_back(&BlenderIDFromPython::convertible, &BlenderIDFromPython::construct<T>, type_id<T>());
        return *this;
    }

    static void *convertible(PyObject *obj)
    {
        if (PyLong_CheckExact(obj) && PyLong_AsVoidPtr(obj) != NULL) {
            return obj;
        }
        else {
            return 0;
        }
    }

    template <class T>
    static void construct(PyObject *obj, converter::rvalue_from_python_stage1_data *data)
    {
        void *storage = ((converter::rvalue_from_python_storage<T>*)data)->storage.bytes;

        PointerRNA bl_object;
        RNA_id_pointer_create((::ID*)PyLong_AsVoidPtr(obj), &bl_object);

        data->convertible = new (storage) T(bl_object);
    }
};

void bindBlenderIDFromPython()
{
    BlenderIDFromPython()
        .from_python<BL::Object>()
        //.from_python<BL::Context>()
    ;
}

} // namespace IECoreBlender
