// Contains a configuration that sets pixel data format and Image AOI.

#ifndef INCLUDED_PIXELFORMATANDAOICONFIGURATION_H_00104928
#define INCLUDED_PIXELFORMATANDAOICONFIGURATION_H_00104928

#include <pylon/ConfigurationEventHandler.h>

namespace Pylon
{
    class CInstantCamera;
}
class CPixelFormatAndAoiConfiguration : public Pylon::CConfigurationEventHandler
{
public:
    void OnOpened( Pylon::CInstantCamera& camera)
    {
        try
        {
            // Allow all the names in the namespace GenApi to be used without qualification.
            using namespace GenApi;
            
            // Get the camera control object.
            INodeMap &control = camera.GetNodeMap();
            
            // Get the parameters for setting the image area of interest (Image AOI).
            //            const CIntegerPtr width = control.GetNode("Width");
            //            const CIntegerPtr height = control.GetNode("Height");
            const CIntegerPtr width = control.GetNode("Width");
            const CIntegerPtr height = control.GetNode("Height");
            
            const CBooleanPtr centerX = control.GetNode("CenterX");
            const CBooleanPtr centerY = control.GetNode("CenterY");
            
            
            const CIntegerPtr offsetX = control.GetNode("OffsetX");
            const CIntegerPtr offsetY = control.GetNode("OffsetY");
            
            // cout<<"width "<<width<<" height "<<height;
            unsigned int offset_x = 0; // (width->GetMax() - 960) / 2;
            unsigned int offset_y = 0; // (height->GetMax() - 600) / 2;
            
            
            cout<<"offset_x "<<offset_x<<" by "<<offset_y;
            // Maximize the Image AOI.
            //            if (IsWritable(offsetX))
            //            {
            //                offsetX->SetValue(offset_x); //offsetX->GetMin());
            //                
            //            }
            //            if (IsWritable(offsetY))
            //            {
            //                offsetY->SetValue(offset_y); //offsetY->GetMin());
            //            }
            if (IsWritable(centerX))
            {
                centerX->SetValue(true);
            }
            
            if (IsWritable(centerY))
            {
                centerY->SetValue(true);
            }      
            //            width->SetValue(width->GetMax());
            //            height->SetValue(height->GetMax());
            
            width->SetValue(960); //width->GetMax());
            height->SetValue(600); //height->GetMax());
            cout<<" width->SetValue(";
            
            
            
            
            
            
            //            CenterX.SetValue(true);
            //            CenterY.SetValue(true);
            //            control.CenterX.SetValue(true);
            //            control.CenterY.SetValue(true);
            
            
            // Set the pixel data format.
            CEnumerationPtr(control.GetNode("PixelFormat"))->FromString("Mono8"); //RGB8 //YCbCr 422
        }
        catch (const GenericException& e)
        {
            throw RUNTIME_EXCEPTION( "Could not apply configuration. const GenericException caught in OnOpened method msg=%hs", e.what());
        }
    }
};

#endif /* INCLUDED_PIXELFORMATANDAOICONFIGURATION_H_00104928 */

