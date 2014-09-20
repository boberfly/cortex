//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007-2011, Image Engine Design Inc. All rights reserved.
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

#ifndef IECORE_REALSPHERICALHARMONICFUNCTION_H
#define IECORE_REALSPHERICALHARMONICFUNCTION_H

#include "boost/static_assert.hpp"
#include "boost/type_traits.hpp"
#include "boost/function.hpp"
#include <vector>

#include "IECore/IECoreExport.h"

namespace IECore
{

/// Class for computing Real Spherical Harmonics functions
/// Based mainly on "Spherical Harmonic Lighting: The Gritty Details" by Robin Green.
/// \ingroup shGroup
template < typename V >
class IECORE_EXPORT RealSphericalHarmonicFunction
{
	public :

		BOOST_STATIC_ASSERT( boost::is_floating_point<V>::value );

		// compute the real harmonic function for the given band l and parameter m at the spherical coordinates theta and phi.
		// l is in the range [0,MAX_BAND]
		// m is in the range [-l,l]
		// phi is in the range [0,2*pi]
		// theta is in the range [0,pi]
		static V evaluate( V phi, V theta, unsigned int l, int m );

		// computes all the bands for the given spherical coordinates and stores the results on the given array.
		// the order in the array follows: index = l*(l+1)+m where l is [0,bands-1] and m[-l,l].
		static void evaluate( V phi, V theta, unsigned int bands, std::vector<V> &result );

		// computes all the bands for the given spherical coordinates and calls the given functor passing l,m and the computed value.
		static void evaluate( V phi, V theta, unsigned int bands, boost::function< void ( unsigned int, int, V ) > functor );

	private :

		static V evaluateFromLegendre( V phi, unsigned int l, int m, double legendreEval );
};

} // namespace IECore

#include "RealSphericalHarmonicFunction.inl"

#endif // IECORE_REALSPHERICALHARMONICFUNCTION_H
