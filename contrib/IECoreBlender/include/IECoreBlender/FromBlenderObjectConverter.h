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

#ifndef IE_COREBLENDER_FROMBLENDEROBJECTCONVERTER_H
#define IE_COREBLENDER_FROMBLENDEROBJECTCONVERTER_H

#include "IECoreBlender/Export.h"
#include "IECoreBlender/FromBlenderConverter.h"

#include "IECore/Object.h"
#include "IECore/SimpleTypedParameter.h"
#include "IECore/CompoundParameter.h"

#include "MEM_guardedalloc.h" // MEM_freeN
#include "RNA_blender_cpp.h"

namespace IECoreBlender
{

IE_CORE_FORWARDDECLARE(FromBlenderObjectConverter);

class IECOREBLENDER_API FromBlenderObjectConverter : public FromBlenderConverter
{
public :

    IE_CORE_DECLARERUNTIMETYPEDEXTENSION(FromBlenderObjectConverter, FromBlenderObjectConverterTypeId, FromBlenderConverter);

    BL::Object &object() const
    {
        return const_cast<BL::Object &>(m_object);
    }

    BL::Context &context() const
    {
        return const_cast<BL::Context &>(m_context);
    }

    static FromBlenderObjectConverterPtr create(const BL::Object &object, IECore::TypeId resultType=IECore::InvalidTypeId);

protected :

    FromBlenderObjectConverter(const std::string &description, const BL::Object &object);

    virtual IECore::ObjectPtr doConversion(IECore::ConstCompoundObjectPtr operands) const
    {
        return doConversion(object(), context(), operands);
    }

    virtual IECore::ObjectPtr doConversion(BL::Object &object, BL::Context &context, IECore::ConstCompoundObjectPtr operands) const = 0;

    typedef FromBlenderObjectConverterPtr (*CreatorFn)(const BL::Object &object);

    static void registerConverter(BL::Object::type_enum fromType, IECore::TypeId resultType, bool defaultConversion, CreatorFn creator);

    /// Creating a static instance of one of these (templated on your Converter type)
    /// within your class will register your converter with the factory mechanism.
    template<class T>
    class IECOREBLENDER_API FromBlenderObjectConverterDescription
    {
    public :
        /// \param fromType The blender type which can be converted.
        /// \param resultType The cortex type which will result from the conversion.
        /// \param defaultConversion Should be true if this conversion is the "best" for a given fromType. If
        /// this is true then this is the converter that will be used when create() is called without specifying
        /// a resultType.
        FromBlenderObjectConverterDescription(BL::Object::type_enum fromType, IECore::TypeId resultType, bool defaultConversion)
        {
            FromBlenderObjectConverter::registerConverter(fromType, resultType, defaultConversion, creator);
            IECore::RunTimeTyped::registerType(T::staticTypeId(), T::staticTypeName(), T::baseTypeId());
        }

    private :
        static FromBlenderObjectConverterPtr creator(const BL::Object &object)
        {
            return new T(object);
        }
    };

private :
    BL::Object m_object;
    BL::Context m_context;

    IECore::StringParameterPtr m_space;

    typedef std::pair<BL::Object::type_enum, IECore::TypeId> Types;
    typedef std::map<Types, CreatorFn> TypesToFnsMap;
    typedef std::map<BL::Object::type_enum, TypesToFnsMap::const_iterator> DefaultConvertersMap;
        
    static TypesToFnsMap &typesToFns();
    static DefaultConvertersMap &defaultConverters();
};

} // namespace IECoreBlender

#endif // IE_COREBLENDER_FROMBLENDEROBJECTCONVERTER_H
