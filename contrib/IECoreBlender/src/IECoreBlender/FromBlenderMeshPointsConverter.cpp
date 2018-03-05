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

#include "IECoreBlender/FromBlenderMeshPointsConverter.h"
#include "IECoreBlender/Convert.h"

#include "IECore/CompoundParameter.h"
#include "IECore/PointsPrimitive.h"

using namespace IECoreBlender;

IE_CORE_DEFINERUNTIMETYPED(FromBlenderMeshPointsConverter);

FromBlenderObjectConverter::FromBlenderObjectConverterDescription<FromBlenderMeshPointsConverter>
    FromBlenderMeshPointsConverter::m_description(BL::Object::type_MESH, IECore::PointsPrimitiveTypeId, false);

void FromBlenderMeshPointsConverter::constructParameters()
{
    IECore::IntParameter::PresetsContainer modifierSettingsPresets;
    modifierSettingsPresets.push_back(IECore::IntParameter::Preset("preview", Preview));
    modifierSettingsPresets.push_back(IECore::IntParameter::Preset("render", Render));

    m_modifierSettings = new IECore::IntParameter(
        "modifierSettings",
        "Modifier settings to apply.",
        Render,
        Preview,
        Render,
        modifierSettingsPresets,
        true
    );
    parameters()->addParameter(m_modifierSettings);

    m_applyModifiers = new IECore::BoolParameter("applyModifiers", "Apply Modifiers", true);
    parameters()->addParameter(m_applyModifiers);

    m_calcTessface = new IECore::BoolParameter("calcTessface", "Calculate tessalated faces", true);
    parameters()->addParameter(m_calcTessface);

    m_calcUndeformed = new IECore::BoolParameter("calcUndeformed", "Calculate undeformed vertex coordinates", false);
    parameters()->addParameter(m_calcUndeformed);
}

IECore::V3fVectorDataPtr FromBlenderMeshPointsConverter::convertPoints(BL::Mesh &mesh) const
{
    IECore::V3fVectorDataPtr pointsData = new IECore::V3fVectorData;
    pointsData->setInterpretation(IECore::GeometricData::Point);
    std::vector<Imath::V3f> &points = pointsData->writable();
    points.reserve(mesh.vertices.length());

    BL::Mesh::vertices_iterator v;
    for (mesh.vertices.begin(v); v != mesh.vertices.end(); ++v) {
        points.push_back(IECore::convert<Imath::V3f>(v->co()));
    }

    return pointsData;
}

BL::Mesh FromBlenderMeshPointsConverter::objectToMesh(BL::Object &object, BL::Context &context, IECore::ConstCompoundObjectPtr operands) const
{
    BL::Mesh mesh = object.to_mesh(context.scene(),
                                   operands->member<IECore::BoolData>("applyModifiers")->readable(),
                                   operands->member<IECore::IntData>("modifierSettings")->readable(),
                                   operands->member<IECore::BoolData>("calcTessface")->readable(),
                                   operands->member<IECore::BoolData>("calcUndeformed")->readable());

    if (operands->member<IECore::StringData>("space")->readable() == "world") {
        mesh.transform(object.matrix_world());
    }

    return mesh;
}

IECore::ObjectPtr FromBlenderMeshPointsConverter::doConversion(BL::Object &object, BL::Context &context, IECore::ConstCompoundObjectPtr operands) const
{
    BL::Mesh mesh = objectToMesh(object, context, operands);

    IECore::PointsPrimitivePtr points = new IECore::PointsPrimitive(convertPoints(mesh));

    context.blend_data().meshes.remove(mesh);

    return points;
}
