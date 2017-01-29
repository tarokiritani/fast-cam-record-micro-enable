/*
 * LabViewFunc.h
 *
 * Copyright (c) 2002-2012 Silicon Software GmbH, All Rights Reserved.
 */
 
/**
 * \file SiSo_LabviewInterface.h
 *
 * \brief Implementation of LabViewWrapper.Dll to ease the use of the
 *        Silicon Software frame grabber library inside LabVIEW
 *        projects. The library encapsulates the frame grabber handle
 *        (fg_struct) that is needed to access the frame grabber library.
 *        The frame grabber handle is stored in a global variable.
 *
 * \author Silicon Software GmbH
 */
#include "fgrab_prototyp.h"
#include "fgrab_struct.h"
#include "fgrab_define.h"


/**
 * \brief The function Fg_convertImageArray16to8LV converts the 16-bit  
 *        image array to 8-bit format.
 *        This function allows proper display of images if the application 
 *        does not contain the correct 16-bit image drawing module but only the 8-bit one.
 *		  The output 8-bit array must be allocated first. It will 
 *		  be half the size of the input 16-bit array.
 * \param  byteSize			Size of source 16-bit array
 * \param  InputArray16		Source array
 * \param  OutputArray8		Destination array
 * \retval The function returns the number of the last transferred image (frameindex_t).
*/
extern "C" int Fg_convertImageArray16to8LV(int byteSize, BYTE* InputArray16, BYTE* OutputArray8);

//*************************************************
// Using user defined board handle 
//*************************************************
#ifdef SISOLABVIEW_VER2

/**
 * \brief The function Fg_InitLV loads an applet on a certain frame grabber.
 * 
 * \param	DLLName        Filename of the applet to be loaded
 * \param	NumberOfmEnable Index of the board to be attached, starting with 0
 * \returns	Address of initialized pointer to the data structure of the frame grabber.
 * \retval	NULL Initialization failed.
*/
extern "C" __int64 Fg_InitLV(const char *DLLName,int NumberOfmEnable);

/**
 * \brief The function Fg_InitConfigLV loads an applet on a certain
 *        frame grabber and initializes the applet by using startup
 *        values. The name of the applet to be loaded and the startup 
 *        values for the applet parameters are retrieved from a configuration
 *        file (*.mcf). The configuration file has to be created beforehand, e.g., by using the tool MicroDisplay.
 * 
 * \param  ConfigName     Filename of the configuration file (*.mcf)
 * \param  NumberOfmEnable Index of the board to be attached, starting with 0
 * \returns	Address of initialized pointer to the data structure of the frame frabber.
 * \retval	NULL Initialization failed.
*/
extern "C" __int64 Fg_InitConfigLV(const char *ConfigName,int NumberOfmEnable);

/**
 * \brief The function Fg_loadConfigLV initializes the currently loaded 
 *        applet by using startup values stored in a configuration file.
 *        The configuration file (*.mcf) has to be created beforehand, e.g., by using the tool MicroDisplay.
 * 
 * \param  FgAddr	Address of pointer to used frame grabber
 * \param  Filename Filename of the configuration file (*.mcf)
 * \retval          0 in case of success, else : error occurred.
*/
extern "C" int Fg_loadConfigLV(__int64 FgAddr, const char * Filename);

/**
 * \brief The function Fg_saveConfigLV saves the current applet settings
 *        to a configuration file.
 * 
 * \param  FgAddr	Address of pointer to used frame grabber
 * \param  Filename  Filename of the configuration file (*.mcf)
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_saveConfigLV(__int64 FgAddr, const char * Filename);

/**
 * \brief The function Fg_FreeMemLVEx releases memory areas that have been reserved as image/frame buffers.
 * 
 * \param  FgAddr	Address of pointer to used frame grabber
 * \param  CamPort   Port to be attached
 * \param  DmaAddr	Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval 0 in case of success, else : error occurred.
*/
extern "C" int Fg_FreeMemLVEx(__int64 FgAddr, int CamPort, __int64 DmaAddr);

/**
 * \brief The function Fg_getParameterLV reads the value of a certain parameter of the applet.
 * Keep in mind that applet parameters are generally of different types (integers, floats, strings, etc.).
 * The buffer (value) needs to be of the correct size, in accordance with the applet parameter.
 *
 * For details see the frame grabber library documentation.
 * 
 * \param  FgAddr	 Address of pointer to used frame grabber
 * \param  Parameter Identifier of the parameter
 * \param  Value     Buffer to be filled
 * \param  CamPort   Port to be attached
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_getParameterLV(__int64 FgAddr, int Parameter,void *Value,int CamPort);

/**
 * \brief	The function Fg_getParameterEx gets special parameter settings.
 * \anchor  Fg_getParameterEx
 * 
 * Fg_getParameterEx() allows querying values that are directly related to image transfers. These
 * values are queried by the dma_mem pointer and passed as arguments. These values are available
 * as long as the buffer memory is still available, i.e., even if the DMA transfer is no longer
 * running or a DMA transfer is running with a different DMA buffer.
 *
 * As all of these values have special characteristics, only a small set of parameter IDs can
 * be queried using this interface:
 *
 *
 * \param   FgAddr		Address of pointer to used frame grabber
 * \param	Parameter	Parameter ID
 * \param	Value		Pointer to memory where requested value will be stored
 * \param	DmaAddr		Address of pointer to variable buffer, should be casted to dma_mem*
 * \param	ImgNr		Image number to query
 *
 * \retval	#FG_OK The parameter value has been read correctly.
 * \retval	#FG_NOT_INIT Initialization has failed.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been entered.
 */
extern "C" int Fg_getParameterLVEx(__int64 FgAddr, int Parameter, void *Value, __int64 DmaAddr, const __int64 ImgNr);

