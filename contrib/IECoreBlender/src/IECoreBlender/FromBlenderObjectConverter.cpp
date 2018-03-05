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

#include "IECoreBlender/FromBlenderObjectConverter.h"

#include "IECore/MessageHandler.h"

extern "C" {
struct bContext;
struct bContext *BPy_GetContext(void);
}

using namespace IECoreBlender;
using namespace IECore;

IE_CORE_DEFINERUNTIMETYPED(FromBlenderObjectConverter);


FromBlenderObjectConverter::FromBlenderObjectConverter(const std::string &description, const BL::Object &object)
        : FromBlenderConverter(description), m_object(object), m_context(PointerRNA_NULL)
{
    PointerRNA context;
    RNA_id_pointer_create((::ID*)BPy_GetContext(), &context);
    m_context = BL::Context(context);

    IECore::StringParameter::PresetsContainer spacePresets;
    spacePresets.push_back(IECore::StringParameter::Preset("object", "object"));
    spacePresets.push_back(IECore::StringParameter::Preset("world", "world"));

    m_space = new IECore::StringParameter(
        "space",
        "Sets the space the object is in.",
        "object",
        spacePresets
    );

    parameters()->addParameter(m_space);
}


FromBlenderObjectConverterPtr FromBlenderObjectConverter::create(const BL::Object &object, IECore::TypeId resultType)
{
    const TypesToFnsMap &m = typesToFns();
    
    TypesToFnsMap::const_iterator it = m.find(Types(const_cast<BL::Object &>(object).type(), resultType));
    if (it!=m.end()) {
        return it->second(object);
    }
    
    // if not then see if the default converter is suitable
    DefaultConvertersMap &dc = defaultConverters();
    DefaultConvertersMap::const_iterator dcIt = dc.find(const_cast<BL::Object &>(object).type());
    if (dcIt != dc.end()) {
        // we only use the default converter if no result type has been specified, or if the requested type is a base class
        // of the type the default converter will create.
        if( resultType==IECore::InvalidTypeId || RunTimeTyped::inheritsFrom(dcIt->second->first.second, resultType)) {
            return dcIt->second->second(object);
        }
    }
    
    return 0;
}

void FromBlenderObjectConverter::registerConverter(BL::Object::type_enum fromType, IECore::TypeId resultType, bool defaultConversion, CreatorFn creator)
{    
    TypesToFnsMap &m = typesToFns();
    TypesToFnsMap::const_iterator it = m.insert(TypesToFnsMap::value_type(Types(fromType, resultType), creator)).first;
    if(defaultConversion) {
        DefaultConvertersMap &dc = defaultConverters();
        if(!dc.insert(DefaultConvertersMap::value_type(fromType, it)).second) {
            IECore::msg(IECore::Msg::Error, "FromBlenderObjectConverter::registerConverter", boost::format("Default conversion for BL::Object::type_enum %d already registered - ignoring second registration.") % fromType);
        }
    }
}

FromBlenderObjectConverter::TypesToFnsMap &FromBlenderObjectConverter::typesToFns()
{
    static TypesToFnsMap m;
    return m;
}

FromBlenderObjectConverter::DefaultConvertersMap &FromBlenderObjectConverter::defaultConverters()
{
    static DefaultConvertersMap m;
    return m;
}
