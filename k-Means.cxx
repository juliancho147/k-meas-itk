#include <iostream>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkScalarImageKmeansImageFiltrer.h>

typedef itk::Image<short, Dim > TImage;
static void CreateImage(ImageType::Pointer image);
int main(int arg, char* argv[])
{
	 std::string input_filename = argv[ 1 ];
	 std::string output_filename = argv[ 2 ];

	 //we set the image filtrer reder as TReader
	 typedef itk::ImagefFileReader< TImage> TReader;
	 //create the pointer 
	 TReader::Pointer reader = TReader::new( );
	 //set the file to get it like a image 
	 reader->SetFileName( input_filename );
	 typedef itk::ScalarImagekmeansFiltrer< TImage , TLabel> TKmeans;
	 TKmeans:: Pointer kmeans = TKmeans::new( );
	 kmeans->SetInput( reader->GetOutput( ) );
	 typedef:: itk::ImageFileWriter< TKmeans > TWriter;
	 TWriter::Pointer writer = TWriter::new( );
	 writer->SetInput(  kmeans->GetOutput( ) );
	 writer->SetFileName( output_filename );

	 try
	 {
	 	writer->Update;

	 } 
	 catch(std::exception& err)
	 {
	 	std::cerr<<"Error caught: "<<err.what( )<<std:endl;
	 	return(EXIT_FAILURE);
	 }
	 return(EXIT_SUCCESS);
}