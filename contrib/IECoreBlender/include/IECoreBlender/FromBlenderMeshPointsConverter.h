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

#ifndef IE_COREBLENDER_FROMBLENDERMESHPOINTSCONVERTER_H
#define IE_COREBLENDER_FROMBLENDERMESHPOINTSCONVERTER_H

#include "IECoreBlender/Export.h"
#include "IECoreBlender/FromBlenderObjectConverter.h"

#include "IECore/TypedParameter.h"
#include "IECore/NumericParameter.h"
#include "IECore/SimpleTypedParameter.h"
#include "IECore/VectorTypedData.h"

namespace IECoreBlender
{

class IECOREBLENDER_API FromBlenderMeshPointsConverter : public FromBlenderObjectConverter
{
public :

    IE_CORE_DECLARERUNTIMETYPEDEXTENSION(FromBlenderMeshPointsConverter, FromBlenderMeshPointsConverterTypeId, FromBlenderObjectConverter);

    FromBlenderMeshPointsConverter(const BL::Object &object)
        : FromBlenderObjectConverter("Converts Blender meshes to IECore::PointsPrimitive objects.", object)
    {
        if (const_cast<BL::Object &>(object).type() != BL::Object::type_MESH) {
            throw IECore::Exception("Object isn't a Blender Mesh type");
        }
        constructParameters();
    }

protected :

    FromBlenderMeshPointsConverter(const std::string &description, const BL::Object &object)
        : FromBlenderObjectConverter(description, object)
    {
        constructParameters();
    }

    virtual IECore::ObjectPtr doConversion(BL::Object &object, BL::Context &context, IECore::ConstCompoundObjectPtr operands) const;

    BL::Mesh objectToMesh(BL::Object &object, BL::Context &context, IECore::ConstCompoundObjectPtr operands) const;

    IECore::V3fVectorDataPtr convertPoints(BL::Mesh &mesh) const;

private :

    void constructParameters();

    enum modifierSettings
    {
        Preview = 0,
        Render = 1,
    };

    IECore::IntParameterPtr m_modifierSettings;
    IECore::BoolParameterPtr m_applyModifiers;
    IECore::BoolParameterPtr m_calcTessface;
    IECore::BoolParameterPtr m_calcUndeformed;

    static FromBlenderObjectConverterDescription<FromBlenderMeshPointsConverter> m_description;
};

IE_CORE_DECLAREPTR(FromBlenderMeshPointsConverter)

} // namespace IECoreBlender

#endif // IE_COREBLENDER_FROMBLENDERMESHPOINTSCONVERTER_H