/**
 * \brief The function Fg_setParameterLV changes the value of a certain parameter of the applet.
 * Keep in mind that applet parameters are generally of different types (integers, floats, strings, etc.).
 * The buffer (value) needs to be of the correct size, in accordance with the applet parameter.
 *
 * For details see the frame grabber library documentation.
 * 
 * \param  FgAddr	 Address of pointer to used frame grabber
 * \param  Parameter Identifier of the parameter
 * \param  Value     Buffer containing the data
 * \param  CamPort   Port to be attached
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_setParameterLV(__int64 FgAddr, int Parameter,void *Value,int CamPort);

/**
 * \brief The function Fg_FreeGrabberLV  releases all allocated and controlled
 *        resources related to the frame grabber.
 * 
 * \param  FgAddr	 Address of pointer to used frame grabber
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_FreeGrabberLV(__int64 FgAddr);

/**
 * \brief The function Fg_AllocMemLVEx allocates memory for storing image data.
 * 
 * \param  FgAddr	  Address of pointer to used frame grabber
 * \param  Size       Number of bytes needed (for all buffers)
 * \param  Nr_Of_Buf  Number of buffers to use
 * \param  CamPort	  Frame grabber port to be started
 * \returns	 Address of pointer to allocated memory
 *
 * \retval	NULL An error occured. Use Fg_getLastErrorDescription() to get further information.
*/
extern "C" __int64 Fg_AllocMemLVEx(__int64 FgAddr, int Size, int Nr_Of_Buf,int CamPort);

/**
 * \brief   Use an allocated user memory as frame buffer.
 * \anchor  Fg_AllocMemHeadLV
 * 
 * To transfer frames into user provided memory these memory areas have to be announced to
 * the framegrabber system. Fg_AllocMemHeadLV() creates the internal control stuctures to handle
 * these memory. Afterwards you have to add the different memory buffers using Fg_AddMemLV().
 * Single buffers can be released with Fg_DelMemLV() from the buffer as long as this buffer is not
 * currently used for frame transfer. The control structure must be released with Fg_FreeMemHeadLV().
 *
 * Please see the chapter "General memory limits" in the framegrabber documentation for general
 * restrictions about the usable memory.
 *
 *
 * \param   FgAddr          Address of pointer to used frame grabber
 * \param   totalBufSize    Size of all image memory in byte.
 * \param   nr_of_buffer    Number of subbuffers.
 *
 * \returns A valid handle on frame buffer.
 *
 * \retval  NULL An error has occured. Use Fg_getLastErrorDescription() for more information.A return value of 
 * FG_INVALID_MODULO indicates, that the requested memory size does not match to the requirements. 
 */
extern "C" __int64 Fg_AllocMemHeadLV(__int64 FgAddr, int totalBufSize, int nr_of_buffer);

/**
 * \brief	Adding user memory.
 * \anchor fg_addmemLV
 *
 * To use an allocated user memory as subbuffer, a handle on the frame buffer must have been created with the
 * function Fg_AllocMemHeadLV(), before. Then, the function Fg_AddMemlV() adds an allocated memory area to the frame buffer management.
 *
 * The assignment between acquired images and subbuffers, which contain the images within PC memory, depends on the acquisition mode.
 *
 * Please note, that there are memory limits existing!
 *
 * \param	FgAddr      Address of pointer to used frame grabber
 * \param	pBuffer     Pointer to user memory to be added.
 * \param	Size        Size of user memory in byte.
 * \param	bufferIndex Index of subbuffer to add (range 0 to BufCnt as passed to Fg_AllocMemHead())
 * \param	memHandle   Pointer to the handle on frame buffer.
 *
 * \returns	Index of subbuffer.
 *
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been used.
 */
extern "C" __int64 Fg_AddMemLV(__int64 FgAddr, void* pBuffer, int size, int bufferIndex, void* memHandle);


/**
 * \brief   Deleting user memory,
 * \anchor  Fg_DelMemLV
 * 
 * With the function Fg_DelMemLV() a user memory area of the image memory management can
 * be removed which has been created with Fg_AllocMemHead() and added with Fg_AddMemLV() before.
 * The memory area won't be freed, it will only be removed from frame memory queues. Releasing of
 * the memory are is up to the user.
 *
 * \param   FgAddr      Address of pointer to used frame grabber
 * \param   memHandle   Pointer to the handle on frame buffer.
 * \param   bufferIndex Index of subbuffers, the user memory should be deleted from.
 *
 * \retval  #FG_OK User memory has been deleted correctly.
 * \retval  #FG_INVALID_PARAMETER An invalid parameter has been used.
 */
extern "C" __int64 Fg_DelMemLV(__int64 FgAddr, void* memHandle, int bufferIndex);

/**
 * \brief	Access on frame buffer.
 * \anchor  Fg_getImagePtrLVEx
 * 
 * With the function Fg_getImagePtrLVEx() a pointer to an image in the frame buffer is created.
 *
 * Reference: The function will return only the correct image, if it still exists in the frame buffer.
 *
 * \param   FgAddr      frame grabber to use
 * \param   PicNr       Number of frame buffer or picture number.
 * \param   CamPort     Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param   memHandle   Pointer to variable buffer.
 *
 * \returns Pointer to current image position.
 */
extern "C" void* Fg_getImagePtrLVEx(__int64 FgAddr, int PicNr, int CamPort, void* memHandle);

