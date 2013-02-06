/******************************************************************************
 *
 * Module Name: nsconvert - Object conversions for objects returned by
 *                          predefined methods
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2013, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights. You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code. No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision. In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change. Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee. Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution. In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE. ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT, ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES. INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS. INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government. In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#define __NSCONVERT_C__

#include "acpi.h"
#include "accommon.h"
#include "acnamesp.h"
#include "acinterp.h"
#include "acpredef.h"
#include "amlresrc.h"

#define _COMPONENT          ACPI_NAMESPACE
        ACPI_MODULE_NAME    ("nsconvert")


/*******************************************************************************
 *
 * FUNCTION:    AcpiNsConvertToInteger
 *
 * PARAMETERS:  OriginalObject      - Object to be converted
 *              ReturnObject        - Where the new converted object is returned
 *
 * RETURN:      Status. AE_OK if conversion was successful.
 *
 * DESCRIPTION: Attempt to convert a String/Buffer object to an Integer.
 *
 ******************************************************************************/

ACPI_STATUS
AcpiNsConvertToInteger (
    ACPI_OPERAND_OBJECT     *OriginalObject,
    ACPI_OPERAND_OBJECT     **ReturnObject)
{
    ACPI_OPERAND_OBJECT     *NewObject;
    ACPI_STATUS             Status;
    UINT64                  Value = 0;
    UINT32                  i;


    switch (OriginalObject->Common.Type)
    {
    case ACPI_TYPE_STRING:

        /* String-to-Integer conversion */

        Status = AcpiUtStrtoul64 (OriginalObject->String.Pointer,
                    ACPI_ANY_BASE, &Value);
        if (ACPI_FAILURE (Status))
        {
            return (Status);
        }
        break;

    case ACPI_TYPE_BUFFER:

        /* Buffer-to-Integer conversion. Max buffer size is 64 bits. */

        if (OriginalObject->Buffer.Length > 8)
        {
            return (AE_AML_OPERAND_TYPE);
        }

        /* Extract each buffer byte to create the integer */

        for (i = 0; i < OriginalObject->Buffer.Length; i++)
        {
            Value |= ((UINT64) OriginalObject->Buffer.Pointer[i] << (i * 8));
        }
        break;

    default:
        return (AE_AML_OPERAND_TYPE);
    }

    NewObject = AcpiUtCreateIntegerObject (Value);
    if (!NewObject)
    {
        return (AE_NO_MEMORY);
    }

    *ReturnObject = NewObject;
    return (AE_OK);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiNsConvertToString
 *
 * PARAMETERS:  OriginalObject      - Object to be converted
 *              ReturnObject        - Where the new converted object is returned
 *
 * RETURN:      Status. AE_OK if conversion was successful.
 *
 * DESCRIPTION: Attempt to convert a Integer/Buffer object to a String.
 *
 ******************************************************************************/

ACPI_STATUS
AcpiNsConvertToString (
    ACPI_OPERAND_OBJECT     *OriginalObject,
    ACPI_OPERAND_OBJECT     **ReturnObject)
{
    ACPI_OPERAND_OBJECT     *NewObject;
    ACPI_SIZE               Length;
    ACPI_STATUS             Status;


    switch (OriginalObject->Common.Type)
    {
    case ACPI_TYPE_INTEGER:
        /*
         * Integer-to-String conversion. Commonly, convert
         * an integer of value 0 to a NULL string. The last element of
         * _BIF and _BIX packages occasionally need this fix.
         */
        if (OriginalObject->Integer.Value == 0)
        {
            /* Allocate a new NULL string object */

            NewObject = AcpiUtCreateStringObject (0);
            if (!NewObject)
            {
                return (AE_NO_MEMORY);
            }
        }
        else
        {
            Status = AcpiExConvertToString (OriginalObject, &NewObject,
                        ACPI_IMPLICIT_CONVERT_HEX);
            if (ACPI_FAILURE (Status))
            {
                return (Status);
            }
        }
        break;

    case ACPI_TYPE_BUFFER:
        /*
         * Buffer-to-String conversion. Use a ToString
         * conversion, no transform performed on the buffer data. The best
         * example of this is the _BIF method, where the string data from
         * the battery is often (incorrectly) returned as buffer object(s).
         */
        Length = 0;
        while ((Length < OriginalObject->Buffer.Length) &&
                (OriginalObject->Buffer.Pointer[Length]))
        {
            Length++;
        }

        /* Allocate a new string object */

        NewObject = AcpiUtCreateStringObject (Length);
        if (!NewObject)
        {
            return (AE_NO_MEMORY);
        }

        /*
         * Copy the raw buffer data with no transform. String is already NULL
         * terminated at Length+1.
         */
        ACPI_MEMCPY (NewObject->String.Pointer,
            OriginalObject->Buffer.Pointer, Length);
        break;

    default:
        return (AE_AML_OPERAND_TYPE);
    }

    *ReturnObject = NewObject;
    return (AE_OK);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiNsConvertToBuffer
 *
 * PARAMETERS:  OriginalObject      - Object to be converted
 *              ReturnObject        - Where the new converted object is returned
 *
 * RETURN:      Status. AE_OK if conversion was successful.
 *
 * DESCRIPTION: Attempt to convert a Integer/String/Package object to a Buffer.
 *
 ******************************************************************************/

ACPI_STATUS
AcpiNsConvertToBuffer (
    ACPI_OPERAND_OBJECT     *OriginalObject,
    ACPI_OPERAND_OBJECT     **ReturnObject)
{
    ACPI_OPERAND_OBJECT     *NewObject;
    ACPI_STATUS             Status;
    ACPI_OPERAND_OBJECT     **Elements;
    UINT32                  *DwordBuffer;
    UINT32                  Count;
    UINT32                  i;


    switch (OriginalObject->Common.Type)
    {
    case ACPI_TYPE_INTEGER:
        /*
         * Integer-to-Buffer conversion.
         * Convert the Integer to a packed-byte buffer. _MAT and other
         * objects need this sometimes, if a read has been performed on a
         * Field object that is less than or equal to the global integer
         * size (32 or 64 bits).
         */
        Status = AcpiExConvertToBuffer (OriginalObject, &NewObject);
        if (ACPI_FAILURE (Status))
        {
            return (Status);
        }
        break;

    case ACPI_TYPE_STRING:

        /* String-to-Buffer conversion. Simple data copy */

        NewObject = AcpiUtCreateBufferObject (OriginalObject->String.Length);
        if (!NewObject)
        {
            return (AE_NO_MEMORY);
        }

        ACPI_MEMCPY (NewObject->Buffer.Pointer,
            OriginalObject->String.Pointer, OriginalObject->String.Length);
        break;

    case ACPI_TYPE_PACKAGE:
        /*
         * This case is often seen for predefined names that must return a
         * Buffer object with multiple DWORD integers within. For example,
         * _FDE and _GTM. The Package can be converted to a Buffer.
         */

        /* All elements of the Package must be integers */

        Elements = OriginalObject->Package.Elements;
        Count = OriginalObject->Package.Count;

        for (i = 0; i < Count; i++)
        {
            if ((!*Elements) ||
                ((*Elements)->Common.Type != ACPI_TYPE_INTEGER))
            {
                return (AE_AML_OPERAND_TYPE);
            }
            Elements++;
        }

        /* Create the new buffer object to replace the Package */

        NewObject = AcpiUtCreateBufferObject (ACPI_MUL_4 (Count));
        if (!NewObject)
        {
            return (AE_NO_MEMORY);
        }

        /* Copy the package elements (integers) to the buffer as DWORDs */

        Elements = OriginalObject->Package.Elements;
        DwordBuffer = ACPI_CAST_PTR (UINT32, NewObject->Buffer.Pointer);

        for (i = 0; i < Count; i++)
        {
            *DwordBuffer = (UINT32) (*Elements)->Integer.Value;
            DwordBuffer++;
            Elements++;
        }
        break;

    default:
        return (AE_AML_OPERAND_TYPE);
    }

    *ReturnObject = NewObject;
    return (AE_OK);
}