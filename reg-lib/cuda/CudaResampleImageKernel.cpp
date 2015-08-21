#include "CudaResampleImageKernel.h"

/* *************************************************************** */
CudaResampleImageKernel::CudaResampleImageKernel(Content *conIn, std::string name) :
        ResampleImageKernel(name)
{
    con = static_cast<CudaContent*>(conIn);

    floatingImage = con->Content::getCurrentFloating();
    warpedImage = con->Content::getCurrentWarped();

    //cuda ptrs
    floatingImageArray_d = con->getFloatingImageArray_d();
    warpedImageArray_d = con->getWarpedImageArray_d();
    deformationFieldImageArray_d = con->getDeformationFieldArray_d();
    mask_d = con->getMask_d();
    floIJKMat_d = con->getFloIJKMat_d();

    if (floatingImage->datatype != warpedImage->datatype) {
        reg_print_fct_error("CudaResampleImageKernel::CudaResampleImageKernel");
        reg_print_msg_error("Floating and warped images should have the same data type. Exit.");
        reg_exit(1);
    }

    if (floatingImage->nt != warpedImage->nt) {
        reg_print_fct_error("CudaResampleImageKernel::CudaResampleImageKernel");
        reg_print_msg_error("Floating and warped images have different dimension along the time axis. Exit.");
        reg_exit(1);
    }
}
/* *************************************************************** */
void CudaResampleImageKernel::calculate(int interp,
                                                     float paddingValue,
                                                     bool *dti_timepoint,
                                                     mat33 * jacMat)
{
    launchResample(this->floatingImage,
                        this->warpedImage,
                        interp,
                        paddingValue,
                        dti_timepoint,
                        jacMat,
                        &this->floatingImageArray_d,
                        &this->warpedImageArray_d,
                        &this->deformationFieldImageArray_d,
                        &this->mask_d,
                        &this->floIJKMat_d);
}
/* *************************************************************** */