/**
 * \brief	The function Fg_getParameterIdByNameLV() searchs for the ID of the parameter referenced by the string name.
 * \anchor  Fg_getParameterIdByNameLV
 * 
 * The string name is the full name of the parameter. The full name of the parameters is determined by VisualApplets during
 * Applet creation. It consists of the process name, the module name in which the parameter has been defined, and the simple
 * parameter name. Each part of the full name is separated by an underscore ('_'). Example: The parameter XOffset in module
 * Buffer1 in process 0 is accessed by the full name "Process0_Buffer1_XOffset".
 *
 * When hierarchical boxes are used, the name(s) of the box(es) have to be added in front of the module name in which the parameter
 * has been defined. Example: The parameter XOffset in module Buffer1 inside hierarchical box HBox1 in process 0 is accessed
 * by the full name "Process0_HBox1_Buffer1_XOffset"
 *
 * Important: Parameters in microEnable IV applets get an additional prefix "Device1". The full name of the above example
 * is on a microEnable IV board is "Device1_Process0_Buffer1_XOffset". Furthermore, for PixelPlant applets the prefix
 * changes to "Device2".
 *
 * \param   FgAddr  frame grabber to use
 * \param   name    Full Parameter name.
 *
 * \returns The parameter ID.
 *
 * \retval  <0 The parameter couldn't be found.
 */
extern "C" int Fg_getParameterIdByNameLV(__int64 FgAddr, char* name);

/**
 * \brief	Releasing memory.
 * \anchor  Fg_FreeMemHeadLV
 * 
 * The function Fg_FreeMemHeadLV() releases a handle on a frame buffer that has been added with Fg_AllocMemHead().
 * If the acquisition and access to the memory is still active, the function is not able to release the memory
 * area and returns with an error message.
 *
 * You do not need to remove the frame memory using Fg_DelMem() before calling this function as they will
 * automatically be removed.
 *
 * Please note that this function will only remove the internal control structures and will not free the memory
 * passed to Fg_AddMemLV().
 *
 * \param	FgAddr      frame grabber to use
 * \param	pMemHead    Handle on memory area, which has been reserved by Fg_AllocMemLVEx() or Fg_AllocMemHeadLV().
 *
 * \retval	#FG_OK Image memory has been released successfully.
 * \retval	#FG_NOT_INIT A failed try to release. Although, frame grabber has not been initialized correctly.
 * \retval	#FG_STILL_ACTIVE Transfer of an image is still active. The transfer has to be stopped before releasing the frame grabber buffer.
 */
extern "C" int Fg_FreeMemHeadLV(__int64 FgAddr, void* pMemHead);

/**
 * \brief The function Fg_AcquireLVEx starts an image acquisition at the given frame grabber port.
 * 
 * \param  FgAddr	 Address of pointer to used frame grabber
 * \param  CamPort   Frame grabber port to be started
 * \param  PicCount  Number of images to be acquired, might be INFINITE
 * \param  Flag		 #ACQ_STANDARD, #ACQ_BLOCK
 * \param  DmaAddr	 Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_AcquireLVEx(__int64 FgAddr, int CamPort ,int PicCount, int Flag, __int64 DmaAddr); 

/**
 * \brief The function Fg_stopAcquireLV stops an image acquisition at the given frame grabber port.
 * 
 * \param  FgAddr	 Address of pointer to used frame grabber
 * \param  CamPort   Frame grabber port to be stopped
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_stopAcquireLV(__int64 FgAddr, int CamPort);

/**
 * \brief The function Fg_getBytesPerPixelLV returns the number of bytes used
 *        for representation of one pixel inside the RAM
 * 
 * 
 * The size of one pixel in the RAM (the output of an applet) can be configured
 * within the applet by using the applet-parameter FG_FORMAT. This funtion returns
 * the current setting of the applet. The value can be used for dimensioning the
 * image buffer according to the formula: width * heigth * BytesPerPixel
 *
 * \param  FgAddr	Address of pointer to used frame grabber
 * \param  CamPort	Index of the frame grabber's camera port, starting with 0
 * \retval Size of 1 Pixel inside the memory in bytes
*/
extern "C" int Fg_getBytesPerPixelLV(__int64 FgAddr, int CamPort);

/**
 * \brief The function Fg_getLastImageArray24BlockingLVEx copies the 24-bit image 
 *        data from the standard buffers to a given array in blocking mode. The 
 *		  array has to be allocated before with according dimensions.
 * 
 * \param  FgAddr		Pointer address frame grabber to use
 * \param  lastPicNr	Requested image number.
 * \param  CamPort		Index of the frame grabber's camera port, starting with 0
 * \param  Array		Destination array
 * \param  DmaAddr		Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval By success the function returns the number of the last transferred image (frameindex_t), 
 *		   otherwise the error code
*/
extern "C" int Fg_getLastImageArray24BlockingLVEx(__int64 FgAddr, int CamPort,BYTE *Array,__int64 DMAAddr);


/**
 * \brief The function Fg_getLastImageArray8BlockingLVEx copies the 8-bit image 
 *        data from the standard buffers to a given array in blocking mode. The 
 *        array has to be allocated before with according dimensions.
 *
 * \param  FgAddr		Pointer address frame grabber to use
 * \param  lastPicNr	Requested image number.
 * \param  CamPort		Index of the frame grabber's camera port, starting with 0
 * \param  Array		Destination array
 * \param  DmaAddr		Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval By success the function returns the number of the last transferred image (frameindex_t), 
 *		   otherwise the error code
*/
extern "C" int Fg_getLastImageArray8BlockingLVEx(__int64 FgAddr, int CamPort,BYTE *Array,__int64 DMAAddr);

