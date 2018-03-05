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

#include "IECoreBlender/FromBlenderCameraConverter.h"
#include "IECoreBlender/Convert.h"

#include "IECore/CompoundParameter.h"
#include "IECore/Camera.h"
#include "IECore/MatrixTransform.h"
#include "IECore/AngleConversion.h"
#include "IECore/TypeIds.h"

#include "OpenEXR/ImathMath.h"

using namespace IECoreBlender;

IE_CORE_DEFINERUNTIMETYPED(FromBlenderCameraConverter);

FromBlenderObjectConverter::FromBlenderObjectConverterDescription<FromBlenderCameraConverter>
    FromBlenderCameraConverter::m_description(BL::Object::type_CAMERA, IECore::CameraTypeId, true);

FromBlenderCameraConverter::FromBlenderCameraConverter(const BL::Object &object)
    :    FromBlenderObjectConverter("Converts Blender camera into IECore::Camera object.", object)
{
    if (const_cast<BL::Object &>(object).type() != BL::Object::type_CAMERA) {
        throw IECore::Exception("Object isn't a Blender Camera type");
    }

    IECore::IntParameter::PresetsContainer resolutionModePresets;
    resolutionModePresets.push_back(IECore::IntParameter::Preset("renderGlobals", RenderGlobals));
    resolutionModePresets.push_back(IECore::IntParameter::Preset("specified", Specified));

    m_resolutionMode = new IECore::IntParameter(
        "resolutionMode",
        "Determines how the resolution of the camera is decided.",
        RenderGlobals,
        RenderGlobals,
        Specified,
        resolutionModePresets,
        true
    );

    parameters()->addParameter(m_resolutionMode);

    IECore::V2iParameter::PresetsContainer resolutionPresets;
    resolutionPresets.push_back(IECore::V2iParameter::Preset("2K", Imath::V2i(2048, 1556)));
    resolutionPresets.push_back(IECore::V2iParameter::Preset("1K", Imath::V2i(1024, 778)));

    m_resolution = new IECore::V2iParameter(
        "resolution",
        "Specifies the resolution of the camera when mode is set to \"Specified\".",
        Imath::V2i(2048, 1556),
        resolutionPresets
    );

    parameters()->addParameter(m_resolution);

}

IECore::ObjectPtr FromBlenderCameraConverter::doConversion(BL::Object &object, BL::Context &context, IECore::ConstCompoundObjectPtr operands) const
{
    BL::Camera camera(object.data());
    BL::RenderSettings renderSettings(context.scene().render());

    // convert things that are required by the IECore::Renderer specification

    IECore::CameraPtr result = new IECore::Camera;
    result->setName(IECore::convert<std::string>(object.name()));

    result->setTransform(new IECore::MatrixTransform(IECore::convert<Imath::M44f>(object.matrix_world())));

    Imath::V2i resolution;
    if (operands->member<IECore::IntData>("resolutionMode")->readable() == RenderGlobals) {
        resolution = Imath::V2i(renderSettings.resolution_x() * renderSettings.resolution_percentage() * 0.01f,
                                renderSettings.resolution_y() * renderSettings.resolution_percentage() * 0.01f);
    }
    else {
        resolution = operands->member<IECore::V2iData>("resolution")->readable();
    }
    result->parameters()["resolution"] = new IECore::V2iData(resolution);

    result->parameters()["clippingPlanes"] = new IECore::V2fData(Imath::V2f(camera.clip_start(), camera.clip_end()));

    BL::Camera::sensor_fit_enum fit = (camera.type() != BL::Camera::type_PERSP ? BL::Camera::sensor_fit_AUTO : camera.sensor_fit());
    float xratio = (float)resolution.x * renderSettings.pixel_aspect_x() / 200.0f;
    float yratio = (float)resolution.y * renderSettings.pixel_aspect_y() / 200.0f;
    float aspectratio, xaspect, yaspect;
    if (fit == BL::Camera::sensor_fit_HORIZONTAL || (fit == BL::Camera::sensor_fit_AUTO && xratio > yratio)) {
        aspectratio = xratio / yratio;
        xaspect = aspectratio;
        yaspect = 1.0f;    
    }
    else if (fit == BL::Camera::sensor_fit_VERTICAL || (fit == BL::Camera::sensor_fit_AUTO && xratio < yratio)) {
        aspectratio = yratio / xratio;
        xaspect = 1.0f;
        yaspect = aspectratio;
    }
    else {
        aspectratio = xaspect = yaspect = 1.0f;
    }

    if (camera.type() == BL::Camera::type_ORTHO) {
        xaspect = xaspect * camera.lens() / (aspectratio * 2.0f);
        yaspect = yaspect * camera.lens() / (aspectratio * 2.0f);
        result->parameters()["projection"] = new IECore::StringData("orthographic");
    }
    else {
        float sensor = (camera.sensor_fit() == BL::Camera::sensor_fit_VERTICAL ? camera.sensor_height() : camera.sensor_width());
        float fov = 360.0f * Imath::Math<double>::atan((sensor * 0.5) / camera.lens() / aspectratio) / M_PI;
        result->parameters()["projection"] = new IECore::StringData("perspective");
        result->parameters()["projection:fov"] = new IECore::FloatData(fov);
    }

    result->parameters()["screenWindow"] = new IECore::Box2fData(Imath::Box2f(Imath::V2f(-xaspect, xaspect), Imath::V2f(-yaspect, yaspect)));

    return result;
}
