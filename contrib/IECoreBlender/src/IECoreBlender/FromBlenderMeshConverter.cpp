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

#include "IECoreBlender/FromBlenderMeshConverter.h"
#include "IECoreBlender/Convert.h"

#include "IECore/CompoundParameter.h"
#include "IECore/MeshPrimitive.h"

using namespace IECoreBlender;

IE_CORE_DEFINERUNTIMETYPED(FromBlenderMeshConverter);

FromBlenderObjectConverter::FromBlenderObjectConverterDescription<FromBlenderMeshConverter>
    FromBlenderMeshConverter::m_description(BL::Object::type_MESH, IECore::MeshPrimitiveTypeId, true);

FromBlenderMeshConverter::FromBlenderMeshConverter(const BL::Object &object)
    :    FromBlenderMeshPointsConverter("Converts Blender mesh into IECore::MeshPrimitive object.", object)
{
    if (const_cast<BL::Object &>(object).type() != BL::Object::type_MESH) {
        throw IECore::Exception("Object isn't a Blender Mesh type");
    }

    IECore::StringParameter::PresetsContainer interpolationPresets;
    interpolationPresets.push_back(IECore::StringParameter::Preset("poly", "linear"));
    interpolationPresets.push_back(IECore::StringParameter::Preset("subdiv", "catmullClark"));

    m_interpolation = new IECore::StringParameter(
        "interpolation",
        "Sets the interpolation type of the new mesh.",
        "subdiv",
        interpolationPresets
    );
    parameters()->addParameter(m_interpolation);

    m_points = new IECore::BoolParameter("points", "Add mesh points as a primitive variable named \"P\".", true);
    parameters()->addParameter(m_points);

    m_normals = new IECore::BoolParameter("normals", "Add mesh normals as a primitive variable named \"N\".", true);
    parameters()->addParameter(m_normals);

    m_st = new IECore::BoolParameter("st", "Add UVs as primitive variables named \"s\" and \"t\".", true);
    parameters()->addParameter(m_st);

    m_extraST = new IECore::BoolParameter("extraST", "Add all uv sets as primitive variables named \"setName_s\" and \"setName_t\".", true);
    parameters()->addParameter(m_extraST);

    m_vcolors = new IECore::BoolParameter("colors", "Add vertex colors as a primitive variable named \"Cs\".", false);
    parameters()->addParameter(m_vcolors);
}

IECore::V3fVectorDataPtr FromBlenderMeshConverter::convertNormals(BL::Mesh &mesh) const
{
    IECore::V3fVectorDataPtr normalsData = new IECore::V3fVectorData;
    normalsData->setInterpretation(IECore::GeometricData::Normal);
    std::vector<Imath::V3f> &normals = normalsData->writable();
    normals.reserve(mesh.vertices.length());

    BL::Mesh::vertices_iterator v;
    for (mesh.vertices.begin(v); v != mesh.vertices.end(); ++v) {
        normals.push_back(IECore::convert<Imath::V3f>(v->normal()).negate());
    }

    return normalsData;
}

void FromBlenderMeshConverter::convertUvs(BL::MeshTextureFaceLayer &UVlayer, IECore::IntVectorDataPtr faceLenData,
                                          IECore::FloatVectorDataPtr sData, IECore::FloatVectorDataPtr tData) const
{
    BL::MeshTextureFaceLayer::data_iterator face;
    const std::vector<int> &faceLen = faceLenData->readable();
    std::vector<float> &s = sData->writable();
    std::vector<float> &t = tData->writable();

    int i = 0;
    for (UVlayer.data.begin(face); face != UVlayer.data.end(); ++face, ++i) {
        /* reversed face winding order */
        if (faceLen[i] == 4) {
            s.push_back(face->uv4().data[0]);
            t.push_back(face->uv4().data[1]);
        }

        s.push_back(face->uv3().data[0]);
        t.push_back(face->uv3().data[1]);

        s.push_back(face->uv2().data[0]);
        t.push_back(face->uv2().data[1]);

        s.push_back(face->uv1().data[0]);
        t.push_back(face->uv1().data[1]);
    }
}

