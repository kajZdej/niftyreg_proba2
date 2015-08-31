#include "_reg_ReadWriteImage.h"
#include "_reg_globalTrans.h"
#include "_reg_tools.h"

#include "Kernel.h"
#include "AffineDeformationFieldKernel.h"
#include "Platform.h"
#include "Content.h"

#define EPS 0.00001

void test(Content *con) {

    Platform *cpuPlatform = new Platform(NR_PLATFORM_CPU);

    Kernel *affineDeformKernel = cpuPlatform->createKernel(AffineDeformationFieldKernel::getName(), con);
    affineDeformKernel->castTo<AffineDeformationFieldKernel>()->calculate();

    delete affineDeformKernel;
    delete cpuPlatform;
}

int main(int argc, char **argv)
{
   if(argc!=4)
   {
      fprintf(stderr, "Usage: %s <refImage> <inputMatrix> <expectedField>\n", argv[0]);
      return EXIT_FAILURE;
   }

   char *inputRefImageName=argv[1];
   char *inputMatFileName=argv[2];
   char *inputDefImageName=argv[3];

   // Read the input reference image
   nifti_image *referenceImage = reg_io_ReadImageFile(inputRefImageName);
   if(referenceImage==NULL){
      reg_print_msg_error("The input reference image could not be read");
      return EXIT_FAILURE;
   }
   // Read the input affine matrix
   mat44 *inputMatrix=(mat44 *)malloc(sizeof(mat44));
   reg_tool_ReadAffineFile(inputMatrix, inputMatFileName);
   //
   reg_tool_ReadAffineFilev2(inputMatrix, inputMatFileName);
   // Read the input deformation field image image
   nifti_image *inputDeformationField = reg_io_ReadImageFile(inputDefImageName);
   if(inputDeformationField==NULL){
      reg_print_msg_error("The input deformation field image could not be read");
      return EXIT_FAILURE;
   }
   // Check the dimension of the input images
   if(referenceImage->nx != inputDeformationField->nx ||
      referenceImage->ny != inputDeformationField->ny ||
      referenceImage->nz != inputDeformationField->nz ||
      (referenceImage->nz>1?3:2) != inputDeformationField->nu){
      reg_print_msg_error("The input reference and deformation field images do not have corresponding sizes");
      return EXIT_FAILURE;
   }

   // Create a deformation field
   nifti_image *test_field=nifti_copy_nim_info(inputDeformationField);
   test_field->data=(void *)malloc(test_field->nvox*test_field->nbyper);

   // Compute the affine deformation field

   //CPU code
   reg_tools_changeDatatype<float>(referenceImage);
   Content *con = new Content(referenceImage, NULL, NULL,inputMatrix, sizeof(float));
   test(con);
   test_field = con->getCurrentDeformationField();

//DEBUG
   //PRINT THE FIELD VALUES
   //reg_io_diplayImageData(test_field);
   //reg_io_WriteImageFile(test_field,"testField.nii");
   //END
//DEBUG

   // Compute the difference between the computed and inputed deformation field
   reg_tools_substractImageToImage(inputDeformationField,test_field,test_field);
   reg_tools_abs_image(test_field);
   double max_difference=reg_tools_getMaxValue(test_field);

   nifti_image_free(referenceImage);
   nifti_image_free(inputDeformationField);

   delete con;
   free(inputMatrix);

   if(max_difference>EPS){
      fprintf(stderr, "reg_test_affine_deformation_field error too large: %g (>%g)\n",
              max_difference, EPS);
      return EXIT_FAILURE;
   } else {
       fprintf(stdout, "reg_test_affine_deformation_field ok: %g (<%g)\n",
               max_difference, EPS);
      return EXIT_SUCCESS;
   }
}
