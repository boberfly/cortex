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

#ifndef IE_COREBLENDER_FROMBLENDERCAMERACONVERTER_H
#define IE_COREBLENDER_FROMBLENDERCAMERACONVERTER_H

#include "IECoreBlender/Export.h"

#include "FromBlenderObjectConverter.h"

#include "IECore/TypedParameter.h"
#include "IECore/NumericParameter.h"
#include "IECore/SimpleTypedParameter.h"

namespace IECoreBlender
{

class IECOREBLENDER_API FromBlenderCameraConverter : public FromBlenderObjectConverter
{
public :

    IE_CORE_DECLARERUNTIMETYPEDEXTENSION(FromBlenderCameraConverter, FromBlenderCameraConverterTypeId, FromBlenderObjectConverter);

    FromBlenderCameraConverter(const BL::Object &object);

protected :

    virtual IECore::ObjectPtr doConversion(BL::Object &object, BL::Context &context, IECore::ConstCompoundObjectPtr operands) const;

private :

    enum ResolutionMode
    {
        Invalid = 0,
        RenderGlobals = 1,
        Specified = 2,
    };

    IECore::IntParameterPtr m_resolutionMode;
    IECore::V2iParameterPtr m_resolution;

    static FromBlenderObjectConverterDescription<FromBlenderCameraConverter> m_description;
};

IE_CORE_DECLAREPTR(FromBlenderCameraConverter)

} // namespace IECoreBlender

#endif // IE_COREBLENDER_FROMBLENDERCAMERACONVERTER_H
