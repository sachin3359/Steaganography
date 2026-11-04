#include <stdio.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr){
    fseek(fptr,0,SEEK_END);
    uint size = ftell(fptr);
    rewind(fptr);
    return size;
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *extn = strrchr(argv[2],'.');
    if(extn == NULL || strcmp(extn,".bmp")!=0){
        printf("Error: Source image must have .bmp extension\n");
        return e_failure;
    }

    encInfo->src_image_fname = argv[2];
    extn = strrchr(argv[3],'.');
    if(extn ==NULL || extn == argv[3]||extn[1]=='\0'){
        printf("Error: Invalid secret file extension\n");
        return e_failure;
}
if(strlen(extn)>=sizeof(encInfo->extn_secret_file)){
    printf("Error: Secret file extension is too long\n");
    return e_failure;
}
encInfo->secret_fname =argv[3];
strcpy(encInfo->extn_secret_file,extn);

if(argv[4] ==NULL){
    encInfo->stego_image_fname = "stego.bmp";

}
else{
    extn=strrchr(argv[4],'.');
    if(extn==NULL||strcmp(extn,".bmp")!=0){
        printf("Error: Output image must have .bmp extension\n");
        return e_failure;
    }
    encInfo->stego_image_fname = argv[4];
}
return e_success;
}


    /*
        -> Check the argv[2] must be having ".bmp" extn
        if yes,  endInfo -> src_image_fname = argv[2]
            open the file in read mode argv[2]
            //encInfo -> fptr_src_image = fopen()
            if the file not present, return e_failure
        if not, return e_failure
        
        -> Check the argv[3] is  a valid file
                encInfo -> secret_fname = argv[3]
        is the file is present or not
        if present, encInfo -> fptr_secret = fopen()
        if not, return e_failure

        -> Check the argv[4] is NULL
        if not NULL, encInfo -> stego_image_fname = argv[4]
        if NULL, encInfo -> stego_image_fname = "default.bmp"

        Check the file extn is only of ".bmp"
        if not, return e_failure


        Open the file in wr mode, and store the file pointer to structure member .
        


    */

}

Status do_encoding(EncodeInfo *encInfo){

    if(openfiles(encInfo)!=e_success){
        printf("Error:unable to open files\n");
        return e_failure;
    }

    printf("Files opened successfully\n");

    if(check_capacity(encInfo)!=e_success){
        printf("Error:Insufficient image capacity\n");
        return e_failure;
    }

    printf("Image has sufficient capacity\n");

    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)!=e_success){
        printf("Error: Unable to copy BMP header\n");
        return e_failure;
    }
    printf("Magic string encoded successfully\n");

    if(encoded_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)!=e_success){
        printf("Error: Unable to encode secret file extension size\n");
        return e_failure;
    }

    printf("Secret file extension size encoded successfully\n");

    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)!=e_success){
        printf("Error: unable to encode extension\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)!=e_success){
        printf("Error: unable to encode secret file size\n");
        return e_failure;
    }
    printf("Secret file size encoded secret file data\n");

    if(encode_secret_file_data(encInfo)!=e_success){
        printf("Error: unable to encode secret file data\n");
        return e_failure;
    }
    printf("secret file data encoded successfully\n");

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)!=e_success){
        printf("Error: unable to copy remaining image data\n");
        return e_failure;

    }

    printf("Remaining image data copied successfully\n");

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}



    /*
    -> check_capacity(encInfo) == e_failure
            printf error msg

    copy_bmp_header(encInfo -> *fptr_src_image, encInfo -> fptr_stego_image) == printf error msg,return e_failure

    -> encode_magic_string( MAGIC_STRING, encInfo ) == e_failure
    printf error msg,return e_failure

        -> char *dot = strchr(argv[3],'.')
        ->strcpy(extn_secret_file, dot)

        encode_secret_file_extn_size(, encInfo)

    */