/**
 * \brief The function Fg_getLastImageArray8LVEx copies the 8-bit image 
 *        data from the standard buffers to a given array in standard mode.  
 *        The array has to be allocated before with according dimensions.
 *
 * \param  FgAddr		Pointer address frame grabber to use
 * \param  lastPicNr	Requested image number.
 * \param  CamPort		Index of the frame grabber's camera port, starting with 0
 * \param  Array		Destination array
 * \param  DmaAddr		Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval By success the function returns the number of accessed buffer, otherwise the error code
*/
extern "C" int Fg_getLastImageArray8LVEx(__int64 FgAddr, int CamPort,BYTE *Array,__int64 DMAAddr);


/**
 * \brief The function Fg_getLastImageArray24LVEx copies the 24-bit image 
 *        data from the standard buffers to a given array in standard mode.  
 *		  The array has to be allocated before with according dimensions.
 * 
 * \param  FgAddr		Pointer address frame grabber to use
 * \param  lastPicNr	Requested image number.
 * \param  CamPort		Index of the frame grabber's camera port, starting with 0
 * \param  Array		Destination array
 * \param  DmaAddr		Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval By success the function returns the number of accessed buffer, otherwise the error code
*/
extern "C" int Fg_getLastImageArray24LVEx(__int64 FgAddr, int CamPort,BYTE *Array,__int64 DMAAddr);

/**
 * \brief	Reading the status of frame buffer.
 * 
 * With the function Fg_getStatusLVEx() you are getting information about the acquisition status. The parameter
 * data depends on the chosen flag. For details, please see tables below.
 *
 * By the help of Fg_setStatusLVEx() buffers can be blocked or unblocked.
 *
 *
 * \param	FgAddr		Pointer address frame grabber to use
 * \param	Param	NUMBER_OF_GRABBED_IMAGES		Sum of all acquired images. The parameter data will be ignored.
					NUMBER_OF_LOST_IMAGES			Sum of lost images. The parameter data requires the DMA no.
					NUMBER_OF_BLOCK_LOST_IMAGES		Sum of all images, which are lost by blocking of the frame buffer. The parameter data requires the DMA no.
					NUMBER_OF_BLOCKED_IMAGES		Sum of blocked images. The parameter data will be ignored.
					NUMBER_OF_ACT_IMAGE				Number of last acquired image. The parameter data requires the timeout value.
					NUMBER_OF_LAST_IMAGE			Number of last get image. The parameter data will be ignored.
					NUMBER_OF_NEXT_IMAGE			Number of next image after last get image. The parameter data will be ignored.
					NUMBER_OF_IMAGES_IN_PROGRESS	Sum of all images which aren't gotten, yet. The parameter data will be ignored.
					BUFFER_STATUS					1 if the buffer is locked, otherwise 0. The parameter data will be ignored.
					GRAB_ACTIVE_X					Tells whether the commad is active (1) at the moment (otherwise 0). The parameter data will be ignored.
 * \param	Data		Optional data, details see parameter "Param".
 * \param	nCamPort	Logical number of used camera port.
 * \param	DMAAddr		Pointer address to variable buffer.
 *
 * \returns	Return depends on given flag, see table above ...
 *
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */
extern "C" int Fg_getStatusLVEx(__int64 FgAddr, int Param, int Data, int nCamPort, __int64 DMAAddr);

/**
 * \brief	Setting the status of frame buffer.
 * 
 * The function Fg_setStatusLVEx() explicitely allows, blocking or releasing of a blocked buffer (see table).
 *
 * By the help of Fg_getStatusLVEx() the acquisition status can be gotten.
 *
 *
 * \param	FgAddr		Pointer address frame grabber to use
 * \param	Param		FG_UNBLOCK_ALL		Explicit release of all buffers.
						FG_UNBLOCK			Explicit release of one buffer.
						FG_BLOCK			Release of one blocking buffer.
 * \param	Data		Optional data.
 * \param	CamPort		Logical number of used camera port. 
 * \param	DMAAddr		Pointer address to variable buffer.
 *
 * \returns	Pointer to the corresponding buffer.
 *
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */
extern "C" int Fg_setStatusLVEx(__int64 FgAddr, int Param, int Data, int CamPort, __int64 DMAAddr);

/**
 * \brief	Getting the image.
 * \anchor  Fg_getImageLVEx
 * 
 * After image acquisition, the buffer number of an image can be explicitely gotten by the help of the function
 * Fg_getImageLVEx(). It is needed for blocking acquisition mode. But it can also be used for standard acquisition mode.
 *
 * The first buffer has the number 0.
 *
 * Depending on the parameter, the internal image counters for the last gotten image will be updated or not. A
 * pointer to the image will be gotten by the function Fg_getImagePtrEx().
 *
 * Param
 * SEL_NEW_IMAGE: Selection of current acquired image (running process).
 * SEL_ACT_IMAGE: Selection of last acquired image.
 * SEL_NEXT_IMAGE: Selection of next image after last get image.
 * SEL_NUMBER: Selection of an image by image number, not available in blocking mode!
 * 
 *
 * \param	FgAddr		Pointer address frame grabber to use
 * \param	Param		SEL_NEW_IMAGE: Selection of current acquired image (running process).
						SEL_ACT_IMAGE: Selection of last acquired image.
						SEL_NEXT_IMAGE: Selection of next image after last get image.
						SEL_NUMBER: Selection of an image by image number, not available in blocking mode!
 * \param	PicNr		0 in blocking mode, image number in default grabbing mode.
 * \param	CamPort		Logical number of used camera port.
 * \param	Timeout		Duration of timeout in seconds.
 * \param	DMAAddr		Pointer address to variable buffer.
 *
 * \returns	Buffer number, beginning with 0.
 *
 * \retval	#FG_TIMEOUT_ERR Timeout occurs.
 * \retval	#FG_INVALID_PARAMETER This error occurs, either if the image number is <= 0 or if the blocking mode has been chosen in case of flag SEL_NUMBER.
 */