IECore::ObjectPtr FromBlenderMeshConverter::doConversion(BL::Object &object, BL::Context &context, IECore::ConstCompoundObjectPtr operands) const
{
    BL::Mesh mesh = objectToMesh(object, context, operands);
    const std::string &interpolation = operands->member<IECore::StringData>("interpolation")->readable();
    int numPolygons = mesh.tessfaces.length();
    int numPolygonsVerts = 0;

    IECore::IntVectorDataPtr faceLenData = new IECore::IntVectorData;
    std::vector<int> &faceLen = faceLenData->writable();
    faceLen.reserve(numPolygons);

    BL::Mesh::tessfaces_iterator f;
    for(mesh.tessfaces.begin(f); f != mesh.tessfaces.end(); ++f) {
        BL::Array<int, 4> verts = f->vertices_raw();
        int vlen = (verts.data[3] == 0 ? 3 : 4);
        numPolygonsVerts += vlen;
        faceLen.push_back(vlen);
    }

    IECore::IntVectorDataPtr vertexIndexData = new IECore::IntVectorData;
    std::vector<int> &vertexIndex = vertexIndexData->writable();
    vertexIndex.reserve(numPolygonsVerts);

    for(mesh.tessfaces.begin(f); f != mesh.tessfaces.end(); ++f) {
        BL::Array<int, 4> verts = f->vertices_raw();

        /* reverse the face winding order */
        if (verts.data[3]) {
            vertexIndex.push_back(verts[3]);
        }

        vertexIndex.push_back(verts.data[2]);
        vertexIndex.push_back(verts.data[1]);
        vertexIndex.push_back(verts.data[0]);
    }

    IECore::MeshPrimitivePtr result = new IECore::MeshPrimitive(faceLenData, vertexIndexData, interpolation);

    if (operands->member<IECore::BoolData>("points")->readable()) {
        result->variables["P"] = IECore::PrimitiveVariable(IECore::PrimitiveVariable::Vertex, convertPoints(mesh));
    }

    if (operands->member<IECore::BoolData>("normals")->readable() && interpolation == "linear") {
        result->variables["N"] = IECore::PrimitiveVariable(IECore::PrimitiveVariable::FaceVarying, convertNormals(mesh));
    }

    if (operands->member<IECore::BoolData>("st")->readable()) {
        BL::Mesh::tessface_uv_textures_iterator layer;
        int uvidx = 0;
        for (mesh.tessface_uv_textures.begin(layer); layer != mesh.tessface_uv_textures.end(); ++layer, ++uvidx) {
            IECore::FloatVectorDataPtr sData = new IECore::FloatVectorData;
            IECore::FloatVectorDataPtr tData = new IECore::FloatVectorData;
            sData->writable().reserve(numPolygonsVerts);
            tData->writable().reserve(numPolygonsVerts);

            convertUvs(*layer, faceLenData, sData, tData);

            if (layer->active_render()) {
                result->variables["s"] = IECore::PrimitiveVariable(IECore::PrimitiveVariable::FaceVarying, sData);
                result->variables["t"] = IECore::PrimitiveVariable(IECore::PrimitiveVariable::FaceVarying, tData);
                result->variables["stIndices"] = IECore::PrimitiveVariable(IECore::PrimitiveVariable::FaceVarying, vertexIndexData);
            }

            if (operands->member<IECore::BoolData>("extraST")->readable()) {
                result->variables[layer->name()+"_s"] = IECore::PrimitiveVariable(IECore::PrimitiveVariable::FaceVarying, sData);
                result->variables[layer->name()+"_t"] = IECore::PrimitiveVariable(IECore::PrimitiveVariable::FaceVarying, tData);
                result->variables[layer->name()+"Indices"] = IECore::PrimitiveVariable(IECore::PrimitiveVariable::FaceVarying, vertexIndexData);
            }
        }
    }

    // TODO: vertex colors

    context.blend_data().meshes.remove(mesh);

    return result;
}
