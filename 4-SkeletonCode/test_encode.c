#include <stdio.h>
#include "encode.h"
#include "types.h"

int main()
{
    EncodeInfo encInfo;
    //(call check_operation_type(arv[1][1]) == e_encode)
    /*
    read_and_validate_encode_args(argv, &encInfo) == e_success
    {
        -> do_encoding(&encInfo) == e_success
    }else{
    stop
    }
    */
    return 0;
}
OperationType check_operation_type(char opt)
{
    /*
    if opt is 'e'
        return e_encode
    if opt is 'd'
        return e_decode
    return e_unsupported
    */

}