extern "C" int Fg_getImageLVEx(__int64 FgAddr,int Param, int PicNr, int CamPort,int Timeout, __int64 DMAAddr); 

/**
 * \brief	Requesting the current image number (non-blocking).
 * \anchor  Fg_getLastPicNumberLVEx
 * 
 * The number of the last, completely transferred image can be requested with a non-blocking call
 * of Fg_getLastPicNumberEx(). The first completely transferred image has
 * the image number 1. An image number 0 means, that a complete image is still not transferred.
 *
 * \param	FgAddr		Pointer address of frame grabber to use
 * \param	CamPort		Logical number of used camera port.
 * \param	DmaAddr		Pointer address to variable buffer.
 *
 * \returns  Number of the last completely transferred image, transmission is running.
 *
 * \retval	0 Transmission is running, no image has been completely transfered, yet.
 * \retval	#FG_TIMEOUT_ERR Timeout occurs.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */
extern "C" __int64 Fg_getLastPicNumberLVEx(__int64 FgAddr, int CamPort, __int64 DmaAddr);

/**
 * \brief	Requesting the current image number (blocking)
 * \anchor  Fg_getLastPicNumberBlockingEx
 * 
 * The number of the last, completely transferred image can be requested with a blocking call of
 * Fg_getLastPicNumberBlockingLVEx().
 *
 * In contrary to Fg_getLastPicNumberEx() the function waits, until the transmission of the preselected
 * image number is successfully finished or a maximum time is elapsed. If the image number is obtained
 * before, the function returns immediately. The first image number value will be 1, if no error occurs.
 *
 * Attention: When working with extreme high framerates, the returned value of the function can be bigger
 * than the preselected image number. In this case it is important to allocate enough frame buffers
 *
 * \param	FgAddr		Pointer address of frame grabber to use
 * \param	PicNr		Requested image number.
 * \param	CamPort		Logical number of used camera port.
 * \param	Timeout		Timeout in miliseconds.
 * \param	DmaAddr		Pointer address to variable buffer.
 *
 * \returns	 Current image number, beginning with 1.
 *
 * \retval	#FG_TIMEOUT_ERR Timeout error occured.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */
extern "C" __int64 Fg_getLastPicNumberBlockingLVEx(__int64 FgAddr, int PicNr, int CamPort, int Timeout, __int64 DmaAddr);

/**
 * \brief	Access on frame buffer.
 * \anchor  Fg_getImagePtrEx
 * 
 * With the function Fg_getImagePtrLVEx() to obtain a array of image frame data 
 * according to picture number(by non-blocking mode) or buffer number(by blocking mode)
 * In both cases the value passes over argument PicIndex
 * This function call Fg_getImagePtrEx() copies the image data to the buffer array
 * 
 *
 * Reference: The function will return only the correct image, if it still exists in the frame buffer.
 *
 * \param[in]	FgAddr		Pointer address frame grabber to use
 * \param[in]	PicIndex	Number of frame buffer or picture number.
 * \param[in]	CamPort		Logical number of used camera port.
 * \param[in]	DmaAddr		Pointer address to variable buffer.
 * \param[out]	ImageDataArray	The buffer array to retrieve the image data, must be allocated before (with according dimensions).
 *
 * returns FG_OK means no error, otherwise the error code.
 */
extern "C" int Fg_getImageDataArrayLVEx(__int64 FgAddr, int PicIndex, int CamPort, __int64 DmaAddr, BYTE* ImageDataArray);

/**
 * \brief	Access on frame buffer. 
 * \anchor  Fg_getImagePtrEx
 * 
 * With the function Fg_getImageDataArrayRGB24LV() to obtain a array of image frame data for 24 bit RGB image
 * according to picture number(by non-blocking mode) or buffer number(by blocking mode)
 * In both cases the value passes over argument PicIndex
 * This function call Fg_getImagePtrEx() copies the image data to the buffer array
 *
 * Because the labview image viewing modules "Flatten Pixmap.vi" and "Draw Flattened Pixmap.vi" expect the input
 * data type of unsigned 32 bit, but from frame grabber transported image data is in 24 bit format.
 * In order to solve this issue the 24 bit image data will be converted to 32 bit data in Fg_getImageDataArrayRGB24LV
 *
 * Reference: The function will return only the correct image, if it still exists in the frame buffer.
 *
 * \param[in]	FgAddr		Pointer address frame grabber to use
 * \param[in]	PicIndex		Number of frame buffer or picture number.
 * \param[in]	CamPort		Logical number of used camera port.
 * \param[in]	DmaAddr		Pointer address to variable buffer.
 * \param[out]	ImageDataArrayRGB	The buffer array to retrieve the image data, must be allocated before (with according dimensions).
 *
 * returns FG_OK without error, otherwise the error code.
 */
extern "C" int Fg_getImageDataArray24LVEx(__int64 FgAddr, int PicIndex, int CamPort, __int64 DmaAddr, BYTE* ImageDataArrayRGB);

/**
 * \brief	The function Fg_getErrorMessageLV get the description and error code 
			of last error message.
 * \param	FgAddr		Address of pointer to used frame grabber.
 * \param	ErrText		Buffer to retieve the error description.
 * \retval	Error code.
*/
extern "C" int Fg_getLastErrorMessageLV(__int64 FgAddr, char* ErrText);

/**
 * \brief	Getting the last error code.
 * \param	FgAddr		Address of pointer to used frame grabber.
 * \retval	The number of the last error that occured.
*/
extern "C" int Fg_getLastErrorNumberLV(__int64 FgAddr);

/**
 * \brief	Description of error message to an error code.
 * 
 * The function Fg_getErrorDescriptionLV() returns the error message, when receiving the error code.
 *
 * \param	FgAddr			Address of pointer to used frame grabber.
 * \param	ErrorNumber		Identification number of error code.
 * \param	ErrText			Buffer to retieve the error description.
 */
