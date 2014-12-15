//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010-2011, Image Engine Design Inc. All rights reserved.
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
//     * Neither the name of Image Engine Design nor the names of any
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

#ifndef IECORE_ENVMAPSHPROJECTOR_H
#define IECORE_ENVMAPSHPROJECTOR_H

#include "IECore/Export.h"
#include "IECore/TypeIds.h"

#include "IECore/Op.h"
#include "IECore/SphericalHarmonicsProjector.h"
#include "IECore/SimpleTypedParameter.h"
#include "IECore/NumericParameter.h"

namespace IECore
{

IE_CORE_FORWARDDECLARE( ObjectParameter );

/// Projects a Lat-Long environment map on a SphericalHarmonics object and returns the resulting SH coefficients.
/// \ingroup shGroup
class IECORE_API EnvMapSHProjector : public Op
{
	public :

		IE_CORE_DECLARERUNTIMETYPEDEXTENSION( EnvMapSHProjector, EnvMapSHProjectorTypeId, Op );

		EnvMapSHProjector();
		
		virtual ~EnvMapSHProjector();

	protected :

		virtual ObjectPtr doOperation( const CompoundObject * operands );

	private :
	
		M44fParameterPtr m_orientationParameter;
		IntParameterPtr m_bandsParameter;
		IntParameterPtr m_samplesParameter;
		BoolParameterPtr m_applyFilterParameter;
		BoolParameterPtr m_rightHandSystemParameter;
		ObjectParameterPtr m_envMapParameter;
};

IE_CORE_DECLAREPTR( EnvMapSHProjector );

} // namespace IECore

#endif // IECORE_ENVMAPSHPROJECTOR_H
