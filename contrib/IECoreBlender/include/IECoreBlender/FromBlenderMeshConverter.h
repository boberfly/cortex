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

#ifndef IE_COREBLENDER_FROMBLENDERMESHCONVERTER_H
#define IE_COREBLENDER_FROMBLENDERMESHCONVERTER_H

#include "IECoreBlender/Export.h"
#include "IECoreBlender/FromBlenderMeshPointsConverter.h"

#include "IECore/TypedParameter.h"
#include "IECore/NumericParameter.h"
#include "IECore/SimpleTypedParameter.h"
#include "IECore/VectorTypedData.h"

namespace IECoreBlender
{

class IECOREBLENDER_API FromBlenderMeshConverter : public FromBlenderMeshPointsConverter
{
public :

    IE_CORE_DECLARERUNTIMETYPEDEXTENSION(FromBlenderMeshConverter, FromBlenderMeshConverterTypeId, FromBlenderMeshPointsConverter);

    FromBlenderMeshConverter(const BL::Object &object);

protected :

    virtual IECore::ObjectPtr doConversion(BL::Object &object, BL::Context &context, IECore::ConstCompoundObjectPtr operands) const;

    IECore::V3fVectorDataPtr convertNormals(BL::Mesh &mesh) const;
    void convertUvs(BL::MeshTextureFaceLayer &UVlayer, IECore::IntVectorDataPtr faceLenData, IECore::FloatVectorDataPtr sData, IECore::FloatVectorDataPtr tData) const;

private :

    IECore::StringParameterPtr m_interpolation;
    IECore::BoolParameterPtr m_points;
    IECore::BoolParameterPtr m_normals;
    IECore::BoolParameterPtr m_st;
    IECore::BoolParameterPtr m_extraST;
    IECore::BoolParameterPtr m_vcolors;

    static FromBlenderObjectConverterDescription<FromBlenderMeshConverter> m_description;
};

IE_CORE_DECLAREPTR(FromBlenderMeshConverter)

} // namespace IECoreBlender

#endif // IE_COREBLENDER_FROMBLENDERMESHCONVERTER_H