extern "C" void Fg_getErrorDescriptionToNumberLV(__int64 FgAddr, int ErrNumber, char* ErrText);

//*************************************************
// Gbe-functions

/**
	\brief Initializes the Gbe board
	\param boardNr				Board ID
	\param init_flag			Initialization flags, should be 0
	\param GbeBoardHandleAddr	Pointer on the address of pointer to the board handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_initBoardLV(int boardNr, int init_flag, __int64& GbeBoardHandleAddr);

/**
	\brief Releases the Gbe board
	\param GbeBoardHandleAddr	Pointer on the address of pointer to the board handle instance
*/
extern "C" void Gbe_freeBoardLV(__int64 GbeBoardHandleAddr);

/**
	\brief Scans the network for cameras
	\param GbeBoardHandleAddr	Address of pointer to the board handle instance
	\param port					Port number
	\param DiscoveryTimeout	Timeout for the camera discovery
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_scanNetworkLV(__int64 GbeBoardHandleAddr, int port, int DiscoveryTimeout);

/**
	\brief Gets the number of cameras found on a port
	\param GbeBoardHandleAddr	Address of pointer to the board handle instance
	\param port					Port number
	\return Number of cameras found
*/
extern "C" int Gbe_getCameraCountLV(__int64 GbeBoardHandleAddr, int port);

/**
	\brief Get the first camera found on a port
	\param GbeBoardHandleAddr	Address of pointer to the board handle instance
	\param port					Port number
	\param CamHandleAddr		Address of pointer to the camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_getFirstCameraLV(__int64 GbeBoardHandleAddr, int port, __int64& CamHandleAddr);

/**
	\brief Releases the camera
	\param CamHandleAddr	Address of pointer to the camera handle instance
*/
extern "C" void Gbe_freeCameraLV(__int64 CamHandleAddr);

/**
	\brief Establishes camera connection
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_connectCameraLV(__int64 CamHandleAddr);

/**
	\brief Releases camera connection
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_disconnectCameraLV(__int64 CamHandleAddr);

/**
	\brief Sets GenICam Integer value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value			Value to write
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_setIntegerValueLV(__int64 CamHandleAddr, const char* name, __int64 value);

/**
	\brief Gets GenICam Integer value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value_ptr		Oointer to value
	\return Gbe error code
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_getIntegerValueLV(__int64 CamHandleAddr, const char* name, __int64* value_ptr);

/**
	\brief Sets GenICam Boolean value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value			Value to write (0 = false, otherwise true)
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_setBooleanValueLV(__int64 CamHandleAddr, const char* name, unsigned int value);

/**
	\brief Gets GenICam Boolean value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value_ptr		Pointer to value
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_getBooleanValueLV(__int64 CamHandleAddr, const char* name, unsigned int* value_ptr);

/**
	\brief Sets GenICam Float value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value			Value to write
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_setFloatValueLV(__int64 CamHandleAddr, const char* name, double value);

/**
	\brief Gets GenICam Float value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value_ptr		Pointer to value
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_getFloatValueLV(__int64 CamHandleAddr, const char* name, double* value_ptr);

/**
	\brief Sets GenICam String value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value			vVlue to write
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_setStringValueLV(__int64 CamHandleAddr, const char* name, const char* value);

/**
	\brief Gets GenICam string value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value_ptr		Pointer to value
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_getStringValueLV(__int64 CamHandleAddr, const char* name, const char* value_ptr);

/**
	\brief Sets GenICam Enumeration value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value			Value to write
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_setEnumerationValueLV(__int64 CamHandleAddr, const char* name, const char* value);

/**
	\brief Gets GenICam Enumeration value
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\param value_ptr		Pointer to value
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_getEnumerationValueLV(__int64 CamHandleAddr, const char* name, const char* value_ptr);

/**
	\brief Executes GenICam Command
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_executeCommandLV(__int64 CamHandleAddr, const char* name);

/**
	\brief Checks status of a GenICam Command
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param name				Feature name
	\return 0 = command in inactive, otherwise active
*/
extern "C" int Gbe_isCommandDoneLV(__int64 CamHandleAddr, const char* name);

/**
	\brief Starts image acquisition
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_startAcquisitionLV(__int64 CamHandleAddr);

/**
	\brief Stops image acquisition
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_stopAcquisitionLV(__int64 CamHandleAddr);

/**
	\brief Get the GenICam XML data for the camera and save it to a local file
	\param CamHandleAddr	Address of pointer to the camera handle instance
	\param filename			Full path and file name of the file that you want to save
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
extern "C" int Gbe_getGenICamXMLLV(__int64 CamHandleAddr, const char *filename);

/**
	\brief Get error description
	\param	errcode		error code (defined in gbe_error.h)
	\param	ErrText		Buffer to retieve the error description.
	\return error description as string
*/
extern "C" void Gbe_getErrorDescriptionLV(int errcode, char* ErrText);

//*************************************************
// Using user defined board handle 
//*************************************************
#else //SISOWRAPPER2


/**
 * \brief	The function Fg_getLastPicNumberBlocking2() 
 * 
 * returns the incremental number of the last frame that is transferred by DMA
 * to the RAM
 *
 * \param  CamPort Index of the frame grabber's camera port, starting with 0
 * \param  Timeout Number of milliseconds to wait for the next transferred image
 * \retval Number of the last previously transferred image / frame starting with 1
*/
extern "C" int Fg_getLastPicNumberBlocking2(int CamPort,int Timeout) ;