Status check_capacity(EncodeInfo *encInfo){
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if(encInfo->image_capacity > (16+32+(strlen(encInfo->extn_secret_file)*8)+32+(encInfo->size_Secret_file*8))){
        return e_success;
    }
    else{
        return e_failure;
    }
    /*
        ->get_image_size_for_bmp(encInfo -> fptr_src_image)> ((get_file_size(encInfo -> fptr_secret) +14)*8)
        ==>return e_success
        ->return e_failure
    */
}
uint get_file_size(FILE *fptr){
    /*-> fseek() to SEEK_END
    -> return ftell()
    */
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){

    char buffer[54]; 
    rewind(fptr_src_image);      

    if(fread(buffer,1,54,fptr_src_image)!=54){
        printf("Error:unable to read BMP file\n");
        return e_failure;
    }

    if(fwrite(buffer,1,54,fptr_dest_image)!=54){
        printf("Error: unable to write BMP file\n");
        return e_failure;
    }
    return e_success;
    /*
    
    * rewind(), src and secret files
    * Declare abuff of 54 bytes, char buff[54]
    * Read 54 bytes from src image to buff
    * write 54 bytes from buff to dest image
    
    */
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){

    char buffer[8];

    for(inti=0;i<strlen(magic_string);i++){
        if(fread(buffer,1,8,encInfo->fptr_src_image)!=8){
            return e_failure;
        }
        if(encode_byte_to_lsb(magic_string[i],buffer)!=e_success){
            return e_failure;
        }

        if(fwrite(buffer,1,8,encInfo->fptr_stego_image)!=8){
            return_failure;
        }
    }
    return e_success;
    /*
        -> declarre a buff of 8 bytes
    ==> run a loop for length of magic string
        -> read 8 bytes from src_image into buff
      -> encode_byte_to_lsb(magic_string[ ] , buff);
      -> write 8 bytes of encoded data from buff to dest_image

      ==>return e_success 
    */
}

Status encode_byte_to_lsb(char data, char *image_buffer){

    unsigned char value = (unsigned char)data;
    for(int i=0;i<8;i++){
        int bit = (value>>(7-i)) &1;

        image_buffer[i] = (image_buffer[i]& 0XFE)|bit;
    }
    return e_success;
    /*
        ==> Loop for 8 times
            *check the nth bit is set or not
         check that nth bit is set or not
         -> If set, Set the lsb of image_buffer[ ]
         -> If clear, clear the lsb of image_buffer[ ]

         ==>return e_success/failure
    */
}

Status encode_secret_file_extn_size(uint size, EncodeInfo *encInfo)
{
    char buffer[32];

    if(fread(buffer,1,32,encInfo->fptr_src_image)!=32){
        printf("Error:unable to read image data\n");
        return e_failure;
    }

    if(encode_size_to_lsb((int)file_size,buffer)!= e_success){
        printf("Error: unable to encode secret file size\n");
        return e_failure;
    }
    if(fwrite(buffer,1,32,encInfo->fptr_stego_image)!=32){
        printf("Error:unable to write image data\n");
        return e_failure;
    }
    return e_success;

    /*
        -> Declare a buff of 32 bytes

        -> Read 32 bytes of buff from src_image
        -> encode_size_to_lsb(size, buff)
        -> write buff to dest_image
        -> return e_success
    */
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo){
    /*
    -> declarre a buff of 8 bytes
    ==> run a loop for length of magic string
        -> read 8 bytes from src_image into buff
      -> encode_byte_to_lsb(file_extn[] , buff);
      -> write 8 bytes of encoded data from buff to dest_image

      ==>return e_success 
    */
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){
    char buffer[32];

    if(fread(buffer,1,32,encInfo->fptr_src_image)!=32){
        printf("Error: unable to read image data\n");
        return e_failure;
    }
    if(encode_size_to_lsb((int)file_size_buffer)!=e_success){
        printf("Error:unable to encode secret file size\n");
        return e_failure;
    }
    if(fwrite(buffer,1,32,encInfo->fptr_stego_image)!=32){
        printf("Error:unable to write image data\n");
        return e_failure;
    }
    return e_success;
}






Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){

    char buffer[1024];
    size_t bytes_read;
    while((bytes_read = fread(buffer,1,sizeof(buffer),fptr_src))>0){
        if(fwrite(buffer,1,bytes_read,fptr_dest)!= bytes_read){
            return e_failure;
        }
    }
    if(ferror(fptr_src))
        return e_failure;

    return e_success;
    /*
    ->read a char from fptr_src
    -> write the char to fptr_dest
    
    */
}