/**
 * \brief	The function Fg_getLastPicNumberBlocking2Ex() 
 * 
 * returns the incremental number of the last frame that is transferred by DMA
 * to the RAM
 *
 * \param  CamPort	Index of the frame grabber's camera port, starting with 0
 * \param  Timeout	Number of milliseconds to wait for the next transferred image
 * \param  DMAAddr	Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval Number of the last previously transferred image / frame starting with 1
*/
extern "C" int Fg_getLastPicNumberBlocking2Ex(int CamPort,int Timeout, __int64 DMAAddr);

/**
 * \brief The function Fg_getBytesPerPixelLV returns the number of bytes used
 *        for representation of one pixel inside the RAM.
 * 
 * 
 * The size of one pixel in the RAM (the output of an applet) can be configured
 * within the applet by using the applet parameter FG_FORMAT. This funtion returns
 * the current setting of the parameter. The value can be used for dimensioning the
 * image buffer according to the formula: width * heigth * BytesPerPixel
 *
 * \param  CamPort Index of the frame grabber's camera port, starting with 0
 * \retval Size of 1 Pixel inside the memory in bytes
*/
extern "C" int Fg_getBytesPerPixelLV(int CamPort);


/**
 * \brief The function Fg_getLastImageArrayBlockingLV copies the image 
 *        data from the standard buffers to a given array. The array has
 *        to be allocated before (with according dimensions). The function
 *        uses the FG_TRANSFERLEN to retrieve the number of bytes to be copied.
 *        A check of the memory is not performed.
 * 
 * \param  CamPort Index of the frame grabber's camera port, starting with 0
 * \param  Array   Destination array
 * \retval The function returns the number of the last transferred image (frameindex_t).
*/
extern "C" int Fg_getLastImageArrayBlockingLV(int CamPort,BYTE *Array);


/**
 * \brief The function Fg_getLastImageArrayBlockingLVEx copies the image 
 *        data from the standard buffers to a given array. The array has
 *        to be allocated before (with according dimensions). The function
 *        uses the FG_TRANSFERLEN to retrieve the number of bytes to be copied.
 *        A check of the memory is not performed.
 * 
 * \param  CamPort	Index of the frame grabber's camera port, starting with 0
 * \param  Array	Destination array
 * \param  DmaAddr	Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval The function returns the number of the last transferred image (frameindex_t).
*/
extern "C" int Fg_getLastImageArrayBlockingLVEx(int CamPort,BYTE *Array,__int64 DmaAddr);


/**
 * \brief The function Fg_getLastImageArrayBlockingLV copies the image 
 *        data from the standard buffers to a given array. The array has
 *        to be allocated before (with according dimensions).
 * 
 * \param  CamPort Index of the frame grabber's camera port, starting with 0
 * \param  Array   Destination array
 * \retval The function returns the number of the last transferred image (frameindex_t).
*/
extern "C" int Fg_getLastImageArray24BlockingLV(int CamPort,BYTE *Array);

/**
 * \brief The function Fg_getLastImageArrayBlockingLVEx copies the image 
 *        data from the standard buffers to a given array. The array has
 *        to be allocated before (with according dimensions).
 * 
 * \param  CamPort Index of the frame grabber's camera port, starting with 0
 * \param  Array   Destination array
 * \param  DmaAddr	Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval The function returns the number of the last transferred image (frameindex_t).
*/
extern "C" int Fg_getLastImageArray24BlockingLVEx(int CamPort,BYTE *Array,__int64 DMAAddr);

extern "C" int Fg_getLastImageArray8BlockingLV(int CamPort,BYTE *Array);
extern "C" int Fg_getLastImageArray8BlockingLVEx(int CamPort,BYTE *Array,__int64 DMAAddr);


/**
 * \brief The function Fg_getImageArray8LV copies the image 
 *        data from the standard buffers to a given array. The array has
 *        to be allocated before (with according dimensions). It uses the size
 *        8 bits per pixel.
 * 
 * \param  CamPort Index of the frame grabber's camera port, starting with 0
 * \param  imNR    Number of the image to get (frameindex_t), starting at 1
 * \retval 0 in case of success, else : error occurred.
*/
extern "C" int Fg_getImageArray8LV(int CamPort,int imNR, BYTE *Array);

/**
 * \brief The function Fg_getImageArray8LVEx copies the image 
 *        data from the standard buffers to a given array. The array has
 *        to be allocated before (with according dimensions). It uses the size 
 *        8 bits per pixel
 * 
 * \param  CamPort Index of the frame grabber's camera port, starting with 0
 * \param  imNR    Number of the image to get (frameindex_t), starting at 1
 * \param  DmaAddr	Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval 0 in case of success, else : error occurred.
*/
extern "C" int Fg_getImageArray8LVEx(int CamPort,int imNR, BYTE *Array, __int64 DMAAddr);

/**
 * \brief The function Fg_InitLV loads an applet on a certain frame grabber.
 * 
 * \param  DLL_Name        Filename of the applet to be loaded
 * \param  NumberOfmEnable Index of the board to be attached, starting with 0
 * \retval 0 in case of success, else : error occurred.
*/
extern "C" int	Fg_InitLV(char *DLL_Name,int NumberOfmEnable);

/**
 * \brief The function Fg_InitCameraLV is obsolete with runtime 5.2
 * 
*/
extern "C" int	Fg_InitCameraLV(char *DLL_Name,int mEnableNr);


/**
 * \brief The function Fg_InitConfigLV loads an applet on a certain
 *        frame grabber and initializes the applet by using startup
 *        values. The name of the applet to be loaded and the startup 
 *        values for the applet parameters are retrieved from an configuration
 *        file (*.mcf). The configuration file has to be created beforehand, e.g., by using the MicroDisplay tool.
 * 
 * \param  Config_Name     Filename of the configuration file (*.mcf)
 * \param  NumberOfmEnable Index of the board to be attached, starting with 0
 * \retval 0 in case of success, else : error occurred.
*/
extern "C" int	Fg_InitConfigLV(char *Config_Name,int NumberOfmEnable);

/**
 * \brief The function Fg_loadConfigLV initializes the currently loaded 
 *        applet by using startup values stored in a configuration 
 *        file(*.mcf). The configuration file has to be created beforehand, e.g., by using the MicroDisplay tool.
 * 
 * \param  Filename Filename of the configuration file (*.mcf)
 * \retval          0 in case of success, else : error occurred.
*/
extern "C" int Fg_loadConfigLV(const char * Filename);

/**
 * \brief The function Fg_saveConfigLV saves the current applet settings
 *        to a configuration file.
 * 
 * \param  Filename  Filename of the configuration file (*.mcf)
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_saveConfigLV(const char * Filename);


/**
 * \brief The function Fg_FreeMemLV releases memory areas that have been reserved as image/frame buffers.
 * 
 * \param  CamPort Camera port
 * \retval         0 in case of success, else : error occurred.
*/
extern "C" int Fg_FreeMemLV(int CamPort);

/**
 * \brief The function Fg_FreeMemLVEx releases memory areas that have been reserved as image/frame buffers.
 * 
 * \param  DmaAddr	Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval 0 in case of success, else : error occurred.
*/
extern "C" int Fg_FreeMemLVEx(int CamPort, __int64 DMAAddr);

/**
 * \brief The function Fg_getParameterLV reads the value of a certain parameter of the applet.
 * Keep in mind that applet parameters are generally of different types (integers, floats, strings, etc.).
 * The buffer (value) needs to be of the correct size, in accordance with the applet parameter.
 *
 * For details see the frame grabber library documentation.
 * 
 * \param  Parameter Identifier of the parameter
 * \param  Value     Buffer to be filled
 * \param  CamPort   Port to be attached
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_getParameterLV(int Parameter,void *Value,int CamPort);

/**
 * \brief	The function Fg_getParameterEx gets special parameter settings.
 * \anchor  Fg_getParameterEx
 * 
 * Fg_getParameterEx() allows querying values that are directly related to image transfers. These
 * values are queried by the dma_mem pointer and passed as arguments. These values are available
 * as long as the buffer memory is still available, i.e., even if the DMA transfer is no longer
 * running or a DMA transfer is running with a different DMA buffer.!!
 *
 * As all of these values have special characteristics, only a small set of parameter IDs can 
 * be queried using this interface:
 *
 *
 * \param	Parameter	Parameter ID
 * \param	Value		Pointer to memory where requested value will be stored
 * \param	DmaIndex	Ignored, should be 0
 * \param	DmaAddr		Address of pointer to variable buffer, should be casted to dma_mem*
 * \param	ImgNr		Image number to query
 *
 * \retval	#FG_OK The parameter value has been read correctly.
 * \retval	#FG_NOT_INIT Initialization has failed.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been entered.
 */
extern "C" int Fg_getParameterLVEx(int Parameter, void *Value, __int64 DMAAddr, const __int64 ImgNr);


/**
 * \brief The function Fg_setParameterLV changes the value of a certain parameter of the applet.!!
 * Keep in mind that applet parameters are generally of different types (integers, floats, strings, etc.).
 * The buffer (value) needs to be of the correct size, in accordance with the applet parameter.
 * For details see the frame grabber library documentation.
 * 
 * \param  Parameter Identifier of the parameter
 * \param  Value     Buffer containing the data
 * \param  CamPort   Port to be attached
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_setParameterLV(int Parameter,void *Value,int CamPort);

/**
 * \brief The function Fg_FreeGrabberLV releases all allocated and controlled
 *        resources related to the frame grabber.
 * 
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_FreeGrabberLV();


/**
 * \brief The function Fg_AllocMemLV allocates memory for storing image data.
 * 
 * \param  Size      Number of bytes needed (for all buffers)
 * \param  Nr_Of_Buf Number of buffers to use
 * \param  CamPort
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_AllocMemLV(int Size, int Nr_Of_Buf,int CamPort);

/**
 * \brief The function Fg_AllocMemLVEx allocates memory for storing image data.
 * 
 * \param  Size       Number of bytes needed (for all buffers)
 * \param  Nr_Of_Buf  Number of buffers to use
 * \param  CamPort	  Frame grabber port to be started
 * \returns	Address of the pointer to the allocated memory
 *
 * \retval	NULL An error occured. Use Fg_getLastErrorDescription() for more information.
*/
extern "C" __int64 Fg_AllocMemLVEx(int Size, int Nr_Of_Buf,int CamPort);

/**
 * \brief The function Fg_AcquireLV starts image acquisition at the given frame grabber port.
 * 
 * \param  CamPort   Frame grabber port to be started
 * \param  PicCount  Number of images to be acquired, might be INFINITE
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_AcquireLV(int CamPort ,int PicCount);

/**
 * \brief The function Fg_AcquireLVEx starts image acquisition at the given frame grabber port.
 * 
 * \param  CamPort   Frame grabber port to be started
 * \param  PicCount  Number of images to be acquired, might be INFINITE
 * \param  DmaAddr	Address of pointer to variable buffer, should be casted to dma_mem*
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_AcquireLVEx(int CamPort ,int PicCount, __int64 DMAAddr);

/**
 * \brief The function Fg_stopAcquireLV stops image acquisition at the given frame grabber port.
 * 
 * \param  CamPort   Frame grabber port to be stopped
 * \retval           0 in case of success, else : error occurred.
*/
extern "C" int Fg_stopAcquireLV(int CamPort);
#endif//SISOWRAPPER2